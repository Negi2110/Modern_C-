// ============================================================
// LAMBDA EXPRESSIONS - CLOSURES, CAPTURES - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <algorithm>
#include <vector>

// ============================================================
// SECTION 1: WHAT IS A LAMBDA?
// ============================================================

// Lambda = unnamed function object (anonymous functor) defined INLINE
// "constructs a CLOSURE" = a function + captured variables bundled together
//
// YOUR COMMENT: "auto creating func object automatically deducing"
// EXACTLY RIGHT - compiler generates a hidden struct with operator()
// just like print_functor but WITHOUT you writing the struct manually
//
// PROGRESSION from simple to modern:
// 1. REGULAR FUNCTION  : named, no state, must define elsewhere
// 2. FUNCTOR STRUCT    : named struct, HAS state, verbose to write
// 3. LAMBDA            : anonymous, HAS state (via capture), inline
//
// ALL THREE produce the same machine code - just different syntax!

// ============================================================
// SECTION 2: COMPARISON - THREE APPROACHES SIDE BY SIDE
// ============================================================

// APPROACH 1: Regular function (NO state)
// void func(int n) {
//     std::cout << n << ",";
// }
// -> cannot access lastResult (no capture mechanism)
// -> must define outside main, separate from use
// -> no state between calls

// APPROACH 2: Functor struct (HAS state - verbose)
struct print_functor {
    int lastResult{-1};    // STATE stored in member variable
    void operator()(int n) {
        lastResult = n;    // update STATE
        std::cout << n << ",";
    }
};
// -> HAS state (lastResult member)
// -> VERBOSE: must define entire struct
// -> can be reused, named, stored
// -> but lastResult is INSIDE the functor, not your main() variable

// APPROACH 3: Lambda (HAS state via capture - concise)
// auto print_v = [&lastResult](int n) { lastResult = n; std::cout << n; };
// -> HAS state (captures lastResult BY REFERENCE)
// -> CONCISE: defined inline where used
// -> can modify OUTER variables (via & capture)
// -> THIS is the modern C++ preferred approach

// ============================================================
// SECTION 3: LAMBDA ANATOMY - COMPLETE BREAKDOWN
// ============================================================

// auto print_v = [&lastResult](int n) { lastResult = n; std::cout << n << ","; };
//
// auto            = type deduced (lambda has unique unnamed type)
// print_v         = name given to the lambda (optional, can use inline)
//
// [&lastResult]   = CAPTURE CLAUSE
//    ^             what variables from outer scope to bring in
//    &             = capture by REFERENCE (modifies original!)
//    &lastResult   = specifically capture lastResult by reference
//
// (int n)         = PARAMETER LIST (like regular function params)
//    int n        = parameter: each element passed by for_each
//
// { ... }         = LAMBDA BODY
//    lastResult = n;        = modifies the CAPTURED variable
//    std::cout << n << ","; = prints the element

// ============================================================
// SECTION 4: CAPTURE CLAUSE - ALL FORMS
// ============================================================

// []              = capture NOTHING (cannot use any outer variables)
// [=]             = capture ALL by VALUE (copies of all outer variables)
// [&]             = capture ALL by REFERENCE (reference to all outer vars)
// [x]             = capture ONLY x by value
// [&x]            = capture ONLY x by reference
// [=, &x]         = capture all by value, but x by reference
// [&, x]          = capture all by reference, but x by value
// [&lastResult]   = YOUR CODE: capture only lastResult by reference
//
// BY VALUE [x]:
// -> copy of x made when lambda created
// -> modifying captured x does NOT affect original
// -> original x can change after lambda created - lambda has old copy
//
// BY REFERENCE [&x]:
// -> lambda holds REFERENCE to original x
// -> modifying captured &x MODIFIES the original variable
// -> original x must still be alive when lambda executes!
// -> DANGER: if x is on stack and lambda outlives x -> dangling reference

// ============================================================
// SECTION 5: WHY [&lastResult] IN YOUR CODE?
// ============================================================

