// ============================================================
// MULTIPLE TEMPLATE PARAMETERS, NON-TYPE TEMPLATE PARAMS - NOTES
// ============================================================

#include <iostream>
#include <typeinfo>

// ============================================================
// SECTION 1: MULTIPLE TEMPLATE PARAMETERS
// ============================================================

// Just like functions can have multiple parameters,
// templates can have multiple template parameters:
//
// template<typename T1, typename T2>
// -> T1 and T2 are INDEPENDENT type parameters
// -> can be same type or different types
// -> compiler deduces or you specify explicitly
//
// YOUR COMMENTED CODE:
// template<typename T1, typename T2>
// void foo(T1 input1, T2 input2) {
//     std::cout << typeid(input1).name() << std::endl;
//     std::cout << typeid(input2).name() << std::endl;
// }
// foo(5, 5.5)       -> T1=int, T2=double (different types, both OK)
// foo(5, "hello")   -> T1=int, T2=const char* (completely different)
// foo<int,float>(5, 5.5) -> explicit: T1=int, T2=float

// ============================================================
// SECTION 2: NON-TYPE TEMPLATE PARAMETER - THE KEY NEW CONCEPT
// ============================================================

// YOUR CODE: template<typename T1, size_t N>
//                      ^^^^^^^^^  ^^^^^^
//                      TYPE param NON-TYPE param
//
// NON-TYPE template parameter = a VALUE (not a type) baked into the template
//
// size_t N = N is an integer VALUE known at COMPILE TIME
// -> NOT a variable (not stored at runtime)
// -> part of the TEMPLATE itself (like a compile-time constant)
// -> DIFFERENT values of N = DIFFERENT template instantiations
//
// foo<int, 3>(5)   -> N=3  -> compiler generates: loop runs 3 times
// foo<float, 4>(5) -> N=4  -> compiler generates: loop runs 4 times
// foo<int, 5>(5)   -> N=5  -> compiler generates: loop runs 5 times
//
// EACH is a COMPLETELY SEPARATE generated function!
// foo<int,3>  is a different function from foo<int,5>
// even though T1 is the same!

template <typename T1, size_t N>
//         ^^^^^^^^^   ^^^^^^
//         type param  NON-TYPE param (VALUE, not a type)
//                     size_t = unsigned integer type (good for sizes/counts)
void foo(T1 input1)
//        ^
//        uses T1 (the type param)
//        N is used INSIDE the body but NOT as a function parameter
//        N is a compile-time constant inside this template
{
    for (size_t i = 0; i < N; i++)
    //                    ^
    //                    N is used as the loop bound
    //                    compiler BAKES IN the actual value
    //                    for N=3: generates "i < 3" in assembly
    //                    for N=5: generates "i < 5" in assembly
    {
        std::cout << typeid(input1).name() << std::endl;
        // typeid(input1).name() = runtime type name string
        // gcc/clang: "i" for int, "f" for float, "d" for double
        // MSVC: "int", "float", "double" (more readable)
    }
}

// ============================================================
// SECTION 3: WHAT COMPILER GENERATES - INSTANTIATION PROOF
// ============================================================

// foo<int, 3>(5) generates:
// void foo_int_3(int input1) {
//     for (size_t i = 0; i < 3; i++) {  // 3 hardcoded!
//         std::cout << typeid(input1).name();
//     }
// }
//
// foo<float, 4>(5) generates:
// void foo_float_4(float input1) {
//     for (size_t i = 0; i < 4; i++) {  // 4 hardcoded!
//         std::cout << typeid(input1).name();
//     }
// }
//
// foo<int, 5>(5) generates:
// void foo_int_5(int input1) {
//     for (size_t i = 0; i < 5; i++) {  // 5 hardcoded!
//         std::cout << typeid(input1).name();
//     }
// }
//
// THREE separate functions generated, even though logic is same
// This is the trade-off: compile-time flexibility vs binary size

// ============================================================
// SECTION 4: main() TRACE - COMPLETE OUTPUT
// ============================================================

int main()
{
    foo<int, 3>(5);
    // T1=int, N=3
    // input1 = 5 (int)
    // loop runs 3 times
    // typeid(int).name() = "i" (gcc) or "int" (MSVC)
    // OUTPUT (gcc): i
    //               i
    //               i

    foo<float, 4>(5);
    // T1=float, N=4
    // input1 = 5 converted to float (5.0f)
    // loop runs 4 times
    // typeid(float).name() = "f" (gcc) or "float" (MSVC)
    // OUTPUT (gcc): f
    //               f
    //               f
    //               f

    foo<int, 5>(5);
    // T1=int, N=5
    // input1 = 5 (int)
    // loop runs 5 times
    // typeid(int).name() = "i" (gcc)
    // OUTPUT (gcc): i
    //               i
    //               i
    //               i
    //               i

    return 0;
}

// FULL OUTPUT (gcc/clang):
// i    <- foo<int,3> iteration 1
// i    <- foo<int,3> iteration 2
// i    <- foo<int,3> iteration 3
// f    <- foo<float,4> iteration 1
// f    <- foo<float,4> iteration 2
// f    <- foo<float,4> iteration 3
// f    <- foo<float,4> iteration 4
// i    <- foo<int,5> iteration 1
// i    <- foo<int,5> iteration 2
// i    <- foo<int,5> iteration 3
// i    <- foo<int,5> iteration 4
// i    <- foo<int,5> iteration 5

// ============================================================
// SECTION 5: REAL WORLD USE - std::array IS THIS EXACT PATTERN
// ============================================================

