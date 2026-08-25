#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <stdexcept>

// ============================================================
// std::lock_guard - RAII MUTEX - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: THE PROBLEM WITH RAW lock()/unlock()
// ============================================================

// YOUR COMMENTED CODE shows the EXACT problem:
//
// glock.lock();
// try {
//     shared_value++;
//     throw "dangerous... abort";   <- exception thrown!
// }
// catch (...) {
//     std::cout << "handle exception";
//     // glock.unlock();            <- MUST remember to unlock here!
//     return;                       <- if forgot unlock: DEADLOCK!
// }
// glock.unlock();                   <- only reached if no exception
//
// PROBLEMS:
// 1. EXCEPTION PATH: if you forget glock.unlock() in catch block
//    -> mutex STAYS LOCKED FOREVER
//    -> every other thread blocks at lock() FOREVER
//    -> application hangs = DEADLOCK
//
// 2. EARLY RETURN: multiple return paths need multiple unlocks
//    -> easy to miss one
//
// 3. MAINTENANCE: add new return path later? easy to forget unlock
//
// ROOT CAUSE: manual resource management = human error prone

// ============================================================
// SECTION 2: RAII - THE SOLUTION PRINCIPLE
// ============================================================

// YOUR COMMENT: "lock_guard is convenient RAII style mechanism"
//
// RAII = Resource Acquisition Is Initialization
// -> ACQUIRE resource in CONSTRUCTOR
// -> RELEASE resource in DESTRUCTOR
// -> destructor ALWAYS runs (even on exception, return, scope exit)
//
// lock_guard follows RAII for mutexes:
// CONSTRUCTOR: calls mutex.lock()   (acquire)
// DESTRUCTOR:  calls mutex.unlock() (release)
//
// THE KEY GUARANTEE:
// C++ GUARANTEES: destructors run when objects go out of scope
// EVEN IF: exception thrown, early return, scope exit
// SO: lock_guard ALWAYS unlocks, no matter what!

// ============================================================
// SECTION 3: YOUR CODE - HOW lock_guard WORKS
// ============================================================

static int shared_value = 0;
std::mutex glock;

void share_value_increment()
{
    std::lock_guard<std::mutex> lockguard(glock);
    //              ^^^^^^^^^^^              ^^^^^
    //              type                     mutex to protect
    //
    // WHAT HAPPENS HERE:
    // 1. lockguard constructed -> glock.lock() called
    // 2. ONLY THIS THREAD is inside the critical section now
    // 3. All other threads BLOCK at their own lock_guard constructor

    shared_value = shared_value + 1;
    // SAFE: only one thread reaches here at a time

    // ┌─ What about exceptions? ────────────────────────────┐
    // │  throw std::runtime_error("something went wrong");  │
    // │                                                      │
    // │  Even if exception thrown here:                      │
    // │  -> C++ unwinds the stack                           │
    // │  -> lockguard DESTRUCTOR is called                  │
    // │  -> glock.unlock() happens AUTOMATICALLY            │
    // │  -> other threads can proceed                       │
    // │  NO DEADLOCK! The RAII guarantees this.             │
    // └─────────────────────────────────────────────────────┘

} // <- lockguard DESTRUCTOR called here
  //    glock.unlock() called AUTOMATICALLY
  //    regardless of: normal exit, early return, or exception!

// ============================================================
// SECTION 4: PROOF - EXCEPTION SAFETY WITH lock_guard
// ============================================================

void exceptionSafeExample()
{
    std::lock_guard<std::mutex> lock(glock);
    // locked!

    shared_value++;

    // SCENARIO A: normal flow
    // function returns normally -> lock destructor -> unlock() ✅

    // SCENARIO B: exception thrown
    if (shared_value > 500) {
        throw std::runtime_error("too high!");
        // exception propagates UP the call stack
        // lock destructor STILL called during unwinding
        // glock.unlock() STILL happens ✅
        // other threads NOT deadlocked ✅
    }

    // SCENARIO C: early return
    if (shared_value == 0) {
        return;  // lock destructor called here ✅
    }

    // SCENARIO D: nested scope
    {
        // inner scope - lock already held by outer lock_guard
        // (would deadlock if you tried to lock glock again here!)
        shared_value++;
    }  // inner scope exits, but lockguard lives until function end
}

// ============================================================
// SECTION 5: SCOPING TRICK - MINIMIZE LOCK HOLD TIME
// ============================================================

// PROBLEM: holding lock too long blocks other threads
// SOLUTION: use inner scope to control lock lifetime precisely

