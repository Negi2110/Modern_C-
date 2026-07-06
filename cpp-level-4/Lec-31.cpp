// ============================================================
// 'this' POINTER - COMPLETE NOTES - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS 'this'?
// ============================================================

// 'this' = a hidden pointer passed to EVERY non-static member function
//          automatically by the compiler
//          points to the CURRENT OBJECT the function is called on
//
// TYPE of 'this':
// -> inside non-const member function: Person* const this
//                                      (const pointer, can't reassign this)
// -> inside const member function:     const Person* const this
//                                      (const pointer to const Person)
//
// 'this' is a POINTER, NOT a reference (Final Jeopardy answer!)
// -> because 'this' existed before references were added to C++
// -> Bjarne Stroustrup chose 'this' to follow Simula usage
//    (as you saw in the Mike Shah Jeopardy slide)

// ============================================================
// SECTION 2: THE MAIN USE - RESOLVING NAME CONFLICTS
// ============================================================

class Person
{
public:
    Person(int age)
    {
        // age = age;
        // PROBLEM: both 'age' on left and right refer to the PARAMETER
        // compiler sees: parameter age = parameter age (assigns to itself!)
        // this->age (the MEMBER) stays uninitialized = GARBAGE (32767 etc.)
        // YOUR COMMENT IS EXACTLY RIGHT: "this will assign garbage value"

        this->age = age;
        // this->age = the MEMBER variable (left side)
        // age       = the PARAMETER       (right side)
        // NOW clearly different: member = parameter value
        // CORRECT assignment!

        // ALTERNATIVE FIXES (all equivalent):
        // 1. m_age = age;     // m_ prefix convention avoids conflict
        // 2. age_ = age;      // _ suffix convention
        // 3. Person(int age) : age(age) {}  // initializer list
        //    -> in initializer list, age(age) means: member age = param age
        //    -> compiler correctly resolves the disambiguation
        //    -> your comment confirms: "this will also work fine"
    }

    int GetAge() const
    {
        return this->age;
        // this-> here is OPTIONAL (no name conflict)
        // return age;  would work identically
        // but this-> makes it EXPLICIT: "this is a member, not a local"
        // good for readability
    }

    Person* getThisAddress()
    {
        return this;
        // returns the ADDRESS of the current object
        // 'this' IS the pointer to current object
        // mike.getThisAddress() returns &mike
        // Rishit->getThisAddress() returns Rishit
    }

    // ~Person() {
    //    delete this;   // YOUR COMMENT: "in general we avoid this"
    // }
    // WHY avoid delete this in destructor?
    // -> destructor is ALREADY called when object is being destroyed
    // -> delete this INSIDE destructor = trying to delete while destructing
    // -> UNDEFINED BEHAVIOR - double free situation
    // -> the object is already in the process of being cleaned up
    // -> delete this is ONLY acceptable in very specific situations
    //    (ref-counted objects managing their own lifetime - very advanced)

private:
    int age;
};

// ============================================================
// SECTION 3: HOW 'this' WORKS INTERNALLY
// ============================================================

// When you call: mike.GetAge()
//
// Compiler SECRETLY transforms it to:
// Person::GetAge(&mike)   // passes address of mike as hidden parameter
//
// Inside GetAge, 'this' = &mike
// this->age = mike's age member
//
// THIS IS WHY:
// mike.getThisAddress()  returns &mike  (same address)
// Rishit->getThisAddress() returns Rishit (same address)
//
// 'this' is just a hidden first parameter to every member function
// passed automatically, you never write it explicitly in the call

// ============================================================
// SECTION 4: 'this' IN COPY ASSIGNMENT - CRITICAL USE
// ============================================================

// You've seen this already in copy assignment operator:
// Person& operator=(const Person& rhs) {
//     if (this == &rhs) {     // self-assignment check!
//         return *this;        // return THIS object
//     }
//     age = rhs.age;
//     return *this;            // enable chaining: p1 = p2 = p3
// }
//
// 'this'  = pointer to current object (Person*)
// *this   = the current object itself (Person&)
// &rhs    = address of right-hand side
// this == &rhs  means: "am I being assigned to myself?"
//
// return *this:
// -> dereferences 'this' to get the OBJECT (not the pointer)
// -> returns it by reference
// -> enables: p1 = p2 = p3; (chaining)

// ============================================================
// SECTION 5: STACK vs HEAP 'this' - YOUR TWO EXAMPLES
// ============================================================