// std::array<int, 10> arr;
// INTERNALLY looks like:
// template<typename T, size_t N>
// class array {
//     T data[N];   // N used as compile-time array size!
// public:
//     size_t size() { return N; }
//     T& operator[](size_t i) { return data[i]; }
// };
//
// std::array<int, 10>  -> T=int,   N=10
// std::array<float, 5> -> T=float, N=5
//
// N is baked into the type at compile time
// -> no runtime overhead (unlike std::vector which stores size separately)
// -> size() always returns compile-time constant
// -> THIS is exactly why std::array<int,10> and std::array<int,5>
//    are DIFFERENT TYPES (different N = different instantiation)

// ============================================================
// SECTION 6: NON-TYPE PARAMETER TYPES ALLOWED
// ============================================================

// C++20 expanded this significantly, but traditionally:
//
// ALLOWED non-type template params:
// int, size_t, long, short, char, bool    (integer types)
// pointers, references (to objects/functions)
// enum values
//
// NOT ALLOWED (traditionally):
// float, double (floating point - added in C++20!)
// std::string
// most class types (some exceptions in C++20)
//
// EXAMPLES:
// template<int N>           // integer
// template<size_t N>        // unsigned integer (most common for sizes)
// template<bool Flag>       // boolean
// template<char C>          // character
// template<int* Ptr>        // pointer
//
// C++20 added:
// template<double D>        // now allowed!
// template<auto N>          // deduce non-type param type automatically

// ============================================================
// SECTION 7: typeid - HOW IT WORKS
// ============================================================

// typeid(x).name()
// -> returns a C-string representing the TYPE of x
// -> from <typeinfo> header
// -> evaluated at RUNTIME (unlike sizeof which is compile-time)
//
// OUTPUT VARIES BY COMPILER:
// gcc/clang (mangled names):
// int           -> "i"
// float         -> "f"
// double        -> "d"
// unsigned int  -> "j"
// const char*   -> "PKc"
//
// MSVC (readable names):
// int           -> "int"
// float         -> "float"
// double        -> "double"
//
// To get human-readable names on gcc/clang:
// #include <cxxabi.h>
// abi::__cxa_demangle(typeid(x).name(), ...)
//
// ALTERNATIVE (compile-time, always readable - C++20):
// std::cout << typeid(input1).name();  // runtime
// // vs
// // no equivalent in standard C++ before C++23

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a non-type template parameter?
//     -> A template parameter that is a VALUE (not a type).
//        Specified as: template<typename T, size_t N>
//        N is a compile-time constant baked into the generated code.
//        Different values of N produce different instantiations.

// Q2: What types can be used as non-type template parameters?
//     -> Traditional: integer types (int, size_t, bool, char),
//        pointers, references, enums.
//        C++20 added: floating point, some class types.
//        NOT allowed traditionally: float, string, most class types.

// Q3: Why is std::array<int,10> a different type from std::array<int,5>?
//     -> N (10 vs 5) is a non-type template parameter.
//        Each different N value = separate template instantiation =
//        completely different type. They are not interchangeable.

// Q4: What does typeid(x).name() return?
//     -> A compiler-dependent string representing x's type.
//        gcc/clang: mangled ("i" for int, "f" for float).
//        MSVC: readable ("int", "float").
//        From <typeinfo> header, evaluated at runtime.

// Q5: How many separate functions does the compiler generate for
//     foo<int,3>(5), foo<float,4>(5), foo<int,5>(5)?
//     -> THREE separate functions:
//        foo_int_3, foo_float_4, foo_int_5 (conceptually).
//        Even foo<int,3> and foo<int,5> are different (same T, different N).

// Q6: What is the difference between T1 and N in template<typename T1, size_t N>?
//     -> T1 is a TYPE parameter (any type: int, float, custom class).
//        N is a NON-TYPE (VALUE) parameter (a compile-time integer).
//        T1 used as type for variables. N used as a compile-time constant.

// Q7: Can N be a runtime variable?
//     -> NO. Template parameters must be compile-time constants.
//        foo<int, n>(5) where n is a runtime variable = COMPILER ERROR.
//        constexpr int n = 3; foo<int, n>(5); -> OK (constexpr is compile-time).

// Q8: What is the real-world benefit of non-type template parameters?
//     -> Bakes sizes/counts into the TYPE itself.
//        std::array<int,10>: size known at compile time, no runtime overhead.
//        Enables compile-time checks (array bounds), zero-cost abstractions.
//        Used in matrix libraries: Matrix<float,4,4> for 4x4 float matrix.
// ```

// **Non-type template parameter in one picture:**

// ```cpp
// template<typename T1, size_t N>
// //        TYPE param  VALUE param (compile-time constant)

// foo<int, 3>(5)   // T1=int,   N=3  → generates loop with "i < 3" hardcoded
// foo<int, 5>(5)   // T1=int,   N=5  → generates loop with "i < 5" hardcoded
// //  same T1 ↑    different N ↑  = COMPLETELY DIFFERENT generated functions!
// ```

// **The `std::array` connection:**

// ```cpp
// std::array<int, 10>  // template<typename T, size_t N> with T=int,  N=10
// std::array<int,  5>  // template<typename T, size_t N> with T=int,  N=5
// // These are DIFFERENT TYPES - you already use non-type templates every day!
// ```

// > **Interview key point**: when `N` is a non-type template parameter, the compiler **bakes the value into the generated code** — there's no runtime variable storing `3` or `5`. The loop bound is literally a constant in the assembly. This is why `std::array::size()` returns a compile-time constant with zero overhead, unlike `std::vector::size()` which reads a stored runtime value.