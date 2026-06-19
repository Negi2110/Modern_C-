// ============================================================
// PUBLIC, PROTECTED, PRIVATE - ACCESS RULES - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: THREE ACCESS LEVELS - FROM FIRST PRINCIPLES
// ============================================================

// Think of a house:
// public    = front door  -> ANYONE can come in (outside world)
// protected = family room -> only FAMILY can use (class + children)
// private   = bedroom     -> only YOU can use   (class itself only)

class Base
{
public:
    int m_public;       // ANYONE can access
                        // outside code, derived classes, base itself

protected:
    int m_protected;    // only Base AND derived classes
                        // outside code CANNOT access

private:
    int m_private;      // ONLY Base itself
                        // derived classes CANNOT access
                        // outside code CANNOT access
};

// ============================================================
// SECTION 2: WHO CAN ACCESS WHAT - MEMBER ACCESS RULES
// ============================================================

// | Member Type  | Inside Base | Inside Derived | Outside (main) |
// |--------------|-------------|----------------|----------------|
// | public       |     YES     |      YES       |      YES       |
// | protected    |     YES     |      YES       |      NO        |
// | private      |     YES     |      NO        |      NO        |

// ============================================================
// SECTION 3: THREE TYPES OF INHERITANCE - COMPLETE RULES
// ============================================================

// RULE: inheritance type = "what is the MAXIMUM access level
//        that derived class members can have?"
//
// public Base member    + public    inheritance = public    in Derived
// public Base member    + protected inheritance = protected in Derived
// public Base member    + private   inheritance = private   in Derived
//
// protected Base member + public    inheritance = protected in Derived
// protected Base member + protected inheritance = protected in Derived
// protected Base member + private   inheritance = private   in Derived
//
// private Base member   + ANY inheritance       = NOT ACCESSIBLE in Derived
//                                                 (exists in memory but hidden)

// ============================================================
// SECTION 4: PUBLIC INHERITANCE (most common)
// ============================================================

// class Derived : public Base
//
// Base public    -> Derived public    (stays public)
// Base protected -> Derived protected (stays protected)
// Base private   -> NOT accessible   (hidden)
//
// class DerivedPublic : public Base {
//     void foo() {
//         m_public    = 1;  // YES - public stays public
//         m_protected = 2;  // YES - protected stays protected
//         m_private   = 3;  // NO  - private never accessible
//     }
// };
// DerivedPublic d;
// d.m_public    = 1;  // YES - still public from outside
// d.m_protected = 2;  // NO  - protected, outside cannot access
// d.m_private   = 3;  // NO  - private, outside cannot access
//
// USE WHEN: true IS-A relationship (Golden IS-A Dog)

// ============================================================
// SECTION 5: PROTECTED INHERITANCE
// ============================================================

// class Derived : protected Base
//
// Base public    -> Derived protected (downgraded!)
// Base protected -> Derived protected (stays protected)
// Base private   -> NOT accessible
//
// class DerivedProtected : protected Base {
//     void foo() {
//         m_public    = 1;  // YES - but now protected in Derived
//         m_protected = 2;  // YES - protected
//         m_private   = 3;  // NO  - private never accessible
//     }
// };
// DerivedProtected d;
// d.m_public    = 1;  // NO! - downgraded to protected, outside blocked
// d.m_protected = 2;  // NO  - protected
//
// YOUR COMMENT: "after protected deriving, public variables are now protected"
// EXACTLY RIGHT!
//
// USE WHEN: want derived class to USE base but hide from outside world
//           (rare, mostly theoretical)

// ============================================================
// SECTION 6: PRIVATE INHERITANCE (your code)
// ============================================================

// class Derived : private Base
//
// Base public    -> Derived private  (downgraded to most restrictive!)
// Base protected -> Derived private  (downgraded!)
// Base private   -> NOT accessible
//
// class DerivedPrivate : private Base {
//     void foo() {
//         m_public    = 1;  // YES - but now PRIVATE in Derived
//         m_protected = 2;  // YES - but now PRIVATE in Derived
//         m_private   = 3;  // NO  - private never accessible
//     }
// };
// DerivedPrivate d;
// d.m_public    = 1;  // NO! - downgraded to private, outside blocked
// d.m_protected = 2;  // NO! - downgraded to private, outside blocked
//
// USE WHEN: "implemented in terms of" (HAS-A via inheritance)
//           rare, prefer composition instead

// ============================================================
// SECTION 7: YOUR ACTUAL CODE TRACED
// ============================================================

