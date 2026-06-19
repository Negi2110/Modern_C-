// ============================================================
// VIRTUAL FUNCTIONS, POLYMORPHISM, override - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: THE PROBLEM - WITHOUT virtual
// ============================================================

// WITHOUT virtual keyword:
// class Base {
//     void MemberFunction() { cout << "Base"; }   // NOT virtual
// };
// class Derived : public Base {
//     void MemberFunction() { cout << "Derived"; } // hides Base's version
// };
//
// Base* p = new Derived();
// p->MemberFunction();
// -> compiler looks at the POINTER TYPE (Base*), not actual object
// -> calls Base::MemberFunction()  -> prints "Base"
// -> WRONG! the object is actually a Derived, but Base version runs!
// -> this is called STATIC BINDING (decided at COMPILE TIME)

// ============================================================
// SECTION 2: THE FIX - virtual KEYWORD
// ============================================================

class Base
{
public:
    Base()  { std::cout << "Base class constructor called" << std::endl; }
    ~Base() { std::cout << "Base class destructor called" << std::endl; }

    virtual void MemberFunction()
    //  ^
    //  virtual = "let the ACTUAL object type decide which version to run"
    //  enables DYNAMIC BINDING (decided at RUNTIME)
    {
        std::cout << "Base class member function called" << std::endl;
    }
};

class Derived : public Base
{
public:
    Derived()  { std::cout << "Derived class constructor called" << std::endl; }
    ~Derived() { std::cout << "Derived class destructor called" << std::endl; }

    void MemberFunction() override
    // ^                  ^
    // automatically      "I am intentionally overriding a virtual
    // virtual too         function from base - check this for me!"
    //                     (override is OPTIONAL but HIGHLY recommended)
    {
        std::cout << "Derived class member function called" << std::endl;
    }
};

// ============================================================
// SECTION 3: YOUR QUESTION - WHY USE override IF FUNCTION ALREADY WORKS?
// ============================================================

// YOUR COMMENT: "if you don't write override and it is declared
//                virtual, it will still be called, but make sure
//                to write it just for..."
//
// CORRECT - override is OPTIONAL syntactically, but ESSENTIAL for safety:
//
// WITHOUT override - DANGEROUS:
// class Base {
//     virtual void MemberFunction(int x) { }
// };
// class Derived : public Base {
//     void MemberFunction(float x) { }  // TYPO! float instead of int!
//     // compiler thinks: "this is a NEW function, not an override"
//     // NO ERROR given - silently creates a DIFFERENT function
//     // Derived now has TWO functions: Base's int version (inherited)
//     //                                Derived's NEW float version
//     // polymorphism BROKEN silently! Bug very hard to find!
// };
//
// WITH override - SAFE:
// class Derived : public Base {
//     void MemberFunction(float x) override { }  // COMPILER ERROR!
//     // "MemberFunction marked override but does not override
//     //  any base class member function"
//     // compiler CATCHES your typo immediately!
// };
//
// THIS IS WHY YOU ALWAYS USE override:
// it doesn't change RUNTIME behavior
// it adds COMPILE TIME SAFETY CHECK
// "I promise this overrides something - verify it for me"

// ============================================================
// SECTION 4: VIRTUAL TABLE (vtable) - HOW IT WORKS INTERNALLY
// ============================================================

// when a class has virtual functions, compiler creates a VTABLE
// (hidden array of function pointers, one per virtual function)
//
// Base vtable:                 Derived vtable:
// [0] -> Base::MemberFunction   [0] -> Derived::MemberFunction
//
// every object of a class with virtual functions gets a HIDDEN POINTER
// to its class's vtable (called vptr)
//
// Base* p = new Derived();
// -> p points to a Derived object
// -> Derived object has vptr pointing to DERIVED's vtable
// -> p->MemberFunction() looks up vtable AT RUNTIME
// -> finds Derived::MemberFunction in the vtable
// -> calls IT, not Base's version
//
// THIS is dynamic binding / runtime polymorphism

// ============================================================
// SECTION 5: SCOPE RESOLUTION OPERATOR :: - YOUR QUESTION
// ============================================================

// YOUR COMMENT: ":: what is this operator called"
// ANSWER: SCOPE RESOLUTION OPERATOR
//
// USES OF :: (you've seen several already):
//
// 1. Defining class method outside class:
//    Vector3f::Vector3f() { }   // "this belongs to Vector3f"
//
// 2. Accessing global variable when shadowed:
//    int x = 5;        // global
//    void foo() {
//        int x = 10;    // local shadows global
//        std::cout << ::x;  // :: forces GLOBAL scope -> prints 5
//    }
//
// 3. CALLING A SPECIFIC BASE VERSION (your code):
//    instance2->Base::MemberFunction();
//    //          ^^^^^
//    //          "specifically call Base's version,
//    //           NOT the virtual dispatch / vtable lookup"
//    -> BYPASSES virtual mechanism entirely
//    -> forces compiler to call Base::MemberFunction directly
//    -> useful when derived class wants to call base's version
//       INSIDE its own override:
//
//    void Derived::MemberFunction() override {
//        Base::MemberFunction();  // call base version first
//        std::cout << "extra derived behavior";
//    }

// ============================================================
// SECTION 6: COMPLETE EXECUTION TRACE
// ============================================================

