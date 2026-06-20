// ============================================================
// CONST CORRECTNESS - COMPLETE PICTURE - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS "CONST CORRECTNESS"?
// ============================================================

// const correctness = using 'const' EVERYWHERE it logically belongs:
// -> variables that shouldn't change
// -> function parameters that shouldn't be modified by the function
// -> member functions that shouldn't modify the object
//
// WHY bother? THREE benefits:
// 1. SELF-DOCUMENTING: reader instantly knows "this won't change"
// 2. COMPILER-ENFORCED SAFETY: accidental modification = compile error
//    (caught immediately, not as a runtime bug weeks later)
// 3. ENABLES const OBJECTS to be usable (see Section 4)

// ============================================================
// SECTION 2: const ON FUNCTION PARAMETERS
// ============================================================

void setValue(const float number)
//             ^^^^^
{
    // number = 72.0f;   // COMPILER ERROR if uncommented!
    // "assignment of read-only parameter 'number'"
}
// PURPOSE: "I promise this function will NOT modify the parameter
//           I received - I'm only going to READ it"
//
// WHY pass by const VALUE here (not even by reference)?
// -> for a float (4 bytes), pass by value is already cheap
// -> const on a BY-VALUE parameter protects against accidentally
//    reassigning the LOCAL COPY inside the function body
//    (a common source of subtle bugs - "I meant to use this
//     variable read-only but accidentally typo'd an assignment")
// -> NOTE: const on a by-value parameter has ZERO effect on the
//    CALLER - the caller's original variable was NEVER at risk
//    anyway (pass by value already makes a copy)
// -> this is purely an INTERNAL safety net for the function's
//    own implementation, not a promise visible from outside

// ============================================================
// SECTION 3: const ON MEMBER FUNCTIONS - getValue() const
// ============================================================

class UserDefinedType
{
public:
    UserDefinedType() {}
    ~UserDefinedType() {}

    UserDefinedType(const UserDefinedType& rhs)
    //               ^^^^^
    {
        // Copy constructor
        // const here: "I promise NOT to modify the object I'm copying FROM"
        // copying should never change the SOURCE object
    }

    void setValue(const int newValue)
    //             ^^^^^
    // const PARAMETER: promise not to modify the local copy of newValue
    // (same reasoning as setValue(const float) above)
    {
        m_size = newValue;
        // THIS modifies m_size - that's fine, setValue is meant to mutate!
    }

    int getValue() const
    //             ^^^^^
    // const MEMBER FUNCTION (trailing const)
    // "I promise this function does NOT modify ANY member variable
    //  of the object it's called on"
    {
        return m_size;
        // m_size = 5;   // would be a COMPILER ERROR if uncommented!
        //               // "cannot assign to non-static data member
        //               //  within const member function"
    }

private:
    int m_size;
};

// ============================================================
// SECTION 4: WHY const MEMBER FUNCTIONS MATTER - THE REAL PAYOFF
// ============================================================

// THE KEY RULE: "const objects can ONLY call const member functions"
//
// const UserDefinedType obj;
// obj.getValue();    // WORKS - getValue() is const
// obj.setValue(5);   // COMPILER ERROR! setValue() is NOT const
//                    // "passing 'const UserDefinedType' as 'this'
//                    //  argument discards qualifiers"
//
// WHY does this rule exist?
// -> obj is declared const = "this object must NEVER change"
// -> setValue() is a function that WOULD change m_size
// -> calling setValue() on a const object would VIOLATE the
//    const promise -> compiler REFUSES to even compile it
// -> getValue() is SAFE because it promises (via trailing const)
//    to never touch any member variable
//
// THIS IS WHY YOU MARK FUNCTIONS const:
// if you DON'T mark getValue() as const:
// int getValue() { return m_size; }   // missing const!
//
// const UserDefinedType obj;
// obj.getValue();   // COMPILER ERROR! even though getValue()
//                   // doesn't actually modify anything,
//                   // the COMPILER has no way to know that
//                   // without the const keyword telling it explicitly
//
// SO: const on member functions is a PROMISE that UNLOCKS the
//     ability for const objects to call them

// ============================================================
// SECTION 5: const REFERENCE PARAMETERS - THE MOST COMMON USE
// ============================================================

