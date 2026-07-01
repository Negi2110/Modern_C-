// ============================================================
// DELEGATING CONSTRUCTORS, DRY PRINCIPLE - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <string>

// ============================================================
// SECTION 1: THE PROBLEM - REPETITION ACROSS CONSTRUCTORS
// ============================================================

// YOUR CODE shows the PROBLEM first (using init() as workaround):
//
// Entity()          -> : name{}, x{0}, y{0}, collection{nullptr} -> init()
// Entity(string)    -> : name{}, x{0}, y{0}, collection{nullptr} -> init()
//
// PROBLEMS with the init() approach:
//
// PROBLEM 1: REPETITION (violates DRY)
// -> name{}, x{0}, y{0}, collection{nullptr} written TWICE
// -> if you add a new member, must update ALL constructors
// -> easy to miss one -> bugs
//
// PROBLEM 2: init() IS PUBLIC (your comment flags this exactly!)
// -> users of your class can call e.init() themselves
// -> init() re-runs the algo and RESETS the name
// -> in main(): e.init() RESETS e.name to "00" after it was "aman"
// -> accidental misuse of implementation detail
//
// PROBLEM 3: DOUBLE WORK IN Entity(string):
// Entity(std::string _name) : name{_name}, x{0}, y{0}, collection{nullptr}
// {
//     init();           // init() sets name = "00"  <- OVERWRITES _name!
//     name = _name;     // then you set it back to _name
// }
// -> name is set THREE times: init list, init(), assignment
// -> confusing, error-prone

// ============================================================
// SECTION 2: THE SOLUTION - DELEGATING CONSTRUCTORS
// ============================================================

// DELEGATING CONSTRUCTOR = one constructor calling another constructor
// of the SAME class to avoid repeating initialization logic
//
// SYNTAX:
// Entity(string _name) : Entity()    // delegates to Entity()
// //                      ^^^^^^^^^
// //                      "run Entity() FIRST, then I add my extra stuff"
// {
//     name = _name;    // only the EXTRA work specific to THIS constructor
// }
//
// HOW IT WORKS:
// Entity e{"aman"};
// STEP 1: Entity() runs first (delegated to)
//         -> name{}, x{0}, y{0}, collection{nullptr} initialized
//         -> init() called (safely, once, from the "master" constructor)
// STEP 2: Entity(string) body runs
//         -> name = "aman"   (overrides what init() set)
//
// RESULT: init() only written and called ONCE
//         shared logic lives in ONE place
//         adding a new member = update ONE constructor only

// ============================================================
// SECTION 3: CORRECTED CODE WITH DELEGATING CONSTRUCTOR
// ============================================================

struct Entity
{
    // "MASTER" constructor - all shared initialization lives here
    Entity() : name{}, x{0}, y{0}, collection{nullptr}
    {
        init();   // called ONCE, from ONE place
    }

    // DELEGATING constructor - delegates to Entity() first
    Entity(std::string _name) : Entity()
    //                           ^^^^^^^
    //                           "call Entity() first"
    //                           CANNOT have other initializers here!
    //                           Entity(string _n) : Entity(), x{5} { }
    //                           -> COMPILER ERROR if you try to mix
    {
        // Entity() already ran: name="00", x=0, y=0, collection=nullptr
        name = _name;    // NOW override name with what caller wanted
        // result: name = "aman"
    }

    // PRIVATE init() - should be private in real code!
    // making it private prevents users from calling e.init() directly
private:
    void init()
    {
        name = std::to_string(x) + std::to_string(y);
        // computes name as "00" (x=0, y=0)
        // could also: add entity to data structure, log creation, etc.
    }

public:
    std::string name;
    int x{0};
    int y{0};
    int* collection{nullptr};
};

// ============================================================
// SECTION 4: TRACING YOUR ACTUAL CODE (with the BUG)
// ============================================================

int main()
{
    Entity e{"aman"};
    // STEP 1: Entity(string) init list runs
    //         name{"aman"} x{0}, y{0}, collection{nullptr}
    // STEP 2: init() called
    //         name = "00"  <- OVERWRITES "aman"!
    // STEP 3: name = _name
    //         name = "aman" <- sets it back
    // RESULT: e.name = "aman"

   // e.init();
    // USER CALLS init() DIRECTLY! (it's public - the problem!)
    // init() runs: name = "00"  <- RESETS name back to "00"
    // THIS IS THE BUG your comment alludes to

    std::cout << e.name       << std::endl;  // prints: 00  (NOT "aman"!)
    std::cout << e.x          << std::endl;  // prints: 0
    std::cout << e.y          << std::endl;  // prints: 0
    std::cout << e.collection << std::endl;  // prints: 0

    return 0;
}

// ============================================================
// SECTION 5: DRY PRINCIPLE
// ============================================================

