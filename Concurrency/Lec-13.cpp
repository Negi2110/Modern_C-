#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>

// ============================================================
// DATA RACE - WHAT IT IS, HOW TO DETECT, HOW TO FIX - NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS A DATA RACE?
// ============================================================

// DATA RACE = when TWO OR MORE threads access the SAME memory location
// AND at least ONE access is a WRITE
// AND the accesses are NOT synchronized
//
// RESULT: UNDEFINED BEHAVIOR (anything can happen!)
// -> wrong result (lost increments)
// -> crash
// -> memory corruption
// -> different result every run (non-deterministic)
//
// WITHOUT mutex, shared_value++ on 10000 threads:
// Expected: 10000
// Actual:   could be 7832, 9241, 6500... DIFFERENT EVERY RUN
//
// WHY INCREMENTS ARE LOST (the race):
// Thread A:  READ shared_value = 5000
// Thread B:  READ shared_value = 5000  ← same value!
// Thread A:  WRITE 5001
// Thread B:  WRITE 5001                ← overwrites A! One increment lost
//
// WITH mutex (your code): ALWAYS prints 10000
// -> lock prevents two threads reading/writing simultaneously
// -> each increment is a complete atomic operation

// ============================================================
// SECTION 2: YOUR CODE - SIMPLE AND CORRECT
// ============================================================

std::mutex gLock;
static int shared_value = 0;

void shared_value_increment()
{
    gLock.lock();
    // ┌── CRITICAL SECTION ───────────────────────────┐
    shared_value = shared_value + 1;  // safe: only us here
    // └───────────────────────────────────────────────┘
    gLock.unlock();
}

// WHAT HAPPENS WITH 10000 THREADS:
// Thread 1:    lock()  →  increment  →  unlock()
// Thread 2:             [waiting...]               lock()  →  ...
// Thread 3:             [waiting...]                          [waiting...]
// ...
// All 10000 complete, each increment counted exactly once
// Result: ALWAYS 10000 (deterministic)

// ============================================================
// SECTION 3: FOUR WAYS TO FIX A DATA RACE
// ============================================================

// ---- FIX 1: std::mutex (your code) ----
namespace fix_mutex
{
    std::mutex lock;
    int value = 0;

    void increment() {
        std::lock_guard<std::mutex> guard(lock);  // RAII version
        value++;
    }
}

// ---- FIX 2: std::atomic (no mutex needed!) ----
namespace fix_atomic
{
    std::atomic<int> value = 0;

    void increment() {
        value++;  // single hardware instruction, inherently atomic
    }
    // FASTEST option for single-variable increments
    // No mutex overhead
}

// ---- FIX 3: Thread-local storage (no sharing!) ----
namespace fix_thread_local
{
    thread_local int local_count = 0;  // each thread has its OWN copy
    std::atomic<int> global_total = 0;

    void increment() {
        local_count++;  // no race: each thread owns its copy
    }

    void finalize(int n) {
        // after all threads done:
        // for (auto& t : threads) { global_total += t's local_count; }
        // but this needs extra bookkeeping...
        // Usually: each thread accumulates locally, adds to total at end
    }
}

// ---- FIX 4: Immutable data / no sharing ----
// Don't share data at all!
// Each thread works on its OWN data
// Combine results after all threads join
namespace fix_no_sharing
{
    void countInRange(int start, int end, int& result) {
        int local = 0;
        for (int i = start; i < end; i++) local++;
        result = local;  // write to OWN result slot, not shared!
    }
}

// ============================================================
// SECTION 4: DETECTING DATA RACES - TOOLS
// ============================================================

// TOOL 1: ThreadSanitizer (TSan) - Google's race detector
// -> compile with: g++ -fsanitize=thread -g file.cpp
// -> detects races at RUNTIME with exact line numbers
// -> overhead: 5-15x slower (for testing only!)
//
// WITHOUT fix (data race):
// ThreadSanitizer reports:
// "WARNING: ThreadSanitizer: data race (pid=12345)
//   Write of size 4 at 0x... by thread T2:
//     #0 shared_value_increment main.cpp:15
//   Previous write of size 4 at 0x... by thread T1:
//     #0 shared_value_increment main.cpp:15"
//
// WITH mutex fix:
// ThreadSanitizer: no races detected ✅
//
// TOOL 2: Helgrind (Valgrind)
// valgrind --tool=helgrind ./program
//
// TOOL 3: AddressSanitizer (ASan) - for memory corruption
// g++ -fsanitize=address -g file.cpp
//
// IN YOUR CMAKE (for ISSST testing):
// target_compile_options(issst_test PRIVATE -fsanitize=thread -g)
// target_link_options(issst_test PRIVATE -fsanitize=thread)

