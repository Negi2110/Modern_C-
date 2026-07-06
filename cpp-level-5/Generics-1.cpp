// ============================================================
// TEMPLATES / GENERICS IN C++ - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS A TEMPLATE AND WHY?
// ============================================================

// WITHOUT templates - the COPY-PASTE problem:
// int    square(int input)    { return input * input; }
// double square(double input) { return input * input; }
// float  square(float input)  { return input * input; }
//
// SAME LOGIC repeated 3 times!
// -> if logic changes (e.g., input * input * 2), update ALL THREE
// -> easy to miss one -> bugs
// -> add long long? write ANOTHER function
// -> DRY violation
//
// YOUR COMMENT IS EXACT:
// "templates avoid copy and paste"
// "serve as a blueprint for compiler to generate code for different types"
//
// WITH templates - ONE definition, works for ANY type:
// template<typename T>
// T square(T input) { return input * input; }
//
// Compiler generates the SPECIFIC versions automatically:
// square<int>    -> generates: int    square(int i)    { return i*i; }
// square<double> -> generates: double square(double i) { return i*i; }
// square<float>  -> generates: float  square(float i)  { return i*i; }
// -> called TEMPLATE INSTANTIATION (happens at COMPILE TIME)

// ============================================================
// SECTION 2: TEMPLATE SYNTAX BREAKDOWN
// ============================================================

template <typename T>
//         ^^^^^^^  ^
//         keyword  template parameter name (placeholder type)
//                  can also write: template<class T> (identical)
//                  'T' is convention but can be anything:
//                  template<typename MyType>
//                  template<typename A, typename B>  (multiple params)

T square(T input)
// ^      ^ ^
// |      | parameter type = T (whatever caller passes)
// |      function name
// return type = T (same type as input)
{
    return input * input;
    // compiler generates the actual code here based on T
    // for T=int:    return input * input;  (int multiplication)
    // for T=double: return input * input;  (double multiplication)
}

// ============================================================
// SECTION 3: HOW COMPILER PROCESSES TEMPLATES - KEY INSIGHT
// ============================================================

// Templates are NOT compiled into code directly
// They are a BLUEPRINT/RECIPE for the compiler to GENERATE code
//
// STEP 1: You write the template
// STEP 2: Compiler sees: square<int>(5)
//         -> "I need to instantiate square with T=int"
//         -> generates int square(int input) { return input*input; }
//         -> compiles THAT specific version
// STEP 3: Compiler sees: square<double>(5.5)
//         -> generates double square(double input) { return input*input; }
//         -> compiles THAT version too
//
// FINAL BINARY contains:
// -> int    version of square (if used)
// -> double version of square (if used)
// -> float  version of square (if used)
// -> NOT a generic version! Each type = separate compiled function
//
// THIS IS WHY: templates must usually be in HEADER FILES (.hpp)
// -> compiler needs to SEE the template definition to generate code
// -> if template is in .cpp, other .cpp files can't generate instances
// -> unlike regular functions where declaration is enough

// ============================================================
// SECTION 4: YOUR CODE TRACED - EXPLICIT vs IMPLICIT
// ============================================================

int main()
{
    std::cout << square<int>(5) << std::endl;
    // EXPLICIT instantiation: <int> tells compiler exactly which type
    // T = int
    // generates: int square(int input) { return 5*5; }
    // result: 25

    std::cout << square<double>(5.5) << std::endl;
    // EXPLICIT instantiation: <double>
    // T = double
    // generates: double square(double input) { return 5.5*5.5; }
    // result: 30.25

    std::cout << square<int>(6.5f) << std::endl;
    // EXPLICIT instantiation: <int> forces T = int
    // 6.5f is float but T=int -> IMPLICIT CONVERSION: 6.5f -> 6
    // generates: int square(int input) { return 6*6; }
    // result: 36  (NOT 42.25! float truncated to int)
    //
    // YOUR COMMENT IS CORRECT:
    // "implicit conversion from float to int"
    // -> if you used {} instead: square<int>{6.5f} -> COMPILER ERROR
    //    (narrowing conversion refused by {})

    // IMPLICIT TYPE DEDUCTION (compiler figures out T automatically):
    // square(5)      -> T deduced as int    (5 is int literal)
    // square(5.5)    -> T deduced as double (5.5 is double literal)
    // square(5.5f)   -> T deduced as float  (5.5f is float literal)
    // No need to write <int> or <double> explicitly!
    // Compiler reads the argument type and deduces T

    return 0;
}

// ============================================================
// SECTION 5: CLASS TEMPLATES (not just function templates)
// ============================================================

// Templates work for CLASSES too:
//
// template<typename T>
// class Stack {
// public:
//     void push(T value) { data.push_back(value); }
//     T    pop()         { T v = data.back(); data.pop_back(); return v; }
//     bool empty()       { return data.empty(); }
// private:
//     std::vector<T> data;
// };
//
// Stack<int>    intStack;      // stack of ints
// Stack<string> stringStack;   // stack of strings
// Stack<float>  floatStack;    // stack of floats
//
// std::vector<T>, std::array<int,N> etc. are ALL class templates!
// That's why you write: std::vector<int>, std::array<int,5>
//                                    ^^^              ^^^
//                                    template argument