void minimizeLockTime()
{
    // Expensive work OUTSIDE the lock (no need to hold it):
    int expensiveResult = computeExpensiveThing(); // long operation

    {   // ← inner scope just for the lock
        std::lock_guard<std::mutex> lock(glock);
        // ┌─ CRITICAL SECTION (as short as possible!) ──┐
        shared_value += expensiveResult;
        // └─────────────────────────────────────────────┘
    }   // ← lock_guard destructor here = unlock IMMEDIATELY

    // More work OUTSIDE the lock:
    postProcessResult(shared_value);  // doesn't need the lock
}
// RULE: hold the lock for the MINIMUM time necessary
// long-held locks = other threads waiting = reduced concurrency

// ============================================================
// SECTION 6: lock_guard vs unique_lock vs scoped_lock
// ============================================================

// LOCK_GUARD (your code - simplest):
// std::lock_guard<std::mutex> lg(m);
// -> locks on construction, unlocks on destruction
// -> CANNOT unlock manually (no unlock() method)
// -> CANNOT be moved or copied
// -> USE: simple critical sections, most common case

// UNIQUE_LOCK (flexible):
// std::unique_lock<std::mutex> ul(m);
// -> locks on construction, unlocks on destruction
// -> CAN unlock/lock manually
// -> CAN be moved (transferred ownership)
// -> REQUIRED for std::condition_variable
// -> USE: producer-consumer, deferred lock, conditional unlock

// SCOPED_LOCK (C++17, multiple mutexes):
// std::scoped_lock sl(m1, m2, m3);
// -> locks ALL mutexes atomically (no deadlock!)
// -> unlocks all on destruction
// -> USE: when you need multiple mutexes

// COMPARISON:
// Feature              | lock_guard | unique_lock | scoped_lock
// ---------------------|------------|-------------|------------
// Auto-unlock          | YES        | YES         | YES
// Manual unlock/lock   | NO         | YES         | NO
// Multiple mutexes     | NO         | NO          | YES
// Condition variable   | NO         | YES         | NO
// Movable              | NO         | YES         | NO
// Overhead             | LOWEST     | SMALL       | LOW

// ============================================================
// SECTION 7: THE DEADLOCK SCENARIO PREVENTED
// ============================================================

// WITHOUT lock_guard (DEADLOCK POSSIBLE):
// Thread 1: glock.lock()
//           shared_value++
//           throw exception!   <- lock() never paired with unlock()
//           // glock.unlock() NEVER CALLED
// Thread 2: glock.lock()       <- BLOCKED FOREVER (deadlock!)
// Thread 3: glock.lock()       <- BLOCKED FOREVER (deadlock!)
// Application: HANGS
//
// WITH lock_guard (NO DEADLOCK):
// Thread 1: lockguard constructed -> glock.lock()
//           shared_value++
//           throw exception!
//           STACK UNWINDING: ~lock_guard() -> glock.unlock() ✅
// Thread 2: glock.lock() succeeds -> proceeds safely ✅
// Thread 3: glock.lock() succeeds -> proceeds safely ✅
// Application: CONTINUES NORMALLY

// ============================================================
// SECTION 8: COMMON MISTAKES WITH MUTEX
// ============================================================

// MISTAKE 1: Lock on wrong mutex (different mutex = no protection)
std::mutex wrongMutex;
// void bad() {
//     std::lock_guard<std::mutex> lock(wrongMutex); // wrong mutex!
//     shared_value++;  // NOT protected by glock!
// }

// MISTAKE 2: Locking glock when already holding glock (deadlock)
// void bad2() {
//     std::lock_guard<std::mutex> lock1(glock);
//     std::lock_guard<std::mutex> lock2(glock); // DEADLOCK!
//     // lock_guard(glock) tries to lock a mutex THIS THREAD already holds
//     // std::mutex is NOT recursive -> deadlock!
//     // Fix: use std::recursive_mutex if you need this
// }

// MISTAKE 3: Using shared_value AFTER the lock is released
// void bad3() {
//     int* ptr;
//     {
//         std::lock_guard<std::mutex> lock(glock);
//         ptr = &shared_value;  // pointer to protected data
//     }  // lock released here!
//     *ptr = 5;  // RACE CONDITION: using shared data without lock!
// }

// MISTAKE 4: Forgetting the variable name (temporary!)
// std::lock_guard<std::mutex> (glock);  // temporary! destroyed immediately!
// shared_value++;  // NOT protected!
// CORRECT: std::lock_guard<std::mutex> lock(glock);  // named variable!

// ============================================================
// SECTION 9: ISSST/DIS CONNECTION
// ============================================================

