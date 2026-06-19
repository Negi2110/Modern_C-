// ============================================================
// FRIEND FUNCTIONS AND FRIEND CLASSES - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS A FRIEND IN C++?
// ============================================================

// Normally: private members = ONLY accessible inside the class itself
// friend = "I trust YOU specifically, you can see my private stuff"
//
// TWO types of friend:
// 1. friend function -> a specific function gets access
// 2. friend class    -> an entire class gets access
//
// DECLARED INSIDE the class that is GRANTING access
// (the class doing the trusting, not the one being trusted)

// ============================================================
// SECTION 2: YOUR CODE - TWO friend RELATIONSHIPS
// ============================================================

class UserDefinedType;  // forward declaration needed

class PrivateSecurityInfo {
    friend class UserDefinedType;
    //     ^
    //     UserDefinedType CAN access PrivateSecurityInfo's private members
    //     PrivateSecurityInfo TRUSTS UserDefinedType

    int passcode;  // private - normally nobody can touch this
                   // BUT UserDefinedType CAN (because of friend above)
};

class UserDefinedType {
public:
    UserDefinedType() : m_privateMemberVariable(0) {
        m_info.passcode = 7;
        //     ^
        //     accessing PrivateSecurityInfo's PRIVATE member!
        //     WORKS because PrivateSecurityInfo declared:
        //     friend class UserDefinedType;
    }

private:
    int m_privateMemberVariable;
    PrivateSecurityInfo m_info;

    // granting access to a FREE FUNCTION:
    friend void printprivatevarofUdt(UserDefinedType udt);
    //     ^
    //     this specific function CAN access UserDefinedType's private members

    // granting access to entire class (pointless here - see section 4):
    friend class PrivateSecurityInfo;
};

// ============================================================
// SECTION 3: FRIEND FUNCTION - HOW IT WORKS
// ============================================================

void printprivatevarofUdt(UserDefinedType udt)
{
    std::cout << udt.m_privateMemberVariable << std::endl;
    //              ^
    //              PRIVATE member accessed from OUTSIDE the class!
    //              WORKS because UserDefinedType declared:
    //              friend void printprivatevarofUdt(...)
}

// WITHOUT friend:
// void printprivatevarofUdt(UserDefinedType udt) {
//     std::cout << udt.m_privateMemberVariable;  // COMPILER ERROR
//                         ^
//                         'int UserDefinedType::m_privateMemberVariable'
//                          is private within this context
// }

// ============================================================
// SECTION 4: ONE-WAY RELATIONSHIP - IMPORTANT POINT
// ============================================================

// YOUR COMMENT: "only one way relationship"
// CORRECT!
//
// PrivateSecurityInfo says: friend class UserDefinedType;
// -> UserDefinedType CAN access PrivateSecurityInfo's private members
// -> PrivateSecurityInfo CANNOT access UserDefinedType's private members
//
// VISUAL:
// PrivateSecurityInfo ----friend----> UserDefinedType
//        ^                                  |
//        |                                  |
//        |    can see my private stuff      |
//        |<---------------------------------+
//
//        CANNOT see UserDefinedType's private stuff
//        (unless UserDefinedType also declares friend)
//
// friend is NOT symmetric:
// A says "B is my friend" -> B can see A's private stuff
//                         -> A CANNOT see B's private stuff
//                            (unless B also says "A is my friend")

// ============================================================
// SECTION 5: WHY YOU SHOULD AVOID FRIEND - YOUR COMMENT
// ============================================================

// YOUR COMMENT: "avoid using them, sometimes people use them
//                for operator overloading"
//
// REASONS TO AVOID:
//
// 1. BREAKS ENCAPSULATION:
//    private means "only I control my data"
//    friend punches a hole in that protection
//    now external code can directly touch private members
//    class invariants can be broken from outside
//
// 2. TIGHT COUPLING:
//    friend class A;  <- A and B are now tightly coupled
//    change B's private members -> A might break
//    hard to maintain separately
//
// 3. HARDER TO REASON ABOUT:
//    when reading a class, you expect only its methods
//    to access its private data
//    friend breaks this mental model
//
// 4. SHOWS DESIGN PROBLEM:
//    needing friend often means your design is wrong
//    maybe the function should BE a member function instead
//    or the classes should be reorganized
//
// WHEN friend IS ACCEPTABLE:
// -> operator<< overloading (most common legitimate use)
// -> unit testing frameworks (test class needs internal access)
// -> tightly related classes (iterator + container)

