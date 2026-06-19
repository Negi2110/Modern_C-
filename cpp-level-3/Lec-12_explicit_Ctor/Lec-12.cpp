// ============================================================
// explicit, LIST INITIALIZATION, TYPE CONVERSION - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <string>

// ============================================================
// SECTION 1: THE PROBLEM - IMPLICIT CONVERSION
// ============================================================

// WITHOUT explicit keyword:
// class UDT {
//     UDT(int i) { m_variable = i; }
// };
//
// SILENT CONVERSIONS happen automatically:
//
// UDT u1 = 500;
// -> compiler sees: "need UDT, have int 500"
// -> finds UDT(int) constructor
// -> silently converts: UDT u1(500)
// -> WORKS but you never wrote UDT u1(500)!
//
// UDT u1 = 500.123f;
// -> compiler sees: "need UDT, have float"
// -> finds UDT(int) constructor
// -> silently converts float to int: 500.123f -> 500
// -> UDT u1(500) called
// -> 0.123f LOST silently!
// -> no warning, no error, data loss!
//
// UDT u1 = 'A';
// -> 'A' is char = 65 in ASCII
// -> silently converts to int: UDT u1(65)
// -> completely unintended behavior, no error!
//
// THESE ARE ALL SILENT BUGS!
// Compiler is "helpfully" converting types you never asked for

// ============================================================
// SECTION 2: explicit KEYWORD - THE FIX
// ============================================================

class UDT
{
public:
    explicit UDT(int i);
    //^
    // "you MUST explicitly construct me"
    // "no silent conversions allowed"
    // compiler will NOT automatically use this constructor
    // to convert other types to UDT

private:
    int m_variable;
};

UDT::UDT(int i)
{
    m_variable = i;
    std::cout << "m_variable: " << m_variable << std::endl;
}

// ============================================================
// SECTION 3: WHAT explicit PREVENTS vs ALLOWS
// ============================================================

// PREVENTED (compiler error):
// UDT u1 = 500;       // ERROR - implicit conversion blocked
// UDT u1 = 500.123f;  // ERROR - implicit conversion blocked
// UDT u1 = 'A';       // ERROR - implicit conversion blocked
//
// void foo(UDT u) { }
// foo(500);           // ERROR - implicit conversion blocked
//
// ALLOWED (explicit construction):
// UDT u1(500);        // OK - direct construction, you chose int
// UDT u1{500};        // OK - list initialization (see section 4)
// UDT u1 = UDT(500);  // OK - explicitly making UDT from int
// UDT u1 = (UDT)500;  // OK - explicit cast

// ============================================================
// SECTION 4: LIST INITIALIZATION {} - THE MODERN WAY
// ============================================================

// UDT u1{500};
//          ^
//          curly braces = LIST INITIALIZATION (C++11)
//
// WHY use {} instead of ()?
//
// BENEFIT 1: PREVENTS NARROWING CONVERSIONS
// UDT u1(500.123f);   // OK with () - float truncated to int silently
// UDT u1{500.123f};   // COMPILER ERROR - narrowing not allowed!
//                      // {} refuses to silently lose data
//
// BENEFIT 2: NO MOST VEXING PARSE PROBLEM
// UDT u1();           // IS THIS an object or a function declaration??
//                      // C++ says: function declaration! (confusing!)
// UDT u1{};           // ALWAYS an object with default constructor
//                      // no ambiguity
//
// BENEFIT 3: WORKS EVERYWHERE uniformly
// int x{5};                    // primitive
// std::string s{"hello"};      // string
// std::vector<int> v{1,2,3};   // vector with values
// UDT u{500};                  // your class
//
// NARROWING CONVERSION:
// any conversion that LOSES DATA is narrowing:
// double -> float  (may lose precision)
// float  -> int    (loses decimal part)
// int    -> char   (may lose value if > 127)
// int    -> bool   (loses all info except 0/non-0)
//
// () allows narrowing silently
// {} REFUSES narrowing with compiler error
// {} is SAFER by default

// ============================================================
// SECTION 5: TYPE CONVERSION - COMPLETE PICTURE
// ============================================================

// C++ has FOUR types of conversion:

// TYPE 1: IMPLICIT CONVERSION (automatic, silent)
// -> compiler does it without you asking
// -> can be dangerous (data loss possible)
//
// int i = 5;
// double d = i;     // int -> double (safe, widening)
// float f = 3.14;   // double -> float (unsafe! narrowing, data loss)
// int x = 3.99;     // double -> int (unsafe! 3.99 -> 3, loss)
// bool b = 42;      // int -> bool (42 -> true)
// char c = 65;      // int -> char ('A')

// TYPE 2: EXPLICIT CAST - C++ style (recommended)
// static_cast<>      -> safe compile-time cast
// dynamic_cast<>     -> safe runtime cast (polymorphism)
// const_cast<>       -> remove/add const
// reinterpret_cast<> -> dangerous low-level bit reinterpret
//
// int i = static_cast<int>(3.99);     // explicit, you know you lose .99
// float f = static_cast<float>(5);    // explicit widening
// double d = static_cast<double>(i);  // explicit widening

// TYPE 3: C-STYLE CAST (avoid in modern C++)
// int i = (int)3.99;   // works but dangerous
//                       // compiler doesn't check safety
//                       // can accidentally reinterpret memory
//                       // use static_cast instead

// TYPE 4: USER-DEFINED CONVERSION (your class)
// constructor conversion:  UDT u = 500;  (implicit, unless explicit)
// conversion operator:     operator int() { return m_variable; }
//                          (converts YOUR type TO another type)

