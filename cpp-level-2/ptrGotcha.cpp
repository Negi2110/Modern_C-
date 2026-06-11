
// ============================================================
// RETURNING POINTERS - WHEN SAFE, WHEN CORRUPTED
// ============================================================

#include <iostream>

// ============================================================
// CASE 1: RETURNING POINTER TO LOCAL VARIABLE -> CORRUPTED!
// ============================================================

int* returnLocal()
{
    int result = 42;       // lives on STACK (local variable)
    return &result;        // returning address of stack variable
}                          // function ends -> stack frame DESTROYED
                           // result no longer exists
                           // returned pointer = DANGLING POINTER

// int* px = returnLocal();
// *px = ???   // UNDEFINED BEHAVIOR
//             // stack memory was reclaimed
//             // could be garbage, could be next function's data
//             // could crash, could silently corrupt

// MEMORY VISUAL:
//
// DURING call:              AFTER return:
// STACK:                    STACK:
// | result = 42 | 0x61ff14  | ???????? | 0x61ff14  <- reused!
//       ^                         ^
//       px points here            px STILL points here but INVALID

// ============================================================
// CASE 2: RETURNING POINTER TO HEAP -> SAFE!
// ============================================================

int* returnHeap()
{
    int* result = new int(42);  // lives on HEAP
    return result;              // returning heap address
}                               // function ends -> stack frame destroyed
                                // BUT heap memory SURVIVES!
                                // heap lives until YOU delete it

// int* px = returnHeap();
// *px = 99;    // SAFE - heap memory still valid
// delete px;   // caller must free it!

// MEMORY VISUAL:
//
// DURING call:                    AFTER return:
// STACK:           HEAP:          STACK:      HEAP:
// |result=0x500|-> [42] 0x500     |empty|     [42] 0x500 <- STILL HERE!
//                                                ^
//                                               px points here, VALID

// ============================================================
// CASE 3: RETURNING POINTER TO STATIC VARIABLE -> SAFE!
// ============================================================

int* returnStatic()
{
    static int result = 42;  // lives in DATA SEGMENT (not stack)
    return &result;          // safe! static survives entire program
}                            // function ends -> stack frame gone
                             // BUT static variable SURVIVES

// int* px = returnStatic();
// *px = 99;    // SAFE - static memory still valid
// NO delete needed (not heap)

// ============================================================
// CASE 4: RETURNING POINTER TO GLOBAL -> SAFE!
// ============================================================

int globalValue = 42;        // lives in DATA SEGMENT

int* returnGlobal()
{
    return &globalValue;     // safe! global lives entire program
}

// ============================================================
// CASE 5: RETURNING POINTER PASSED IN -> SAFE!
// ============================================================

int* returnPassedIn(int* px)
{
    *px = 42;
    return px;               // returning what caller gave you
}                            // memory belongs to CALLER, still valid

// int x = 0;
// int* px = returnPassedIn(&x);
// *px = 99;   // SAFE - x still exists in caller's scope

// ============================================================
// SECTION: SIMPLE RULE TO REMEMBER
// ============================================================

//  WHERE memory lives      |  Survives function end?  |  Safe to return?
// -------------------------|--------------------------|------------------
//  Stack (local variable)  |  NO - destroyed          |  NEVER
//  Heap  (new)             |  YES - until delete      |  YES (caller deletes)
//  Static variable         |  YES - entire program    |  YES
//  Global variable         |  YES - entire program    |  YES
//  Passed-in pointer       |  YES - caller owns it    |  YES

int main()
{
    // CASE 1: CORRUPTED - never do this
    int* bad = returnLocal();
    // *bad is undefined behavior - stack memory gone

    // CASE 2: SAFE - heap survives
    int* good = returnHeap();
    std::cout << *good << std::endl;  // prints 42 - safe!
    delete good;                       // caller must clean up

    // CASE 3: SAFE - static survives
    int* s = returnStatic();
    std::cout << *s << std::endl;     // prints 42 - safe!
    // NO delete needed

    return 0;
}
// ```

// **One line answer:**

// ```
// Stack pointer  -> CORRUPTED  (stack frame destroyed with function)
// Heap pointer   -> SAFE       (heap lives until you delete it)
// Static pointer -> SAFE       (lives entire program)
// Global pointer -> SAFE       (lives entire program)
// ```

// > This is exactly why `new` exists — when you **need memory to outlive the function that created it**, put it on the heap. The caller then owns it and is responsible for `delete`.