// int lastResult = -1;   // exists in main()'s scope
//
// WITHOUT capture ([] empty):
// auto print_v = [](int n) {
//     lastResult = n;   // COMPILER ERROR! lastResult not captured
// };
//
// WITH value capture [lastResult]:
// auto print_v = [lastResult](int n) {
//     lastResult = n;   // ERROR: copy is const by default!
// };
// -> captures a COPY of lastResult at lambda creation time (-1)
// -> CANNOT modify the copy (lambda operator() is const by default)
// -> even if you could modify, it changes the COPY, not the original
// -> std::cout << lastResult in main() would still show -1
//
// WITH reference capture [&lastResult]:
// auto print_v = [&lastResult](int n) {
//     lastResult = n;   // OK: modifies the ORIGINAL in main()
// };
// -> lambda holds REFERENCE to main()'s lastResult
// -> every assignment updates the ACTUAL variable in main()
// -> after for_each: lastResult = 9 (last element)

// ============================================================
// SECTION 6: HOW COMPILER TRANSFORMS LAMBDA
// ============================================================

// YOUR LAMBDA:
// [&lastResult](int n) { lastResult = n; std::cout << n << ","; }
//
// COMPILER GENERATES (approximately):
// struct __lambda_unique_name {
//     int& m_lastResult;  // reference member (& capture)
//
//     __lambda_unique_name(int& lr) : m_lastResult(lr) {}
//
//     void operator()(int n) const {
//         m_lastResult = n;          // modifies via reference
//         std::cout << n << ",";
//     }
// };
//
// auto print_v = __lambda_unique_name(lastResult);
//
// -> EXACTLY like print_functor but auto-generated!
// -> THIS IS WHY lambda = syntactic sugar for functor

// ============================================================
// SECTION 7: std::for_each - HOW IT USES THE LAMBDA
// ============================================================

// std::for_each(begin(v), end(v), print_v);
//
// INTERNALLY (approximately):
// template<typename Iterator, typename Callable>
// void for_each(Iterator first, Iterator last, Callable func) {
//     while (first != last) {
//         func(*first);   // calls print_v.operator()(*first)
//         ++first;
//     }
// }
//
// SEQUENCE OF CALLS:
// print_v(1) -> lastResult=1, prints "1,"
// print_v(3) -> lastResult=3, prints "3,"
// print_v(2) -> lastResult=2, prints "2,"
// print_v(5) -> lastResult=5, prints "5,"
// print_v(9) -> lastResult=9, prints "9,"
//
// AFTER for_each:
// lastResult in main() = 9 (LAST element, because & reference)
// std::cout << lastResult -> prints 9

// ============================================================
// SECTION 8: main() TRACE
// ============================================================

int main()
{
    std::vector<int> v{1, 3, 2, 5, 9};

    int lastResult = -1;
    // lastResult lives in main()'s stack frame

    auto print_v = [&lastResult](int n) {
        lastResult = n;          // reference: modifies main()'s variable!
        std::cout << n << ",";
    };
    // print_v is a lambda (closure object)
    // captures &lastResult = reference to main's lastResult

    std::for_each(begin(v), end(v), print_v);
    // calls print_v for each element: 1, 3, 2, 5, 9
    // prints: 1,3,2,5,9,
    // lastResult updated each time via reference
    // final value: 9 (last element processed)

    std::cout << '\n' << lastResult << std::endl;
    // prints: 9  (updated by lambda via reference capture!)

    // ---- OTHER COMMENTED APPROACHES ----

    // std::for_each(begin(v), end(v), print_functor());
    // -> functor version: HAS its own lastResult (not main's!)
    // -> main's lastResult stays -1 (no link to functor's internal state)

    // std::for_each(begin(v), end(v), func);
    // -> regular function: no state, cannot update lastResult

    // std::for_each(begin(v), end(v), [](int n) { std::cout << n << ","; });
    // -> inline lambda: [] = no capture, cannot access lastResult

    std::cout << std::endl;
    return 0;
}

// FULL OUTPUT:
// 1,3,2,5,9,
// 9

// ============================================================
// SECTION 9: MUTABLE LAMBDA - when you need value capture + modify
// ============================================================

