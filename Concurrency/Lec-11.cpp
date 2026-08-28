#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>

// ============================================================
// std::async AND std::future - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS std::async?
// ============================================================

// std::async = high-level way to run a function asynchronously
// -> you give it a function + arguments
// -> it runs the function (possibly in a new thread)
// -> returns a std::future<T> to get the result later
//
// YOUR COMMENT: "easy way to launch threads"
// YOUR COMMENT: "we block on the future's result if we need it"
//
// COMPARED TO std::thread:
// std::thread:  run function, join to wait (no easy return value)
// std::async:   run function, get() to wait AND get the return value
//
// std::async is HIGHER LEVEL than std::thread:
// -> automatically handles thread creation/destruction
// -> automatically captures return value in future
// -> exception propagation (exceptions from async task caught at get())
// -> may use thread pool (implementation defined)

// ============================================================
// SECTION 2: std::future - WHAT IS IT?
// ============================================================

// std::future<T> = a handle to a RESULT THAT DOESN'T EXIST YET
// -> placeholder for a value that will be computed asynchronously
// -> calling get() either:
//    a) returns the value immediately if already computed
//    b) BLOCKS until the value is computed
//
// ANALOGY:
// You order food at a restaurant (std::async)
// You get a ticket/buzzer (std::future)
// You do other things while food is being prepared
// When you call get() (check the buzzer):
//   -> if food is ready: get it immediately
//   -> if not ready: wait until it is

// ============================================================
// SECTION 3: YOUR CODE EXPLAINED
// ============================================================

int square(int x)
{
    return x * x;
}

int main()
{
    // LAUNCH async task:
    std::future<int> asyncFunction = std::async(&square, 12);
    // -> std::async called with function pointer &square and arg 12
    // -> square(12) starts running (possibly in another thread)
    // -> asyncFunction = future that will hold the result (144)
    // -> main() CONTINUES immediately (doesn't wait!)
    //
    // std::async<int>: deduced from return type of square (int)
    // Could also write: auto asyncFunction = std::async(&square, 12);

    // DO OTHER WORK while square(12) computes in background:
    for (int i = 0; i < 10; i++)
    {
        std::cout << square(i) << '\n';
        // prints: 0,1,4,9,16,25,36,49,64,81
        // square(12)=144 is being computed CONCURRENTLY
        // (or has already been computed, future holds the result)
    }

    // BLOCKING GET:
    int result = asyncFunction.get();
    // YOUR COMMENT: "we are blocked at the 'get()' operation,
    //                until our result has been computed"
    //
    // IF square(12) is DONE: get() returns 144 immediately
    // IF square(12) is NOT DONE: get() BLOCKS until it finishes
    //
    // get() can only be called ONCE per future!
    // Second get() = std::future_error exception
    // After get(): future is in "invalid" state

    std::cout << "Result: " << result << '\n';  // Result: 144

    return 0;
}

// ============================================================
// SECTION 4: LAUNCH POLICIES
// ============================================================

// std::async has TWO launch policies:
//
// std::launch::async (FORCE new thread):
// auto f = std::async(std::launch::async, square, 12);
// -> GUARANTEES a new thread is created
// -> function runs concurrently
// -> use when you NEED true parallelism
//
// std::launch::deferred (LAZY evaluation):
// auto f = std::async(std::launch::deferred, square, 12);
// -> does NOT create a thread immediately
// -> function runs ONLY when you call f.get() or f.wait()
// -> runs in the CALLING thread (synchronous!)
// -> use when you're not sure you'll need the result
//
// std::launch::async | std::launch::deferred (DEFAULT = your code):
// auto f = std::async(square, 12);  // no policy specified = default
// -> IMPLEMENTATION DECIDES (might be thread, might be deferred)
// -> non-deterministic! could be sequential!
//
// RECOMMENDATION: always specify launch::async if you want parallelism:
// auto f = std::async(std::launch::async, square, 12);

// ============================================================
// SECTION 5: EXCEPTION HANDLING WITH async
// ============================================================

int riskyFunction(int x)
{
    if (x < 0) throw std::invalid_argument("negative input!");
    return x * x;
}