// ============================================================
// SECTION 5: PERFORMANCE COMPARISON - 10000 THREADS
// ============================================================

void perfComparison()
{
    const int N = 10000;

    // MUTEX VERSION (your code):
    {
        std::mutex m;
        int value = 0;
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> threads;
        threads.reserve(N);
        for (int i = 0; i < N; i++) {
            threads.emplace_back([&]() {
                std::lock_guard<std::mutex> lock(m);
                value++;
            });
        }
        for (auto& t : threads) t.join();

        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>
                  (end - start).count();
        std::cout << "Mutex: " << value << " in " << ms << "ms\n";
    }

    // ATOMIC VERSION:
    {
        std::atomic<int> value = 0;
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> threads;
        threads.reserve(N);
        for (int i = 0; i < N; i++) {
            threads.emplace_back([&]() { value++; });
        }
        for (auto& t : threads) t.join();

        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>
                  (end - start).count();
        std::cout << "Atomic: " << value << " in " << ms << "ms\n";
    }
    // Atomic is typically 2-10x faster than mutex for this case
}

// ============================================================
// SECTION 6: COMMON DATA RACE PATTERNS IN REAL CODE
// ============================================================

// PATTERN 1: Check-Then-Act (TOCTOU race)
// BUGGY:
// if (map.find(key) == map.end()) {  // CHECK
//     map[key] = value;              // ACT  ← another thread inserted between!
// }
// FIX: hold lock across both check AND act

// PATTERN 2: Double-Checked Locking (classic bug)
// BUGGY:
// if (!initialized) {           // check without lock
//     lock();
//     if (!initialized) {       // check with lock
//         initialize();
//         initialized = true;
//     }
//     unlock();
// }
// FIX: use std::atomic<bool> initialized or std::call_once

// PATTERN 3: Iterator invalidation across threads
// BUGGY:
// // Thread A: iterating over vector
// for (auto& e : vec) { process(e); }
// // Thread B: push_back to same vector (causes reallocation!)
// vec.push_back(newItem);
// FIX: mutex around ALL vector accesses

// PATTERN 4: Compound operations
// BUGGY:
// if (queue.size() > 0) {  // check
//     auto item = queue.front();  // another thread might pop() here!
//     queue.pop();
// }
// FIX: hold lock across the entire check+pop

// ============================================================
// SECTION 7: HAPPENS-BEFORE AND MEMORY VISIBILITY
// ============================================================

// WHY do we need synchronization beyond just "correctness"?
// Modern CPUs and compilers REORDER instructions for performance!
//
// WITHOUT synchronization:
// Thread A writes: result = 42; notified = true;
// Thread B reads:  while(!notified); cout << result;
//
// CPU might reorder A's writes so Thread B sees notified=true
// but result still has old value! (memory visibility problem)
//
// SOLUTION: mutex, atomic, condition_variable ALL provide
// HAPPENS-BEFORE guarantees:
// "everything Thread A did before unlock() is visible to
//  Thread B after lock()"
//
// YOUR CODE:
// gLock.lock()    → establishes happens-before
// shared_value++  → safe (happens within the lock)
// gLock.unlock()  → makes changes visible to next thread that locks

// ============================================================
// SECTION 8: ISSST/DIS REAL DATA RACES TO WATCH FOR
// ============================================================

// YOUR SIMULATION likely has these potential races:
//
// RACE 1: Entity state map
// Network thread: entities[id].position = newPos;  // WRITE
// Render thread:  auto pos = entities[id].position; // READ
// FIX: std::shared_mutex (readers can share, writer exclusive)
//
// RACE 2: PDU send counter
// Multiple threads sending PDUs:
// pdusSent++;  // race if multiple threads send simultaneously
// FIX: std::atomic<int> pdusSent;
//
// RACE 3: Exercise state
// Network thread: exerciseRunning = true;
// Sim thread:     if (exerciseRunning) { ... }
// FIX: std::atomic<bool> exerciseRunning;
//
// RACE 4: Shared PDU buffer
// Network thread: buffer.push(pdu);
// Sim thread:     auto pdu = buffer.front(); buffer.pop();
// FIX: mutex + condition_variable (producer-consumer pattern)

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a data race?
//     -> Two or more threads access the same memory, at least one writes,
//        without synchronization. Result is UNDEFINED BEHAVIOR.
//        Can cause: wrong results, crashes, memory corruption.

