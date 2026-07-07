// ============================================================
// TEMPLATE FULL SPECIALIZATION - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <typeinfo>
#include <cmath>

// ============================================================
// SECTION 1: WHY TEMPLATE SPECIALIZATION EXISTS
// ============================================================

// GENERIC template works for MOST types:
// template<typename T>
// bool equal(T a, T b) { return (a == b); }
//
// PROBLEM with floating point:
// float a = 1.0f - 0.999999f;  // should be 0.000001f
// float b = 0.000001f;
// a == b  -> might be FALSE! (floating point precision errors)
//
// WHY? floating point numbers are stored in BINARY
// 0.999999 cannot be represented EXACTLY in binary
// tiny rounding errors accumulate
// a - b might be 0.0000009999... not exactly 0.000001
// == comparison FAILS even though mathematically equal
//
// SOLUTION: template specialization
// "for MOST types: use == comparison"
// "for float/double: use epsilon (tolerance) comparison"

// ============================================================
// SECTION 2: GENERIC TEMPLATE (base case)
// ============================================================

template <typename T>
bool equal(T a, T b)
{
    return (a == b);
    // works correctly for: int, char, bool, string etc.
    // WRONG for: float, double (precision issues)
}

// ============================================================
// SECTION 3: FULL SPECIALIZATION FOR float
// ============================================================

template <>            // empty template parameter list = FULL specialization
bool equal<float>(float a, float b)
//          ^^^^^
//          explicitly specifying: this specialization is for float
{
    std::cout << "equal<float>\n" << std::endl;
    return fabs(a - b) < 0.00001f;
    // fabs = floating point absolute value (from <cmath>)
    // if |a - b| < 0.00001, consider them "equal"
    // 0.00001f = epsilon (tolerance threshold)
    // "close enough" rather than "exactly equal"
}

// YOUR COMMENT: "same" referring to this non-template overload:
// bool equal(float a, float b) { return fabs(a==b); }
// -> for function templates, a regular overload CAN override the template
// -> BUT: template<> specialization is PREFERRED and cleaner
// -> it makes the intent EXPLICIT: "this is a customization of the template"

// ============================================================
// SECTION 4: FULL SPECIALIZATION FOR double
// ============================================================

template <>
bool equal<double>(double a, double b)
//          ^^^^^^
//          specialization for double
{
    std::cout << "equal<double>\n" << std::endl;
    return fabs(a - b) < 0.0000000001;
    // SMALLER epsilon than float version!
    // double has MORE precision (~15-16 digits vs ~6-7 for float)
    // so we can demand CLOSER comparison for "equal"
    // 0.0000000001 = 10^-10 (tighter tolerance)
}

// ============================================================
// SECTION 5: FULL vs PARTIAL SPECIALIZATION
// ============================================================

// FULL SPECIALIZATION:
// template<>
// bool equal<float>(float a, float b) { ... }
// -> ALL template parameters are SPECIFIED (none left open)
// -> template<> has EMPTY angle brackets
// -> exactly ONE specific type being customized
// -> allowed for BOTH function and class templates
//
// PARTIAL SPECIALIZATION:
// -> only SOME template parameters are specified
// -> remaining parameters still generic
// -> ONLY allowed for CLASS templates (NOT function templates!)
//
// Example of PARTIAL (class template only):
// template<typename T, size_t N>
// class Array { ... };       // generic
//
// template<size_t N>
// class Array<int, N> { ... }; // partial: T fixed to int, N still generic
// -> "for any N, but specifically when T=int, use this version"
//
// For FUNCTION templates:
// -> full specialization: allowed (your code)
// -> partial specialization: NOT directly allowed
//    -> use OVERLOADING instead (write a new function template with fewer params)

// ============================================================
// SECTION 6: SPECIALIZATION SELECTION PRIORITY
// ============================================================

// When calling equal<float>(a, b), compiler checks:
//
// PRIORITY ORDER:
// 1. EXACT MATCH non-template function (regular overload)
//    bool equal(float a, float b) { ... }  <- picked FIRST if exists
//
// 2. FULL SPECIALIZATION of template
//    template<> bool equal<float>(float a, float b) { ... } <- picked SECOND
//
// 3. GENERIC TEMPLATE (instantiated for float)
//    template<typename T> bool equal(T a, T b) { ... } <- last resort
//
// YOUR COMMENT: "bool equal(float,float) [is] same [as the specialization]"
// -> functionally similar behavior, but:
//    regular overload: picked by overload resolution (step 1)
//    specialization:   picked by template machinery (step 2)
//    for simple cases: end result is the same
//    difference matters in complex template code with forwarding

// ============================================================
// SECTION 7: main() TRACE - COMPLETE OUTPUT
// ============================================================

int main()
{
    std::cout << equal<int>(2, 2) << std::endl;
    // T=int, NO specialization -> generic template used
    // 2 == 2 -> true -> 1
    // OUTPUT: 1

    std::cout << equal<int>(2, 3) << std::endl;
    // T=int, NO specialization -> generic template used
    // 2 == 3 -> false -> 0
    // OUTPUT: 0

    std::cout << equal<float>(1.0f - 0.999999f, 0.000001f) << std::endl;
    // T=float -> SPECIALIZATION used
    // 1.0f - 0.999999f = approximately 0.000001f (with float precision error)
    // fabs(result - 0.000001f) < 0.00001f?
    // -> the difference is tiny -> true -> 1
    // OUTPUT: equal<float>
    //         1

    std::cout << equal<float>(1.0f, -2.0f) << std::endl;
    // T=float -> SPECIALIZATION used
    // fabs(1.0f - (-2.0f)) = fabs(3.0f) = 3.0f
    // 3.0f < 0.00001f? -> NO -> false -> 0
    // OUTPUT: equal<float>
    //         0

    std::cout << equal<double>(1.0 - 0.9999999999, 0.0000000001) << std::endl;
    // T=double -> SPECIALIZATION used
    // 1.0 - 0.9999999999 = approximately 0.0000000001 (with double precision)
    // fabs(result - 0.0000000001) < 0.0000000001?
    // -> very close -> likely true -> 1
    // OUTPUT: equal<double>
    //         1

    return 0;
}

