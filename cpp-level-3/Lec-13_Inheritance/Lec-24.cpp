// ============================================================
// VIRTUAL INHERITANCE - SOLVING THE DIAMOND PROBLEM - NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: SETTING UP THE DIAMOND
// ============================================================

struct Entity
{
    int x{4}, y{5};
};
//        Entity
//       /      \
//   Warrior   Goblin
//       \      /
//       Player
//
// THIS IS THE EXACT DIAMOND SHAPE from your earlier "Coltriever" example

// ============================================================
// SECTION 2: WHAT virtual DOES HERE - YOUR MAIN QUESTION
// ============================================================

struct Warrior : virtual public Entity
//               ^^^^^^^
{
    int power{10};
};

struct Goblin : virtual Entity
//              ^^^^^^^
// (note: 'virtual Entity' alone defaults to PRIVATE here since
//  Goblin is a struct... actually NO - struct defaults to PUBLIC
//  even with virtual, this is equivalent to 'virtual public Entity')
{
    int power{7};
};

// WITHOUT virtual (the diamond problem from your earlier session):
// -> Warrior gets its OWN private copy of Entity (x, y)
// -> Goblin gets its OWN SEPARATE private copy of Entity (x, y)
// -> Player (inheriting both) ends up with TWO Entity subobjects
//    -> mike.x would be AMBIGUOUS (which Entity's x? Warrior's or Goblin's?)
//
// WITH virtual (what your code does):
// -> Warrior and Goblin AGREE to SHARE a single Entity subobject
// -> "virtual" tells the compiler: "don't give ME my own private
//    Entity copy - instead, COORDINATE with anyone else who ALSO
//    virtually inherits Entity, so we all point to the SAME ONE"
// -> Player ends up with ONLY ONE Entity subobject total
//    -> mike.x is now UNAMBIGUOUS - there's only one to choose from!
//
// THIS IS EXACTLY WHAT YOUR COMMENT SAYS:
// "what does virtual keyword do to solve this diamond problem"
// ANSWER: it makes Warrior and Goblin SHARE one Entity instead of
//         each having their own independent copy

// ============================================================
// SECTION 3: Player - THE DIAMOND CONVERGES
// ============================================================

struct Player : public Goblin, public Warrior
//              ^^^^^^^^^^^^^^  ^^^^^^^^^^^^^^
//              Goblin FIRST    Warrior SECOND
//              (order matters! see Section 5)
{
};

// ============================================================
// SECTION 4: main() - TRACING YOUR EXACT QUESTIONS
// ============================================================

