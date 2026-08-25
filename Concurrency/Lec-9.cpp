#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <chrono>

// ============================================================
// std::atomic - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS std::atomic?
// ============================================================

// std::atomic<T> = a type whose operations are GUARANTEED to be
// indivisible (atomic = cannot be interrupted mid-operation)
//
// PROBLEM WITH REGULAR int (race condition):
// shared_value += 1 compiles to THREE CPU instructions:
// 1. LOAD  shared_value from memory to register
// 2. ADD   register + 1
// 3. STORE register back to memory
//
// If Thread A is interrupted between step 1 and 3:
// Thread A: LOAD (gets 5)
// Thread B: LOAD (gets 5) ← reads SAME value!
// Thread A: ADD + STORE → 6
// Thread B: ADD + STORE → 6 ← should be 7! ONE INCREMENT LOST!
//
// SOLUTION WITH atomic:
// std::atomic<int> shared_value = 0;
// shared_value += 1  → ONE uninterruptible CPU instruction (LOCK ADD)
// No thread can interrupt between load and store
// RESULT: always correct, no lost increments
//
// YOUR CODE:
// static std::atomic<int> shared_value = 0;
// 1000 threads each += 1 -> always prints 1000 (deterministic!)

// ============================================================
// SECTION 2: atomic vs mutex - WHEN TO USE WHICH
// ============================================================

// MUTEX:
// -> protects COMPLEX operations (multiple statements)
// -> protects ANY shared data structure (vector, map, etc.)
// -> serializes access (one thread at a time)
// -> higher overhead (OS involvement, cache effects)
//
// ATOMIC:
// -> protects SINGLE VARIABLE operations
// -> only works on: int, float, bool, pointer types
// -> lock-free (no OS calls, hardware instruction)
// -> LOWER overhead than mutex
// -> USE: counters, flags, simple values
//
// YOUR CODE COMPARISON:
// WITH mutex (previous session):
// std::mutex glock;
// void inc() { std::lock_guard<std::mutex> lock(glock); shared_value++; }
// -> full mutex overhead, serializes threads
//
// WITH atomic (your code):
// std::atomic<int> shared_value = 0;
// void inc() { shared_value += 1; }
// -> no mutex needed! hardware handles it
// -> faster, simpler code

// ============================================================
// SECTION 3: YOUR CODE EXPLAINED
// ============================================================

static std::atomic<int> shared_value = 0;
// atomic<int>: all operations on this variable are atomic
// = thread-safe by design, no mutex needed
// supports: +=, -=, ++, --, load(), store(), exchange(), etc.

void share_value_increment()
{
    shared_value += 1;
    // ATOMIC fetch_add operation:
    // ONE hardware instruction: LOCK XADD (x86)
    // -> reads current value
    // -> adds 1
    // -> writes back
    // ALL IN ONE UNINTERRUPTIBLE OPERATION
    // No thread can see an intermediate state
    //
    // EQUIVALENT WAYS TO WRITE THIS:
    // shared_value++;           // post-increment (atomic)
    // ++shared_value;           // pre-increment  (atomic)
    // shared_value.fetch_add(1); // explicit fetch_add
    // shared_value += 1;         // YOUR CODE
    // ALL are equivalent and thread-safe
}

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
    // ALWAYS prints: 1000 (deterministic!)
    // 1000 threads * 1 atomic increment = exactly 1000

    return 0;
}

// ============================================================
// SECTION 4: ALL ATOMIC OPERATIONS
// ============================================================

void atomicOperations()
{
    std::atomic<int> a = 0;

    // STORE (write):
    a.store(42);           // a = 42 (atomic write)
    a = 42;                // same (assignment operator)

    // LOAD (read):
    int val = a.load();    // val = 42 (atomic read)
    int val2 = a;          // same (implicit conversion)

    // INCREMENT / DECREMENT:
    a++;                   // post-increment (atomic)
    ++a;                   // pre-increment  (atomic)
    a--;                   // post-decrement (atomic)
    --a;                   // pre-decrement  (atomic)

    // COMPOUND ASSIGNMENT:
    a += 5;                // atomic add
    a -= 3;                // atomic subtract
    a &= 0xFF;             // atomic AND
    a |= 0x01;             // atomic OR
    a ^= 0x10;             // atomic XOR

    // FETCH AND MODIFY (returns OLD value):
    int old = a.fetch_add(1); // reads old value, adds 1, returns old
    int old2 = a.fetch_sub(1); // reads old, subtracts 1, returns old
    // useful when you need BOTH old and new value

    // EXCHANGE (swap value):
    int previous = a.exchange(100); // sets a=100, returns old value

    // COMPARE AND EXCHANGE (CAS - compare and swap):
    int expected = 100;
    bool swapped = a.compare_exchange_strong(expected, 200);
    // IF a == expected: set a = 200, return true
    // IF a != expected: set expected = a (update expected), return false
    // THE fundamental building block of lock-free algorithms!

    // IS LOCK FREE? (does not use mutex internally):
    bool lockFree = a.is_lock_free();
    // true for most int/pointer types on modern CPUs
    // false for large types (struct with 128 bytes = uses mutex internally)
}

