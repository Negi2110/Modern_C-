// ============================================================
// constexpr - COMPLETE NOTES - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS constexpr? - FROM FIRST PRINCIPLES
// ============================================================

// YOU ALREADY KNOW const:
// const int x = 5;  -> x cannot change after initialization
//                      BUT initialization CAN happen at runtime:
// const int n = getUserInput();  // set at runtime, then locked
//
// constexpr = STRONGER than const:
// -> value MUST be known at COMPILE TIME (not just runtime-locked)
// -> compiler COMPUTES the value during compilation
// -> result is BAKED INTO the binary (no computation at runtime)
// -> zero runtime overhead
//
// THINK OF IT AS:
// const     = "this won't change" (runtime guarantee)
// constexpr = "this is known at compile time" (compile-time guarantee)
//
// EVERY constexpr IS const (but NOT every const IS constexpr):
// constexpr int x = 5;             // compile-time AND immutable
// const int y = getUserInput();    // runtime value, but immutable after
// constexpr int z = getUserInput(); // ERROR! getUserInput() not constexpr

// ============================================================
// SECTION 2: constexpr VARIABLE
// ============================================================

constexpr float f = 3.14f;
// -> 3.14f is a literal (known at compile time)
// -> f = 3.14f baked into binary
// -> accessing f at runtime = just reading a constant (like #define but typed)
//
// COMPARE to const:
// const float g = 3.14f;    // also works here (literal IS compile-time)
// constexpr float f = 3.14f; // STRONGER guarantee, same result here
//
// WHERE constexpr variable IS required (not just const):
// constexpr int SIZE = 10;
// int arr[SIZE];              // array size MUST be compile-time constant
//                              // const SIZE would also work here IF
//                              // initialized with literal
// std::array<int, SIZE> a;    // template non-type param = compile-time required
//
// CANNOT do with non-constexpr:
// int n = 10;
// int arr[n];        // VLA - not standard C++, compiler-dependent
// std::array<int,n>; // ERROR - n not compile-time constant

// ============================================================
// SECTION 3: constexpr FUNCTION
// ============================================================

constexpr int add(int a, int b)
{
    return a + b;
    // SIMPLE body required (C++11: very restricted)
    // C++14: can have if/else, loops
    // C++17: most things allowed
    // C++20: virtual, try/catch, new/delete (!) allowed
}
// constexpr function = CAN be called at compile time
//                      IF arguments are compile-time constants
//
// add(5, 18) -> called with literals -> computed at COMPILE TIME
//               result (23) baked into binary, no function call at runtime
//
// add(x, y)  -> called with runtime values -> computed at RUNTIME
//               same as a regular function call
//
// THIS IS THE KEY:
// constexpr function = DUAL PURPOSE:
// -> compile-time when possible (args are constexpr)
// -> runtime otherwise (args are runtime values)

// ============================================================
// SECTION 4: main() TRACE
// ============================================================

int main()
{
    constexpr int i = add(5, 18);
    // add(5, 18):
    // -> 5 and 18 are LITERALS (compile-time constants)
    // -> add is constexpr function
    // -> compiler COMPUTES: 5 + 18 = 23 AT COMPILE TIME
    // -> i = 23 baked into binary
    // -> AT RUNTIME: no addition, no function call, just "i = 23"
    // -> equivalent to: constexpr int i = 23;
    //
    // PROOF: look at assembly (with -O0 even):
    // constexpr int i = add(5,18);
    // assembly: mov DWORD PTR [rbp-4], 23  <- just stores 23, no call!

    // NON-CONSTEXPR usage (would still compile):
    // int x = 5, y = 18;
    // int j = add(x, y);  // x,y not constexpr -> add runs at RUNTIME
    // constexpr int k = add(x, y); // ERROR! x,y not compile-time constants

    return 0;
}

// ============================================================
// SECTION 5: constexpr EVOLUTION ACROSS C++ VERSIONS
// ============================================================

// C++11 - VERY RESTRICTIVE:
// constexpr function must have:
// -> exactly ONE return statement
// -> no if/else
// -> no loops
// -> no local variables
// constexpr int add(int a, int b) { return a + b; }  // OK (yours!)
// constexpr int max(int a, int b) { return a>b?a:b; }  // OK (ternary)
// constexpr int factorial(int n) {  // ERROR in C++11!
//     int result = 1;
//     for (int i=1; i<=n; i++) result *= i;
//     return result;
// }

// C++14 - RELAXED:
// can have: if/else, loops, local variables, multiple returns
// constexpr int factorial(int n) {  // OK in C++14+
//     int result = 1;
//     for (int i=1; i<=n; i++) result *= i;
//     return result;
// }

// C++17 - MORE RELAXED:
// if constexpr (compile-time branching in templates)
// constexpr lambdas

// C++20 - MOST RELAXED:
// constexpr virtual functions
// constexpr std::vector (dynamic allocation in constexpr!)
// constexpr try/catch
// constexpr new/delete

// ============================================================
// SECTION 6: if constexpr - C++17 (template use case)
// ============================================================

