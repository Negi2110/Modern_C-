// ============================================================
// assert AND static_assert - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <cassert>  // for assert (runtime)
// static_assert is a keyword - no header needed (C++11)

// ============================================================
// SECTION 1: WHAT IS AN ASSERTION?
// ============================================================

// assertion = a CHECK that something MUST be true
// if the check FAILS -> program signals an ERROR immediately
// "if this isn't true, something is fundamentally wrong"
//
// WITHOUT assertions:
// checkAge(-5) -> m_age = -5 -> used elsewhere -> crash 200 lines later
// "why did the program crash??" (hard to debug)
//
// WITH assertions:
// checkAge(-5) -> assert(age > 0) FAILS immediately
// "assertion failed: age > 0" at the exact line
// (easy to find the bug!)
//
// TWO TYPES:
// assert        -> RUNTIME check (happens when program runs)
// static_assert -> COMPILE TIME check (happens during compilation)

// ============================================================
// SECTION 2: assert - RUNTIME ASSERTION
// ============================================================

// #include <cassert>
//
// assert(condition);
// assert(condition && "message");  // trick to add message
//
// IF condition is TRUE:  nothing happens, continues normally
// IF condition is FALSE: prints error message + ABORTS program
//
// assert(age > 0 && "age was less than zero");
// -> if age <= 0: program aborts with:
//    "Assertion failed: (age > 0 && "age was less than zero"),
//     function checkAge, file main.cpp, line 8."
//
// WHY && "message string"?
// -> "message string" is a const char* which is always truthy (non-null)
// -> true && "message" = true (condition unchanged)
// -> false && "message" = false (still fails correctly)
// -> BUT the string appears in the error output for context
// -> clever hack since assert() only takes ONE argument
//
// ASSERT IS DISABLED IN RELEASE BUILDS:
// #define NDEBUG      // defining this DISABLES all assert() calls
// -> in debug builds: assertions run (slower but safe)
// -> in release builds: assertions removed (faster, no safety net)
// -> controlled by NDEBUG preprocessor macro
// -> compiler usually defines NDEBUG for release (-DNDEBUG flag)

// void checkAge(int inputAge)
// {
//     assert(inputAge > 0 && "age was less than zero");
//     // RUNTIME: checked when function actually called
//     // if inputAge is negative at runtime -> abort with message
// }

// ============================================================
// SECTION 3: static_assert - COMPILE TIME ASSERTION
// ============================================================

// static_assert(constant_expression, "error message");
// C++11: message is MANDATORY
// C++17: message is OPTIONAL
//
// IF expression is TRUE:  nothing, compilation continues
// IF expression is FALSE: COMPILATION FAILS with your message
// -> your program never even BUILDS if assertion fails
// -> caught before you can even run the program!
//
// EXPRESSION MUST BE:
// -> evaluatable at COMPILE TIME
// -> constexpr or literal
// -> cannot depend on runtime values

constexpr int add(int a, int b) {
    return a + b;
}

int main()
{
    // STATIC ASSERTION 1: verifying constexpr function result
    static_assert(add(2, 2) == 4, "2+2 should equal 4");
    // -> add(2,2) computed at COMPILE TIME = 4
    // -> 4 == 4 -> TRUE -> compilation continues
    // -> if add() were broken and returned 5:
    //    COMPILE ERROR: "2+2 should equal 4"
    //    (caught during compilation, not at runtime!)

    // STATIC ASSERTION 2: verifying platform assumptions
    static_assert(sizeof(int) == 4, "int is 4 bytes");
    // -> sizeof(int) known at compile time
    // -> on 64-bit desktop: 4 == 4 -> TRUE -> OK
    // -> on some embedded/16-bit systems: sizeof(int) might be 2
    //    COMPILE ERROR: "int is 4 bytes"
    //    (prevents subtle bugs from wrong size assumptions!)
    //
    // THIS IS THE MOST POWERFUL USE:
    // your code assumes int = 4 bytes
    // if compiled on platform where it's 2 bytes -> wrong results
    // static_assert CATCHES THIS at compile time on that platform

    // CONSTEXPR + STATIC_ASSERT:
    // constexpr int age = 7;
    // static_assert(age > 0, "message");
    // -> age is constexpr (compile-time) -> static_assert can check it
    // -> 7 > 0 -> TRUE -> OK (just shows a warning in some IDEs)
    //
    // static_assert(age > 100, "too young");
    // -> 7 > 100 -> FALSE -> COMPILE ERROR: "too young"

    return 0;
}

// ============================================================
// SECTION 4: assert vs static_assert - COMPLETE COMPARISON
// ============================================================

// | Feature              | assert              | static_assert        |
// |----------------------|---------------------|----------------------|
// | When checked         | RUNTIME             | COMPILE TIME         |
// | Header needed        | <cassert>           | none (keyword)       |
// | Can be disabled      | YES (NDEBUG)        | NO (always on)       |
// | Expression type      | any bool expression | constexpr only       |
// | On failure           | abort() at runtime  | compilation fails    |
// | Message support      | && "string" trick   | second argument      |
// | Performance impact   | yes (debug builds)  | zero (compile time)  |
// | Available since      | C (via C++)         | C++11                |

// ============================================================
// SECTION 5: WHEN TO USE WHICH
// ============================================================

// USE assert WHEN:
// -> checking runtime conditions (user input, function args)
// -> validating invariants during development
// -> checking values that only exist at runtime (array index, pointer)
// -> want to disable checks in release builds for performance
//
// assert(ptr != nullptr);              // runtime pointer check
// assert(index >= 0 && index < size); // runtime bounds check
// assert(age > 0 && "negative age");  // runtime value check

