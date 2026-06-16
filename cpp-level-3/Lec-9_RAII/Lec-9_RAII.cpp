
// ============================================================
// RAII, EXCEPTIONS, operator[], const overload - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: YOUR QUESTION - CAN struct HAVE CONSTRUCTOR/DESTRUCTOR?
// ============================================================

// YES! Already answered last session but confirmed here:
// struct Surface {
//     Surface() { surface = allocate... }  // constructor - VALID in C++
//     ~Surface() { SDL_FreeSurface(m_surface); } // destructor - VALID
//     SDL_Surface* m_surface;
// };
//
// struct and class are IDENTICAL except default access
// BOTH can have constructors, destructors, methods
// SDL example is PERFECT use of RAII with struct!

// ============================================================
// SECTION 2: RAII - THE CORE CONCEPT HERE
// ============================================================

// RAII = Resource Acquisition Is Initialization
//
// ACQUIRE resource in CONSTRUCTOR (new, open file, connect socket)
// RELEASE resource in DESTRUCTOR  (delete, close file, disconnect)
//
// YOUR class demonstrates RAII perfectly:
// collection() { data = new int[10]; }  // ACQUIRE heap
// ~collection() { delete[] data; }      // RELEASE heap
//
// BENEFIT: resource CANNOT leak
// -> constructor runs when object born
// -> destructor ALWAYS runs when object dies
// -> even if EXCEPTION is thrown (see section 3)
// -> even if return is called early
// -> compiler GUARANTEES destructor runs
//
// SDL EXAMPLE (your commented code):
// Surface s;                    // constructor: allocates SDL surface
// // use surface...
// // exception thrown here?
// // function returns early?
// // DOESN'T MATTER:
// }                             // destructor: SDL_FreeSurface called
//                               // ALWAYS. No leak possible.

// ============================================================
// SECTION 3: EXCEPTION IN CONSTRUCTOR - DESTRUCTOR STILL CALLED?
// ============================================================

class collection
{
public:
    collection()
    {
        data = new int[10];  // ACQUIRE resource first
        try {
            throw 20;        // exception thrown!
        } catch(int e) {
            // exception CAUGHT here inside constructor
            // constructor completes normally after catch
            // object IS fully constructed
            // destructor WILL be called when object dies
        }
    }
    // KEY POINT:
    // exception is CAUGHT inside constructor
    // -> constructor finishes successfully
    // -> object is fully born
    // -> destructor called normally at scope end
    //
    // DIFFERENT SITUATION - exception NOT caught:
    // collection() {
    //     data = new int[10];
    //     throw 20;           // NOT caught inside constructor!
    // }
    // -> constructor FAILS (object never fully born)
    // -> destructor is NOT called (object was never born)
    // -> data leaks! (allocated but never deleted)
    // -> FIX: delete[] data before rethrowing OR use unique_ptr

    ~collection()
    {
        std::cout << "destructor" << std::endl;
        delete[] data;
        // called REGARDLESS of exceptions (if object was fully constructed)
        // RAII guarantee in action
    }

    // ============================================================
    // SECTION 4: operator[] - TWO VERSIONS (const and non-const)
    // ============================================================

    // VERSION 1: NON-CONST (for modifiable collections)
    int& operator[](std::size_t idx)
    {
        return data[idx];
        // returns REFERENCE to element
        // allows: myCollection[0] = 32;  (modification)
        // called on NON-CONST collection objects
    }

    // VERSION 2: CONST (for read-only collections)
    const int& operator[](std::size_t idx) const
    //                                      ^
    //                               trailing const = called on const objects
    {
        return data[idx];
        // returns CONST REFERENCE
        // allows: int x = myCollection[0]; (reading only)
        // prevents: myCollection[0] = 32;  (modification blocked)
        // called on CONST collection objects
    }

    // WHY TWO VERSIONS?
    //
    // collection myCol;              // non-const object
    // myCol[0] = 32;                 // calls NON-CONST operator[]
    //                                // returns int& -> can write
    //
    // const collection myConstCol;   // const object
    // int x = myConstCol[0];         // calls CONST operator[]
    //                                // returns const int& -> read only
    // myConstCol[0] = 32;            // COMPILER ERROR -> protected!
    //
    // WITHOUT const version:
    // const collection myConstCol;
    // myConstCol[0];   // COMPILER ERROR
    //                  // non-const operator[] cannot be called on const object
    //
    // RULE: const objects can ONLY call const member functions
    //       always provide const version for read-only access

    // WHY std::size_t for index?
    // -> same reason as .size() returns size_t
    // -> index is NEVER negative
    // -> unsigned type matches array index semantics
    // -> avoids signed/unsigned comparison warnings

private:
    int* data;
};

// ============================================================
// SECTION 5: HOW operator[] WORKS - TRANSLATION
// ============================================================

