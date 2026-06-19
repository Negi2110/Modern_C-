// ============================================================
// INHERITANCE CONSTRUCTOR CHAINING - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <string>

// ============================================================
// SECTION 1: THE PROBLEM - BASE CLASS WITH NO DEFAULT CONSTRUCTOR
// ============================================================

// If base class has a PARAMETERIZED constructor:
// class TopLevelClass {
//     TopLevelClass(std::string arg) { }  // needs an argument
//     // NO TopLevelClass() exists!       // no default constructor!
// };
//
// class EntityBase : public TopLevelClass {
//     EntityBase() { }   // COMPILER ERROR!
//                        // tries to call TopLevelClass() automatically
//                        // TopLevelClass() doesn't exist!
// };
//
// FIX: explicitly call base constructor in initializer list:
// EntityBase() : TopLevelClass("arg") { }  // tell compiler which to call

// ============================================================
// SECTION 2: THREE CLASSES, THREE LEVELS
// ============================================================

class TopLevelClass
{
public:
    TopLevelClass(std::string arg)
    // NO default constructor (no TopLevelClass())
    // MUST be called with a string argument
    {
        std::cout << "TopLevelClass constructor: " << arg << std::endl;
    }
    // no destructor written -> compiler generates empty one
};

class EntityBase : public TopLevelClass
{
public:
    // DEFAULT constructor of EntityBase:
    EntityBase() : TopLevelClass("arg")
    //              ^
    //              MUST explicitly call TopLevelClass constructor
    //              "arg" is the string we pass up to TopLevelClass
    //              if you remove this -> COMPILER ERROR
    //              (no TopLevelClass() default constructor exists)
    {
        std::cout << "EntityBase constructor called" << std::endl;
    }

    // PARAMETERIZED constructor of EntityBase:
    EntityBase(std::string name) : TopLevelClass("arg"), m_name(name)
    //                              ^                     ^
    //                              call TopLevelClass    initialize m_name
    //                              FIRST                 SECOND
    //
    // ORDER in initializer list:
    // 1. TopLevelClass("arg") called first (base always first)
    // 2. m_name(name) initialized
    // 3. constructor body runs
    {
        std::cout << "EntityBase(name) constructor called" << std::endl;
    }

    ~EntityBase()
    {
        std::cout << "EntityBase destructor called" << std::endl;
    }

private:
    std::string m_name;
};

class Monster : public EntityBase
{
public:
    Monster() : EntityBase("default")
    //           ^
    //           explicitly calling EntityBase's parameterized constructor
    //           passing "default" as the name
    //           EntityBase will then call TopLevelClass("arg")
    //           CHAIN: Monster -> EntityBase -> TopLevelClass
    {
        std::cout << "Monster constructor called" << std::endl;
    }

    ~Monster()
    {
        std::cout << "Monster destructor called" << std::endl;
    }
};

// ============================================================
// SECTION 3: COMPLETE EXECUTION TRACE
// ============================================================

int main()
{
    Monster badMonster;
    // CONSTRUCTION CHAIN (always base first, top to bottom):
    //
    // Monster() called
    //   -> calls EntityBase("default") first (initializer list)
    //      -> calls TopLevelClass("arg") first (initializer list)
    //         -> TopLevelClass body runs
    //            prints: "TopLevelClass constructor: arg"
    //      <- TopLevelClass done
    //      -> m_name("default") initialized
    //      -> EntityBase(name) body runs
    //         prints: "EntityBase(name) constructor called"
    //   <- EntityBase done
    //   -> Monster body runs
    //      prints: "Monster constructor called"
    // <- Monster done, badMonster fully constructed
    //
    // DESTRUCTION CHAIN (always reverse, bottom to top):
    //
    // badMonster goes out of scope:
    // ~Monster() runs first
    //    prints: "Monster destructor called"
    // ~EntityBase() runs next
    //    prints: "EntityBase destructor called"
    // ~TopLevelClass() runs last (compiler generated, empty)
    //
    // OUTPUT:
    // TopLevelClass constructor: arg
    // EntityBase(name) constructor called
    // Monster constructor called
    // Monster destructor called
    // EntityBase destructor called

    return 0;
}

// ============================================================
// SECTION 4: CONSTRUCTOR CHAINING VISUAL
// ============================================================

