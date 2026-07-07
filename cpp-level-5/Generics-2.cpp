// ============================================================
// TEMPLATE FUNCTIONS vs auto PARAMETERS (C++20) - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: CLASSIC TEMPLATE - RECAP WITH ANATOMY
// ============================================================

// ANATOMY OF A TEMPLATE FUNCTION:
//
// template <typename T>     <- template parameter list
// ^^^^^^^   ^^^^^^^  ^
// keyword   keyword  placeholder type name (T = convention)
//
// T square(T input)         <- function signature using T
// ^        ^
// return   parameter type
// type
//
// { return input * input; } <- function body (same logic for ALL types)
//
// "instantiation" = when compiler GENERATES the specific overload
// square<int>    -> instantiation with T=int
// square<double> -> instantiation with T=double
// each is a SEPARATE generated function in the binary

template <typename T>
T square(T input)
{
    return input * input;
}

// ============================================================
// SECTION 2: auto PARAMETER - C++20 ABBREVIATED FUNCTION TEMPLATE
// ============================================================

auto square2(auto input)
//   ^        ^
//   return   parameter type deduced automatically
//   type     'auto' here = "accept any type"
//   also
//   deduced
{
    return input * input;
}
// YOUR COMMENT: "auto one is C++20 feature"
// CORRECT - abbreviated function templates added in C++20
//
// THIS IS SYNTACTIC SUGAR for:
// template<typename T>
// auto square2(T input) { return input * input; }
//
// COMPILER sees auto parameter -> AUTOMATICALLY creates a template
// -> same code generation, same behavior
// -> just shorter to write

// ============================================================
// SECTION 3: DIFFERENCE BETWEEN template<typename T> AND auto
// ============================================================

// SIMILARITY: both generate template code, both instantiated at compile time
// both produce same assembly for the same types
//
// DIFFERENCES:
//
// 1. SYNTAX:
//    template<typename T>          // classic (C++98, C++11)
//    T square(T input)
//
//    auto square2(auto input)      // abbreviated (C++20 only)
//
// 2. EXPLICIT INSTANTIATION:
//    square<int>(5.5)    // EXPLICIT: forces T=int, 5.5 converted to 5
//    square2<int>(5.5)   // ERROR! abbreviated templates cannot be
//                        // called with explicit <int> syntax easily
//                        // (no template parameter to refer to by name)
//
// 3. MULTIPLE PARAMETERS OF SAME TYPE:
//    template<typename T>
//    T add(T a, T b)      // BOTH a and b MUST be same type T
//    add(5, 5.5)          // ERROR: T can't be both int AND double
//
//    auto add2(auto a, auto b)  // a and b can be DIFFERENT types!
//    add2(5, 5.5)               // OK: a=int, b=double (two separate
//                               // hidden template params generated)
//    // equivalent to:
//    // template<typename T1, typename T2>
//    // auto add2(T1 a, T2 b) { return a + b; }
//
// 4. READABILITY:
//    template syntax: explicit, clear it's a template (pre-C++20 compatible)
//    auto syntax:     shorter, but less obvious it's a template

// ============================================================
// SECTION 4: auto RETURN TYPE - WHAT IT MEANS
// ============================================================

// auto square2(auto input) -> return type is also auto
//
// compiler DEDUCES the return type from the return statement
// return input * input;
// -> if input is int:    int*int    = int    -> return type deduced as int
// -> if input is double: double*double = double -> deduced as double
// -> if input is float:  float*float = float -> deduced as float
//
// SAME as writing: (since C++14)
// template<typename T>
// auto square(T input) -> decltype(input * input)
// { return input * input; }
//
// or simply (C++14 return type deduction):
// template<typename T>
// auto square(T input) { return input * input; }
// -> compiler deduces return type from return statement

// ============================================================
// SECTION 5: HISTORY OF TEMPLATES
// ============================================================

// YOUR COMMENT: "templates have been around before modern C++"
// CORRECT:
//
// C++98:  templates introduced (function and class templates)
// C++11:  variadic templates, template aliases, extern templates
// C++14:  variable templates, generic lambdas (auto params in lambdas)
// C++17:  class template argument deduction (CTAD), if constexpr
// C++20:  concepts, abbreviated function templates (auto params)
//         constraints, requires clauses
//
// "Modern C++" typically means C++11 and later
// Templates existed BEFORE modern C++ (since C++98)
// but have been ENHANCED significantly in modern versions

// ============================================================
// SECTION 6: typename vs class IN TEMPLATE PARAMETER LIST
// ============================================================

// YOUR COMMENT: "typename sometimes 'class'"
//
// template<typename T>  // preferred in modern C++
// template<class T>     // older style, identical meaning here
//
// WHEN 'class' is required (edge case):
// template<template<class> class Container>  // template template parameter
//          ^^^^^^^^^^^^^^^^
//          here 'class' is required, 'typename' might not work
//          (compiler-dependent, C++17 allows typename here too)
//
// FOR NORMAL USE: always prefer typename (clearer intent)
// T can be int, float, string, your custom class
// writing 'class T' misleadingly suggests T must be a class type

