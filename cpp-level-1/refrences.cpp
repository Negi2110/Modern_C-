// ============================================================
// REFERENCES - ALIASES, typeid, INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <typeinfo>  // for typeid().name()

// ============================================================
// SECTION 1: WHAT IS A REFERENCE?
// ============================================================

// & has TWO completely different meanings in C++:
//
// MEANING 1: Address-of operator (you already know this)
//   int x = 42;
//   &x  ->  gives ADDRESS of x  (used in expressions)
//
// MEANING 2: Reference declarator (NEW - what this code is about)
//   int& ref = x;  -> ref is an ALIAS for x
//                     & here is part of the TYPE, not an operator
//
// INTERVIEW: What are the two uses of & in C++?
//   -> Address-of operator (&x) and reference declarator (int& ref)

int main()
{
    int x = 42;

    // ============================================================
    // SECTION 2: CREATING A REFERENCE
    // ============================================================

    int& ref = x;
    // int&  = the TYPE  (reference to int)
    // ref   = the NAME
    // = x   = what it refers to (MANDATORY at declaration)
    //
    // ref is NOT a new variable
    // ref IS x - same memory, same address, same value
    // ref is just another NAME for x (alias)
    //
    // int& ref;        // ERROR - must initialize at declaration
    //                  // a reference with nothing to refer to = illegal
    //                  // this is what your comment means: "alone will generate error"
    //                  // SAFER than pointer (pointer can be null/uninitialized)
    //
    // int* y = nullptr; // VALID   - pointer can be null
    // int& ref = null;  // INVALID - reference CANNOT be null, ever

    // ref = x = 43;    // sets BOTH to 43 (they are the same variable)
    // after this: x == 43, ref == 43, &x == &ref (all same)

    // ============================================================
    // SECTION 3: OUTPUT EXPLAINED
    // ============================================================

    std::cout << "x value  :" << x   << std::endl;  // 42
    std::cout << "ref value:" << ref << std::endl;  // 42  <- same!

    std::cout << "x type   :" << typeid(x).name()   << std::endl;  // i (int)
    std::cout << "ref type :" << typeid(ref).name() << std::endl;  // i (int)
    // NOTICE: typeid(ref).name() prints "i" NOT "i&"
    // Because ref IS an int, it is not a separate "reference type"
    // typeid sees through the reference to the underlying type
    // This proves ref behaves exactly like x to the type system

    std::cout << "x address  :" << &x   << std::endl;  // 0x61ff14
    std::cout << "ref address:" << &ref << std::endl;  // 0x61ff14 <- SAME!
    // &ref gives address of x, not address of ref
    // because ref has NO address of its own
    // ref IS x - they share one memory location

    // ============================================================
    // SECTION 4: MEMORY VISUAL
    // ============================================================

    // PASS BY VALUE (copy - different address):
    // memory: [42]      [42]
    //          x       arg       <- two separate boxes
    //         0x14     0x04      <- different addresses
    //
    // REFERENCE (alias - same address):
    // memory: [42]
    //          x                 <- ONE box
    //         0x14
    //          ^
    //          |
    //         ref                <- just another label on SAME box
    //         0x14               <- same address!

    // ============================================================
    // SECTION 5: typeid - WHAT IS IT?
    // ============================================================

    // typeid(x).name() -> returns a string of the type name
    // defined in <typeinfo> header
    //
    // OUTPUT varies by compiler:
    // gcc/clang:  "i"  for int, "f" for float, "d" for double
    // MSVC:       "int", "float", "double"  (more readable)
    //
    // Common type codes (gcc):
    // i  = int
    // f  = float
    // d  = double
    // c  = char
    // b  = bool
    // PKc = const char* (pointer to const char)
    //
    // typeid(ref).name() == typeid(x).name() == "i"
    // Reference is transparent to typeid
    // It sees the underlying type, not the reference wrapper

    // ============================================================
    // SECTION 6: REFERENCE RULES - ALL RESTRICTIONS
    // ============================================================

    // RULE 1: Must be initialized at declaration
    // int& ref;          // ERROR - uninitialized reference

    // RULE 2: Cannot be null
    // int& ref = nullptr; // ERROR - references are always valid

    // RULE 3: Cannot be reassigned to refer to different variable
    // int a = 10, b = 20;
    // int& ref = a;   // ref refers to a
    // ref = b;        // does NOT make ref refer to b
    //                 // COPIES value of b into a  (a = 20 now)
    //                 // ref still refers to a!
    // This is a common interview trick question!

    // RULE 4: Reference type includes & but typeid ignores it
    // int&  is the full type
    // typeid sees "int" regardless

    // ============================================================
    // SECTION 7: int*& - POINTER REFERENCE (your commented code)
    // ============================================================

    // int* y = nullptr;   // y is a pointer to int, currently null
    // int*& ref2 = y;     // ref2 is a REFERENCE TO A POINTER
    //                     // ref2 is an alias for y
    //                     // modifying ref2 modifies y directly
    //
    // READ TYPE RIGHT TO LEFT:
    // int*&  ->  & (reference to) * (pointer to) int
    //        ->  "reference to pointer to int"
    //
    // USE CASE: when you want to modify a pointer inside a function
    // void allocate(int*& ptr) {
    //     ptr = new int(42);  // modifies the ORIGINAL pointer
    // }
    // int* p = nullptr;
    // allocate(p);     // p is now pointing to 42 on heap
    // vs
    // void allocate(int* ptr) {
    //     ptr = new int(42);  // modifies COPY of pointer
    // }                       // original p still null!

    // ============================================================
    // SECTION 8: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: What is a reference in C++?
    //     -> An alias for an existing variable. Same memory address,
    //        same value. Not a new variable, just another name.

    // Q2: What is the difference between reference and pointer?
    //     -> Reference: cannot be null, cannot be reassigned,
    //                   must initialize at declaration, cleaner syntax
    //        Pointer:   can be null, can be reassigned, can be
    //                   uninitialized, needs * to dereference

    // Q3: What does typeid(ref).name() return for int& ref?
    //     -> "i" (same as int). typeid sees through reference
    //        to the underlying type. Reference is transparent.

    // Q4: What does &ref return?
    //     -> Address of the variable ref refers to (same as &x)
    //        Reference has no address of its own.

    // Q5: Can a reference be reassigned to refer to another variable?
    //     -> NO. Once bound, always bound to same variable.
    //        ref = b; copies b's value into ref's target, does not rebind.
    //        This is a common trick question!

    // Q6: What is int*& ?
    //     -> Reference to a pointer. Alias for a pointer variable.
    //        Used when you need to modify the pointer itself inside a function.

    // Q7: Why is reference safer than pointer?
    //     -> Cannot be null (always valid)
    //        Cannot be uninitialized (must bind at declaration)
    //        Cannot accidentally point to wrong memory

    // Q8: Where is ref stored in memory?
    //     -> Nowhere separately. ref IS x. Same memory location.
    //        Compiler may implement it as a pointer internally,
    //        but from C++ perspective ref has no independent existence.

    return 0;
}


// **The two interview trick questions from this topic:**

// ```cpp
// // TRICK 1: Can reference be reassigned?
// int a = 10, b = 20;
// int& ref = a;
// ref = b;          // looks like rebinding but it's NOT
//                   // actually copies b's value into a
//                   // ref STILL refers to a (a is now 20)

// // TRICK 2: What does typeid(ref).name() print?
// int x = 42;
// int& ref = x;
// typeid(ref).name()  // prints "i" (int), NOT "i&"
//                     // reference is invisible to typeid
// ```

// > Reference = **sticky alias**. Sticks to one variable forever. Cannot be null. Cannot be uninitialized. Safer than pointer in every way — just less flexible.