// ============================================================
// FUNCTIONS - DECLARATION, DEFINITION, OVERLOADING - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: FUNCTION DECLARATION (Forward Declaration)
// ============================================================

// Declaration = telling the compiler "this function EXISTS"
// Definition  = actual implementation (the body)
//
// WHY forward declaration?
// -> C++ compiler reads TOP TO BOTTOM
// -> If main() calls add() before add() is defined,
//    compiler says: "I don't know what add() is!" -> ERROR
// -> Forward declaration tells compiler the signature upfront
// -> Actual definition can come AFTER main()
//
// Declaration syntax: return_type name(param_types);
// Parameter NAMES are optional in declaration, types are enough:
// int add(int, int);       // valid
// int add(int a, int b);   // also valid, names ignored by compiler

int add(int a, int b);           // forward declaration - int version
auto add(float a, float b) -> float;  // forward declaration - float version
                                       // trailing return type syntax (C++11)

// ============================================================
// SECTION 2: TRAILING RETURN TYPE SYNTAX
// ============================================================

// Classic syntax:
// float add(float a, float b);

// Trailing return type (C++11):
// auto add(float a, float b) -> float;
//
// Both are IDENTICAL to the compiler
//
// WHY trailing return type exists?
// -> Useful when return type DEPENDS on parameter types
// -> Template functions where return type is complex to write upfront
// -> Lambda functions use this style
//
// Example where trailing is necessary:
// template<typename A, typename B>
// auto multiply(A a, B b) -> decltype(a * b);
// return type depends on A and B -> can't write it at front

// ============================================================
// SECTION 3: void FUNCTION
// ============================================================

void print(void) {
    std::cout << "Hello functions!" << std::endl;
}
// void return type = returns nothing
// void parameter   = takes nothing (same as empty parentheses)
// print()  == print(void)  -> identical in C++
//
// INTERVIEW: What is the difference between void and no return?
//   -> void explicitly says "returns nothing"
//      Missing return in non-void function = undefined behavior
//
// INTERVIEW: Can you have a return statement in void function?
//   -> YES but only bare return; (no value)
//      void print() { if(done) return; cout << "hi"; }

// ============================================================
// SECTION 4: FUNCTION OVERLOADING
// ============================================================

// Two functions with SAME NAME but DIFFERENT parameters
// Compiler picks the right one based on argument types
//
// int add(int a, int b)     -> called when you pass ints
// float add(float a, float b) -> called when you pass floats
//
// RULES for overloading:
// -> Parameters must differ in TYPE or COUNT (or both)
// -> Return type ALONE cannot differentiate overloads
//    int add(int,int) and float add(int,int) -> COMPILER ERROR
//
// HOW compiler picks which overload to call:
// -> add(4, 9)          -> both are int literals   -> calls int version
// -> add(3.1234f, 9.3f) -> both are float literals -> calls float version
// -> add(3.14, 9.3)     -> both are double!        -> compiler may warn
//                           no double overload exists -> tries to convert

int main()
{
    int sum = add(4, 9);
    std::cout << sum << std::endl;          // prints 13

    std::cout << add(3.1234f, 9.345f) << std::endl;  // prints 12.4684
    // 'f' suffix is CRITICAL here
    // without 'f': add(3.1234, 9.345) -> double literals
    //              no double overload -> compiler picks int or float
    //              via implicit conversion -> wrong result!

    print();   // prints "Hello functions!"

    return 0;
}

// ============================================================
// SECTION 5: FUNCTION DEFINITIONS (after main - allowed because of forward decl)
// ============================================================

int add(int a, int b)
{
    return a + b;
    // simple, returns int sum
}

float add(float a, float b)
{
    return a + b;
    // same name, different types -> overloaded version
}

// ============================================================
// SECTION 6: HOW COMPILER RESOLVES OVERLOADS - NAME MANGLING
// ============================================================