// USE static_assert WHEN:
// -> verifying compile-time constants and types
// -> checking platform/architecture assumptions
// -> verifying template arguments
// -> ensuring type sizes match protocol requirements (DIS PDUs!)
// -> checking constexpr function results
//
// static_assert(sizeof(int) == 4, "need 32-bit int");
// static_assert(sizeof(MyPDU) == 128, "PDU must be exactly 128 bytes");
// static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
// static_assert(N > 0, "array size must be positive");

// ============================================================
// SECTION 6: static_assert IN TEMPLATES - POWERFUL USE
// ============================================================

// template<typename T, size_t N>
// class Container {
//     static_assert(N > 0, "Container size must be positive");
//     static_assert(std::is_trivially_copyable_v<T>,
//                   "T must be trivially copyable for this container");
//     T m_data[N];
// };
//
// Container<int, 0> c;    // COMPILE ERROR: "Container size must be positive"
// Container<std::string, 5> c2; // COMPILE ERROR: "must be trivially copyable"
// Container<int, 5> c3;   // OK
//
// MUCH better than:
// -> running program and getting weird behavior because N=0
// -> debugging why string objects cause crashes in this container
// Static assert catches these at compile time!

// ============================================================
// SECTION 7: ISSST/DIS PROTOCOL USE CASE
// ============================================================

// In DIS/protocol work (YOUR ISSST project):
// PDUs must have EXACT sizes for network compatibility
//
// struct EntityStatePDU {
//     uint16_t protocol_version;
//     uint16_t exercise_id;
//     uint8_t  pdu_type;
//     // ... more fields
// };
//
// static_assert(sizeof(EntityStatePDU) == 144,
//               "EntityStatePDU must be exactly 144 bytes for DIS compliance");
//
// -> if you add/remove a field accidentally
// -> or if padding changes on a new platform
// -> COMPILE ERROR immediately with clear message
// -> prevents sending malformed PDUs that would break simulation!
//
// This is a REAL use case for static_assert in protocol/embedded work

// ============================================================
// SECTION 8: assert WITH NDEBUG
// ============================================================

// COMPILE with assertions enabled (debug mode):
// g++ -g main.cpp              -> NDEBUG not defined -> assert() runs
//
// COMPILE without assertions (release mode):
// g++ -O2 -DNDEBUG main.cpp   -> NDEBUG defined -> assert() = nothing
//
// OR in code:
// #define NDEBUG  // put BEFORE #include <cassert>
// #include <cassert>
// now assert() does NOTHING (compiled out)
//
// Qt projects: CMAKE_BUILD_TYPE=Debug   -> no NDEBUG (asserts run)
//              CMAKE_BUILD_TYPE=Release -> NDEBUG defined (asserts removed)
//
// WARNING: don't put side effects in assert!
// assert(ptr = new int(5));  // DANGEROUS!
// -> in debug: works (ptr gets allocated)
// -> in release: assert removed -> ptr never allocated -> crash!
// assert should only CHECK, never DO anything

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between assert and static_assert?
//     -> assert: runtime check, aborts on failure, can be disabled.
//        static_assert: compile-time check, compilation fails on failure,
//        cannot be disabled, expression must be constexpr.

// Q2: What does NDEBUG do?
//     -> Defining NDEBUG disables all assert() calls (compiled to nothing).
//        Typically defined in release builds for performance.
//        static_assert is NEVER affected by NDEBUG (always compile-time).

// Q3: Why use static_assert(sizeof(int)==4)?
//     -> Code that assumes int is 4 bytes will misbehave silently on
//        platforms where it's 2 bytes. static_assert catches this at
//        compile time on the problematic platform with a clear message.

// Q4: Why does assert(condition && "message") work?
//     -> "message" is const char* (non-null pointer = truthy).
//        true && true = condition unchanged.
//        false && true = still false (assertion still fails).
//        The string appears in the abort message for context.

// Q5: Can static_assert check runtime values?
//     -> NO. Expression must be a compile-time constant (constexpr).
//        static_assert(x > 0) where x is a runtime int = COMPILE ERROR.
//        Only constexpr variables, literals, sizeof, typeof etc.

// Q6: Where is static_assert most useful in templates?
//     -> Validating template arguments: size > 0, type requirements
//        (trivially copyable, is_integral etc.).
//        Gives clear compile error at the call site instead of
//        cryptic error deep inside template instantiation.

// Q7: What happens if static_assert fails?
//     -> Compilation STOPS with an error message showing:
//        file, line, and the message you provided.
//        Program is never built or run. Caught at compile time.

// Q8: Why is assert() dangerous with side effects?
//     -> assert(ptr = malloc(10)) works in debug but ptr is never
//        assigned in release (assert compiled out).
//        assert should only CHECK conditions, never perform actions.
//        Side-effect-free expressions only inside assert().
// ```

// **assert vs static_assert - the essential difference:**

// ```cpp
// // RUNTIME (assert):
// assert(age > 0);          // checked when program RUNS
//                           // can be disabled with NDEBUG
//                           // useful for runtime values

// // COMPILE TIME (static_assert):
// static_assert(sizeof(int) == 4, "need 32-bit int");  // checked during COMPILATION
//                                                         // CANNOT be disabled
//                                                         // only for compile-time values
// ```

// **The ISSST connection:**

// ```cpp
// // DIS protocol requires EXACT byte sizes:
// static_assert(sizeof(EntityStatePDU) == 144,
//               "EntityStatePDU must be 144 bytes for DIS compliance");
// // If padding changes, field added, or wrong platform:
// // COMPILE ERROR immediately → never send a malformed PDU!
// ```

// > **The interview answer that impresses**: `static_assert` moves bug detection from **runtime** (when users experience it) to **compile time** (when you write the code). In protocol work like DIS/ISSST, it's especially powerful because you can guarantee struct sizes match network format requirements before a single byte is ever sent.