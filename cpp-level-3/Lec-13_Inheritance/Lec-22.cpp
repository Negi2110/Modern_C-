// ============================================================
// std::initializer_list, {} vs (), CONSTRUCTOR RESOLUTION - NOTES
// ============================================================

#include <iostream>
#include <vector>
#include <initializer_list>

// ============================================================
// SECTION 1: WHAT IS std::initializer_list?
// ============================================================

// std::initializer_list<int> = a lightweight, READ-ONLY view
// over a sequence of values written with {curly braces}
//
// std::initializer_list<int> data = {1, 2, 3, 4, 5};
// -> data does NOT own the values (no heap allocation by data itself)
// -> the {1,2,3,4,5} values live in a temporary array
//    created by the COMPILER, and data just gives you READ access
// -> supports: .begin(), .end(), .size() - like a tiny read-only container
// -> this is EXACTLY how std::vector's {1,2,3} constructor works internally!

struct UDT
{
    UDT(int a, int b, int c)
    {
        std::cout << "constructor with three param" << std::endl;
    }
    // REGULAR 3-parameter constructor

    UDT(std::initializer_list<int> data) : m_data(data)
    // SPECIAL constructor that accepts a {brace list} of ANY length
    {
        std::cout << "initializer_list constructor called" << std::endl;
    }

    void printData()
    {
        for (auto& i : m_data)
            std::cout << i << " ,";
        std::cout << std::endl;
    }

    ~UDT() {}

private:
    std::initializer_list<int> m_data;
};

// ============================================================
// SECTION 2: THE CRITICAL RULE - {} PREFERS initializer_list
// ============================================================

// WHEN YOU WRITE: UDT udt{1, 2, 3};
//
// COMPILER'S DECISION PROCESS (in this EXACT priority order):
//
// STEP 1: "Is there a constructor taking std::initializer_list
//          that MATCHES this {} call?"
//         -> YES! UDT(std::initializer_list<int>) exists
//         -> USE IT IMMEDIATELY, stop looking
//
// STEP 2 (only reached if NO initializer_list constructor exists):
//         "Try regular constructor overload resolution"
//         -> would match UDT(int,int,int) for {1,2,3}
//
// THIS IS WHY YOUR COMMENT IS CORRECT:
// "if we comment out [the initializer_list constructor]
//  ALL the {1,2,3,4,5,...} will call the three param one"
//
// WITHOUT initializer_list constructor:
// UDT udt{1, 2, 3};   // matches UDT(int,int,int) - 3 params exactly
// -> prints: "constructor with three param"
//
// WITH initializer_list constructor (your actual code):
// UDT udt{1, 2, 3};   // {} ALWAYS prefers initializer_list FIRST
// -> prints: "initializer_list constructor called"
// -> the 3-param constructor is COMPLETELY IGNORED here,
//    even though {1,2,3} could ALSO match it exactly!

// ============================================================
// SECTION 3: () NEVER USES initializer_list - DIRECT MATCH ONLY
// ============================================================

// UDT udt1(2, 13, 78);
// -> () parentheses NEVER consider initializer_list constructors
// -> goes straight to NORMAL overload resolution
// -> matches UDT(int a, int b, int c) - 3 separate int parameters
// -> prints: "constructor with three param"
// -> udt1.printData() prints NOTHING (m_data was never initialized
//    by the 3-param constructor, stays empty/default)
//
// YOUR COMMENT CONFIRMS THIS EXACTLY:
// "nothing will be printed because init list constructor is not called"

// ============================================================
// SECTION 4: COMPLETE SIDE-BY-SIDE COMPARISON
// ============================================================

// UDT udt{1, 2, 3};      -> {} -> initializer_list ctor called
//                            "initializer_list constructor called"
//                            printData(): 1 ,2 ,3 ,
//
// UDT udt1(2, 13, 78);   -> () -> regular 3-param ctor called
//                            "constructor with three param"
//                            printData(): (nothing, m_data empty)
//
// THIS IS A FAMOUS C++ GOTCHA:
// SAME number of arguments (3), SAME types (int,int,int)
// but {} and () call COMPLETELY DIFFERENT constructors!

// ============================================================
// SECTION 5: NARROWING CONVERSION - WHY {} IS STRICTER
// ============================================================

// UDT udt3{1.2, 3.4, 5.6};   // COMPILER ERROR if uncommented!
//
// WHY?
// -> {} tries initializer_list<int> constructor FIRST
// -> 1.2, 3.4, 5.6 are DOUBLES being squeezed into ints
// -> this is a NARROWING CONVERSION (data loss: 1.2 -> 1)
// -> {} REFUSES narrowing conversions (you learned this with
//    UDT u1{500.123f} from the explicit/list-init session!)
// -> COMPILER ERROR: "narrowing conversion from double to int"
//
// COMPARE:
// int c{4.2};    // ERROR - {} refuses narrowing (double -> int)
// int b(4.2);    // OK    - () allows narrowing silently (b = 4)
// int a = 4.2;   // OK    - = also allows narrowing silently (a = 4)
//
// THIS IS THE SAME RULE from before, just appearing again here:
// {} = STRICT, catches data-loss bugs at compile time
// () and = ALLOW silent truncation

// ============================================================
// SECTION 6: THE "MOST VEXING PARSE" - UDT udt() PROBLEM
// ============================================================

