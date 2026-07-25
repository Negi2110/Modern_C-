// ============================================================
// DEFAULT FUNCTION ARGUMENTS - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT ARE DEFAULT ARGUMENTS?
// ============================================================

// Default argument = a value used when caller DOESN'T provide one
// Defined in the function DECLARATION (or definition if no separate decl)
//
// void point(int x, int y=0, int z=0)
//                      ^^^      ^^^
//                      default  default
//                      y=0 if   z=0 if
//                      not given not given

void point(int x, int y = 0, int z = 0)
{
    std::cout << x << std::endl;
    std::cout << y << std::endl;
    std::cout << z << std::endl;
}

// ============================================================
// SECTION 2: CALLING WITH DIFFERENT ARGUMENT COUNTS
// ============================================================

int main()
{
    point(2, 5, 6);
    // x=2, y=5, z=6  (all provided, no defaults used)
    // prints: 2, 5, 6

    point(2, 5);
    // x=2, y=5, z=0  (z uses default)
    // prints: 2, 5, 0

    point(2);
    // x=2, y=0, z=0  (y and z both use default)
    // prints: 2, 0, 0

    // point();        // ERROR: x has no default, MUST be provided
    // point(2,,6);    // ERROR: cannot skip y to provide z
    //                 // must always fill from LEFT to RIGHT

    return 0;
}

// ============================================================
// SECTION 3: THE RULES
// ============================================================

// RULE 1: defaults must be on the RIGHT side
// void foo(int x=0, int y, int z)    // ERROR: y,z have no default
//                                     // but x (before them) does
// void foo(int x, int y=0, int z=0)  // OK: rightmost have defaults
// void foo(int x=0, int y=0, int z=0)// OK: all have defaults
//
// RULE 2: cannot skip arguments
// point(2, , 6)  // ERROR: must provide left to right
//                 // if you skip y, you can't provide z
//
// RULE 3: default defined in DECLARATION, not repeated in definition
// // header (.hpp):
// void point(int x, int y=0, int z=0);   // defaults here
//
// // source (.cpp):
// void point(int x, int y, int z) { }    // NO defaults here!
//                                          // would be a REDEFINITION error
//
// RULE 4: can add defaults later (in separate declaration)
// void point(int x, int y, int z);        // first declaration: no defaults
// void point(int x, int y, int z=0);     // second: add z default
// void point(int x, int y=0, int z=0);   // third: add y default
// -> defaults accumulate across re-declarations in same scope
// -> must still go right to left

// ============================================================
// SECTION 4: DEFAULT ARGUMENTS vs OVERLOADING - COMPARISON
// ============================================================

// SAME behavior using OVERLOADING (more verbose):
// void point(int x, int y, int z) { std::cout << x << y << z; }
// void point(int x, int y)        { point(x, y, 0); }
// void point(int x)               { point(x, 0, 0); }
//
// SAME behavior using DEFAULT ARGS (concise):
// void point(int x, int y=0, int z=0) { std::cout << x << y << z; }
//
// WHEN to prefer OVERLOADING over defaults:
// -> different LOGIC for different argument counts (not just filling in 0)
// -> virtual functions (defaults don't work well with virtual dispatch!)
// -> when types differ between versions (not just values)
//
// WHEN to prefer DEFAULT ARGS:
// -> same logic, just fewer parameters needed
// -> simple "fill in a standard value" case
// -> your point() example: 0 is natural default for coordinates

// ============================================================
// SECTION 5: DEFAULT ARGS + VIRTUAL FUNCTIONS - THE TRAP
// ============================================================

// CLASSIC interview trap:
// struct Base {
//     virtual void foo(int x = 10) { std::cout << "Base " << x; }
// };
// struct Derived : Base {
//     void foo(int x = 20) override { std::cout << "Derived " << x; }
// };
//
// Base* p = new Derived;
// p->foo();   // prints: "Derived 10"  <- SURPRISING!
//
// WHY?
// -> which function RUNS: determined at RUNTIME (virtual dispatch) = Derived
// -> which DEFAULT VALUE used: determined at COMPILE TIME (static type) = Base
// -> p is Base* -> compiler uses Base's default (10) at compile time
// -> but vtable routes to Derived::foo at runtime
// -> result: Derived::foo runs with x=10 (Base's default, not Derived's 20!)
//
// RULE: NEVER redefine default arguments in derived class overrides
//        it creates exactly this confusion

// ============================================================
// SECTION 6: INTERVIEW QUESTIONS
// ============================================================

// Q1: What are default arguments?
//     -> Values automatically used when caller doesn't provide them.
//        Defined in function declaration. Allow calling with fewer args.

// Q2: What is the ordering rule for default arguments?
//     -> All defaults must be on the RIGHTMOST parameters.
//        Once a parameter has a default, all to its right must too.
//        Cannot skip parameters when calling (must fill left to right).

// Q3: Where should defaults be defined - declaration or definition?
//     -> In the DECLARATION (.hpp / function prototype).
//        NOT repeated in the definition (.cpp).
//        Repeating in definition = redefinition error.

// Q4: What is the difference between default args and overloading?
//     -> Default args: one function, some params have fallback values.
//        Overloading: multiple functions, different signatures.
//        Default args preferred for "same logic, fewer params" cases.
//        Overloading preferred when different behavior is needed.

// Q5: What is the virtual function + default argument trap?
//     -> Default value used = COMPILE TIME type (pointer/ref type).
//        Function called = RUNTIME type (actual object type).
//        So Derived override runs but uses Base's default value!
//        Never redefine default args in overriding functions.

// Q6: Can default arguments be expressions or function calls?
//     -> YES: void foo(int x = getValue());  // function call
//             void foo(int x = SIZE * 2);    // expression
//             void foo(int x = INT_MAX);     // constant
//        The expression is evaluated at EACH call where default is used.
//        Not just once at declaration time.

// Q7: Can you add defaults to previously declared functions?
//     -> YES, in subsequent declarations in the same scope.
//        Must still go right to left.
//        Common pattern: add defaults incrementally in multiple declarations.

// Q8: Can a constructor have default arguments?
//     -> YES. Constructor(int x = 0, int y = 0) allows:
//        MyClass obj;       // x=0, y=0
//        MyClass obj(5);    // x=5, y=0
//        MyClass obj(5, 3); // x=5, y=3
//        Effectively gives you multiple constructors in one.
// ```

// **The rules in one picture:**

// ```cpp
// void point(int x, int y=0, int z=0);  // ✅ correct: defaults on right

// // Calling - must fill LEFT to RIGHT, cannot skip:
// point(2, 5, 6);  // x=2, y=5, z=6   all provided
// point(2, 5);     // x=2, y=5, z=0   z defaults
// point(2);        // x=2, y=0, z=0   y,z default
// point( , 5, 6);  // ❌ ERROR: can't skip x (no default)
// point(2, , 6);   // ❌ ERROR: can't skip y to give z
// ```

// **The virtual trap - the interview killer question:**

// ```cpp
// Base*  p = new Derived;
// p->foo();
// // WHICH function runs?  → Derived::foo  (runtime/vtable)
// // WHICH default used?   → Base's default (compile-time/static type)
// // Result: Derived runs with Base's default value → SURPRISING!
// ```

// > **The one rule to remember**: default arguments are a compile-time feature resolved by the **static type** of the pointer/reference, while virtual dispatch is a runtime feature resolved by the **actual object type**. These two systems don't coordinate — that mismatch is exactly what makes the virtual+default trap so surprising in interviews.