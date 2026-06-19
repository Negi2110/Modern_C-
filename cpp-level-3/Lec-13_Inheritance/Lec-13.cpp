// ============================================================
// INHERITANCE - BASE, DERIVED, IS-A RELATIONSHIP - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS INHERITANCE?
// ============================================================

// Inheritance = one class GETS all the data and behavior of another
// BASE class    = parent (Dog)     - the general type
// DERIVED class = child  (Golden)  - the specific type
//
// "IS-A" relationship:
// Golden IS-A Dog    -> Golden has everything Dog has + more
// Husky  IS-A Dog    -> Husky  has everything Dog has + more
//
// REAL WORLD ANALOGY:
// Dog blueprint:    has legs, can bark, can walk
// Golden blueprint: IS-A Dog + can retrieve sticks
// Husky blueprint:  IS-A Dog + can pull sleds
//
// CODE WITHOUT inheritance (bad):
// class Dog    { float x,y; void Bark(); void Walk(); };
// class Golden { float x,y; void Bark(); void Walk(); void Retrieve(); };
// class Husky  { float x,y; void Bark(); void Walk(); void PullSled(); };
// -> x,y, Bark, Walk DUPLICATED in every class!
// -> change Walk logic -> update in ALL classes (error prone)
//
// CODE WITH inheritance (good):
// class Dog    { float x,y; void Bark(); void Walk(); };
// class Golden : public Dog { void Retrieve(); }; // gets x,y,Bark,Walk FREE
// class Husky  : public Dog { void PullSled(); }; // gets x,y,Bark,Walk FREE
// -> change Walk in Dog -> ALL derived classes updated automatically!

// ============================================================
// SECTION 2: BASE CLASS - Dog
// ============================================================

class Dog  // BASE class
{
public:
    Dog() : x(0), y(0)  // initializer list
    {
        // ALL dogs start at position 0,0
    }

    void Bark()
    {
        std::cout << "bark!" << std::endl;
        // ALL dogs can bark
        // defined ONCE, available to all derived classes
    }

    void Walk()
    {
        x += 1;
        y += 1;
        // ALL dogs can walk
        // defined ONCE, inherited by all
    }

    float x, y;   // position - ALL dogs have this
                  // public here so derived classes can access directly
};
// sizeof(Dog) = 8 bytes (two floats: x=4, y=4)

// ============================================================
// SECTION 3: DERIVED CLASS - Golden
// ============================================================

class Golden : public Dog
//             ^
//             public inheritance = public members of Dog
//                                  stay public in Golden
//                                  (most common, use this by default)
{
public:
    Golden() : m_stickRetrieved(0)
    //    ^
    //    Dog's constructor called AUTOMATICALLY first
    //    then Golden's constructor runs
    //    Dog() sets x=0, y=0
    //    Golden() sets m_stickRetrieved=0
    {
    }

    void Retriving()  // Golden-SPECIFIC behavior (Dog doesn't have this)
    {
        std::cout << "retrieving a stick\n";
        m_stickRetrieved++;
    }

    float m_stickRetrieved;   // Golden-SPECIFIC data
};
// sizeof(Golden) = 12 bytes
// = Dog's data (x=4, y=4) + Golden's data (m_stickRetrieved=4)
// Golden CONTAINS Dog's data + its own!

// ============================================================
// SECTION 4: DERIVED CLASS - Husky (EMPTY)
// ============================================================

class Husky : public Dog
{
    // EMPTY class body
    // BUT Husky still has:
    // -> x, y (from Dog)
    // -> Bark() (from Dog)
    // -> Walk() (from Dog)
    // -> Husky() constructor (auto-generated, calls Dog() automatically)
    // -> ~Husky() destructor (auto-generated)
    //
    // sizeof(Husky) = 8 bytes (same as Dog - no extra data added)
};

int main()
{
    // ============================================================
    // SECTION 5: USING BASE CLASS
    // ============================================================

    Dog dog;
    dog.Bark();    // "bark!"
    // dog has: x, y, Bark(), Walk()

    // ============================================================
    // SECTION 6: USING DERIVED CLASS
    // ============================================================

    Golden dog1;
    // Golden HAS (inherited from Dog):
    // -> x = 0, y = 0 (Dog's constructor ran first)
    // -> Bark() method
    // -> Walk() method
    // Golden ALSO HAS (its own):
    // -> m_stickRetrieved = 0
    // -> Retriving() method

    dog1.Bark();       // inherited from Dog -> "bark!"
    dog1.Walk();       // inherited from Dog -> x=1, y=1
    dog1.Retriving();  // Golden's own method -> "retrieving a stick"
                       // m_stickRetrieved = 1

    std::cout << dog1.x << "," << dog1.y << std::endl;
    // prints: 1,1  (Walk() modified Dog's x and y)

    std::cout << "sticks retrieved: " << dog1.m_stickRetrieved << std::endl;
    // prints: 1

    // ============================================================
    // SECTION 7: sizeof - MEMORY LAYOUT
    // ============================================================

    std::cout << "sizeof(Dog)   :" << sizeof(Dog)    << std::endl; // 8
    std::cout << "sizeof(Golden):" << sizeof(Golden) << std::endl; // 12

    // MEMORY LAYOUT of Golden object:
    // [  x  (4 bytes)  ]  <- from Dog
    // [  y  (4 bytes)  ]  <- from Dog
    // [  m_stickRetrieved (4 bytes) ] <- Golden's own
    // total = 12 bytes
    //
    // Golden literally CONTAINS a Dog inside it!
    // This is why: Dog* p = &dog1;  <- VALID (pointer to Dog part)

    return 0;
}

