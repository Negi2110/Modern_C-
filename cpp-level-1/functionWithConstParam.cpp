// ============================================================
// const - TWO USES, std::is_const, decltype - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <type_traits>  // for std::is_const_v, std::is_const
#include <vector>
#include <algorithm>

// ============================================================
// SECTION 1: TWO USES OF const (your comment is spot on)
// ============================================================

// USE 1: const variable
// const int x = 42;
// -> x cannot be modified after initialization
// -> must be initialized at declaration
// -> lives in main's stack, value locked forever

// USE 2: const parameter
// void foo(const int x)       // pass by const value
// void foo(const int& x)      // pass by const reference <- most important
// void foo(const std::vector<int>& x)  // real world usage
//
// const parameter = "I PROMISE not to modify what you give me"
// gives caller CONFIDENCE that their data is safe

// ============================================================
// SECTION 2: THE COMMENTED CODE EXPLAINED
// ============================================================

// #include <type_traits>  -> header for compile-time type checking utilities

// const int x = 42;

// std::is_const_v<int>          -> 0 (false) - plain int is NOT const
// std::is_const_v<const int>    -> 1 (true)  - const int IS const
// std::is_const<decltype(x)>::value -> 1 (true) - x is const int

// WHAT IS decltype?
// decltype(x) = "give me the EXACT TYPE of x at compile time"
// if x is const int -> decltype(x) is const int
// if x is int&      -> decltype(x) is int&
// if x is float     -> decltype(x) is float
//
// decltype does NOT evaluate x, just deduces its type
// purely a compile time tool

// WHAT IS std::is_const?
// compile time check: "is this type const?"
// std::is_const<const int>::value  -> true  (1)
// std::is_const<int>::value        -> false (0)
// std::is_const_v<T> is shorthand for std::is_const<T>::value (C++17)

// FULL EXAMPLE RESTORED:
// int main() {
//     const int x = 42;
//     std::cout << "x:" << x << std::endl;
//     // prints: x:42
//
//     std::cout << std::is_const_v<int> << std::endl;
//     // prints: 0  (int is not const)
//
//     std::cout << std::is_const_v<const int> << std::endl;
//     // prints: 1  (const int IS const)
//
//     std::cout << std::is_const<decltype(x)>::value << std::endl;
//     // decltype(x) = const int (exact type of x)
//     // is_const<const int>::value = 1
//     // prints: 1
//
//     // x = 37;  // COMPILER ERROR - x is const, cannot modify
// }

// ============================================================
// SECTION 3: passByref with const - THE SAFEST SIGNATURE
// ============================================================

void passByval(std::vector<int> arg)
{
    arg[0] = 1;
    // full copy made (expensive - 400KB for 100K ints)
    // modifies copy only, original safe
    // copy destroyed when function returns
}

void passByref(const std::vector<int>& alias)
{
    // alias[0] = 1;  // COMPILER ERROR if uncommented
    //                // const& = read-only reference
    //                // "I can SEE your data but I CANNOT touch it"
    //
    // NO copy made (efficient - only 8 bytes)
    // alias IS x but const prevents any modification
    // BEST of both worlds:
    // -> speed of reference (no copy)
    // -> safety of const (no mutation)
}

// ============================================================
// SECTION 4: THREE VERSIONS OF REFERENCE - SIDE BY SIDE
// ============================================================

// VERSION 1: non-const reference
// void f(std::vector<int>& v)
// -> no copy          (efficient)
// -> CAN modify       (dangerous if modification not intended)
// -> USE: when you NEED to mutate original

// VERSION 2: const reference  (your current code)
// void f(const std::vector<int>& v)
// -> no copy          (efficient)
// -> CANNOT modify    (safe, compiler enforced)
// -> USE: when you only need to READ large objects
// -> DEFAULT CHOICE for large objects in modern C++

// VERSION 3: pass by value
// void f(std::vector<int> v)
// -> full copy        (expensive for large objects)
// -> CAN modify copy  (original safe)
// -> USE: when you need a local modifiable copy
//         OR for small types (int, float, char)

// ============================================================
// SECTION 5: const CORRECTNESS - important concept
// ============================================================

