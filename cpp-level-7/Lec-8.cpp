// ============================================================
// MACROS, PREPROCESSOR, source_location - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <source_location>  // C++20

// ============================================================
// SECTION 1: WHAT IS THE PREPROCESSOR?
// ============================================================

// YOUR COMMENT: "anything that starts with '#' is part of the preprocessor"
// CORRECT! The preprocessor runs BEFORE compilation:
//
// YOUR CODE -> [PREPROCESSOR] -> pure C++ -> [COMPILER] -> object file
//
// Preprocessor does:
// #include   -> copy-paste file contents
// #define    -> text substitution
// #ifdef     -> conditional compilation (include/exclude code)
// #pragma    -> compiler hints
//
// Preprocessor knows NOTHING about C++ types, scope, or semantics
// It just does TEXT MANIPULATION on source code before compiler sees it
//
// YOUR QUESTION: "used in .hpp or header files?"
// -> Used in BOTH .hpp and .cpp files
// -> Most COMMON in .hpp: include guards (#ifndef/#pragma once)
//    and conditional feature detection (#ifdef _WIN32 etc.)
// -> In .cpp: debugging macros, platform-specific code

// ============================================================
// SECTION 2: #define - SIMPLE CONSTANT
// ============================================================

// #define PI 3.1415926
// -> preprocessor finds every "PI" in code and replaces with "3.1415926"
// -> NO type safety (just text)
// -> NO scope (visible everywhere after definition)
// -> NO debugger visibility (disappears after preprocessing)
//
// PREFER constexpr:
// constexpr float PI = 3.14159f;
// -> HAS type (float)
// -> HAS scope (follows normal scoping rules)
// -> visible in debugger
// -> compiler can optimize it
// -> catches narrowing errors: constexpr int x = PI; // warning/error
//
// #ifdef PI / #endif:
// -> "if PI is defined (as a macro), include this code"
// -> NOT checking if PI has a specific value, just if it EXISTS as a macro
// -> #define PI  (no value!) is enough to make #ifdef PI true
// -> used for feature flags, platform detection, debug modes

// ============================================================
// SECTION 3: #define DEBUG - CONDITIONAL COMPILATION
// ============================================================

// #define DEBUG 1  (or just #define DEBUG)
//
// #ifdef DEBUG
//     LOG(add(7, 2));  // this code INCLUDED in debug build
// #else
//     add(7, 2);       // this code INCLUDED in release build
// #endif
//
// COMPILE WITH DEBUG:    g++ -DDEBUG main.cpp  (defines DEBUG)
// COMPILE WITHOUT DEBUG: g++ main.cpp          (no DEBUG defined)
//
// IN YOUR CODE:
// DEBUG is NOT defined (no #define DEBUG in active code)
// -> #ifdef DEBUG is FALSE
// -> add(7, 2) runs (the #else branch)
// -> LOG macro is never expanded

// ============================================================
// SECTION 4: YOUR LOG MACRO - ANATOMY
// ============================================================

#define LOG(param)                 \
    std::cout << "LOG:" << #param; \
    param;                         \
    std::cout << std::endl;
//
// MULTI-LINE macro: backslash \ continues the definition on next line
// (no space after \, just newline)
//
// #param = STRINGIFICATION operator
// -> converts the macro argument to a string literal
// -> #param where param=add(7,2)  ->  "add(7,2)" (the text!)
// -> allows printing "LOG:add(7,2)" to show WHAT was logged
//
// EXPANSION of LOG(add(7,2)):
// std::cout << "LOG:" << "add(7,2)";  // #param = stringified
// add(7, 2);                           // param = actual call
// std::cout << std::endl;
//
// RESULT:
// LOG:add(7,2)
// [then source_location output from inside add()]
// [newline]

// ============================================================
// SECTION 5: MACRO DANGERS - WHY PREFER constexpr/inline
// ============================================================

// DANGER 1: No type safety
// #define SQUARE(x) x * x
// SQUARE(2+3)  -> 2+3 * 2+3 = 2+6+3 = 11  (NOT 25!)
// FIX: #define SQUARE(x) ((x) * (x))  (extra parentheses)
// BETTER: constexpr auto square(auto x) { return x * x; }

// DANGER 2: Multiple evaluation
// #define MAX(a,b) ((a) > (b) ? (a) : (b))
// int i = 5;
// MAX(i++, 3)  -> ((i++) > (3) ? (i++) : (3))
// i++ evaluated TWICE! -> undefined behavior

// DANGER 3: No scope
// #define SIZE 10  // visible everywhere after this line
// later in code: int SIZE = 20;  // redefinition conflict!

// DANGER 4: Hard to debug
// Compiler errors point to the EXPANSION, not the macro
// "error in line 5" when macro was defined on line 2 -> confusing

// ============================================================
// SECTION 6: __LINE__, __FILE__, __FUNCTION__ - BUILT-IN MACROS
// ============================================================

// C++ provides built-in preprocessor macros:
// __LINE__     = current line number (integer)
// __FILE__     = current file name (string)
// __FUNCTION__ = current function name (string, not standard but widely supported)
// __DATE__     = compilation date "Jan 29 2026"
// __TIME__     = compilation time "10:36:28"
//
// USAGE (your commented code):
// std::cout << __LINE__ << ":" << __FILE__ << std::endl;
// prints: "15:main.cpp" (or whatever the actual line/file is)
//
// LIMITATION:
// void log(std::string msg) {
//     std::cout << __FILE__ << ":" << __LINE__;  // always points to HERE
//                                                  // NOT where log() was CALLED!
// }
// log("error");   // shows log()'s file/line, not main()'s line!

// ============================================================
// SECTION 7: std::source_location (C++20) - THE MODERN FIX
// ============================================================

