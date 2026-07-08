// ============================================================
// AUTO RETURN TYPE, DECLTYPE, TRAILING RETURN - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: THE PROBLEM - WHY NOT JUST auto?
// ============================================================

// YOUR QUESTION: "why use decltype when we have auto?"
//
// FIRST: auto alone WORKS in C++14 and later:
// auto Multiply(const T1& a, const T2& b)
// {
//     return a * b;  // compiler deduces return type from return statement
// }
// -> completely valid in C++14+
// -> compiler looks at "a * b" and deduces the type
//
// SO why bother with -> decltype(a*b) at all?
//
// REASON 1: C++11 COMPATIBILITY
// -> auto return type deduction (without trailing return) = C++14 feature
// -> auto + -> decltype() trailing return = C++11 feature
// -> if you need C++11 compatibility: must use trailing return
// -> your code uses the C++11 style
//
// REASON 2: DOCUMENTATION AND EXPLICITNESS
// -> auto alone: reader must infer what type comes back
// -> -> decltype(a*b): EXPLICITLY shows "return type is whatever a*b produces"
// -> self-documenting: the expression in decltype IS the logic
//
// REASON 3: DECLARATIONS IN HEADER FILES
// -> in a header with only declaration (no body):
// auto Multiply(const T1&, const T2&);           // ERROR! no body to deduce from
// auto Multiply(const T1&, const T2&) -> decltype(a*b); // OK! explicit type
//
// REASON 4: COMPLEX CASES WHERE auto ALONE IS AMBIGUOUS
// -> multiple return paths with different types
// -> SFINAE (substitution failure) in advanced template code
// -> decltype makes the type contract crystal clear

// ============================================================
// SECTION 2: ANATOMY OF THE FUNCTION
// ============================================================

template <typename T1, typename T2>
//         ^^^^^^^^^   ^^^^^^^^^
//         two independent type parameters
//         T1 and T2 can be DIFFERENT types
//         this is the KEY over single-typename templates

auto Multiply(const T1& a, const T2& b) -> decltype(a * b)
// ^                                       ^^^^^^^^^^^^^^^^
// |                                       TRAILING RETURN TYPE
// auto = "return type specified after ->"  "return type = type of expression a*b"
// placeholder
//
// const T1& a  = const reference (no copy, cannot modify)
// const T2& b  = const reference (no copy, cannot modify)
// -> efficient for large objects too (matrix, vector etc.)
{
    return a * b;
    // actual multiplication
    // type of result = whatever T1 * T2 produces
    // for float * int: result is float (int promoted to float first)
}

// ============================================================
// SECTION 3: WHY TRAILING RETURN TYPE IS NEEDED HERE
// ============================================================

// ATTEMPT WITHOUT trailing return:
// T1 Multiply(const T1& a, const T2& b)   // WRONG for float*int!
// ^
// return type = T1 = float
// but float * int = float (OK here, but what about int * double?)
// int Multiply with double * int -> would return int (loses decimals!)
//
// ATTEMPT WITH T2:
// T2 Multiply(const T1& a, const T2& b)   // also WRONG sometimes
// ^
// return type = T2 = int
// float * int = float, but returning int = truncation!
//
// THE REAL PROBLEM:
// float * int   -> result is float   (int promoted to float)
// int   * float -> result is float   (int promoted to float)
// int   * double-> result is double  (int promoted to double)
// float * double-> result is double  (float promoted to double)
//
// The return type DEPENDS ON THE COMBINATION of T1 and T2!
// You cannot just say "T1" or "T2" - neither is always correct
//
// SOLUTION: decltype(a * b) = "whatever type a*b actually produces"
// -> compiler computes the correct return type based on promotion rules
// -> float * int -> decltype = float -> correct!
// -> int * double -> decltype = double -> correct!
// -> This is called ARITHMETIC TYPE PROMOTION

// ============================================================
// SECTION 4: DECLTYPE - WHAT IS IT?
// ============================================================

// decltype(expression) = "give me the TYPE of this expression"
// evaluated at COMPILE TIME (not runtime)
// does NOT execute the expression (just inspects its type)
//
// EXAMPLES:
// int x = 5;
// float y = 3.14f;
// decltype(x + y)    = float  (int + float = float via promotion)
// decltype(x * x)    = int    (int * int = int)
// decltype(y * y)    = float  (float * float = float)
// decltype(x)        = int    (type of x is int)
//
// IN YOUR CODE:
// decltype(a * b) where a=float, b=int
// = decltype(float * int)
// = float  (int promoted to float in arithmetic)
//
// IMPORTANT: a and b don't actually multiply!
// decltype just asks: "IF we multiplied them, what TYPE would result?"
// purely a compile-time type inspection

// ============================================================
// SECTION 5: YOUR CODE TRACED
// ============================================================

int main()
{
    std::cout << Multiply<float, int>(7.22f, 5) << std::endl;
    // T1=float, T2=int
    // a = 7.22f (const float&)
    // b = 5     (const int&)
    //
    // RETURN TYPE:
    // decltype(a * b) = decltype(float * int) = float
    // (int 5 promoted to float 5.0f before multiplication)
    //
    // COMPUTATION:
    // a * b = 7.22f * 5 = 36.1f
    //
    // OUTPUT: 36.1

    return 0;
}

// ============================================================
// SECTION 6: YOUR COMMENT - "WHAT IF WE HAVE A MATRIX?"
// ============================================================

