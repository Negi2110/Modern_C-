// ============================================================
// FUNCTORS (FUNCTION OBJECTS) - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <vector>
#include <algorithm>

// ============================================================
// SECTION 1: WHAT IS A FUNCTOR?
// ============================================================

// FUNCTOR = an object that can be CALLED LIKE A FUNCTION
// achieved by overloading operator()
// "function with STATE" - this is the KEY difference from a regular function
//
// REGULAR FUNCTION:
// int add(int a, int b) { return a + b; }
// -> stateless (no memory between calls)
// -> cannot "remember" previous calls
//
// FUNCTOR:
// struct Adder {
//     int m_total = 0;
//     int operator()(int x) { m_total += x; return m_total; }
// };
// Adder a;
// a(5);   // m_total = 5
// a(10);  // m_total = 15 (REMEMBERS previous call!)
// -> HAS STATE (member variables persist between calls)
// -> behaves like a function but carries data with it

// ============================================================
// SECTION 2: YOUR QUESTION - "ISN'T IT LIKE OBJECT OVERRIDING?"
// ============================================================

// YOUR COMMENT: "isn't it like an object or object overriding I can say?"
//
// PARTIALLY RIGHT - it IS operator OVERLOADING:
// operator() = overloading the CALL operator
// just like: operator+ overloads +
//            operator[] overloads []
//            operator() overloads ()   <- making it callable
//
// NOT "object overriding" (that's inheritance/virtual functions)
// BUT IS "operator overloading" specifically for the () operator
//
// TWO OVERLOADS of operator() in your code:
// operator()(int)   -> called when you pass int
// operator()(float) -> called when you pass float
// SAME NAME, DIFFERENT PARAMETER TYPES = function OVERLOADING
// compiler picks correct version based on argument type

// ============================================================
// SECTION 3: YOUR VALUE FUNCTOR
// ============================================================

struct Value
{
    int   m_result1{0};   // STATE: remembers last int result
    float m_result2{0};   // STATE: remembers last float result

    // OVERLOAD 1: called when () with int argument
    int operator()(int newResult)
    {
        m_result1 = newResult;  // update STATE (member variable)
        return newResult;
    }

    // OVERLOAD 2: called when () with float argument
    float operator()(float newResult2)
    {
        m_result2 = newResult2; // update STATE (member variable)
        return newResult2;
    }
};

// ============================================================
// SECTION 4: main() TRACE
// ============================================================

int main()
{
    Value v;
    // v.m_result1 = 0 (default)
    // v.m_result2 = 0.0f (default)

    v(42);
    // compiler sees: v.operator()(42)
    // 42 is int -> matches operator()(int newResult)
    // m_result1 = 42
    // returns 42 (ignored here)
    //
    // v(42.0) would call float version (double->float)
    // v(42)   calls int version (exact match)

    v(42.7f);
    // 42.7f is float -> matches operator()(float newResult2)
    // m_result2 = 42.7f
    // returns 42.7f (ignored here)

    std::cout << v.m_result1 << "," << v.m_result2 << std::endl;
    // v.m_result1 = 42  (set by first call)
    // v.m_result2 = 42.7 (set by second call)
    // prints: 42,42.7

    return 0;
}

// ============================================================
// SECTION 5: FUNCTOR vs FUNCTION vs LAMBDA COMPARISON
// ============================================================

// REGULAR FUNCTION (no state):
// int multiply(int x, int y) { return x * y; }
// multiply(3, 4);  // = 12
// multiply(3, 4);  // = 12 (no memory of previous calls)

// FUNCTOR (has state):
// struct Multiplier {
//     int m_factor;
//     Multiplier(int factor) : m_factor(factor) {}
//     int operator()(int x) { return x * m_factor; }
// };
// Multiplier triple(3);
// triple(4);  // = 12 (3 * 4, factor stored in object!)
// triple(5);  // = 15 (3 * 5, SAME factor remembered!)

// LAMBDA (C++11 - modern functor shorthand):
// auto triple = [factor=3](int x) { return x * factor; };
// triple(4);  // = 12
// triple(5);  // = 15
// -> lambda IS a functor internally! compiler generates a struct with operator()

// ============================================================
// SECTION 6: FUNCTORS WITH std::algorithm - MOST COMMON USE
// ============================================================

// Functors shine with STL algorithms:
//
// struct GreaterThan {
//     int m_threshold;
//     GreaterThan(int t) : m_threshold(t) {}
//     bool operator()(int value) const {
//         return value > m_threshold;
//     }
// };
//
// std::vector<int> v = {1, 5, 3, 8, 2, 9, 4};
//
// // COUNT elements greater than 5:
// int count = std::count_if(v.begin(), v.end(), GreaterThan(5));
// // GreaterThan(5) creates a functor object with threshold=5
// // count_if calls functor(element) for each element
// // functor REMEMBERS the threshold (state!)
// // result: 2 (8 and 9 are > 5)
//
// WITHOUT functor (must use global variable or lambda):
// int threshold = 5;  // external state - messy!
// int count = std::count_if(v.begin(), v.end(),
//     [threshold](int x) { return x > threshold; });
// -> lambda IS a functor: captures threshold as state