// FULL OUTPUT:
// 1
// 0
// equal<float>
//
// 1
// equal<float>
//
// 0
// equal<double>
//
// 1

// ============================================================
// SECTION 8: EPSILON COMPARISON - THE REAL POINT
// ============================================================

// WRONG: comparing floats with ==
// float a = 0.1f + 0.2f;  // stored as 0.30000001192...
// float b = 0.3f;          // stored as 0.30000001192...
// a == b  -> true or false? UNPREDICTABLE!
//
// RIGHT: epsilon comparison (what your specialization does)
// fabs(a - b) < epsilon
// "are they within epsilon of each other?"
//
// CHOOSING EPSILON:
// float:  ~0.00001f     (6-7 significant digits of precision)
// double: ~0.0000000001 (15-16 significant digits of precision)
//         (your code uses 10^-10 for double)
//
// INDUSTRY ALTERNATIVES:
// Use relative epsilon (scales with magnitude):
// fabs(a-b) / fabs(a) < epsilon  <- better for large numbers
// Or: std::numeric_limits<float>::epsilon() * multiplier

// ============================================================
// SECTION 9: YOUR COMMENT - "we can write our own customization"
// ============================================================

// EXACTLY RIGHT - this is the ENTIRE POINT of specialization!
//
// Template provides GENERIC behavior
// Specialization provides CUSTOM behavior for SPECIFIC types
//
// COMMON USE CASES:
// 1. Floating point comparison (your example - industry standard!)
// 2. std::hash<> specialization (for unordered_map with custom types)
//    template<> struct hash<MyClass> { size_t operator()(MyClass c) {...} }
//
// 3. std::numeric_limits<> (different limits per type)
//    std::numeric_limits<int>::max()   -> 2147483647
//    std::numeric_limits<float>::max() -> 3.4e+38
//
// 4. Type traits (std::is_integral, std::is_floating_point etc.)
//    template<typename T> struct is_integral { bool value = false; };
//    template<> struct is_integral<int> { bool value = true; };
//    template<> struct is_integral<long> { bool value = true; };

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is template specialization?
//     -> Providing a custom implementation of a template for a
//        specific type, overriding the generic version for that type.
//        Compiler uses specialized version when that exact type is used.

// Q2: What is the syntax for full template specialization?
//     -> template<>  (empty angle brackets, all params specified)
//        bool equal<float>(float a, float b) { ... }
//        The <float> after the function name says "this is for float".

// Q3: Why is floating point == comparison wrong?
//     -> Binary representation cannot exactly store most decimals.
//        Rounding errors accumulate. 1.0f-0.999999f might not equal
//        0.000001f exactly. Use epsilon (tolerance) comparison instead.

// Q4: What is the difference between full and partial specialization?
//     -> Full: all template params specified. template<> class X<int,5>{}
//        Partial: some params still generic. template<typename T> class X<T,5>{}
//        Partial only works for CLASS templates, not function templates.

// Q5: What is the selection priority when multiple versions exist?
//     -> 1. Regular overload (non-template)
//        2. Full template specialization
//        3. Generic template instantiation
//        Most specific match wins.

// Q6: Can function templates be partially specialized?
//     -> NO. Partial specialization only works for class templates.
//        For functions: use overloading (write additional function
//        templates) to achieve similar behavior.

// Q7: What is epsilon comparison and when do you use it?
//     -> Instead of a==b, use fabs(a-b) < epsilon.
//        Use when comparing floating point numbers.
//        Epsilon value depends on type precision:
//        float: ~0.00001, double: ~0.0000000001.

// Q8: What is a real-world use of template specialization?
//     -> std::hash<> specialization for unordered_map with custom types.
//        std::numeric_limits<T> specialized for each numeric type.
//        std::is_integral<T> type trait specialized for integer types.
//        Floating point comparison (your code) - industry standard pattern.
// ```

// **Full specialization syntax - memorize the pattern:**

// ```cpp
// // GENERIC (base case):
// template<typename T>
// bool equal(T a, T b) { return a == b; }

// // FULL SPECIALIZATION (empty template<>):
// template<>
// bool equal<float>(float a, float b) { return fabs(a-b) < 0.00001f; }
// //          ^^^^^
// //          specifying WHICH type this specialization is for
// ```

// **Why floating point == is broken:**

// ```
// 0.1f + 0.2f in binary = 0.30000001192...
// 0.3f        in binary = 0.30000001192...

// These might NOT be equal with ==
// But fabs(a - b) < 0.00001f ? → YES, "equal enough"
// ```

// > **The interview one-liner**: template specialization lets you say *"use the generic logic for everyone, EXCEPT for this specific type — use MY custom logic instead."* The floating-point epsilon comparison is the **most common real-world example** and comes up constantly in graphics, physics, and simulation code — exactly the domain you work in with ISSST.