// DRY = Don't Repeat Yourself
//
// VIOLATION (your original code):
// Entity()        { ... same init list ... init(); }
// Entity(string)  { ... same init list ... init(); }
// -> same init list appears TWICE
// -> init() called from BOTH constructors
// -> if you add member 'int z{0}' -> must add to BOTH init lists
//    -> easy to miss one -> bug
//
// DRY SOLUTION (delegating constructor):
// Entity()        { ... full init list ... init(); }  // master
// Entity(string)  : Entity() { name = _name; }        // delegates
// -> init list appears ONCE (in the master)
// -> init() called ONCE (in the master)
// -> add member 'int z{0}' -> update ONLY the master constructor

// ============================================================
// SECTION 6: RULES FOR DELEGATING CONSTRUCTORS
// ============================================================

// RULE 1: delegation goes in the INITIALIZER LIST position
// Entity(string) : Entity() { }    // CORRECT
// Entity(string) { Entity(); }     // WRONG - creates TEMPORARY Entity, not delegation

// RULE 2: cannot mix delegation with other initializers
// Entity(string _n) : Entity(), x{5} { }   // COMPILER ERROR
// -> once you delegate, the delegated ctor handles ALL initialization
// -> you cannot also try to initialize members yourself in the list

// RULE 3: no circular delegation
// Entity() : Entity(0) { }      // delegates to Entity(int)
// Entity(int) : Entity() { }    // delegates back to Entity()
// -> COMPILER ERROR: circular delegation detected

// RULE 4: delegation target's body runs COMPLETELY first
// then the delegating constructor's body runs
// Entity(string) : Entity() { /* Entity() body ran first */ name = _name; }

// ============================================================
// SECTION 7: MAKING init() PRIVATE - THE REAL FIX
// ============================================================

// The correct design:
// private:
//     void init() { ... }   // users CANNOT call this
//
// public:
//     Entity() { init(); }
//     Entity(string) : Entity() { name = _name; }
//
// NOW: e.init()  -> COMPILER ERROR
//                   "init() is private within this context"
// -> impossible for user to accidentally reset the object state
// -> implementation detail stays internal

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a delegating constructor?
//     -> A constructor that calls another constructor of the SAME class
//        in its initializer list. Allows sharing initialization logic
//        without repeating code. Introduced in C++11.

// Q2: What is the DRY principle and how do delegating constructors
//     help?
//     -> DRY = Don't Repeat Yourself. When multiple constructors share
//        the same initialization logic, delegating lets one "master"
//        constructor hold all shared logic, and others delegate to it,
//        eliminating repetition.

// Q3: Why is using a public init() function considered bad design?
//     -> It exposes internal initialization logic to users of the class.
//        Users can accidentally (or intentionally) call it to reset
//        the object state, bypassing invariants. init() should be
//        private if it must exist at all, OR replaced with delegating
//        constructors entirely.

// Q4: Can you mix delegation with member initialization in the list?
//     -> NO. Entity(string) : Entity(), x{5} { } is a COMPILER ERROR.
//        Once you delegate, the delegated constructor handles all
//        member initialization. You cannot also initialize members.

// Q5: In what order do bodies execute with delegating constructors?
//     -> The DELEGATED-TO constructor's body runs FIRST,
//        then the DELEGATING constructor's body runs.
//        Entity(string) : Entity() { name=_name; }
//        -> Entity() body runs, then name=_name runs.

// Q6: What bug exists in your original code's main()?
//     -> e.init() is called publicly, which resets e.name to "00"
//        even though it was set to "aman" by the constructor.
//        Fix: make init() private.

// Q7: Can constructors delegate in a circle?
//     -> NO. Circular delegation (A delegates to B, B delegates to A)
//        is a compiler error. The chain must terminate.

// Q8: What is the preferred modern C++ alternative to an init() function?
//     -> Delegating constructors. They keep initialization logic
//        inside constructors (where it belongs), cannot be called
//        by users, and clearly express the initialization intent.
// ```

// **The core problem and fix side by side:**

// ```cpp
// // PROBLEM - DRY violation + public init() bug:
// Entity()       : name{}, x{0}, y{0} { init(); }  // repeated!
// Entity(string) : name{}, x{0}, y{0} { init(); name=_name; } // repeated!
// // user can call e.init() and reset everything -> BUG

// // SOLUTION - delegating constructor:
// Entity()       : name{}, x{0}, y{0} { init(); }  // master - one place
// Entity(string) : Entity()           { name=_name; } // delegates
// // init() made private -> users can't call it -> safe
// ```

// > **DRY rule in interviews**: if you find yourself writing the same initialization list in multiple constructors, that's the signal to create a **master constructor** and **delegate** to it. The `init()` helper function is the old C++03 workaround — delegating constructors replaced it in C++11.