void exceptionExample()
{
    // Exception in async task is CAPTURED in the future:
    auto f = std::async(std::launch::async, riskyFunction, -5);

    // Exception NOT thrown yet (it's stored in the future)

    try {
        int result = f.get();  // EXCEPTION THROWN HERE at get()!
        std::cout << result;
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Caught: " << e.what() << '\n';
    }
    // KEY: exceptions propagate from the async thread to the calling thread
    // via the future. You catch them at get().
}

// ============================================================
// SECTION 6: FUTURE STATE METHODS
// ============================================================

void futureStateExample()
{
    auto f = std::async(std::launch::async, square, 10);

    // wait() = block until result ready (but don't get the value):
    f.wait();
    // f is now ready

    // wait_for() = wait for a maximum duration:
    auto status = f.wait_for(std::chrono::milliseconds(100));
    if (status == std::future_status::ready) {
        std::cout << "Result: " << f.get() << '\n';
    } else if (status == std::future_status::timeout) {
        std::cout << "Not ready yet (timed out)\n";
    } else if (status == std::future_status::deferred) {
        std::cout << "Deferred (not started yet)\n";
    }

    // valid() = does this future have a shared state?
    // auto f2 = std::move(f);  // transfer future to f2
    // f.valid() = false (moved from)
    // f2.valid() = true
}

// ============================================================
// SECTION 7: MULTIPLE async TASKS - PARALLEL COMPUTATION
// ============================================================

void parallelExample()
{
    // Square of numbers 1-8, computed in parallel:
    std::vector<std::future<int>> futures;

    for (int i = 1; i <= 8; i++) {
        futures.push_back(
            std::async(std::launch::async, square, i)
        );
    }
    // All 8 tasks running concurrently now!
    // Compute other work here...

    // Collect results:
    std::cout << "Squares: ";
    for (auto& f : futures) {
        std::cout << f.get() << " ";  // blocks for each until ready
    }
    std::cout << '\n';
    // Output: Squares: 1 4 9 16 25 36 49 64
    // But computed in PARALLEL (much faster for heavy computation)
}

// ============================================================
// SECTION 8: std::promise - THE OTHER SIDE OF std::future
// ============================================================

// future = read end (consumer)
// promise = write end (producer)
// Together they form a one-time channel between threads
//
// std::promise<T> creates the "sender"
// promise.get_future() creates the matching "receiver"

void promiseExample()
{
    std::promise<int> myPromise;
    std::future<int>  myFuture = myPromise.get_future();

    // Worker thread: sets the value via promise
    std::thread worker([](std::promise<int> p) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int computed = 42 * 2;
        p.set_value(computed);  // "I'm done, here's the result"
    }, std::move(myPromise));   // promise is move-only!

    // Main thread: reads when ready
    std::cout << "Waiting for promise...\n";
    int result = myFuture.get();  // blocks until set_value() called
    std::cout << "Promise result: " << result << '\n';  // 84

    worker.join();
}

// PROMISE = manual control over when a future becomes ready
// async   = automatic (future ready when function returns)
// Use promise when: function finishes in stages, or you need
//                   to set the value from a specific point

// ============================================================
// SECTION 9: std::async vs std::thread COMPARISON
// ============================================================

// std::thread:
// -> lower level, explicit thread management
// -> must join() or detach()
// -> no easy way to get return value (need promise/shared state)
// -> exception: std::terminate if exception escapes thread function
//
// std::async:
// -> higher level, no explicit thread management
// -> future automatically managed (no join/detach needed)
// -> return value captured in future (just call get())
// -> exception: stored in future, thrown at get()
// -> may use thread pool (more efficient for many small tasks)
//
// USE std::thread when:
// -> long-running background threads
// -> need explicit control (priority, CPU affinity, name)
// -> thread communicates via shared data + mutex
//
// USE std::async when:
// -> need result of async computation
// -> fire-and-forget with result
// -> parallel computation with multiple futures
// -> exception safety matters
//
// YOUR CODE: perfect use case for async
// square(12) = quick computation, need the result later
// async lets you do other work while it computes