// myCollection[0] = 32;
// STEP 1: myCollection[0]
//         -> myCollection.operator[](0)
//         -> returns int& (reference to data[0])
// STEP 2: (reference) = 32
//         -> writes 32 through the reference
//         -> data[0] is now 32
//
// std::cout << myCollection[0]
// -> myCollection.operator[](0)
// -> returns int& to data[0]
// -> cout reads value through reference
// -> prints 32
//
// CHAIN: myCollection[0] is an LVALUE (reference returned)
// -> can appear on LEFT side of = (assignment)
// -> can appear on RIGHT side (reading)
// -> behaves exactly like a normal array element!

// ============================================================
// SECTION 6: DESTRUCTION ORDER TRACE
// ============================================================

int main()
{
    collection myCollection;
    // constructor: data = new int[10], throw caught, object born
    // "destructor" NOT printed yet

    collection myCollection2;
    // constructor: data = new int[10], throw caught, object born
    // "destructor" NOT printed yet

    myCollection[0] = 32;   // non-const operator[] called, data[0]=32
    myCollection[1] = 43;   // data[1]=43
    myCollection[2] = 65;   // data[2]=65

    std::cout << myCollection[0] << std::endl;
    // non-const operator[] called, returns ref to data[0]
    // prints: 32

    return 0;
    // LIFO destruction order:
    // myCollection2 destroyed FIRST (created second)
    //   -> ~collection() called -> "destructor" printed -> delete[] data
    // myCollection destroyed SECOND (created first)
    //   -> ~collection() called -> "destructor" printed -> delete[] data
}

// OUTPUT:
// 32
// destructor    <- myCollection2
// destructor    <- myCollection

// ============================================================
// SECTION 7: EXCEPTION SAFETY LEVELS - INTERVIEW ESSENTIAL
// ============================================================

// C++ defines 3 levels of exception safety:
//
// LEVEL 1: NO GUARANTEE
// -> exception may cause leaks, corruption
// -> worst case
//
// LEVEL 2: BASIC GUARANTEE
// -> no leaks (RAII handles cleanup)
// -> object in VALID but unspecified state after exception
// -> your collection class provides this level
//
// LEVEL 3: STRONG GUARANTEE
// -> if exception thrown, state is EXACTLY as before the operation
// -> "commit or rollback" semantics
// -> harder to implement
//
// LEVEL 4: NOTHROW GUARANTEE
// -> function NEVER throws (marked noexcept)
// -> destructors should always be noexcept
// -> ~collection() noexcept { delete[] data; }

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is RAII?
//     -> Resource Acquisition Is Initialization.
//        Acquire resource in constructor, release in destructor.
//        Guarantees cleanup even on exceptions or early return.
//        Foundation of leak-free C++ code.

// Q2: Is destructor called if constructor throws an exception?
//     -> IF exception is caught inside constructor -> object born
//        -> destructor called normally later.
//        IF exception escapes constructor (not caught) -> object NOT born
//        -> destructor NOT called -> must clean up before rethrowing.

// Q3: Why have two versions of operator[]?
//     -> Non-const: called on modifiable objects, returns int&
//                   allows reading and writing
//        Const:     called on const objects, returns const int&
//                   allows reading only, prevents modification
//        Without const version, const objects cannot use [] at all.

// Q4: What does operator[] return and why reference?
//     -> Returns reference (int&) to element.
//        Allows natural syntax: myCollection[0] = 32;
//        If returned by value: myCollection[0] = 32; would fail
//        (cannot assign to temporary value).

// Q5: Why use std::size_t for index parameter?
//     -> Unsigned type, cannot be negative (matches array semantics).
//        Same type returned by sizeof and .size().
//        Avoids signed/unsigned comparison warnings.

// Q6: What is the destruction order for stack objects?
//     -> Reverse of construction order (LIFO).
//        myCollection2 created second -> destroyed first.
//        myCollection created first -> destroyed second.

// Q7: Can struct have constructor and destructor?
//     -> YES. In C++, struct and class are identical except
//        default access (public vs private).
//        Both support all special member functions.

// Q8: What is the difference between the two operator[] versions?
//     -> int& operator[](size_t idx)        <- non-const, modifiable
//        const int& operator[](size_t idx) const <- const, read-only
//        Trailing 'const' on function = called on const objects.
//        Return type 'const int&' = cannot modify returned value.
// ```

// **The two `operator[]` versions - visual:**

// ```cpp
// collection myCol;              // non-const object
// myCol[0] = 32;                 // calls int& operator[]       ✅ write
// int x = myCol[0];              // calls int& operator[]       ✅ read

// const collection myConstCol;   // const object
// int x = myConstCol[0];         // calls const int& operator[] ✅ read
// myConstCol[0] = 32;            // COMPILER ERROR              ❌ write blocked
// ```

// > **RAII in one sentence**: tie your resource to an object's lifetime — constructor opens the door, destructor closes it — **compiler guarantees the door is always closed**, no matter how the room is exited.