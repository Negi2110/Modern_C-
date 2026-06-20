// ============================================================
// VTABLE MECHANISM - DEEP DIVE - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: YOUR QUESTION - WHY DOES DERIVED HAVE A VTABLE
//             WHEN IT DOESN'T DECLARE ANY virtual FUNCTION ITSELF?
// ============================================================

// class Derived : public Base {
//     void MemberFunction() override { }   // no 'virtual' written here!
//     ~Derived() { }                       // no 'virtual' written here!
// };
//
// ANSWER: virtual-ness is INHERITED, not re-declared
//
// RULE: "Once virtual, always virtual - for the ENTIRE hierarchy below"
//
// Base declares MemberFunction() as virtual
// -> EVERY class that derives from Base (directly or indirectly)
//    automatically has MemberFunction() as virtual too
// -> doesn't matter if Derived writes 'virtual' or not
// -> compiler treats Derived::MemberFunction() as virtual AUTOMATICALLY
//    because it OVERRIDES a virtual function from Base
//
// SAME applies to destructors:
// Base declares ~Base() as virtual
// -> Derived's ~Derived() is AUTOMATICALLY virtual too
// -> this is EXACTLY why your earlier code worked correctly
//    even though ~Derived() didn't explicitly say 'virtual'
//
// SO: Derived doesn't get its OWN separate "virtual decision"
//     it INHERITS the obligation to participate in Base's vtable system
//     Derived MUST have a vtable because it's PART OF a polymorphic
//     hierarchy that Base started

// ============================================================
// SECTION 2: EVERY CLASS IN THE HIERARCHY GETS ITS OWN VTABLE
// ============================================================

// IMPORTANT CLARIFICATION:
// Base and Derived do NOT share ONE vtable
// EACH class gets its OWN SEPARATE vtable (created at compile time)
//
// Base's vtable (created once, at compile time, lives in code/data segment):
// ┌─────────────────────────────────────┐
// │ Base::vtable                         │
// │ [0] -> &Base::~Base                  │
// │ [1] -> &Base::MemberFunction         │
// └─────────────────────────────────────┘
//
// Derived's vtable (ALSO created once, at compile time):
// ┌─────────────────────────────────────┐
// │ Derived::vtable                      │
// │ [0] -> &Derived::~Derived            │  <- OVERRIDDEN entry
// │ [1] -> &Derived::MemberFunction      │  <- OVERRIDDEN entry
// └─────────────────────────────────────┘
//
// WHY does Derived need its OWN vtable (and not just reuse Base's)?
// -> because Derived OVERRIDES both functions
// -> the entries must point to DIFFERENT addresses (Derived's versions)
// -> if no override existed, Derived's vtable entry would just point
//    back to Base's implementation (inherited, not overridden)

// ============================================================
// SECTION 3: YOUR COMMENT - "vtable* is extra member variable"
//             CORRECTING THE DETAIL
// ============================================================

// YOUR COMMENT: "vtable*(extra member variable of class)
//                (inside stack frame of each object)"
//
// SLIGHT CORRECTION NEEDED:
//
// THE VTABLE ITSELF:
// -> created ONCE per CLASS at COMPILE TIME
// -> lives in the program's READ-ONLY DATA / CODE segment
// -> NOT per-object, NOT on the stack
// -> shared by ALL objects of that class
//
// THE VPTR (vtable pointer):
// -> THIS is the per-object thing you're thinking of
// -> EVERY object of a polymorphic class gets a HIDDEN pointer (vptr)
// -> vptr is added as an EXTRA hidden member at the START of the object
// -> vptr POINTS TO that class's vtable
// -> vptr lives WHEREVER the object lives (stack OR heap)
//
// CORRECTED PICTURE:
//
// class Base { virtual void f(); int* baseData; };
//
// MEMORY LAYOUT of a Base object:
// ┌──────────────────┐
// │ vptr (8 bytes)    │ <- hidden, points to Base::vtable
// │ baseData (8 bytes)│ <- your actual member
// └──────────────────┘
// sizeof(Base) = 16 bytes (8 for vptr + 8 for baseData pointer)
// WITHOUT virtual functions: sizeof(Base) would be 8 bytes (just baseData)
// -> virtual functions ADD 8 bytes (vptr) to EVERY object!
//
// MEMORY LAYOUT of a Derived object:
// ┌──────────────────┐
// │ vptr (8 bytes)    │ <- points to Derived::vtable (NOT Base's!)
// │ baseData (8 bytes)│ <- inherited from Base
// │ derivedData(8byte)│ <- Derived's own
// └──────────────────┘
// sizeof(Derived) = 24 bytes
//
// SO TO ANSWER PRECISELY:
// -> vtable (the array of function pointers) = ONE per CLASS, compile time, shared
// -> vptr (pointer to that vtable) = ONE per OBJECT, set during construction

