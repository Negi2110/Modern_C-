// ============================================================
// COPY CONSTRUCTOR, COPY ASSIGNMENT, DEEP vs SHALLOW - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <string>

// ============================================================
// SECTION 1: THE BIG 4 - COMPILER AUTO-GENERATES THESE
// ============================================================

// C++ compiler auto-generates 4 special functions if you don't write them:
// 1. Default Constructor      -> Student() { }
// 2. Destructor               -> ~Student() { }
// 3. Copy Constructor         -> Student(const Student& rhs) { }
// 4. Copy Assignment Operator -> Student& operator=(const Student& rhs) { }
//
// The auto-generated versions do SHALLOW COPY
// For classes with raw pointers -> shallow copy = DISASTER (double free!)
// THIS is why you write your own -> DEEP COPY

// ============================================================
// SECTION 2: SHALLOW vs DEEP COPY - THE CORE PROBLEM
// ============================================================

// SHALLOW COPY (what compiler gives you for free):
//
// myArray.data  -----> [0][1][4][9]...[81]  (heap)
//                           |
//                     pointer value copied
//                           |
// myArray2.data -----> [0][1][4][9]...[81]  (SAME heap block!)
//
// PROBLEM:
// ~Array() called for myArray  -> delete[] data -> heap freed
// ~Array() called for myArray2 -> delete[] data -> SAME heap freed AGAIN
// -> DOUBLE FREE -> crash / undefined behavior
//
// DEEP COPY (what you wrote manually):
//
// myArray.data  -----> [0][1][4][9]...[81]  (heap block 1)
//
// myArray2.data -----> [0][1][4][9]...[81]  (heap block 2 - NEW allocation!)
//                       SEPARATE copy of all values
//
// Each object has OWN heap memory
// Each destructor frees its OWN block
// -> no double free, no crash

// ============================================================
// SECTION 3: COPY CONSTRUCTOR vs COPY ASSIGNMENT - KEY DIFFERENCE
// ============================================================

// COPY CONSTRUCTOR:
// Array(const Array& rhs)
// -> called when object is CREATED from another object
// -> object does NOT exist yet, being constructed FOR THE FIRST TIME
//
// COPY ASSIGNMENT OPERATOR:
// Array& operator=(const Array& rhs)
// -> called when EXISTING object is assigned from another
// -> object ALREADY EXISTS (already has data, maybe heap allocation!)
// -> must handle existing resources BEFORE copying new ones
//
// HOW TO TELL WHICH IS CALLED:
// Array myArray2 = myArray;    // COPY CONSTRUCTOR (created + initialized together)
// Array myArray2(myArray);     // COPY CONSTRUCTOR (explicit version, same thing)
//
// Array myArray2;              // DEFAULT CONSTRUCTOR (myArray2 exists now)
// myArray2 = myArray;          // COPY ASSIGNMENT (myArray2 already existed!)

class Array
{
public:
    // ============================================================
    // SECTION 4: DEFAULT CONSTRUCTOR
    // ============================================================

    Array()
    {
        std::cout << "constructor" << std::endl;
        data = new int[10];          // allocate on heap
        for (int i = 0; i < 10; i++)
            data[i] = i * i;         // 0,1,4,9,16,25,36,49,64,81
    }

    // ============================================================
    // SECTION 5: DESTRUCTOR
    // ============================================================

    ~Array()
    {
        delete[] data;   // free heap - MUST match new int[10] with delete[]
        // data = nullptr;  // good practice to add after delete
    }

    // ============================================================
    // SECTION 6: COPY CONSTRUCTOR - DEEP COPY
    // ============================================================
//for case1. Array mynewArray= someexistingAraay;
    Array(const Array& rhs)
    //          ^       ^
    //        const    rhs = "right hand side" (naming convention)
    //        (cannot accidentally modify source)
    //        passed by REFERENCE (if by value -> infinite recursion!
    //        copying would call copy constructor -> copy constructor...forever)
    {
        std::cout << "copy constructor" << std::endl;
        data = new int[10];           // NEW allocation for THIS object
        for (int i = 0; i < 10; i++)
            data[i] = rhs.data[i];   // copy VALUES, not pointer
        // rhs.data is THEIR pointer, data is MY new pointer
        // two separate heap blocks, same values
    }
    // WHY const Array& and not Array&?
    // const -> promise not to modify source (rhs)
    //          Array copy = original; should not change 'original'
    // &     -> reference to avoid copy (which would cause infinite loop!)
    //          if you wrote Array(Array rhs) -> passing by value
    //          -> compiler tries to copy rhs -> calls copy constructor
    //          -> which tries to copy its arg -> infinite recursion!