// Q2: Without mutex, what would shared_value be after 10000 threads?
//     -> Undefined, but typically LESS than 10000.
//        Lost increments from simultaneous read-modify-write.
//        Non-deterministic: different result each run.
//        With mutex: ALWAYS exactly 10000.

// Q3: What is the difference between a data race and a race condition?
//     -> Data race: SPECIFIC undefined behavior (concurrent unsync access).
//        Race condition: LOGICAL bug where outcome depends on timing.
//        All data races are race conditions.
//        Not all race conditions are data races (could be with sync but wrong logic).

// Q4: What tool detects data races automatically?
//     -> ThreadSanitizer (TSan): g++ -fsanitize=thread
//        Reports exact file/line of race at runtime.
//        5-15x overhead (testing only). Also: Helgrind (Valgrind).

// Q5: When would you use atomic instead of mutex for this code?
//     -> std::atomic<int> is faster and simpler for single-variable
//        integer operations (++, +=, etc.).
//        Use mutex when: multiple variables need atomic update together,
//        or protecting complex data structures.

// Q6: Is your code with mutex still a data race?
//     -> NO. Mutex provides proper synchronization.
//        lock() establishes happens-before relationship.
//        All writes inside critical section are visible after unlock().
//        ThreadSanitizer would report no races.

// Q7: What is a happens-before relationship?
//     -> Guarantee that operations in thread A are VISIBLE to thread B.
//        mutex unlock() in A happens-before lock() in B.
//        Without this: CPU/compiler reordering can hide writes.
//        std::mutex, std::atomic, std::condition_variable all provide it.

// Q8: What is the most common data race in production code?
//     -> The "check-then-act" race (TOCTOU):
//        if (condition) { /* another thread changes condition here! */ action(); }
//        Fix: hold the lock across BOTH the check and the action.

int main()
{
    std::vector<std::thread> threads;
    threads.reserve(10000);

    for (int i = 0; i < 10000; i++) {
        threads.push_back(std::thread(shared_value_increment));
    }

    for (int i = 0; i < 10000; i++) {
        threads[i].join();
    }

    std::cout << "Shared value: " << shared_value << '\n';
    // WITH mutex:    ALWAYS 10000 ✅
    // WITHOUT mutex: anything < 10000 ❌ (non-deterministic)

    return 0;
}
// ```

// **The race condition visualized:**

// ```
// WITHOUT MUTEX (10 threads each incrementing once, expected=10):

// Thread 1: READ=0  ADD=1  WRITE=1
// Thread 2: READ=0           ADD=1  WRITE=1  ← reads BEFORE T1 writes!
// Thread 3:   READ=0                   ADD=1  WRITE=1 ← same!
// Thread 4:     READ=1  ADD=2  WRITE=2  ← lucky, caught T1's write
// ...
// Result: 7 (not 10!) - 3 increments LOST

// WITH MUTEX (serialized):
// Thread 1: [lock → READ=0 → WRITE=1 → unlock]
// Thread 2:                               [lock → READ=1 → WRITE=2 → unlock]
// Thread 3:                                                             [lock → READ=2 → WRITE=3 → unlock]
// Result: always 10 ✅ (every increment counted)
// ```

// **The four fixes compared:**

// ```cpp
// // UNSAFE (race condition):
// static int v = 0;
// void inc() { v++; }                    // 3 instructions, interruptible

// // MUTEX (safe, serialized):
// std::mutex m; int v = 0;
// void inc() { std::lock_guard l(m); v++; }  // one at a time

// // ATOMIC (safe, lock-free, FASTEST for this):
// std::atomic<int> v = 0;
// void inc() { v++; }                    // 1 hardware instruction

// // THREAD-LOCAL (no sharing, no sync needed):
// thread_local int v = 0;
// void inc() { v++; }                    // each thread has own copy
// ```

// > **To detect before it ships**: always compile with `-fsanitize=thread` in your test builds. ThreadSanitizer catches races that human review misses — including subtle ones like the check-then-act pattern. For your ISSST simulation, the entity state map is the highest-risk shared resource: if network and render threads both access it without synchronization, TSan will catch it immediately.