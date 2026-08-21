// ============================================================
// CONCURRENCY AND PARALLELISM - INTRODUCTION NOTES
// ============================================================

// ============================================================
// SECTION 1: THE KEY DEFINITIONS
// ============================================================

// YOUR COMMENT: "performance is the currency of computing"
// -> you TRADE performance for: correctness, simplicity,
//    portability, maintainability
// -> concurrency/parallelism = spending hardware resources
//    to BUY better performance

// CONCURRENCY:
// "multiple things CAN happen at once, order matters,
//  tasks sometimes wait on shared resources"
// -> about DEALING WITH multiple things at once
// -> does NOT require multiple cores
// -> tasks may interleave on a SINGLE core
// -> the STRUCTURE of the program handles multiple concerns
//
// PARALLELISM:
// "everything happens at once, instantaneously"
// -> about DOING multiple things at the same time
// -> REQUIRES multiple cores/processors
// -> subset of concurrency (parallel is always concurrent,
//    concurrent is NOT always parallel)

// YOUR COFFEE MACHINE ANALOGY (perfect!):
// CONCURRENT: one coffee machine, two queues
//   -> machine serves one person at a time
//   -> but both queues are making PROGRESS (interleaved)
//   -> machine switches between queues
//
// PARALLEL: two coffee machines, two queues
//   -> BOTH machines running SIMULTANEOUSLY
//   -> truly at the same time, no switching
//
// KEY INSIGHT: "The world is concurrent"
// -> real world = multiple things happening, sharing resources
// -> software must model this reality

// ============================================================
// SECTION 2: WHY CONCURRENCY IS NECESSARY
// ============================================================

// REAL WORLD EXAMPLES (your comments):
// -> Orchestra: instruments play simultaneously, coordinated
// -> Subway transit: multiple trains, shared tracks, scheduling
// -> Traffic stop: cars wait, take turns, shared intersection
//
// COMPUTER SCIENCE EXAMPLES (your comments):
// -> Memory allocator: multiple threads allocating simultaneously
// -> File I/O: waiting for disk while processing other work
// -> Network requests: awaiting data over slow network
//
// WITHOUT CONCURRENCY:
// while (waiting_for_network_response) {
//     // BLOCKED - nothing else can happen
//     // CPU sits IDLE waiting
//     // single thread = single point of failure for responsiveness
// }
//
// WITH CONCURRENCY:
// // Thread 1: send request, wait for response
// // Thread 2: process UI, respond to user input
// // Thread 3: process already-received data
// // -> application stays responsive while waiting

// ============================================================
// SECTION 3: DOES OUR HARDWARE SUPPORT CONCURRENCY?
// ============================================================

// YOUR QUESTION: "Does our hardware support concurrency?"
//
// SINGLE CORE CPU:
// -> ONE set of registers, ONE execution unit
// -> can only execute ONE instruction at a time
// -> CONCURRENCY: OS switches rapidly between tasks (time-slicing)
//   -> illusion of simultaneous execution
//   -> actually: task1 runs 10ms, pauses, task2 runs 10ms, etc.
//   -> fast enough switching = feels concurrent to humans
// -> PARALLELISM: NOT possible on single core
//   -> one core = one thing at a time (no matter how fast)
//
// MULTI-CORE CPU (modern standard):
// -> 4, 8, 16, 32+ independent execution cores
// -> each core has its own registers, execution pipeline
// -> CONCURRENCY: YES (each core can handle different tasks)
// -> PARALLELISM: YES (cores truly run SIMULTANEOUSLY)
//
// CHECKING IN C++:
// #include <thread>
// unsigned int cores = std::thread::hardware_concurrency();
// // returns: number of concurrent threads supported
// // = number of logical cores (including hyperthreading)
// // returns 0 if not computable
//
// ON YOUR MACHINE (O2I Linux system):
// // cat /proc/cpuinfo | grep "processor" | wc -l
// // or: nproc
// // typical modern desktop: 4-16 cores

// ============================================================
// SECTION 4: CONCURRENCY vs PARALLELISM - PRACTICAL DIFFERENCE
// ============================================================

