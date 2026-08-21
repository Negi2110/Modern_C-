#include <iostream>
#include <thread>
#include <vector>
#include <stop_token>
#include <chrono>
#include <mutex>

// ============================================================
// std::jthread (C++20) - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS jthread AND WHY IT EXISTS
// ============================================================

// std::thread PROBLEM: YOU must remember to join() or detach()
// Forget join() -> destructor calls std::terminate() -> CRASH!
//
// std::jthread SOLUTION: "joining thread"
// -> AUTOMATICALLY calls join() in its DESTRUCTOR
// -> RAII: Resource Acquisition Is Initialization
// -> same guarantee as unique_ptr vs raw pointer:
//    unique_ptr: auto-deletes    -> std::jthread: auto-joins
//    raw pointer: manual delete  -> std::thread: manual join
//
// YOUR CODE:
// NO explicit join() needed! Commenting out the join loop is CORRECT!
// When Jthreads vector goes out of scope (end of main):
// -> each jthread's destructor called in REVERSE order
// -> destructor calls join() automatically
// -> all threads finish before main() truly exits

// ============================================================
// SECTION 2: jthread vs thread - COMPARISON
// ============================================================

// | Feature                    | std::thread | std::jthread |
// |----------------------------|-------------|--------------|
// | Auto-join on destruction   | NO (crash!) | YES ✅       |
// | Cooperative cancellation   | NO          | YES ✅       |
// | stop_token support         | NO          | YES ✅       |
// | API (push/join/etc)        | same        | same + more  |
// | C++ version                | C++11       | C++20        |
// | Performance overhead       | none        | tiny         |
// | Move-only                  | YES         | YES          |
//
// PREFER jthread in ALL new C++20 code
// Only use thread when: C++17 compatibility needed,
// or you explicitly want detach() semantics

// ============================================================
// SECTION 3: YOUR CODE EXPLAINED
// ============================================================

int main()
{
    std::mutex coutMtx;  // for clean output (added for clarity)

    auto lambda = [](int x)
    {
        std::cout << "hello from thread: "
                  << std::this_thread::get_id() << '\n';
        std::cout << "Argument passed in: " << x << '\n';
    };

    std::vector<std::jthread> jthreads;
    jthreads.reserve(10);  // prevent reallocation

    for (int i = 0; i < 10; i++)
    {
        jthreads.push_back(std::jthread(lambda, i));
        // Creates jthread -> starts thread immediately
        // NO need to join() anywhere!
    }

    // for (int i = 0; i < 10; i++) {
    //     threads[i].join();   // NOT NEEDED with jthread!
    // }

    std::cout << "main: returning...\n";

    return 0;
    // HERE: jthreads vector goes out of scope
    // DESTRUCTOR of each jthread called (reverse order: 9,8,7...0)
    // Each destructor calls join() automatically
    // ALL 10 threads finish before main() returns
    // NO std::terminate(), NO crash, NO leak
}

// ============================================================
// SECTION 4: DESTRUCTION ORDER - IMPORTANT DETAIL
// ============================================================

// std::vector destructs elements in REVERSE order:
// jthreads[9] destructor (joins thread 9)
// jthreads[8] destructor (joins thread 8)
// ...
// jthreads[0] destructor (joins thread 0)
//
// THEN main() truly exits
//
// THIS MEANS: "main: returning..." might print BEFORE threads finish!
// The threads are still running when cout prints that line
// They finish during the vector destruction (after the cout)
//
// TIMELINE:
// cout "main: returning..."
// vector destructor starts (jthreads[9].~jthread() = join thread 9)
// thread 9 finishes
// jthreads[8].~jthread() = join thread 8
// ...all threads finish...
// main() truly returns

// ============================================================
// SECTION 5: COOPERATIVE CANCELLATION - jthread's KILLER FEATURE
// ============================================================

// jthread gives each thread a std::stop_token automatically
// -> lets you ask a thread to STOP cleanly
// -> thread checks stop_requested() and exits gracefully
// -> much better than: setting a global bool flag (race condition)
//                  or: terminating the thread forcefully (dangerous)

