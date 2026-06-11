// ============================================================
// POINTER ARITHMETIC & ARRAY INTERNALS - INTERVIEW NOTES
// ============================================================

#include <iostream>

int main()
{
    int array[] = {1, 3, 5, 7, 9};
    // compiler deduces size = 5 from initializer list
    // array lives on STACK (size known at compile time)
    //
    // MEMORY LAYOUT:
    // index:    [0]  [1]  [2]  [3]  [4]
    // value:    [1]  [3]  [5]  [7]  [9]
    // address: 100  104  108  112  116   (each int = 4 bytes)
    //           ^
    //           array name points HERE (first element)

    int* px = array;
    // array NAME decays to pointer to first element
    // px = &array[0] = address 100
    // NO & needed because array already decays to pointer
    // int* px = &array[0];  // same thing, more explicit

    // ============================================================
    // SECTION 1: THREE WAYS TO ACCESS - ALL IDENTICAL
    // ============================================================

    // WAY 1: array subscript with pointer
    // px[i]

    // WAY 2: pointer arithmetic + dereference
    // *(px + i)

    // WAY 3: array subscript with array name
    // array[i]

    // ALL THREE ARE 100% IDENTICAL - compiler generates same code
    // array[i]  ==  *(array + i)  ==  *(px + i)  ==  px[i]
    // This is NOT convenience syntax - it IS the definition of []

    // ============================================================
    // SECTION 2: POINTER ARITHMETIC EXPLAINED
    // ============================================================

    std::cout << "px is:  " << px << std::endl;
    // prints base address: 0x...100  (address of array[0])

    std::cout << *(px + 0) << std::endl;  // 1
    // px + 0 = address 100 + (0 * sizeof(int)) = 100 + 0  = 100
    // *(100) = value at address 100 = 1

    std::cout << *(px + 1) << std::endl;  // 3
    // px + 1 = address 100 + (1 * sizeof(int)) = 100 + 4  = 104
    // *(104) = value at address 104 = 3

    std::cout << *(px + 2) << std::endl;  // 5
    // px + 2 = address 100 + (2 * sizeof(int)) = 100 + 8  = 108
    // *(108) = value at address 108 = 5

    std::cout << *(px + 3) << std::endl;  // 7
    // px + 3 = address 100 + (3 * sizeof(int)) = 100 + 12 = 112
    // *(112) = value at address 112 = 7

    std::cout << *(px + 4) << std::endl;  // 9
    // px + 4 = address 100 + (4 * sizeof(int)) = 100 + 16 = 116
    // *(116) = value at address 116 = 9

    // KEY INSIGHT:
    // pointer + n  does NOT add n bytes
    // pointer + n  adds n * sizeof(pointed_type) bytes
    // for int*: +1 = +4 bytes, +2 = +8 bytes etc.
    // compiler knows the type, does the math automatically
    // THIS is why type matters for pointers!

    // ============================================================
    // SECTION 3: SAME WITH ARRAY NAME DIRECTLY
    // ============================================================

    std::cout << *(array + 0) << std::endl;  // 1
    std::cout << *(array + 1) << std::endl;  // 3
    std::cout << *(array + 2) << std::endl;  // 5
    std::cout << *(array + 3) << std::endl;  // 7
    std::cout << *(array + 4) << std::endl;  // 9
    // array name == px == &array[0]
    // both give same base address
    // *(array + i) == *(px + i)  always

    // ============================================================
    // SECTION 4: SUBSCRIPT [] IS JUST SYNTACTIC SUGAR
    // ============================================================

    std::cout << array[0] << std::endl;  // 1
    std::cout << array[1] << std::endl;  // 3
    std::cout << array[2] << std::endl;  // 5
    std::cout << array[3] << std::endl;  // 7
    std::cout << array[4] << std::endl;  // 9
    //
    // array[i]  is DEFINED as  *(array + i)
    // not the other way around!
    // [] is just easier to read and write
    //
    // MIND BENDING FACT:
    // array[2] == *(array + 2) == *(2 + array) == 2[array]
    // 2[array] is actually VALID C++ (addition is commutative)
    // never write it in real code - but proves [] is just math

    // ============================================================
    // SECTION 5: COMMENTED CODE - px++ EXPLAINED
    // ============================================================

    // px++ -> moves pointer to NEXT element (adds 4 bytes for int*)
    //
    // int* px = array;         // px = 100 (points to array[0] = 1)
    // std::cout << *px;        // 1
    //
    // px++;                    // px = 104 (points to array[1])
    // std::cout << px;         // 104  (address jumped by 4 bytes)
    // std::cout << *px;        // 3    (value at new address)
    //
    // px++;                    // px = 108 (points to array[2])
    // std::cout << px;         // 108  (jumped another 4 bytes)
    // std::cout << *px;        // 5
    //
    // VISUAL of px++ moving through array:
    //
    // start:  px -> [1][3][5][7][9]
    //               ^
    // px++:   px -> [1][3][5][7][9]
    //                  ^
    // px++:   px -> [1][3][5][7][9]
    //                     ^
    //
    // WHY array++ does NOT work:
    // px++    -> VALID   (px is a pointer variable, can be changed)
    // array++ -> INVALID (array name is constant pointer, cannot move)
    //            same as changing where array starts = illegal

    // ============================================================
    // SECTION 6: sizeof PROOF - ARRAY vs POINTER
    // ============================================================

    // sizeof(array) = 20 bytes (5 ints x 4 bytes) <- knows full size
    // sizeof(px)    = 8 bytes  (just a pointer)   <- lost size info
    //
    // This is ARRAY DECAY:
    // when array is assigned to pointer, size info is LOST
    // int* px = array;
    // sizeof(px) = 8  (not 20 anymore!)
    //
    // How to get element count:
    // sizeof(array) / sizeof(array[0]) = 20/4 = 5  <- only works on array
    // sizeof(px) / sizeof(px[0])       = 8/4  = 2  <- WRONG! don't do this

    // ============================================================
    // SECTION 7: POINTER ARITHMETIC WITH DIFFERENT TYPES
    // ============================================================

    // int*    px; px+1 = +4 bytes  (sizeof int = 4)
    // double* px; px+1 = +8 bytes  (sizeof double = 8)
    // char*   px; px+1 = +1 byte   (sizeof char = 1)
    // bool*   px; px+1 = +1 byte   (sizeof bool = 1)
    //
    // Formula: new_address = old_address + (n * sizeof(type))
    // Compiler does this math, you just write px + 1

    // ============================================================
    // SECTION 8: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: What is array[i] equivalent to in pointer notation?
    //     -> *(array + i). [] is syntactic sugar for pointer arithmetic.
    //        Compiler converts array[i] to *(array + i) internally.

    // Q2: What does pointer + 1 actually add in bytes?
    //     -> sizeof(pointed_type) bytes, NOT 1 byte.
    //        int* + 1 = +4 bytes. double* + 1 = +8 bytes.

    // Q3: What is array decay?
    //     -> Array name converting to pointer to first element.
    //        Size information is LOST in this conversion.
    //        sizeof(array) != sizeof(pointer to array).

    // Q4: Why does array++ not work but px++ does?
    //     -> Array name is a constant pointer (address of array fixed).
    //        Cannot move where array starts.
    //        px is a regular pointer variable, can be reassigned.

    // Q5: What is 2[array] in C++?
    //     -> Valid! Same as array[2] == *(array + 2) == *(2 + array).
    //        Addition is commutative. Never use in real code.
    //        Proves [] is just pointer arithmetic.

    // Q6: How do you get array size from a pointer?
    //     -> You CANNOT. Size info is lost after decay.
    //        Must pass size separately to functions.
    //        void f(int* arr, int size) <- C style
    //        void f(std::array<int,5>& arr) <- C++ style (no decay)

    // Q7: What is the address of array[3] if array starts at 100?
    //     -> 100 + (3 * sizeof(int)) = 100 + 12 = 112

    // Q8: What is the difference between px+1 and (px+1)?
    //     -> Same thing. Parentheses just for clarity.
    //        *(px+1) vs *px+1 ARE different though:
    //        *(px+1) = value at next element
    //        *px+1   = value at current element + 1 (precedence!)

    return 0;
}
// ```

// **The one fact that unlocks everything:**

// ```cpp
// array[i]  ==  *(array + i)  ==  *(px + i)  ==  px[i]
// // [] is NOT special syntax - it IS pointer arithmetic
// // compiler sees array[3] and immediately converts to *(array + 3)
// ```

// **And the byte math interviewers test:**

// ```
// px points to address 100 (int*)
// px + 0  =  100  (100 + 0*4)   ->  value 1
// px + 1  =  104  (100 + 1*4)   ->  value 3
// px + 2  =  108  (100 + 2*4)   ->  value 5
// px + 3  =  112  (100 + 3*4)   ->  value 7
// px + 4  =  116  (100 + 4*4)   ->  value 9
// ```

// > `+1` on a pointer never means `+1 byte`. It means `+sizeof(type)` bytes. The compiler does this math automatically because it knows the type.