// CONCURRENCY (about structure):
// -> Program DESIGNED to handle multiple tasks
// -> May or may not use multiple cores
// -> Example: GUI app handles user input WHILE loading a file
//   -> if single core: switches between them rapidly (concurrent)
//   -> if multi-core: can run truly simultaneously (parallel)
//
// PARALLELISM (about execution):
// -> Tasks ACTUALLY execute at the same time
// -> REQUIRES multiple cores
// -> Example: ray tracer splits image into 8 regions
//   -> 8 threads, each rendering their region simultaneously
//   -> needs 8 cores for true parallelism

// INTERLEAVING vs OVERLAPPING:
//
// INTERLEAVING (single core, concurrent):
// Time:  0ms  10ms  20ms  30ms  40ms
// Task A: ████      ████      ████
// Task B:      ████      ████
// -> A and B alternate (time-sliced by OS)
//
// OVERLAPPING (multi-core, parallel):
// Time:  0ms         40ms
// Core1: ████████████████  (Task A runs fully)
// Core2: ████████████████  (Task B runs fully, same time!)
// -> A and B run at the EXACT same time

// ============================================================
// SECTION 5: WHY PERFORMANCE IMPROVES WITH CONCURRENCY
// ============================================================

// AMDAHL'S LAW:
// Speedup = 1 / (S + (1-S)/N)
// S = serial fraction of program (can't be parallelized)
// N = number of cores/processors
//
// EXAMPLE:
// Program: 20% serial code, 80% parallelizable
// 1 core:  1x speed
// 2 cores: 1/(0.2 + 0.8/2) = 1/0.6 = 1.67x
// 4 cores: 1/(0.2 + 0.8/4) = 1/0.4 = 2.5x
// 8 cores: 1/(0.2 + 0.8/8) = 1/0.3 = 3.33x
// ∞ cores: 1/0.2 = 5x MAX (serial bottleneck limits speedup!)
//
// KEY INSIGHT: maximize parallelizable code, minimize serial code
// In practice: synchronization, coordination, I/O wait are often serial

// ============================================================
// SECTION 6: TYPES OF CONCURRENCY IN C++
// ============================================================

// 1. MULTI-THREADING (std::thread, C++11)
// -> Multiple threads of execution in ONE process
// -> Share same memory space (easy data sharing, hard to get right)
// -> Most common form of concurrency in C++
// -> YOUR upcoming sessions will cover this!
//
// 2. ASYNC TASKS (std::async, C++11)
// -> Higher-level than threads
// -> "run this function asynchronously, give me a future result"
// -> std::future to retrieve result later
//
// 3. COROUTINES (C++20)
// -> Cooperative multitasking (tasks yield control voluntarily)
// -> Lightweight, no OS scheduling overhead
// -> Good for: networking, I/O, generators
//
// 4. PARALLEL ALGORITHMS (C++17)
// -> STL algorithms with execution policy:
// -> std::execution::par -> run in parallel
// -> std::sort(std::execution::par, v.begin(), v.end());
// -> simplest way to add parallelism!

// ============================================================
// SECTION 7: THE PROBLEMS CONCURRENCY INTRODUCES
// ============================================================

// RACE CONDITION:
// Two threads access same data, at least one writes
// Result depends on who goes first (undefined behavior!)
// int x = 0;
// Thread A: x++  (read 0, write 1)
// Thread B: x++  (read 0, write 1) <- should be 2!
// Result: x = 1 (one increment LOST!)
//
// DEADLOCK:
// Thread A holds Lock1, wants Lock2
// Thread B holds Lock2, wants Lock1
// -> both wait forever, neither can proceed
//
// STARVATION:
// Some threads never get CPU time
// High-priority threads monopolize resources
//
// THESE PROBLEMS require:
// -> Mutexes (mutual exclusion locks)
// -> Atomic operations
// -> Condition variables
// -> Careful design

// ============================================================
// SECTION 8: ISSST/DIS CONCURRENCY RELEVANCE
// ============================================================

