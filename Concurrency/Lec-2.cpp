#include <iostream>
#include <thread>
#include <chrono>

// ============================================================
// std::thread - COMPLETE INTRODUCTION NOTES (C++11)
// ============================================================

// ============================================================
// SECTION 1: THREAD SUPPORT LIBRARY (from your screenshot)
// ============================================================

// #include <thread> gives you:
//
// THREADS:
// std::thread   (C++11) -> manages a separate thread
// std::jthread  (C++20) -> thread with AUTO-JOIN and cancellation
//                          (jthread = "joining thread")
//
// FUNCTIONS IN std::this_thread NAMESPACE:
// std::this_thread::yield()      -> hint: let other threads run
// std::this_thread::get_id()     -> get current thread's ID
// std::this_thread::sleep_for()  -> pause for a duration
// std::this_thread::sleep_until()-> pause until a time point
//
// ALSO FROM Thread Support Library:
// std::mutex                     -> mutual exclusion
// std::condition_variable        -> thread synchronization
// std::future / std::promise     -> async results
// std::atomic                    -> lock-free atomic operations

// ============================================================
// SECTION 2: THREAD LIBRARIES - HISTORY (from your screenshot)
// ============================================================

// BEFORE C++11: no standard threading in C++
// Used platform-specific or third-party:
// -> pthreads (POSIX): C-style, Linux/Mac
//    pthread_create(&tid, NULL, func, arg);
//    pthread_join(tid, NULL);
// -> Boost.Thread: C++ wrapper, portable
// -> Intel TBB (Thread Building Blocks): task-based
//
// YOUR SCREENSHOT: "std::thread is implemented with pthread
//                  on most POSIX systems"
// -> std::thread is essentially a C++ wrapper over pthreads on Linux
// -> same performance, much safer/cleaner API
//
// RECOMMENDATION (from slide): "use standard C++ threading library
// for portability reasons as the default choice"
// -> std::thread works on Windows, Linux, Mac without changes
// -> pthreads: Linux/Mac only
// -> Win32 threads: Windows only
// -> std::thread: ONE code for ALL platforms

// ============================================================
// SECTION 3: WHEN TO USE THREADS (from your screenshot)
// ============================================================

// 1. HEAVY COMPUTATIONS:
// -> GPU: 100s-1000s of threads for massively parallel tasks
//    (graphics rendering, ML, physics simulation)
// -> CPU: threads for complex computations where decisions needed
//    (your ISSST radar simulation: separate thread per sensor)
// -> CUDA: take advantage of GPU hardware directly
//
// 2. SEPARATING WORK:
// -> Performance: work happens simultaneously instead of sequentially
// -> SIMPLIFIES LOGIC: each thread has ONE clear responsibility
//    (more important than raw performance gain!)
//
// ISSST EXAMPLE (from your project):
// Thread 1: receive DIS PDUs from network (I/O bound, waits a lot)
// Thread 2: process entity states (CPU bound, runs DIS protocol)
// Thread 3: Qt UI thread (must stay responsive)
// -> WITHOUT threads: UI freezes while waiting for network data
// -> WITH threads: all three run concurrently, app stays responsive

// ============================================================
// SECTION 4: HIGH-LEVEL VIEW OF THREADS (from your screenshot)
// ============================================================

// ONE PROCESS = ONE APPLICATION = can have MANY THREADS
//
// WHAT EACH THREAD HAS (its OWN private):
// -> Stack (stack1, stack2 in diagram) - local variables
// -> Thread ID (TID) - unique identifier
// -> Program Counter (PC) - which instruction executing
// -> Stack Pointer (SP) - current stack position
// -> Data registers - CPU register state
// -> Condition codes - CPU flag state
// -> Its own logical control flow (can run different code)
//
// WHAT THREADS SHARE (with ALL other threads in same process):
// -> Heap memory (new/delete allocations)
// -> Global/static variables (read/write data)
// -> Code segment (read-only code/data)
// -> Shared libraries
// -> File descriptors
// -> Kernel context (VM structures, descriptor table)
//
// THIS IS BOTH THE POWER AND THE DANGER:
// POWER: threads can share data easily (same heap, same globals)
// DANGER: unsynchronized access to shared data = race conditions!
// (two threads writing to same variable = data corruption)

