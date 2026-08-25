#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

// ============================================================
// std::mutex, CRITICAL SECTION, try_lock - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: THE PROBLEM - RACE CONDITION WITHOUT MUTEX
// ============================================================

// WITHOUT mutex, what happens to shared_value++?
//
// shared_value = shared_value + 1 is NOT ONE operation!
// CPU actually does THREE steps:
// STEP 1: READ  shared_value from memory -> register (value=5)
// STEP 2: ADD   register + 1             -> register (value=6)
// STEP 3: WRITE register -> shared_value in memory
//
// WITH TWO THREADS (race condition):
// Thread A: READ shared_value = 5
// Thread B: READ shared_value = 5   <- reads SAME value before A writes!
// Thread A: ADD  5+1 = 6
// Thread B: ADD  5+1 = 6
// Thread A: WRITE 6 -> shared_value
// Thread B: WRITE 6 -> shared_value  <- OVERWRITES Thread A's result!
// RESULT: shared_value = 6 (should be 7!)
// ONE INCREMENT IS LOST - this is a RACE CONDITION
//
// WITHOUT MUTEX: 1000 threads * 5 increments = 5000 expected
// ACTUAL RESULT: could be anything < 5000 (data corruption!)

// ============================================================
// SECTION 2: WHAT IS A CRITICAL SECTION?
// ============================================================

// YOUR QUESTION: "critical section mean?"
//
// CRITICAL SECTION = a section of code that accesses SHARED RESOURCES
// and must NOT be executed by more than ONE thread at a time
//
// In your code:
// shared_value = shared_value + 1;  <- accesses shared_value
// shared_value = shared_value + 1;  <- accesses shared_value
// shared_value = shared_value + 1;  <- accesses shared_value
// shared_value = shared_value + 1;  <- accesses shared_value
// shared_value = shared_value + 1;  <- accesses shared_value
// ^^^ ALL FIVE LINES = one CRITICAL SECTION
//
// Everything between lock() and unlock() = critical section
// Only ONE thread can be INSIDE the critical section at any time
// All others WAIT at lock() until the thread inside calls unlock()

// ============================================================
// SECTION 3: YOUR CODE - mutex.lock() and mutex.unlock()
// ============================================================

static int shared_value = 0;
std::mutex glock;

void share_value_increment()
{
    glock.lock();
    // ┌─── CRITICAL SECTION START ───────────────────────────┐
    // │ Only ONE thread can be here at a time!                │
    // │ All other threads BLOCK at glock.lock() above         │
    shared_value = shared_value + 1;  // safe: only us here
    shared_value = shared_value + 1;
    shared_value = shared_value + 1;
    shared_value = shared_value + 1;
    shared_value = shared_value + 1;
    // └─── CRITICAL SECTION END ─────────────────────────────┘
    glock.unlock();
    // NOW: next waiting thread can acquire the lock
}

// RESULT: 1000 threads * 5 = 5000 EVERY TIME (deterministic!)
// GUARANTEE: no two threads ever inside critical section together

// ============================================================
// SECTION 4: PROBLEM WITH RAW lock()/unlock()
// ============================================================

// YOUR CODE uses raw lock()/unlock() - this is DANGEROUS:
//
// void bad_example() {
//     glock.lock();
//     shared_value++;
//     if (error_condition) {
//         return;  // FORGOT to unlock! DEADLOCK for all other threads!
//     }
//     throw std::exception(); // exception skips unlock()! DEADLOCK!
//     glock.unlock();          // never reached!
// }
//
// SOLUTION: RAII wrappers that auto-unlock on destruction

// ============================================================
// SECTION 5: LOCK GUARD - SAFER ALTERNATIVE (PREFER THIS)
// ============================================================

void safeIncrement()
{
    std::lock_guard<std::mutex> lock(glock);
    // ┌─── CRITICAL SECTION ─────────────────────────────────┐
    // │ lock_guard AUTOMATICALLY unlocks when it goes out of scope
    // │ even if: return, throw, exception - ALWAYS unlocks!  │
    shared_value = shared_value + 1;
    shared_value = shared_value + 1;
    shared_value = shared_value + 1;
    shared_value = shared_value + 1;
    shared_value = shared_value + 1;
    // └── lock_guard destructor called here = unlock() ───────┘
}
// RAII: constructor = lock(), destructor = unlock()
// Cannot forget to unlock - destructor ALWAYS runs

// ============================================================
// SECTION 6: YOUR QUESTION - try_lock EXAMPLE
// ============================================================

// try_lock() = attempt to lock, but DON'T WAIT if locked
// returns TRUE if lock acquired
// returns FALSE immediately if already locked (non-blocking!)
//
// mutex.lock()     -> BLOCKS until lock available (waiting)
// mutex.try_lock() -> returns immediately (true/false)

