#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

// ============================================================
// MULTIPLE THREADS WITH VECTOR - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: YOUR QUESTION - what if join() is inside the create loop?
// ============================================================

// YOUR QUESTION: "what if we write threads[i].join() inside the first loop?"
//
// CODE WOULD LOOK LIKE:
// for (int i = 0; i < 10; i++) {
//     threads.push_back(std::thread(lambda, i));
//     threads[i].join();   // join IMMEDIATELY after creating
// }
//
// WHAT HAPPENS:
// i=0: create thread 0 → JOIN (wait for thread 0 to finish) → continue
// i=1: create thread 1 → JOIN (wait for thread 1 to finish) → continue
// i=2: create thread 2 → JOIN (wait for thread 2 to finish) → continue
// ...
//
// RESULT: SEQUENTIAL execution! threads run ONE AT A TIME
// Thread 1 must finish BEFORE Thread 2 even starts!
// No concurrency benefit at all - same as calling lambda(0..9) directly
//
// YOUR ACTUAL CODE (two separate loops):
// Loop 1: create ALL 10 threads first (all start running concurrently!)
// Loop 2: join ALL 10 threads (wait for all to finish)
// RESULT: TRUE concurrency - all 10 threads run SIMULTANEOUSLY

// ============================================================
// SECTION 2: THE VISUAL DIFFERENCE
// ============================================================

// JOIN INSIDE CREATE LOOP (sequential - BAD for performance):
// Time:  0ms    10ms   20ms   30ms   40ms
// T0:    [===]
// T1:          [===]
// T2:                 [===]
// T3:                        [===]
// Total: 40ms (sum of all thread times)
//
// JOIN IN SEPARATE LOOP (concurrent - GOOD):
// Time:  0ms    10ms
// T0:    [===]
// T1:    [===]
// T2:    [===]
// T3:    [===]
// Total: ~10ms (longest single thread time)
// -> Up to 4x speedup (with 4 cores) or more

// ============================================================
// SECTION 3: YOUR CODE EXPLAINED
// ============================================================

int main()
{
    auto lambda = [](int x)
    {
        // std::this_thread::get_id() = unique ID of THIS thread
        // different for each thread (assigned by OS)
        std::cout << "Hello from thread: "
                  << std::this_thread::get_id() << '\n';
        std::cout << "Argument passed in: " << x << '\n';
        // OUTPUT IS INTERLEAVED AND UNPREDICTABLE ORDER
        // all 10 threads running simultaneously = race on cout!
        // might see garbled output like:
        // "Hello Hello from thread from thread: ..."
        // (cout is not thread-safe without mutex!)
    };

    std::vector<std::thread> threads;
    threads.reserve(10);  // good practice: avoid reallocation
    //                       reallocation while threads exist = UB!

    // LOOP 1: CREATE all threads (they ALL start running immediately)
    for (int i = 0; i < 10; i++)
    {
        threads.push_back(std::thread(lambda, i));
        // std::thread(lambda, i) creates and STARTS the thread
        // push_back stores the thread in the vector
        // loop continues WITHOUT waiting for thread to finish
        // all 10 threads are running by end of this loop!
    }
    // At this point: 10 threads running concurrently + main thread
    // = 11 concurrent flows of execution!

    // LOOP 2: JOIN all threads (wait for ALL to finish)
    for (int i = 0; i < 10; i++)
    {
        threads[i].join();
        // wait for thread i to finish
        // i=0: wait for thread 0
        // i=1: thread 1 might ALREADY be done (we joined 0 first)
        // i=2: thread 2 might ALREADY be done
        // etc.
        // joining in order is fine - threads run concurrently
        // regardless of join order
    }

    std::cout << "hello from my main thread\n";
    // ALWAYS prints LAST (after all joins complete)

    return 0;
}

// ============================================================
// SECTION 4: WHY threads.push_back vs threads[i].join() in loops
// ============================================================