// UDT udt();
// -> looks like: "create udt, calling default constructor"
// -> ACTUALLY means: "declare a FUNCTION named udt,
//                      taking no parameters, returning UDT"
// -> THIS IS THE MOST VEXING PARSE (a famous C++ ambiguity)
//
// COMPILER sees udt() and thinks "function declaration" because
// ANYTHING that COULD be parsed as a function declaration,
// WILL be parsed as a function declaration (C++ grammar rule)
//
// FIX: use {} instead
// UDT udt{};   // UNAMBIGUOUSLY an object, never confused as a function
//             // BUT this would ALSO need a default constructor to exist!
//             // (UDT has none here - only 3-param and initializer_list)
//             // so even udt{} would fail in THIS specific class

// ============================================================
// SECTION 7: std::vector EXAMPLE - SAME RULE, REAL WORLD IMPACT
// ============================================================

// std::vector<int> v1{1, 2, 3, 4, 5};
// -> {} triggers vector's initializer_list constructor
// -> creates a vector CONTAINING the values: [1, 2, 3, 4, 5]
// -> size() = 5

// std::vector<int> v1(10, 1);
// -> () triggers vector's (count, value) constructor
// -> creates a vector with 10 ELEMENTS, each set to value 1
// -> size() = 10, contents: [1,1,1,1,1,1,1,1,1,1]
//
// THIS IS THE SAME UDT BEHAVIOR, just with vector instead!
// {} = "these ARE my values"
// () = "construct ME using these arguments/rules"
//
// CLASSIC BUG EVERY C++ DEVELOPER HITS ONCE:
// std::vector<int> v{10, 1};   // {} -> vector [10, 1] (TWO elements!)
// std::vector<int> v(10, 1);   // () -> vector of TEN 1's
// -> SAME numbers, COMPLETELY different result depending on brackets!

// ============================================================
// SECTION 8: main() FULL TRACE
// ============================================================

int main()
{
    UDT udt{1, 2, 3};
    // {} -> tries initializer_list FIRST -> MATCHES -> uses it
    // prints: "initializer_list constructor called"
    udt.printData();
    // prints: 1 ,2 ,3 ,

    UDT udt1(2, 13, 78);
    // () -> NEVER considers initializer_list -> regular overload resolution
    // matches UDT(int,int,int)
    // prints: "constructor with three param"
    udt1.printData();
    // prints: (empty line - m_data was never set, stays default/empty)

    return 0;
}

// ============================================================
// SECTION 9: THE GOLDEN RULE TO REMEMBER
// ============================================================

// IF a class has BOTH:
// -> a regular constructor matching your argument count
// -> an initializer_list constructor
//
// THEN:
// {arg1, arg2, ...}  ALWAYS prefers initializer_list (if it could match)
// (arg1, arg2, ...)  NEVER considers initializer_list at all
//
// THIS PRIORITY RULE IS ABSOLUTE - there's no further negotiation,
// {} commits to trying initializer_list FIRST, full stop.

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::initializer_list?
//     -> A lightweight, read-only object representing a sequence of
//        values written with curly braces. Backed by a compiler-
//        generated temporary array. Provides begin()/end()/size().

// Q2: If a class has both a 3-param constructor and an initializer_list
//     constructor, which gets called by UDT udt{1,2,3}?
//     -> The initializer_list constructor ALWAYS wins when using {}.
//        Curly-brace initialization tries initializer_list constructors
//        first, before considering any other overload.

// Q3: Does UDT udt(1,2,3) ever consider the initializer_list constructor?
//     -> NO. Parentheses NEVER trigger initializer_list constructor
//        resolution. They go straight to normal overload resolution.

// Q4: Why does {} reject narrowing conversions but () doesn't?
//     -> List initialization ({}) was designed in C++11 specifically
//        to catch accidental data loss at compile time. Parentheses
//        and = preserve old C-style implicit narrowing behavior for
//        backward compatibility.

// Q5: What is the Most Vexing Parse?
//     -> When syntax like UDT udt(); is ambiguous between "declare
//        a variable" and "declare a function returning UDT taking no
//        arguments" - C++ grammar always resolves it as a function
//        declaration. Using {} avoids this entirely.

// Q6: std::vector<int> v(10,1) vs std::vector<int> v{10,1} - difference?
//     -> v(10,1): vector of 10 elements, each = 1 (count, value)
//        v{10,1}: vector containing exactly [10, 1] (2 elements,
//                  via initializer_list constructor)

// Q7: Why might using {} everywhere be considered safer in modern C++?
//     -> Prevents most vexing parse, rejects narrowing conversions,
//        and provides uniform initialization syntax across all types
//        (primitives, classes, containers). Catches more bugs at
//        compile time than () or =.

// Q8: What's a real-world danger of this {} vs () distinction?
//     -> Classes (like std::vector) offering BOTH a count-based
//        constructor and an initializer_list constructor create a
//        trap: same argument values but DIFFERENT brackets produce
//        completely different objects. Easy source of subtle bugs.
// ```

// **The rule in one sentence:**

// ```
// {1, 2, 3}  →  ALWAYS tries initializer_list constructor FIRST
// (1, 2, 3)  →  NEVER even considers initializer_list constructor
// ```

// **The classic vector trap - memorize this:**

// ```cpp
// std::vector<int> v1{10, 1};   // [10, 1]              <- 2 elements
// std::vector<int> v2(10, 1);   // [1,1,1,1,1,1,1,1,1,1] <- 10 elements
// //                same numbers, opposite meaning!
// ```

// > **Practical interview takeaway**: when a class offers both a regular constructor and an `initializer_list` constructor with overlapping argument counts, `{}` and `()` become **two different APIs** to the same class. This is exactly why `std::vector` trips up beginners — always know which bracket style you're using and why.