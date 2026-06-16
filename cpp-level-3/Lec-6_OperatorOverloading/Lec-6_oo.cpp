// ============================================================
// OPERATOR OVERLOADING - operator+ - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS OPERATOR OVERLOADING?
// ============================================================

// C++ lets you define what OPERATORS mean for YOUR types
// Built-in types already have + - * / = etc. defined
// YOUR custom types (Vector3f) don't know what + means
// -> you TEACH it what + means by overloading
//
// WITHOUT operator overloading:
// Vector3f result = myVector + myVector2;  // COMPILER ERROR
//                            ^
//                   "no match for operator+ with Vector3f"
//
// WITH operator overloading:
// Vector3f result = myVector + myVector2;  // WORKS!
// -> compiler calls your operator+ function
// -> you define exactly what + does
//
// OPERATORS YOU CAN OVERLOAD:
// +  -  *  /  %           (arithmetic)
// =  +=  -=  *=           (assignment)
// ==  !=  <  >  <=  >=    (comparison)
// []  ()  ->              (access)
// <<  >>                  (stream - cout/cin)
// ++  --                  (increment/decrement)
//
// OPERATORS YOU CANNOT OVERLOAD:
// ::   (scope resolution)
// .    (member access)
// ?:   (ternary)
// sizeof

// ============================================================
// SECTION 2: SYNTAX BREAKDOWN - operator+
// ============================================================

// Vector3f operator+(const Vector3f& rhs) const
// ^         ^         ^               ^     ^
// |         |         |               |     |
// return    keyword   parameter       ref   THIS function
// type      +name     type            only  doesn't modify
//                                           current object
//
// FULL READING:
// "a function named operator+
//  takes a const reference to another Vector3f (rhs = right hand side)
//  returns a new Vector3f (the result)
//  and promises not to modify the current object (trailing const)"
//
// myVector + myVector2
//    ^             ^
//   this          rhs
// (left side)  (right side)
//
// compiler translates:
// myVector + myVector2
// INTO:
// myVector.operator+(myVector2)
// -> called ON myVector (this = myVector)
// -> myVector2 passed as rhs

class Vector3f
{
public:
    Vector3f()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    // ============================================================
    // SECTION 3: operator+ IMPLEMENTATION EXPLAINED
    // ============================================================

    Vector3f operator+(const Vector3f& rhs) const
    {
        Vector3f result;       // create a NEW vector for the result
                               // x=0, y=0, z=0 (default constructor)

        result.x = x + rhs.x; // x     = THIS object's x (myVector.x = 1.0)
                               // rhs.x = right side's x (myVector2.x = 3.0)
                               // result.x = 4.0

        result.y = y + rhs.y; // 2.0 + 2.0 = 4.0
        result.z = z + rhs.z; // 3.0 + 1.0 = 4.0

        return result;         // return the new vector by VALUE
                               // caller gets their own copy
    }
    // WHY const at the end (trailing const)?
    // -> promises: this function does NOT modify myVector
    // -> myVector + myVector2 should NEVER change myVector!
    // -> addition creates a NEW result, doesn't touch the inputs
    // -> without const: myVector.x += rhs.x would be allowed (bug!)
    //
    // WHY const Vector3f& rhs?
    // -> reference: no copy of myVector2 made (efficient)
    // -> const: promises not to modify myVector2 (rhs is read-only)
    //
    // WHY return by VALUE (not reference)?
    // -> result is a LOCAL variable inside operator+
    // -> returning reference to local = DANGLING REFERENCE!
    // -> must return by value (copy out the result)
    // -> compiler often optimizes this copy away (RVO)

    float x, y, z;
};

int main()
{
    Vector3f myVector;
    myVector.x = 1.0f;
    myVector.y = 2.0f;
    myVector.z = 3.0f;

    Vector3f myVector2;
    myVector2.x = 3.0f;
    myVector2.y = 2.0f;
    myVector2.z = 1.0f;

    // ============================================================
    // SECTION 4: TWO WAYS TO USE operator+
    // ============================================================

    // WAY 1: store in new variable
    // Vector3f result = myVector + myVector2;
    // -> myVector.operator+(myVector2) called
    // -> returns new Vector3f {4.0, 4.0, 4.0}
    // -> stored in result
    // -> myVector UNCHANGED (trailing const protects it)
    // -> myVector2 UNCHANGED

    // WAY 2: assign back to existing (your code)
    myVector = myVector + myVector2;
    // STEP 1: myVector + myVector2
    //         -> operator+ called
    //         -> returns temporary Vector3f {4.0, 4.0, 4.0}
    // STEP 2: myVector = (temporary)
    //         -> copy assignment operator called
    //         -> myVector.x = 4.0, y = 4.0, z = 4.0
    //         -> temporary destroyed
    // myVector NOW = {4.0, 4.0, 4.0}

    std::cout << myVector.x << ","
              << myVector.y << ","
              << myVector.z << std::endl;
    // prints: 4,4,4

    return 0;
}

// ============================================================
// SECTION 5: operator+ vs operator+= - DIFFERENCE
// ============================================================

