
// ============================================================
// SCOPES & VARIABLE LIFETIME - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS A SCOPE?
// ============================================================

// Scope = the region of code where a variable EXISTS and is ACCESSIBLE
// In C++, scope is defined by curly braces { }
//
// Every { opens a new scope
// Every } closes that scope -> variables inside are DESTROYED

// ============================================================
// SECTION 2: TYPES OF SCOPE IN C++
// ============================================================

// 1. GLOBAL SCOPE   -> outside all functions, lives entire program
// 2. LOCAL SCOPE    -> inside a function
// 3. BLOCK SCOPE    -> inside any { } block
// 4. CLASS SCOPE    -> inside a class
// 5. NAMESPACE SCOPE -> inside a namespace

// ============================================================
// SECTION 3: YOUR CODE EXPLAINED LINE BY LINE
// ============================================================

// int x = 42;  <- GLOBAL SCOPE
//                 lives from program start to program end
//                 accessible everywhere in this file
//                 stored in DATA SEGMENT of memory

int main() {

    // ---- BLOCK 1 ----
    {
        int x = 7;      // x born here, stored on STACK
                        // scope = only inside these { }
        // x is accessible here
    }                   // x DIES here, stack memory released
    // x is NOT accessible here -> compiler error if you try

    // ---- BLOCK 2 ----
    {
        int x = 47;     // NEW x born here, completely different variable
                        // same name, different scope, different memory
        // x is accessible here
    }                   // x DIES here again

    // INTERVIEW POINT:
    // Both blocks have 'int x' but they are TWO DIFFERENT variables
    // They just happen to have the same name
    // They never exist at the same time -> no conflict

    return 0;
}

// ============================================================
// SECTION 4: MEMORY - WHERE do these variables live?
// ============================================================

// GLOBAL int x = 42;
//   -> DATA SEGMENT (fixed memory, allocated at program start)
//   -> Lives until program exits

// Local/Block int x = 7;
//   -> STACK (automatic memory)
//   -> Born when { is entered
//   -> Destroyed when } is hit
//   -> Stack pointer moves up and down automatically

// ============================================================
// SECTION 5: SHADOWING - When inner scope hides outer scope
// ============================================================

// int x = 42;  // global

// int main() {
//     int x = 7;           // local x SHADOWS global x
//     std::cout << x;      // prints 7, NOT 42
//     std::cout << ::x;    // prints 42 -> :: forces GLOBAL scope
// }

// INTERVIEW: What is variable shadowing?
//   -> When an inner scope declares a variable with the SAME NAME
//      as outer scope, the inner one hides the outer one.
//      Outer variable still exists, just not directly accessible.
//      Use :: operator to access global when shadowed.

// ============================================================
// SECTION 6: WHAT HAPPENS IN MEMORY - STACK VISUAL
// ============================================================

//  main() starts
//  |
//  |-- Block 1 enters { }
//  |       STACK: [x=7]       <- x pushed onto stack
//  |-- Block 1 exits { }
//  |       STACK: []          <- x popped, memory freed
//  |
//  |-- Block 2 enters { }
//  |       STACK: [x=47]      <- NEW x pushed onto stack
//  |-- Block 2 exits { }
//  |       STACK: []          <- x popped again
//  |
//  main() ends

// ============================================================
// SECTION 7: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is scope in C++?
//     -> Region of code where a variable is visible and accessible.
//        Defined by { } braces.

// Q2: What happens to a local variable when its scope ends?
//     -> It is DESTROYED. Destructor is called (for objects),
//        stack memory is released automatically.

// Q3: Can two variables have the same name in C++?
//     -> YES, if they are in DIFFERENT scopes.
//        They are completely separate variables.
//        Same name, different memory locations.

// Q4: What is the difference between scope and lifetime?
//     -> Scope    = where in CODE the variable is accessible (compile time concept)
//        Lifetime = how long the variable EXISTS in MEMORY (runtime concept)
//        Usually they match for local variables, but not always.
//        Example: static local variable -> scope is local
//                 but lifetime is entire program duration

// Q5: What is a static local variable?
//     -> void counter() {
//            static int count = 0;  // initialized only ONCE
//            count++;               // retains value between calls
//            std::cout << count;
//        }
//        counter(); // prints 1
//        counter(); // prints 2  <- count survived scope exit!
//        counter(); // prints 3
//
//        Scope  = inside counter() only
//        Lifetime = entire program (stored in DATA SEGMENT, not stack)

// Q6: What is the :: operator?
//     -> Scope resolution operator.
//        ::x  -> access global x
//        ClassName::method -> access class member
//        std::cout -> access cout inside std namespace

// Q7: What is the difference between global and local variables?
//     -> Global: DATA segment, lives entire program, accessible everywhere
//        Local:  STACK, lives only inside its { }, auto destroyed on exit
// ```

// **The one concept that surprises most beginners:**

// > Scope and Lifetime are **different things**
// > `static local variable` — scope is **local** (only accessible inside function) but lifetime is **global** (survives the entire program)

// That `static` local variable question **always comes up** in C++/Qt interviews because it's used heavily in singleton patterns and lazy initialization.