// ============================================================
// PASSING ARRAYS/VECTORS TO FUNCTIONS, size_t - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <array>
#include <vector>

// ============================================================
// SECTION 1: YOUR MAIN QUESTION - WHAT IS size_t?
// ============================================================

// size_t = unsigned integer type used for SIZES and COUNTS
//
// defined in <cstddef> (included by most headers automatically)
//
// WHY size_t and not int?
//
// int   = signed   = can be negative (-2billion to +2billion)
// size_t = unsigned = cannot be negative (0 to +4billion on 32-bit)
//                                        (0 to +18 quintillion on 64-bit)
//
// Size of something can NEVER be negative
// int size = -5;   // meaningless for a size
// size_t size = -5 // impossible, wraps to huge positive number
//
// size_t is guaranteed to hold the size of the LARGEST possible object
// on that platform
//
// On 32-bit: size_t = 4 bytes (uint32_t)
// On 64-bit: size_t = 8 bytes (uint64_t)
//
// WHERE you see size_t:
// sizeof()          -> returns size_t
// vec.size()        -> returns size_t
// arr.size()        -> returns size_t
// strlen()          -> returns size_t
// std::string::npos -> is size_t

// ============================================================
// SECTION 2: THREE WAYS TO PASS ARRAY TO FUNCTION
// ============================================================

// WAY 1: Raw array (C style) - DECAYS to pointer, size LOST
// void printArray(int arr[], int size)
// void printArray(int* arr, int size)   // identical to above!
// -> int arr[] and int* arr are SAME THING in function parameter
// -> size must be passed separately because sizeof(arr) = 8 (pointer!)
// -> no bounds checking possible without size

// WAY 2: std::array - NO decay, size baked into type
// void printArray(const std::array<int,5>& arr)
// -> size 5 is part of the TYPE itself
// -> no decay, arr.size() = 5 inside function
// -> DOWNSIDE: only works for arrays of EXACTLY size 5
//              std::array<int,3> won't compile with this function
// -> fix: use template
//    template<size_t N>
//    void printArray(const std::array<int,N>& arr)

// WAY 3: std::vector - dynamic size, most flexible
// void printVector(const std::vector<int>& vec)
// -> vec.size() works correctly inside function
// -> works for any size vector
// -> const& = no copy + read only = best practice

void printVector(const std::vector<int>& vec);  // forward declaration

int main()
{
    // int array[] = {1, 3, 5, 7, 9};        // raw array
    // std::array<int,5> array = {1,3,5,7,9}; // std::array

    std::vector<int> myVec = {1, 3, 5, 7, 9};
    printVector(myVec);
    // myVec passed as const reference
    // no copy of 5 elements made
    // vec inside function IS myVec (same memory)
    // const prevents accidental modification

    return 0;
}

// ============================================================
// SECTION 3: printVector IMPLEMENTATION EXPLAINED
// ============================================================

void printVector(const std::vector<int>& vec)
{
    // sizeof(vec) = 24 bytes (vector object size - ptr+size+capacity)
    // NOT the elements size - that's on heap
    // this is why sizeof is useless for vectors

    // vec.size() = 5 (number of elements currently in vector)
    // returns size_t (unsigned)
    //
    // COMPARISON WARNING:
    // for (int i = 0; i < vec.size(); i++)
    //                  ^        ^
    //                 int    size_t
    // comparing signed int with unsigned size_t -> compiler warning!
    //
    // FIXES:
    // Fix 1: cast i to size_t
    //   for (size_t i = 0; i < vec.size(); i++)
    //
    // Fix 2: cast size() to int
    //   for (int i = 0; i < (int)vec.size(); i++)
    //
    // Fix 3: range-based for (cleanest - no index needed)
    //   for (const auto& element : vec)
    //       std::cout << element << std::endl;

    for (int i = 0; i < (int)vec.size(); i++)
    {
        std::cout << vec[i] << std::endl;
    }
    // prints: 1, 2, 3, 4, 5 (each on new line)
}

// ============================================================
// SECTION 4: THE DECAY PROBLEM - IMPORTANT
// ============================================================