class Derived : private Base   // private inheritance
{
public:
    Derived()
    {
        // INSIDE Derived constructor:
        m_base_public_member_variable    = 123;  // YES
        // Base's public -> now private in Derived
        // but INSIDE Derived itself -> still accessible

        m_base_protected_member_variable = 234;  // YES
        // Base's protected -> now private in Derived
        // but INSIDE Derived itself -> still accessible

        // m_base_private_member_variable = 123;  // NO - NEVER
        // Base's private -> not accessible in ANY derived class
        // regardless of inheritance type
    }
};

int main()
{
    Derived d;
    // Constructor order:
    // 1. Base() called first  -> "Base Constructor"
    // 2. Derived() called     -> "Derived Constructor"

    // d.m_base_public_member_variable = 123;
    // ERROR! private inheritance downgraded it to private
    // outside code cannot access

    // d.m_base_protected_member_variable = 23;
    // ERROR! protected -> also cannot be seen outside
    // (would also be private due to private inheritance)

    return 0;
    // Destruction order (REVERSE):
    // 1. ~Derived() called first -> "Derived Destructor"
    // 2. ~Base() called after    -> "Base Destructor"
}

// ============================================================
// SECTION 8: COMPLETE TABLE - ALL COMBINATIONS
// ============================================================

// BASE MEMBER    INHERITANCE    RESULT IN DERIVED    OUTSIDE ACCESS
// ─────────────────────────────────────────────────────────────────
// public      +  public      =  public               YES
// public      +  protected   =  protected             NO
// public      +  private     =  private               NO
// ─────────────────────────────────────────────────────────────────
// protected   +  public      =  protected             NO
// protected   +  protected   =  protected             NO
// protected   +  private     =  private               NO
// ─────────────────────────────────────────────────────────────────
// private     +  public      =  NOT ACCESSIBLE        NO
// private     +  protected   =  NOT ACCESSIBLE        NO
// private     +  private     =  NOT ACCESSIBLE        NO

// SIMPLE RULE TO REMEMBER:
// inheritance type = MAXIMUM level any base member can achieve in derived
// public    = no restriction (keep original access)
// protected = everything becomes AT MOST protected
// private   = everything becomes AT MOST private
// private base members = ALWAYS inaccessible regardless

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What are the three access specifiers in C++?
//     -> public:    accessible everywhere
//        protected: accessible inside class and derived classes only
//        private:   accessible inside the class only

// Q2: What is the difference between protected and private?
//     -> Both block outside access.
//        protected: derived classes CAN access
//        private:   derived classes CANNOT access
//        Use protected when you want derived classes to use it.
//        Use private for strict encapsulation.

// Q3: What does public inheritance mean for access levels?
//     -> Base public stays public, base protected stays protected.
//        Outside code can still access public members.
//        True IS-A relationship preserved.

// Q4: What does protected inheritance do to public members?
//     -> Downgrades base's public members to protected in derived.
//        Outside code can no longer access them.
//        Derived class can still use them internally.

// Q5: What does private inheritance do?
//     -> Everything from base becomes private in derived.
//        Neither outside code NOR further derived classes can access.
//        Used for "implemented in terms of" (prefer composition instead).

// Q6: Can a derived class access base's private members?
//     -> NO. Never. Regardless of inheritance type.
//        Private means only the class ITSELF can access.
//        Not even derived classes get access.

// Q7: What is the constructor/destructor order?
//     -> Constructor: base first, then derived.
//        Destructor: derived first, then base (reverse order).

// Q8: When would you use protected vs private inheritance?
//     -> Both are rare. Public inheritance is the standard.
//        protected: when further derivation needs base access.
//        private: "implemented in terms of" (HAS-A via inheritance).
//        In practice: prefer composition over private inheritance.
// ```

// **The complete rules in one picture:**

// ```
// BASE MEMBER:    public      protected    private
//                   │              │           │
//                   ▼              ▼           ▼
// PUBLIC inh:    public       protected   BLOCKED
// PROTECTED inh: protected    protected   BLOCKED
// PRIVATE inh:   private      private     BLOCKED

// OUTSIDE code can see:
// public    -> YES (always)
// protected -> NO  (derived class internal only)
// private   -> NO  (same class internal only)
// ```

// > **Memory trick**: inheritance type = a **ceiling**. Public inheritance = no ceiling (keep original). Protected inheritance = ceiling at protected (nothing above protected). Private inheritance = ceiling at private (everything becomes private).