// Internally compiler renames overloaded functions:
// int add(int,int)     -> _Z3addii   (ii = int, int)
// float add(float,float) -> _Z3addff (ff = float, float)
//
// This is called NAME MANGLING
// Linker uses mangled names to find correct function
//
// INTERVIEW: What is name mangling?
//   -> Compiler encodes function name + parameter types into
//      a unique symbol name for the linker
//      This is how overloading works under the hood
//      C does NOT have name mangling -> no function overloading in C

// ============================================================
// SECTION 7: FUNCTION ANATOMY
// ============================================================

//  return_type  name  ( parameters )   {  body  }
//      |         |          |                |
//     int       add    int a, int b     return a+b;
//
// PARTS:
// -> return type : what the function gives back (int, float, void)
// -> name        : identifier to call the function
// -> parameters  : inputs (type + name), local to the function
// -> body        : code that runs when function is called
// -> return       : sends value back to caller, exits function

// ============================================================
// SECTION 8: STACK FRAMES - WHAT HAPPENS WHEN FUNCTION IS CALLED
// ============================================================

// When add(4, 9) is called:
//
// STACK:
// |              |
// | add() frame  |  <- pushed on stack
// |   a = 4      |     local copies of arguments
// |   b = 9      |
// |   return addr|     where to go back after function ends
// |______________|
// | main() frame |  <- was here before
// |   sum = ?    |
// |______________|
//
// add() runs, computes 13
// add() frame POPPED off stack
// 13 returned to main(), stored in sum
//
// INTERVIEW: What is a stack frame?
//   -> Block of memory on the call stack for one function call
//      Contains: local variables, parameters, return address
//      Created on function call, destroyed on function return

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between declaration and definition?
//     -> Declaration: signature only, tells compiler function exists
//                     int add(int, int);
//        Definition:  full implementation with body
//                     int add(int a, int b) { return a+b; }

// Q2: What is function overloading?
//     -> Multiple functions with same name but different parameter
//        types or count. Compiler picks correct version at compile time.

// Q3: Can you overload based on return type only?
//     -> NO. Compiler error.
//        int add(int,int) and float add(int,int) -> ambiguous
//        Parameters must differ.

// Q4: What is name mangling?
//     -> Compiler encodes parameter types into function symbol name
//        Enables overloading. C++ has it, C does not.
//        extern "C" disables mangling for C compatibility.

// Q5: What is a trailing return type?
//     -> auto add(float a, float b) -> float;
//        C++11 feature. Useful for templates where return type
//        depends on parameter types (decltype).

// Q6: What happens without forward declaration if function is defined after main?
//     -> Compiler reads top to bottom.
//        Sees call to add() in main() but has not seen add() yet.
//        COMPILER ERROR: use of undeclared function.

// Q7: What is the difference between parameter and argument?
//     -> Parameter: variable in function definition  (int a, int b)
//        Argument:  actual value passed when calling (add(4, 9))
//        4 and 9 are arguments. a and b are parameters.

// Q8: What does extern "C" do?
//     -> Tells C++ compiler to use C-style linking (no name mangling)
//        Used when C++ code needs to call C functions or vice versa
//        extern "C" { int add(int, int); }
// ```

// **The three things interviewers always ask from this topic:**

// ```
// 1. Declaration vs Definition
//    -> Declaration = promise  |  Definition = delivery

// 2. Overloading rule
//    -> Same name, different PARAMETERS (not return type)

// 3. Name mangling
//    -> How overloading works under the hood
//    -> Why C has no overloading (no mangling)
//    -> extern "C" disables it for C interop
// ```

// > **Stack frame** question also comes up heavily in Qt/embedded interviews because understanding call stack = understanding crashes, stack overflows, and debugger output.




// Both are **identical** to the compiler. It's just two different ways to write the same thing:

// ```cpp
// ============================================================
// TRAILING RETURN TYPE vs CLASSIC SYNTAX - WHEN IT ACTUALLY MATTERS
// ============================================================