int main()
{
    Player mike;

    std::cout << mike.x << std::endl;
    std::cout << mike.y << std::endl;
    // mike.x  -> UNAMBIGUOUS now (virtual inheritance shares ONE Entity)
    // WITHOUT virtual, this line would be a COMPILER ERROR
    // ("ambiguous access of x - via Goblin::Entity or Warrior::Entity?")

    // ----------------------------------------------------------
    // YOUR QUESTION: which x/y value does mike.x actually show,
    // and does inheritance ORDER decide it?
    // ----------------------------------------------------------
    //
    // ANSWER: YES, you are RIGHT - and here's PRECISELY why:
    //
    // With virtual inheritance, the SHARED Entity subobject is
    // constructed EXACTLY ONCE for the WHOLE hierarchy.
    // WHO constructs it? -> the MOST-DERIVED class (Player) is
    // responsible for initializing the virtual base directly!
    //
    // Since Player does NOT explicitly call Entity's constructor
    // itself (Player has no custom constructor here), the compiler
    // falls back to calling Entity's DEFAULT constructor ONCE.
    //
    // Entity's default member initializers (x{4}, y{5}) ALWAYS run
    // -> mike.x == 4, mike.y == 5 -> REGARDLESS of Goblin/Warrior order!
    //
    // YOUR INTUITION ("it took Goblin's x/y because Goblin was first
    // in the inheritance list") is a COMMON and REASONABLE guess,
    // but it's actually NOT what's happening here:
    // -> Goblin and Warrior do NOT have their own separate Entity
    //    copies to "win" with - there is only ONE shared Entity,
    //    and its value (x=4, y=5) comes from Entity's OWN default
    //    member initializers, NOT from "whichever derived class
    //    came first in the list"
    // -> Changing the order to "Warrior, Goblin" would NOT change
    //    mike.x or mike.y at all - it would STILL be 4 and 5,
    //    because there's still just ONE Entity, default-constructed
    //
    // WHAT inheritance ORDER actually affects (see Section 5):
    // -> construction/destruction ORDER of Goblin vs Warrior's OWN
    //    members (like their separate 'power' members)
    // -> NOT which Entity values "win" (there's only one Entity!)

    std::cout << mike.Goblin::power << std::endl;
    // mike.power        -> AMBIGUOUS! both Goblin AND Warrior have
    //                      their OWN 'power' member (NOT shared,
    //                      NOT virtual - only Entity was inherited
    //                      virtually, power is each class's OWN data)
    // mike.Goblin::power -> explicitly disambiguates: "give me
    //                       SPECIFICALLY Goblin's power" -> prints 7
    //
    // YOUR COMMENT CONFIRMS THIS:
    // "for this we have to be selective"
    // CORRECT - power is NOT part of the virtual-shared Entity,
    // so ambiguity STILL exists for power and MUST be resolved
    // manually with Goblin:: or Warrior::

    // std::cout << mike.power << std::endl;
    // STILL A COMPILER ERROR even with virtual inheritance!
    // YOUR COMMENT IS CORRECT: "after virtually inheriting it,
    // it's still error because same name"
    // WHY? virtual inheritance only deduplicates the SHARED BASE
    // (Entity). It does nothing for power, because power is
    // declared SEPARATELY and INDEPENDENTLY in Goblin and Warrior -
    // they were NEVER the same shared thing to begin with!

    return 0;
}

// ============================================================
// SECTION 5: WHAT INHERITANCE ORDER ACTUALLY CONTROLS
// ============================================================

// struct Player : public Goblin, public Warrior { };
//
// ORDER determines CONSTRUCTION SEQUENCE for Goblin and Warrior
// (their OWN non-shared parts, like 'power'):
//
// CONSTRUCTION ORDER for Player mike;
// 1. Entity (the SHARED virtual base) constructed FIRST, ALWAYS,
//    regardless of where it appears in the inheritance list
//    (virtual bases are ALWAYS constructed before ANY non-virtual
//    bases, by C++ language rule)
// 2. Goblin's "own part" constructed (power = 7)
//    (listed FIRST in "public Goblin, public Warrior")
// 3. Warrior's "own part" constructed (power = 10)
//    (listed SECOND)
// 4. Player's own body runs (empty here)
//
// IF YOU REVERSED THE ORDER:
// struct Player : public Warrior, public Goblin { };
// -> Entity STILL constructed first (rule never changes for virtual base)
// -> Warrior's own part constructed BEFORE Goblin's own part now
// -> mike.x / mike.y STILL = 4, 5 (Entity is STILL just one shared object)
// -> ONLY the construction ORDER of Warrior vs Goblin's "power" swaps

// ============================================================
// SECTION 6: YOUR COMMENTED-OUT EXPERIMENTS - WITHOUT virtual
// ============================================================

// mike.Warrior::x   // WORKS without virtual - explicitly picks
// mike.Warrior::y   // Warrior's OWN private copy of Entity's x,y
// mike.Goblin::x    // WORKS without virtual - explicitly picks
// mike.Goblin::y    // Goblin's OWN SEPARATE private copy
//
// WITHOUT virtual inheritance:
// -> Warrior::x and Goblin::x are TWO DIFFERENT memory locations
//    (TWO separate Entity subobjects exist)
// -> mike.Warrior::x might be 4, mike.Goblin::x might ALSO be 4
//    (same VALUE, because both default-initialize the same way)
//    BUT they are STORED at DIFFERENT ADDRESSES in memory!
// -> mike.x ALONE (no ::) would be AMBIGUOUS -> compiler error
//
// WITH virtual inheritance (your actual active code):
// -> mike.Warrior::x and mike.Goblin::x would now refer to the
//    EXACT SAME memory location (the ONE shared Entity)
// -> mike.x works WITHOUT needing :: at all, because there's
//    only one possible x to refer to

