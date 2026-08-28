#include <iostream>
#include <future>
#include <thread>
#include <chrono>

// ============================================================
// std::async - BACKGROUND LOADING PATTERN - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT THIS PATTERN SOLVES
// ============================================================

// PROBLEM: loading a large file/resource blocks the main thread
// -> nothing else runs while loading
// -> UI freezes, game loop stops, no responsiveness
//
// SOLUTION: background thread loads while main thread continues
// -> main loop keeps running (game renders, UI responsive)
// -> periodically CHECK if loading is done (non-blocking poll)
// -> when done: use the loaded resource
//
// YOUR PATTERN:
// bufferedFileLoader() = background task (loads 20000 bytes slowly)
// main loop            = "game loop" / "UI loop" / "sim loop"
// wait_for(1 microsecond) = non-blocking poll of loading status
//
// REAL WORLD USE CASES:
// -> Game: load level assets while showing loading screen
// -> DIS Sim: load terrain/entity database while sim starts
// -> App: load config file while showing splash screen
// -> Server: pre-fetch data while handling current request

// ============================================================
// SECTION 2: THE THREE FUTURE STATUSES
// ============================================================

// std::future_status has THREE values:
//
// future_status::ready
// -> task COMPLETED, result is available
// -> get() will return immediately
// -> YOUR CODE: breaks out of loop and prints "data is ready"
//
// future_status::timeout
// -> task NOT done within the wait_for duration
// -> result not yet available
// -> YOUR CODE: this is what happens on each loop iteration
//    (wait_for(1 microsecond) returns timeout almost always
//     because loader takes 250ms per 1000 bytes)
//
// future_status::deferred
// -> task launched with launch::deferred (hasn't started yet)
// -> won't run until you call get() or wait()
// -> YOUR CODE won't see this (you use launch::async)

// ============================================================
// SECTION 3: YOUR CODE EXPLAINED
// ============================================================

bool bufferedFileLoader()
{
    size_t bytesLoaded = 0;

    while (bytesLoaded < 20000)
    {
        std::cout << "thread: Loading file....\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        // simulate I/O: 250ms per chunk
        bytesLoaded += 1000;
        // loads 1000 bytes every 250ms
        // total: 20000 bytes / 1000 per chunk = 20 iterations
        // total time: 20 * 250ms = 5 seconds to load
    }

    return true;  // success! stored in the future<bool>
}

int main()
{
    // LAUNCH background loading:
    std::future<bool> backgroundThread =
        std::async(std::launch::async, bufferedFileLoader);
    // bufferedFileLoader() starts running in a new thread IMMEDIATELY
    // main() continues without waiting
    // backgroundThread = future<bool> that will hold 'true' when done

    std::future_status status;

    // MAIN LOOP (game loop / UI loop / sim loop):
    while (true)
    {
        // MAIN THREAD WORK (happens every iteration):
        std::cout << "main thread is running\n";

        // Simulate main thread doing its own work:
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        // 50 microseconds of "work" (render frame, process input, etc.)

        // NON-BLOCKING POLL:
        status = backgroundThread.wait_for(std::chrono::microseconds(1));
        // wait_for(1 microsecond):
        // -> checks if future is ready
        // -> waits AT MOST 1 microsecond (almost instant)
        // -> returns: ready (done!) or timeout (not done yet)
        //
        // THIS IS NOT BLOCKING the main loop
        // 1 microsecond wait is negligible compared to frame time
        // Main loop continues at full speed

        if (status == std::future_status::ready)
        {
            std::cout << "our data is ready....\n";
            bool success = backgroundThread.get();
            // get() returns the bool (true) - loading succeeded
            // could check: if (!success) { handle error; }
            break;  // exit the main loop
        }
        // if timeout: loop continues, main thread keeps working
    }

    std::cout << "Program is complete\n";
    return 0;
    // backgroundThread future goes out of scope
    // if task somehow still running: blocks until done
}

// ============================================================
// SECTION 4: TIMELINE VISUALIZATION
// ============================================================