// ============================================================
// SECTION 6: CONVERSION OPERATORS - THE OTHER DIRECTION
// ============================================================

// Your UDT constructor converts INT -> UDT
// Conversion operator converts UDT -> something else:
//
// class UDT {
// public:
//     explicit UDT(int i) : m_variable(i) { }
//
//     // conversion operator: UDT -> int
//     explicit operator int() const {
//         return m_variable;
//     }
//
//     // conversion operator: UDT -> string
//     explicit operator std::string() const {
//         return std::to_string(m_variable);
//     }
// };
//
// UDT u{500};
// int x = static_cast<int>(u);         // explicit: 500
// std::string s = static_cast<std::string>(u); // explicit: "500"
//
// WITHOUT explicit on conversion operator:
// int x = u;   // implicit! silently converts UDT to int
// if (u) { }   // UDT silently converted to bool! dangerous!

// ============================================================
// SECTION 7: WIDENING vs NARROWING - SAFE vs UNSAFE
// ============================================================

// WIDENING (safe, no data loss):
// char   -> short -> int -> long -> long long
// float  -> double -> long double
// int    -> double (int fits in double exactly)
//
// NARROWING (unsafe, potential data loss):
// double -> float  (precision loss)
// double -> int    (decimal lost: 3.99 -> 3)
// int    -> char   (value loss if > 127)
// int    -> bool   (only 0/1 preserved)
//
// {} REFUSES narrowing -> compiler error -> forces you to be explicit
// () ALLOWS narrowing  -> silent conversion -> potential bugs

// ============================================================
// SECTION 8: REAL WORLD - WHERE explicit MATTERS MOST
// ============================================================

// std::string example (NO explicit - causes confusion):
// std::string s = 'A';   // COMPILER ERROR (can't convert char)
// std::string s = 65;    // COMPILER ERROR (can't convert int)
// BUT: std::string s(5, 'A'); // "AAAAA" - 5 copies of 'A'
//
// std::vector example:
// std::vector<int> v(10);    // vector with 10 elements (all 0)
// std::vector<int> v = 10;   // ERROR - explicit prevents this confusion
//
// YOUR class - why explicit matters:
// void processUDT(UDT u) { }
//
// WITHOUT explicit:
// processUDT(42);      // silently creates UDT(42) - confusing!
// processUDT(3.14f);   // silently truncates to UDT(3) - bug!
//
// WITH explicit:
// processUDT(42);      // ERROR - must be explicit
// processUDT(UDT{42}); // OK - clear intent

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does the explicit keyword do?
//     -> Prevents implicit (automatic) conversions using that constructor.
//        Constructor can only be called with direct/explicit syntax.
//        Eliminates silent data loss from unexpected conversions.

// Q2: What is list initialization {} and why prefer it over ()?
//     -> Curly brace initialization (C++11).
//        Prevents narrowing conversions (compiler error if data would be lost).
//        Eliminates most vexing parse ambiguity.
//        Uniform syntax works for all types.

// Q3: What is a narrowing conversion?
//     -> Conversion that potentially loses data.
//        double->int (loses decimals), float->char (loses range).
//        {} refuses narrowing. () allows it silently.

// Q4: What is the difference between () and {} initialization?
//     -> (): allows narrowing conversions silently
//            UDT u(500.5f) -> truncates to 500, no warning
//        {}: refuses narrowing, compiler error
//            UDT u{500.5f} -> COMPILER ERROR, must be exact type

// Q5: When should you mark a constructor explicit?
//     -> Almost always for single-argument constructors.
//        Prevents unexpected implicit conversions.
//        Makes code clearer (intent is obvious).
//        Only omit explicit when implicit conversion is INTENDED.

// Q6: What are the four C++ cast operators?
//     -> static_cast:       safe compile-time cast (most common)
//        dynamic_cast:      safe runtime cast for polymorphism
//        const_cast:        add/remove const qualifier
//        reinterpret_cast:  dangerous low-level bit reinterpretation

// Q7: What is the most vexing parse?
//     -> UDT u1(); looks like object creation
//        but C++ parses it as function declaration!
//        Fix: UDT u1{}; (always unambiguously an object)

// Q8: What is a conversion operator?
//     -> Member function that converts YOUR type to another type.
//        operator int() const { return m_variable; }
//        Mark explicit to prevent silent implicit conversions.
//        Without explicit: if(myUDT) could silently convert to bool!

int main()
{
    // UDT u1(500.123f);  // compiles but 500.123f -> 500 (data loss!)
    // UDT u1 = 500;      // ERROR with explicit (implicit blocked)
    // UDT u1{500.123f};  // ERROR - narrowing conversion refused by {}

    UDT u1{500};           // CORRECT
    // {} checks: is 500 safely convertible to int? YES (exact match)
    // explicit UDT(int) called directly
    // prints: m_variable: 500

    return 0;
}
// ```

// **The three protection layers together:**

// ```cpp
// explicit UDT(int i);   // layer 1: no implicit conversion TO UDT
// UDT u{500};            // layer 2: no narrowing conversion IN {}

// // Together they mean:
// UDT u{500};      // ✅ exact int, explicit construction
// UDT u{500.5f};   // ❌ narrowing refused by {}
// UDT u = 500;     // ❌ implicit conversion refused by explicit
// UDT u(500.5f);   // ⚠️  compiles but 500.5 silently becomes 500
// ```

// > **Rule of thumb**: use `explicit` on every single-argument constructor unless you specifically WANT implicit conversion (rare). Use `{}` instead of `()` for initialization — it's stricter and catches more bugs at compile time.