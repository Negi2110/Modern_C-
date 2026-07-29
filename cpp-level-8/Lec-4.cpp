#include <iostream>
#include <array>
#include <algorithm>
#include <numeric>   // for std::accumulate, std::iota

// ============================================================
// std::array - COMPLETE NOTES (C++11)
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::array<T, N> = a SAFER wrapper around C-style arrays
// -> fixed size (N known at compile time, template parameter)
// -> stack allocated (no heap, no dynamic allocation)
// -> does NOT decay to pointer when passed to functions
// -> has size(), empty(), at() etc. (C-style array has none of these)
// -> zero overhead vs C-style array (same memory layout)
//
// YOUR COMMENT:
// "stack allocated - 4 byte integer * 5 = 20 bytes"
// "fixed size - cannot resize"
// "homogeneous type - all elements same type"
//
// C-STYLE ARRAY PROBLEMS (also in your comments):
// void print(int data[4]) { sizeof(data); }  // gives 8! (pointer size!)
// -> array DECAYS to pointer when passed to function
// -> size information LOST
// -> must pass size separately: print(data, 4)
//
// std::array SOLUTION:
// void print(std::array<int,5> param) { param.size(); }  // gives 5! correct
// -> NO decay, size is part of the TYPE itself

// ============================================================
// SECTION 2: CONSTRUCTION / INITIALIZATION
// ============================================================

// std::array<int, 5> a;                  // uninitialized! garbage values
// std::array<int, 5> a = {};             // zero-initialized {0,0,0,0,0}
// std::array<int, 5> a = {1, 2, 3, 4, 5}; // fully initialized
// std::array<int, 5> a = {1, 2};        // {1,2,0,0,0} rest are zero
// std::array<int, 5> a{1, 2, 3, 4, 5};  // same (C++11 uniform init)
//
// FILL after creation:
// a.fill(0);                              // set ALL elements to 0
// std::fill(a.begin(), a.end(), 7);       // set ALL to 7 (algorithm)
// std::iota(a.begin(), a.end(), 1);       // fill {1,2,3,4,5} sequential

// ============================================================
// SECTION 3: IMPORTANT METHODS
// ============================================================

// SIZE:
// a.size()      -> N (compile-time size, always correct)  O(1)
// a.max_size()  -> same as size() (fixed, can't grow)     O(1)
// a.empty()     -> true only if N==0 (rare)               O(1)
//
// ELEMENT ACCESS:
// a[i]          -> NO bounds check, UB if out of range    O(1)
// a.at(i)       -> WITH bounds check, throws out_of_range O(1)
// a.front()     -> first element (a[0])                   O(1)
// a.back()      -> last element  (a[N-1])                 O(1)
// a.data()      -> raw int* pointer (for C API interop)   O(1)
//                  YOUR CODE: legacyCCodeBase(arr.data(), 5)
//
// FILL:
// a.fill(val)   -> set ALL elements to val                O(n)
//
// SWAP:
// a.swap(b)     -> swap contents with another array       O(n)
//                  both must be same type AND same size!
//
// ITERATORS (work with ALL STL algorithms):
// a.begin()     -> iterator to first element
// a.end()       -> iterator to one past last
// a.rbegin()    -> reverse iterator (from back)
// a.rend()      -> reverse iterator (to before front)
// a.cbegin()    -> const iterator (read-only)
// a.cend()      -> const iterator

// ============================================================
// SECTION 4: STL ALGORITHMS WITH std::array
// ============================================================

// std::sort(a.begin(), a.end());             // sort ascending    O(n log n)
// std::sort(a.begin(), a.end(), greater<int>());// sort descending O(n log n)
// std::reverse(a.begin(), a.end());          // reverse in place  O(n)
// std::find(a.begin(), a.end(), val);        // find value        O(n)
// std::count(a.begin(), a.end(), val);       // count occurrences O(n)
// std::accumulate(a.begin(), a.end(), 0);    // sum all elements  O(n)
// std::fill(a.begin(), a.end(), val);        // fill with value   O(n)
// std::iota(a.begin(), a.end(), 1);          // fill 1,2,3,4,5    O(n)
// std::max_element(a.begin(), a.end());      // iterator to max   O(n)
// std::min_element(a.begin(), a.end());      // iterator to min   O(n)
// std::binary_search(a.begin(),a.end(),val); // requires sorted!  O(log n)

// ============================================================
// SECTION 5: C API INTEROP - .data()
// ============================================================

void legacyCCodeBase(int* elem, int size)
{
    // C function expecting raw pointer and size
    // YOUR CODE: legacyCCodeBase(arr.data(), 5)
    // arr.data() returns int* to underlying array
    // perfectly safe to pass to C functions
    // this is the BRIDGE between modern C++ and legacy C code
}

// ============================================================
// SECTION 6: PASSING TO FUNCTIONS - NO DECAY
// ============================================================

