// ============================================================
// VIRTUAL DESTRUCTOR - PROVING THE LEAK FIX - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: YOUR COMMENT - WHAT VIRTUAL DESTRUCTOR DOES
// ============================================================

// YOUR COMMENT: "if not used it will do memory leak for derived class"
// "it tells compiler that since base has virtual destructor,
//  when we delete base pointer, it calls derived destructor
//  FIRST, then base destructor"
//
// 100% CORRECT - let's prove it with this exact code

class Base
{
public:
    Base()
    {
        std::cout << "Base class constructor called" << std::endl;
        baseData = new int[10];   // Base ACQUIRES a resource (RAII)
    }

    virtual ~Base()
    //  ^
    //  THE FIX - without this keyword, derived destructor
    //  would NEVER be called when deleting through Base*
    {
        std::cout << "Base class destructor called" << std::endl;
        delete[] baseData;        // Base RELEASES its resource (RAII)
    }

    virtual void MemberFunction()
    {
        std::cout << "Base class member function called" << std::endl;
    }

    int* baseData;   // RAII managed - acquired in ctor, released in dtor
};

class Derived : public Base
{
public:
    Derived()
    {
        std::cout << "Derived class constructor called" << std::endl;
        derivedData = new int[15];  // Derived ACQUIRES its own resource
    }

    ~Derived()
    // NOTE: doesn't need 'virtual' keyword here explicitly
    // ONCE a function is virtual in base, it STAYS virtual
    // in every derived class automatically (even without writing 'virtual')
    // but writing it is still good practice for clarity
    {
        std::cout << "Derived class destructor called" << std::endl;
        delete[] derivedData;       // Derived RELEASES its resource
    }

    void MemberFunction() override
    {
        std::cout << "Derived class member function called" << std::endl;
    }

    int* derivedData;
};

// ============================================================
// SECTION 2: COMPLETE EXECUTION TRACE - WITH virtual ~Base()
// ============================================================

int main()
{
    Base* instance = new Derived();
    // STEP 1: Base() constructor runs
    //   -> baseData = new int[10]   (10 ints allocated on heap)
    //   prints: "Base class constructor called"
    // STEP 2: Derived() constructor runs
    //   -> derivedData = new int[15]  (15 ints allocated on heap)
    //   prints: "Derived class constructor called"
    //
    // HEAP STATE NOW:
    // baseData    -> [10 ints]  (owned by Base part of object)
    // derivedData -> [15 ints]  (owned by Derived part of object)

    instance->MemberFunction();
    // virtual dispatch: instance points to Derived object
    // vtable lookup -> calls Derived::MemberFunction()
    // prints: "Derived class member function called"

    delete instance;
    // instance is Base* but points to a Derived object
    //
    // BECAUSE ~Base() is virtual:
    // -> compiler does VTABLE LOOKUP for the destructor
    // -> finds the MOST DERIVED destructor (Derived's)
    // -> calls ~Derived() FIRST
    //      prints: "Derived class destructor called"
    //      delete[] derivedData -> 15 ints FREED  ✓ no leak!
    // -> ~Derived() automatically calls ~Base() after (compiler inserted)
    //      prints: "Base class destructor called"
    //      delete[] baseData -> 10 ints FREED  ✓ no leak!
    //
    // BOTH allocations properly freed = NO MEMORY LEAK

    return 0;
}

// FULL OUTPUT:
// Base class constructor called
// Derived class constructor called
// Derived class member function called
// Derived class destructor called
// Base class destructor called

// ============================================================
// SECTION 3: WHAT WOULD HAPPEN WITHOUT virtual (PROOF OF THE BUG)
// ============================================================

// IF Base::~Base() was NOT virtual:
//
// class Base {
//     ~Base() { delete[] baseData; }   // NOT virtual
// };
//
// Base* instance = new Derived();
// delete instance;
//
// WITHOUT virtual:
// -> compiler does STATIC binding for destructor (compile-time decision)
// -> looks at POINTER TYPE (Base*), ignores actual object type
// -> calls ONLY ~Base()
// -> ~Derived() is NEVER CALLED
// -> delete[] derivedData NEVER RUNS
// -> 15 ints (derivedData) LEAKED FOREVER
//
// OUTPUT WOULD BE (missing the derived destructor line!):
// Base class constructor called
// Derived class constructor called
// Derived class member function called
// Base class destructor called    <- ONLY this, ~Derived() skipped!
//
// HEAP STATE AFTER (WITHOUT virtual):
// baseData    -> freed correctly
// derivedData -> STILL ALLOCATED, pointer is now gone (instance destroyed)
//                -> 60 bytes LEAKED, unrecoverable for rest of program

// ============================================================
// SECTION 4: WHY DOES THIS HAPPEN? - VTABLE MECHANISM
// ============================================================

