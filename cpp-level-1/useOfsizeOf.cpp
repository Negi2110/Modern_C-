// ============================================================
// sizeof - ALL DATA TYPES COMPARED - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <vector>

int main()
{
    // ============================================================
    // SECTION 1: YOUR MAIN QUESTION - WHY IS POINTER LARGER THAN int?
    // ============================================================

    // int x = 7;
    // sizeof(x) = 4 bytes
    //
    // int* px = &x;
    // sizeof(px) = 8 bytes  <- LARGER than int!
    //
    // WHY? Because pointer stores a MEMORY ADDRESS
    // Memory address size depends on ARCHITECTURE:
    //
    // 32-bit system:
    // -> RAM addressable up to 2^32 = 4GB
    // -> address needs 32 bits = 4 bytes to store
    // -> sizeof(any pointer) = 4 bytes
    //
    // 64-bit system (your system):
    // -> RAM addressable up to 2^64 = 18 exabytes
    // -> address needs 64 bits = 8 bytes to store
    // -> sizeof(any pointer) = 8 bytes
    //
    // pointer size has NOTHING to do with what it points to:
    // sizeof(int*)    = 8  (stores address)
    // sizeof(double*) = 8  (stores address)
    // sizeof(char*)   = 8  (stores address)
    // sizeof(bool*)   = 8  (stores address)
    // ALL pointers = 8 bytes on 64-bit, 4 bytes on 32-bit
    // because ALL addresses are same size on same architecture

    // ============================================================
    // SECTION 2: ALL VARIABLES AND THEIR sizeof
    // ============================================================

    int x = 7;
    int* px = &x;
    int array[] = {1, 3, 5, 7, 9};
    int* dynamicallyAllocatedArray = new int[1000];
    std::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.push_back(5);
    v.push_back(5);
    v.push_back(5);
    v.push_back(5);   // 8 elements pushed

    std::cout << "x        :" << sizeof(x)    << std::endl; // 4
    std::cout << "px       :" << sizeof(px)   << std::endl; // 8
    std::cout << "array    :" << sizeof(array) << std::endl; // 20
    std::cout << "dynArray :" << sizeof(dynamicallyAllocatedArray) << std::endl; // 8
    std::cout << "v        :" << sizeof(v)    << std::endl; // 24

    // ============================================================
    // SECTION 3: EACH sizeof RESULT EXPLAINED
    // ============================================================

    // sizeof(x) = 4
    // -> int = 4 bytes, always
    // -> stores value 7 directly on stack

    // sizeof(px) = 8
    // -> pointer on 64-bit = 8 bytes (stores address)
    // -> does NOT matter that it points to int (4 bytes)
    // -> the ADDRESS itself needs 8 bytes to store

    // sizeof(array) = 20
    // -> int array[5] = 5 * sizeof(int) = 5 * 4 = 20 bytes
    // -> compiler KNOWS the size because it's a stack array
    // -> sizeof gives TOTAL size of entire array
    // -> element count: sizeof(array)/sizeof(array[0]) = 20/4 = 5

    // sizeof(dynamicallyAllocatedArray) = 8
    // -> this is just a POINTER (int*)
    // -> actual 1000 ints live on HEAP (4000 bytes)
    // -> but sizeof only sees the pointer variable = 8 bytes
    // -> sizeof has NO IDEA how much heap was allocated!
    // -> this is the key difference from stack array

    // sizeof(v) = 24
    // -> std::vector object on stack = 24 bytes (on most systems)
    // -> vector internally stores 3 things:
    //    1. pointer to heap data  (8 bytes)
    //    2. size (current elements) (8 bytes)
    //    3. capacity (allocated space) (8 bytes)
    //    total = 24 bytes
    // -> does NOT matter how many elements pushed (8 pushbacks here)
    // -> actual element data lives on HEAP
    // -> sizeof(v) = 24 whether v has 0 or 1 million elements!

    // ============================================================
    // SECTION 4: MEMORY VISUAL - WHERE EVERYTHING LIVES
    // ============================================================

    // STACK:
    // |__________________________|
    // | x = 7          (4 bytes) |
    // | px = 0x...     (8 bytes) | -> points to x
    // | array[5]       (20 bytes)| -> [1][3][5][7][9] all here
    // | dynArr = 0x... (8 bytes) | -> points to heap
    // | v object       (24 bytes)| -> ptr+size+capacity, data on heap
    // |__________________________|
    //
    // HEAP:
    // |__________________________________|
    // | new int[1000]  (4000 bytes)      | <- dynArr points here
    // | vector data    (8 * 4 = 32 bytes)| <- v's internal ptr points here
    // |__________________________________|

    // ============================================================
    // SECTION 5: SIZEOF SUMMARY TABLE
    // ============================================================

    // | Variable                    | sizeof | Why                          |
    // |-----------------------------|--------|------------------------------|
    // | int x                       | 4      | int = 4 bytes                |
    // | int* px                     | 8      | pointer = 8 bytes (64-bit)   |
    // | int array[5]                | 20     | 5 * 4 = 20 (full array)      |
    // | int* dynArr (new int[1000]) | 8      | just the pointer, not heap   |
    // | std::vector<int> v          | 24     | ptr+size+capacity on stack   |
    // | std::array<int,5>           | 20     | same as raw array, no extra  |
    // | bool                        | 1      | 1 byte (not 1 bit!)          |
    // | char                        | 1      | 1 byte                       |
    // | float                       | 4      | IEEE 754 single              |
    // | double                      | 8      | IEEE 754 double              |
    // | long long                   | 8      | 64-bit integer               |

    // ============================================================
    // SECTION 6: THE DECAY PROBLEM - SIZEOF IN FUNCTIONS
    // ============================================================

    // void printSize(int arr[]) {
    //     std::cout << sizeof(arr); // prints 8 NOT 20!
    // }                             // arr decayed to int* inside function
    //                               // sizeof sees pointer, not array
    //
    // printSize(array);  // passes pointer to first element
    //                    // size info LOST
    //
    // FIX 1: pass size separately (C style)
    // void printSize(int arr[], int size) { }
    //
    // FIX 2: use std::array (no decay)
    // void printSize(std::array<int,5>& arr) {
    //     sizeof(arr); // 20 - correct!
    // }
    //
    // FIX 3: use template (deduce size)
    // template<size_t N>
    // void printSize(int (&arr)[N]) {
    //     std::cout << N; // 5 - correct!
    // }

    // ============================================================
    // SECTION 7: sizeof IS COMPILE TIME
    // ============================================================

    // sizeof is evaluated at COMPILE TIME (not runtime)
    // it is an OPERATOR not a function (no parentheses needed for types)
    //
    // sizeof(int)   // type - parentheses required
    // sizeof x      // variable - parentheses optional
    // sizeof(x)     // also valid
    //
    // sizeof CANNOT know heap allocation size:
    // int* p = new int[1000];
    // sizeof(p) = 8  // pointer size only
    // 1000 * 4 = 4000 bytes on heap -> sizeof has NO access to this
    // you must track heap size yourself (or use vector which does it)

    // ============================================================
    // SECTION 8: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: Why is sizeof(pointer) larger than sizeof(int)?
    //     -> Pointer stores a memory ADDRESS, not a value.
    //        On 64-bit system, address = 64 bits = 8 bytes.
    //        int = 32 bits = 4 bytes. Address space is larger.

    // Q2: Does sizeof(pointer) depend on what it points to?
    //     -> NO. sizeof(int*) == sizeof(double*) == sizeof(char*) == 8
    //        All pointers store addresses, addresses are same size.

    // Q3: What does sizeof(vector) return?
    //     -> 24 bytes (on most 64-bit systems).
    //        The vector OBJECT size (ptr+size+capacity).
    //        NOT the size of elements stored. Elements are on heap.

    // Q4: Why does sizeof(dynArray) return 8 not 4000?
    //     -> dynArray is just int* (a pointer).
    //        sizeof sees the pointer (8 bytes), not heap allocation.
    //        sizeof cannot inspect heap at compile time.

    // Q5: How do you get element count of a stack array?
    //     -> sizeof(array) / sizeof(array[0])
    //        = 20 / 4 = 5 elements
    //        Only works on actual array, not pointer to array.

    // Q6: What is sizeof on 32-bit vs 64-bit for pointers?
    //     -> 32-bit: sizeof(pointer) = 4 bytes
    //        64-bit: sizeof(pointer) = 8 bytes
    //        int/float/double sizes are same on both.

    // Q7: Is sizeof a function or operator?
    //     -> OPERATOR. Evaluated at compile time.
    //        sizeof(int) needs parentheses (type).
    //        sizeof x    no parentheses needed (variable).

    // Q8: Why does sizeof(vector) stay 24 regardless of elements?
    //     -> Vector object on stack always has 3 pointers = 24 bytes.
    //        Elements stored on heap (separate allocation).
    //        Stack object size is fixed, heap grows/shrinks dynamically.

    delete[] dynamicallyAllocatedArray;  // always free heap memory!
    dynamicallyAllocatedArray = nullptr; // avoid dangling pointer

    return 0;
}
// ```

// **The sizeof cheat sheet interviewers test:**

// ```cpp
// int x;              // sizeof = 4   (value)
// int* px;            // sizeof = 8   (address, 64-bit)
// int array[5];       // sizeof = 20  (full array, 5*4)
// int* heap=new[1000];// sizeof = 8   (just pointer, NOT 4000!)
// std::vector<int> v; // sizeof = 24  (ptr+size+cap, NOT elements!)
// ```

// > **The trick question:** `sizeof(dynamicArray)` where `dynamicArray = new int[1000]`
// > Answer is **8** (pointer size), NOT 4000.
// > sizeof **cannot see the heap** — it only sees the pointer variable on the stack.