// if constexpr = compile-time if (evaluated at compile time in templates)
// REMOVES the dead branch from generated code entirely
//
// template<typename T>
// void print(T value) {
//     if constexpr (std::is_integral_v<T>) {
//         std::cout << "int: " << value;
//     } else if constexpr (std::is_floating_point_v<T>) {
//         std::cout << "float: " << value;
//     } else {
//         std::cout << "other: " << value;
//     }
// }
// print(5)    -> only "int: 5" branch compiled for this instantiation
// print(3.14) -> only "float: 3.14" branch compiled
//
// WITHOUT if constexpr (regular if):
// both branches must compile even if only one runs!
// -> problem if branches have type-specific code

// ============================================================
// SECTION 7: constexpr vs #define vs const - COMPARISON
// ============================================================

// #define PI 3.14159f
// -> preprocessor text replacement (before compilation)
// -> NO type safety (just text)
// -> NO scope (global, can conflict with names)
// -> can't debug (doesn't exist in debugger)
//
// const float PI = 3.14159f;
// -> has type (float)
// -> has scope (follows normal scoping rules)
// -> might or might not be compile-time (depends on initializer)
// -> CAN be passed to constexpr contexts if literal-initialized
//
// constexpr float PI = 3.14159f;
// -> GUARANTEED compile-time
// -> has type + scope (like const)
// -> debugger can see it
// -> ALWAYS prefer over #define and usually over const for constants
//
// SUMMARY:
// #define    : no type, no scope, text replacement
// const      : typed, scoped, runtime OR compile-time
// constexpr  : typed, scoped, GUARANTEED compile-time

// ============================================================
// SECTION 8: PRACTICAL EXAMPLES
// ============================================================

// COMPILE-TIME ARRAY SIZES:
// constexpr size_t BUFFER_SIZE = 1024;
// char buffer[BUFFER_SIZE];           // OK: compile-time size
// std::array<char, BUFFER_SIZE> arr;  // OK: template non-type param

// COMPILE-TIME MATH:
// constexpr double PI = 3.14159265358979;
// constexpr double circleArea(double r) { return PI * r * r; }
// constexpr double area = circleArea(5.0);  // computed at compile time!
// double arr[static_cast<int>(area)];       // array size = computed area

// COMPILE-TIME LOOKUP TABLE:
// constexpr int squares[10] = {0,1,4,9,16,25,36,49,64,81};
// constexpr int sq = squares[7];  // 49, compile-time

// IN TEMPLATES (your non-type template params session):
// template<typename T, size_t N>
// class Array { T data[N]; };
// constexpr size_t SIZE = 10;
// Array<int, SIZE> arr;  // SIZE must be compile-time -> constexpr fits!

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is constexpr?
//     -> Keyword that guarantees a variable or function can be
//        evaluated at compile time. Value is computed during
//        compilation and baked into the binary. Zero runtime overhead.

// Q2: What is the difference between const and constexpr?
//     -> const: value cannot change after initialization, but
//               initialization can happen at runtime.
//        constexpr: value MUST be computable at compile time.
//               Every constexpr is const, but not vice versa.

// Q3: When is a constexpr function called at compile time vs runtime?
//     -> Compile time: when ALL arguments are compile-time constants
//                      (literals, constexpr variables).
//        Runtime: when ANY argument is a runtime value.
//        Same function, used both ways! Compiler chooses.

// Q4: What must a constexpr function body contain in C++11?
//     -> Only a single return statement (very restrictive).
//        No if/else, no loops, no local variables.
//        C++14 relaxed this to allow if/else, loops, locals.
//        C++20 allows almost everything.

// Q5: What is if constexpr (C++17)?
//     -> Compile-time if statement in templates. The non-taken branch
//        is completely removed from generated code. Allows branches
//        that would fail to compile for some types to be safely ignored.

// Q6: Why prefer constexpr over #define for constants?
//     -> constexpr has type safety, scope, and is visible to debugger.
//        #define is untyped text replacement with no scope, can cause
//        naming conflicts, invisible to debugger.

// Q7: Can constexpr functions use if/else and loops in C++14?
//     -> YES. C++14 greatly relaxed constexpr requirements.
//        Can have: multiple returns, local variables, if/else, loops.
//        C++11 was restricted to single return statement only.

// Q8: What is the runtime cost of a constexpr computation?
//     -> ZERO when used in a compile-time context (constexpr variable,
//        template argument, array size). The computation happens during
//        compilation, result is a literal in the binary.
//        If used at runtime with non-constexpr args: same cost as
//        a regular function call.
// ```

// **const vs constexpr - the key distinction:**

// ```cpp
// const int a = 5;              // compile-time OR runtime - both OK
// const int b = getUserInput(); // RUNTIME - locked after, but not compile-time

// constexpr int c = 5;          // MUST be compile-time - OK (literal)
// constexpr int d = getUserInput(); // ERROR! getUserInput() not constexpr
// ```

// **constexpr function - dual personality:**

// ```cpp
// constexpr int add(int a, int b) { return a + b; }

// // COMPILE TIME (args are literals/constexpr):
// constexpr int i = add(5, 18);  // 23 baked into binary, no function call!

// // RUNTIME (args are variables):
// int x = 5, y = 18;
// int j = add(x, y);  // regular function call at runtime
// ```

// > **One-liner for interviews**: `constexpr` moves computation from **runtime to compile-time**. The program starts with the answer already computed — the CPU never has to calculate it. This is why `constexpr int SIZE = 10; std::array<int, SIZE> arr;` works, while `const int n = 10; std::array<int, n> arr;` might not in all contexts — `constexpr` is the **guaranteed** compile-time version.