    // ============================================================
    // SECTION 7: COPY ASSIGNMENT OPERATOR - DEEP COPY
    // ============================================================
//case 2. when first we do Array newArray;
//then ..newArray = someexistingArray
    Array& operator=(const Array& rhs)
    // ^
    // returns reference to THIS object (allows chaining: a = b = c)
    {
        std::cout << "copy assignment operator" << std::endl;

        // STEP 1: SELF-ASSIGNMENT CHECK
        if (&rhs == this)
        {
            return *this;
        }
        // &rhs  = address of right hand side object
        // this  = pointer to current (left hand side) object
        // if same address -> we are doing myArray = myArray (self-assign!)
        // without this check:
        // -> delete[] data   (deletes our own data!)
        // -> data[i] = rhs.data[i]  (rhs.data was just deleted! UB!)
        // self-assign check SAVES us from this disaster

        // STEP 2: FREE EXISTING RESOURCE
        if (data != nullptr)
            delete[] data;
        // THIS is the key difference from copy constructor!
        // copy constructor: object was JUST created, data pointer
        //                   is UNINITIALIZED (no existing heap to free)
        // copy assignment:  object EXISTS, data ALREADY points to heap
        //                   MUST free old heap BEFORE allocating new one
        //                   otherwise -> MEMORY LEAK

        // STEP 3: ALLOCATE NEW MEMORY AND COPY
        data = new int[10];
        for (int i = 0; i < 10; i++)
            data[i] = rhs.data[i];

        // STEP 4: RETURN THIS OBJECT (enables chaining)
        return *this;
        // *this = the current object (dereferenced 'this' pointer)
        // returning reference allows: a = b = c;
        // (c copied to b, b (modified) returned, then copied to a)
    }

    // ============================================================
    // SECTION 8: this POINTER - WHAT IS IT?
    // ============================================================

    // 'this' = hidden pointer passed to every non-static member function
    // points to the CURRENT OBJECT being operated on
    //
    // myArray2 = myArray;
    // -> operator= called on myArray2
    // -> 'this' = &myArray2  (pointer to myArray2)
    // -> rhs    = myArray    (reference to myArray)
    //
    // *this = dereference 'this' = the actual myArray2 object
    // &rhs  = address of the right-hand side object (myArray)
    //
    // self-assignment: myArray2 = myArray2
    // -> this = &myArray2, &rhs = &myArray2 -> SAME ADDRESS
    // -> if (&rhs == this) = TRUE -> caught!

    void printData()
    {
        for (int i = 0; i < 10; i++)
            std::cout << data[i] << std::endl;
    }

    void setData(int index, int value)
    {
        data[index] = value;
    }

private:
    int* data;   // raw pointer -> MUST write Big 4 manually!
};

int main()
{
    Array myArray;
    // "constructor" printed
    // myArray.data -> [0,1,4,9,16,25,36,49,64,81]

    myArray.setData(0, 10000000);
    myArray.setData(1, 77);
    myArray.setData(2, 99999);
    // myArray.data -> [10000000, 77, 99999, 9, 16, 25, 36, 49, 64, 81]

    Array myArray2;
    // "constructor" printed (myArray2 gets its OWN heap block)
    // myArray2.data -> [0,1,4,9,16,25,36,49,64,81]

    myArray2 = myArray;
    // COPY ASSIGNMENT called (myArray2 already exists!)
    // "copy assignment operator" printed
    // myArray2 frees its old heap [0,1,4,...81]
    // myArray2 allocates NEW heap block
    // values copied from myArray
    // myArray2.data -> [10000000, 77, 99999, 9, 16, 25, 36, 49, 64, 81]

    myArray.printData();   // [10000000, 77, 99999, 9, ...]
    myArray2.printData();  // [10000000, 77, 99999, 9, ...] (same values, DIFFERENT memory)

    return 0;
    // myArray2 destroyed first (LIFO): delete[] data
    // myArray destroyed second:        delete[] data
    // TWO SEPARATE deletions, NO double free!
}

