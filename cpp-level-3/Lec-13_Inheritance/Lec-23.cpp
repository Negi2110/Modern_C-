// ============================================================
// COMPOSITION vs AGGREGATION vs INHERITANCE - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: THE THREE RELATIONSHIP TYPES - FROM FIRST PRINCIPLES
// ============================================================

// INHERITANCE   -> "IS-A"     -> Golden IS-A Dog
// COMPOSITION   -> "HAS-A"    -> Character HAS-A position (strong ownership)
// AGGREGATION   -> "HAS-A"    -> Character HAS-A reference to a home
//                                (weak ownership / borrowed, not owned)
//
// YOUR COMMENT: "to avoid inheritance use composition = has-a"
// CORRECT! When the relationship is NOT genuinely "is-a",
// prefer composition - it's more flexible and less coupled.

struct Point2d
{
    int x, y;
};

// ============================================================
// SECTION 2: INHERITANCE VERSION (commented out - the WRONG choice here)
// ============================================================

// class Character : public Point2d   // inheritance
// {
// public:
//     Character() {}
//     ~Character() {}
// };
//
// WHY THIS IS WRONG DESIGN:
// "Is a Character a Point2d?" -> doesn't make logical sense!
// A Character HAS a position, it ISN'T literally a coordinate.
//
// PRACTICAL PROBLEMS this creates:
// -> Character would inherit Point2d's x,y DIRECTLY as ITS OWN x,y
//    (confusing - what does "Character.x" even mean conceptually?)
// -> Character can ONLY have ONE position this way
//    (what if Character needs BOTH a position AND a direction,
//     which are both Point2d-shaped? Can't inherit from Point2d twice!)
// -> THIS is exactly the limitation composition solves

// ============================================================
// SECTION 3: COMPOSITION VERSION (commented out - the CORRECT pattern)
// ============================================================

// class Character   // "has-a" relationship - composition
// {
// public:
//     Character() {}
//     ~Character() {}
// private:
//     Point2d m_position;    // Character HAS a position
//     Point2d m_direction;   // Character HAS a direction
// };
//
// NOW Character can have MULTIPLE Point2d members - no inheritance limit!
// MUCH cleaner: "Character.m_position.x" reads naturally
//
// COMPOSITION DEFINITION (the STRONG version of has-a):
// -> the CONTAINED object's LIFETIME is TIED to the CONTAINING object
// -> m_position is BORN when Character is born (member initialization)
// -> m_position DIES when Character dies (member destruction)
// -> Character FULLY OWNS m_position - nobody else can access it,
//    nobody else manages its lifetime
// -> THIS is the most common, safest relationship in OOP design

// ============================================================
// SECTION 4: AGGREGATION VERSION (your ACTUAL active code)
// ============================================================

class Character   // also "has-a" - but AGGREGATION this time
{
public:
    Character(Point2d& ref) : ref_home(ref)
    //                        ^^^^^^^^^^^^
    // reference member MUST be initialized in initializer list
    // (you learned this rule already: references can't be assigned later,
    //  must be BOUND at construction time)
    {
    }
    ~Character() {}

private:
    Point2d m_position;    // COMPOSITION (owned, lifetime tied to Character)
    Point2d m_direction;   // COMPOSITION (owned, lifetime tied to Character)

    Point2d& ref_home;     // AGGREGATION (borrowed, NOT owned!)
    // ^
    // this is a REFERENCE to an EXTERNAL Point2d
    // Character does NOT own this object
    // Character does NOT control its lifetime
    // Character just "knows about" / "points to" it
};

// ============================================================
// SECTION 5: YOUR COMMENT - THE KEY DIFFERENCE EXPLAINED
// ============================================================

// YOUR COMMENT: "also has-a relationship aggregation but difference
//                is lifetime not necessarily tied to the object"
// EXACTLY RIGHT - here's the precise distinction:
//
// COMPOSITION:
// -> "Character OWNS this Point2d"
// -> m_position created WHEN Character is created
// -> m_position destroyed WHEN Character is destroyed
// -> NOBODY ELSE has access to or responsibility for m_position
// -> if Character dies, m_position is GONE (its lifetime ENDS with Character)
//
// AGGREGATION:
// -> "Character KNOWS ABOUT / REFERS TO this Point2d, but doesn't own it"
// -> ref_home points to somePoint, which was created SEPARATELY,
//    BEFORE Character even existed
// -> if Character dies, somePoint is COMPLETELY UNAFFECTED
//    -> somePoint continues to exist, has its OWN independent lifetime
// -> Character is just "borrowing" access to it

// ============================================================
// SECTION 6: TRACING YOUR EXACT CODE - LIFETIME PROOF
// ============================================================

Point2d somePoint;
// ^
// GLOBAL variable - created when program STARTS
// lives for the ENTIRE duration of the program
// destroyed only when program ENDS (after main() returns)

void foo()
{
    Character mike(somePoint);
    // mike is created HERE, passing a REFERENCE to somePoint
    // mike.ref_home now refers to the SAME somePoint (no copy!)
    //
    // mike.m_position  -> OWNED by mike (composition) - exists only
    //                     as long as mike exists
    // mike.m_direction -> OWNED by mike (composition) - same as above
    // mike.ref_home    -> REFERS to somePoint (aggregation) -
    //                     somePoint exists INDEPENDENTLY of mike

}   // foo() returns -> mike goes out of scope -> mike is DESTROYED
    // mike's destructor runs:
    // -> m_position destroyed (it was OWNED, dies with mike)
    // -> m_direction destroyed (it was OWNED, dies with mike)
    // -> ref_home is JUST A REFERENCE - there's nothing to "destroy"
    //    references don't own memory, they just point to it
    // -> somePoint is COMPLETELY UNAFFECTED - still exists!
    //    (it's a global, will exist until the program itself ends)

