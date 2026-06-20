// ============================================================
// MULTIPLE INHERITANCE, DIAMOND PROBLEM, =default/=delete - NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: YOUR QUESTION - = default MEANING
// ============================================================

// noncopyable() = default;
//              ^^^^^^^^^^
// MEANING: "use the COMPILER-GENERATED version of this function"
//
// WHY write this explicitly when compiler does it automatically anyway?
//
// NORMALLY:
// struct Foo { };
// Foo f;   // compiler auto-generates default constructor for you
//
// BUT once you write ANY constructor in a class
// (even a deleted one, like the copy constructor below):
//
// struct noncopyable {
//     noncopyable(const noncopyable&) = delete;  // you wrote a ctor!
// };
// -> compiler STOPS auto-generating the DEFAULT constructor
//    (the rule from way back: "writing ANY constructor suppresses
//     the auto-generated default constructor")
//
// noncopyable n;   // would now be a COMPILER ERROR without = default!
//                  // "no default constructor exists"
//
// FIX: explicitly tell compiler "still generate the normal default one"
// noncopyable() = default;
// -> compiler creates the SAME default constructor it would have
//    made automatically, just because YOU asked for it explicitly
// -> now: noncopyable n;  WORKS again

// ============================================================
// SECTION 2: YOUR QUESTION - WHY = delete ON COPY CONSTRUCTOR/ASSIGNMENT
// ============================================================

// noncopyable(const noncopyable&) = delete;
// noncopyable& operator=(const noncopyable& rhs) = delete;
//
// PURPOSE: create a class that is EXPLICITLY UNCOPYABLE
//
// struct noncopyable {
//     noncopyable() = default;
//     noncopyable(const noncopyable&) = delete;       // no copying!
//     noncopyable& operator=(const noncopyable&) = delete;  // no copy assign!
// };
//
// THIS IS A REUSABLE "BASE CLASS PATTERN":
// any class that inherits from noncopyable AUTOMATICALLY becomes
// non-copyable too! (deleted functions are inherited as deleted)
//
// struct DOG : public noncopyable { };
//
// DOG dog1;
// DOG dog2 = dog1;   // COMPILER ERROR!
// // tries to call DOG's copy constructor
// // DOG's copy constructor implicitly calls noncopyable's copy constructor
// // noncopyable's copy constructor is DELETED
// // -> DOG's copy constructor becomes IMPLICITLY DELETED too!
//
// YOUR COMMENT IN CODE CONFIRMS THIS:
// "DOG dog3 = dog2; --error copy constructor is deleted"
//
// WHY is this useful?
// -> some classes should NEVER be copied (file handles, sockets,
//    mutexes, unique resources, singleton-like objects)
// -> instead of writing =delete on EVERY such class individually,
//    you write ONE noncopyable base class and inherit from it
// -> classic, well-known C++ idiom (similar to boost::noncopyable)

// ============================================================
// SECTION 3: THE noncopyable PATTERN IN ACTION
// ============================================================

struct noncopyable
{
    noncopyable() = default;
    // explicitly restore default constructor (since we wrote other ctors below)

    noncopyable(const noncopyable&) = delete;
    // forbid copy construction entirely - compile error if attempted

    noncopyable& operator=(const noncopyable& rhs) = delete;
    // forbid copy assignment entirely - compile error if attempted

    // NOTE: move constructor/assignment are NOT deleted here
    // -> they would still work IF explicitly added
    // -> but since we didn't write them, and we DID write copy
    //    operations (deleted ones count as "written"),
    //    the compiler does NOT auto-generate move versions either
    // -> so noncopyable is ALSO non-movable by side effect
};

// ============================================================
// SECTION 4: THE DOG HIERARCHY - SIMPLE INHERITANCE FIRST
// ============================================================

struct DOG : public noncopyable
{
    virtual void bark()
    {
        std::cout << "Barking" << std::endl;
    }
};
// DOG inherits noncopyable's deleted copy operations
// DOG also inherits noncopyable's = default constructor (runs fine)

struct Golden : public DOG
{
    virtual void bark() override
    {
        std::cout << "Golden Barking" << std::endl;
    }
};

struct BorderCollie : public DOG
{
    virtual void bark() override
    {
        std::cout << "Border Collie Barking" << std::endl;
    }
};

// ============================================================
// SECTION 5: MULTIPLE INHERITANCE - THE MAIN TOPIC
// ============================================================

struct Coltriever : public Golden, public BorderCollie
//                  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//                  inherits from BOTH Golden AND BorderCollie
//                  (multiple inheritance - comma-separated base list)
{
    void bark() override
    {
        std::cout << "Coltriever Barking" << std::endl;
    }
};

// ============================================================
// SECTION 6: THE DIAMOND PROBLEM - WHY "CAUTION" IN YOUR COMMENT
// ============================================================

// YOUR COMMENT: "Caution multiple inheritance"
// HERE IS EXACTLY WHY:
//
//                    DOG
//                   /    \
//              Golden    BorderCollie
//                   \    /
//                Coltriever
//
// THIS SHAPE is called THE DIAMOND PROBLEM
//
// Coltriever inherits from BOTH Golden AND BorderCollie
// Golden inherits from DOG
// BorderCollie ALSO inherits from DOG
//
// QUESTION: how many copies of DOG does Coltriever actually contain?
//
// DEFAULT ANSWER (without virtual inheritance): TWO COPIES!
// -> one DOG-subobject comes through the Golden path
// -> ANOTHER separate DOG-subobject comes through the BorderCollie path
// -> Coltriever literally contains TWO SEPARATE DOG instances inside it!
//
// THIS CAUSES AMBIGUITY:
// Coltriever c;
// c.bark();  // works because Coltriever OVERRIDES bark() itself
//
// BUT if Coltriever did NOT override bark():
// struct Coltriever : public Golden, public BorderCollie { };
// Coltriever c;
// c.bark();   // COMPILER ERROR! "ambiguous call to member function bark"
//             // compiler doesn't know: Golden's bark() (via DOG copy #1)?
//             //                        or BorderCollie's bark() (via DOG copy #2)?
//
// THIS is exactly WHY multiple inheritance needs "Caution"