int main()
{
    // STACK OBJECT:
    Person mike(500);
    // mike lives on the STACK
    // mike has a fixed address on the stack frame of main()

    std::cout << "Address:" << mike.getThisAddress() << std::endl;
    // mike.getThisAddress() -> returns this -> returns &mike
    // prints some stack address like: 0x61ff0c

    std::cout << "Address:" << &mike << std::endl;
    // &mike = address of mike on stack
    // SAME address as getThisAddress() returned!
    // both print: 0x61ff0c

    // HEAP OBJECT:
    Person* Rishit = new Person(500);
    // Rishit is a POINTER on the stack
    // the actual Person object lives on the HEAP
    // Rishit stores the HEAP address of the Person object

    std::cout << "Address:" << Rishit->getThisAddress() << std::endl;
    // Rishit->getThisAddress() -> calls getThisAddress on heap Person
    // 'this' inside = the heap address (what Rishit points to)
    // prints some heap address like: 0x55a3c0

    std::cout << "Address:" << Rishit << std::endl;
    // Rishit = the heap address stored in the pointer
    // SAME as getThisAddress() returned!
    // both print: 0x55a3c0

    // KEY OBSERVATION:
    // stack addresses (mike):   0x61ff0c  (high address, on stack)
    // heap addresses (Rishit):  0x55a3c0  (lower address, on heap)
    // stack and heap are in DIFFERENT memory regions!

    delete Rishit;
    // properly frees the heap Person object
    // Rishit pointer still holds old address (dangling)
    // Rishit = nullptr;  // good practice after delete

    return 0;
}

// ============================================================
// SECTION 6: WHERE 'this' IS USEFUL - REAL CASES
// ============================================================

// USE 1: Name disambiguation (your main example)
// Person(int age) { this->age = age; }

// USE 2: Return current object for CHAINING (method chaining / builder pattern)
// class Builder {
//     Builder& setName(string n) { name = n; return *this; }
//     Builder& setAge(int a)     { age = a;  return *this; }
// };
// Builder b;
// b.setName("John").setAge(25);  // chaining works because each returns *this

// USE 3: Self-assignment check in operator=
// if (this == &rhs) return *this;

// USE 4: Pass current object to another function
// void registerWithManager() {
//     Manager::getInstance().add(this);  // register myself
// }

// USE 5: Explicitly clarify member vs local (readability)
// void foo() { this->x = 5; }  // clearly a member, not local

// ============================================================
// SECTION 7: 'this' IN const vs NON-CONST FUNCTIONS
// ============================================================

// non-const member function:
// int GetAge() { return this->age; }
// type of 'this' = Person* const
//                  ^       ^
//                  |       const pointer (can't do: this = &other)
//                  non-const data (CAN do: this->age = 5)

// const member function:
// int GetAge() const { return this->age; }
// type of 'this' = const Person* const
//                  ^^^^^  ^       ^
//                  |      |       const pointer
//                  |      can't reassign pointer
//                  const data (CANNOT do: this->age = 5)
//
// THIS is why const member functions cannot modify members:
// 'this' is const Person* -> all members accessed through it are const

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is 'this' in C++?
//     -> A hidden pointer automatically passed to every non-static
//        member function. Points to the current object the function
//        is called on. Type is T* const (or const T* const in
//        const functions).

// Q2: Why use this->age = age instead of age = age in constructor?
//     -> age = age assigns the parameter to itself (no effect).
//        this->age explicitly refers to the MEMBER variable,
//        disambiguating it from the parameter with the same name.

// Q3: Why is 'this' a pointer and not a reference?
//     -> Historical reason: 'this' was introduced before references
//        were added to C++. Bjarne Stroustrup chose it to follow
//        Simula's 'this' usage. (Final Jeopardy answer from Mike Shah!)

// Q4: What is *this?
//     -> Dereferences the 'this' pointer, giving you the actual object.
//        Used in copy assignment to return a reference to current object:
//        return *this;  enables chaining: a = b = c;

// Q5: Why should you avoid delete this in a destructor?
//     -> The destructor is already called during object destruction.
//        delete this inside destructor = double free = undefined behavior.
//        The object is already being destroyed, deleting it again corrupts
//        the heap or causes a crash.

// Q6: Does 'this' exist in static member functions?
//     -> NO. Static functions don't operate on a specific object.
//        They have no 'this' pointer. Calling 'this' in a static
//        function = compiler error.

// Q7: Are mike.getThisAddress() and &mike guaranteed to be equal?
//     -> YES. 'this' inside any member function called on mike
//        is always equal to &mike. They refer to the same memory
//        address - the address of the mike object.

// Q8: What is the type of 'this' in a const member function?
//     -> const Person* const
//        First const: data pointed to is const (can't modify members)
//        Second const: the pointer itself is const (can't reassign this)
//        This is why const member functions cannot modify any member.
// ```

// **The name conflict - the most common use:**

// ```cpp
// Person(int age) {
//     age = age;        // WRONG: param = param (member untouched = garbage!)
//     this->age = age;  // RIGHT: member = param
// }

// // Three equivalent fixes:
// this->age = age;           // explicit this
// m_age = age;               // m_ prefix avoids conflict
// Person(int age):age(age){} // initializer list (cleanest)
// ```

// **The `this` pointer type table:**

// ```
// Function type          | Type of 'this'
// -----------------------|------------------
// regular member func    | Person* const
// const member func      | const Person* const
// static member func     | (doesn't exist!)
// ```

// > **The Final Jeopardy answer** from Mike Shah's slide: `this` is a **pointer** (not a reference) because it was introduced into C++ *before references existed*, following Simula's usage of `this`. Had it been designed today, it would likely be a reference.