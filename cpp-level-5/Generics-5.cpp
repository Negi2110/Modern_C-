// ============================================================
// VARIADIC TEMPLATES, PARAMETER PACKS - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS VARIADIC? - FROM FIRST PRINCIPLES
// ============================================================

// VARIADIC = variable number of arguments
//
// REGULAR function: fixed number of parameters
// int add(int a, int b)  -> ALWAYS exactly 2 args
//
// VARIADIC function: ANY number of parameters
// Sum(1)               -> 1 argument
// Sum(1, 2)            -> 2 arguments
// Sum(1, 2, 3, 4, 5)   -> 5 arguments
// Sum(1,2,3,...,100)   -> 100 arguments
//
// C had variadic functions via ... (printf uses this)
// C++ added VARIADIC TEMPLATES (type-safe, compile-time)
// -> much safer than C's va_list/va_arg approach

// ============================================================
// SECTION 2: ANATOMY OF VARIADIC TEMPLATE
// ============================================================

// template<typename T, typename... Args>
//                      ^^^^^^^^^^^^^^^
//                      PARAMETER PACK
//                      '...' = "zero or more types"
//                      Args = name of the pack (convention)
//
// T Sum(T start, Args... args)
//                ^^^^^^^^^^^^^
//                PACK EXPANSION in function params
//                '...' after Args = "expand into separate params"
//                args = the VALUES of the pack
//
// THREE DOTS (...) APPEAR IN THREE PLACES:
// 1. typename... Args   -> DECLARES the parameter pack (in template list)
// 2. Args... args       -> EXPANDS the pack into function parameters
// 3. Sum(args...)       -> EXPANDS the pack into function call arguments

// ============================================================
// SECTION 3: BASE CASE - REQUIRED FOR RECURSION TO STOP
// ============================================================

template <typename T>
T Sum(T arg)
{
    return arg;
    // BASE CASE: called when only ONE argument remains
    // returns it as-is
    // WITHOUT this: compiler error (infinite recursion at compile time)
    // THIS IS JUST A REGULAR template function (no variadic)
}

// ============================================================
// SECTION 4: RECURSIVE VARIADIC TEMPLATE
// ============================================================

template <typename T, typename... Args>
//                     ^^^^^^^^^^^^^
//                     PARAMETER PACK: zero or more additional types
T Sum(T start, Args... args)
//              ^^^^^^^^^^^
//              PACK EXPANSION: args expands into remaining parameters
{
    return start + Sum(args...);
    //                  ^^^^
    //                  PACK EXPANSION in function call
    //                  passes remaining args to next Sum call
    //                  peels off ONE argument each recursion level
}

// ============================================================
// SECTION 5: HOW RECURSION WORKS - COMPILE TIME UNROLLING
// ============================================================

// Sum(1, 2, 3) is called:
//
// CALL 1: Sum<int, int, int>(1, 2, 3)
//   T=int, start=1, Args={int,int}, args={2,3}
//   return 1 + Sum(2, 3)
//                 ^
//                 args... expands to: 2, 3
//
// CALL 2: Sum<int, int>(2, 3)
//   T=int, start=2, Args={int}, args={3}
//   return 2 + Sum(3)
//
// CALL 3: Sum<int>(3)
//   ONE argument -> matches BASE CASE (not variadic version)
//   return 3
//
// UNWINDING back:
// Sum(3)    = 3
// Sum(2,3)  = 2 + 3 = 5
// Sum(1,2,3)= 1 + 5 = 6
//
// ALL OF THIS HAPPENS AT COMPILE TIME!
// Compiler generates equivalent of: 1 + 2 + 3 = 6
// No actual recursion at runtime (compiler unrolls it)

// ============================================================
// SECTION 6: YOUR main() TRACE
// ============================================================

int main()
{
    std::cout << Sum(17) << std::endl;
    // ONE argument -> matches BASE CASE directly
    // T=int, arg=17
    // returns 17
    // OUTPUT: 17

    std::cout << Sum<double, double>(1, 3, 2.2f, 5.7, 3, 0, 5, 4) << std::endl;
    // EXPLICIT: T=double, first Args type=double
    // args: 1, 3, 2.2f, 5.7, 3, 0, 5, 4
    //
    // TRUNCATION ISSUE (your comment):
    // Sum<double, double> -> T=double, FIRST extra arg must be double
    // remaining args deduced from their literals:
    // 1   = int literal  -> converted to double OK (1.0)
    // 3   = int literal  -> converted to double OK (3.0)
    // 2.2f= float literal-> NARROWING to double? actually float->double is WIDENING
    //                       BUT: T is double, when Sum recurses:
    //                       Sum(3, 2.2f, 5.7, 3, 0, 5, 4)
    //                       T=double, 3 converted to 3.0
    //                       then Sum(2.2f, 5.7, 3, 0, 5, 4)
    //                       T deduced from 2.2f = FLOAT (not double!)
    //                       2.2f as float = 2.20000... (float precision)
    //                       then added to rest...
    //                       TRUNCATION happens because some middle calls
    //                       use float precision instead of double
    //
    // CLEAN RESULT (no truncation):
    // 1 + 3 + 2.2 + 5.7 + 3 + 0 + 5 + 4 = 23.9
    // BUT output might be slightly off due to type mixing

    return 0;
}

// ============================================================
// SECTION 7: C++17 FOLD EXPRESSIONS - MODERN ALTERNATIVE
// ============================================================