// ============================================================
// SECTION 7: INSTANTIATION - YOUR COMMENT EXPLAINED
// ============================================================

// "instantiation = generation of our overloaded function"
// PERFECT description!
//
// square<int>(5)
// -> EXPLICIT instantiation
// -> compiler generates: int square(int input) { return input*input; }
// -> this generated function is compiled and put in the binary
//
// square(5)
// -> IMPLICIT instantiation (type deduction)
// -> compiler sees 5 is int -> T=int
// -> same generated function as above
//
// square<double>(5)
// -> EXPLICIT instantiation with different type
// -> compiler generates: double square(double i) { return i*i; }
// -> DIFFERENT generated function in binary
//
// square2(5.5)
// -> auto parameter: input deduced as double
// -> compiler generates: double square2(double i) { return i*i; }

// ============================================================
// SECTION 8: CONCEPTS (C++20) - CONSTRAINING TEMPLATES
// ============================================================

// Problem: what if someone calls square with a type that doesn't
// support * operator?
//
// struct Foo { };
// square(Foo{});  // ERROR but the error message is TERRIBLE
//                 // "binary operator* not found for Foo"
//                 // deep inside template code -> hard to understand
//
// C++20 CONCEPTS = constraints on template parameters:
//
// template<typename T>
// requires std::is_arithmetic_v<T>  // T must be a number type
// T square(T input) { return input * input; }
//
// OR with abbreviated syntax:
// auto square(std::integral auto input)  // input must be an integer type
//
// square(Foo{});  // NOW: clear error at the call site
//                 // "Foo does not satisfy constraint is_arithmetic"
//
// BUILT-IN CONCEPTS (from <concepts> header):
// std::integral        -> int, long, short etc.
// std::floating_point  -> float, double etc.
// std::same_as<T>      -> exactly type T
// std::derived_from<T> -> derived from T
// std::convertible_to<T> -> convertible to T

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a function template?
//     -> A blueprint that tells the compiler how to generate a function
//        for different types. The compiler instantiates (generates) a
//        specific version for each type used. Avoids code duplication.

// Q2: What is template instantiation?
//     -> The process of the compiler generating a specific function
//        from a template for a particular type. Each unique type
//        argument produces a separate compiled function in the binary.

// Q3: What is the difference between template<typename T> and auto parameter?
//     -> Both generate templates. Classic template: explicit T name,
//        can use explicit <Type> syntax, enforces same type for multiple
//        params of type T. auto parameter (C++20): shorter syntax,
//        each auto param is an independent hidden template parameter,
//        allows different types for different auto params.

// Q4: Can auto parameter functions be called with explicit type?
//     -> Not easily - abbreviated templates have no named parameter
//        to specify explicitly. Classic templates allow square<int>(5.5)
//        to force a specific instantiation.

// Q5: What does auto return type mean in a template function?
//     -> Compiler deduces return type from the return statement.
//        auto square(auto input) { return input*input; }
//        If input is double, return type deduced as double.
//        Same effect as trailing return type with decltype.

// Q6: When did function templates first appear in C++?
//     -> C++98 (before "modern C++"). Modern C++ (C++11+) added
//        improvements like variadic templates, but the core template
//        mechanism has existed since the beginning.

// Q7: What are concepts in C++20 and why are they useful?
//     -> Constraints on template parameters that make error messages
//        clearer and document intent. requires std::integral<T>
//        ensures T is an integer type, giving clear errors at the
//        call site instead of deep inside template code.

// Q8: What is the difference between template<typename T> T add(T a, T b)
//     and auto add(auto a, auto b)?
//     -> Classic: a and b must be the SAME type T. add(5, 5.5) ERROR.
//        auto:    a and b can be DIFFERENT types (two hidden params).
//                 add(5, 5.5) OK: a=int, b=double, return=double.
// ```

// **The key difference - same type vs different types:**

// ```cpp
// // CLASSIC template: a and b MUST be same type
// template<typename T>
// T add(T a, T b) { return a + b; }
// add(5, 5.5);    // ERROR: T can't be int AND double simultaneously

// // auto parameter: each auto = independent hidden template param
// auto add(auto a, auto b) { return a + b; }
// add(5, 5.5);    // OK: a=int, b=double (two separate type params)
// // equivalent to:
// // template<typename T1, typename T2>
// // auto add(T1 a, T2 b) { return a + b; }
// ```

// **Evolution of "generic function" syntax:**

// ```cpp
// // C++98:  template<typename T> T square(T i) { return i*i; }
// // C++14:  template<typename T> auto square(T i) { return i*i; }
// // C++20:  auto square(auto i) { return i*i; }  ← your square2
// // All generate the same code - just increasingly shorter to write
// ```

// > **The one-liner**: `auto` parameters in C++20 are **syntactic sugar for templates** — the compiler creates a hidden template parameter for each `auto`. The difference from classic templates is that each `auto` is its **own independent type**, while `template<typename T>` forces all `T` parameters to be the **same type**.