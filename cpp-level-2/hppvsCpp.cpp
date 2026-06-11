// inerface(.hpp or .h) and implementation(.cpp,.cxx,.c)
//interface-types,function,classes,declaration
//.cpp-where work is being done actual implementation(sometime we dont have it)
//


#include <iostream>
#include "amanmath.hpp"//"file path"

int main()
{   std::cout<<add(4,15)<<std::endl;
    return 0;
}



// ============================================================
// HEADER FILES, INCLUDE GUARDS, COMPILATION MODEL - INTERVIEW NOTES
// ============================================================

// ============================================================
// SECTION 1: WHY SPLIT INTO .hpp AND .cpp?
// ============================================================

// SINGLE FILE PROBLEMS:
// if everything in one .cpp:
// -> other files cannot use your functions
// -> entire codebase in one file = unmaintainable
// -> change one thing -> recompile EVERYTHING
//
// SOLUTION: split into interface + implementation
//
// .hpp (header)     = WHAT exists   (declarations, types, classes)
// .cpp (source)     = HOW it works  (actual implementations)
//
// ANALOGY:
// .hpp = restaurant MENU      (what's available, what it costs)
// .cpp = kitchen RECIPE       (how to actually make the dish)
// customer (main.cpp) only needs the MENU to order
// doesn't need to know the recipe

// ============================================================
// SECTION 2: YOUR MAIN QUESTION - WHAT IS #ifndef/#define/#endif?
// ============================================================

// These are INCLUDE GUARDS
// Problem they solve:

// imagine main.cpp includes:
// #include "amanmath.hpp"   // int add(int,int) declared -> fine
// #include "amanmath.hpp"   // int add(int,int) declared AGAIN -> ERROR!
//                           // "redeclaration of function add"
//
// In large projects this happens EASILY:
// main.cpp includes A.hpp and B.hpp
// A.hpp includes amanmath.hpp
// B.hpp ALSO includes amanmath.hpp
// -> amanmath.hpp included TWICE in main.cpp -> redeclaration error!
//
// INCLUDE GUARD SOLUTION:
// #ifndef AMANMATH_HPP    // "if NOT defined AMANMATH_HPP"
// #define AMANMATH_HPP    // "now define it (mark as seen)"
//
//     int add(int a, int b);  // declarations here
//
// #endif                  // "end of if block"
//
// FIRST include:
// -> AMANMATH_HPP not defined yet -> enter the block
// -> define AMANMATH_HPP (now it's marked)
// -> process declarations
//
// SECOND include (same file):
// -> AMANMATH_HPP already defined -> SKIP entire block
// -> no redeclaration -> no error!

// ============================================================
// SECTION 3: MODERN ALTERNATIVE - #pragma once
// ============================================================

// Instead of ifndef/define/endif, modern compilers support:
// #pragma once
//
// amanmath.hpp with pragma once:
// #pragma once
// int add(int a, int b);
//
// -> same effect as include guards
// -> simpler, less typing, no name collision risk
// -> NOT officially in C++ standard BUT
//    supported by ALL major compilers (gcc, clang, MSVC)
// -> used in Qt and most modern C++ projects
//
// INTERVIEW: Which to use?
// -> #pragma once: modern projects, simpler
// -> #ifndef guards: guaranteed standard compliant, portable
// -> both are acceptable, know both

// ============================================================
// SECTION 4: THE THREE FILES EXPLAINED
// ============================================================

// FILE 1: amanmath.hpp (INTERFACE)
// ================================
// #ifndef AMANMATH_HPP
// #define AMANMATH_HPP
//
// int add(int a, int b);   // DECLARATION only
//                          // no body, just signature
//                          // tells compiler: "add exists, here's its type"
//
// #endif
//
// FILE 2: amanmath.cpp (IMPLEMENTATION)
// ======================================
// #include "amanmath.hpp"  // include own header (good practice)
//                          // ensures implementation matches declaration
//
// int add(int a, int b) {  // DEFINITION (actual work)
//     return a + b;
// }
//
// FILE 3: main.cpp (USER)
// ========================
// #include "amanmath.hpp"  // only needs the interface!
//                          // doesn't need to know HOW add works
//                          // just needs to know it EXISTS and its type
//
// int main() {
//     std::cout << add(4, 15) << std::endl;  // 19
// }

// ============================================================
// SECTION 5: HOW COMPILATION ACTUALLY WORKS
// ============================================================

// C++ compilation happens in THREE STAGES:
//
// STAGE 1: PREPROCESSOR
// -> handles all # directives
// -> #include "amanmath.hpp" -> literally copy-pastes hpp content
// -> #ifndef/#define/#endif -> processes include guards
// -> #pragma once -> skip if seen before
// -> output: pure C++ code with all includes expanded
//
// STAGE 2: COMPILER
// -> compiles each .cpp file SEPARATELY into .o (object file)
// -> main.cpp     -> main.o
// -> amanmath.cpp -> amanmath.o
// -> each file compiled INDEPENDENTLY
// -> main.o knows add() exists (from declaration) but not WHERE yet
//
// STAGE 3: LINKER
// -> combines all .o files into final executable
// -> main.o says "I need add()"
// -> linker finds add() in amanmath.o
// -> connects the call -> executable works!
//
// VISUAL:
// main.cpp ----[compiler]----> main.o ----+
//                                          +--[linker]--> program.exe
// amanmath.cpp -[compiler]---> amanmath.o -+