// non-virtual destructor:
// delete instance;
// -> compiler generates: Base::~Base(instance)  (hardcoded at compile time)
// -> based on STATIC TYPE of pointer (Base*)
// -> object's ACTUAL type (Derived) is IGNORED
//
// virtual destructor:
// delete instance;
// -> compiler generates: vtable lookup using instance's vptr
// -> vptr points to DERIVED's vtable (set during Derived construction)
// -> finds Derived::~Derived in the vtable
// -> calls it -> Derived's destructor automatically chains to Base's after
//
// THIS IS EXACTLY THE SAME MECHANISM as MemberFunction()!
// virtual destructor = just a destructor that participates in
//                      the SAME vtable dispatch as any other virtual function

// ============================================================
// SECTION 5: RAII CONNECTION - YOUR COMMENT
// ============================================================

// YOUR COMMENT: "RAII also the basedata in the base class"
//
// RAII applies at EVERY LEVEL of the inheritance chain:
//
// Base:
//   constructor ACQUIRES baseData
//   destructor  RELEASES baseData
//
// Derived:
//   constructor ACQUIRES derivedData
//   destructor  RELEASES derivedData
//
// FULL OBJECT LIFECYCLE (RAII at both levels):
// new Derived()  -> Base acquires, then Derived acquires
// delete         -> Derived releases, then Base releases
//
// virtual destructor GUARANTEES this full chain runs correctly
// even when accessed through a Base* pointer
// THIS is why virtual destructor is essential for RAII
// to work correctly in inheritance hierarchies

// ============================================================
// SECTION 6: THE RULE - WHEN TO MARK DESTRUCTOR VIRTUAL
// ============================================================

// RULE: "If a class has ANY virtual function OR is intended
//        to be used polymorphically (deleted through base
//        pointer), its destructor MUST be virtual."
//
// Your Base class qualifies on BOTH counts:
// 1. Has virtual MemberFunction() -> designed for polymorphism
// 2. Used as: Base* instance = new Derived();  delete instance;
//             -> literally deleted through base pointer
//
// CONVERSE RULE:
// "If a class is NOT meant to be inherited from
//  (no virtual functions, not a base class),
//  destructor does NOT need to be virtual."
// -> avoids unnecessary vtable overhead for simple value types
//    (like your earlier Vector3f, Array, IntArray classes -
//     none of those needed virtual destructors)

// ============================================================
// SECTION 7: INTERVIEW QUESTIONS
// ============================================================

// Q1: What bug occurs without a virtual destructor in this exact code?
//     -> delete instance (Base* pointing to Derived) would only call
//        ~Base(), skipping ~Derived() entirely. derivedData (15 ints)
//        would leak because delete[] derivedData never executes.

// Q2: Why does ~Derived() not need the 'virtual' keyword explicitly?
//     -> Once a function is declared virtual in the base class,
//        it remains virtual in ALL derived classes automatically,
//        even without re-stating 'virtual'. Writing it anyway is
//        good practice for readability.

// Q3: What is the destructor call order when deleting Derived via Base*?
//     -> ~Derived() called FIRST (most-derived first)
//        ~Base() called SECOND (automatically chained by compiler)
//        This is the standard reverse-of-construction order.

// Q4: How does the compiler know to call ~Derived() instead of ~Base()?
//     -> Virtual destructor participates in the vtable, just like
//        any other virtual function. instance's vptr points to
//        Derived's vtable, so the lookup finds Derived's destructor.

// Q5: Does marking ~Base() virtual add any runtime cost?
//     -> Yes, negligible: one vtable pointer per object (already
//        present since MemberFunction is virtual) and one indirect
//        call instead of direct. Vastly outweighed by the leak it prevents.

// Q6: What is the general rule for when to make a destructor virtual?
//     -> If the class has any virtual functions, or is designed to
//        be deleted through a base class pointer, the destructor
//        must be virtual. Otherwise it's unnecessary overhead.

// Q7: How does RAII relate to virtual destructors here?
//     -> RAII guarantees cleanup happens automatically at scope/delete.
//        But in an inheritance chain, RAII only works CORRECTLY at
//        every level if the destructor chain actually runs fully -
//        which requires virtual destructor when deleting via base pointer.

// Q8: If Derived also had a raw pointer member but Base's destructor
//     was non-virtual, what specifically leaks?
//     -> ONLY Derived's resources leak (derivedData, 15 ints = 60 bytes).
//        Base's resources (baseData) are still freed correctly because
//        ~Base() IS called - just not ~Derived() before it.
// ```

// **Side-by-side proof - the exact difference in output:**

// ```
// WITH virtual ~Base():              WITHOUT virtual ~Base():
// ─────────────────────              ─────────────────────────
// Base constructor                    Base constructor
// Derived constructor                 Derived constructor
// Derived member function             Derived member function
// Derived destructor   ✓              (MISSING - never called!)
// Base destructor                     Base destructor

// Result: NO LEAK                     Result: 15 ints LEAKED
// ```

// > Your code is now **correct** because `virtual ~Base()` is present. This single keyword is the difference between `delete instance;` cleaning up **everything** versus silently leaking the entire `Derived` portion of the object every single time.