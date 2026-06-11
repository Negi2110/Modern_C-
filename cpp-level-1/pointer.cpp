// ============================================================
// POINTERS - COMPLETE BEGINNER TO INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS A POINTER?
// ============================================================

// Pointer = a variable that stores a MEMORY ADDRESS
// That's it. Nothing magical.
//
// Regular variable:  stores a VALUE    (int x = 7   -> stores 7)
// Pointer variable:  stores an ADDRESS (int* px = &x -> stores 0x61ff14)
//
// int*  = "pointer to int"    (type = address of an int)
// float* = "pointer to float" (type = address of a float)
// char*  = "pointer to char"  (type = address of a char)
//
// SIZE of any pointer = 8 bytes on 64-bit system (always!)
// regardless of what it points to
// sizeof(int*)   = 8
// sizeof(float*) = 8
// sizeof(char*)  = 8
// Because address is always 64 bits on 64-bit system

int main()
{
    int x = 7;

    // ============================================================
    // SECTION 2: THREE SYMBOLS - & * and * - DIFFERENT MEANINGS
    // ============================================================

    // & has TWO meanings (you learned this in references session):
    // 1. Address-of operator: &x  -> gives address of x
    // 2. Reference declarator: int& ref = x -> alias

    // * has TWO meanings:
    // 1. Pointer declarator: int* px  -> px is a pointer to int
    // 2. Dereference operator: *px    -> go to address, get the value

    int* px  = &x;   // px  stores ADDRESS of x
    int* px2 = &x;   // px2 ALSO stores ADDRESS of x
                     // two different pointers, same address stored
                     // px  and px2 are different variables
                     // but both point to same x

    // ============================================================
    // SECTION 3: MEMORY VISUAL - EVERYTHING AT ONCE
    // ============================================================

    //         STACK MEMORY:
    //
    //  address:  0x61ff14      0x61ff08       0x61ff00
    //            [  x = 7  ]  [ px=0x61ff14 ] [ px2=0x61ff14 ]
    //                 ^              |                |
    //                 |______________|________________|
    //                 both px and px2 point to x
    //
    // x   lives at 0x61ff14, stores value 7
    // px  lives at 0x61ff08, stores value 0x61ff14 (address of x)
    // px2 lives at 0x61ff00, stores value 0x61ff14 (address of x)

    x = 9;   // change x's value to 9
             // px and px2 still point to x
             // *px and *px2 now give 9 (they look at x's current value)

    // ============================================================
    // SECTION 4: OUTPUT EXPLAINED LINE BY LINE
    // ============================================================

    std::cout << "x stores:" << x << std::endl;
    // prints: x stores: 9
    // x's value

    std::cout << "x addr:" << &x << std::endl;
    // prints: x addr: 0x61ff14  (some hex address)
    // address of x on stack

    std::cout << "px stores:" << px << std::endl;
    // prints: px stores: 0x61ff14
    // px holds the ADDRESS of x
    // px stores == &x  (same value!)

    std::cout << "px addr:" << &px << std::endl;
    // prints: px addr: 0x61ff08  (DIFFERENT from &x)
    // this is the address of px ITSELF on the stack
    // px is also a variable, it also has its own address
    // KEY POINT: &px != px != &x  (three different things)

    std::cout << "px dereferences:" << *px << std::endl;
    // prints: px dereferences: 9
    // *px = "go to address stored in px, get the value there"
    // px stores 0x61ff14
    // go to 0x61ff14 -> find x -> value is 9
    // *px == x  (always, as long as px points to x)

    std::cout << "px dereference to:" << *px + 4 << std::endl;
    // prints: 13  (9 + 4)
    // *px gives 9, then + 4 = 13
    // IMPORTANT: *px + 4  is NOT the same as *(px + 4)
    // *px + 4   -> dereference px (get 9), then add 4 = 13
    // *(px + 4) -> move pointer 4 ints forward, then dereference
    //              (pointer arithmetic - dangerous if out of bounds!)

    std::cout << "px2 stores:" << px2 << std::endl;
    // prints: px2 stores: 0x61ff14
    // SAME address as px - both point to x

    std::cout << "px2 dereferences:" << *px2 << std::endl;
    // prints: 9
    // same as *px because both point to x

    // ============================================================
    // SECTION 5: THREE THINGS TO KNOW ABOUT px
    // ============================================================

    // 1. What px STORES (the address it holds):
    //    std::cout << px;    -> 0x61ff14  (address of x)
    //    same as: std::cout << &x;

    // 2. What px POINTS TO (value at that address):
    //    std::cout << *px;   -> 9  (value of x)
    //    same as: std::cout << x;

    // 3. Where px LIVES (address of the pointer itself):
    //    std::cout << &px;   -> 0x61ff08  (px's own address on stack)
    //    different from px and &x

    // ============================================================
    // SECTION 6: POINTER vs REFERENCE - COMPARISON
    // ============================================================

    // | Feature              | Pointer (int*)    | Reference (int&)  |
    // |----------------------|-------------------|-------------------|
    // | Can be null          | YES (int* p=null) | NO (always valid) |
    // | Can be reassigned    | YES (p = &y)      | NO (stuck to x)   |
    // | Needs initialization | NO (dangerous!)   | YES (mandatory)   |
    // | Syntax to access     | *px (dereference) | ref (direct)      |
    // | Has own address      | YES (&px exists)  | NO (&ref == &x)   |
    // | Size                 | 8 bytes           | 0 bytes (alias)   |

    // ============================================================
    // SECTION 7: POINTER ARITHMETIC
    // ============================================================

    // px + 1  -> moves pointer forward by sizeof(int) = 4 bytes
    //            NOT by 1 byte!
    // px + 1  -> address (0x61ff14 + 4) = 0x61ff18
    //
    // This is how arrays work internally:
    // int arr[3] = {10, 20, 30};
    // int* p = arr;       // p points to arr[0]
    // *(p + 0) == arr[0]  // 10
    // *(p + 1) == arr[1]  // 20
    // *(p + 2) == arr[2]  // 30
    //
    // arr[i] is LITERALLY syntactic sugar for *(arr + i)
    // INTERVIEW: What is arr[2] equivalent to in pointer notation?
    //   -> *(arr + 2)

    // ============================================================
    // SECTION 8: NULL POINTER - IMPORTANT
    // ============================================================

    // int* p = nullptr;  // p points to nothing
    //                    // safe way to say "no address yet"
    // *p = 5;            // CRASH - dereferencing null pointer
    //                    // segmentation fault
    //
    // Always check before dereferencing:
    // if (p != nullptr) {
    //     *p = 5;        // safe
    // }
    //
    // nullptr is C++11 (prefer over NULL or 0)
    // NULL = 0 (integer), nullptr = actual null pointer type

    // ============================================================
    // SECTION 9: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: What is a pointer?
    //     -> A variable that stores a memory address.
    //        int* px = &x; px holds the address of x.

    // Q2: What is the size of a pointer?
    //     -> Always 8 bytes on 64-bit system, regardless of type.
    //        sizeof(int*) == sizeof(double*) == sizeof(char*) == 8

    // Q3: What is dereferencing?
    //     -> Using * to get the value at the address stored in pointer.
    //        *px = "go to address in px, retrieve the value there"

    // Q4: What is the difference between px, *px, and &px?
    //     -> px   = address stored in pointer (address of x)
    //        *px  = value at that address (value of x)
    //        &px  = address of the pointer itself (px's own location)

    // Q5: What happens if you dereference a null pointer?
    //     -> Undefined behavior, typically segmentation fault (crash).
    //        Always check != nullptr before dereferencing.

    // Q6: What is pointer arithmetic?
    //     -> Moving pointer by multiples of sizeof(pointed type).
    //        px + 1 moves forward by 4 bytes (for int*), not 1 byte.
    //        Foundation of how array indexing works.

    // Q7: Can two pointers point to the same variable?
    //     -> YES. px and px2 both store &x.
    //        Modifying x through *px is visible through *px2.
    //        This is aliasing - important source of bugs.

    // Q8: What is the difference between *px + 4 and *(px + 4)?
    //     -> *px + 4    : dereference px (get value), then add 4
    //        *(px + 4)  : move pointer 4 positions forward, then dereference
    //                     pointer arithmetic, potentially dangerous

    return 0;
}
// ```

// **The three things about a pointer - most common interview question:**

// ```cpp
// int x = 9;
// int* px = &x;

// px    // 0x61ff14 -> ADDRESS stored IN the pointer  (where it points TO)
// *px   // 9        -> VALUE at that address           (what it points TO)
// &px   // 0x61ff08 -> ADDRESS of the pointer ITSELF  (where pointer lives)
// ```

// > Think of a pointer like a **house address written on paper**.
// > `px` = the address written on the paper
// > `*px` = what's actually inside that house
// > `&px` = where the paper itself is kept