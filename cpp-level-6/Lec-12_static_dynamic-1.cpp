// ============================================================
// C++ CASTING - static_cast, dynamic_cast, reinterpret_cast - NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: CORE GUIDELINES (from your comments)
// ============================================================

// ES.49: "If you must use a cast, use a named cast"
// WHY named cast over C-style (type)value:
// -> MORE SEARCHABLE (grep for "dynamic_cast" finds all uses)
// -> SHOWS INTENT (reader knows exactly what kind of cast)
// -> COMPILER CATCHES inappropriate uses (C-style too liberal)
//
// C.146: Use dynamic_cast where class hierarchy navigation is unavoidable
//
// GENERAL RULE:
// AVOID casts where possible
// PREFER dynamic_cast if you must cast (safety at runtime)
// USE static_cast if you KNOW the cast is safe AND need performance
// NEVER use reinterpret_cast unless dealing with raw memory/hardware

// ============================================================
// SECTION 2: THE THREE CLASSES
// ============================================================

struct Base
{
    int basedata{1};
    Base() { basedata = 11; }

    virtual void Do()   // VIRTUAL = enables dynamic_cast + runtime polymorphism
    {
        std::cout << "Base::Do() \t" << basedata << "\n";
    }
    // virtual ~Base() = default;  // SHOULD have this! (as learned before)
};

struct Derived : Base
{
    int deriveddata{2};
    Derived() { deriveddata = 22; }

    void Do() override
    {
        std::cout << "Derived::Do()\t" << deriveddata << "\n";
    }
};

struct Unrelated
{
    float unrelateddata{3.14f};
    Unrelated() { unrelateddata = 33.1414f; }

    void Do()
    {
        std::cout << "Unrelated::Do()\t" << unrelateddata << "\n";
    }
    // NO virtual -> not polymorphic
    // NOT related to Base or Derived at all
};

// ============================================================
// SECTION 3: static_cast - SAFE KNOWN CONVERSIONS
// ============================================================

// static_cast = compile-time cast, NO runtime safety check
// use when YOU KNOW the cast is valid
//
// CASE 1: Numeric conversion (safe, explicit)
// float f{3.14f};
// int i = static_cast<int>(f);  // explicit truncation to 3
// SAME as: int i = f; or int i = (int)f;
// BUT: intent is CLEAR, searchable, catches some errors
//
// CASE 2: Upcasting (Derived -> Base) - ALWAYS safe
// Derived d;
// static_cast<Base>(d).Do();  // treat 'd' as Base
// -> Base::Do() called (value semantics, object SLICED to Base)
// -> object slicing: derived-specific data LOST!
//
// CASE 3: Downcasting (Base* -> Derived*) - UNSAFE without check!
// Base* b = new Base;
// static_cast<Derived*>(b)->Do();  // COMPILES but WRONG!
// -> b is actually a Base object, not a Derived
// -> accesses deriveddata which doesn't exist in Base
// -> undefined behavior! reads garbage memory
// -> NO runtime check - static_cast trusts YOU
//
// WHEN static_cast downcast IS safe:
// Base* b = new Derived;           // b ACTUALLY points to Derived
// static_cast<Derived*>(b)->Do();  // safe! we KNOW it's Derived
// (but prefer dynamic_cast for safety unless performance critical)

// ============================================================
// SECTION 4: dynamic_cast - SAFE RUNTIME-CHECKED CAST
// ============================================================

// dynamic_cast REQUIREMENTS:
// 1. Only works with POINTER or REFERENCE types
// 2. Types must be POLYMORPHIC (have at least one virtual function)
//    -> needs vtable to check actual runtime type
//    -> if no virtual functions: compile error
//
// dynamic_cast<T*>(ptr):
// -> returns T* if cast is valid (ptr actually IS a T)
// -> returns nullptr if cast is INVALID
// -> check result before using! (null check)
//
// dynamic_cast<T&>(ref):
// -> returns T& if valid
// -> throws std::bad_cast if invalid (reference can't be null)

// ============================================================
// SECTION 5: main() - COMPLETE TRACE
// ============================================================

