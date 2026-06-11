// ============================================================
// 4 DEADLY POINTER ERRORS - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: DANGLING POINTER (your commented function)
// ============================================================

// int* returnMemory()
// {
//     int result = 23;   // result lives on STACK (local variable)
//     return &result;    // returning address of stack variable!
// }                      // function ends -> stack frame DESTROYED
//                        // result no longer exists
//                        // returned pointer points to DEAD memory
//
// int* px = returnMemory();
// *px = 42;   // UNDEFINED BEHAVIOR - writing to freed stack memory
//             // may crash, may silently corrupt other data
//
// MEMORY VISUAL:
//
// DURING function call:        AFTER function returns:
// STACK:                       STACK:
// | result = 23 | (0x61ff14)   | ???????????? | (0x61ff14)
//      ^                            ^
//      px points here               px STILL points here but
//                                   memory is GONE (reused/freed)
//
// FIX: return heap memory instead
// int* returnMemory()
// {
//     int* result = new int(23);  // heap - outlives function!
//     return result;              // caller must delete!
// }

// ============================================================
// SECTION 2: ALL 4 DEADLY POINTER ERRORS
// ============================================================

int main()
{
    // ----------------------------------------------------------
    // ERROR 1: nullptr DEREFERENCE
    // ----------------------------------------------------------

    // int* px = nullptr;
    // *px = 42;
    //
    // nullptr = pointer to address 0
    // address 0 is RESERVED by OS - no program can access it
    // dereferencing = trying to read/write address 0
    // OS catches this -> SEGMENTATION FAULT -> crash
    //
    // MEMORY VISUAL:
    // px = 0x00000000  (address zero)
    // *px              (go to address 0... CRASH!)
    //
    // WHY it exists:
    // int* px;          // uninitialized - garbage address (dangerous)
    // int* px = nullptr // at least we KNOW it's invalid
    //                   // crash is immediate and clear
    //                   // better than silent corruption
    //
    // FIX: always check before dereferencing
    // if (px != nullptr) {
    //     *px = 42;      // safe
    // }
    // modern C++: if (px) { *px = 42; }  // nullptr is falsy

    // ----------------------------------------------------------
    // ERROR 2: MEMORY LEAK
    // ----------------------------------------------------------

    // int* array = new int[1000];
    // // ... use array ...
    // // forgot delete[]!
    // return 0;
    //
    // heap memory allocated but NEVER freed
    // OS cannot reclaim it while program runs
    // program holds 4000 bytes it no longer needs
    //
    // WHY dangerous:
    // -> small leak in short program = fine (OS cleans up at exit)
    // -> leak in long-running program (server, game, Qt app) = death
    //    memory usage grows and grows -> system runs out -> crash
    //
    // REAL WORLD: Qt applications, servers, embedded systems
    // ALL must be leak-free. Tools: Valgrind, AddressSanitizer
    //
    // MEMORY VISUAL:
    // STACK:                  HEAP:
    // [array = 0x...] ------> [_][_]...[_]  1000 ints
    //  array goes out                ^
    //  of scope/deleted              |
    //  pointer GONE                  still here! LEAKED
    //                                OS cannot free it
    //
    // FIX:
    // int* array = new int[1000];
    // // ... use array ...
    // delete[] array;       // FREE the memory
    // array = nullptr;      // avoid dangling pointer

    // ----------------------------------------------------------
    // ERROR 3: DANGLING POINTER (stack version)
    // ----------------------------------------------------------

    // int* px = returnMemory();  // points to dead stack memory
    // std::cout << *px;          // UNDEFINED BEHAVIOR
    //
    // already explained above in SECTION 1
    // other ways dangling pointer is created:
    //
    // WAY 1: returning address of local variable (your example)
    //
    // WAY 2: using after delete
    // int* p = new int(42);
    // delete p;
    // *p = 99;   // DANGLING - memory freed, p still has old address
    //
    // WAY 3: pointer to out-of-scope variable
    // int* p;
    // {
    //     int y = 7;
    //     p = &y;      // p points to y
    // }                // y destroyed here
    // *p = 42;         // DANGLING - y is gone
    //
    // FIX: set to nullptr after delete, never return local address
    // delete p;
    // p = nullptr;   // now clearly invalid, crash is obvious if misused

    // ----------------------------------------------------------
    // ERROR 4: DOUBLE FREE (your actual code)
    // ----------------------------------------------------------

    int* x = new int;    // allocate one int on heap
    *x = 42;             // write 42 to it

    delete x;            // FREE the memory -> heap block returned to OS
                         // x still holds old address (dangling now)

    delete x;            // DOUBLE FREE -> UNDEFINED BEHAVIOR
                         // trying to free memory that's already free
                         // heap data structures get CORRUPTED
                         // may crash immediately
                         // may cause crash much later (harder to debug!)
                         // may allow SECURITY EXPLOIT (heap exploit)
    //
    // MEMORY VISUAL:
    //
    // after new:         HEAP: [42]  <- valid, x points here
    // after delete:      HEAP: [??]  <- freed, x is now dangling
    // after delete again:      OS/heap gets confused, corruption!
    //
    // WHY it's dangerous:
    // heap manager keeps a FREE LIST of available blocks
    // double free puts same block in free list TWICE
    // two different allocations get SAME memory -> silent corruption
    // used in security exploits (double free vulnerability)
    //
    // FIX: set to nullptr after delete
    // delete x;
    // x = nullptr;
    // delete x;    // delete nullptr is SAFE (does nothing)
    //              // this is guaranteed by C++ standard

    return 0;
}