// ============================================================
// SECTION 7: HOW TO ACTUALLY FIX THE DIAMOND (not in your code,
//             but essential interview knowledge)
// ============================================================

// FIX: VIRTUAL INHERITANCE
//
// struct DOG : public noncopyable { virtual void bark() {} };
// struct Golden : virtual public DOG { };          // <- virtual here
// struct BorderCollie : virtual public DOG { };    // <- virtual here
// struct Coltriever : public Golden, public BorderCollie { };
//
// WITH virtual inheritance:
// -> Golden and BorderCollie SHARE ONE SINGLE DOG subobject
// -> Coltriever contains ONLY ONE DOG, not two
// -> ambiguity resolved automatically
//
// YOUR CODE does NOT use virtual inheritance here
// -> Coltriever DOES have two separate DOG subobjects internally
// -> but it WORKS in your code only because Coltriever explicitly
//    overrides bark() itself, sidestepping the ambiguity entirely

// ============================================================
// SECTION 8: MAIN() TRACE - POLYMORPHISM ACROSS THE HIERARCHY
// ============================================================

int main()
{
    DOG* dog          = new DOG;
    DOG* golden        = new Golden;        // DOG* can point to Golden (IS-A)
    DOG* borderCollie  = new BorderCollie;  // DOG* can point to BorderCollie (IS-A)
    Coltriever* coltriever = new Coltriever;
    // Coltriever* used directly (not DOG*) here
    // could a DOG* point to a Coltriever too? YES (through EITHER path)
    // but it would be slightly ambiguous WHICH DOG subobject it points to
    // (another reason multiple inheritance needs caution!)

    dog->bark();          // virtual dispatch -> DOG::bark() -> "Barking"
    golden->bark();       // virtual dispatch -> Golden::bark() -> "Golden Barking"
    borderCollie->bark(); // virtual dispatch -> BorderCollie::bark() -> "Border Collie Barking"
    coltriever->bark();   // direct call -> Coltriever::bark() -> "Coltriever Barking"

    dog = golden;
    // VALID - golden is DOG* already, this just reassigns the pointer
    // (no new object, no slicing, no problem - both are DOG* type)

    return 0;
    // BUG: nothing is deleted! all 4 'new' allocations LEAK
    // ALSO: DOG has NO virtual destructor -> would leak derived
    //       resources too even if delete WAS called
}

// FULL OUTPUT:
// Barking
// Golden Barking
// Border Collie Barking
// Coltriever Barking

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does = default mean?
//     -> Explicitly requests the compiler-generated version of a
//        special member function, even when the class has other
//        user-declared constructors that would otherwise suppress it.

// Q2: What does = delete mean?
//     -> Explicitly forbids a function from being used. Any attempt
//        to call it is a compile-time error. Commonly used to make
//        classes non-copyable or prevent unwanted implicit conversions.

// Q3: What is the noncopyable pattern?
//     -> A reusable base class with deleted copy constructor and
//        copy assignment operator. Any class inheriting from it
//        automatically becomes non-copyable too.

// Q4: What is the diamond problem in multiple inheritance?
//     -> When a class inherits from two classes that both derive
//        from the same base, the most-derived class ends up with
//        TWO separate copies of that base's data/functions, causing
//        ambiguity when accessing them.

// Q5: How do you fix the diamond problem?
//     -> Virtual inheritance: struct Golden : virtual public DOG { };
//        Ensures only ONE shared DOG subobject exists, no matter
//        how many paths lead to it.

// Q6: Does your code use virtual inheritance? What's the consequence?
//     -> NO. Coltriever has TWO separate DOG subobjects (via Golden
//        and via BorderCollie). It avoids ambiguity errors only
//        because it explicitly overrides bark() itself.

// Q7: Why does deleting the copy constructor in noncopyable affect DOG too?
//     -> DOG inherits from noncopyable. DOG's auto-generated copy
//        constructor would need to call noncopyable's copy constructor,
//        which is deleted - so DOG's copy constructor becomes
//        implicitly deleted as well.

// Q8: Is multiple inheritance generally recommended?
//     -> Generally discouraged for class hierarchies with state
//        (diamond problem risk). Commonly used SAFELY for pure
//        interfaces (abstract classes with only pure virtual functions)
//        since there's no shared data to duplicate.
// ```

// **The diamond shape - the entire "Caution" in one picture:**

// ```
//               DOG
//              /    \
//         Golden    BorderCollie
//              \    /
//           Coltriever

// WITHOUT virtual inheritance:
// Coltriever contains TWO DOG subobjects (ambiguous)

// WITH virtual inheritance (Golden : virtual public DOG):
// Coltriever contains ONE shared DOG subobject (safe)
// ```

// **`= default` vs `= delete` - side by side:**

// ```cpp
// Foo() = default;              // "still generate the normal version, please"
// Foo(const Foo&) = delete;     // "forbid this, even though it could exist"
// ```

// > **Multiple inheritance is safe for pure interfaces** (only pure virtual functions, no data) — that's why your `IRenderer`-style classes earlier never had this problem. It becomes dangerous specifically when the base classes **carry actual data or state**, like `DOG` does here through `noncopyable` and its virtual function table — that's the diamond problem your comment is warning about.