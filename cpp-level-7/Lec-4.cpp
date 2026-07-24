// ============================================================
// CAPTURING 'this' IN MEMBER FUNCTION LAMBDAS - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: YOUR COMPILER FLAGS QUESTION
// ============================================================

// g++ -g -Wall -Werror -std=c++20 Lec-4.cpp -o main
//
// -g          = include DEBUG SYMBOLS in binary
//               -> lets debugger (gdb, lldb) show line numbers, variable names
//               -> makes binary larger but enables step-through debugging
//               -> REMOVE for release builds
//
// -Wall       = enable ALL common WARNINGS
//               -> signed/unsigned comparison, unused variables,
//                  missing return, shadowed variables etc.
//               -> YOU SHOULD ALWAYS USE THIS
//
// -Werror     = treat all WARNINGS as ERRORS
//               -> code won't compile if any warning exists
//               -> forces you to fix ALL warnings before building
//               -> professional codebases use this to maintain quality
//               -> very strict: even unused parameter = compile error
//
// -std=c++20  = use C++20 standard
//               -> enables: concepts, ranges, coroutines, std::span etc.
//               -> [=, this] in lambda (C++20 change for this capture)
//
// -o main     = output file named 'main'
//               -> without -o: default output is 'a.out'

// ============================================================
// SECTION 2: WHY CAPTURING 'this' IS NEEDED IN MEMBER FUNCTIONS
// ============================================================

// PROBLEM: lambda inside a member function cannot access member variables
// by default (even though the member function can)
//
// struct MyStruct {
//     int counter{0};
//     void Functions() {
//         auto f = []() {
//             counter++;  // COMPILER ERROR!
//                         // counter is a member variable
//                         // lambda doesn't know about 'this'
//         };
//     }
// };
//
// WHY? lambda creates a NEW scope
// 'counter' is actually 'this->counter'
// without capturing 'this', lambda has no access to 'this' pointer
// -> must explicitly capture 'this' to access member variables

// ============================================================
// SECTION 3: ALL FOUR WAYS TO CAPTURE 'this'
// ============================================================

// WAY 1: [this] - capture this POINTER by reference (not a copy!)
// auto f = [this]() {
//     counter++;  // this->counter++ - modifies the ACTUAL object
// };
// -> captures the POINTER itself (not a copy of the object)
// -> [this] = reference to the object (via pointer)
// -> modifying counter changes the REAL counter in the struct
// -> DANGER: if object destroyed before lambda executes -> dangling this
// -> YOUR COMMENT: "this is ref here" -> CORRECT!
//    (capturing this pointer = you can modify the object via pointer)

// WAY 2: [*this] - capture COPY of the object (C++17)
// auto f = [*this]() mutable {
//     counter++;  // modifies the COPY's counter, not original!
// };
// YOUR QUESTION: "why use mutable with [*this]?"
// -> [*this] makes a COPY of the entire struct object
// -> by default, that copy is CONST (operator() is const)
// -> counter++ on a const copy -> COMPILER ERROR
// -> mutable makes the copy's operator() non-const -> writable
// -> modifying counter modifies COPY's counter, original unchanged
// -> SAFE: object can be destroyed, lambda has its own complete copy
// -> EXPENSIVE: copies entire struct (could be large!)

// WAY 3: [obj = this] - named capture (alias for this)
// auto f = [obj = this]() {
//     obj->counter++;  // obj is just another name for this
// };
// YOUR QUESTION: "obj = this??"
// -> creates a local alias 'obj' pointing to the SAME object as this
// -> obj->counter = this->counter (same thing!)
// -> useful when: you want a clearer name, or when 'this' might change
//    (e.g., in async code where 'this' capture needs explicit naming)
// -> same behavior as [this] but more explicit

// WAY 4: [&] - capture all by reference (INCLUDES this implicitly)
// auto f = [&]() {       // YOUR ACTUAL CODE
//     counter++;  // works! [&] captures this, so this->counter accessible
// };
// -> [&] captures ALL local variables AND 'this' by reference
// -> most convenient, counter modifies the real object
// -> YOUR COMMENT: "using [=] is deprecated, it will take by value 
//                  and counter will be 0"
// -> IN C++20: [=] no longer implicitly captures 'this' (deprecated!)
//    use [=, this] explicitly if you need both value captures AND this