void cancellationExample()
{
    // LAMBDA WITH stop_token parameter:
    auto longTask = [](std::stop_token stopToken, int id)
    {
        int count = 0;
        while (!stopToken.stop_requested())
        // check if someone requested us to stop
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
            count++;
            std::cout << "Thread " << id
                      << " working... count=" << count << '\n';
        }
        std::cout << "Thread " << id << " stopping gracefully\n";
    };

    // CREATE jthread (stop_token automatically passed as FIRST arg):
    std::jthread worker(longTask, 1);
    // Note: stop_token is AUTOMATICALLY injected as first parameter!
    // You write lambda(stop_token, int id) and call jthread(lambda, 1)
    // jthread detects stop_token parameter and passes it automatically

    // Let it work for a bit:
    std::this_thread::sleep_for(std::chrono::milliseconds(350));

    // REQUEST STOP (cooperative - thread decides when to actually stop):
    worker.request_stop();
    // sets the stop_token's stop_requested() to true
    // thread sees this in its while loop and exits cleanly

    // jthread destructor joins automatically
    // (would also call request_stop() first if not already done)
    std::cout << "Main: worker stopped\n";
}

// ============================================================
// SECTION 6: STOP TOKEN MECHANISM
// ============================================================

// Three components work together:
//
// std::stop_source  -> owns the stop state, can request_stop()
// std::stop_token   -> read-only view, can check stop_requested()
// std::stop_callback-> register callback to run when stop requested
//
// jthread internally holds a stop_source
// get_stop_token()   -> gives you the token (to pass to other threads)
// get_stop_source()  -> gives you the source (to request stop)
// request_stop()     -> shortcut for get_stop_source().request_stop()