// You've used this pattern MANY times in previous sessions:
// void printVector(const std::vector<int>& vec)
// void passByref(const std::vector<int>& alias)
//
// const& combines TWO benefits simultaneously:
// 1. REFERENCE = no copy made (efficient, especially for large objects)
// 2. const     = promise not to modify the caller's original object
//
// THIS is the single most common const usage in real C++ code:
// "give me efficient access, but I promise to only READ, not WRITE"

// ============================================================
// SECTION 6: "WHAT" const PROTECTS - LAYERS OF PROTECTION
// ============================================================

// const can apply to FOUR different things, often combined:
//
// 1. const local variable:
//    const float myNumber = 2.5f;
//    myNumber = 3.5f;   // ERROR - cannot reassign
//
// 2. const function parameter (by value):
//    void f(const float x) { }
//    -> protects the LOCAL copy inside the function from reassignment
//
// 3. const reference parameter:
//    void f(const std::string& s) { }
//    -> protects the CALLER's original object from modification
//    -> AND avoids copying (reference)
//
// 4. const member function (trailing const):
//    int getValue() const { }
//    -> protects ALL member variables from modification
//    -> "this entire function treats the object as read-only"
//    -> required for const objects to call this function at all

// ============================================================
// SECTION 7: main() TRACE
// ============================================================

int main()
{
    UserDefinedType u;
    // u is NON-const -> can call ANY member function (const or not)

    u.setValue(100);
    // setValue is non-const, u is non-const -> allowed
    // m_size = 100

    std::cout << "u.getValue(): " << u.getValue() << std::endl;
    // getValue is const, u is non-const -> allowed
    // (non-const objects CAN call const functions - no restriction there!)
    // prints: u.getValue(): 100

    // const float myNumber = 2.5f;
    // myNumber = 3.5f;        // would be COMPILER ERROR (const variable)
    // setValue(myNumber);     // would call the FREE setValue(const float)
    //                         // (different function from UserDefinedType's
    //                         //  member setValue - this is the GLOBAL one)
    //                         // myNumber passed by value (copy), safe either way

    return 0;
}

// ============================================================
// SECTION 8: THE FULL RULE TABLE - const OBJECT vs const FUNCTION
// ============================================================

// | Object type    | Can call const function? | Can call non-const function? |
// |-----------------|---------------------------|-------------------------------|
// | non-const obj   |          YES              |             YES               |
// | const obj       |          YES              |             NO (error!)       |
//
// SIMPLE RULE: const objects are RESTRICTED to const functions only
//              non-const objects have FULL access to everything

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is const correctness?
//     -> Practice of using const wherever a variable, parameter, or
//        member function logically should not modify state. Improves
//        safety (compiler catches violations) and documents intent.

// Q2: What does a trailing const on a member function mean?
//     -> Promises the function will not modify any member variable
//        of the object. Required for the function to be callable on
//        const objects.

// Q3: Why can't const objects call non-const member functions?
//     -> Non-const functions might modify the object, which would
//        violate the const guarantee. Compiler enforces this by
//        rejecting such calls at compile time.

// Q4: What is the benefit of const on a by-value parameter?
//     -> Protects the function's LOCAL copy from accidental
//        reassignment inside the function body. Has no effect on
//        the caller (a copy was already made).

// Q5: What is the most common practical use of const in C++?
//     -> const reference parameters: const T& - combines avoiding
//        a copy (reference) with a promise not to modify the
//        caller's data (const). Used constantly for large objects.

// Q6: Can a non-const object call a const member function?
//     -> YES, always. const-ness restricts what CONST objects can
//        call, but non-const objects can call both const and
//        non-const functions freely.

// Q7: What happens if you try to modify a member variable inside
//     a const member function?
//     -> Compiler error: "cannot assign to non-static data member
//        within const member function." This is caught at compile
//        time, not runtime.

// Q8: Why is const on the copy constructor parameter important?
//     -> const UserDefinedType(const UserDefinedType& rhs)
//        Promises that copying FROM rhs will never modify rhs.
//        Copying should be a read-only operation on the source.
// ```

// **The one rule that explains everything:**

// ```cpp
// const UserDefinedType obj;
// obj.getValue();   // ✅ getValue() is const -> safe, allowed
// obj.setValue(5);  // ❌ setValue() is NOT const -> compiler refuses
// ```

// > **const correctness in one sentence**: mark every function `const` that genuinely doesn't modify the object — this isn't just style, it's what **unlocks** that function for use by `const` objects and `const&` parameters, which is everywhere in real-world C++ (especially Qt, where you'll see `const` on getters constantly).