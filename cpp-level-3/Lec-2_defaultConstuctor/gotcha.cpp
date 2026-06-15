// ============================================================
// CAN DESTRUCTORS BE OVERLOADED LIKE CONSTRUCTORS? - SHORT ANSWER: NO
// ============================================================

#include <iostream>
#include <string>

// ============================================================
// SECTION 1: THE DIRECT ANSWER
// ============================================================

// CONSTRUCTORS: can be overloaded (multiple versions, different params)
// Student();
// Student(std::string name);
// Student(std::string name, int age);
// -> compiler picks based on arguments YOU provide when creating object
//
// DESTRUCTORS: CANNOT be overloaded - only ONE per class, EVER
// ~Student();              // valid
// ~Student(int x);         // COMPILER ERROR
// ~Student(std::string s); // COMPILER ERROR
//
// class Student {
// public:
//     ~Student();           // OK - the one and only destructor
//     ~Student(int code);    // ERROR: destructor cannot have parameters
// };

// ============================================================
// SECTION 2: WHY CAN'T DESTRUCTORS BE OVERLOADED? (the real WHY)
// ============================================================

// Constructors are CALLED EXPLICITLY by YOU with arguments:
// Student aman("aman");     // YOU choose which constructor, YOU pass args
// Student bob;               // YOU choose this one instead
//
// Destructors are called AUTOMATICALLY by the COMPILER:
// {
//     Student aman("aman");
// }   // <- compiler inserts call to ~Student() HERE, automatically
//     //    YOU never write this call, YOU never pass arguments
//
// If overloading were allowed:
// ~Student();
// ~Student(int code);
//
// {
//     Student aman("aman");
// }   // which destructor does the compiler call here???
//     // ~Student()?  ~Student(5)?  WHERE would the argument come from?
//     // there is NO mechanism to "pass arguments" to an automatic call
//
// THIS is the fundamental reason:
// overloading needs DIFFERENT ARGUMENTS to choose between versions
// destructor calls NEVER take arguments (compiler-triggered)
// -> no way to ever select between "overloads"
// -> so the language simply DISALLOWS it entirely

// ============================================================
// SECTION 3: "BUT I WANT DIFFERENT CLEANUP LOGIC!" - THE REAL SOLUTION
// ============================================================

// You CANNOT have multiple destructors, BUT you CAN have
// different cleanup logic INSIDE the single destructor,
// based on the OBJECT'S STATE (member variables)

class Student
{
public:
    Student(std::string name, bool hasScholarship)
        : m_name(name), m_hasScholarship(hasScholarship)
    {
        std::cout << "Constructor: " << m_name << std::endl;
    }

    ~Student()   // ONLY ONE destructor - ever
    {
        // DIFFERENT BEHAVIOR based on STATE, not based on "overload"
        if (m_hasScholarship)
        {
            std::cout << "Destructor (scholarship cleanup): "
                      << m_name << std::endl;
            // e.g., log to scholarship office, release special resources
        }
        else
        {
            std::cout << "Destructor (regular cleanup): "
                      << m_name << std::endl;
        }
    }

private:
    std::string m_name;
    bool m_hasScholarship;
};

// ============================================================
// SECTION 4: VIRTUAL DESTRUCTOR - THE ONE "VARIATION" THAT EXISTS
// ============================================================

// While destructors can't be OVERLOADED, they CAN be VIRTUAL
// (this is a different concept entirely - inheritance related)

// class Base
// {
// public:
//     virtual ~Base() { std::cout << "~Base" << std::endl; }
// };
//
// class Derived : public Base
// {
// public:
//     ~Derived() { std::cout << "~Derived" << std::endl; }
// };
//
// Base* p = new Derived();
// delete p;
// // WITHOUT virtual: only ~Base() called -> ~Derived() SKIPPED!
// //                  (memory leak if Derived has heap members)
// // WITH virtual:    BOTH called, ~Derived() THEN ~Base()
// //                  (correct cleanup order, derived-first)
//
// virtual ~Base() ensures the CORRECT (most-derived) destructor
// runs first, even when deleting through a base class pointer.
// THIS is a "must-know" for any class meant to be a base class.
//
// INTERVIEW: When should a destructor be virtual?
//   -> Whenever a class is intended to be used POLYMORPHICALLY
//      (i.e., you might delete a Derived* through a Base* pointer).
//      If base class destructor is NOT virtual, deleting through
//      a base pointer only calls ~Base(), causing undefined
//      behavior / resource leaks for derived members.

// ============================================================
// SECTION 5: ONE MORE THING - DESTRUCTOR CANNOT RETURN VALUES EITHER
// ============================================================

// ~Student();           // OK
// int ~Student();       // ERROR - no return type allowed
// void ~Student();      // ERROR - not even void
//
// SAME applies to constructors:
// Student();            // OK
// int Student();        // ERROR
//
// Both constructors AND destructors have NO return type at all
// (not even implicitly void). This is unique to these special functions.

int main()
{
    {
        Student aman("Aman", true);    // has scholarship
        Student bob("Bob", false);     // no scholarship
    }   // both destroyed here, reverse order: bob first, then aman
        // each prints DIFFERENT cleanup message based on STATE

    return 0;
}

// ============================================================
// SECTION 6: SIDE-BY-SIDE SUMMARY TABLE
// ============================================================

// | Feature                  | Constructor      | Destructor          |
// |---------------------------|------------------|----------------------|
// | Can be overloaded?         | YES              | NO (only one ever)  |
// | Can take parameters?       | YES              | NO (zero params)    |
// | Return type?               | none (not void)  | none (not void)     |
// | Called explicitly w/ args? | YES (you choose) | NO (compiler calls) |
// | Can be virtual?             | NO               | YES (often should!) |
// | Number per class            | many (overloads) | exactly 1           |

// ============================================================
// SECTION 7: INTERVIEW QUESTIONS
// ============================================================

// Q1: Can destructors be overloaded?
//     -> NO. A class can have only ONE destructor, with zero
//        parameters. Compiler calls it automatically with no
//        way to "choose" between versions, so overloading is
//        meaningless and disallowed.

// Q2: Why can constructors be overloaded but not destructors?
//     -> Constructors are called EXPLICITLY by the programmer,
//        who supplies different arguments to select an overload.
//        Destructors are called IMPLICITLY by the compiler with
//        NO arguments - there's no mechanism to choose an overload.

// Q3: How do you handle different cleanup needs without
//     multiple destructors?
//     -> Use the object's member variables/state to branch
//        inside the single destructor (if/else on flags, etc).

// Q4: What is a virtual destructor and when is it needed?
//     -> A destructor marked 'virtual' in a base class, ensuring
//        the correct (most-derived) destructor runs when deleting
//        through a base class pointer. Needed for any class meant
//        to be inherited from and used polymorphically.

// Q5: Can a destructor have a return type?
//     -> NO. Like constructors, destructors have no return type
//        at all - not even void.
// ```

// **One-line summary:**

// > Constructors are overloaded because **you** call them with different arguments. Destructors are called **automatically by the compiler with zero arguments** — there's nothing to overload on. The only "variation" destructors support is `virtual` (for correct polymorphic cleanup), which is a completely different concept from overloading.