int main()
{
    foo();
    // AFTER foo() returns:
    // mike no longer exists (destroyed when foo() ended)
    // somePoint STILL EXISTS (global, survives beyond mike's lifetime)
    //
    // THIS PROVES THE AGGREGATION CONCEPT:
    // the REFERENCED object (somePoint) outlived the
    // REFERENCING object (mike)
    // -> their lifetimes are INDEPENDENT - this is the defining
    //    characteristic of aggregation vs composition

    return 0;
}

// ============================================================
// SECTION 7: SIDE-BY-SIDE COMPARISON TABLE
// ============================================================

// | Aspect              | Composition          | Aggregation            |
// |----------------------|------------------------|--------------------------|
// | Relationship          | "has-a" (strong)       | "has-a" (weak)           |
// | Ownership              | YES - fully owns       | NO - just references     |
// | Lifetime tied?         | YES - dies together    | NO - independent         |
// | Typical member type    | by VALUE (Point2d m_x) | by REFERENCE/POINTER     |
// | Created by             | the containing object  | created elsewhere, passed in |
// | Example here           | m_position, m_direction| ref_home                 |

// ============================================================
// SECTION 8: WHY USE A REFERENCE FOR AGGREGATION (not a pointer)?
// ============================================================

// Point2d& ref_home;   // reference chosen here
//
// COULD have used a pointer instead:
// Point2d* ptr_home;   // pointer alternative
//
// WHY reference is often preferred for aggregation:
// -> reference CANNOT be null (guaranteed to always refer to something)
// -> reference CANNOT be reassigned later (always points to the SAME
//    somePoint for mike's entire lifetime - a stable, fixed relationship)
// -> if the relationship NEEDS to change later (Character could be
//    reassigned to a DIFFERENT home at some point), a POINTER would
//    be the better choice instead (pointers CAN be reassigned)

// ============================================================
// SECTION 9: GENERAL DESIGN GUIDANCE - WHEN TO USE WHAT
// ============================================================

// USE INHERITANCE when:
// -> genuine IS-A relationship
// -> derived class needs to be usable POLYMORPHICALLY through base pointer
// -> Golden IS-A Dog -> makes sense, Golden can BE used as a Dog
//
// USE COMPOSITION when:
// -> HAS-A relationship, AND the contained object's lifetime
//    should be fully controlled by the container
// -> most common, safest, most flexible choice in everyday design
// -> Character HAS-A position (Character creates/owns/destroys it)
//
// USE AGGREGATION when:
// -> HAS-A relationship, but the object is SHARED, EXTERNALLY OWNED,
//    or needs to OUTLIVE the container
// -> Character KNOWS ABOUT a home, but doesn't create or destroy it
// -> common for: shared resources, references to managers/services,
//    parent-child UI relationships where parent isn't "owned" by child

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between composition and aggregation?
//     -> Both are "has-a" relationships. Composition: the contained
//        object's lifetime is tied to (owned by) the container - it's
//        created and destroyed with it. Aggregation: the object exists
//        independently - the container just references/borrows it.

// Q2: Why prefer composition over inheritance when there's no true
//     IS-A relationship?
//     -> Avoids forcing an unnatural "is-a" semantic, allows multiple
//        instances of the same type as members (inheritance allows
//        only one base of each type), and reduces coupling.

// Q3: In the Character example, which members are composition and
//     which is aggregation? Why?
//     -> m_position and m_direction: composition (owned by value,
//        created/destroyed with Character).
//        ref_home: aggregation (reference to an externally-owned
//        Point2d, independent lifetime).

// Q4: What happens to ref_home when the Character object is destroyed?
//     -> Nothing happens to the referenced object (somePoint). The
//        reference itself simply ceases to exist along with Character,
//        but the object it pointed to is completely unaffected.

// Q5: Why must ref_home be initialized in the constructor's initializer
//     list rather than the constructor body?
//     -> References must be bound at the moment of creation - they
//        cannot be left uninitialized or reassigned afterward. This
//        is a hard C++ language rule for all reference members.

// Q6: Why might you choose a pointer instead of a reference for an
//     aggregation relationship?
//     -> If the relationship needs to be optional (nullable) or
//        reassignable to a different object later, a pointer is more
//        appropriate since references cannot be null or rebound.

// Q7: Is composition generally considered safer than aggregation?
//     -> Yes, generally. Composition has clear, predictable ownership
//        and lifetime - no risk of dangling references. Aggregation
//        requires the programmer to guarantee the referenced object
//        outlives the aggregating object, which is a manual responsibility.

// Q8: What design problem does composition solve that inheritance can't?
//     -> Inheritance only allows ONE base class of a given type. With
//        composition, Character can have BOTH m_position AND
//        m_direction as separate Point2d members - inheritance could
//        never express "has two of the same type."
// ```

// **The three relationships in one picture:**

// ```
// INHERITANCE (IS-A):        COMPOSITION (HAS-A, owned):    AGGREGATION (HAS-A, borrowed):
// Golden : public Dog        class Character {              class Character {
//                                 Point2d m_position;  ←owns      Point2d& ref_home;  ←borrows
//                             };  (dies with Character)      };  (independent lifetime)
// ```

// **Lifetime proof from your exact code:**

// ```cpp
// Point2d somePoint;          // born when program starts

// void foo() {
//     Character mike(somePoint);
// }   // mike dies HERE — but somePoint keeps living!

// // the referenced object OUTLIVED the object referencing it
// // → this is the defining signature of aggregation
// ```

// > **The interview soundbite**: composition = *"I made you, I own you, you die when I die."* Aggregation = *"I know about you, I use you, but you existed before me and will exist after me."*