// In your simulation code, lock_guard protects shared state:
//
// std::mutex entityMutex;
// std::unordered_map<uint32_t, EntityState> entities;
//
// // Network thread (writes):
// void onPDUReceived(EntityStatePDU pdu) {
//     std::lock_guard<std::mutex> lock(entityMutex);
//     entities[pdu.entityID] = pdu.state;
//     // mutex released automatically, even if exception in map.operator[]
// }
//
// // Simulation thread (reads):
// EntityState getState(uint32_t id) {
//     std::lock_guard<std::mutex> lock(entityMutex);
//     return entities.at(id);  // might throw if id not found
//     // lock_guard ensures unlock even if .at() throws out_of_range!
// }
//
// // UI thread (reads):
// void updateDisplay() {
//     std::lock_guard<std::mutex> lock(entityMutex);
//     for (const auto& [id, state] : entities) {
//         drawEntity(id, state);
//     }
//     // unlock at end of for loop scope
// }

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is RAII and how does lock_guard implement it?
//     -> RAII: acquire resource in constructor, release in destructor.
//        lock_guard: constructor calls mutex.lock(), destructor calls
//        mutex.unlock(). Destructor ALWAYS runs (exception or not).

// Q2: What problem does lock_guard solve over raw lock/unlock?
//     -> Exception safety. Raw unlock() can be skipped if exception
//        thrown or early return. lock_guard destructor ALWAYS runs.
//        Eliminated entire class of "forgot to unlock" deadlocks.

// Q3: What is a DEADLOCK?
//     -> Thread(s) waiting forever for a resource that can never be released.
//        Cause: mutex locked but never unlocked (forgotten unlock, exception).
//        Symptom: application hangs, threads never proceed.
//        Prevention: RAII (lock_guard), always unlock in same scope.

// Q4: What is the difference between lock_guard and unique_lock?
//     -> lock_guard: simpler, cannot manually unlock, lower overhead.
//        unique_lock: can manually unlock/relock, movable, required
//        for condition_variable. Use lock_guard unless you need more control.

// Q5: Can you call unlock() on a lock_guard?
//     -> NO. lock_guard has no unlock() method by design.
//        It forces you to use scoping for unlock control.
//        If you need manual unlock: use unique_lock instead.

// Q6: What is the "temporary lock_guard" mistake?
//     -> std::lock_guard<std::mutex>(m); <- no variable name!
//        Creates a TEMPORARY that's destroyed immediately.
//        Mutex locked then instantly unlocked -> NO protection!
//        ALWAYS give it a name: std::lock_guard<std::mutex> lock(m);

// Q7: How do you minimize lock hold time?
//     -> Wrap only the critical section in an inner scope.
//        Do expensive work BEFORE acquiring lock.
//        Release lock ASAP after writing shared data.
//        Short lock hold = other threads wait less = better concurrency.

// Q8: What does your code print and why?
//     -> Always prints "Shared Value: 1000".
//        Each of 1000 threads increments once (NOT 5 times in THIS code).
//        lock_guard ensures exactly one thread increments at a time.
//        Result is deterministic (same every run).

int main()
{
    std::vector<std::thread> threads;
    threads.reserve(1000);

    for (int i = 0; i < 1000; i++) {
        threads.push_back(std::thread(share_value_increment));
    }

    for (int i = 0; i < 1000; i++) {
        threads[i].join();
    }

    std::cout << "Shared Value: " << shared_value << '\n';
    // Always: 1000 (deterministic with lock_guard)

    return 0;
}
// ```

// **RAII guarantee - what lock_guard's destructor ensures:**

// ```
// Normal exit:       lock()...unlock() ✅
// Early return:      lock()...unlock() ✅
// Exception thrown:  lock()...unlock() ✅ (stack unwinding!)
// Scope exit:        lock()...unlock() ✅

// Raw lock/unlock:
// Normal exit:       lock()...unlock() ✅
// Early return:      lock()... MISSING unlock() ❌ DEADLOCK!
// Exception thrown:  lock()... MISSING unlock() ❌ DEADLOCK!
// ```

// **Minimize lock hold time:**

// ```cpp
// // BAD - holding lock during expensive work:
// {
//     std::lock_guard<std::mutex> lock(m);
//     auto result = expensiveComputation();  // other threads BLOCKED here!
//     shared_value = result;
// }

// // GOOD - lock only for the write:
// auto result = expensiveComputation();  // lock NOT held, others can proceed
// {
//     std::lock_guard<std::mutex> lock(m);
//     shared_value = result;             // lock held for minimum time
// }
// ```

// > **The one rule**: every `mutex.lock()` needs a matching `mutex.unlock()` — and humans are terrible at guaranteeing this across exceptions and early returns. `lock_guard` makes the compiler guarantee it for you. There is almost no situation where raw `lock()`/`unlock()` is better than `lock_guard` for simple critical sections. If you need more control, reach for `unique_lock`, not raw calls.