// operator+  -> creates NEW object, doesn't modify either input
//               a + b -> new result, a and b unchanged
//               return type: Vector3f (new object)
//
// operator+= -> modifies THIS object in place, no new object
//               a += b -> a is modified, b unchanged
//               return type: Vector3f& (reference to THIS)
//
// IMPLEMENTING operator+=:
// Vector3f& operator+=(const Vector3f& rhs)
// {
//     x += rhs.x;   // modify THIS object directly
//     y += rhs.y;
//     z += rhs.z;
//     return *this;  // return reference to modified THIS
// }
//
// BONUS: once you have operator+=, you can implement operator+
// using it (common pattern):
// Vector3f operator+(const Vector3f& rhs) const
// {
//     Vector3f result = *this;  // copy of this object
//     result += rhs;            // use operator+=
//     return result;
// }

// ============================================================
// SECTION 6: CHAINING OPERATORS
// ============================================================

// Because operator+ returns a Vector3f (not void):
// Vector3f a, b, c, d;
// Vector3f result = a + b + c + d;
//
// evaluates left to right:
// STEP 1: a + b      -> temp1 {Vector3f}
// STEP 2: temp1 + c  -> temp2 {Vector3f}
// STEP 3: temp2 + d  -> result {Vector3f}
//
// works because each + returns a Vector3f
// which can be used as input to the next +

// ============================================================
// SECTION 7: FREE FUNCTION vs MEMBER FUNCTION - TWO STYLES
// ============================================================

// MEMBER FUNCTION (your code):
// Vector3f operator+(const Vector3f& rhs) const { }
// -> called as: myVector.operator+(myVector2)
// -> left side MUST be Vector3f (it's 'this')
// -> 3.0f + myVector  FAILS (3.0f has no operator+)
//
// FREE FUNCTION (outside class):
// Vector3f operator+(const Vector3f& lhs, const Vector3f& rhs) { }
// -> called as: operator+(myVector, myVector2)
// -> both sides are explicit parameters
// -> more flexible (enables: scalar + vector)
//
// For symmetric operators like + - * :
// -> free function is often preferred (both sides treated equally)
// For asymmetric operators like = [] () :
// -> member function is required or preferred

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is operator overloading?
//     -> Defining what built-in operators (+,-,*,= etc.) mean
//        for user-defined types. Compiler calls your function
//        when it sees the operator used with your type.

// Q2: What does the trailing const mean on operator+?
//     -> Promises that this function does NOT modify the object
//        it's called on (the left-hand side).
//        Addition should never modify its inputs.

// Q3: Why does operator+ return by value and not reference?
//     -> Result is a LOCAL variable inside operator+.
//        Returning reference to local = dangling reference (UB).
//        Must return by value. Compiler optimizes copy away (RVO).

// Q4: What is the difference between operator+ and operator+=?
//     -> operator+:  creates NEW object, inputs unchanged
//                    return type: T (new object)
//        operator+=: modifies THIS in place, no new object
//                    return type: T& (reference to this)

// Q5: How does myVector + myVector2 translate internally?
//     -> myVector.operator+(myVector2)
//        Called on myVector (this = &myVector)
//        myVector2 passed as rhs parameter.

// Q6: Can you overload all operators?
//     -> Most yes. Cannot overload: :: . ?: sizeof
//        Some must be member functions: = [] () ->
//        Others can be member or free function: + - * / etc.

// Q7: What is rhs and why use this naming convention?
//     -> rhs = "right hand side"
//        In a + b, a = left hand side (this), b = right hand side (rhs)
//        Common convention in C++ operator overloading.
//        Makes code self-documenting.

// Q8: What is RVO (Return Value Optimization)?
//     -> Compiler optimization that eliminates unnecessary copies
//        when returning objects by value from functions.
//        operator+ returns a local Vector3f by value.
//        Without RVO: copy made when returning.
//        With RVO: object constructed directly in caller's memory.
//        Zero copy overhead in practice.
// ```

// **The translation the compiler does - most important to remember:**

// ```cpp
// // What YOU write:
// myVector + myVector2

// // What COMPILER sees:
// myVector.operator+(myVector2)
// //  ^this                ^rhs
// ```

// **operator+ vs operator+= side by side:**

// ```cpp
// // operator+  = addition with NEW result (don't touch inputs)
// a + b          // a unchanged, b unchanged, NEW object returned

// // operator+= = addition IN PLACE (modify left side)
// a += b         // a modified, b unchanged, NO new object
// ```

// > The trailing `const` on `operator+` is the **most common interview trick** — it means "I promise not to modify `myVector` when you do `myVector + something`." Without it, addition could secretly change your data, which would be a bug.




// ============================================================
// HOW myVector = myVector + myVector2 ACTUALLY EXECUTES
// ============================================================

// ============================================================
// STEP BY STEP - WHAT COMPILER DOES
// ============================================================

// YOU WRITE:
//myVector = myVector + myVector2;

