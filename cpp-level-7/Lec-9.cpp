// ============================================================
// inline FUNCTIONS - PERFORMANCE, TRADE-OFFS - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS inline AND WHY?
// ============================================================

// NORMAL FUNCTION CALL OVERHEAD (your comment explains this perfectly):
// "add() - args, body, ret, return addr"
// "every time I call add I have to go up to this block where add is...
//  there is activation context... making copy of arguments,
//  return address generation, generating return value"
//
// WHAT HAPPENS EACH CALL TO add(4, 5):
// 1. PUSH arguments (4 and 5) onto stack
// 2. PUSH return address (where to come back to in main)
// 3. JUMP to add() function location in memory
// 4. CREATE stack frame (allocate space for local vars)
// 5. EXECUTE function body
// 6. COPY return value to return register
// 7. POP stack frame
// 8. JUMP BACK to return address in main
// 9. READ return value from register
//
// FOR A TINY FUNCTION like add(a,b):
// The overhead (steps 1-4 + 7-9) might cost MORE than the actual work (step 5)!
// -> a + b is ONE instruction
// -> the call machinery around it might be 10+ instructions
// -> THIS is the performance problem inline solves

// ============================================================
// SECTION 2: HOW INLINE SOLVES IT
// ============================================================

// YOUR COMMENT: "we just insert add into main like 4+5, compute result there"
// EXACTLY RIGHT!
//
// WITHOUT inline:                  WITH inline:
// main:                            main:
//   push 4                           // no push, no jump, no return
//   push 5                           int value = 4 + 5;  // just this!
//   call add                         // compiler inserts the body HERE
//   mov value, eax                   // "locality" - next instruction known
//
// inline says: "compiler, please COPY the function body to every call site"
// -> no call overhead
// -> no stack frame for the inlined function
// -> CPU's branch predictor works better (no jump to unknown address)
//
// YOUR COMMENT: "locality - we can guess the next instruction easily (hardware)"
// -> CPU has a branch predictor that speeds up sequential code
// -> function calls = jump to distant memory = branch predictor struggles
// -> inlined code = sequential in memory = branch predictor happy

// ============================================================
// SECTION 3: inline IS A HINT, NOT A GUARANTEE
// ============================================================

// inline keyword = REQUEST to compiler, not a COMMAND
// Compiler may IGNORE it if:
// -> function is too large (increases code size too much)
// -> function is recursive (can't inline recursion)
// -> function contains loops (compiler judges not worth it)
// -> debug mode (inlining makes debugging harder)
//
// Compiler may INLINE even WITHOUT keyword:
// -> modern compilers (with -O2/-O3) inline automatically
// -> small functions defined in headers are automatically candidates
// -> "implicit inline" for class member functions defined in class body
//
// SO: 'inline' keyword today mostly matters for:
// 1. LINKER: allows multiple definitions across translation units
// 2. HINT: suggests inlining (but compiler decides)

// ============================================================
// SECTION 4: __attribute__((always_inline)) - FORCE IT
// ============================================================

__attribute__((__always_inline__)) inline auto add(int a, int b)
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// GCC/Clang extension: FORCE inlining regardless of compiler judgment
// NOT standard C++ (compiler extension)
// -> overrides compiler's decision not to inline
// -> even in debug mode
// -> even if function is large
//
// EQUIVALENT in MSVC: __forceinline
// __forceinline auto add(int a, int b) { return a+b; }
//
// STANDARD C++20 alternative:
// [[likely]] [[unlikely]] -> hints for branch prediction
// (no standard C++ attribute to force inlining exists yet)
//
// USE __attribute__((always_inline)) WHEN:
// -> hot path in performance-critical code
// -> profiler shows function call overhead is a bottleneck
// -> embedded systems with zero tolerance for call overhead
// -> ISSST simulation tick loop (called thousands of times per second)
{
    int d = 9;   // local variables - will these be inlined too?
    int e = 3;   // YES! entire body is inserted at call site
    int f = 8;   // but compiler will likely OPTIMIZE AWAY d,e,f,g
    int g = 19;  // since they're never used -> DEAD CODE ELIMINATION
    return a + b;
    // with always_inline: becomes literally "4 + 5 = 9" in main
    // d,e,f,g? compiler removes them (unused variables with optimization)
}

// ============================================================
// SECTION 5: TRADE-OFFS - YOUR COMMENTS
// ============================================================

// BENEFIT 1: PERFORMANCE
// -> no call overhead (no push, no jump, no return)
// -> better cache locality (code stays in instruction cache)
// -> enables further optimizations (constant folding, dead code elim)
// -> add(4, 5) -> compiler sees LITERAL 4+5 -> computes 9 at compile time!

// COST 1: CODE SIZE INCREASE
// YOUR COMMENT: "my code size increases"
// -> every call site gets a COPY of the function body
// -> add() called 1000 times = 1000 copies of add's code in binary
// -> larger binary = more instruction cache pressure
// -> can HURT performance if binary gets too large (cache thrashing)
// -> small functions: fine. Large functions: rarely worth it

// COST 2: DEBUGGING DIFFICULTY
// YOUR COMMENT: "debugging maybe more difficult"
// -> inlined code has no separate stack frame
// -> debugger cannot set breakpoint "inside add()" (it doesn't exist separately)
// -> call stack in debugger doesn't show add() as a separate frame
// -> harder to inspect local variables of inlined function
// -> THIS is why compilers often DON'T inline in debug builds (-O0)