// C++17 introduced FOLD EXPRESSIONS - much simpler than recursion:
//
// template<typename... Args>
// auto Sum_fold(Args... args) {
//     return (args + ...);  // FOLD EXPRESSION
//     //      ^^^^^^^
//     //      expands to: arg1 + arg2 + arg3 + ...
// }
//
// TYPES OF FOLD EXPRESSIONS:
// (args + ...)    -> unary right fold: arg1 + (arg2 + (arg3 + ...))
// (... + args)    -> unary left fold:  ((arg1 + arg2) + arg3) + ...
// (init + ... + args) -> binary fold with initial value
//
// YOUR CODE uses recursive approach (pre-C++17 style)
// C++17 fold expression achieves same with much less code!
//
// Sum_fold(1, 2, 3, 4, 5) = 15
// Sum_fold(1.5, 2.5, 3.0) = 7.0

// ============================================================
// SECTION 8: sizeof... OPERATOR
// ============================================================

// sizeof...(args) = number of elements in the parameter pack
//
// template<typename... Args>
// void printCount(Args... args) {
//     std::cout << sizeof...(args) << std::endl;
//     // sizeof...(Args) would also work (type pack)
// }
// printCount(1, 2, 3)        -> 3
// printCount(1, "hi", 3.14)  -> 3
// printCount()               -> 0

// ============================================================
// SECTION 9: REAL WORLD VARIADIC TEMPLATE USES
// ============================================================

// 1. printf replacement (type-safe):
// template<typename... Args>
// void print(std::string fmt, Args... args) { ... }
//
// 2. std::make_unique / std::make_shared:
// template<typename T, typename... Args>
// std::unique_ptr<T> make_unique(Args&&... args) {
//     return std::unique_ptr<T>(new T(args...));
// }
// make_unique<Person>("John", 25, "Engineer")
//                      ^    ^    ^
//                      forwarded to Person's constructor
//
// 3. std::tuple (stores any number of types):
// std::tuple<int, string, float> t(1, "hi", 3.14f);
// -> internally uses variadic template parameter pack
//
// 4. Emplace functions (std::vector::emplace_back):
// vec.emplace_back(args...); // forwards to constructor directly

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a variadic template?
//     -> A template that accepts any number of type parameters
//        using a parameter pack (typename... Args).
//        Allows writing functions that work with any number of
//        arguments of any types, type-safely, at compile time.

// Q2: What does ... mean in template<typename... Args>?
//     -> Declares a parameter pack: "zero or more type parameters".
//        Args... in function params expands the pack into separate params.
//        args... in function calls expands into separate arguments.

// Q3: Why is a base case needed for recursive variadic templates?
//     -> Recursion peels off one argument each call.
//        Eventually only one argument remains.
//        Without a base case (single-arg version), recursion has
//        no stopping condition -> compile-time infinite recursion -> error.

// Q4: When does the recursion actually happen - compile time or runtime?
//     -> COMPILE TIME. The compiler generates separate function
//        instantiations for each step. No actual runtime recursion.
//        The final code is equivalent to: a + b + c + d (unrolled).

// Q5: What is a fold expression (C++17)?
//     -> A way to apply a binary operator to all elements of a
//        parameter pack without recursion.
//        (args + ...) expands to: arg1 + (arg2 + (arg3 + ...))
//        Much simpler than the recursive approach for simple operations.

// Q6: What does sizeof...(args) return?
//     -> The NUMBER of elements in the parameter pack.
//        Evaluated at compile time. Not sizeof the types!
//        printCount(1,"hi",3.14) -> sizeof...(args) = 3.

// Q7: What causes the "truncation" in Sum<double,double>(1,3,2.2f,...)?
//     -> Mixed types in the pack. When recursion reaches 2.2f,
//        T is deduced as float (not double) for that call.
//        float precision used instead of double precision.
//        Fix: use all double literals: Sum(1.0, 3.0, 2.2, 5.7, 3.0, 0.0, 5.0, 4.0)

// Q8: What is a real-world use of variadic templates?
//     -> std::make_unique<T>(args...) forwards constructor arguments.
//        std::tuple stores any combination of types.
//        printf-style type-safe formatting (e.g., fmtlib/std::format).
//        Logging systems that accept any number of values to log.
// ```

// // **The recursion unrolled - what compiler actually generates:**

// // ```cpp
// Sum(1, 2, 3, 4)

// Compiler sees this and generates equivalent of:
// Step 1: Sum(1, 2, 3, 4) = 1 + Sum(2, 3, 4)
// Step 2:                       2 + Sum(3, 4)
// Step 3:                           3 + Sum(4)
// Step 4:                               4      ← base case
// Result: 1 + 2 + 3 + 4 = 10
// ALL at compile time - no runtime recursion!
// ```

// **C++17 fold expression - same result, much simpler:**

// ```cpp
// // Your recursive approach (pre-C++17):
// template<typename T, typename... Args>
// T Sum(T start, Args... args) { return start + Sum(args...); }

// // C++17 fold expression (much simpler):
// template<typename... Args>
// auto Sum(Args... args) { return (args + ...); }
// // (1 + (2 + (3 + 4))) = 10  ← same result
// ```

// > **The three dots rule**: `...` always means "pack of things." Before the name (`...Args`) it **declares** the pack. After the name (`Args...`) it **expands** the pack. Once you understand that the `...` just means "do this for all of them", variadic templates become much less mysterious.