// ============================================================
// SECTION 7: ACCUMULATING FUNCTOR - SHOWS STATE POWER
// ============================================================

// struct Sum {
//     int m_total{0};
//
//     int operator()(int value) {
//         m_total += value;    // ACCUMULATES across calls
//         return m_total;
//     }
// };
//
// Sum summer;
// summer(10);  // m_total = 10
// summer(20);  // m_total = 30
// summer(5);   // m_total = 35
// std::cout << summer.m_total;  // 35
//
// YOUR Value struct works similarly:
// v(42)   -> sets and remembers m_result1
// v(42.7f)-> sets and remembers m_result2
// v is both callable AND stores state!

// ============================================================
// SECTION 8: FUNCTOR IN std::sort - CUSTOM COMPARATOR
// ============================================================

// struct SortDescending {
//     bool operator()(int a, int b) const {
//         return a > b;  // descending order
//     }
// };
//
// std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};
// std::sort(v.begin(), v.end(), SortDescending());
// // SortDescending() creates functor object
// // sort uses it as comparator
// // result: {9, 6, 5, 4, 3, 2, 1, 1}
//
// EQUIVALENT LAMBDA:
// std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
//
// BOTH work - functor is more verbose but can have complex state
// Lambda is more concise for simple cases

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a functor?
//     -> An object that can be called like a function by overloading
//        operator(). Also called "function object". Key benefit over
//        regular functions: can have STATE (member variables that
//        persist between calls).

// Q2: What makes functors different from regular functions?
//     -> STATE. Regular functions are stateless (no memory between calls).
//        Functors have member variables that persist, allowing them to
//        accumulate results, remember settings, count calls, etc.

// Q3: How is a functor called?
//     -> Using () syntax on the object: v(42) or myFunctor(args...).
//        Compiler translates: v(42) -> v.operator()(42).
//        Looks like a function call but IS an object method.

// Q4: Your comment: "is it like object overriding?" - what IS it?
//     -> It's OPERATOR OVERLOADING, specifically overloading operator().
//        Not "overriding" (which is virtual function / inheritance).
//        Multiple operator() overloads = function OVERLOADING.
//        Compiler picks the right overload based on argument types.

// Q5: What is the relationship between functors and lambdas?
//     -> Lambdas ARE functors internally. When you write a lambda,
//        the compiler generates a struct with operator(). Captured
//        variables become member variables of that struct.
//        Functors are the explicit version of what lambdas do implicitly.

// Q6: Why use functors with STL algorithms?
//     -> Algorithms like sort, count_if, transform take CALLABLES.
//        Functors let you pass behavior WITH state (e.g., threshold value).
//        More efficient than function pointers (can be inlined by compiler).
//        More flexible than regular functions (carry configuration data).

// Q7: Can operator() be overloaded?
//     -> YES (as your code shows). Different parameter types = different
//        overloads. v(42) calls int version, v(42.7f) calls float version.
//        Compiler uses normal overload resolution to pick the right one.

// Q8: What is the performance advantage of functors over function pointers?
//     -> Function pointers prevent inlining (compiler sees an address, not code).
//        Functors (and lambdas) can be INLINED by the compiler.
//        Template functions accepting functor types see the full code.
//        This is why STL algorithms with lambdas/functors can be faster
// //        than those using function pointers.
// ```

// **Functor vs function - the state difference:**

// ```cpp
// // FUNCTION (stateless):
// int count = 0;   // must use external variable
// void record(int x) { count++; }   // messy, not encapsulated

// // FUNCTOR (state inside the object):
// struct Counter {
//     int count{0};
//     void operator()(int x) { count++; }  // state is INSIDE
// };
// Counter c;
// c(1); c(2); c(3);
// std::cout << c.count;  // 3 - state carried with the object!
// ```

// **Your VALUE functor - how operator() dispatch works:**

// ```cpp
// Value v;
// v(42);      // compiler: v.operator()(42)   → int overload
// v(42.7f);   // compiler: v.operator()(42.7f) → float overload
// // Same () syntax, different types → different overloads called
// ```

// > **To answer your question directly**: it's **operator overloading** (specifically `operator()`), not "object overriding." Overriding is when a derived class replaces a virtual function from a base class. What you have here is **two overloads of the same operator** — `operator()(int)` and `operator()(float)` — and the compiler picks the right one based on the argument type, exactly like any other function overload.