// COST 3: LONGER COMPILE TIME
// -> more code to process at each call site
// -> template + inline heavy code = slow compilation (STL headers!)

// ============================================================
// SECTION 6: main() TRACE
// ============================================================

int main()
{
    int value = add(4, 5);
    // WITH always_inline + optimization:
    // -> add() body inserted here: int value = 4 + 5;
    // -> d,e,f,g local vars: DEAD CODE (unused) -> optimizer removes them
    // -> 4+5 are LITERALS -> constant folding: int value = 9;
    // -> effectively: int value = 9; (computed at compile time!)
    //
    // ASSEMBLY would likely show:
    // mov eax, 9   <- just stores 9 directly, no addition even!

    return value;
    // returns 9

    return 0;
    // DEAD CODE: never reached (return value is above this!)
    // compiler will warn: "unreachable code"
    // another bug in the code alongside unused d,e,f,g
}

// ============================================================
// SECTION 7: WHERE inline IS ACTUALLY REQUIRED (ODR)
// ============================================================

// ONE DEFINITION RULE (ODR):
// A non-inline function can only be DEFINED in ONE translation unit
//
// If you put this in a .hpp included by multiple .cpp files:
// int add(int a, int b) { return a+b; }  // LINKER ERROR! multiple definitions
//
// FIX with inline:
// inline int add(int a, int b) { return a+b; }  // OK in header!
// -> inline tells linker: "multiple definitions are OK, they're all the same"
// -> linker picks one, discards others
//
// THIS is why:
// -> class member functions defined INSIDE class body are implicitly inline
// -> template functions defined in headers work (they're implicitly inline)
// -> constexpr functions are implicitly inline (C++17)

// ============================================================
// SECTION 8: MODERN C++ - WHEN TO USE inline
// ============================================================

// DON'T use inline for performance unless:
// -> profiler PROVES the call is a bottleneck
// -> function is TINY (1-3 lines)
// -> called in a HOT LOOP (thousands/millions of times per second)
//
// DO use inline when:
// -> defining function in a header file (ODR compliance)
// -> small helper in header-only library
// -> getters/setters in class definition (already implicitly inline)
//
// MODERN COMPILERS are better at deciding than you are:
// g++ -O2 or -O3 automatically inlines where beneficial
// Manually adding inline everywhere = premature optimization
//
// PROFILE FIRST, OPTIMIZE SECOND

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does the inline keyword do?
//     -> Two things: (1) HINTS to compiler to substitute function body
//        at call site instead of generating a function call. (2) Allows
//        the function to be defined in multiple translation units (ODR).

// Q2: What is function call overhead?
//     -> The work done to call a function beyond the actual computation:
//        pushing arguments, saving return address, creating stack frame,
//        jumping to function, returning and restoring state.
//        For tiny functions, overhead can exceed actual work.

// Q3: Is inline guaranteed to inline the function?
//     -> NO. It's a HINT. Compiler may ignore it if function is too large,
//        recursive, or if optimization is disabled (-O0/debug mode).
//        __attribute__((always_inline)) forces it (GCC/Clang extension).

// Q4: What are the trade-offs of inlining?
//     -> PROS: no call overhead, better locality, enables further opts.
//        CONS: larger binary (code bloat), harder debugging, longer compile.

// Q5: Why is inline important for header-only libraries?
//     -> Without inline, defining a function in a .hpp included by multiple
//        .cpp files violates ODR (multiple definition linker error).
//        inline tells linker to accept multiple identical definitions.

// Q6: What functions are implicitly inline?
//     -> Member functions defined INSIDE the class body.
//        constexpr functions (C++17).
//        Template function definitions in headers.

// Q7: What does __attribute__((always_inline)) do?
//     -> GCC/Clang extension that FORCES inlining regardless of compiler
//        judgment, size, or optimization level. Not standard C++.
//        MSVC equivalent: __forceinline.

// Q8: When should you NOT use inline for performance?
//     -> For large functions (code bloat outweighs benefit).
//        Before profiling (premature optimization).
//        In debug builds (harder debugging).
//        Modern compilers with -O2/-O3 make better inlining decisions.
// ```

// **The call overhead vs inlined - what CPU actually does:**

// ```
// WITHOUT inline (add(4,5)):          WITH inline (always_inline):
// push 5                               mov eax, 9   ← compiler computed 4+5
// push 4                                              at COMPILE TIME!
// call add          ← jump away        (nothing else)
// [in add: setup frame]
// [int d=9,e=3,f=8,g=19 on stack]
// [compute a+b]
// [teardown frame]
// ret               ← jump back
// mov value, eax

// = ~10 instructions                   = 1 instruction
// ```

// **The two real reasons to use `inline` today:**

// ```
// 1. HEADER FILES (ODR compliance):
//    // mymath.hpp
//    inline int square(int x) { return x*x; }  // ← inline required!
//    // without inline: multiple .cpp files including this = linker error

// 2. FORCED HOT PATH (profiler confirmed bottleneck):
//    __attribute__((always_inline)) void SimTick() { ... }
//    // only after profiling proves the call overhead matters
// ```

// > **The honest answer for interviews**: *"The `inline` keyword originally hinted at inlining for performance, but modern compilers with `-O2`/`-O3` make better inlining decisions than humans. Today, `inline` is most important for its **ODR effect** — allowing function definitions in headers — rather than for performance. I'd only use `__attribute__((always_inline))` after profiling proves a specific hot-path call is a bottleneck."*