// ============================================================
// CASE 1: IDENTICAL - no difference at all
// ============================================================

// float add(float a, float b);          // classic
// auto  add(float a, float b) -> float; // trailing
//
// Compiler sees EXACT same thing
// Same function signature
// Same name mangling (_Z3addff)
// Just different syntax, zero practical difference here

// ============================================================
// CASE 2: WHERE TRAILING RETURN TYPE IS ACTUALLY NEEDED
// ============================================================

// PROBLEM with classic syntax in templates:
//
// imagine you want a function that multiplies
// an int and a float together
// what should the return type be?
//
// int   * float = ??? (could be float, double, depends on compiler)
//
// WRONG attempt with classic syntax:
// int multiply(int a, float b) { return a * b; } // loses decimals
// float multiply(int a, float b) { return a*b; } // hardcoded, not flexible
//
// CORRECT with trailing return type + decltype:
// template<typename A, typename B>
// auto multiply(A a, B b) -> decltype(a * b)
// {
//     return a * b;
// }
// decltype(a*b) = "whatever type a*b produces"
// compiler figures it out automatically
// multiply(3, 4.5f)   -> return type is float
// multiply(3, 4.5)    -> return type is double
// multiply(3, 4)      -> return type is int
//
// With CLASSIC syntax this is IMPOSSIBLE to write:
// ??? multiply(A a, B b)   // what do you put here???
// decltype(a*b) multiply(A a, B b) // ERROR - a and b not declared yet!
//                                  // they are declared AFTER the return type
//                                  // compiler doesn't know a and b yet

// ============================================================
// THAT IS THE KEY INSIGHT
// ============================================================

// classic syntax:
// [return type] [name] ([parameters])
//      ^                     ^
//      |                     |
// written FIRST         declared HERE
//
// Problem: return type is written BEFORE parameters are declared
//          so return type CANNOT depend on parameter names
//
// trailing syntax:
// auto [name] ([parameters]) -> [return type]
//                   ^                ^
//                   |                |
//             declared HERE    written AFTER
//
// Now return type is written AFTER parameters
// so return type CAN use parameter names (a, b) via decltype

// ============================================================
// LAMBDA FUNCTIONS - trailing return type used here too
// ============================================================

// auto square = [](int x) -> int {
//     return x * x;
// };
//
// Lambda functions ALWAYS use trailing return type style
// Because they have no function name to put return type before
// You will see this heavily in modern C++ and Qt code

// ============================================================
// SIMPLE RULE TO REMEMBER
// ============================================================

// Normal function, return type is simple and known upfront?
//   -> use classic:   float add(float a, float b)
//      cleaner, easier to read

// Template function, return type depends on parameters?
// Lambda function?
//   -> use trailing:  auto add(A a, B b) -> decltype(a+b)
//      only way to express it correctly
// ```

// **One line answer for interview:**

// > Both are identical for simple functions. Trailing return type exists for **templates and lambdas** where the return type **depends on the parameters** — which is impossible to express with classic syntax because parameters aren't declared yet when the return type is written.


// Yes, 100% correct. Writing this:

// ```cpp
// auto add(float a, float b) -> float;
// ```

// is **completely pointless** compared to:

// ```cpp
// float add(float a, float b);
// ```

// Because you already **hardcoded** `float` as return type anyway. You got zero benefit from trailing syntax. You just made it longer and harder to read.

// Trailing return type only makes sense when the return type **cannot be hardcoded** — like this:

// ```cpp
// // THIS makes sense - return type is unknown until compiler sees a*b
// template<typename A, typename B>
// auto multiply(A a, B b) -> decltype(a * b);

// // THIS is pointless - you already know it's float, just write float
// auto add(float a, float b) -> float;  // overkill, no benefit
// ```

// **Your teacher likely wrote it just to show the syntax exists** — not because it was the right choice there.

// > Rule: if you can write the return type **directly** → use classic syntax.
// > Trailing syntax only earns its place when return type **depends on parameters** at compile time.