// ============================================================
// SECTION 4: THE [=] DEPRECATION (C++20) - YOUR COMMENT
// ============================================================

// PRE-C++20: [=] IMPLICITLY captured 'this'
// auto f = [=]() {
//     counter++;  // worked! [=] silently captured this
// };
//
// C++20: [=] no LONGER captures 'this' implicitly (deprecated warning!)
// auto f = [=]() {
//     counter++;  // WARNING/ERROR: this not captured!
//                  // counter would be 0 or garbage
// };
//
// C++20 FIX: explicitly capture 'this' alongside [=]:
// auto f = [=, this]() {
//     counter++;  // OK: explicit this capture
// };
// OR: just use [&] or [this] directly
//
// YOUR COMMENT IS CORRECT:
// "using [=] is deprecated - it will take by value and counter will be 0"
// -> [=] makes copies of LOCALS but in C++20 no longer grabs 'this'
// -> counter (which is this->counter) becomes inaccessible

// ============================================================
// SECTION 5: YOUR ACTUAL CODE
// ============================================================

struct MyStruct
{
    void Functions()
    {
        auto f = [&]()
        // [&] captures ALL local variables AND 'this' by reference
        // -> counter accessible as this->counter
        // -> modifies the REAL counter in the MyStruct object
        {
            counter++;
            // = this->counter++
            // modifies actual counter in the struct instance

            std::cout << "counter inside lambda: "
                      << counter << std::endl;
        };

        f();
        // calls lambda once
        // counter incremented: 0->1 (first call), 1->2 (second), etc.

        std::cout << "counter: " << counter << std::endl;
        // prints same value as inside lambda (same counter!)
    }

    int counter{0};
};

// ============================================================
// SECTION 6: main() TRACE
// ============================================================

int main()
{
    MyStruct instance;
    // instance.counter = 0

    instance.Functions();
    // f() called: counter++ -> 0->1
    // prints: "counter inside lambda: 1"
    // prints: "counter: 1"

    instance.Functions();
    // f() called: counter++ -> 1->2
    // prints: "counter inside lambda: 2"
    // prints: "counter: 2"

    instance.Functions();
    // counter++ -> 2->3
    // prints: "counter inside lambda: 3"
    // prints: "counter: 3"

    instance.Functions();
    // counter++ -> 3->4
    // prints: "counter inside lambda: 4"
    // prints: "counter: 4"

    return 0;
}

// FULL OUTPUT:
// counter inside lambda: 1
// counter: 1
// counter inside lambda: 2
// counter: 2
// counter inside lambda: 3
// counter: 3
// counter inside lambda: 4
// counter: 4

// ============================================================
// SECTION 7: COMPLETE COMPARISON TABLE
// ============================================================

// | Capture   | 'this' captured? | Modifies original? | Copy? | C++17+ |
// |-----------|------------------|-------------------|-------|--------|
// | [this]    | YES (pointer)    | YES               | NO    | YES    |
// | [*this]   | YES (by copy)    | NO (own copy)     | YES   | YES    |
// | [obj=this]| YES (named ptr)  | YES               | NO    | YES    |
// | [&]       | YES (implicit)   | YES               | NO    | YES    |
// | [=]       | C++20: NO!       | N/A (deprecated)  | YES   | NO     |
// | [=,this]  | YES (explicit)   | YES               | NO    | C++20  |

// ============================================================
// SECTION 8: PRACTICAL ADVICE - WHICH TO USE
// ============================================================

// SIMPLE CASE (most code):
// [&] -> simplest, works, captures everything including this
//        fine when lambda is short-lived (called immediately)

// ASYNC / STORED LAMBDA:
// [*this] -> makes copy of entire object
//            safe if original object might be destroyed
//            expensive if object is large
//
// SHARED POINTER PATTERN (most robust for async):
// class MyClass : public std::enable_shared_from_this<MyClass> {
//     void StartAsync() {
//         auto self = shared_from_this();   // shared_ptr to this
//         async_op([self]() {
//             self->counter++;   // object kept alive by shared_ptr!
//         });
//     }
// };