void stopTokenDetails()
{
    auto worker = [](std::stop_token st)
    {
        // Register a callback for when stop is requested:
        std::stop_callback cb(st, []() {
            std::cout << "Stop was requested! Cleaning up...\n";
            // could close file handles, flush buffers, etc.
        });

        while (!st.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        std::cout << "Worker exiting\n";
    };

    std::jthread jt(worker);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    jt.request_stop();  // triggers callback + sets stop_requested()

    // jt.~jthread() calls join() automatically
}

// ============================================================
// SECTION 7: JTHREAD DESTRUCTOR BEHAVIOR
// ============================================================

// std::jthread::~jthread():
// if (joinable()) {
//     request_stop();   // 1. ASK thread to stop (if it checks!)
//     join();           // 2. WAIT for thread to actually finish
// }
//
// NOTE: request_stop() in destructor only helps if thread
// checks stop_requested(). If your lambda doesn't use stop_token,
// destructor just calls join() (no stop request possible).
//
// EXAMPLE - destructor behavior:
// {
//     std::jthread t([](std::stop_token st) {
//         while (!st.stop_requested()) {
//             // do work
//         }
//     });
//     // ... later ...
// }  // t goes out of scope:
//    // 1. request_stop() called -> thread sees it and exits loop
//    // 2. join() called -> waits for thread to finish loop and return
//    // CLEAN shutdown!

// ============================================================
// SECTION 8: COMPLETE EXAMPLE - BACKGROUND WORKER PATTERN
// ============================================================

class BackgroundWorker
{
    // Using jthread for clean RAII thread management
    std::jthread m_thread;
    std::mutex   m_mutex;

public:
    BackgroundWorker()
        : m_thread([this](std::stop_token st) {
            this->run(st);
        })
    {}

    // Destructor automatically:
    // 1. Requests stop (m_thread.~jthread calls request_stop())
    // 2. Joins (waits for run() to finish)
    // ~BackgroundWorker() = default;  // jthread handles it!

private:
    void run(std::stop_token st)
    {
        while (!st.stop_requested()) {
            std::lock_guard<std::mutex> lock(m_mutex);
            // do background work...
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "BackgroundWorker: clean exit\n";
    }
};

// ============================================================
// SECTION 9: ISSST/DIS CONNECTION
// ============================================================

// In your DRDO simulation:
//
// class DISNetworkPlugin {
//     std::jthread m_receiveThread;    // auto-joins on destruction!
//     std::jthread m_processThread;   // auto-joins on destruction!
//
//     void startReceiving() {
//         m_receiveThread = std::jthread([this](std::stop_token st) {
//             while (!st.stop_requested()) {
//                 auto pdu = receiveUDP();   // blocking receive
//                 if (pdu) processPDU(*pdu);
//             }
//             std::cout << "DIS receive thread stopped\n";
//         });
//     }
//
//     // ~DISNetworkPlugin():
//     // m_receiveThread.~jthread()  -> request_stop() + join()
//     // m_processThread.~jthread()  -> request_stop() + join()
//     // No manual thread management needed!
// };

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::jthread and how does it differ from std::thread?
//     -> C++20 "joining thread". Automatically calls join() in
//        destructor (RAII). Also supports cooperative cancellation
//        via stop_token. Otherwise same API as std::thread.

// Q2: What happens when a jthread goes out of scope?
//     -> Destructor calls request_stop() then join().
//        Thread is asked to stop cooperatively, then main waits
//        for it to finish. No std::terminate(), no crash.

// Q3: What is a stop_token and how do you use it?
//     -> Read-only cancellation token. Check stop_requested() in
//        your thread loop. When main calls request_stop() (or jthread
//        destructor fires), stop_requested() returns true.
//        Thread exits its loop cleanly.

// Q4: Does jthread automatically pass stop_token to your lambda?
//     -> YES, if your lambda's FIRST parameter is std::stop_token.
//        jthread detects this and injects the token automatically.
//        Remaining parameters passed as normal arguments.

// Q5: When would you still use std::thread instead of jthread?
//     -> C++17 or earlier code (jthread = C++20 only).
//        When you explicitly want detach() semantics.
//        When you KNOW you'll always join manually and don't
//        want the stop_token overhead.

// Q6: Is the join() order guaranteed when vector<jthread> is destroyed?
//     -> YES. Vector destructs in REVERSE order (last inserted first).
//        jthreads[9] joins, then [8], then [7]... down to [0].
//        All guaranteed to finish before vector destruction completes.

// Q7: Can you call request_stop() on jthread multiple times?
//     -> YES, it's idempotent. First call sets the stop state.
//        Subsequent calls are no-ops. stop_requested() returns true
//        after first request_stop(), regardless of how many times called.

// Q8: What is a stop_callback?
//     -> Registered function that runs when stop is requested.
//        std::stop_callback cb(st, []{ cleanup(); });
//        Runs immediately if stop already requested.
//        Used for: closing sockets, flushing buffers, cleanup tasks.
// ```

// **jthread vs thread in one picture:**

// ```cpp
// // std::thread - MANUAL (you must remember):
// {
//     std::thread t(work);
//     // forgot join() or detach() !
// }  // CRASH: std::terminate() called!

// // std::jthread - AUTOMATIC (RAII):
// {
//     std::jthread t(work);
//     // no explicit join needed!
// }  // SAFE: destructor calls join() automatically ✅
// ```

// **The stop_token mechanism:**

// ```cpp
// std::jthread t([](std::stop_token st) {
//     while (!st.stop_requested()) {  // checks if asked to stop
//         doWork();
//     }
//     cleanup();  // graceful exit
// });

// // Later (or in destructor automatically):
// t.request_stop();  // sets stop_requested() = true
//                    // thread sees it, exits loop, calls cleanup()
// // t.~jthread() -> join() -> waits for cleanup() to finish
// ```

// **Your code - what happens at return 0:**

// ```
// return 0;
//     ↓
// vector<jthread> destructor
//     ↓
// jthreads[9].~jthread()  → join() thread 9
// jthreads[8].~jthread()  → join() thread 8
// ...
// jthreads[0].~jthread()  → join() thread 0
//     ↓
// ALL threads finished → main() truly exits ✅
// ```

// > **The bottom line**: `jthread` is `thread` with a safety net. In any C++20 code, there is no reason to use `std::thread` unless you need `detach()` or backward compatibility. The automatic join on destruction eliminates the single most common threading bug — forgetting to join — and the `stop_token` gives you clean shutdown that no global flag can match.