// ============================================================
// SECTION 10: ISSST/DIS CONNECTION
// ============================================================

// std::async useful in simulation for:
//
// PARALLEL PDU PROCESSING:
// auto future1 = std::async(launch::async, processEntityStatePDU, pdu1);
// auto future2 = std::async(launch::async, processFirePDU, pdu2);
// auto future3 = std::async(launch::async, processDetPDU, pdu3);
// // all three process in parallel
// future1.get(); future2.get(); future3.get();
// // wait for all to complete
//
// ASYNC RADAR COMPUTATION:
// auto radarResult = std::async(launch::async, computeRadarDetections, entities);
// // while radar computes in background:
// renderUI();
// updateNetworkStatus();
// // now get the radar results:
// auto detections = radarResult.get();
// updateTacticalDisplay(detections);

// ============================================================
// INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::async and what does it return?
//     -> High-level way to run a function asynchronously.
//        Returns std::future<T> where T is the function's return type.
//        The future holds the result once computation completes.

// Q2: What does future::get() do?
//     -> Returns the result when ready. BLOCKS if not yet computed.
//        Can only be called ONCE (future becomes invalid after get()).
//        Also re-throws any exception that occurred in the async task.

// Q3: What is the default launch policy and why is it a problem?
//     -> async | deferred: implementation decides.
//        Might run synchronously (no parallelism!) or asynchronously.
//        Use std::launch::async explicitly for guaranteed new thread.

// Q4: What is the difference between future::wait() and future::get()?
//     -> wait(): blocks until result ready, does NOT return the value.
//        get(): blocks until ready AND returns (moves) the value.
//        Can call wait() multiple times, get() only ONCE.

// Q5: How are exceptions handled with std::async?
//     -> Exceptions in the async function are captured in the future.
//        They are NOT thrown in the worker thread.
//        They ARE thrown when you call future::get() in the caller thread.
//        Allows exception-safe parallel computation.

// Q6: What is std::promise and how does it relate to std::future?
//     -> promise = write end, future = read end.
//        promise.set_value(x) makes future.get() return x.
//        async creates both automatically; promise gives manual control.
//        Use promise when you need to set the value at a specific time.

// Q7: Can you call get() on a future twice?
//     -> NO. Second call throws std::future_error.
//        After get(): future is in "invalid" state.
//        Use std::shared_future if multiple threads need the result.

// Q8: When would you use std::async instead of std::thread?
//     -> When you need the return value of the async computation.
//        When exception safety matters (propagates to get()).
//        When you want simpler code (no manual join/detach).
// //        When running many short tasks (may use thread pool).
// ```

// **The async/future timeline:**

// ```
// main():                          square(12) async task:
//   async(square, 12) ─────────────→ [starts running]
//   ← future returned               [computing...]
//   for loop: prints 0,1,4...81     [computing...]
//   (work done concurrently!)       [done! result=144 stored in future]
//   future.get() ──────────────────→ [returns 144 immediately - already done]
//   cout << 144
// ```

// **Launch policies:**

// ```cpp
// // DEFAULT (unpredictable - AVOID):
// auto f = std::async(square, 12);           // might not be parallel!

// // FORCE async (guaranteed new thread):
// auto f = std::async(std::launch::async, square, 12);    // ✅ parallel

// // DEFERRED (lazy, runs at get()):
// auto f = std::async(std::launch::deferred, square, 12); // runs at f.get()
// ```

// **async vs thread:**

// ```
// std::thread:                    std::async:
// thread t(func, arg);            auto f = async(launch::async, func, arg);
// // no return value               // return value captured in f
// // must join()                   // no join needed
// // exception = terminate()       // exception stored → thrown at get()
// int result = ???                 int result = f.get();  ← clean!
// t.join();
// ```

// > **The key insight**: `std::async` is `std::thread` + `std::future` bundled together. The future is the missing piece from raw threads — it's how the result (or exception) **travels back** from the worker thread to the caller thread. For any task where you need a return value from a background computation, `std::async` is almost always the cleanest solution.