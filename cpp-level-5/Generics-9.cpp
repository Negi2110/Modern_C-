// ============================================================
// CLASS TEMPLATE ARGUMENT DEDUCTION (CTAD) C++17 - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <vector>

// ============================================================
// SECTION 1: THE PROBLEM CTAD SOLVES
// ============================================================

// BEFORE C++17 - had to specify template arguments EXPLICITLY:
// Entry<int, int> entry(5, 100);         // verbose, redundant
// std::pair<int, double> p(5, 3.14);     // compiler KNOWS these types!
// std::vector<float> v = {1.0f, 2.0f};  // obvious from the values
//
// AFTER C++17 (CTAD) - compiler DEDUCES from constructor arguments:
// Entry entry(5, 100);        // compiler deduces Entry<int,int>
// std::pair p(5, 3.14);       // compiler deduces pair<int,double>
// std::vector v{1.0f,2.0f};   // compiler deduces vector<float>
//
// CTAD = Class Template Argument Deduction
// -> same idea as function template argument deduction (existed since C++98)
//    but now extended to CLASSES (C++17)
//
// FUNCTION templates already had this:
// template<typename T>
// T square(T input) { return input*input; }
// square(5)     -> T deduced as int    (C++98)
// square(5.5f)  -> T deduced as float  (C++98)
//
// CLASS templates only got this in C++17:
// Entry entry(5, 100) -> deduced as Entry<int,int> (C++17)

// ============================================================
// SECTION 2: THE TEMPLATE CLASS WITH CONSTRUCTOR
// ============================================================

template <typename first, typename second>
struct Entry
{
    Entry(first key, second value)
    //    ^^^^^       ^^^^^^
    //    constructor parameters have template types
    //    compiler reads these to DEDUCE first and second
    {
        m_key   = key;
        m_value = value;
        // better would be initializer list:
        // Entry(first key, second value) : m_key(key), m_value(value) {}
    }

    first  m_key;    // type = first template param
    second m_value;  // type = second template param
};

// ============================================================
// SECTION 3: HOW CTAD WORKS INTERNALLY
// ============================================================

// Entry entry(5, 100);
//
// COMPILER DOES:
// STEP 1: see Entry without <> -> try CTAD
// STEP 2: look at Entry's constructors
//         Entry(first key, second value)
// STEP 3: match constructor params to provided arguments
//         key   = 5   -> first  deduced as int
//         value = 100 -> second deduced as int
// STEP 4: generate Entry<int, int>
// STEP 5: call Entry<int,int>::Entry(int, int)
//
// RESULT: entry is of type Entry<int, int>
// EQUIVALENT TO:
// Entry<int, int> entry(5, 100);  // what you had to write pre-C++17
//
// DIFFERENT ARGUMENT TYPES:
// Entry entry(5, 3.14);    -> Entry<int, double>
// Entry entry("hi", 5.0f); -> Entry<const char*, float>
// Entry entry(5, "hello"); -> Entry<int, const char*>
// Compiler figures it ALL out from the constructor arguments!

// ============================================================
// SECTION 4: std::vector CTAD - YOUR SECOND EXAMPLE
// ============================================================

// std::vector v{1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
//
// BEFORE C++17:
// std::vector<float> v{1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
//
// WITH CTAD (C++17):
// std::vector v{1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
// -> compiler deduces vector<float> from the float literals
//
// std::vector v{1, 2, 3}         -> vector<int>
// std::vector v{1.5, 2.5, 3.5}   -> vector<double>
// std::vector v{"a", "b", "c"}   -> vector<const char*>
// std::vector v{1, 2.0, 3}       -> DEDUCTION CONFLICT!
//                                    1=int, 2.0=double -> ambiguous
//                                    must specify: vector<double> v{1,2.0,3}

// ============================================================
// SECTION 5: DEDUCTION GUIDES - ADVANCED (good to know)
// ============================================================

// Sometimes CTAD can't deduce correctly or deduces wrong type
// You can write DEDUCTION GUIDES to help:
//
// template<typename T>
// struct MyContainer {
//     MyContainer(T* ptr, size_t n) { ... }
// };
//
// // Without guide: hard to deduce T from pointer
// MyContainer c(arr, 10);   // might fail or deduce wrong T
//
// // WITH deduction guide:
// template<typename T>
// MyContainer(T*, size_t) -> MyContainer<T>;  // guide
//
// // Now: MyContainer c(arr, 10); correctly deduces T
//
// std::vector has built-in deduction guides for initializer_list
// THAT is why std::vector v{1.0f,2.0f} works in C++17

// ============================================================
// SECTION 6: CTAD vs make_ FUNCTIONS
// ============================================================