// void printArray(int arr[]) {
//     std::cout << sizeof(arr); // prints 8 NOT 20!
// }
//
// WHY?
// int arr[] in function parameter == int* arr
// they are IDENTICAL - compiler treats them the same
// arr is now just a pointer to first element
// sizeof(pointer) = 8 on 64-bit
// size info (5 elements) completely LOST
//
// PROOF:
// void f(int arr[])  { std::cout << sizeof(arr); } // 8
// void f(int* arr)   { std::cout << sizeof(arr); } // 8 - SAME!
// void f(int arr[5]) { std::cout << sizeof(arr); } // 8 - STILL 8!
// even writing [5] explicitly does nothing - still decays to pointer
//
// ONLY std::array avoids this:
// void f(std::array<int,5>& arr) { sizeof(arr); } // 20 - correct!

// ============================================================
// SECTION 5: size_t GOTCHA - UNDERFLOW
// ============================================================

// DANGEROUS CODE:
// for (size_t i = vec.size()-1; i >= 0; i--)  // reverse loop
// {
//     std::cout << vec[i];
// }
//
// BUG: when i = 0, i-- wraps to HUGE number (underflow!)
// size_t is UNSIGNED - cannot go below 0
// 0 - 1 = 18446744073709551615 (on 64-bit)
// loop never ends / accesses garbage memory
//
// FIX: use int for reverse loops
// for (int i = (int)vec.size()-1; i >= 0; i--)  // safe

// ============================================================
// SECTION 6: VECTOR METHODS RELATED TO SIZE
// ============================================================

// vec.size()     -> current number of elements (size_t)
// vec.capacity() -> allocated space (may be more than size)
// vec.empty()    -> true if size == 0 (bool)
// vec.resize(n)  -> change size to n (fills with 0 if growing)
// vec.reserve(n) -> pre-allocate capacity (no size change, avoids realloc)
//
// SIZE vs CAPACITY:
// std::vector<int> v;
// v.push_back(1);   // size=1, capacity=1
// v.push_back(2);   // size=2, capacity=2
// v.push_back(3);   // size=3, capacity=4 (doubled!)
// v.push_back(4);   // size=4, capacity=4
// v.push_back(5);   // size=5, capacity=8 (doubled again!)
//
// capacity grows by doubling to avoid reallocating every push_back
// INTERVIEW: What is the amortized complexity of push_back?
//   -> O(1) amortized. Occasionally O(n) when reallocation happens
//      but averaged over many operations = O(1).

// ============================================================
// SECTION 7: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is size_t?
//     -> Unsigned integer type for sizes and counts.
//        Cannot be negative. Size matches platform address size.
//        8 bytes on 64-bit, 4 bytes on 32-bit.

// Q2: Why use size_t instead of int for sizes?
//     -> Sizes are never negative. Unsigned prevents negative values.
//        Can represent larger sizes (no sign bit wasted).
//        Returned by sizeof, .size() etc - matches their type.

// Q3: What is the danger of size_t in reverse loops?
//     -> Unsigned underflow. size_t i = 0; i-- wraps to huge number.
//        Use int and cast .size() to int for reverse loops.

// Q4: What is array decay and how to prevent it?
//     -> Array losing size info when converted to pointer.
//        Prevent with std::array (no decay) or
//        pass size as separate parameter (C style).

// Q5: What is the difference between size() and capacity()?
//     -> size()     = actual number of elements in vector
//        capacity() = total allocated space (always >= size)
//        capacity grows by doubling when size exceeds it.

// Q6: Why pass vector by const reference?
//     -> No copy (efficient) + cannot modify (safe).
//        vec.size() still works on const reference.
//        Best practice for all large read-only parameters.

// Q7: What does vec.size() return for empty vector?
//     -> 0 (size_t). vec.empty() also returns true.
//        Check empty() before accessing elements.

// Q8: int arr[] vs int* arr as function parameter - same or different?
//     -> IDENTICAL. Compiler converts int arr[] to int* arr.
//        Both are just pointers, no size info.
//        sizeof(arr) = 8 in both cases (pointer size).
// ```

// **The three things to always remember when passing to functions:**

// ```cpp
// // raw array         -> decays to pointer, size LOST, pass size separately
// void f(int* arr, int size)

// // std::array        -> no decay, size in type, only exact size works
// void f(const std::array<int,5>& arr)

// // std::vector       -> no decay, .size() works, any size, heap data
// void f(const std::vector<int>& vec)  // <- most flexible, use this
// ```

// > And `size_t` in one line: **unsigned integer for sizes** — can never be negative, matches platform address size. The **reverse loop underflow** trap (`size_t i = 0; i--` wraps to huge number) is a **classic interview gotcha**.