// ============================================================
// SECTION 5: MEMORY ORDER - ADVANCED (interview topic)
// ============================================================

// std::atomic operations have a MEMORY ORDER parameter
// controls: how operations are seen by OTHER cores/threads
//
// DEFAULT: memory_order_seq_cst (sequential consistency)
// -> strongest guarantee, correct behavior, slight overhead
// -> USE: when you're not sure (your code uses this implicitly)
//
// OTHER ORDERS (for performance tuning):
// memory_order_relaxed -> no ordering guarantees, just atomicity
//                         USE: simple counters (your use case!)
// memory_order_acquire -> after this, later reads see all prior writes
// memory_order_release -> before this, earlier writes visible to acquire
// memory_order_acq_rel -> both acquire and release
//
// YOUR CODE with explicit relaxed order (slightly faster counter):
// shared_value.fetch_add(1, std::memory_order_relaxed);
// -> SAFE for your use case (just counting, no ordering needed)
// -> tells CPU: don't reorder memory operations around this
// -> default (seq_cst) is safe but has more overhead

// ============================================================
// SECTION 6: ATOMIC BOOL - COMMON FLAG PATTERN
// ============================================================

std::atomic<bool> g_running = true;

void workerThread()
{
    while (g_running.load())  // atomic read of flag
    {
        // do work...
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "Worker thread stopping\n";
}

void atomicFlagExample()
{
    std::thread worker(workerThread);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    g_running.store(false);  // atomic write: signal thread to stop
    // without atomic: this write might not be visible to worker thread!
    // (CPU caching, compiler optimization might hide the write)

    worker.join();
}

// ============================================================
// SECTION 7: WHAT atomic CANNOT PROTECT
// ============================================================

// atomic protects SINGLE VARIABLE OPERATIONS
// It does NOT protect COMPOUND OPERATIONS across multiple atomics!

std::atomic<int> x = 0;
std::atomic<int> y = 0;

void compound_bad()
{
    // RACE: not atomic together even though each is atomic separately!
    x = x + 1;   // atomic read + atomic write, but NOT together!
    y = y + 1;   // another thread might see x incremented but not y
}
// If you need x and y to be updated atomically TOGETHER:
// -> use a mutex to protect BOTH operations
// -> or redesign data structure

// Also, atomic cannot protect:
// -> STL containers (vector, map, etc.)
// -> Reading and writing a struct's multiple fields
// -> Compound conditions: if (a == 5 && b == 10) { }
//    (another thread might change b between the two checks)

// ============================================================
// SECTION 8: PERFORMANCE COMPARISON
// ============================================================

// BENCHMARK RESULTS (approximate, varies by hardware):
//
// Operation               | Time per op (single thread)
// ------------------------|-----------------------------
// regular int ++          | 0.3 ns  (fastest, no sync)
// atomic<int> ++          | 5-20 ns (hardware atomic instruction)
// mutex lock_guard + ++   | 20-100 ns (OS involvement possible)
//
// For your 1000 threads case:
// atomic: no blocking, threads run in parallel, hardware resolves conflicts
// mutex:  threads SERIALIZE (one at a time through critical section)
//
// atomic is FASTER for single-variable operations
// mutex is NECESSARY for complex/multi-variable operations

// ============================================================
// SECTION 9: ATOMIC POINTER (useful pattern)
// ============================================================

// std::atomic<T*> - atomic pointer operations
// Useful for: lock-free data structures, message passing

struct Node { int value; Node* next; };

std::atomic<Node*> g_head = nullptr;

void pushFrontLockFree(int value)
{
    Node* newNode = new Node{value, nullptr};
    Node* oldHead;

    do {
        oldHead = g_head.load();       // read current head
        newNode->next = oldHead;       // point new node to current head
    } while (!g_head.compare_exchange_weak(oldHead, newNode));
    // CAS: if head is still oldHead, set head = newNode
    // if head changed (another thread inserted), retry the loop
    // THIS IS LOCK-FREE PUSH_FRONT!
}

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::atomic and why does it solve the race condition?
//     -> std::atomic wraps a type and ensures all operations on it
//        are indivisible (atomic). The CPU executes them as a single
//        hardware instruction (LOCK prefix on x86) that cannot be
//        interrupted between read and write.

// Q2: What is the difference between atomic and mutex?
//     -> atomic: lock-free, hardware instruction, for SINGLE variables,
//               lower overhead, works on: int, bool, pointer.
//        mutex:  OS-based lock, for COMPLEX operations/structures,
//               higher overhead, works on: ANY shared data.
//        Use atomic for counters/flags, mutex for complex data.

// Q3: What is fetch_add and how does it differ from +=?
//     -> Both atomically add to the value.
//        fetch_add(n) RETURNS the value BEFORE the addition (old value).
//        += modifies in place, no return of old value.
//        fetch_add useful when you need both old and new values.

// Q4: What is compare_exchange_strong (CAS)?
//     -> Compare-And-Swap: atomic conditional update.
//        IF atomic == expected: set to desired, return true.
//        IF atomic != expected: update expected to actual, return false.
//        Foundation of all lock-free algorithms and data structures.

// Q5: Does atomic guarantee correctness for all multi-variable operations?
//     -> NO. atomic protects individual variables only.
//        If you need to update x and y atomically TOGETHER:
//        use a mutex. Two atomic updates are not one atomic operation.

// Q6: What does is_lock_free() return?
//     -> true if the atomic implementation uses hardware instructions
//        (LOCK prefix) rather than a mutex internally.
//        Usually true for int, bool, pointer types on modern CPUs.
//        Large structs might return false (uses mutex internally).

// Q7: What is memory_order_relaxed and when to use it?
//     -> Weakest memory ordering: only atomicity guaranteed, no
//        ordering relative to other operations.
//        Use for simple counters where you don't care about ordering:
//        shared_value.fetch_add(1, memory_order_relaxed);
//        Slightly faster than default seq_cst.

// Q8: When would you choose atomic<bool> over a regular bool?
//     -> When multiple threads read/write the bool (stop flag pattern).
//        Regular bool: compiler/CPU can cache it in register,
//        other threads might not see the update (optimization issue).
//        atomic<bool>: guarantees visibility across all threads.
//        Simpler and faster than mutex for a single flag.
// ```

// **atomic vs mutex - when to use which:**

// ```
// Single variable (counter, flag, pointer)?
// → std::atomic<int>  O(1) hardware instruction, lock-free

// Multiple variables or complex structure?
// → std::mutex + lock_guard  serialize access, correct ordering

// Both simple AND complex?
// → atomic for counters + mutex for the complex structure
// ```

// **The hardware difference:**

// ```
// Regular int ++:          atomic<int> ++:
// READ  value              LOCK XADD instruction
// ADD   1                  (single uninterruptible CPU operation)
// WRITE value              ← thread CANNOT interrupt between read and write

// Thread can interrupt      Thread CANNOT interrupt anywhere
// between any step!         = always correct result
// = RACE CONDITION          = NO race condition
// ```

// **The three alternatives compared:**

// ```cpp
// // 1. UNSAFE (race condition):
// static int x = 0;
// void inc() { x++; }           // 3 instructions, can be interrupted

// // 2. MUTEX (safe, slower):
// std::mutex m;
// void inc() {
//     std::lock_guard<std::mutex> lock(m);
//     x++;                       // serialized, one thread at a time
// }

// // 3. ATOMIC (safe, faster):
// std::atomic<int> x = 0;
// void inc() { x++; }           // 1 hardware instruction, lock-free
// ```

// > **The rule of thumb**: reach for `std::atomic` first when protecting a **single variable** (counter, flag, version number). Only escalate to `mutex` when you need to protect **multiple variables together** or complex data structures. For your ISSST work: `atomic<bool> g_running` for thread control flags, `mutex` for the entity state map.