// WHY NOT just do:
// std::thread t0(lambda, 0);
// std::thread t1(lambda, 1);
// std::thread t2(lambda, 2);
// ...
// t0.join(); t1.join(); t2.join();
//
// PROBLEM: doesn't scale! for N threads = N lines of code
// vector<thread> = handles ANY number of threads with a loop
// N could be: number of CPU cores, number of files to process,
//             number of entities to simulate, etc.

// ============================================================
// SECTION 5: THE COUT RACE CONDITION IN YOUR CODE
// ============================================================

// std::cout IS NOT THREAD SAFE by default!
// (individual operator<< calls are safe, but sequences are not)
//
// WHAT YOU MIGHT SEE (garbled output):
// "Hello Hello from thread: from thread: 140234 140235
//  Argument passed in: Argument passed in: 0 1"
//
// WHY: Thread 0 starts writing "Hello from thread: "
//      Thread 1 starts writing "Hello from thread: " simultaneously
//      OS switches between them mid-write
//      Output gets mixed!
//
// FIX 1: Use mutex to protect cout:
// std::mutex coutMutex;
// auto lambda = [&coutMutex](int x) {
//     std::lock_guard<std::mutex> lock(coutMutex);
//     std::cout << "Thread " << std::this_thread::get_id()
//               << " arg: " << x << '\n';
//     // lock released here -> next thread can print
// };
//
// FIX 2: Use std::osyncstream (C++20):
// std::osyncstream(std::cout) << "Thread " << x << '\n';
// -> atomic: whole stream writes as one unit (no interleaving)

// ============================================================
// SECTION 6: IMPORTANT - VECTOR REALLOCATION DANGER
// ============================================================

// DANGER: if vector reallocates while threads are running:
// -> all existing thread objects might be MOVED in memory
// -> if threads hold references/pointers to vector elements: DANGLING!
//
// SAFE PATTERN:
// threads.reserve(10);            // pre-allocate BEFORE creating threads
// for (int i=0; i<10; i++)
//     threads.emplace_back(lambda, i);  // no reallocation possible!
//
// OR: resize first:
// threads.resize(10);             // 10 default-constructed threads (invalid!)
// for (int i=0; i<10; i++)
//     threads[i] = std::thread(lambda, i);  // move-assign
//
// NOTE: std::thread is MOVE-ONLY (cannot copy!)
// push_back(std::thread(...)) works because thread is moved into vector
// threads.push_back(t) where t is lvalue = COMPILE ERROR!
// threads.push_back(std::move(t)) = correct for named thread

// ============================================================
// SECTION 7: EMPLACE_BACK - CLEANER ALTERNATIVE
// ============================================================

// threads.emplace_back(lambda, i);
// vs
// threads.push_back(std::thread(lambda, i));
//
// emplace_back: constructs thread DIRECTLY in vector (no move!)
// push_back:    constructs thread THEN moves into vector
// Prefer emplace_back (cleaner, one less move operation)

// ============================================================
// SECTION 8: MODERN RANGE-BASED JOIN
// ============================================================

// Instead of:
// for (int i=0; i<threads.size(); i++) threads[i].join();
//
// Prefer:
// for (auto& t : threads) t.join();
//
// Or even cleaner with algorithm:
// std::for_each(threads.begin(), threads.end(),
//               [](std::thread& t){ t.join(); });
//
// C++20 jthread version (auto-joins on destruction!):
// std::vector<std::jthread> jthreads;
// for (int i=0; i<10; i++)
//     jthreads.emplace_back(lambda, i);
// // NO explicit join needed! jthread destructor joins automatically

// ============================================================
// SECTION 9: COMPLETE IMPROVED VERSION
// ============================================================

// void improvedVersion()
// {
//     std::mutex coutMutex;

//     auto safeLambda = [&coutMutex](int x)
//     {
//         // Simulate some work:
//         std::this_thread::sleep_for(
//             std::chrono::milliseconds(10 * x));

//         // Thread-safe output:
//         std::lock_guard<std::mutex> lock(coutMutex);
//         std::cout << "Thread ID: " << std::this_thread::get_id()
//                   << " | arg: " << x << '\n';
//     };