// Time →   0ms    50ms   100ms  250ms  500ms  750ms  ...  5000ms
//
// Main:    [work][poll][work][poll][work][poll]...[work][poll]
//           ↑each loop: 50µs work + 1µs poll
//           polls thousands of times per second
//
// Loader:  [loading chunk 1.............]
//                                       [loading chunk 2.............]
//                                                                     ...
//           250ms per chunk, 20 chunks = 5 seconds total
//
// When loader finishes:
// Main's next poll returns: future_status::ready → break!
//
// CONCURRENCY:
// Main thread: prints "main thread running" ~thousands of times
// Load thread: prints "thread: Loading file...." 20 times
// Output is INTERLEAVED (both printing simultaneously)

// ============================================================
// SECTION 5: wait_for vs wait vs get - COMPARISON
// ============================================================

// future.get()
// -> BLOCKS until result ready (could be seconds!)
// -> returns the result
// -> main loop would FREEZE until file loaded
// -> WRONG for this pattern (defeats the purpose)
//
// future.wait()
// -> BLOCKS until result ready (no timeout)
// -> doesn't return the result (just waits)
// -> same problem as get() for our use case
//
// future.wait_for(duration)
// -> waits AT MOST 'duration' then returns status
// -> returns: ready / timeout / deferred
// -> NON-BLOCKING from main loop's perspective
// -> CORRECT for polling pattern (YOUR CODE)
//
// future.wait_until(time_point)
// -> same as wait_for but uses absolute time point
// -> wait_until(now + 1ms) same as wait_for(1ms)

// ============================================================
// SECTION 6: IMPROVED VERSION WITH PROGRESS
// ============================================================

// More realistic: background thread reports progress

struct LoadResult {
    bool success;
    size_t bytesLoaded;
    std::string errorMessage;
};

// Shared progress (atomic for thread safety):
#include <atomic>
std::atomic<size_t> g_progress = 0;

LoadResult bufferedFileLoaderWithProgress()
{
    size_t totalBytes = 20000;
    size_t bytesLoaded = 0;

    while (bytesLoaded < totalBytes)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        bytesLoaded += 1000;
        g_progress.store(bytesLoaded);  // atomic: thread-safe update
    }

    return {true, bytesLoaded, ""};
}

void improvedMain()
{
    g_progress.store(0);

    auto future = std::async(std::launch::async,
                             bufferedFileLoaderWithProgress);
    std::future_status status;
    int frame = 0;

    while (true)
    {
        frame++;

        // Show progress (reads atomic without mutex):
        size_t loaded = g_progress.load();
        float pct = (float)loaded / 20000.0f * 100.0f;
        std::cout << "Frame " << frame
                  << " | Loading: " << (int)pct << "%"
                  << " | Main working...\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60fps

        status = future.wait_for(std::chrono::microseconds(1));
        if (status == std::future_status::ready)
        {
            LoadResult result = future.get();
            if (result.success) {
                std::cout << "Loading complete! "
                          << result.bytesLoaded << " bytes\n";
            } else {
                std::cout << "Loading FAILED: " << result.errorMessage << '\n';
            }
            break;
        }
    }
}

// ============================================================
// SECTION 7: ISSST/DIS CONNECTION
// ============================================================

// This EXACT pattern applies to your DRDO simulation:
//
// SCENARIO: Load terrain database while simulation starts
//
// struct TerrainData { /* large dataset */ };
//
// TerrainData loadTerrainDatabase() {
//     // read large binary file
//     // parse 10000 entity positions
//     // build spatial index
//     return terrain;  // might take 3-5 seconds
// }
//
// int main() {
//     // start loading in background:
//     auto terrainFuture = std::async(std::launch::async,
//                                     loadTerrainDatabase);
//
//     // initialize DIS network while terrain loads:
//     DISPlugin dis;
//     dis.initialize("239.255.0.1", 3000);
//
//     // start simulation loop:
//     while (simRunning) {
//         auto status = terrainFuture.wait_for(0ms);
//         if (status == future_status::ready) {
//             terrain = terrainFuture.get();
//             enableTerrainRendering();
//         }
//         processIncomingPDUs();
//         updateEntityStates();
//         renderTacticalDisplay();
//     }
// }
//
// RESULT: simulation starts IMMEDIATELY, terrain appears when ready
// vs BLOCKING: simulation waits 5 seconds before doing anything

// ============================================================
// SECTION 8: MULTIPLE BACKGROUND TASKS
// ============================================================