int main()
{
    Base*    b = new Base;     // b points to actual Base object
    Derived* d = new Derived;  // d points to actual Derived object

    // ---- DYNAMIC_CAST 1: Downcast Base* to Derived* ----
    if (dynamic_cast<Derived*>(b))
    // b actually points to Base, NOT Derived
    // dynamic_cast checks vtable at RUNTIME
    // vtable says: "this is a Base, not a Derived"
    // cast FAILS -> returns nullptr -> false
    // if block SKIPPED (correctly!)
    {
        std::cout << "Treating 'b' like Derived\n";
        // NEVER prints - correct behavior!
    }

    // ---- DYNAMIC_CAST 2: Upcast Derived* to Base* ----
    if (dynamic_cast<Base*>(d))
    // d is Derived, Derived IS-A Base
    // Derived inherits from Base
    // upcasting is ALWAYS valid
    // dynamic_cast succeeds -> returns valid Base* -> true
    // if block ENTERED
    {
        std::cout << "Treating 'd' like Base\n";

        // CALL 1: d->Do()
        d->Do();
        // d is Derived*, virtual dispatch
        // -> Derived::Do() called
        // prints: "Derived::Do()  22"

        // CALL 2: d->Base::Do()
        d->Base::Do();
        // EXPLICIT scope resolution: "call Base's version specifically"
        // BYPASSES virtual dispatch (calls Base::Do directly)
        // prints: "Base::Do()     11"

        // CALL 3: d->Derived::Do()
        d->Derived::Do();
        // EXPLICIT scope resolution: "call Derived's version specifically"
        // also bypasses virtual dispatch
        // prints: "Derived::Do()  22"

        // CALL 4: (static_cast<Base*>(d))->Base::Do()
        (static_cast<Base*>(d))->Base::Do();
        // static_cast: d (Derived*) -> Base* (upcast, always safe)
        // Base::Do() with explicit scope -> calls Base version
        // no runtime overhead (static_cast + explicit scope)
        // prints: "Base::Do()     11"
        //
        // YOUR COMMENT: "no runtime cost here" - CORRECT!
        // when you KNOW d is a valid Derived, can use static_cast
        // for same result as dynamic_cast but with zero runtime overhead

        // CALL 5: (static_cast<Base*>(d))->Do()  [COMMENTED OUT]
        // (static_cast<Base*>(d))->Do();
        // cast d to Base*, then call virtual Do()
        // VIRTUAL DISPATCH still happens based on ACTUAL object type!
        // actual object is Derived -> Derived::Do() called
        // YOUR COMMENT: "static_cast does not affect the virtual call"
        // -> even after casting pointer to Base*, vtable still used
        // -> still calls Derived::Do() (correct polymorphic behavior)
        // prints: "Derived::Do()  22"
        // -> dynamic_cast here would give same result

        // CALL 6: ((Unrelated*)d)->Do()  [C-STYLE CAST - DANGEROUS]
        ((Unrelated*)d)->Do();
        // Unrelated is COMPLETELY unrelated to Base/Derived hierarchy
        // C-style cast: forces compiler to treat d's memory as Unrelated
        // Unrelated::Do() reads unrelateddata (float)
        // but d's memory doesn't have a float where unrelateddata should be!
        // reads GARBAGE (deriveddata, basedata, or vtable pointer bytes)
        // -> UNDEFINED BEHAVIOR, prints garbage float value
        //
        // YOUR COMMENT: "this will compile but will not work as expected"
        // CORRECT - compiler doesn't stop you, behavior is garbage
        //
        // dynamic_cast<Unrelated*>(d)->Do()  [COMMENTED OUT - CRASH]
        // dynamic_cast checks: is d an Unrelated? -> NO -> returns nullptr
        // ->Do() on nullptr -> SEGMENTATION FAULT (crash!)
        //
        // reinterpret_cast<Unrelated*>(d)->Do()
        // same as C-style cast: reinterprets d's bits as Unrelated*
        // same undefined behavior as C-style cast

        // if (dynamic_cast<Unrelated*>(d))  [COMMENTED OUT]
        // {
        //     std::cout << "this will not be printed\n";
        // }
        // dynamic_cast: d is not Unrelated -> nullptr -> false
        // SAFE way to check before using! if block correctly skipped
    }

    // NOTE: memory leak! should be:
    delete b;
    delete d;
    // no virtual destructor in Base -> only ~Base() called for d -> leak!
    // (another reason to have virtual ~Base())
}

// ============================================================
// SECTION 6: ALL FOUR CASTS SUMMARIZED
// ============================================================

// static_cast<T>(expr)
// -> compile-time only, no runtime check
// -> for: numeric conversions, upcasts (always safe)
//         known-safe downcasts (when YOU guarantee the type)
// -> fast: zero runtime overhead
// -> DANGER: no safety net for wrong downcasts

// dynamic_cast<T*>(ptr)
// -> runtime check using vtable
// -> for: safe downcasting in polymorphic hierarchy
// -> safe: returns nullptr if invalid (or throws for references)
// -> REQUIRES: virtual functions in class (polymorphic type)
// -> COST: small runtime overhead (vtable lookup)

// const_cast<T>(expr)
// -> adds/removes const qualifier
// -> for: passing to legacy C APIs that lack const
// -> DANGER: undefined behavior if you modify originally-const object

// reinterpret_cast<T>(expr)
// -> raw bit reinterpretation, no conversion
// -> for: hardware registers, pointer-to-integer, type punning
// -> MOST DANGEROUS: no type safety at all
// -> same result as C-style cast for unrelated types

// ============================================================
// SECTION 7: WHEN EACH CAST IS APPROPRIATE
// ============================================================

