// ============================================================
// const vs constexpr - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: const
// ============================================================

// const = "this value cannot be changed after initialization"
// It is a RUNTIME constant (value is locked, but set at runtime)

// const float PI = 3.1345465476;
//
// STEP 1: 3.1345465476 is a double literal (15-16 digits)
// STEP 2: narrowing conversion -> squeezed into float (6-7 digits)
// STEP 3: PI stores 3.13455 (rest is lost, same narrowing as last time)
// STEP 4: const means PI is now READ-ONLY, cannot be reassigned
//
// PI = 3;  -> COMPILER ERROR: assignment of read-only variable 'PI'

// WHY use const?
// -> Prevents accidental modification of important values
// -> Compiler can sometimes optimize const variables
// -> Self-documenting code ("this value is never meant to change")

// ============================================================
// SECTION 2: constexpr
// ============================================================

// constexpr = "this value is computed at COMPILE TIME"
// It is a COMPILE-TIME constant

// constexpr int expression = 3+6+8;
//
// Without constexpr:
//   int expression = 3+6+8;
//   -> CPU adds 3+6+8 at RUNTIME (when program is actually running)
//   -> Wastes a tiny bit of CPU time
//
// With constexpr:
//   constexpr int expression = 3+6+8;
//   -> Compiler adds 3+6+8 at COMPILE TIME
//   -> By the time your program runs, expression is ALREADY 17
//   -> Zero runtime cost, value baked directly into the binary

// ============================================================
// SECTION 3: const vs constexpr - KEY DIFFERENCE
// ============================================================

// const    -> value fixed at RUNTIME
//             can be initialized with a runtime value
//
// constexpr -> value fixed at COMPILE TIME
//              must be initialized with a compile-time expression

// Example to show the difference:

// int getUserInput() { return 42; }  // runtime function

// const int a = getUserInput();      // VALID
//                                    // a is const but set at runtime

// constexpr int b = getUserInput();  // INVALID (if not constexpr fn)
//                                    // compiler doesn't know return
//                                    // value at compile time -> ERROR

// constexpr int c = 3 + 6 + 8;      // VALID
//                                    // compiler can solve 3+6+8 itself

// ============================================================
// SECTION 4: WHY does cout print 3.13455 and not 3.1345465476?
// ============================================================

// Two reasons:

// REASON 1: Narrowing conversion (double literal -> float)
//   3.1345465476 stored as float loses digits after ~7th digit

// REASON 2: cout default precision is 6 significant digits
//   std::cout << PI  -> prints only 6 significant digits by default
//   So you see: 3.13455

// To print more digits:
//   std::cout << std::fixed << std::setprecision(10) << PI;
//   -> shows you exactly how much precision float actually stored

// ============================================================
// SECTION 5: WHERE constexpr is actually used in real life
// ============================================================

// 1. Array sizes (must be compile-time constant)
//    constexpr int SIZE = 100;
//    int arr[SIZE];              // VALID
//
//    const int SIZE2 = 100;
//    int arr2[SIZE2];            // works in practice but technically
//                                // compiler-dependent (VLA gray area)

// 2. Template parameters (must be compile-time)
//    constexpr int N = 5;
//    std::array<int, N> myArr;   // VALID

// 3. Switch case labels
//    constexpr int CASE_A = 1;
//    switch(x) { case CASE_A: ... }  // VALID

// ============================================================
// SECTION 6: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between const and constexpr?
//     -> const: value cannot change, but set at runtime
//        constexpr: value computed at compile time, zero runtime cost

// Q2: Can a constexpr variable be modified?
//     -> NO. constexpr implies const automatically.
//        It is both compile-time AND read-only.

// Q3: Can const be used with pointers?
//     -> Yes, 3 ways:
//        const int* p      -> pointer to const data (data locked)
//        int* const p      -> const pointer (address locked)
//        const int* const p -> both locked

// Q4: When should you prefer constexpr over const?
//     -> When the value is known at compile time and used in
//        contexts requiring compile-time constants (array size,
//        templates, switch cases).

// Q5: Is constexpr always evaluated at compile time?
//     -> NOT guaranteed 100%. If used in a runtime context,
//        compiler MAY evaluate at runtime. But if used in a
//        compile-time context (array size etc.), it MUST be
//        compile time. Use 'if consteval' (C++23) to force it.