// ============================================================
// SECTION 6: WHAT HAPPENS WITHOUT INCLUDE GUARDS
// ============================================================

// File A.hpp:
// #include "amanmath.hpp"   // int add declared (1st time)
//
// File B.hpp:
// #include "amanmath.hpp"   // int add declared (2nd time)
//
// main.cpp:
// #include "A.hpp"          // amanmath.hpp processed -> add declared
// #include "B.hpp"          // amanmath.hpp processed AGAIN
//                           // COMPILER ERROR:
//                           // "error: redeclaration of 'int add(int, int)'"
//
// WITH include guards:
// #include "A.hpp"          // AMANMATH_HPP not defined -> process -> define it
// #include "B.hpp"          // AMANMATH_HPP already defined -> SKIP
//                           // no error!

// ============================================================
// SECTION 7: "" vs <> FOR INCLUDES
// ============================================================

// #include <iostream>     // ANGLE BRACKETS
// -> system/standard library headers
// -> compiler searches in SYSTEM include paths
// -> /usr/include, compiler installation folder etc.
//
// #include "amanmath.hpp" // QUOTES
// -> YOUR OWN headers (or third party)
// -> compiler searches in CURRENT directory FIRST
// -> then falls back to system paths
// -> use for anything you wrote yourself
//
// INTERVIEW: What is the difference between <> and ""?
// -> <> = system headers (standard library, OS headers)
//    "" = local/project headers (your own files)

// ============================================================
// SECTION 8: DECLARATION vs DEFINITION - RECAP IN THIS CONTEXT
// ============================================================

// DECLARATION (in .hpp):
// int add(int a, int b);
// -> tells compiler: "add exists, takes 2 ints, returns int"
// -> no memory allocated, no code generated
// -> can appear MULTIPLE times (that's fine)
//
// DEFINITION (in .cpp):
// int add(int a, int b) { return a+b; }
// -> actual implementation, code generated
// -> memory/code allocated
// -> must appear EXACTLY ONCE (One Definition Rule = ODR)
//
// ODR - ONE DEFINITION RULE:
// -> definition must exist in exactly ONE .cpp file
// -> if add() defined in two .cpp files -> LINKER ERROR
//    "multiple definition of add"
// -> declaration in .hpp is fine to include many times
// -> definition in .cpp must be unique

// ============================================================
// SECTION 9: WHY amanmath.cpp INCLUDES amanmath.hpp
// ============================================================

// amanmath.cpp:
// #include "amanmath.hpp"   // includes own header
//
// WHY?
// -> ensures implementation MATCHES declaration
// -> if hpp says: int add(int a, int b)
//    but cpp defines: float add(float a) -> COMPILER ERROR caught!
// -> without including own header -> mismatch only found at link time
//    (much harder to debug)
// -> best practice: always include own header in .cpp

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the purpose of header files?
//     -> Separate interface (declarations) from implementation.
//        Allow multiple .cpp files to use the same functions/classes
//        without seeing the implementation.

// Q2: What are include guards and why are they needed?
//     -> #ifndef/#define/#endif pattern preventing a header from
//        being included more than once in the same translation unit.
//        Prevents redeclaration errors in complex include trees.

// Q3: What is #pragma once?
//     -> Modern alternative to include guards.
//        Tells compiler to include this file only once.
//        Simpler syntax, widely supported but not officially standard.

// Q4: What are the three stages of C++ compilation?
//     -> 1. Preprocessor: handles #include, #define, #ifdef etc.
//        2. Compiler: compiles each .cpp to .o object file separately
//        3. Linker: combines .o files, resolves function references

// Q5: What is the One Definition Rule (ODR)?
//     -> Every function/variable must be DEFINED exactly once.
//        Declarations can appear many times (in headers).
//        Multiple definitions = linker error.

// Q6: What is the difference between "" and <> in includes?
//     -> <> searches system/compiler paths (standard library)
//        "" searches current directory first, then system paths
//        Use <> for std library, "" for your own headers.

// Q7: Why does amanmath.cpp include its own header?
//     -> Ensures implementation matches declaration.
//        Catches signature mismatches at compile time
//        rather than getting obscure linker errors.

// Q8: What is a translation unit?
//     -> One .cpp file + all headers it includes (after preprocessing).
//        Each translation unit compiled independently into .o file.
//        Linker then combines all translation units.
// ```

// **The three files relationship - drawn simply:**

// ```
// amanmath.hpp          amanmath.cpp          main.cpp
// (interface)           (implementation)      (user)
// ─────────────         ────────────────      ────────────
// #ifndef guard    <──  #include "hpp"   <──  #include "hpp"
// int add(int,int);     int add(int,int)       add(4, 15);
// #endif                { return a+b; }
// ```

// **Compilation pipeline:**
// ```
// main.cpp ──[compiler]──> main.o ──┐
//                                    ├──[linker]──> program
// amanmath.cpp ──[compiler]──> .o ──┘
// ```

// > Include guards answer one question: *"Has this file already been included?"* First time — process it. Second time — skip it. That's all `#ifndef`/`#define`/`#endif` does.