// ============================================================
// SECTION 4: WHEN IS vptr SET? - DURING CONSTRUCTION
// ============================================================

// new Derived();
//
// STEP 1: memory allocated for Derived object (24 bytes)
// STEP 2: Base() constructor runs
//         -> compiler SECRETLY sets vptr = &Base::vtable FIRST
//            (because at this exact moment, only Base part exists)
//         -> baseData = new int[10]
// STEP 3: Derived() constructor runs
//         -> compiler SECRETLY updates vptr = &Derived::vtable
//            (NOW the object is "fully" a Derived)
//         -> derivedData = new int[15]
//
// THIS IS WHY: calling a virtual function FROM INSIDE a base
// constructor does NOT use the derived override!
// (vptr hasn't been updated to Derived's vtable yet at that point)
// -> classic interview gotcha: "never call virtual functions
//    from constructors expecting derived behavior"

class Base
{
public:
    Base()
    {
        std::cout << "Base class constructor called" << std::endl;
        baseData = new int[10];
        // vptr currently points to Base::vtable at this point
    }
    virtual ~Base()
    {
        std::cout << "Base class destructor called" << std::endl;
        delete[] baseData;
        // vptr has been reverted to Base::vtable by now too
        // (destructors run in reverse - Derived's vptr already
        //  reverted before ~Base() body executes)
    }
    virtual void MemberFunction()
    {
        std::cout << "Base class member function called" << std::endl;
    }
    int* baseData;
};

class Derived : public Base
{
public:
    Derived()
    {
        std::cout << "Derived class constructor called" << std::endl;
        derivedData = new int[15];
        // vptr NOW points to Derived::vtable (updated after Base() finished)
    }
    ~Derived()
    {
        std::cout << "Derived class destructor called" << std::endl;
        delete[] derivedData;
    }
    void MemberFunction() override
    {
        std::cout << "Derived class member function called" << std::endl;
    }
    int* derivedData;
};

// ============================================================
// SECTION 5: TRACING YOUR EXACT main() - TWO SCENARIOS
// ============================================================

int main()
{
    // ------------------------------------------------------------
    // SCENARIO 1: Base* pointing to a Derived object
    // ------------------------------------------------------------

    Base* instance = new Derived();
    // construction: Base() then Derived()
    // vptr ends up pointing to Derived::vtable

    instance->MemberFunction();
    // instance is declared as Base*, but the OBJECT is Derived
    // virtual dispatch:
    //   1. follow instance's vptr (NOT the pointer's declared type!)
    //   2. vptr -> Derived::vtable
    //   3. look up MemberFunction entry -> Derived::MemberFunction
    //   4. CALL Derived::MemberFunction
    // prints: "Derived class member function called"

    delete instance;
    // virtual destructor -> vtable lookup -> finds Derived's dtor
    // ~Derived() then ~Base() (correct chain, no leak)

    // ------------------------------------------------------------
    // SCENARIO 2: Base* pointing to an ACTUAL Base object
    // ------------------------------------------------------------

    instance = new Base;
    // construction: ONLY Base() runs (it IS a Base, nothing more)
    // vptr points to Base::vtable

    instance->MemberFunction();
    // virtual dispatch:
    //   1. follow instance's vptr
    //   2. vptr -> Base::vtable (because object IS actually Base)
    //   3. look up MemberFunction entry -> Base::MemberFunction
    //   4. CALL Base::MemberFunction
    // prints: "Base class member function called"

    delete instance;
    // ~Base() called (nothing to chain to, it's a pure Base object)

    return 0;
}

// FULL OUTPUT:
// Base class constructor called
// Derived class constructor called
// Derived class member function called    <- vtable picked Derived's version
// Derived class destructor called
// Base class destructor called
// Base class constructor called
// Base class member function called       <- vtable picked Base's version
// Base class destructor called

// ============================================================
// SECTION 6: THE KEY INSIGHT - POINTER TYPE vs OBJECT TYPE
// ============================================================

// instance is ALWAYS declared as Base*
// but its BEHAVIOR changes based on what it ACTUALLY points to:
//
// Base* instance = new Derived();  -> vptr -> Derived::vtable -> Derived behavior
// instance = new Base;             -> vptr -> Base::vtable    -> Base behavior
//
// THIS is the entire point of polymorphism:
// "the POINTER TYPE tells you WHAT INTERFACE you can call
//  the OBJECT TYPE (via vptr/vtable) tells you WHICH IMPLEMENTATION runs"

// ============================================================
// SECTION 7: INTERVIEW QUESTIONS
// ============================================================

// Q1: Why does Derived have a vtable if it doesn't write 'virtual' itself?
//     -> virtual-ness is inherited from Base. Any function overriding
//        a virtual base function is automatically virtual too,
//        forcing Derived to participate in the vtable mechanism.