// Monster badMonster;
//
// CONSTRUCTION (top to bottom):
//
// TopLevelClass ──────────────────────┐
//    constructor("arg") runs FIRST    │ level 1 (grandparent)
// ──────────────────────────────────  │
// EntityBase ─────────────────────────┤
//    constructor("default") runs 2nd  │ level 2 (parent)
//    m_name = "default"               │
// ──────────────────────────────────  │
// Monster ────────────────────────────┤
//    constructor() runs LAST          │ level 3 (child)
// ────────────────────────────────────┘
//
// DESTRUCTION (bottom to top - REVERSE):
//
// Monster ────────────────────────────┐
//    destructor runs FIRST            │ level 3
// ──────────────────────────────────  │
// EntityBase ─────────────────────────┤
//    destructor runs 2nd              │ level 2
// ──────────────────────────────────  │
// TopLevelClass ──────────────────────┤
//    destructor runs LAST             │ level 1
// ────────────────────────────────────┘

// ============================================================
// SECTION 5: WHAT HAPPENS WITHOUT EXPLICIT CHAIN CALL
// ============================================================

// class EntityBase : public TopLevelClass {
//     EntityBase() {   // NO : TopLevelClass(...) in initializer list
//         // compiler tries: TopLevelClass() automatically
//         // TopLevelClass() does NOT exist!
//         // COMPILER ERROR:
//         // "no matching function for call to TopLevelClass::TopLevelClass()"
//     }
// };
//
// RULE:
// If base class has ONLY parameterized constructors (no default):
// -> derived class MUST explicitly call one in its initializer list
// -> compiler cannot "guess" what argument to pass
//
// If base class HAS a default constructor:
// -> compiler calls it automatically if you don't specify
// -> no need to write it in initializer list (optional)

// ============================================================
// SECTION 6: PASSING ARGUMENTS UP THE CHAIN
// ============================================================

// What if you want Monster to pass its OWN argument up?
//
// class Monster : public EntityBase {
// public:
//     Monster(std::string monsterName) : EntityBase(monsterName)
//     //       ^                                    ^
//     //       Monster receives name               passes it to EntityBase
//     //                                           EntityBase passes "arg" to TopLevel
//     { }
// };
//
// Monster badMonster("Goblin");
// -> TopLevelClass("arg") called
// -> EntityBase("Goblin") called, m_name = "Goblin"
// -> Monster("Goblin") body runs

// ============================================================
// SECTION 7: INTERVIEW QUESTIONS
// ============================================================

// Q1: What happens if base class has no default constructor?
//     -> Derived class MUST explicitly call a base constructor
//        in its initializer list. Compiler cannot auto-call one
//        that doesn't exist. Failure = compiler error.

// Q2: In what order do constructors run in inheritance?
//     -> Top-most base first, then down to most-derived last.
//        "Most senior family member first."
//        TopLevelClass -> EntityBase -> Monster

// Q3: In what order do destructors run?
//     -> Exact reverse of constructors.
//        Most-derived first, then up to top-most base last.
//        Monster -> EntityBase -> TopLevelClass

// Q4: How do you call a specific base constructor from derived?
//     -> In the derived constructor's initializer list:
//        Derived() : Base(argument) { }
//        Must be first in initializer list (before members).

// Q5: Can you skip a level in the chain?
//     -> NO. Each class can only call its DIRECT parent's constructor.
//        Monster calls EntityBase, EntityBase calls TopLevelClass.
//        Monster cannot directly call TopLevelClass.

// Q6: What if you don't call base constructor in initializer list?
//     -> Compiler tries to call base's DEFAULT constructor automatically.
//        If no default constructor exists -> compiler error.
//        If default constructor exists -> it's called silently.

// Q7: Can derived class constructor initialize base class members?
//     -> Not directly in the initializer list.
//        Must go through base class constructor.
//        Monster cannot do : m_name("abc") directly.
//        Must pass through EntityBase constructor.

// Q8: What is constructor chaining?
//     -> Each derived class constructor calls its base class constructor,
//        creating a chain from most-derived up to top-most base.
//        Ensures every level is properly initialized before
//        derived class code runs.
// ```

// **The chain in one picture:**

// ```
// Monster() called
//     │
//     ├──► calls EntityBase("default")   [initializer list]
//     │         │
//     │         ├──► calls TopLevelClass("arg")  [initializer list]
//     │         │         │
//     │         │         └──► TopLevelClass body runs  ← FIRST
//     │         │
//     │         └──► EntityBase body runs  ← SECOND
//     │
//     └──► Monster body runs  ← THIRD (LAST)

// DESTRUCTION runs in reverse:
// Monster destructor    ← FIRST
// EntityBase destructor ← SECOND
// TopLevelClass destructor ← THIRD (LAST)
// ```

// > **The golden rule**: construction goes **top to bottom** (grandparent → parent → child). Destruction goes **bottom to top** (child → parent → grandparent). This ensures every level is ready before the next one builds on it.