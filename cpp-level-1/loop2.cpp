// ============================================================
// auto vs auto& - COPY vs REFERENCE - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <array>
#include <algorithm>

int main(){

    std::array<int, 3> myArray;  // uninitialized - garbage values

    // ============================================================
    // SECTION 1: std::fill - filling BEFORE the loop (correct way)
    // ============================================================

    std::fill(std::begin(myArray), std::end(myArray), 1024);
    // myArray = {1024, 1024, 1024}
    // fill called ONCE before loop -> efficient
    // last session you called fill INSIDE loop -> redundant (3x same work)

    // ============================================================
    // SECTION 2: LOOP 1 - auto element (COPY)
    // ============================================================

    for (auto element : myArray) {
        element = 999;                      // modifies the COPY only
        std::cout << element << std::endl;  // prints 999
    }
    // myArray is STILL {1024, 1024, 1024} - untouched
    //
    // WHAT HAPPENS INTERNALLY:
    //
    // iteration 1: element = copy of myArray[0] = 1024
    //              element = 999  (copy changed, original safe)
    //              print 999
    //
    // iteration 2: element = copy of myArray[1] = 1024
    //              element = 999
    //              print 999
    //
    // iteration 3: element = copy of myArray[2] = 1024
    //              element = 999
    //              print 999
    //
    // MEMORY VISUAL:
    //
    // myArray:  [1024] [1024] [1024]   <- never changes
    //               \
    //                copy
    //                  \
    // element:        [999]            <- temporary copy, dies each iteration

    // ============================================================
    // SECTION 3: LOOP 2 - auto& element (REFERENCE)
    // ============================================================

    for (auto& element : myArray) {
        // element = 999;               // commented out - not modifying
        std::cout << element << std::endl;  // prints 1024
    }
    // element is a REFERENCE to actual myArray element
    // element IS myArray[i] - same memory address
    // since element=999 is commented out, myArray stays {1024,1024,1024}
    // prints: 1024, 1024, 1024
    //
    // MEMORY VISUAL:
    //
    // myArray:  [1024] [1024] [1024]
    //              ^
    //              |
    // element: (points HERE - same memory, no copy made)

    // ============================================================
    // SECTION 4: PROOF - auto vs auto& are different memory locations
    // ============================================================

    // for (auto element : myArray) {
    //     std::cout << &element << std::endl;   // address of COPY
    //     std::cout << &myArray[0] << std::endl; // address of original
    //     // these two addresses will be DIFFERENT
    // }
    //
    // for (auto& element : myArray) {
    //     std::cout << &element << std::endl;   // address of element
    //     std::cout << &myArray[0] << std::endl; // address of original
    //     // these two addresses will be SAME
    // }

    // ============================================================
    // SECTION 5: OUTPUT SUMMARY OF YOUR CODE
    // ============================================================

    // Loop 1 (auto - copy):
    //   prints: 999
    //           999
    //           999
    //   myArray after: {1024, 1024, 1024}  <- unchanged

    // Loop 2 (auto& - reference):
    //   prints: 1024
    //           1024
    //           1024
    //   myArray after: {1024, 1024, 1024}  <- unchanged (999 line commented)

    // ============================================================
    // SECTION 6: ALL 5 VARIATIONS - COMPLETE CHEAT SHEET
    // ============================================================

    // for (int element   : myArray)  -> copy, type hardcoded
    // for (auto element  : myArray)  -> copy, type deduced        (read only use)
    // for (int& element  : myArray)  -> reference, type hardcoded
    // for (auto& element : myArray)  -> reference, type deduced   (read + modify)
    // for (const auto& e : myArray)  -> reference, read-only      (read, no modify, no copy)
    //                                   BEST for large objects like std::string

    // ============================================================
    // SECTION 7: COMMENTED CODE EXPLAINED
    // ============================================================

    // int arr[] = {1, 3, 5};           // C-style raw array
    // std::array<int,3> arr2{1,3,5};   // modern C++ array

    // Classic index loop:
    // for (int i = 0; i < arr2.size(); i++) -> .size() returns 3

    // Scope-extended loop (i survives after loop):
    // int i = 0;
    // for (; i < 3; i++) { }
    // std::cout << i;  // i = 3 here, still accessible

    // Infinite loop:
    // for(;;) { }   // no condition = never stops, needs break inside

    // int& reference loop:
    // for (int& element : arr2) -> reference to each element
    //   same as auto& but type is manually written

    // while loop:
    // int count = 3;
    // while (count > 0) { count--; }  // prints 3, 2, 1

    // ============================================================
    // SECTION 8: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: What is the output of loop 1 (auto copy) after std::fill?
    //     -> prints 999 three times
    //        BUT myArray stays {1024,1024,1024} - copy was modified not original

    // Q2: What is the output of loop 2 (auto& reference)?
    //     -> prints 1024 three times (element=999 is commented out)

    // Q3: If element=999 was uncommented in loop 2, what changes?
    //     -> myArray becomes {999, 999, 999}
    //        Because auto& directly modifies the original

    // Q4: How do you prove auto makes a copy and auto& does not?
    //     -> Print &element and &myArray[0]
    //        auto  -> different addresses (copy in different memory)
    //        auto& -> same address (element IS myArray[0])

    // Q5: What is the performance difference between auto and auto&?
    //     -> auto   : copies each element every iteration
    //                 for int = 4 bytes copied (negligible)
    //                 for std::string = entire string copied (expensive!)
    //        auto&  : no copy, just a reference (always 8 bytes on 64-bit)
    //                 always faster for large objects

    // Q6: What does const auto& give you over auto&?
    //     -> auto&       : reference, CAN accidentally modify original
    //        const auto& : reference, CANNOT modify original
    //                      compiler error if you try to modify
    //                      best practice for read-only loops on large objects

    // Q7: Can you use auto with raw arrays?
    //     -> YES
    //        int arr[] = {1,2,3};
    //        for (auto x : arr) { }   // works perfectly

    return 0;
}
// ```

// **The single line to lock this in memory forever:**

// ```cpp
// for (auto  element : myArray)  // PHOTOCOPY  - original safe,  you get a duplicate
// for (auto& element : myArray)  // ORIGINAL   - you are holding the real thing
// ```

// > Think of `auto` as getting a **photocopy** of a document. You can scribble all over it — the original file is untouched. `auto&` is you holding the **original** — whatever you write on it is permanent.