// EXPLICIT IS BETTER:
// [this] over [&] when you ONLY need this (not other locals)
// -> clearer intent: "I only need the object, not local variables"

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: Why can't a lambda in a member function access member variables
//     without capturing 'this'?
//     -> Member variables are accessed as this->member. Lambda has its
//        own scope and no implicit access to 'this'. Must explicitly
//        capture 'this' (or use [&] which captures it implicitly).

// Q2: What is the difference between [this] and [*this]?
//     -> [this]:  captures the THIS POINTER (reference to object).
//                  modifies original object. Dangerous if object destroyed.
//        [*this]: captures a COPY of the entire object (C++17).
//                  modifies only the copy. Safe if original destroyed.
//                  Requires mutable to modify copied members.

// Q3: Why was [=] deprecated for capturing 'this' in C++20?
//     -> [=] implicitly captured 'this' which was surprising and error-prone.
//        In C++20, [=] only captures local variables by value.
//        Use [=, this] explicitly if you need both.
//        Compiler flags like -Wall will warn about this usage.

// Q4: What does [*this] mutable do?
//     -> [*this]: copies the entire struct object into the lambda.
//        mutable: makes the copy writable (otherwise counter++ would fail).
//        Together: lambda has its own modifiable copy of the struct.
//        Changes inside lambda don't affect the original object.

// Q5: What does -Werror do in g++ compilation?
//     -> Treats all warnings as errors. Code won't compile if ANY
//        warning exists. Forces developers to fix all warnings.
//        Used in professional codebases to maintain code quality.

// Q6: What is -g flag in g++?
//     -> Includes debug symbols (variable names, line numbers) in binary.
//        Required for debuggers (gdb, lldb) to work properly.
//        Increases binary size. Remove for release/production builds.

// Q7: Why does counter keep incrementing across calls to Functions()?
//     -> Lambda captures 'this' (via [&]), so counter is this->counter.
//        this->counter lives in the MyStruct instance object.
//        Each call to Functions() creates a new lambda but the same
//        instance.counter persists between calls (member variable).

// Q8: What is [obj = this] and when would you use it?
//     -> Creates a named capture 'obj' as an alias for the this pointer.
//        obj->counter = this->counter (same object, different name).
//        Used for clarity, or in async code where you want explicit naming
//        of the captured object pointer, or when 'this' semantics could
//        be ambiguous (nested lambdas etc.).
// ```

// **The four ways to capture `this`:**

// ```cpp
// auto f = [this]()        { counter++; }  // pointer: modifies original
// auto f = [*this]() mutable { counter++; }  // copy: modifies copy only
// auto f = [obj=this]()   { obj->counter++; } // named alias: same as [this]
// auto f = [&]()           { counter++; }  // all by ref: includes this
// ```

// **The C++20 deprecation trap:**

// ```cpp
// // C++17 and before: [=] silently captured 'this':
// auto f = [=]() { counter++; };  // worked (bad surprise!)

// // C++20: [=] no longer captures 'this':
// auto f = [=]() { counter++; };  // WARNING/ERROR: counter not accessible!

// // C++20 FIX:
// auto f = [=, this]() { counter++; }; // explicit this + value captures
// auto f = [&]()        { counter++; }; // or just use [&]
// ```

// **The `-Wall -Werror` combo:**

// ```
// -Wall   = "tell me about ALL problems"
// -Werror = "refuse to compile if there are any problems"
// Together = zero-tolerance for sloppy code
// Used in: professional C++ codebases, Qt projects, your ISSST work
// ```

// > **The practical rule**: use `[&]` for lambdas called immediately (most common). Use `[*this]` or `shared_from_this()` for lambdas stored and called later (async callbacks, Qt signals) — when the original object might not exist by the time the lambda runs. This distinction is **critical** in Qt/event-driven code where lambdas are stored in signal connections.