// COMPILER READS IT AS TWO SEPARATE OPERATIONS:
// Operation 1: myVector + myVector2   (operator+)
// Operation 2: myVector = (result)    (operator=)
//
// RULE: compiler always solves RIGHT SIDE first, then assigns
// same as: int x = 3 + 4;  -> solve 3+4 first -> then x = 7

// ============================================================
// OPERATION 1: myVector + myVector2
// ============================================================

// compiler sees: Vector3f + Vector3f
// asks: "does Vector3f have operator+ defined?"
// YES -> call it
//
// translates to:
// myVector.operator+(myVector2)
//    ^                   ^
//   this                rhs
//
// INSIDE operator+:
// Vector3f operator+(const Vector3f& rhs) const
// {
//     Vector3f result;        // NEW temporary object created on stack
//                             // result.x=0, result.y=0, result.z=0
//
//     result.x = x + rhs.x;  // x     = myVector.x  = 1.0f
//                             // rhs.x = myVector2.x = 3.0f
//                             // result.x = 4.0f
//
//     result.y = y + rhs.y;  // 2.0f + 2.0f = 4.0f
//     result.z = z + rhs.z;  // 3.0f + 1.0f = 4.0f
//
//     return result;          // result {4.0, 4.0, 4.0} returned
//                             // compiler creates TEMPORARY object
//                             // in caller's space to hold this
// }
//
// AFTER OPERATION 1:
// myVector  = {1.0, 2.0, 3.0}  <- UNCHANGED (trailing const protected it)
// myVector2 = {3.0, 2.0, 1.0}  <- UNCHANGED (const& protected it)
// TEMPORARY = {4.0, 4.0, 4.0}  <- new object sitting in memory

// ============================================================
// OPERATION 2: myVector = TEMPORARY
// ============================================================

// compiler sees: Vector3f = Vector3f
// asks: "does Vector3f have operator= defined?"
// NO explicit one in your code -> compiler uses FREE version
// (Vector3f has no raw pointer, so free version is FINE here)
//
// translates to:
// myVector.operator=(TEMPORARY)
//    ^                   ^
//   this              rhs (the temporary result)
//
// INSIDE compiler-generated operator=:
// copies x, y, z from TEMPORARY into myVector:
// myVector.x = TEMPORARY.x  -> myVector.x = 4.0f
// myVector.y = TEMPORARY.y  -> myVector.y = 4.0f
// myVector.z = TEMPORARY.z  -> myVector.z = 4.0f
//
// TEMPORARY destroyed immediately after (its job is done)

// ============================================================
// FINAL STATE:
// ============================================================

// myVector  = {4.0, 4.0, 4.0}  <- updated
// myVector2 = {3.0, 2.0, 1.0}  <- untouched

// ============================================================
// VISUAL TIMELINE
// ============================================================

//  myVector + myVector2
//      |
//      | operator+ called
//      | (myVector=this, myVector2=rhs)
//      |
//      v
//  TEMPORARY {4.0, 4.0, 4.0} born
//      |
//      | operator= called
//      | (myVector=this, TEMPORARY=rhs)
//      |
//      v
//  myVector updated to {4.0, 4.0, 4.0}
//      |
//      v
//  TEMPORARY destroyed
// ```

// **In plain English:**

// ```
// myVector = myVector + myVector2
//               |
//               |  Step 1: solve this first
//               |  operator+ runs
//               |  produces TEMPORARY {4,4,4}
//               v
// myVector =  TEMPORARY
//               |
//               |  Step 2: now assign
//               |  operator= runs
//               |  copies TEMPORARY into myVector
//               v
//            done, TEMPORARY dies
// ```

// > Two operator calls happen, one after the other:
// > **operator+** produces the temporary → **operator=** copies it into `myVector`.
// > You only wrote `operator+` explicitly. `operator=` was **auto-generated** by compiler because `Vector3f` has no raw pointer (trivial members only).


// Let me make it dead simple:

// ```cpp
// // ONE rule to remember:
// // Is the object being BORN on that line?

// Vector3f result = something;   // result BORN here = COPY CONSTRUCTOR
// result = something;            // result already alive = OPERATOR=
// ```

// **Your specific line:**

// ```cpp
// Vector3f result = myVector + myVector2;
// //   ^
// //   result is being BORN here (first time we see 'result')
// //   so COPY CONSTRUCTOR is called
// ```

// **Real life analogy:**

// ```
// Copy Constructor = baby being BORN with dad's eyes
//                    (created with someone else's values)

// Operator=        = adult CHANGING their hair color
//                    (already exists, getting new values)
// ```

// **Three lines, three different calls:**

// ```cpp
// Vector3f result;                        // DEFAULT CONSTRUCTOR
//                                         // born with zeros

// Vector3f result = myVector + myVector2; // COPY CONSTRUCTOR
//                                         // born from temporary

// result = myVector + myVector2;          // OPERATOR=
//                                         // already alive, gets new values
// ```

// > **One question tells you everything:**
// > Is this the line where `result` is created for the **first time**?
// > **YES** → copy constructor
// > **NO** → operator=