//     std::vector<std::thread> threads;
//     threads.reserve(10);  // prevent reallocation

//     // Create all threads (all start concurrently):
//     for (int i = 0; i < 10; i++) {
//         threads.emplace_back(safeLambda, i);
//     }

//     // Join all (wait for all to complete):
//     for (auto& t : threads) {
//         t.join();  // cleaner range-based for
//     }

//     std::cout << "All threads done!\n";
// }

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: Why are the two loops (create + join) separate?
//     -> To achieve TRUE concurrency. Creating all threads first
//        lets them all run simultaneously. Joining inside the create
//        loop makes them run sequentially (defeats the purpose).
//        Two loops = parallel. One loop with join = serial.

// Q2: Why use vector<thread> instead of individual thread variables?
//     -> Scales to any number of threads (loop + container).
//        Can determine thread count at runtime (cores, file count etc.)
//        Individual variables: N threads = N lines (doesn't scale).

// Q3: Is std::cout thread-safe?
//     -> Individual operator<< calls are safe (since C++11).
//        BUT sequences of << calls are NOT atomic (can interleave).
//        Fix: protect with mutex, or use std::osyncstream (C++20).

// Q4: Why use reserve() before creating threads in vector?
//     -> Prevents reallocation during push_back.
//        If vector reallocates while threads exist: UB (thread objects moved).
//        reserve() ensures capacity up front = no reallocation.

// Q5: Can you copy a std::thread?
//     -> NO. std::thread is MOVE-ONLY (copy constructor deleted).
//        push_back(std::thread(...)) works (move from temporary).
//        push_back(namedThread) = COMPILE ERROR.
//        push_back(std::move(namedThread)) = correct.

// Q6: What is emplace_back vs push_back for threads?
//     -> emplace_back(lambda, i): constructs thread directly in vector.
//        push_back(std::thread(lambda,i)): constructs then moves.
//        emplace_back is preferred (one fewer move operation).

// Q7: What is std::this_thread::get_id()?
//     -> Returns the thread ID of the CURRENTLY EXECUTING thread.
//        Type: std::thread::id (printable with <<).
//        Unique per thread. Main thread has its own ID.
//        Used for debugging which thread is running.

// Q8: What happens if you forget to join one thread in the vector?
//     -> That thread's destructor is called without join/detach.
//        std::thread destructor calls std::terminate() if joinable.
//        Program CRASHES. Fix: always join all, or use jthread.
// ```

// **The core difference - one picture:**

// ```
// JOIN INSIDE CREATE LOOP (sequential - BAD):
// for i in 0..9:
//     create thread[i]   ← start
//     join thread[i]     ← wait for finish BEFORE creating next

// Time: T0──T1──T2──T3──T4──T5──T6──T7──T8──T9  (serial, slow!)


// JOIN IN SEPARATE LOOP (concurrent - YOUR CODE - GOOD):
// Loop 1: create T0,T1,T2,...T9  ← all start at once
// Loop 2: join T0, join T1, ...  ← wait for all

// Time: T0═══╗
//       T1═══╣
//       T2═══╣  (all running at same time!)
//       T9═══╝
//            ^ all done here (only as long as the SLOWEST thread)
// ```

// **The cout race - what goes wrong:**

// ```
// Thread 0: "Hello from thread: "
// Thread 1:                      "Hello from thread: "  ← INTERLEAVED!
// Thread 0:                                            "140234"
// Result: "Hello from thread: Hello from thread: 140234"  ← garbled!

// FIX: mutex around cout so only ONE thread prints at a time:
// {
//     std::lock_guard<std::mutex> lock(coutMutex);
//     std::cout << "Thread " << id << " arg: " << x << '\n';
// }  // lock released, next thread can print
// ```

// > **The one rule to remember**: always create ALL threads first, then join ALL threads. Joining inside the creation loop makes threads run one-at-a-time (sequential), completely defeating the purpose of using threads. The whole point is that ALL threads are alive and running concurrently between the two loops.