// BEFORE CTAD: "make_" helper functions were the solution:
// std::make_pair(5, 3.14)      -> returns pair<int,double>
// std::make_unique<T>(args...)  -> returns unique_ptr<T>
// std::make_shared<T>(args...)  -> returns shared_ptr<T>
//
// THESE WERE WORKAROUNDS because functions had deduction but classes didn't!
//
// WITH C++17 CTAD:
// std::pair p(5, 3.14);        // deduces pair<int,double> directly!
// std::pair p = {5, 3.14};     // also works!
//
// BUT: std::make_unique and std::make_shared are STILL preferred:
// -> exception safety (as discussed in PIMPL session)
// -> std::unique_ptr u(new int(5));  // works but raw new exposed
// -> auto u = std::make_unique<int>(5); // better - no raw new

// ============================================================
// SECTION 7: WHAT CTAD CANNOT DO
// ============================================================

// CANNOT deduce from member variables alone:
// template<typename T>
// struct Foo { T value; };
//
// Foo f;  // ERROR: no constructor to deduce from
// Foo f = {5};  // might work with aggregate deduction (C++20)
//
// CANNOT deduce partial args:
// Entry<int> e(5, 100);  // ERROR: must specify ALL or NONE
//                          can't mix explicit + deduced
//
// CANNOT deduce when ambiguous:
// std::vector v{5, 6};   // OK: vector<int>
// std::vector v(5, 6);   // DIFFERENT: vector of 5 sixes (uses size ctor)
//                          // NOT CTAD issue, just {} vs () gotcha again!

// ============================================================
// SECTION 8: main() TRACE
// ============================================================

int main()
{
    Entry entry(5, 100);
    // CTAD: first=int (from 5), second=int (from 100)
    // entry is Entry<int,int>
    // m_key = 5, m_value = 100

    std::cout << entry.m_key   << std::endl;  // prints: 5
    std::cout << entry.m_value << std::endl;  // prints: 100

    std::vector v{1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    // CTAD: all float literals -> vector<float>
    // v = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f}

    return 0;
}

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is CTAD?
//     -> Class Template Argument Deduction (C++17).
//        Allows compiler to deduce template type arguments from
//        constructor arguments, eliminating need to specify them explicitly.

// Q2: What C++ version introduced CTAD?
//     -> C++17. Before that, only function templates supported
//        argument deduction. Classes always needed explicit <T> specification.

// Q3: Entry entry(5, 100) - what type is entry?
//     -> Entry<int, int>. Compiler sees constructor Entry(first, second)
//        and matches: first=int (from 5), second=int (from 100).

// Q4: What is the difference between CTAD and make_ functions?
//     -> make_ functions (make_pair, make_unique) were WORKAROUNDS
//        because functions had deduction but classes didn't (pre-C++17).
//        CTAD makes make_ functions unnecessary for simple cases.
//        make_unique/make_shared still preferred for exception safety.

// Q5: std::vector v{5,6} vs std::vector v(5,6) - difference?
//     -> v{5,6}: CTAD + initializer_list -> vector<int> with [5,6]
//        v(5,6): constructor(size,value) -> vector<int> with [6,6,6,6,6]
//        Same {} vs () gotcha from the initializer_list session!

// Q6: What is a deduction guide?
//     -> User-provided hint that tells compiler how to deduce template
//        arguments in cases where constructors alone are ambiguous.
//        template<typename T> MyClass(T*,size_t) -> MyClass<T>;

// Q7: Can you partially specify template args with CTAD?
//     -> NO. Must specify ALL explicitly or let compiler deduce ALL.
//        Entry<int> e(5, 100) -> ERROR (partial specification).
//        Entry e(5, 100)      -> OK (full deduction, C++17).
//        Entry<int,int> e(5,100) -> OK (full explicit, always worked).

// Q8: Why was CTAD not in C++11/14?
//     -> Language evolution. C++11 added deduction for function templates
//        and auto. CTAD for classes required more complex machinery
//        (deduction guides, handling overloaded constructors, etc.)
//        standardized in C++17 after enough implementation experience.
// ```

// **CTAD in one picture:**

// ```cpp
// // BEFORE C++17: redundant, verbose
// Entry<int, int>   entry(5, 100);      // you repeat what compiler already knows!
// std::vector<float> v{1.0f, 2.0f};    // obvious from the values!

// // C++17 CTAD: compiler reads constructor args and deduces
// Entry entry(5, 100);              // deduces Entry<int,int>
// std::vector v{1.0f, 2.0f};       // deduces vector<float>
// //      ↑ no <> needed!
// ```

// **The `{}` vs `()` trap still exists with CTAD:**

// ```cpp
// std::vector v{5, 6};   // CTAD → vector<int> containing [5, 6]    ← 2 elements
// std::vector v(5, 6);   // NOT CTAD → vector<int> of 5 sixes       ← 5 elements
// //           ^               uses count,value constructor
// ```

// > **The connection**: CTAD is why in C++17 you can write `std::pair p(5, 3.14)` instead of `std::make_pair(5, 3.14)`. The `make_` functions existed **specifically because** classes couldn't deduce types — CTAD made them largely redundant for simple use cases, though `make_unique` and `make_shared` are still preferred for exception safety.