// | Situation                          | Use                    |
// |------------------------------------|------------------------|
// | Numeric conversion (int->float)    | static_cast            |
// | Known-safe upcast (Derived->Base)  | static_cast (implicit) |
// | Unknown downcast (Base*->Derived*) | dynamic_cast           |
// | Remove const for legacy C API      | const_cast             |
// | Hardware register / raw memory     | reinterpret_cast       |
// | ANY unrelated type cast            | DON'T (redesign!)      |

// ============================================================
// SECTION 8: SLICING - static_cast WITH VALUES (not pointers)
// ============================================================

// Derived d;
// Base b = static_cast<Base>(d);  // OBJECT SLICING!
// -> creates a BASE OBJECT from Derived
// -> COPIES only the Base PORTION of d
// -> deriveddata (22) LOST FOREVER
// -> b.Do() calls Base::Do() (it's now a Base object, not Derived)
//
// THIS IS WHY polymorphism needs POINTERS or REFERENCES:
// Base* ptr = &d;  // ptr points to full Derived object
// ptr->Do();       // Derived::Do() called (vtable preserved)
//
// Base val = d;    // SLICED: only Base part, Derived::Do() never called
// val.Do();        // Base::Do() called (not polymorphic!)

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between static_cast and dynamic_cast?
//     -> static_cast: compile-time, no runtime check, fast.
//        Trusts programmer that cast is valid. Wrong downcast = UB.
//        dynamic_cast: runtime check via vtable. Returns nullptr
//        (or throws) if invalid. Requires polymorphic types.

// Q2: When does dynamic_cast return nullptr?
//     -> When the actual runtime type of the pointed-to object
//        cannot be cast to the requested type. Base* b = new Base;
//        dynamic_cast<Derived*>(b) = nullptr (b is not a Derived).

// Q3: What does dynamic_cast require to work?
//     -> The type must be POLYMORPHIC (have at least one virtual function).
//        This ensures a vtable exists for runtime type checking (RTTI).
//        Works only with pointer or reference types, not values.

// Q4: What happens when you C-style cast to an unrelated type?
//     -> Undefined behavior. Compiler accepts it, but accessing members
//        reads garbage (wrong offsets, wrong types). Same as
//        reinterpret_cast - just reinterprets the raw bits.

// Q5: Does static_cast affect virtual dispatch?
//     -> NO. Even after static_cast<Base*>(derivedPtr), calling a
//        virtual function still uses the vtable of the ACTUAL object.
//        The cast changes the pointer type but not the object itself.

// Q6: What is object slicing?
//     -> When a Derived object is assigned/cast to a Base VALUE (not pointer).
//        Only the Base portion is copied, derived-specific data is lost.
//        Polymorphism breaks: the copy is a Base, not Derived.
//        Use pointers/references to avoid slicing.

// Q7: Why prefer dynamic_cast over C-style cast for downcasting?
//     -> dynamic_cast checks at runtime if cast is valid.
//        Returns nullptr for invalid cast (catchable!).
//        C-style cast blindly reinterprets memory -> UB, crashes.
//        Also: dynamic_cast is searchable (grep), shows clear intent.

// Q8: What is RTTI and why does dynamic_cast need it?
//     -> Run-Time Type Information. Data stored alongside each object
//        (in/near the vtable) describing its actual type.
//        dynamic_cast uses RTTI to check if "this Base* is actually
//        a Derived*" at runtime. No virtual = no vtable = no RTTI
//        = dynamic_cast won't compile for that type.
// ```

// **The four casts - when to use which:**

// ```
// static_cast   → YOU guarantee correctness at compile time
//                 Zero runtime cost. Wrong downcast = undefined behavior.

// dynamic_cast  → Let RUNTIME verify correctness via vtable
//                 Small cost. Wrong downcast = nullptr (safe!).
//                 REQUIRES virtual functions.

// const_cast    → Only for adding/removing const
//                 Use reluctantly, mostly for C API interop.

// reinterpret_cast → Raw bit reinterpretation
//                    Same as C-style cast. Last resort only.
//                    Hardware, protocol buffers, type punning.
// ```

// **The key behavior to remember:**

// ```cpp
// Base* b = new Base;
// Base* bActuallyDerived = new Derived;

// // DOWNCAST to Derived:
// dynamic_cast<Derived*>(b)               // nullptr  ← safe, correct!
// dynamic_cast<Derived*>(bActuallyDerived) // valid ptr ← safe, correct!
// static_cast<Derived*>(b)                // UB!      ← compiles, crashes

// // VIRTUAL DISPATCH is NOT affected by cast:
// (static_cast<Base*>(new Derived))->Do(); // Derived::Do() still called!
// //                                        vtable ignores the cast
// ```

// > **The core message**: casting is a signal that something is wrong with your design. Before reaching for `dynamic_cast`, ask if you can restructure to avoid it. But when you **must** cast in a hierarchy, `dynamic_cast` is the safe choice — it says *"check at runtime if this is valid and give me nullptr if it's not"*, which is infinitely better than the silent corruption you get from `static_cast` or C-style casts on wrong types.