// ============================================================
// SECTION 6: MULTIPLE TEMPLATE PARAMETERS
// ============================================================

// template<typename A, typename B>
// A convert(B input) {
//     return static_cast<A>(input);
// }
//
// convert<int, double>(5.7)  -> int (5)
// convert<double, int>(5)    -> double (5.0)
//
// TRAILING RETURN TYPE with templates (from earlier session):
// template<typename A, typename B>
// auto multiply(A a, B b) -> decltype(a * b) {
//     return a * b;
// }
// multiply(3, 4.5)   -> double (int * double = double)
// multiply(3, 4)     -> int    (int * int = int)

// ============================================================
// SECTION 7: NON-TYPE TEMPLATE PARAMETERS
// ============================================================

// Templates can also take VALUES not just types:
//
// template<typename T, int SIZE>
// class Array {
//     T data[SIZE];
// public:
//     int size() { return SIZE; }
// };
//
// Array<int, 10>   myArray;    // array of 10 ints  (compile time!)
// Array<float, 5>  floatArr;   // array of 5 floats
//
// THIS IS EXACTLY std::array<int, 10>!
// std::array is a template with type AND size as parameters

// ============================================================
// SECTION 8: TEMPLATE SPECIALIZATION - OVERRIDING FOR SPECIFIC TYPE
// ============================================================

// Generic template:
// template<typename T>
// T square(T input) { return input * input; }
//
// Specialization for bool (different behavior needed):
// template<>
// bool square(bool input) { return input; }  // squaring a bool = itself
//
// When square<bool>(true) called:
// -> compiler uses SPECIALIZED version, not generic
// -> allows different implementation for specific types

// ============================================================
// SECTION 9: typename vs class IN TEMPLATE
// ============================================================

// template<typename T>  // modern, preferred
// template<class T>     // older, identical meaning here
//
// BOTH mean exactly the same thing in this context:
// T can be ANY type (int, double, string, your own class, etc.)
//
// 'class' is confusing because T can be int (not a class!)
// 'typename' is clearer: "T is a type name, could be anything"
// Modern C++ prefers typename

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a template in C++?
//     -> A blueprint that tells the compiler how to generate code
//        for different data types. Avoids copy-pasting the same
//        logic for each type. Instantiated at compile time.

// Q2: What is template instantiation?
//     -> When compiler generates a specific version of a template
//        for a given type argument. square<int> causes compiler to
//        generate int square(int input) { return input*input; }
//        Each unique type = separate generated function in binary.

// Q3: What is the difference between typename and class in templates?
//     -> Identical meaning in template parameter lists.
//        typename is preferred (clearer - T can be any type, not just class).

// Q4: Why must templates usually be defined in header files?
//     -> Compiler needs to see the full template definition to
//        instantiate it. If in .cpp, other .cpp files can't generate
//        their own instantiations. Unlike regular functions where
//        only the declaration is needed in the header.

// Q5: What is implicit type deduction in templates?
//     -> Compiler automatically deduces T from the argument type.
//        square(5) -> T=int, square(5.5) -> T=double.
//        No need to explicitly write square<int>(5).

// Q6: What is template specialization?
//     -> Providing a specific implementation for a particular type,
//        overriding the generic template for that type.
//        template<> bool square(bool b) { return b; }
//        Compiler uses specialized version when that exact type is used.

// Q7: What is square<int>(6.5f) and what is the result?
//     -> Explicit instantiation with T=int. 6.5f (float) implicitly
//        converted to int (6). Returns 6*6 = 36.
//        Data loss: 0.5 silently truncated.

// Q8: What is a non-type template parameter? Give an example.
//     -> A template parameter that is a VALUE, not a type.
//        template<typename T, int N> class Array { T data[N]; };
//        Array<int, 10> = array of 10 ints, size known at compile time.
//        std::array<int, 10> uses exactly this pattern.
// ```

// **Templates in one picture:**

// ```cpp
// // You write ONE blueprint:
// template<typename T>
// T square(T input) { return input * input; }

// // Compiler generates MULTIPLE specific functions:
// square<int>    → int    square(int i)    { return i*i; }  ← in binary
// square<double> → double square(double i) { return i*i; }  ← in binary
// square<float>  → float  square(float i)  { return i*i; }  ← in binary
// // Each called version gets its own compiled copy in the final exe
// ```

// **Explicit vs implicit instantiation:**

// ```cpp
// square<int>(5)      // EXPLICIT: you told compiler T=int
// square(5)           // IMPLICIT: compiler deduces T=int from argument
// square<int>(6.5f)   // EXPLICIT T=int + implicit conversion 6.5f→6
// ```

// > **The core interview answer**: templates are not compiled directly — they are **code generators**. The compiler uses your template as a blueprint and generates separate, fully-typed functions for each type you use. This is why templates give you **zero runtime overhead** — by the time the program runs, all the generic code has already been resolved into concrete typed functions.