// WRONG (C-style - loses size info):
// void print(int data[5]) {
//     sizeof(data);  // 8 bytes (pointer!) not 20 bytes (array)
// }

// CORRECT (std::array - keeps size):
void printarray(std::array<int, 5> param)
// NOTE: this passes BY VALUE (copies the array!)
// for large arrays prefer: const std::array<int,5>& param
{
    std::cout << param.size()  << std::endl;  // 5 (correct!)
    std::cout << sizeof(param) << std::endl;  // 20 (5 * 4 bytes, correct!)
}

// BEST practice for functions:
// void readOnly (const std::array<int,5>& a)  // read, no copy
// void modifies (std::array<int,5>& a)         // modify, no copy
// void ownsCopy (std::array<int,5> a)          // rare, makes copy

// TEMPLATE version (any size):
// template<size_t N>
// void print(const std::array<int,N>& a) {
//     for (const auto& e : a) std::cout << e << " ";
// }
// -> works for array<int,5>, array<int,10>, array<int,100> etc.

// ============================================================
// SECTION 7: std::array vs C-ARRAY vs std::vector
// ============================================================

// | Feature              | C array     | std::array  | std::vector  |
// |----------------------|-------------|-------------|--------------|
// | Size                 | fixed       | fixed       | dynamic      |
// | Memory               | stack       | stack       | heap         |
// | Size in function     | LOST (decay)| kept        | kept         |
// | .size()              | NO          | YES         | YES          |
// | bounds check(.at())  | NO          | YES         | YES          |
// | STL algorithms       | YES(ptr)    | YES         | YES          |
// | Overhead vs C array  | zero        | zero        | has ptr+size |
// | sizeof               | n*sizeof(T) | n*sizeof(T) | 24 bytes     |
// | Resize               | NO          | NO          | YES          |
// | Default init to 0    | NO (global) | NO (use={}) | YES          |

// ============================================================
// SECTION 8: DSA USE CASES
// ============================================================

// USE std::array WHEN:
// -> size is known at compile time and fixed
// -> performance critical (stack > heap, no allocator overhead)
// -> want std::string style API on a C-style array
// -> replacing C-style arrays in modern code
// -> graph adjacency lists with fixed nodes
// -> fixed-size lookup tables
// -> fixed board games: chess 8x8, tic-tac-toe 3x3
//
// COMMON DSA PATTERNS:

// Pattern 1: Fixed size buffer
// std::array<char, 256> buffer = {};
// buffer.fill(0);

// Pattern 2: Frequency count (26 letters)
// std::array<int, 26> freq = {};
// for (char c : str) freq[c - 'a']++;

// Pattern 3: DP table (fixed size)
// std::array<int, 100> dp = {};
// dp.fill(INT_MAX);
// dp[0] = 0;

// Pattern 4: Direction vectors (common in grid problems)
// std::array<std::array<int,2>, 4> dirs = {{{0,1},{0,-1},{1,0},{-1,0}}};
// for (auto& d : dirs) {
//     int nx = x + d[0], ny = y + d[1];
// }

// Pattern 5: Sorted search with binary_search
// std::array<int, 5> arr = {1, 3, 5, 7, 9};
// // array must be sorted for binary_search!
// bool found = std::binary_search(arr.begin(), arr.end(), 5);

// ============================================================
// SECTION 9: YOUR COMMENT - at() for bounds checking
// ============================================================

// std::cout << data.at(8) << std::endl;  // YOUR COMMENTED CODE
// -> array size is 5, index 8 is OUT OF BOUNDS
// -> at() throws std::out_of_range exception
// -> without try/catch: program terminates with exception message
// -> data[8] would be SILENT UB (reads garbage/crashes)
//
// try {
//     std::cout << data.at(8);
// } catch (const std::out_of_range& e) {
//     std::cout << "Out of range: " << e.what() << '\n';
// }

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::array and how does it differ from C-style array?
//     -> std::array is a fixed-size stack-allocated container wrapping
//        a C-style array. Key differences: does NOT decay to pointer
//        when passed to functions, has .size(), .at(), iterators,
//        works with STL algorithms cleanly.

// Q2: What does "array decay" mean and how does std::array fix it?
//     -> C-style array loses its size info when passed to a function
//        (becomes a pointer). std::array embeds size N in its TYPE
//        so size is never lost. sizeof() always gives correct result.

// Q3: What is the difference between a[i] and a.at(i)?
//     -> a[i]: no bounds check, undefined behavior if out of range.
//        a.at(i): bounds check, throws std::out_of_range if invalid.
//        Use at() during development, [i] in performance-critical loops.

// Q4: Where is std::array allocated?
//     -> Stack (like C-style array). No heap allocation.
//        This makes it faster than std::vector for small fixed-size data.
//        std::array<int,5> = 20 bytes on the stack.