// ============================================================
// SECTION 3: nullptr DELETE IS ALWAYS SAFE
// ============================================================

// delete nullptr;    // SAFE - does nothing, guaranteed by standard
// delete[] nullptr;  // SAFE - does nothing, guaranteed by standard
//
// THIS is why setting to nullptr after delete prevents double free:
// int* p = new int(42);
// delete p;
// p = nullptr;      // p is now nullptr
// delete p;         // delete nullptr = safe, no crash!

// ============================================================
// SECTION 4: MODERN C++ SOLUTIONS - AVOID ALL 4 ERRORS
// ============================================================

// All 4 errors come from MANUAL memory management
// Modern C++ solves this with SMART POINTERS:

// #include <memory>
//
// std::unique_ptr<int> x = std::make_unique<int>(42);
// // no new, no delete needed
// // automatically deleted when x goes out of scope
// // cannot be copied (prevents double free)
// // cannot be null unless you explicitly set it
//
// SOLVES:
// nullptr deref  -> use .get() to check before access
// memory leak    -> auto-deleted on scope exit (RAII)
// dangling ptr   -> unique_ptr owns memory, clear ownership
// double free    -> cannot delete manually, auto-managed

// ============================================================
// SECTION 5: RAII - ROOT SOLUTION TO ALL POINTER ERRORS
// ============================================================

// RAII = Resource Acquisition Is Initialization
// -> acquire resource in CONSTRUCTOR
// -> release resource in DESTRUCTOR
// -> destructor ALWAYS called when object goes out of scope
//
// std::vector, std::string, std::unique_ptr ALL use RAII
// They never leak because destructor always cleans up
//
// INTERVIEW: What is RAII?
//   -> Design pattern where resource lifetime is tied to object lifetime
//      Constructor acquires, destructor releases
//      Guarantees cleanup even if exception is thrown
//      Foundation of modern C++ memory safety

// ============================================================
// SECTION 6: INTERVIEW QUESTIONS
// ============================================================

// Q1: What are the 4 common pointer errors?
//     -> 1. nullptr dereference  - access address 0 -> segfault
//        2. Memory leak          - new without delete -> RAM waste
//        3. Dangling pointer     - pointer to freed/dead memory
//        4. Double free          - delete same pointer twice -> corruption

// Q2: What is a segmentation fault?
//     -> OS signal when program accesses memory it shouldn't.
//        Common causes: nullptr deref, out of bounds, dangling pointer.
//        OS protects memory -> kills the process.

// Q3: How do you prevent double free?
//     -> Set pointer to nullptr after delete.
//        delete nullptr is safe (does nothing).
//        Better: use smart pointers (unique_ptr) - no manual delete.

// Q4: What is the difference between dangling pointer and null pointer?
//     -> Null pointer: deliberately set to nullptr, points to nothing.
//                      Safe to check (if px), crash is immediate & clear.
//        Dangling ptr: points to freed/invalid memory.
//                      Looks valid, UB when accessed, hard to debug.

// Q5: What is RAII?
//     -> Resource Acquisition Is Initialization.
//        Resource tied to object lifetime.
//        Constructor acquires, destructor releases automatically.
//        Foundation of leak-free C++ code.

// Q6: What tools detect memory errors?
//     -> Valgrind: detects leaks, dangling ptrs, double free (Linux)
//        AddressSanitizer (ASan): compiler flag -fsanitize=address
//        Dr. Memory: Windows equivalent of Valgrind

// Q7: Is delete nullptr safe?
//     -> YES. C++ standard guarantees delete nullptr does nothing.
//        Always set pointer to nullptr after delete to prevent double free.

// Q8: What is a memory leak in a Qt application?
//     -> Heap allocation without corresponding delete.
//        Qt partially solves this via parent-child ownership:
//        QObject* child = new QWidget(parent);
//        When parent is deleted, all children auto-deleted.
//        But non-QObject heap allocations still need manual management.
// ```

// **The 4 errors as a quick table - memorize this:**

// ```
// ERROR            CAUSE                      SYMPTOM              FIX
// ─────────────────────────────────────────────────────────────────────
// nullptr deref   access address 0           instant crash        check != nullptr
// memory leak     new without delete         RAM grows forever    always delete / use RAII  
// dangling ptr    pointer to dead memory     silent corruption    set nullptr after delete
// double free     delete same ptr twice      heap corruption      set nullptr after delete
// ```

// > Notice: **setting pointer to `nullptr` after `delete`** fixes both dangling pointer AND double free in one shot. It's the single most important habit for raw pointer safety.