// ============================================================
// SECTION 7: MEMORY LAYOUT - VISUAL PROOF
// ============================================================

// WITHOUT virtual inheritance:
//
// Player mike:
// ┌─────────────────────────┐
// │ Goblin::Entity  x=4 y=5 │  <- Goblin's OWN private Entity copy
// │ Goblin::power = 7       │
// ├─────────────────────────┤
// │ Warrior::Entity x=4 y=5 │  <- Warrior's OWN SEPARATE Entity copy
// │ Warrior::power = 10     │
// └─────────────────────────┘
// TWO Entity copies exist - mike.x is AMBIGUOUS

// WITH virtual inheritance:
//
// Player mike:
// ┌─────────────────────────┐
// │ Goblin::power = 7       │  <- own data, NOT shared
// ├─────────────────────────┤
// │ Warrior::power = 10     │  <- own data, NOT shared
// ├─────────────────────────┤
// │ shared Entity  x=4 y=5  │  <- ONE Entity, both Goblin and
// └─────────────────────────┘     Warrior point to THIS SAME spot
// ONE Entity copy exists - mike.x is now UNAMBIGUOUS

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does virtual inheritance solve?
//     -> The diamond problem: when a class inherits from two
//        classes that share a common base, virtual inheritance
//        ensures only ONE copy of that common base exists, instead
//        of one copy per inheritance path.

// Q2: In this code, why is mike.x unambiguous but mike.power is not?
//     -> Entity (containing x,y) is virtually inherited by BOTH
//        Goblin and Warrior, so there's only ONE shared copy.
//        'power' is declared SEPARATELY in each class (not from a
//        shared virtual base), so two independent copies still exist.

// Q3: Does the order of "public Goblin, public Warrior" affect
//     which Entity values mike.x/mike.y show?
//     -> NO. There's only ONE shared Entity (default-constructed
//        once), so its values come from Entity's own initializers,
//        regardless of base-class listing order.

// Q4: What does inheritance order actually affect in this scenario?
//     -> The CONSTRUCTION ORDER of Goblin's and Warrior's own
//        (non-shared) members - here, the order their respective
//        'power' members get initialized, not the shared Entity.

// Q5: In what order is the virtual base (Entity) constructed
//     relative to Goblin and Warrior?
//     -> ALWAYS first - C++ guarantees virtual base classes are
//        constructed BEFORE any non-virtual bases or derived class
//        members, regardless of inheritance list order.

// Q6: How do you access Goblin's power specifically when ambiguous?
//     -> Use the scope resolution operator: mike.Goblin::power
//        This explicitly disambiguates which class's member to use.

// Q7: Without virtual inheritance, are mike.Warrior::x and
//     mike.Goblin::x the same memory location?
//     -> NO. Without virtual, each path creates its OWN independent
//        Entity subobject - different memory addresses, even though
//        both happen to hold the same default value (4).

// Q8: Who is responsible for constructing a virtual base class?
//     -> The MOST-DERIVED class (Player here), even if Player
//        doesn't explicitly mention Entity in its own initializer
//        list - the compiler inserts the call automatically.
// ```

// **Correcting your specific hypothesis - the key insight:**

// ```cpp
// struct Player : public Goblin, public Warrior { };
// // mike.x == 4 regardless of order, because:

// // WITHOUT virtual: TWO separate Entity copies → mike.x AMBIGUOUS (compiler error)
// // WITH virtual:    ONE shared Entity copy     → mike.x = 4 (Entity's own default)
// //                  order does NOT pick "whose x wins" - there's only one x!
// ```

// **What order DOES change:**

// ```cpp
// struct Player : public Goblin, public Warrior { };
// // construction: Entity (always first) → Goblin's power → Warrior's power

// struct Player : public Warrior, public Goblin { };
// // construction: Entity (always first) → Warrior's power → Goblin's power
// //               ^^^ this part is UNCHANGED - virtual base always goes first
// ```

// > **The one-sentence fix to your mental model**: virtual inheritance doesn't make one derived class's copy of the base "win" — it makes there be **only one copy in the first place**, constructed once by the most-derived class, completely independent of inheritance list order.