// By default: lambda operator() is CONST
// value-captured variables CANNOT be modified
//
// auto bad = [lastResult](int n) {
//     lastResult = n;  // ERROR: const lambda, can't modify copy
// };
//
// FIX: add 'mutable' keyword
// auto good = [lastResult](int n) mutable {
//     lastResult = n;  // OK: modifies the COPY
//     // but main()'s lastResult still unchanged!
// };
// -> mutable = make the copy modifiable
// -> still doesn't affect the original
// -> use [&] when you want to affect the original

// ============================================================
// SECTION 10: LAMBDA RETURN TYPE
// ============================================================

// DEDUCED (C++14):
// auto add = [](int a, int b) { return a + b; }; // return type deduced as int

// EXPLICIT (when needed):
// auto divide = [](int a, int b) -> float {  // must return float
//     return (float)a / b;
// };

// MULTIPLE RETURN TYPES (must be explicit):
// auto f = [](bool flag) -> int {
//     if (flag) return 1;
//     return 0;
// };

// ============================================================
// SECTION 11: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a lambda expression?
//     -> An anonymous function object (closure) that can capture variables
//        from the surrounding scope. Syntax: [capture](params){ body }.
//        Compiler generates a unique functor struct internally.

// Q2: What is a closure?
//     -> The combination of a function AND the environment it captures.
//        A lambda with [&x] closes over x - it "closes" around x's value.
//        The lambda object carries its captured variables with it.

// Q3: What is the difference between [x] and [&x] capture?
//     -> [x]:  captures a COPY of x when lambda is created.
//               modifying captured x affects only the copy, not original.
//               original can change but lambda has old value.
//        [&x]: captures a REFERENCE to x.
//               modifying captured &x MODIFIES the original.
//               original must stay alive while lambda is used.

// Q4: Why does [&lastResult] capture allow lastResult to be updated in main()?
//     -> The lambda holds a reference to main()'s lastResult.
//        When lambda assigns lastResult = n, it writes through the reference
//        to the actual variable in main(). No copy involved.

// Q5: What is the difference between a lambda and a functor?
//     -> A lambda IS a functor. Compiler generates a hidden struct with
//        operator() for every lambda. Captures become member variables.
//        Lambda = syntactic sugar for writing a functor inline.

// Q6: What does 'mutable' do on a lambda?
//     -> Makes the lambda's operator() non-const, allowing modification
//        of value-captured variables (the copies). Does NOT affect originals.
//        Without mutable: value captures are read-only inside the lambda.

// Q7: What is [] vs [=] vs [&] capture?
//     -> []:  capture nothing (cannot use any outer variables)
//        [=]: capture all outer variables by value (copies)
//        [&]: capture all outer variables by reference (references)
//        Specific: [&x, y] - x by ref, y by value

// Q8: Can a lambda be stored and called later?
//     -> YES. auto f = [](int x) { return x*2; };
//        f(5);  // call later -> 10
//        Can be stored in std::function<int(int)> for type-erased storage.
//        Must ensure captured references remain valid until lambda is called!
// ```

// **Lambda anatomy - memorize this:**

// ```cpp
// auto print_v = [&lastResult](int n) { lastResult = n; std::cout << n; };
//              ^            ^         ^
//              |            |         BODY
//              |            PARAMS
//              CAPTURE
//              & = by reference (modifies original)
// //              no & = by value (copy, original unchanged)
// ```

// **What compiler generates from your lambda:**

// ```cpp
// // YOU write:
// [&lastResult](int n) { lastResult = n; std::cout << n << ","; }

// // COMPILER generates:
// struct __lambda {
//     int& m_lastResult;           // reference member
//     void operator()(int n) const {
//         m_lastResult = n;        // modifies via reference
//         std::cout << n << ",";
//     }
// };
// // Lambda IS a functor. Functor IS a lambda. Same thing, different syntax.
// ```

// > **The connection to your functor session**: `print_functor` and the lambda `[&lastResult](int n){...}` produce **identical machine code**. The lambda is just the compiler auto-generating the functor for you. The key insight your code shows is: lambdas can reach into the surrounding scope and **modify** variables there (via `&` capture) — something a regular function can never do without a global variable or pointer parameter.