// Q6: What is the difference between #define and constexpr?
//     -> #define PI 3.14  -> preprocessor macro, no type safety,
//                            no scope, just text replacement
//        constexpr float PI = 3.14f -> has type, has scope,
//                            compiler can optimize, debugger can see it
//        Always prefer constexpr over #define in modern C++

int main() {
    const float PI = 3.1345465476;       // narrowing + read-only
    constexpr int expression = 3+6+8;    // compiler solves this = 17

    std::cout << PI << std::endl;        // prints 3.13455 (6 sig digits default)
    // PI = 3;                           // ERROR: read-only variable
    std::cout << PI << std::endl;        // same value, nothing changed

    return 0;
}





// Perfect question. The answer is **when** the locking happens:

// ```cpp
// ============================================================
// WHY constexpr when const already locks the value?
// ============================================================

// const only answers ONE question:
//   "Can this value change?" -> NO
//
// constexpr answers TWO questions:
//   "Can this value change?" -> NO
//   "Is this value known at COMPILE TIME?" -> YES
//
// constexpr is a STRONGER guarantee than const
// Every constexpr is const, but NOT every const is constexpr

// ============================================================
// THE REAL PROBLEM const CANNOT SOLVE
// ============================================================

// SITUATION 1 - Array size must be known at COMPILE TIME:

    const int size1 = 10;
    int arr1[size1];          // works on most compilers BUT
                              // technically undefined behavior in standard C++
                              // because const is a runtime concept
                              // compiler just "gets lucky" here

    constexpr int size2 = 10;
    int arr2[size2];          // 100% LEGAL, guaranteed by standard
                              // compiler KNOWS size2 = 10 at compile time

// SITUATION 2 - Template parameter must be COMPILE TIME:

    const int N1 = 5;
    std::array<int, N1> a1;   // may or may not work, compiler dependent

    constexpr int N2 = 5;
    std::array<int, N2> a2;   // always works, N2 is guaranteed compile time

// SITUATION 3 - THIS is where const completely FAILS:

    int getUserAge() { return 25; } // some runtime function

    const int age = getUserAge();      // VALID - const allows runtime value
    constexpr int age2 = getUserAge(); // ERROR - constexpr needs compile
                                       // time value, getUserAge() runs
                                       // at runtime, compiler has no idea
                                       // what it returns while compiling

// This example proves const and constexpr are fundamentally different
// const just means "locked"
// constexpr means "locked AND solved before program even starts"

// ============================================================
// TIMELINE VISUAL
// ============================================================

// Your C++ code lifecycle:
//
// [YOUR CODE] -> [COMPILE TIME] -> [BINARY/EXE] -> [RUNTIME]
//
// const    lives here ---------------------------------> HERE (runtime)
//                                                       value locked here
//
// constexpr lives here -------> HERE (compile time)
//                                value solved HERE
//                                binary already has the answer baked in

// ============================================================
// SIMPLE ANALOGY
// ============================================================

// Imagine you are cooking:
//
// const     = "I will decide the recipe today morning and NOT change it"
//             (decided when cooking starts = runtime)
//
// constexpr = "Recipe was already printed in the cookbook"
//             (decided before cooking even starts = compile time)
//
// Both mean the recipe won't change mid-cooking.
// But constexpr means the decision happened WAY earlier.

// ============================================================
// PERFORMANCE PROOF - WHY constexpr is faster
// ============================================================

// Without constexpr:
//   int result = 3 + 6 + 8;
//   Assembly generated:
//     MOV eax, 3
//     ADD eax, 6
//     ADD eax, 8       <- CPU does this addition AT RUNTIME
//     MOV [result], eax

// With constexpr:
//   constexpr int result = 3 + 6 + 8;
//   Assembly generated:
//     MOV [result], 17  <- compiler already solved it, 17 baked in
//                          CPU does ZERO addition at runtime
// ```

// **One line to nail it in interview:**

// > `const` = value **locked at runtime**
// > `constexpr` = value **solved at compile time, locked forever**

// `const` just prevents modification. `constexpr` moves the entire **computation** out of your running program into the compiler — your CPU never even sees the math. That's why it exists.