// const correctness = using const wherever possible
// to communicate intent and prevent bugs
//
// WITHOUT const correctness:
// void print(std::vector<int>& v) {
//     v[0] = 999;  // accidental modification - bug!
//     for(auto x : v) cout << x;
// }
//
// WITH const correctness:
// void print(const std::vector<int>& v) {
//     v[0] = 999;  // COMPILER ERROR - caught immediately!
//     for(auto x : v) cout << x;
// }
//
// const is a CONTRACT between caller and function:
// caller:   "here is my data"
// function: "I promise const std::vector<int>& means I won't touch it"
// compiler: "I will enforce this promise"

// ============================================================
// SECTION 6: decltype vs auto - KEY DIFFERENCE
// ============================================================

// auto   -> deduces type BUT strips const and reference
//           const int x = 42;
//           auto y = x;        -> y is int (const stripped!)
//           y = 99;            -> VALID, y is not const

// decltype -> deduces EXACT type including const and reference
//             const int x = 42;
//             decltype(x) y = x; -> y is const int
//             y = 99;             -> ERROR, y is const

// INTERVIEW: What is the difference between auto and decltype?
//   -> auto strips qualifiers (const, &) during deduction
//      decltype preserves exact type including const and references

// ============================================================
// SECTION 7: std::is_const with references - TRICKY
// ============================================================

// const int x = 42;
// int& ref = const_cast<int&>(x);  // just for example
//
// std::is_const_v<const int>   -> 1  (true)
// std::is_const_v<const int&>  -> 0  (FALSE! - tricky!)
//
// WHY? const int& means "reference to const int"
// The REFERENCE itself is not const, the thing it refers to is
// To check: std::is_const_v<std::remove_reference_t<const int&>>
//        -> std::is_const_v<const int> -> 1 (true)
//
// INTERVIEW: is std::is_const_v<const int&> true or false?
//   -> FALSE. References are transparent to is_const.
//      Must remove reference first with std::remove_reference_t

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What are the two uses of const in C++?
//     -> 1. const variable: value locked after initialization
//        2. const parameter: function promises not to modify argument

// Q2: What is const reference and why is it the best way to pass large objects?
//     -> const T& : no copy (efficient) + cannot modify (safe)
//        Best of both worlds for read-only access to large data

// Q3: What is decltype?
//     -> Compile-time type deduction that preserves EXACT type
//        including const and reference qualifiers.
//        Unlike auto which strips them.

// Q4: What is std::is_const?
//     -> Compile-time type trait from <type_traits>
//        Checks if a type is const qualified.
//        std::is_const_v<T> is C++17 shorthand for ::value

// Q5: What is const correctness?
//     -> Practice of marking everything const that should not change.
//        Makes code self-documenting and catches bugs at compile time.
//        Expected in professional C++ code.

// Q6: What is the difference between const int* and int* const?
//     -> const int* p  : pointer to const int (data locked, ptr can move)
//        int* const p  : const pointer to int (ptr locked, data can change)
//        const int* const p : both locked

// Q7: Can you pass a const vector to a non-const reference parameter?
//     -> NO. const std::vector<int> x;
//        void f(std::vector<int>& v);
//        f(x);  // ERROR - drops const qualifier, compiler rejects

// Q8: What does std::is_const_v<const int&> return?
//     -> 0 (false). References are transparent to is_const.
//        Use std::remove_reference_t first to get correct result.

int main()
{
    std::vector<int> x(100000);
    std::fill(std::begin(x), std::end(x), 1);

    passByref(x);
    // x passed as const reference
    // alias[0]=1 inside is commented -> no modification
    // x remains {1,1,1,...1}
    // efficient: only 8 bytes passed, no 400KB copy

    return 0;
}
// ```

// **The single most important interview point here:**

// ```cpp
// void f(std::vector<int> v)         // copy    - expensive, safe
// void f(std::vector<int>& v)        // ref     - fast, can mutate
// void f(const std::vector<int>& v)  // constref- fast + safe <- DEFAULT CHOICE
// ```

// > `const&` is the **default** for any large object you only need to read.
// > It is the most common function signature you will write in real Qt/C++ code.
// > **No copy. No mutation. Compiler enforced.**