// In your DRDO simulation work, concurrency matters for:
//
// 1. PDU RECEPTION (network thread):
//    -> Dedicated thread receives UDP packets continuously
//    -> Decodes DIS PDUs as they arrive
//    -> Pushes to shared entity state map
//
// 2. SIMULATION TICK (main/sim thread):
//    -> Reads entity states to update simulation
//    -> Must NOT block on network I/O
//    -> Must be deterministic (consistent frame timing)
//
// 3. RENDERING / TDF DISPLAY (UI thread):
//    -> Qt GUI must stay responsive
//    -> Cannot block on network or simulation
//    -> Must read entity states safely
//
// SHARED RESOURCE (the problem):
// std::unordered_map<EntityID, EntityState> entityStates;
// -> network thread WRITES to it
// -> sim thread READS from it
// -> UI thread READS from it
// -> WITHOUT synchronization: race condition = crash/corruption!
//
// SOLUTION (preview of upcoming sessions):
// std::mutex entityMutex;
// // network thread:
// { std::lock_guard<std::mutex> lock(entityMutex);
//   entityStates[id] = newState; }
// // sim thread:
// { std::lock_guard<std::mutex> lock(entityMutex);
//   auto state = entityStates[id]; }

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between concurrency and parallelism?
//     -> Concurrency: DEALING with multiple things at once (structure).
//        Can happen on single core (time-slicing).
//        Parallelism: DOING multiple things at the same time (execution).
//        Requires multiple cores. Parallelism IS concurrent,
//        concurrency is NOT necessarily parallel.

// Q2: Can you have concurrency without parallelism?
//     -> YES. Single-core CPU with OS time-slicing.
//        Tasks interleave rapidly (concurrent structure)
//        but only one runs at any moment (no parallelism).

// Q3: What is a race condition?
//     -> When two or more threads access shared data without
//        synchronization and at least one modifies it.
//        Result depends on execution order = undefined behavior.
//        Fix: mutex, atomic operations, lock-free data structures.

// Q4: How does std::thread::hardware_concurrency() help?
//     -> Returns number of logical cores available.
//        Use to decide thread pool size.
//        Creating more threads than cores = overhead from switching.
//        Optimal: threads ≈ hardware_concurrency() for CPU-bound work.

// Q5: What is Amdahl's Law?
//     -> Maximum speedup = 1 / (S + (1-S)/N)
//        S = serial fraction, N = cores.
//        Even with infinite cores, serial code limits speedup.
//        With 20% serial: max speedup = 5x regardless of cores.

// Q6: What are the main problems introduced by concurrency?
//     -> Race conditions (unsynchronized shared data access).
//        Deadlock (circular wait for locks).
//        Livelock (threads respond to each other, no progress).
//        Starvation (thread never gets CPU time).

// Q7: What C++ features enable concurrency?
//     -> std::thread (C++11): low-level thread management.
//        std::async/future (C++11): task-based parallelism.
//        std::mutex, atomic (C++11): synchronization primitives.
//        std::execution::par (C++17): parallel STL algorithms.
//        Coroutines (C++20): cooperative concurrency.

// Q8: In a DIS simulation, what should run in separate threads?
//     -> Network I/O thread: receive/send UDP packets.
//        Simulation thread: update entity states, physics.
//        UI/render thread: update display, handle user input.
//        Each thread has a clear responsibility.
//        Shared data (entity map) protected by mutex.
// ```

// **Concurrency vs Parallelism - the clearest picture:**

// ```
// CONCURRENT (1 core):            PARALLEL (multi-core):
// Core 1: AAABBBAAABBB            Core 1: AAAAAAAAAAAA
//                                 Core 2: BBBBBBBBBBBB

// Tasks INTERLEAVE rapidly        Tasks run SIMULTANEOUSLY
// "dealing with many things"      "doing many things"
// Structure (design)              Execution (hardware)
// ```

// **Coffee machine analogy:**

// ```
// CONCURRENT: 1 machine, 2 queues     PARALLEL: 2 machines, 2 queues
// [☕machine]                          [☕machine1] [☕machine2]
//  Queue A: 👤👤👤                    Queue A: 👤👤👤  Queue B: 👤👤👤
//  Queue B: 👤👤👤
// Makes one coffee at a time,         Both machines run SIMULTANEOUSLY
// switches between queues             Queue A and B served in parallel
// ```

// > **The foundation**: everything in concurrent C++ programming — threads, mutexes, atomics, futures — exists to solve one fundamental problem: **multiple things want to happen at once, but some resources can only be used by one thing at a time**. Your ISSST system already IS concurrent (network, simulation, UI all need to run), the question is whether it's structured to handle that correctly.