int add(int a, int b)
{
    // OLD WAY: __LINE__, __FILE__ - shows add()'s location, not caller's
    // std::cout << __LINE__ << ":" << __FILE__;

    // NEW WAY (C++20): source_location
    std::source_location location = std::source_location::current();
    // current() called HERE captures THIS location (inside add())
    //
    // TO capture CALLER's location (more useful for logging):
    // int add(int a, int b,
    //         std::source_location loc = std::source_location::current())
    //                                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //                                    default arg evaluated at CALL SITE!
    //                                    -> gives caller's location, not add's
    // add(7, 2);  -> loc shows where add() was called (main.cpp line X)

    std::cout << "File:     " << location.file_name()     << std::endl;
    std::cout << "Function: " << location.function_name() << std::endl;
    std::cout << "Line:     " << location.line()          << std::endl;
    std::cout << "Column:   " << location.column()        << std::endl;

    return a + b;
}

// ============================================================
// SECTION 8: THE REAL POWER - source_location AS DEFAULT PARAMETER
// ============================================================

// PATTERN that makes source_location actually useful:
//
// void Log(std::string msg,
//          std::source_location loc = std::source_location::current())
// {
//     std::cout << loc.file_name() << ":"
//               << loc.line() << " "
//               << loc.function_name() << ": "
//               << msg << "\n";
// }
//
// int main() {
//     Log("something happened");  // loc captured at THIS call site!
//     Log("another event");       // shows main.cpp line X
// }
//
// OUTPUT:
// main.cpp:5 main: something happened    <- points to main's line!
// main.cpp:6 main: another event
//
// vs __LINE__/__FILE__: would ALWAYS show Log()'s location, not caller's
// This is the ENTIRE POINT of std::source_location:
// capture location at CALL SITE, not definition site

// ============================================================
// SECTION 9: YOUR CODE TRACE - NO DEBUG DEFINED
// ============================================================

int main()
{
    // #ifdef DEBUG -> FALSE (DEBUG not defined)
    //     LOG(add(7, 2));  <- SKIPPED
    // #else
         add(7, 2);         // <- RUNS this branch
    // #endif
    // add() prints source_location info:
    // File:     main.cpp
    // Function: int add(int, int)
    // Line:     (line of source_location::current() call)
    // Column:   (column number)
    // returns 9 (ignored)

    // #ifdef PI -> FALSE (PI not defined as macro)
    // #else
         // std::cout << 22 / 7.0f  <- RUNS
    // #endif
    // 22 / 7.0f = 3.142857...
    // prints: 3.14286

    return 0;
}

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the C++ preprocessor?
//     -> Runs BEFORE compilation. Handles # directives: includes,
//        defines, conditional compilation. Does text manipulation
//        with no knowledge of C++ types or scope.

// Q2: Why prefer constexpr over #define for constants?
//     -> constexpr has type, scope, debugger visibility, and compile-time
//        checks. #define is untyped text replacement, no scope, invisible
//        to debugger, can cause subtle bugs.

// Q3: What does #param (stringification) do in a macro?
//     -> Converts the macro argument to a string literal.
//        LOG(add(7,2)) -> #param becomes "add(7,2)" (the text as-is).
//        Allows printing the expression that was logged.

// Q4: What is the limitation of __LINE__ and __FILE__?
//     -> They capture the location WHERE THEY APPEAR in source code,
//        not where the containing function was called from.
//        A logging function using __LINE__ always shows the log
//        function's own location, not the caller's.

// Q5: How does std::source_location solve the __LINE__ problem?
//     -> As a DEFAULT PARAMETER with value source_location::current(),
//        it's evaluated at the CALL SITE, not inside the function.
//        The log function gets the caller's file/line/function info.

// Q6: What is #ifdef and when is it used?
//     -> Conditional compilation: include code only if a macro is defined.
//        Used for: debug vs release builds, platform-specific code,
//        feature flags, include guards (traditionally).

// Q7: What are the dangers of function-like macros?
//     -> No type safety, no scope, double evaluation of arguments,
//        operator precedence surprises ((x+y)*z becomes x+y*z without
//        parentheses), hard to debug. Prefer inline functions or templates.

// Q8: What information does std::source_location provide?
//     -> file_name(): source file path
//        function_name(): enclosing function name
//        line(): line number
//        column(): column number
//        All captured at compile time. Requires C++20 (<source_location>).
// ```

// **The source_location trick - the real interview answer:**

// ```cpp
// // WRONG way (shows log function's location, not caller's):
// void Log(std::string msg) {
//     std::cout << __FILE__ << ":" << __LINE__;  // always "log.cpp:3"
// }

// // RIGHT way (shows CALLER's location):
// void Log(std::string msg,
//          std::source_location loc = std::source_location::current())
// //                                   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// //                                   evaluated WHERE Log() IS CALLED
// {
//     std::cout << loc.file_name() << ":" << loc.line();
// }

// Log("error!");  // prints: main.cpp:15  ← caller's actual location!
// ```

// **Preprocessor vs compiler - what each sees:**

// ```
// YOUR CODE:        #define SQUARE(x) ((x)*(x))
//                   int r = SQUARE(2+3);

// PREPROCESSOR:     int r = ((2+3)*(2+3));    ← text substitution done

// COMPILER:         sees the expanded code, compiles it
//                   (never sees the macro name at all)
// ```

// > **The bottom line**: macros are a C legacy that C++ is gradually replacing — `constexpr` replaces `#define` constants, `inline` functions replace function macros, `std::source_location` replaces `__LINE__`/`__FILE__`. The only macros with no modern replacement are `#ifdef` for conditional compilation (platform/debug flags) and include guards (though `#pragma once` is widely used instead).