// ============================================================
// SECTION 5: std::thread CREATION - YOUR CODE EXPLAINED
// ============================================================

void test(int x)
{
    std::cout << "hello from thread" << '\n';
    std::cout << "Argument passed in: " << x << '\n';
}

int main()
{
    // CREATE A THREAD:
    std::thread myThread(&test, 100);
    //            ^         ^      ^
    //            |         |      argument passed to test()
    //            |         function pointer (& optional for free functions)
    //            thread object - creates and STARTS the thread immediately
    //
    // AS SOON AS THIS LINE RUNS:
    // -> new thread is created by OS
    // -> test(100) starts executing IN THE NEW THREAD
    // -> main() continues executing IN ITS OWN THREAD
    // -> BOTH threads run "simultaneously" (or interleaved on single core)
    //
    // ORDER IS NOT GUARANTEED:
    // "hello from thread" might print BEFORE or AFTER
    // "hello from my main thread" - it's a race!

    myThread.join();
    // JOIN = "wait for this thread to finish before continuing"
    // main() BLOCKS HERE until myThread completes test(100)
    // -> ensures test() finishes before main() continues
    //
    // WITHOUT join() OR detach():
    // -> std::thread destructor called on myThread
    // -> if not joined/detached: std::terminate() called!
    // -> YOUR COMMENTED: // myThread.join(); at end = WRONG!
    //    (would terminate() because thread already finished)
    //    Actually: calling join() twice = exception!

    std::cout << "hello from my main thread" << '\n';
    // This ALWAYS prints AFTER test() finishes (because of join)
    // Without join: might print BEFORE test() (race condition)

    return 0;
}

// OUTPUT (always in this order after join):
// hello from thread
// Argument passed in: 100
// hello from my main thread

// ============================================================
// SECTION 6: THREAD CONSTRUCTOR - WHAT IT ACCEPTS
// ============================================================

// std::thread can be constructed with ANY CALLABLE:

// FREE FUNCTION:
// std::thread t1(&test, 100);       // function pointer
// std::thread t1(test, 100);        // same (& optional for functions)

// LAMBDA (most common in modern C++):
// std::thread t2([](int x) {
//     std::cout << x;
// }, 42);

// FUNCTOR (struct with operator()):
// struct Task {
//     void operator()(int x) { std::cout << x; }
// };
// std::thread t3(Task{}, 42);

// MEMBER FUNCTION:
// struct Obj {
//     void run(int x) { std::cout << x; }
// };
// Obj obj;
// std::thread t4(&Obj::run, &obj, 42);
//                            ^
//                            pointer to object!

// ============================================================
// SECTION 7: JOIN vs DETACH - THE TWO CHOICES
// ============================================================

// EVERY thread must be either JOINED or DETACHED before destruction!
//
// JOIN: myThread.join()
// -> calling thread (main) WAITS until myThread finishes
// -> resources cleaned up after join
// -> can only call join() ONCE per thread
// -> after join: thread is no longer joinable
// -> USE WHEN: you need the result before continuing
//
// DETACH: myThread.detach()
// -> calling thread says "I don't care when you finish"
// -> thread runs independently (becomes "daemon" thread)
// -> resources cleaned up automatically when thread finishes
// -> after detach: you lose control of the thread
// -> USE WHEN: fire-and-forget (logging, background cleanup)
//
// NEITHER JOIN NOR DETACH:
// -> std::thread destructor calls std::terminate()
// -> program crashes with error!
// -> ALWAYS join or detach!
//
// CHECK IF JOINABLE:
// if (myThread.joinable()) myThread.join();

// ============================================================
// SECTION 8: std::this_thread FUNCTIONS
// ============================================================

// void exampleThisThread() {
//
//     // Get this thread's ID:
//     std::thread::id tid = std::this_thread::get_id();
//     std::cout << "Thread ID: " << tid << '\n';
//
//     // Sleep for duration:
//     std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     // pauses THIS thread for 100ms
//     // other threads continue running!
//
//     // Sleep until time point:
//     auto wakeTime = std::chrono::steady_clock::now()
//                   + std::chrono::seconds(1);
//     std::this_thread::sleep_until(wakeTime);
//
//     // Yield (hint to OS: let other threads run):
//     std::this_thread::yield();
//     // useful in spin-wait loops to avoid hogging CPU
// }