// This is the REAL power of this pattern!
//
// struct Matrix { float data[4][4]; };
// Matrix operator*(const Matrix& a, const Matrix& b) { ... }
//
// template<typename T1, typename T2>
// auto Multiply(const T1& a, const T2& b) -> decltype(a * b)
// {
//     return a * b;
// }
//
// Multiply(matrix1, matrix2)
// -> T1=Matrix, T2=Matrix
// -> decltype(matrix1 * matrix2) = Matrix (uses our operator*)
// -> returns a Matrix!
//
// Multiply(scalar, matrix)  // if scalar*matrix is defined
// -> T1=float, T2=Matrix
// -> decltype(float * Matrix) = Matrix
// -> returns scaled Matrix!
//
// THE SAME TEMPLATE WORKS FOR ALL OF THESE:
// Multiply(5, 3)           -> int
// Multiply(5.0f, 3)        -> float
// Multiply(5.0, 3)         -> double
// Multiply(matrix, matrix) -> Matrix
// Multiply(5.0f, matrix)   -> whatever float*Matrix returns
// -> ONE template handles ALL without modification!

// ============================================================
// SECTION 7: std::declval - WHEN OBJECTS CAN'T BE DEFAULT CONSTRUCTED
// ============================================================

// PROBLEM: what if T1 has no default constructor?
// decltype(T1() * T2())  <- tries to DEFAULT CONSTRUCT T1 and T2
//                           ERROR if they have no default constructor!
//
// SOLUTION: std::declval<T>()
// -> creates a "fake" value of type T for decltype purposes
// -> never actually constructed at runtime
// -> purely a compile-time trick
//
// template<typename T1, typename T2>
// auto Multiply(const T1& a, const T2& b)
//     -> decltype(std::declval<T1>() * std::declval<T2>())
// {
//     return a * b;
// }
// -> works even if T1 and T2 have no default constructor
// -> your code uses a and b directly (fine when they exist as params)

// ============================================================
// SECTION 8: THREE WAYS TO WRITE THIS - COMPARISON
// ============================================================

// WAY 1: Trailing return with decltype (C++11 - your code)
// auto Multiply(const T1& a, const T2& b) -> decltype(a*b)
// { return a*b; }
// PRO: C++11 compatible, explicit type contract
// CON: verbose

// WAY 2: auto return deduction (C++14)
// auto Multiply(const T1& a, const T2& b)
// { return a*b; }
// PRO: clean, simple
// CON: C++14+ only, less explicit about type
// NOTE: In C++14+, this is usually preferred over WAY 1

// WAY 3: trailing return with decltype on expressions (C++11)
// auto Multiply(const T1& a, const T2& b)
//     -> decltype(std::declval<T1>() * std::declval<T2>())
// { return a*b; }
// PRO: works in declarations, most explicit
// CON: most verbose

// IN PRACTICE (modern C++): WAY 2 is preferred for simplicity
// WAY 1 is common in C++11 codebases and tutorials

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does auto -> decltype(expr) mean as a return type?
//     -> Trailing return type syntax. auto = placeholder, actual
//        return type is specified after ->. decltype(expr) evaluates
//        the type of the expression at compile time without executing it.

// Q2: Why use decltype(a*b) instead of just T1 or T2?
//     -> T1 and T2 alone can't express the RESULT type of mixed-type
//        arithmetic. float*int = float (not int). The correct return type
//        depends on arithmetic promotion rules, which decltype captures.

// Q3: What is the difference between auto return type and -> decltype?
//     -> auto alone (C++14): deduces from return statement, simpler.
//        -> decltype (C++11): explicit type contract, works in declarations
//        without body, C++11 compatible, more self-documenting.

// Q4: Does decltype execute the expression?
//     -> NO. decltype is purely compile-time type inspection.
//        decltype(a*b) doesn't actually multiply a and b.
//        It only inspects what TYPE that multiplication would produce.

// Q5: What is arithmetic type promotion?
//     -> Rules for converting types in mixed arithmetic:
//        int + float -> float (int promoted)
//        float + double -> double (float promoted)
//        Always promotes to the "larger/more precise" type.
//        decltype(a*b) correctly captures the promoted result type.

// Q6: Why pass by const reference (const T1& a) instead of by value?
//     -> Avoids copying large objects (matrices, vectors).
//        const = promise not to modify. & = no copy made.
//        Efficient for any size object. Safe (cannot change inputs).

// Q7: What is std::declval and when is it needed?
//     -> Creates a fake value of type T for use inside decltype.
//        Used when T has no default constructor.
//        decltype(std::declval<T1>() * std::declval<T2>()) works
//        even if T1 and T2 can't be default-constructed.

// Q8: How does this pattern work for Matrix multiplication?
//     -> template<typename T1, typename T2>
//        auto Multiply(const T1& a, const T2& b) -> decltype(a*b)
//        If Matrix has operator*, then Multiply(m1, m2) automatically
//        works and returns Matrix. Same template handles scalars AND
//        matrices without any modification.
// ```

// **The core reason for `decltype` - arithmetic promotion:**

// ```cpp
// // You cannot just pick T1 or T2 as return type:
// float  * int    = float   ← not int!
// int    * double = double  ← not int!
// float  * double = double  ← not float!

// // decltype(a * b) = "whatever THIS specific combination produces"
// // compiler applies promotion rules at compile time
// // -> always correct, always the right type
// ```

// **Three ways compared:**

// ```cpp
// // C++11 (your code) - explicit:
// auto Multiply(const T1& a, const T2& b) -> decltype(a*b)

// // C++14 - simpler, same result:
// auto Multiply(const T1& a, const T2& b)  // deduced from return statement

// // Header declaration only (must use trailing):
// auto Multiply(const T1& a, const T2& b) -> decltype(a*b); // no body needed
// ```

// > **The matrix comment is the key insight**: this pattern isn't just for `int` and `float`. The same `Multiply` template with `decltype` works for **any two types that support `*`** — scalars, matrices, vectors, complex numbers — without writing a single additional line of code. That's the real power of combining templates with `decltype`.