// Q2: Is there one shared vtable for Base and Derived, or separate ones?
//     -> Separate. Each class gets its OWN vtable at compile time.
//        Derived's vtable entries point to Derived's overridden
//        functions; entries for non-overridden functions point
//        back to Base's implementation.

// Q3: Where does the vtable live in memory? Where does vptr live?
//     -> vtable: one per class, compile-time constant, lives in
//                read-only data/code segment, shared by all objects.
//        vptr:   one per OBJECT, hidden member, lives wherever the
//                object lives (stack or heap), points to its class's vtable.

// Q4: Does adding virtual functions increase object size?
//     -> YES. Adds one vptr (8 bytes on 64-bit) to every object of
//        that class, regardless of how many virtual functions exist.

// Q5: When is vptr set during object construction?
//     -> Set (and re-set) at the START of EACH constructor in the
//        chain. Base's constructor sets vptr to Base::vtable first;
//        Derived's constructor then updates it to Derived::vtable.

// Q6: What happens if you call a virtual function inside a base
//     class constructor?
//     -> It calls the BASE version, not the derived override, because
//        vptr still points to Base::vtable at that point in construction
//        (Derived's constructor hasn't run yet to update it).

// Q7: What determines which function actually runs - pointer type
//     or object type?
//     -> OBJECT type (via vptr/vtable), NOT pointer/reference type.
//        This is the core mechanism of dynamic dispatch.

// Q8: Why is Base's vtable used for "instance = new Base;" in this code?
//     -> Because at that point, instance genuinely points to an actual
//        Base object (not a Derived one). Its vptr was set to
//        Base::vtable during construction, so the lookup correctly
//        resolves to Base's MemberFunction.
// ```

// **Correcting the one detail in your notes:**

// ```
// vtable  = ONE per CLASS  (compile time, shared, lives in code segment)
// vptr    = ONE per OBJECT (runtime, hidden member, points to vtable)

// NOT: "vtable is extra member variable on stack"
// YES: "vptr is the extra hidden member; vtable itself is class-level, not per-object"
// ```

// **The construction-time vptr update - the real interview gotcha:**

// ```
// new Derived():
//   Base()    runs → vptr = &Base::vtable     (temporarily!)
//   Derived() runs → vptr = &Derived::vtable  (final, correct value)

// → calling a virtual function INSIDE Base's constructor
//   uses Base's version, even though the object will eventually be Derived
// ```

// > **One sentence to remember**: the vtable is a **per-class blueprint** built once at compile time; the vptr is a **per-object compass** set during construction that tells `delete`/virtual calls which blueprint to follow.





// Yes — **virtual-ness propagates infinitely down the chain, no matter how many levels deep:**

// ```cpp
// class Base {
// public:
//     virtual void MemberFunction() { std::cout << "Base\n"; }
//     //  ^^^^^^^ declared virtual ONCE, here
// };

// class Derived : public Base {
// public:
//     void MemberFunction() override { std::cout << "Derived\n"; }
//     // no 'virtual' written - but STILL virtual (inherited)
// };

// class GrandChild : public Derived {
// public:
//     void MemberFunction() override { std::cout << "GrandChild\n"; }
//     // ALSO no 'virtual' written - STILL virtual
//     // doesn't matter that Derived (its DIRECT parent) didn't write it either
//     // traces all the way back to Base's original declaration
// };

// class GreatGrandChild : public GrandChild {
// public:
//     void MemberFunction() override { std::cout << "GreatGrandChild\n"; }
//     // still virtual, 4 levels down, automatically
// };
// ```

// **Proof with polymorphism:**

// ```cpp
// Base* p = new GreatGrandChild();
// p->MemberFunction();
// // prints: "GreatGrandChild"
// // even though p is declared as Base*
// // even though NONE of the derived classes wrote 'virtual'
// // the chain still works perfectly
// ```

// **Why this works - each level builds its own vtable entry:**

// ```
// Base::vtable           [0] -> Base::MemberFunction
// Derived::vtable        [0] -> Derived::MemberFunction        (overrides Base's)
// GrandChild::vtable     [0] -> GrandChild::MemberFunction      (overrides Derived's)
// GreatGrandChild::vtable[0] -> GreatGrandChild::MemberFunction (overrides GrandChild's)

// each class still gets its OWN vtable
// each one's entry points to ITS OWN version
// the chain just keeps replacing the same "slot"
// ```

// **One rule, applies forever:**

// > Virtual is declared **once**, at the **top** of the hierarchy. Every class below — no matter how many generations — **automatically inherits the virtual-ness** for that function. Writing `virtual` again at lower levels is optional (purely for readability); `override` is what you should actually write every time, because it makes the compiler **verify** you're correctly overriding something up the chain.

// **The only way to break this chain:** there isn't one. Once virtual, always virtual, for that function name+signature, for the rest of the hierarchy — permanently.