// void multipleBackgroundLoads()
// {
//     // All loading happens SIMULTANEOUSLY:
//     auto terrain   = std::async(std::launch::async, []{ return loadTerrain(); });
//     auto entities  = std::async(std::launch::async, []{ return loadEntities(); });
//     auto sounds    = std::async(std::launch::async, []{ return loadSounds(); });
//     auto textures  = std::async(std::launch::async, []{ return loadTextures(); });

//     // Main loop continues while all four load in parallel:
//     while (true) {
//         showLoadingScreen();  // render loading UI

//         // Check if ALL are ready:
//         bool allReady =
//             terrain.wait_for(0ms)  == std::future_status::ready &&
//             entities.wait_for(0ms) == std::future_status::ready &&
//             sounds.wait_for(0ms)   == std::future_status::ready &&
//             textures.wait_for(0ms) == std::future_status::ready;

//         if (allReady) {
//             // Get all results:
//             auto t = terrain.get();
//             auto e = entities.get();
//             auto s = sounds.get();
//             auto x = textures.get();
//             startSimulation(t, e, s, x);
//             break;
//         }
//     }
// }

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does wait_for() return and what are its possible values?
//     -> std::future_status enum with three values:
//        ready:    task completed, get() will return immediately.
//        timeout:  task not done within the specified duration.
//        deferred: task not started (deferred launch policy).

// Q2: Why use wait_for(1 microsecond) instead of get() in the loop?
//     -> get() BLOCKS until done (loop stops = frozen UI/game).
//        wait_for(1µs) returns almost immediately regardless.
//        Main loop continues running at full speed.
//        Non-blocking poll = responsive main thread.

// Q3: What is the polling pattern and when do you use it?
//     -> Periodically check if async task is done without blocking.
//        Use in: game loops, UI loops, simulation ticks.
//        Anytime the main thread must keep running while waiting.
//        Alternative to blocking get() when responsiveness matters.

// Q4: What happens to the future when the while loop breaks?
//     -> future goes out of scope at end of main().
//        If task is complete: destructor does nothing extra.
//        Future's destructor does NOT call get() automatically.
//        The bool result (true) was stored but we broke without calling get().
//        (Could call backgroundThread.get() to verify success before break)

// Q5: What is the output order of this program?
//     -> Interleaved: "main thread running" and "thread: Loading file..."
//        print simultaneously from two threads.
//        Order is non-deterministic (both cout without mutex = race!).
//        In practice: output lines are not garbled (cout is somewhat atomic)
//        but order between main and loader is unpredictable.

// Q6: How would you get loading progress without blocking?
//     -> Use std::atomic<size_t> shared between threads.
//        Loader updates atomic counter (no mutex needed for single value).
//        Main thread reads it with load() (non-blocking, thread-safe).

// Q7: What if bufferedFileLoader() throws an exception?
//     -> Exception is captured in the future (not thrown immediately).
//        When get() is called: exception is re-thrown in main thread.
//        Wrap get() in try/catch to handle loading failures gracefully.

// Q8: Could you use std::thread instead of std::async here?
//     -> Yes, but more complex. Need shared variable + mutex for result.
//        std::async + future is cleaner: result travels back via get().
//        Also: async handles exception propagation automatically.
//        std::thread: exception in thread = std::terminate (crash!).
// ```

// **The polling pattern timeline:**

// ```
// Time →    0         250ms      500ms     ...     5000ms
// Loader:   [chunk1.........][chunk2.........][...][done!→future=true]
// Main:     [work][poll][work][poll][work][poll]...[poll→READY!→break]
//            50µs  1µs  50µs  1µs
//            ← each iteration is ~51 microseconds
//            ← thousands of iterations while loader takes 5 seconds
// ```

// **The three wait methods:**

// ```cpp
// // BLOCKS FOREVER (wrong for game loop):
// bool result = future.get();        // ← main loop frozen for 5 seconds!

// // BLOCKS FOREVER (no result):
// future.wait();                      // ← same problem

// // NON-BLOCKING POLL (correct):
// auto status = future.wait_for(std::chrono::microseconds(1));
// if (status == std::future_status::ready) { ... }  // ← main loop continues!
// ```

// > **The real-world rule**: any time you have a main loop (game, simulation, UI) that must stay responsive, **never call `get()` in the main loop** — it blocks. Instead, use `wait_for(0ns)` or `wait_for(1µs)` to poll. The background task runs freely, and the main loop checks status every frame. This is the foundation of how every game engine loads assets in the background while the game keeps running.