int main()
{
    Base* instance = new Derived();
    // CONSTRUCTION (base first, derived second):
    // "Base class constructor called"
    // "Derived class constructor called"

    instance->MemberFunction();
    // instance is Base*, but POINTS TO a Derived object
    // virtual mechanism: looks at ACTUAL object type (Derived)
    // -> vtable lookup -> calls Derived::MemberFunction()
    // prints: "Derived class member function called"
    // (WITHOUT virtual, this would print "Base class member function called" - WRONG!)

    delete instance;
    // virtual DESTRUCTOR matters here too!
    // Base's destructor is NOT virtual in THIS code... wait let's check
    // actually Base::~Base() is NOT marked virtual in your code!
    // THIS IS A BUG (see Section 7)

    std::cout << std::endl;

    Base* binstance = new Base();
    binstance->MemberFunction();
    // binstance is Base*, points to actual Base object
    // calls Base::MemberFunction() (nothing to override, it IS base)
    // prints: "Base class member function called"
    delete binstance;

    std::cout << std::endl;

    Base* instance2 = new Derived();
    instance2->Base::MemberFunction();
    //          ^^^^^
    //          scope resolution forces BASE version
    //          bypasses virtual dispatch entirely
    // prints: "Base class member function called"
    // even though instance2 points to a Derived object!
    // :: OVERRIDES the polymorphic behavior intentionally

    delete instance2;

    return 0;
}

// ============================================================
// SECTION 7: CRITICAL BUG IN YOUR CODE - NON-VIRTUAL DESTRUCTOR!
// ============================================================

// class Base {
//     ~Base() { }   // NOT virtual!
// };
//
// Base* instance = new Derived();
// delete instance;
// -> delete looks at POINTER TYPE (Base*), not actual object
// -> WITHOUT virtual destructor:
//    only ~Base() called
//    ~Derived() NEVER called!
//    if Derived has heap members -> THEY LEAK!
//
// FIX (CRITICAL for any class meant to be inherited):
// class Base {
//     virtual ~Base() { }   // MUST be virtual!
// };
//
// WITH virtual destructor:
// delete instance;
// -> looks up vtable -> finds Derived's destructor
// -> ~Derived() called FIRST (correct!)
// -> ~Base() called SECOND (correct chain!)
//
// INTERVIEW RULE: "If a class has ANY virtual function,
//                  it should have a virtual destructor."
// Why? Because if it has virtual functions, it's DESIGNED
// to be used polymorphically (Base* pointing to Derived)
// -> deletion through base pointer WILL happen
// -> non-virtual destructor = guaranteed bug

// ============================================================
// SECTION 8: STATIC vs DYNAMIC BINDING - SUMMARY
// ============================================================

// STATIC BINDING (compile time):
// -> non-virtual functions
// -> decided by POINTER/REFERENCE TYPE
// -> fast (no vtable lookup)
// -> WRONG for polymorphism

// DYNAMIC BINDING (runtime):
// -> virtual functions
// -> decided by ACTUAL OBJECT TYPE
// -> small overhead (vtable lookup - one extra memory read)
// -> CORRECT for polymorphism

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does virtual do?
//     -> Enables dynamic (runtime) binding instead of static
//        (compile-time) binding. The ACTUAL object type determines
//        which function runs, not the pointer/reference type.

// Q2: What is the vtable?
//     -> Hidden array of function pointers, one per virtual function.
//        Each class with virtual functions has its own vtable.
//        Each object has a hidden vptr pointing to its class's vtable.
//        Enables runtime polymorphism.

// Q3: Why use override even though it's optional?
//     -> Compiler verifies the function ACTUALLY overrides a base
//        virtual function. Catches typos (wrong signature) at
//        compile time instead of silently creating a new function.

// Q4: Why is a non-virtual destructor dangerous in a base class?
//     -> delete through base pointer only calls base destructor.
//        Derived destructor never runs -> derived's resources leak.
//        Rule: any class with virtual functions needs virtual destructor.

// Q5: What does Base::MemberFunction() do when called via instance->?
//     -> Scope resolution operator bypasses virtual dispatch.
//        Forces the BASE class version to run regardless of
//        actual object type. Used to explicitly call base behavior.

// Q6: What is the performance cost of virtual functions?
//     -> One extra memory lookup (vtable indirection) per call.
//        Cannot be inlined by compiler (decided at runtime).
//        Negligible in 99% of real-world code.

// Q7: What is the difference between static and dynamic binding?
//     -> Static:  decided at compile time, based on pointer/reference type
//        Dynamic: decided at runtime, based on actual object type
//        virtual keyword enables dynamic binding.

// Q8: What does :: (scope resolution) do in obj->Base::Method()?
//     -> Explicitly specifies WHICH class's version to call,
//        bypassing the virtual mechanism.
//        Useful when derived override wants to also call base's version.
// ```

// **The critical bug to flag from your code:**

// ```cpp
// class Base {
//     ~Base() { }              // NOT virtual - BUG!
//     virtual void MemberFunction() { }
// };

// Base* p = new Derived();
// delete p;   // only ~Base() runs, ~Derived() SKIPPED -> leak!

// // FIX:
// class Base {
//     virtual ~Base() { }      // MUST be virtual
// };
// ```

// **The interview rule that always gets asked:**

// > *"If a class has even ONE virtual function, it must have a virtual destructor."*
// > A virtual function means the class is **designed for polymorphism** (Base pointer → Derived object). That guarantees someone will eventually `delete` through a base pointer — and without a virtual destructor, that's a guaranteed resource leak.