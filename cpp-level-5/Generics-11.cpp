// ============================================================
// TEMPLATE DEFAULT PARAMETERS - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT ARE TEMPLATE DEFAULT PARAMETERS?
// ============================================================

// Same concept as DEFAULT FUNCTION PARAMETERS:
// void foo(int x = 10)  // x defaults to 10 if not provided
// foo()     -> x = 10
// foo(5)    -> x = 5
//
// TEMPLATE DEFAULT PARAMETERS:
// template<typename T = int, int Size = 10>
// -> T defaults to int if not specified
// -> Size defaults to 10 if not specified
//
// Container<float, 5>  -> T=float, Size=5    (both specified)
// Container<float>     -> T=float, Size=10   (Size defaults to 10)
// Container<>          -> T=int,   Size=10   (both default)
// Container            -> T=int,   Size=10   (C++17 CTAD style)
//
// YOUR COMMENT: "=int only works C++17 and after, not previous version"
// SMALL CORRECTION: default template params for CLASSES work since C++11!
// template<typename T=int> in CLASS templates = C++11
// What requires C++17 is CTAD (writing Container c2 without <>)
// Container<> c2; would work in C++11 (explicit empty brackets)
// Container   c2; requires C++17 CTAD

// ============================================================
// SECTION 2: SYNTAX AND RULES
// ============================================================

template <typename T = int, int Size = 10>
//                   ^^^^^          ^^^
//                   default type   default value
//                   T=int if not   Size=10 if not
//                   specified      specified
//
// RULES (same as function default params):
// RULE 1: defaults must be on the RIGHT side
//   template<typename T=int, int Size>     // ERROR: Size has no default
//                                           // but T (before it) does
//   template<typename T, int Size=10>      // OK: rightmost has default
//   template<typename T=int, int Size=10>  // OK: all have defaults
//
// RULE 2: once you start using defaults, all remaining must be defaults
//   Container<float>    // OK: T=float, Size defaults to 10
//   Container<,5>       // ERROR: can't skip T to specify Size

class Container
{
public:
    Container()
    {
        m_data = new T[Size];
        std::cout << Size << std::endl;
        // Size is a compile-time constant here
        // for Container<int,5>:  prints 5
        // for Container (c2):    prints 10 (default)
    }
    ~Container()
    {
        delete[] m_data;
    }

private:
    T* m_data;
};

// ============================================================
// SECTION 3: main() TRACE
// ============================================================

int main()
{
    Container<int, 5> c1;
    // EXPLICIT: T=int, Size=5
    // m_data = new int[5]  (5 ints on heap)
    // prints: 5

    Container c2;
    // C++17 CTAD with defaults:
    // no arguments at all -> both defaults used
    // T=int (default), Size=10 (default)
    // equivalent to: Container<int, 10> c2;
    // m_data = new int[10]  (10 ints on heap)
    // prints: 10

    return 0;
    // LIFO:
    // ~c2: delete[] m_data (10 ints freed)
    // ~c1: delete[] m_data (5 ints freed)
}

// OUTPUT:
// 5
// 10

// ============================================================
// SECTION 4: FOUR WAYS TO CREATE THE CONTAINER
// ============================================================

// Container<float, 20> c;  // T=float, Size=20  (fully explicit)
// Container<float>     c;  // T=float, Size=10  (Size defaults)
// Container<>          c;  // T=int,   Size=10  (C++11: empty <> needed)
// Container            c;  // T=int,   Size=10  (C++17: no <> needed via CTAD)
//
// PRE-C++17 (C++11/14):
// Container c2;   // ERROR in C++11/14
//                 // compiler doesn't know it's a template instantiation
//                 // needs Container<> c2; (explicit empty brackets)
//
// C++17:
// Container c2;   // OK via CTAD
//                 // compiler uses default params to complete the type

// ============================================================
// SECTION 5: REAL WORLD USE CASES
// ============================================================

// USE CASE 1: STL CONTAINERS (they all have default allocator)
// template<typename T, typename Allocator = std::allocator<T>>
// class vector { ... };
//
// std::vector<int>  v;   // T=int, Allocator=std::allocator<int> (default)
// std::vector<int, MyAllocator> v2;  // custom allocator for special memory
//
// YOU use vector<int> all the time - you're using a default template param!
//
// USE CASE 2: DEFAULT BUFFER SIZE
// template<typename T, size_t BufferSize = 1024>
// class NetworkBuffer { T buffer[BufferSize]; };
//
// NetworkBuffer<char>       nb;  // 1024 char buffer (default)
// NetworkBuffer<char, 4096> nb2; // 4096 char buffer (custom)
//
// USE CASE 3: DEFAULT COMPARISON TYPE (std::map)
// template<typename Key, typename Value,
//          typename Compare = std::less<Key>>  // default: ascending sort
// class map { ... };
//
// std::map<int,string>                    m;  // ascending order (default)
// std::map<int,string,std::greater<int>>  m2; // descending order (custom)