// ============================================================
// SECTION 8: THREE TYPES OF INHERITANCE
// ============================================================

// class Golden : public Dog     <- PUBLIC inheritance (most common)
// -> public members of Dog    -> public in Golden
// -> protected members of Dog -> protected in Golden
// -> private members of Dog   -> NOT accessible in Golden

// class Golden : protected Dog  <- PROTECTED inheritance (rare)
// -> public members of Dog    -> protected in Golden
// -> protected members of Dog -> protected in Golden
// -> private members of Dog   -> NOT accessible in Golden

// class Golden : private Dog    <- PRIVATE inheritance (very rare)
// -> public members of Dog    -> private in Golden
// -> protected members of Dog -> private in Golden
// -> private members of Dog   -> NOT accessible in Golden
//
// DEFAULT:
// class Golden : Dog   <- defaults to PRIVATE (class keyword)
// struct Golden : Dog  <- defaults to PUBLIC  (struct keyword)
// ALWAYS write public explicitly to be clear

// ============================================================
// SECTION 9: CONSTRUCTOR/DESTRUCTOR ORDER IN INHERITANCE
// ============================================================

// CONSTRUCTION ORDER (base first, then derived):
// Golden dog1;
// 1. Dog() called first    -> x=0, y=0
// 2. Golden() called after -> m_stickRetrieved=0
//
// DESTRUCTION ORDER (derived first, then base - REVERSE):
// dog1 goes out of scope:
// 1. ~Golden() called first
// 2. ~Dog() called after
//
// ALWAYS base constructed first, destroyed last
// ALWAYS derived constructed last, destroyed first

// ============================================================
// SECTION 10: WHAT DERIVED CLASS INHERITS
// ============================================================

// INHERITED (derived gets these):
// -> public members of base
// -> protected members of base
// -> base's constructor is called automatically (not inherited itself)
// -> base's destructor is called automatically (not inherited itself)
//
// NOT INHERITED:
// -> private members of base (exist in memory but not accessible)
// -> base's constructors themselves (must call via initializer list)
// -> base's destructor itself (called automatically)
// -> friend relationships
// -> static members (shared, not inherited per-instance)

// ============================================================
// SECTION 11: IS-A vs HAS-A
// ============================================================

// IS-A  -> use INHERITANCE
// Golden IS-A Dog -> class Golden : public Dog
//
// HAS-A -> use COMPOSITION (member variable)
// Dog HAS-A position -> class Dog { float x, y; }
// Car HAS-A Engine   -> class Car { Engine m_engine; }
//
// COMMON MISTAKE: using inheritance when composition is right
// class Dog : public Position { }  // WRONG - Dog IS-A position? No!
// class Dog { Position m_pos; }    // RIGHT  - Dog HAS-A position
//
// RULE: only use inheritance for TRUE is-a relationships
// "Does a Golden make sense as a Dog?" -> YES -> inheritance OK
// "Does a Dog make sense as a Position?" -> NO -> use composition

// ============================================================
// SECTION 12: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is inheritance?
//     -> Mechanism where a derived class acquires all public and
//        protected members of a base class.
//        Models "is-a" relationship.
//        Promotes code reuse and hierarchical design.

// Q2: What is public inheritance?
//     -> Most common type. Public members of base stay public in derived.
//        Preserves the "is-a" relationship.
//        class Golden : public Dog -> Golden IS-A Dog.

// Q3: In what order are constructors/destructors called?
//     -> Constructors: base first, then derived.
//        Destructors:  derived first, then base (reverse).

// Q4: What does a derived class inherit?
//     -> All public and protected members of base.
//        Private members exist in memory but are not accessible.
//        Constructors and destructor not inherited but called automatically.

// Q5: What is the difference between IS-A and HAS-A?
//     -> IS-A: use inheritance (Golden is-a Dog)
//        HAS-A: use composition/member variable (Dog has-a position)
//        Using inheritance for HAS-A is a design mistake.

// Q6: Why is sizeof(Golden) > sizeof(Dog)?
//     -> Golden contains ALL of Dog's data PLUS its own members.
//        Golden physically contains Dog's x,y (8 bytes) +
//        m_stickRetrieved (4 bytes) = 12 bytes total.

// Q7: Can you call Dog's methods on a Golden object?
//     -> YES. Golden inherits all public methods of Dog.
//        dog1.Bark(), dog1.Walk() both work on Golden object.

// Q8: What is the default inheritance type for class vs struct?
//     -> class:  defaults to private inheritance
//        struct: defaults to public inheritance
//        Always write 'public' explicitly for clarity.
// ```

// **Inheritance in one picture:**

// ```
// Dog (base):
// ┌─────────────────────┐
// │ float x             │
// │ float y             │
// │ void Bark()         │
// │ void Walk()         │
// └─────────────────────┘
//          ▲
//          │ inherits (IS-A)
//          │
// Golden (derived):
// ┌─────────────────────┐
// │ float x             │ ← from Dog
// │ float y             │ ← from Dog
// │ void Bark()         │ ← from Dog
// │ void Walk()         │ ← from Dog
// ├─────────────────────┤
// │ float m_stickRet    │ ← Golden's own
// │ void Retriving()    │ ← Golden's own
// └─────────────────────┘
// ```

// > **Key rule**: if you can say *"X is a Y"* naturally in English, inheritance is probably right. If you say *"X has a Y"*, use a member variable instead. `Golden is a Dog` ✅. `Dog is a Position` ❌ — Dog *has* a position.