// Q5: What does .data() return and when do you use it?
//     -> Returns raw pointer (int*) to the underlying array.
//        Use ONLY when interfacing with C APIs that expect raw pointers.
//        YOUR CODE: legacyCCodeBase(arr.data(), 5)

// Q6: What is the overhead of std::array vs C-style array?
//     -> ZERO overhead. Same memory layout, same size.
//        sizeof(std::array<int,5>) == sizeof(int[5]) == 20 bytes.
//        Compiler optimizes away the wrapper completely.

// Q7: Can std::array be used with STL algorithms?
//     -> YES. Has begin()/end() iterators so ALL STL algorithms work:
//        std::sort, std::find, std::count, std::accumulate etc.

// Q8: When would you choose std::array over std::vector?
//     -> Size known at compile time AND won't change.
//        Need stack allocation (no heap, faster).
//        Need zero overhead wrapper with STL API.
//        Examples: direction vectors, frequency tables, fixed boards.

int main()
{
    // ---- BASIC CREATION ----
    std::array<int, 5> arr = {11, 2, 8, 4, 5};

    // ---- SIZE METHODS ----
    std::cout << "size:     " << arr.size()     << '\n';  // 5
    std::cout << "max_size: " << arr.max_size() << '\n';  // 5 (same as size!)
    std::cout << "sizeof:   " << sizeof(arr)    << '\n';  // 20 (5*4 bytes)
    std::cout << "empty:    " << arr.empty()    << '\n';  // 0 (false)

    // ---- ACCESS ----
    std::cout << "front: "  << arr.front() << '\n';  // 11
    std::cout << "back:  "  << arr.back()  << '\n';  // 5
    std::cout << "at(2): "  << arr.at(2)   << '\n';  // 8
    std::cout << "[1]:   "  << arr[1]      << '\n';  // 2

    // ---- SORT ----
    std::sort(arr.begin(), arr.end());
    std::cout << "sorted: ";
    for (const auto& e : arr) std::cout << e << " ";
    std::cout << '\n';  // 2 4 5 8 11

    // ---- REVERSE ----
    std::reverse(arr.begin(), arr.end());
    std::cout << "reversed: ";
    for (const auto& e : arr) std::cout << e << " ";
    std::cout << '\n';  // 11 8 5 4 2

    // ---- FILL ----
    std::array<int, 5> filled;
    filled.fill(7);
    std::cout << "filled: ";
    for (const auto& e : filled) std::cout << e << " ";
    std::cout << '\n';  // 7 7 7 7 7

    // ---- IOTA (sequential fill) ----
    std::array<int, 5> seq;
    std::iota(seq.begin(), seq.end(), 1);
    std::cout << "iota: ";
    for (const auto& e : seq) std::cout << e << " ";
    std::cout << '\n';  // 1 2 3 4 5

    // ---- ACCUMULATE (sum) ----
    int sum = std::accumulate(arr.begin(), arr.end(), 0);
    std::cout << "sum: " << sum << '\n';  // 30

    // ---- MAX/MIN ELEMENT ----
    auto maxIt = std::max_element(arr.begin(), arr.end());
    auto minIt = std::min_element(arr.begin(), arr.end());
    std::cout << "max: " << *maxIt << '\n';  // 11
    std::cout << "min: " << *minIt << '\n';  // 2

    // ---- C API INTEROP ----
    legacyCCodeBase(arr.data(), 5);  // raw pointer for C functions

    // ---- BOUNDS CHECK ----
    try {
        std::cout << arr.at(10) << '\n';  // throws!
    } catch (const std::out_of_range& e) {
        std::cout << "caught: " << e.what() << '\n';
    }

    // ---- DSA: FREQUENCY COUNT ----
    std::string word = "hello";
    std::array<int, 26> freq = {};
    for (char c : word) freq[c - 'a']++;
    std::cout << "freq of l: " << freq['l' - 'a'] << '\n';  // 2

    return 0;
}
// ```

// **Quick reference - methods that matter:**

// ```cpp
// std::array<int, 5> a = {3, 1, 4, 1, 5};

// a.size()          // 5          always correct, unlike C-array
// a.at(2)           // 4          safe access (throws if bad index)
// a[2]              // 4          fast access (no check)
// a.front()         // 3          first
// a.back()          // 5          last
// a.fill(0)         // {0,0,0,0,0} set all
// a.data()          // int*       for C API interop

// std::sort(a.begin(), a.end());       // sort
// std::reverse(a.begin(), a.end());    // reverse
// std::iota(a.begin(), a.end(), 1);    // fill 1,2,3,4,5
// std::accumulate(a.begin(),a.end(),0);// sum = 14
// ```

// > **DSA golden rule**: reach for `std::array` when your C-style array has a **fixed size known at compile time** — you get the same zero-overhead stack performance but with `.size()`, `.at()`, and full STL algorithm compatibility. The most common DSA use: `std::array<int, 26> freq = {};` for character frequency counts.