
// ============================================================
// LOOPS, RANGE-BASED FOR, std::fill, auto, references - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <array>
#include <algorithm>  // for std::fill

int main(){

    // ============================================================
    // SECTION 1: UNINITIALIZED std::array - GARBAGE VALUES
    // ============================================================

    std::array<int, 3> myArray;   // NO = {} -> garbage values on stack
    // myArray = {?, ?, ?}  unknown values

    // ============================================================
    // SECTION 2: std::fill INSIDE range-based for - WHAT HAPPENS?
    // ============================================================

    for (auto& element : myArray) {
        std::fill(std::begin(myArray), std::end(myArray), 1024);
        std::cout << element << std::endl;
    }

    // std::fill(begin, end, value)
    // -> fills ENTIRE array with given value
    // -> here fills all 3 elements with 1024
    //
    // BUT std::fill is called EVERY iteration of the loop
    // That is REDUNDANT - same work done 3 times
    //
    // CORRECT way: call fill ONCE before the loop
    // std::fill(std::begin(myArray), std::end(myArray), 1024);
    // for (auto& element : myArray) {
    //     std::cout << element << std::endl;
    // }
    //
    // Output of your code: 1024
    //                       1024
    //                       1024
    // (works correctly here but inefficient)

    // ============================================================
    // SECTION 3: FOUR TYPES OF LOOPS IN C++
    // ============================================================

    int arr[] = {1, 3, 5};
    std::array<int, 3> arr2{1, 3, 5};

    // ---- LOOP TYPE 1: Classic for loop ----
    for (int i = 0; i < 3; i++) {
        std::cout << arr[i] << std::endl;
    }
    // i is born at start, dies after loop
    // Full control: start, condition, increment all visible

    // ---- LOOP TYPE 2: Scope-extended for loop ----
    int i = 0;             // i declared OUTSIDE -> survives after loop
    for (; i < 3; i++) {   // init section is empty
        std::cout << arr[i] << std::endl;
    }
    // i still accessible here (= 3 after loop ends)
    // INTERVIEW: When would you want i after the loop?
    //   -> To check HOW MANY iterations ran, or WHERE loop stopped

    // ---- LOOP TYPE 3: Infinite loop ----
    // for(;;) { }   // no init, no condition, no increment = infinite
    // while(true) { } // same thing
    // INTERVIEW: When is infinite loop used?
    //   -> Game loops, server event loops, embedded system main loops
    //      Always needs a break; condition inside

    // ---- LOOP TYPE 4: Range-based for loop (C++11) ----
    for (int element : arr2) {        // copy of each element
        std::cout << element << std::endl;
    }
    // element is a COPY -> modifying element does NOT change arr2

    // ============================================================
    // SECTION 4: int vs auto vs int& vs auto& in range-based for
    // ============================================================

    // VERSION 1: int element (COPY)
    for (int element : arr2) {
        element = 999;        // modifies COPY only, arr2 unchanged
        std::cout << element << std::endl;
    }

    // VERSION 2: auto element (COPY, type deduced)
    for (auto element : arr2) {
        element = 999;        // still a copy, arr2 unchanged
        std::cout << element << std::endl;
    }
    // auto = compiler deduces type automatically
    // here auto = int (because arr2 holds ints)
    // INTERVIEW: What is auto?
    //   -> Type deduction keyword (C++11)
    //      Compiler figures out the type from the initializer
    //      Does NOT mean dynamic typing - type is fixed at compile time

    // VERSION 3: int& element (REFERENCE - modifies original)
    for (int& element : arr2) {
        element = 999;        // modifies arr2 directly!
        std::cout << element << std::endl;
    }
    // arr2 is now {999, 999, 999}

    // VERSION 4: auto& element (REFERENCE, type deduced)
    for (auto& element : arr2) {
        element = 999;        // modifies arr2 directly!
        std::cout << element << std::endl;
    }
    // BEST PRACTICE for large objects (avoids copying)
    // For small types like int, copy is fine
    // For large objects like std::string, always use auto&

    // ============================================================
    // SECTION 5: WHAT IS A REFERENCE?
    // ============================================================

    // int& element = arr2[0];
    // element is NOT a copy - it IS arr2[0]
    // They share the same memory address
    //
    // int x = 5;
    // int& ref = x;   // ref is another name for x
    // ref = 10;       // x is now 10
    //
    // INTERVIEW: What is the difference between pointer and reference?
    //   -> Reference: cannot be null, cannot be reassigned to another var
    //                 always valid, cleaner syntax
    //      Pointer:   can be null, can point to different vars,
    //                 needs * to dereference, needs & to get address

    // ============================================================
    // SECTION 6: WHILE LOOP
    // ============================================================

    int count = 3;
    while (count > 0) {
        std::cout << count << std::endl;
        count--;
    }
    // prints: 3, 2, 1
    //
    // INTERVIEW: difference between for and while?
    //   -> for:   use when number of iterations is KNOWN beforehand
    //      while: use when number of iterations depends on a CONDITION
    //
    // do-while: executes body AT LEAST ONCE, checks condition after
    // do {
    //     std::cout << count << std::endl;
    //     count--;
    // } while (count > 0);

    // ============================================================
    // SECTION 7: std::fill vs std::fill_n
    // ============================================================

    // std::fill(begin, end, value)
    // -> fills from begin to end with value
    // std::fill(std::begin(arr2), std::end(arr2), 0);

    // std::fill_n(begin, count, value)
    // -> fills 'count' elements starting from begin
    // std::fill_n(std::begin(arr2), 2, 0);  // fills only first 2 elements

    // ============================================================
    // SECTION 8: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: What is range-based for loop? How does it work internally?
    //     -> Syntactic sugar introduced in C++11
    //        for (auto x : container) internally becomes:
    //        auto it = begin(container);
    //        while (it != end(container)) { x = *it; ++it; }

    // Q2: What is the difference between auto and auto&?
    //     -> auto   = makes a COPY  (safe, original unchanged)
    //        auto&  = makes a REFERENCE (modifies original, no copy cost)

    // Q3: When should you use auto& in range-based for?
    //     -> Always for large objects (std::string, structs, vectors)
    //        to avoid expensive copies.
    //        For primitives (int, float) copy is fine.

    // Q4: What does std::fill do?
    //     -> Fills a range [begin, end) with a given value.
    //        From <algorithm> header.

    // Q5: What is the difference between while and do-while?
    //     -> while:    checks condition FIRST, may never execute body
    //        do-while: executes body FIRST, then checks condition
    //                  guaranteed to run AT LEAST ONCE

    // Q6: What does auto deduce for std::array<int,3>?
    //     -> auto element : arr2  -> auto = int (copy)
    //        auto& element : arr2 -> auto& = int& (reference)

    // Q7: Can range-based for loop work on raw arrays?
    //     -> YES. for (int x : arr) works on raw arrays too.
    //        Compiler uses begin/end internally.

    // Q8: What is const auto& and when to use it?
    //     -> for (const auto& element : arr2)
    //        Reference (no copy) + const (cannot modify)
    //        BEST PRACTICE when you only need to READ large objects
    //        No copy cost + protected from accidental modification

    return 0;
}
// ```

// **The four variations every interviewer tests:**

// ```cpp
// for (int element   : arr)  // copy,      cannot modify original
// for (int& element  : arr)  // reference, CAN modify original
// for (auto element  : arr)  // copy,      type deduced
// for (auto& element : arr)  // reference, type deduced  <- most common in real code
// for (const auto& e : arr)  // reference, read-only     <- best for large objects
// ```

// > Rule of thumb: small types like `int` → `auto`. Large types like `std::string`, structs → `const auto&` to avoid copying.