void tryLockExample()
{
    if (glock.try_lock())
    {
        // ┌─── Got the lock! ───────────────────────────┐
        shared_value++;
        glock.unlock();
        // └────────────────────────────────────────────┘
        std::cout << "Got lock, incremented\n";
    }
    else
    {
        // Lock is held by another thread - DON'T WAIT
        // Do something else instead:
        std::cout << "Lock busy, doing other work\n";
        // could: retry later, skip, use fallback, etc.
    }
}

// WHEN TO USE try_lock:
// -> when you have useful work to do if lock is busy
// -> implementing lock-free fallback behavior
// -> avoiding deadlock (try both locks, back off if either fails)
// -> polling-style acquisition with timeout logic

void tryLockWithRetry()
{
    // Spin until we get the lock (busy-wait - not recommended for long waits)
    while (!glock.try_lock()) {
        std::this_thread::yield();  // hint OS: let other threads run
        // or: sleep_for(microseconds) to reduce CPU usage
    }
    // HERE: we have the lock
    shared_value++;
    glock.unlock();
}

// ============================================================
// SECTION 7: ALL FOUR MUTEX TYPES
// ============================================================

// std::mutex
// -> basic mutex, non-recursive
// -> same thread CANNOT lock twice (deadlock if tried!)
// -> YOUR CODE uses this

// std::recursive_mutex
// -> same thread CAN lock multiple times
// -> must unlock same number of times
// -> USE: recursive functions that need the lock

// std::timed_mutex
// -> try_lock_for(duration): wait up to a time limit
// -> try_lock_until(time_point): wait until a specific time
// -> USE: when you need timeout behavior

// std::recursive_timed_mutex
// -> combines recursive + timed

// ============================================================
// SECTION 8: TIMED MUTEX EXAMPLE
// ============================================================

std::timed_mutex timedLock;

void timedLockExample()
{
    // Try to acquire for up to 100 milliseconds:
    if (timedLock.try_lock_for(std::chrono::milliseconds(100)))
    {
        // Got the lock within 100ms
        shared_value++;
        timedLock.unlock();
        std::cout << "Got timed lock\n";
    }
    else
    {
        // Couldn't get lock within 100ms - timeout!
        std::cout << "Timed out waiting for lock\n";
        // handle the timeout: skip, retry, error, etc.
    }
}

// WHEN TO USE timed_mutex:
// -> real-time systems where waiting too long is unacceptable
// -> YOUR ISSST: "if DIS network lock not available in 50ms, skip frame"
// -> deadlock detection: if waiting > N seconds, log error and abort

// ============================================================
// SECTION 9: unique_lock - MOST FLEXIBLE WRAPPER
// ============================================================

// lock_guard: simple RAII, always locked, no manual control
// unique_lock: RAII + can manually lock/unlock + works with condition_variable

void uniqueLockExample()
{
    // Basic usage (like lock_guard):
    std::unique_lock<std::mutex> lock(glock);
    shared_value++;
    // auto-unlocks on scope exit

    // Advanced: manual unlock (release early):
    std::unique_lock<std::mutex> lock2(glock);
    shared_value++;
    lock2.unlock();    // release lock early
    // do non-critical work here (without holding lock)
    doExpensiveComputation();
    lock2.lock();      // re-acquire if needed
    shared_value++;
    // unlocks again at scope exit

    // Deferred locking:
    std::unique_lock<std::mutex> lock3(glock, std::defer_lock);
    // doesn't lock yet!
    // ... do setup ...
    lock3.lock();      // lock when ready
    shared_value++;
}

// ============================================================
// SECTION 10: LOCKING MULTIPLE MUTEXES SAFELY
// ============================================================

// DEADLOCK SCENARIO:
// Thread A: locks mutex1, waits for mutex2
// Thread B: locks mutex2, waits for mutex1
// -> Both wait forever!
//
// std::lock() locks multiple mutexes ATOMICALLY (no deadlock):
std::mutex mutex1, mutex2;

void safeMultiLock()
{
    // Lock BOTH atomically (no deadlock possible):
    std::lock(mutex1, mutex2);

    // Adopt ownership (for RAII auto-unlock):
    std::lock_guard<std::mutex> lg1(mutex1, std::adopt_lock);
    std::lock_guard<std::mutex> lg2(mutex2, std::adopt_lock);

    // Both locked, both will auto-unlock at scope exit
    shared_value++;
}

// C++17 CLEANER VERSION: std::scoped_lock
void modernMultiLock()
{
    std::scoped_lock lock(mutex1, mutex2);  // locks both atomically
    shared_value++;
    // both auto-unlock at scope exit
}

// ============================================================
// SECTION 11: VISUAL - HOW MUTEX WORKS
// ============================================================