// ============================================================
// SECTION 9: std::jthread (C++20) - SAFER ALTERNATIVE
// ============================================================

// std::jthread = "joining thread"
// -> automatically calls join() in destructor
// -> also supports cooperative cancellation (stop_token)
// -> PREFER jthread over thread in C++20 code
//
// std::jthread jt(test, 100);
// // jt goes out of scope -> AUTOMATICALLY joins!
// // no need to manually call join()
// // no risk of forgetting to join (no std::terminate)
//
// COOPERATIVE CANCELLATION:
// std::jthread jt([](std::stop_token st) {
//     while (!st.stop_requested()) {
//         // do work
//     }
// });
// jt.request_stop();  // signal thread to stop
// // jt destructor joins automatically

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::thread and how do you create one?
//     -> Represents an OS thread of execution. Created by passing
//        a callable (function, lambda, functor) to the constructor.
//        Thread starts IMMEDIATELY on construction.
//        std::thread t(func, arg1, arg2);

// Q2: What must you do with every std::thread before it's destroyed?
//     -> EITHER join() or detach().
//        join(): wait for thread to complete.
//        detach(): let thread run independently.
//        Neither -> destructor calls std::terminate() (crash!).

// Q3: What is the difference between join() and detach()?
//     -> join(): calling thread WAITS for the thread to finish.
//        Resources freed after join. Thread no longer usable.
//        detach(): thread runs independently, caller doesn't wait.
//        Fire-and-forget. Resources freed when thread finishes.

// Q4: What do threads share and what is private to each thread?
//     -> Shared: heap memory, globals, static vars, code, file descriptors.
//        Private: stack, registers, program counter, thread ID.
//        Shared memory = power (easy data sharing) + danger (race conditions).

// Q5: What happens if you call join() twice on the same thread?
//     -> Throws std::system_error (joinable() returns false after first join).
//        Always check joinable() first, or use RAII wrapper.

// Q6: Why is std::jthread (C++20) preferred over std::thread?
//     -> Automatically joins in destructor (no risk of terminate).
//        Supports cooperative cancellation via stop_token.
//        RAII-safe: no manual join/detach management needed.

// Q7: When would you use threads vs std::async?
//     -> std::thread: when you need full control (priority, CPU affinity).
//        std::async: when you just need a result asynchronously.
//        async is higher-level (may use thread pool, easier error handling).
//        thread for long-running tasks, async for one-off computations.

// Q8: What is std::thread::hardware_concurrency()?
//     -> Returns number of concurrent threads the hardware supports
//        (logical CPU cores including hyperthreading).
//        Use to decide thread pool size for CPU-bound work.
// //        Returns 0 if not determinable.
// ```

// **The thread model (from your screenshot):**

// ```
// ONE PROCESS (your application):

// Thread 1 (main)          Thread 2 (myThread)    SHARED:
// ┌─────────────┐          ┌─────────────┐         ┌──────────────┐
// │ Stack 1     │          │ Stack 2     │         │ Heap memory  │
// │ local vars  │          │ local vars  │         │ Global vars  │
// │ PC₁         │          │ PC₂         │         │ Code segment │
// │ Registers₁  │          │ Registers₂  │         │ Libraries    │
// │ TID₁        │          │ TID₂        │         └──────────────┘
// └─────────────┘          └─────────────┘
// runs main()              runs test(100)          ← DANGER ZONE
//                                                    (race conditions)
// ```

// **Thread lifecycle:**

// ```cpp
// std::thread t(func, arg);  // ① CREATE: OS creates thread, func starts
//                             //   main() and func() run concurrently

// t.join();                  // ② SYNC: main() WAITS for func() to finish
//                            //   OR: t.detach() = run independently

// // ③ DESTROY: after join/detach, t's destructor is safe
// // Without ②: destructor → std::terminate() → CRASH!
// ```

// > **The key insight from the slide**: using threads to separate work **simplifies logic** as much as it improves performance. A DIS network thread that only receives packets, a simulation thread that only processes state, and a UI thread that only draws — each thread is simpler to reason about than one thread trying to do all three. This is why threads are worth the synchronization complexity even when you don't need raw performance.