// ============================================================
// SECTION 6: FRIEND FOR operator<< - THE LEGITIMATE USE CASE
// ============================================================

// class Vector3f {
// public:
//     float x, y, z;
//
//     // friend gives operator<< access to private members
//     friend std::ostream& operator<<(std::ostream& os,
//                                     const Vector3f& v);
// private:
//     float m_secret;  // some private member
// };
//
// std::ostream& operator<<(std::ostream& os, const Vector3f& v) {
//     os << v.m_secret;  // CAN access private member (friend!)
//     return os;
// }
//
// WHY friend here specifically?
// -> operator<< MUST be free function (left side is ostream)
// -> free function cannot access private members normally
// -> friend is the ONLY clean way to give it access
// -> this is considered acceptable use of friend

// ============================================================
// SECTION 7: ALTERNATIVES TO FRIEND (BETTER DESIGN)
// ============================================================

// INSTEAD OF friend function:
// void printprivatevarofUdt(UserDefinedType udt) { ... }  // friend
//
// BETTER: make it a PUBLIC member function:
// class UserDefinedType {
// public:
//     void print() {
//         std::cout << m_privateMemberVariable << std::endl;
//     }
// };
//
// INSTEAD OF friend class:
// friend class PrivateSecurityInfo;
//
// BETTER: provide public getter/setter:
// class UserDefinedType {
// public:
//     int getPrivateMember() const { return m_privateMemberVariable; }
// };
//
// RULE: if you find yourself writing friend, ask:
// "Should this function just be a member function?"
// "Should I provide a getter/setter instead?"
// "Is my class design correct?"

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a friend function in C++?
//     -> A function declared with 'friend' inside a class that
//        gets access to the class's private and protected members.
//        Declared inside the granting class, defined outside.

// Q2: What is a friend class?
//     -> An entire class declared as friend, giving ALL its methods
//        access to the granting class's private members.

// Q3: Is friend relationship symmetric?
//     -> NO. One-way only.
//        A says "B is friend" -> B sees A's private stuff.
//        A CANNOT see B's private stuff unless B also says "A is friend".

// Q4: Is friend relationship transitive?
//     -> NO. A friends B, B friends C -> C cannot see A's private.
//        Each friendship must be explicitly declared.

// Q5: Why should you avoid friend?
//     -> Breaks encapsulation (punches hole in private protection).
//        Creates tight coupling between classes.
//        Often signals a design problem.
//        Harder to maintain and reason about.

// Q6: When is friend acceptable?
//     -> operator<< and operator>> overloading (must be free function).
//        Unit testing frameworks needing internal access.
//        Tightly coupled pairs like iterator + container.

// Q7: What is the difference between friend function and member function?
//     -> Member function: IS part of the class, always has access
//        Friend function: is NOT part of class, GRANTED access
//        Member function preferred when possible (cleaner design)

// Q8: Can friend access inherited private members?
//     -> NO. Friend of base class is NOT friend of derived class.
//        Friend of derived class is NOT friend of base class.
//        Each friendship is specific to the declaring class only.

int main()
{
    UserDefinedType instance;
    // constructor runs:
    // m_privateMemberVariable = 0 (initializer list)
    // m_info.passcode = 7 (UserDefinedType is friend of PrivateSecurityInfo)

    printprivatevarofUdt(instance);
    // friend function called
    // accesses m_privateMemberVariable (private!) directly
    // prints: m_privatevariable:: 0

    return 0;
}
// ```

// **Friend in one picture:**

// ```
// NORMAL:                          FRIEND:
// ┌─────────────────┐              ┌─────────────────┐
// │   MyClass       │              │   MyClass       │
// │  ┌───────────┐  │              │  ┌───────────┐  │
// │  │ private   │  │              │  │ private   │◄─┼── friend function
// │  │ int x;    │  │              │  │ int x;    │  │   CAN touch this!
// │  └───────────┘  │              │  └───────────┘  │
// └─────────────────┘              └─────────────────┘
// outsiders: ❌ no access          friend: ✅ has access
// ```

// **The one rule:**

// > If you find yourself writing `friend`, first ask: *"Should this just be a member function?"* — 90% of the time the answer is yes, and `friend` is not needed.