// Thread 1      Thread 2      Thread 3      shared_value
//   |              |              |              0
// lock()         lock()         lock()
//   |           BLOCKED        BLOCKED
// val = 0
// val + 1 = 5
// WRITE 5
// unlock()         |              |              5
//                lock()         BLOCKED
//                val = 5
//                val + 5 = 10
//                WRITE 10
//                unlock()        |              10
//                              lock()
//                              val = 10
//                              val + 5 = 15
//                              WRITE 15
//                              unlock()         15
//
// FINAL: 15 (correct! each thread's 5 increments happen atomically)

// ============================================================
// SECTION 12: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a critical section?
//     -> Code that accesses shared resources and must be executed
//        by only ONE thread at a time. Protected by mutex.
//        Between lock() and unlock() = the critical section.

// Q2: What is the difference between lock() and try_lock()?
//     -> lock(): BLOCKS until mutex is available (waiting).
//        try_lock(): returns immediately, true if got lock, false if busy.
//        Use try_lock when you have useful work to do while lock is busy.

// Q3: Why use lock_guard over raw lock()/unlock()?
//     -> RAII: lock_guard auto-unlocks in destructor.
//        Raw unlock() can be skipped by: early return, exception, throw.
//        Forgotten unlock = deadlock for ALL other threads.
//        lock_guard = exception-safe, cannot forget.

// Q4: What is deadlock and how does it happen?
//     -> Two or more threads wait for each other's locks forever.
//        Thread A holds mutex1, wants mutex2.
//        Thread B holds mutex2, wants mutex1.
//        Both wait forever. Fix: always lock in same order,
//        or use std::lock()/std::scoped_lock for multiple mutexes.

// Q5: What is the difference between mutex and recursive_mutex?
//     -> mutex: same thread cannot lock twice (deadlock).
//        recursive_mutex: same thread CAN lock N times, must unlock N times.
//        Use recursive_mutex for recursive functions needing the lock.

// Q6: When would you use timed_mutex?
//     -> When waiting indefinitely is unacceptable.
//        Real-time systems, watchdog timers, deadlock detection.
//        try_lock_for(100ms): "I'll wait 100ms, then give up."

// Q7: What is the result of your code without mutex?
//     -> Undefined (could be anything from 1 to 5000).
//        Race conditions on shared_value cause lost increments.
//        WITH mutex: always exactly 5000 (deterministic).

// Q8: What is scoped_lock and when do you use it?
//     -> C++17 RAII wrapper that locks MULTIPLE mutexes atomically.
//        Prevents deadlock when locking multiple mutexes.
//        std::scoped_lock lock(m1, m2, m3);  // all or nothing.

int main()
{
    // RESET for clean run:
    shared_value = 0;

    std::vector<std::thread> threads;
    threads.reserve(1000);

    for (int i = 0; i < 1000; i++) {
        threads.push_back(std::thread(share_value_increment));
    }

    for (int i = 0; i < 1000; i++) {
        threads[i].join();
    }

    std::cout << "Shared Value: " << shared_value << '\n';
    // ALWAYS prints: 5000 (with mutex = deterministic!)
    // WITHOUT mutex: could print anything < 5000

    // try_lock demo:
    if (glock.try_lock()) {
        std::cout << "Main got the lock!\n";
        glock.unlock();
    }

    return 0;
}
// ```

// **The race condition visualized:**

// ```
// WITHOUT MUTEX (race):            WITH MUTEX (safe):

// Thread A    Thread B             Thread A    Thread B
// READ  5     READ  5              lock()      BLOCKED...
// ADD   6     ADD   6              READ  5     BLOCKED...
// WRITE 6     WRITE 6              ADD   6     BLOCKED...
//                                  WRITE 6     BLOCKED...
// Result: 6 ← LOST one!          unlock()    lock() ← now allowed
//                                              READ  6
//                                              ADD   7
//                                              WRITE 7
//                                              unlock()
//                                 Result: 7 ✅ CORRECT!
// ```

// **lock() vs try_lock() vs timed:**

// ```cpp
// glock.lock();                              // WAIT forever (blocking)
// bool got = glock.try_lock();               // return immediately (non-blocking)
// bool got = tm.try_lock_for(100ms);         // wait UP TO 100ms
// bool got = tm.try_lock_until(deadline);   // wait UNTIL a time point
// ```

// **The RAII hierarchy:**

// ```
// lock_guard   → simplest, always locked, auto-unlocks ← USE MOST
// unique_lock  → flexible, can unlock/relock, needed for condition_variable
// scoped_lock  → locks MULTIPLE mutexes atomically (C++17) ← USE FOR MULTI-MUTEX

// Raw lock()/unlock() → AVOID (can forget, not exception-safe)
// ```

// > **The critical section rule**: keep it as **small as possible**. Only the code that TOUCHES shared data needs to be inside the critical section. Expensive operations (file I/O, network calls, heavy computation) should be OUTSIDE the mutex — holding a lock longer than necessary means other threads wait longer, destroying the concurrency benefit.