// ============================================================
// SECTION 6: COMBINING DEFAULT PARAMS WITH CTAD
// ============================================================

// WITH both default params AND CTAD (C++17):
//
// Container c;           // CTAD: no constructor args to deduce from
//                        // falls back to DEFAULT PARAMS
//                        // T=int, Size=10
//
// BUT what if constructor takes arguments?
// template<typename T=int, int Size=10>
// class Container {
//     Container(T initialValue) { ... }  // has an arg
// };
//
// Container c(5);        // CTAD: T deduced as int from 5
//                        // Size still uses default = 10
//                        // -> Container<int, 10>
//
// Container<float> c(5.0f);  // T=float explicit, Size=10 default
//
// Container<float,20> c(5.0f);  // T=float, Size=20 (fully explicit)

// ============================================================
// SECTION 7: PARTIAL SPECIALIZATION WITH DEFAULT PARAMS
// ============================================================

// Default params interact with specialization:
//
// template<typename T=int, int Size=10>
// class Container { ... };  // primary template
//
// template<int Size>
// class Container<double, Size> { ... };  // partial spec for double
//
// Container c;              // uses primary: T=int, Size=10
// Container<double, 5> c2;  // uses specialization: T=double, Size=5
// Container<double>    c3;  // uses specialization: T=double, Size=10

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What are template default parameters?
//     -> Default values for template parameters, used when the
//        caller doesn't specify them explicitly. Same concept as
//        default function parameters but for template arguments.

// Q2: From which C++ version are default template params for classes?
//     -> C++11 for class templates (template<typename T=int>).
//        C++17 required for CTAD (Container c; without <>) which is
//        what makes default params look cleaner.

// Q3: What does Container c2; (no <>) require vs Container<> c2;?
//     -> Container<> c2;  works from C++11 (empty explicit brackets)
//        Container   c2;  requires C++17 CTAD

// Q4: What is the ordering rule for template default parameters?
//     -> Same as function default params: rightmost first.
//        Once a param has a default, all to its right must also have defaults.
//        Cannot skip a parameter to use its default while specifying a later one.

// Q5: Which STL containers use default template parameters?
//     -> std::vector<T, Allocator=std::allocator<T>>
//        std::map<K, V, Compare=std::less<K>, Allocator=...>
//        std::set<T, Compare=std::less<T>, Allocator=...>
//        You use these every day without specifying the defaults!

// Q6: Can non-type template params have defaults?
//     -> YES. template<typename T=int, int Size=10>
//        Both type params (typename) and non-type params (int)
//        can have default values.

// Q7: Container<float> c; - what is Size?
//     -> 10 (the default). T=float (explicitly specified),
//        Size not specified so defaults to 10.
//        Same as Container<float, 10> c;

// Q8: Why are default template params useful in library design?
//     -> Allow simple use cases without boilerplate while supporting
//        advanced use cases with explicit specification.
//        std::vector<int> (simple) vs std::vector<int,MyAllocator> (advanced).
//        Single template handles both without needing separate classes.
// ```

// **Default params - from simple to explicit:**

// ```cpp
// template<typename T = int, int Size = 10>
// class Container { };

// Container<float, 20> c;   // T=float,  Size=20   fully explicit
// Container<float>     c;   // T=float,  Size=10   Size defaults
// Container<>          c;   // T=int,    Size=10   both default (C++11)
// Container            c;   // T=int,    Size=10   both default (C++17 CTAD)
// ```

// **The STL connection you already use daily:**

// ```cpp
// std::vector<int> v;
// // actually: std::vector<int, std::allocator<int>>
// //                                 ^^^^^^^^^^^^^^^^ default param!
// // you've been using template default params the whole time!

// std::map<string, int> m;
// // actually: std::map<string, int, std::less<string>, std::allocator<...>>
// //                                 ^^^^^^^^^^^^^^^^^ default comparator
// ```

// > **The correction to your comment**: `typename T = int` in class templates works since **C++11** (not C++17). What **C++17** added is `Container c2;` without any `<>` — that's CTAD. In C++11/14, you'd need `Container<> c2;` to use all defaults. The `<>` was mandatory to tell the compiler "this is a template instantiation."