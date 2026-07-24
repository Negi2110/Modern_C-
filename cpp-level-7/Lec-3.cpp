// ============================================================
// LAMBDA CAPTURES - [=], [&], mutable, GLOBALS - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <vector>
#include <algorithm>

// ============================================================
// SECTION 1: YOUR QUESTIONS ANSWERED UPFRONT
// ============================================================

// Q1: "why [=]?"
// [=] = capture ALL LOCAL variables by VALUE (make copies)
// -> lastResult captured as COPY (value -1 baked into lambda)
// -> modifying inside lambda modifies the COPY, not original
// -> WITHOUT mutable: even the copy is read-only (ERROR if you try to write)
// -> WITH mutable: copy becomes writable (but original still unchanged)

// Q2: "& without mutable will also work?"
// YES! [&] without mutable works because:
// -> [&] captures by REFERENCE
// -> reference variables are writable by nature
// -> no need for mutable (mutable only needed for VALUE captures)
// -> modifying captured variable MODIFIES the original

// ============================================================
// SECTION 2: GLOBAL VARIABLES AND CAPTURES
// ============================================================

int global = 1;          // global, external linkage (other files can see)
static int global1 = -1; // global, internal linkage (this file only)

// KEY RULE: GLOBAL VARIABLES ARE NEVER CAPTURED BY LAMBDA
// -> [=] or [&] only affect LOCAL variables
// -> globals are accessible directly inside lambda WITHOUT capture
// -> they don't need to be captured - already in scope everywhere!
//
// global  = 10;  // works inside lambda (no capture needed)
// global1 = 11;  // works inside lambda (no capture needed)
// static globals follow same rule as regular globals for lambda access

// ============================================================
// SECTION 3: COMPLETE CAPTURE SYNTAX TABLE
// ============================================================

// []              = capture nothing (no local vars accessible)
// [=]             = capture ALL locals by VALUE (copies)
// [&]             = capture ALL locals by REFERENCE
// [x]             = capture ONLY x by value
// [&x]            = capture ONLY x by reference
// [=, &x]         = all by value EXCEPT x by reference
// [&, x]          = all by reference EXCEPT x by value
// [this]          = capture current object (in member function lambdas)
// [*this]         = capture copy of current object (C++17)
//
// NOTE: globals, statics, constexpr = accessible without capture

// ============================================================
// SECTION 4: [=] vs [&] - WHEN MUTABLE IS NEEDED
// ============================================================

// [&] (reference):
// auto f = [&lastResult](int n) {  // or [&] for all
//     lastResult = n;   // OK: modifying via reference (no mutable needed)
// };
// -> reference is inherently writable
// -> modifies ORIGINAL variable
// -> mutable NOT required

// [=] (value) WITHOUT mutable:
// auto f = [=](int n) {
//     lastResult = n;   // COMPILER ERROR: copies are const by default
// };
// -> captured copies are READ-ONLY in non-mutable lambda
// -> lambda's operator() is const by default
// -> const means: all captured VALUE members are const

// [=] WITH mutable:
// auto f = [=](int n) mutable {
//     lastResult = n;   // OK: mutable makes copies writable
// };
// -> mutable removes const from lambda's operator()
// -> copies become writable
// -> BUT: original lastResult in main() STILL UNCHANGED
// -> only the internal copy is modified

// ============================================================
// SECTION 5: WHAT COMPILER GENERATES FOR [=] mutable
// ============================================================

// YOUR LAMBDA: [=](int n) mutable { global=10; global1=11; std::cout<<n<<","; }
//
// COMPILER GENERATES (approximately):
// struct __lambda {
//     int m_lastResult;  // VALUE copy (captured by =)
//                        // (even though not used in body, still captured!)
//
//     void operator()(int n)   // NOT const (because mutable!)
//     {
//         ::global  = 10;  // global: accessed directly, no member
//         ::global1 = 11;  // global1: accessed directly, no member
//         std::cout << n << ",";
//     }
// };
// auto print_v = __lambda{lastResult};  // copies lastResult into struct

// ============================================================
// SECTION 6: RAW FUNCTION POINTER vs LAMBDA
// ============================================================

// YOUR COMMENT: "Is essentially a 'raw function pointer'"
// void(*print_v)(int);
//
// A lambda WITH captures CANNOT be a raw function pointer!
// -> raw function pointer = stateless (just a code address)
// -> lambda with capture = HAS STATE (captured variables)
// -> these are fundamentally different
//
// ONLY a lambda WITH EMPTY CAPTURE [] can convert to function pointer:
// void(*fp)(int) = [](int n) { std::cout << n; };  // OK: no capture
// void(*fp)(int) = [=](int n) { ... };              // ERROR: has capture
//
// FOR lambdas WITH captures, use:
// std::function<void(int)> print_v = [=](int n) mutable { ... };
// or just: auto print_v = [=](int n) mutable { ... };

// ============================================================
// SECTION 7: main() TRACE
// ============================================================