// ============================================================
// SECTION 9: OPERATOR= RETURN - WHY Array& NOT void?
// ============================================================

// returning Array& enables CHAINING:
// Array a, b, c;
// a = b = c;
// -> reads right to left:
//    b = c  (copy assignment, returns b&)
//    a = b  (copy assignment on returned b&)
//
// if you returned void:
// a = b = c;  // ERROR - can't assign void to a

// ============================================================
// SECTION 10: RULE OF THREE (C++03) / RULE OF FIVE (C++11)
// ============================================================

// RULE OF THREE:
// If you need to write ANY of these three manually:
// 1. Destructor
// 2. Copy Constructor
// 3. Copy Assignment Operator
// -> You PROBABLY need to write ALL THREE
// (because needing one usually means you manage raw resource)
//
// WHY?
// -> Need custom destructor   = you have heap memory
// -> Having heap memory       = shallow copy is wrong
// -> Shallow copy is wrong    = need custom copy constructor
// -> Need custom copy ctor    = need custom copy assignment too
//
// RULE OF FIVE (C++11 adds move semantics):
// If you write any of the three above, also consider:
// 4. Move Constructor
// 5. Move Assignment Operator
//
// YOUR Array class implements 3 of 5 (missing move versions)

// ============================================================
// SECTION 11: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between shallow and deep copy?
//     -> Shallow: copies pointer value (both objects share same heap)
//                 -> double free when both destructors run!
//        Deep:    allocates NEW heap memory, copies VALUES
//                 each object has independent heap block, safe.

// Q2: When is copy constructor called vs copy assignment?
//     -> Copy constructor: object is being CREATED from another
//                          Array a2 = a1; or Array a2(a1);
//        Copy assignment:  object ALREADY EXISTS, assigned new value
//                          Array a2; a2 = a1;

// Q3: Why must copy constructor take const reference (const T&)?
//     -> Reference: prevents infinite recursion (pass by value would
//                   call copy constructor to make the copy of the copy...)
//        Const:     promises not to modify source object.

// Q4: What is the self-assignment check and why is it critical?
//     -> if (&rhs == this) return *this;
//        Without it: myArray = myArray would delete our own data
//        then try to copy from deleted memory -> undefined behavior.

// Q5: What is the Rule of Three?
//     -> If you need custom destructor, copy constructor, OR copy
//        assignment -> you need all three. Managing raw resources
//        requires all three to be correct.

// Q6: What does 'this' pointer represent?
//     -> Hidden pointer to the current object inside member functions.
//        *this = the object itself. Used in operator= to return
//        the current object and detect self-assignment.

// Q7: Why does copy assignment free existing data before copying?
//     -> The object already HAS heap memory from its own constructor.
//        Allocating new memory without freeing old = memory leak.
//        Must free old data first, then allocate fresh block.

// Q8: Why does copy assignment return Array& and not void?
//     -> Enables chaining: a = b = c;
//        Matches behavior of built-in types where = returns lvalue.
//        Returning void would break this natural C++ idiom.
// ```

// **The 3 things that make copy assignment different from copy constructor:**

// ```cpp
// Array& operator=(const Array& rhs)
// {
//     // 1. SELF-ASSIGNMENT CHECK (constructor never needs this)
//     if (&rhs == this) return *this;

//     // 2. FREE EXISTING RESOURCE (constructor has no existing resource)
//     delete[] data;

//     // 3. ALLOCATE + COPY (same as constructor from here)
//     data = new int[10];
//     for (int i = 0; i < 10; i++) data[i] = rhs.data[i];

//     // 4. RETURN *this (constructor never returns anything)
//     return *this;
// }
// ```

// > **Rule of Three in one sentence**: if your class has a raw pointer member, you MUST write destructor + copy constructor + copy assignment. Miss any one of them and you get either a memory leak, a double free, or silent data corruption.