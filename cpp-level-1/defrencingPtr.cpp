
// ============================================================
// MUTATING THROUGH POINTER - INTERVIEW NOTES
// ============================================================

#include <iostream>

int main()
{
    int x = 7;
    int* px = &x;   // px stores address of x

    // ============================================================
    // SECTION 1: WHAT *px = 42 ACTUALLY DOES
    // ============================================================

    *px = 42;
    // Step 1: look at px  -> get address (0x61ff14)
    // Step 2: go to that address in memory
    // Step 3: write 42 into that memory location
    // Step 4: that memory location IS x
    // Result: x is now 42
    //
    // *px = 42  is IDENTICAL to  x = 42
    // both write to the same memory location
    //
    // MEMORY VISUAL:
    //
    // BEFORE *px = 42:
    // address:  0x61ff14    0x61ff08
    //           [  x = 7 ]  [ px = 0x61ff14 ]
    //                ^              |
    //                |______________|
    //
    // AFTER *px = 42:
    // address:  0x61ff14    0x61ff08
    //           [ x = 42 ]  [ px = 0x61ff14 ]
    //                ^              |
    //                |______________|
    //
    // px itself did NOT change (still holds same address)
    // only the VALUE at that address changed (7 -> 42)

    std::cout << "x stores:" << x << std::endl;
    // prints: x stores: 42
    // x changed even though we never wrote x = 42 directly!
    // we changed it THROUGH the pointer

    // ============================================================
    // SECTION 2: THREE WAYS TO CHANGE x - ALL IDENTICAL RESULT
    // ============================================================

    // WAY 1: direct assignment
    // x = 42;          // straightforward

    // WAY 2: through pointer dereference
    // *px = 42;        // same result, indirect

    // WAY 3: through reference
    // int& ref = x;
    // ref = 42;        // same result, alias

    // All three write 42 to address 0x61ff14
    // All three make x == 42
    // Different syntax, identical machine code generated

    // ============================================================
    // SECTION 3: THIS IS WHY POINTERS ARE POWERFUL
    // ============================================================

    // You can change a variable WITHOUT having its name
    // As long as you have its ADDRESS, you can read and write it
    //
    // Real world uses:
    //
    // 1. Functions modifying caller's variables:
    //    void doubleIt(int* p) { *p = *p * 2; }
    //    int x = 5;
    //    doubleIt(&x);   // x is now 10
    //    (pass by pointer = indirect modification)
    //
    // 2. Dynamic memory (heap):
    //    int* p = new int(42);  // allocate on heap
    //    *p = 99;               // modify heap value through pointer
    //    delete p;              // free heap memory
    //
    // 3. Arrays (pointer arithmetic):
    //    int arr[3] = {1,2,3};
    //    int* p = arr;
    //    *(p+0) = 10;   // arr[0] = 10
    //    *(p+1) = 20;   // arr[1] = 20
    //    *(p+2) = 30;   // arr[2] = 30

    // ============================================================
    // SECTION 4: COMMON MISTAKES WITH POINTER MUTATION
    // ============================================================

    // MISTAKE 1: confusing px = 42 with *px = 42
    //
    // px = 42;    // WRONG (usually compiler error or warning)
    //             // trying to store integer 42 as an address
    //             // px is int*, not int
    //             // 42 is not a valid memory address
    //
    // *px = 42;   // CORRECT
    //             // dereference first, THEN assign value

    // MISTAKE 2: dereferencing uninitialized pointer
    //
    // int* p;     // uninitialized! contains garbage address
    // *p = 42;    // UNDEFINED BEHAVIOR - writing to random memory
    //             // likely crash (segmentation fault)
    //             // ALWAYS initialize pointers!
    //
    // int* p = nullptr;  // safe initialization
    // *p = 42;           // still crashes BUT predictably
    //                    // null pointer dereference = clear crash reason

    // MISTAKE 3: dangling pointer
    //
    // int* p;
    // {
    //     int y = 7;
    //     p = &y;       // p points to y
    // }                 // y DESTROYED here (scope ended)
    // *p = 42;          // UNDEFINED BEHAVIOR - y no longer exists
    //                   // p is now a DANGLING POINTER
    //                   // points to memory that was freed

    // ============================================================
    // SECTION 5: const POINTER VARIATIONS - REVISION
    // ============================================================

    // int* px = &x          // can change what px points to
    //                       // can change value through *px
    //                       // *px = 42 VALID, px = &y VALID

    // const int* px = &x    // pointer to const int
    //                       // CANNOT change value through pointer
    //                       // *px = 42 ERROR
    //                       // px = &y VALID (can point elsewhere)

    // int* const px = &x    // const pointer to int
    //                       // CANNOT change where pointer points
    //                       // *px = 42 VALID (can change value)
    //                       // px = &y ERROR

    // const int* const px = &x  // const pointer to const int
    //                            // CANNOT change value OR pointer
    //                            // *px = 42 ERROR
    //                            // px = &y  ERROR

    // ============================================================
    // SECTION 6: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: What does *px = 42 do?
    //     -> Goes to memory address stored in px,
    //        writes 42 at that location.
    //        Equivalent to directly assigning the pointed-to variable.

    // Q2: What is the difference between px = 42 and *px = 42?
    //     -> px = 42   : tries to store 42 as an address (wrong/error)
    //        *px = 42  : writes 42 to the memory px points to (correct)

    // Q3: What is a dangling pointer?
    //     -> A pointer that points to memory that has been freed/destroyed.
    //        Dereferencing it is undefined behavior (likely crash).
    //        Common cause: pointer to local variable after scope ends.

    // Q4: What is an uninitialized pointer?
    //     -> A pointer declared without a value.
    //        Contains garbage address. Dereferencing = undefined behavior.
    //        Always initialize: int* p = nullptr or int* p = &x.

    // Q5: What are the four const pointer combinations?
    //     -> int* p          : can change value AND address
    //        const int* p    : cannot change value, can change address
    //        int* const p    : can change value, cannot change address
    //        const int* const: cannot change either

    // Q6: How is *px = 42 different from a reference assignment?
    //     -> int& ref = x; ref = 42;  -> same end result (x = 42)
    //        But pointer needs explicit * to dereference.
    //        Reference is automatically dereferenced (transparent).
    //        Pointer can be null/reassigned, reference cannot.

    // Q7: After *px = 42, what is the value of px?
    //     -> px is UNCHANGED. Still holds &x (same address).
    //        Only the VALUE at that address changed (7 -> 42).
    //        The pointer itself was not modified.

    return 0;
}
// ```

// **The single line that defines pointer mutation:**

// ```cpp
// *px = 42;
// // * on LEFT side of = means WRITE to that address
// // * on RIGHT side of = means READ from that address
// // *px = 42  -> WRITE 42 to where px points
// // int y = *px -> READ value from where px points
// ```

// > `*` on the **left** = write to that address
// > `*` on the **right** = read from that address