int main()
{
    std::vector<int> v{1, 3, 2, 5, 9};

    int lastResult = -1;
    // lastResult2 = -1;

    auto print_v = [=](int n) mutable
    // [=]: capture lastResult by VALUE (copy = -1 baked in)
    // mutable: allow modifying the captured copies
    // globals NOT captured (accessible directly)
    {
        // lastResult = n;    // would modify the COPY (not main's lastResult)
        // lastResult2 = n;   // same: modifies copy
        global  = 10;  // global: NOT captured, modifies actual global directly
        global1 = 11;  // same: modifies actual static global directly
        std::cout << n << ",";
    };

    std::for_each(begin(v), end(v), print_v);
    // calls print_v(1), print_v(3), print_v(2), print_v(5), print_v(9)
    // each call: global=10, global1=11, prints n
    // (global and global1 set to same values each call - redundant but valid)
    // prints: 1,3,2,5,9,

    std::cout << '\n' << global  << std::endl;
    // global was 1, lambda set it to 10
    // prints: 10

    std::cout << '\n' << global1 << std::endl;
    // global1 was -1, lambda set it to 11
    // prints: 11

    // IF we uncommented: std::cout << lastResult
    // lastResult in main() would STILL be -1!
    // lambda modified its COPY (via mutable [=]), not the original
    // THIS IS THE KEY DIFFERENCE between [=] mutable and [&]

    return 0;
}

// OUTPUT:
// 1,3,2,5,9,
// 10
// 11

// ============================================================
// SECTION 8: SIDE-BY-SIDE COMPARISON OF ALL CAPTURE STYLES
// ============================================================

// Given: int lastResult = -1; int global = 1;

// STYLE 1: [&lastResult] - specific by reference
// auto f = [&lastResult](int n) { lastResult = n; };
// -> lastResult in main() IS modified (reference)
// -> global: accessible but NOT captured
// -> lastResult after for_each = 9 (last element)

// STYLE 2: [lastResult] - specific by value (read-only)
// auto f = [lastResult](int n) { /* lastResult = n; ERROR */ };
// -> lastResult in lambda = read-only copy of -1
// -> must add mutable to write to it (but still doesn't change main's var)

// STYLE 3: [&] - all locals by reference
// auto f = [&](int n) { lastResult = n; };  // no mutable needed
// -> ALL locals accessible by reference
// -> lastResult in main() IS modified
// -> simplest when you want to modify multiple outer vars

// STYLE 4: [=] - all locals by value (read-only)
// auto f = [=](int n) { /* lastResult = n; ERROR */ };
// -> ALL locals as read-only copies
// -> cannot modify any captured variable

// STYLE 5: [=] mutable (YOUR CODE) - all locals by value, writable copies
// auto f = [=](int n) mutable { lastResult = n; /* modifies COPY */ };
// -> ALL locals as WRITABLE copies (mutable removes const)
// -> copies can be changed but original vars UNCHANGED
// -> global still directly accessible and modifiable

// STYLE 6: [] - no capture
// auto f = [](int n) { /* lastResult = n; ERROR - not captured */ };
// -> cannot access any outer local variables
// -> CAN be converted to raw function pointer!

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does [=] capture do?
//     -> Captures ALL local variables in scope by VALUE (makes copies).
//        Copies are READ-ONLY by default (lambda operator() is const).
//        Add mutable to make copies writable.
//        Does NOT capture globals (they're accessible directly).

// Q2: What does mutable do on a lambda?
//     -> Removes const from the lambda's operator().
//        Makes value-captured copies WRITABLE.
//        Does NOT affect references captures (already writable).
//        Does NOT make original captured variables writable.

// Q3: Do lambdas capture global variables?
//     -> NO. Global variables (and static locals, thread_local) are
//        always accessible inside lambdas WITHOUT capture.
//        [=] and [&] only affect local variables.

// Q4: When can a lambda be converted to a raw function pointer?
//     -> ONLY when capture list is EMPTY [].
//        Lambda with any capture has state (closure object) and
//        cannot be represented as a plain function pointer.
//        Use std::function for type-erased storage of lambdas with captures.

// Q5: What is the difference between [=] mutable and [&]?
//     -> [=] mutable: copies captured, copies writable, originals UNCHANGED.
//        [&]: references captured, references writable, originals CHANGED.
//        Both allow modification inside lambda, but very different effects!

// Q6: Can you mix [=] and specific [&x] captures?
//     -> YES. [=, &x]: all by value except x (by reference).
//        [&, x]: all by reference except x (by value).
//        [=, &lastResult]: what you'd write to capture lastResult by ref
//        but everything else by value.

// Q7: What happens if a lambda with [&] outlives the captured variable?
//     -> DANGLING REFERENCE (undefined behavior).
//        Lambda holds reference to variable that no longer exists.
//        Example: return a lambda from a function that captures local vars.
//        The returned lambda has dangling refs to destroyed locals.

// Q8: What is the difference between static int and global for lambda access?
//     -> Both accessible without capture inside lambdas.
//        global (no static): external linkage (other .cpp files can access).
//        static global: internal linkage (only this .cpp file).
// //        For lambda capture behavior: no difference - both are globals.
// ```

// **The capture decision tree:**

// ```
// Need to MODIFY original variable?
// ├── YES → use [&x] or [&]          (reference, no mutable needed)
// │
// └── NO → need to READ original?
//     ├── YES → use [x] or [=]       (value copy, read-only, no mutable)
//     │
//     └── Need to MODIFY a local COPY (not original)?
//         └── use [=] mutable         (writable copies, original unchanged)
// ```

// **The global rule - always confuses people:**

// ```cpp
// int global = 1;   // global

// auto f = [=](int n) mutable {
//     global = 10;  // WORKS: no capture needed, globals always accessible
//     // local = n; // would need capture
// };
// // [=] only captures LOCALS, globals need no capture
// ```

// > **The key distinction for interviews**: `[=] mutable` and `[&]` both let you write to variables inside the lambda — but `[=] mutable` writes to a **private copy** (original unchanged), while `[&]` writes to the **original** directly. Choose based on whether you want the outer scope to see the change.