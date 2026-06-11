
// ============================================================
// std::array vs raw array - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <numeric>
#include <iterator>
#include <array>    // needed for std::array

int main() {

    // ============================================================
    // SECTION 1: std::array vs raw array
    // ============================================================

    // Raw array:
    // int ids[100] = {};
    // -> C-style, no bounds checking, decays to pointer
    // -> sizeof(ids) = 400, but if passed to function -> becomes pointer
    // -> No helper methods

    // std::array:
    // std::array<int, 100> ids = {};
    // -> C++ wrapper around raw array
    // -> Size is FIXED at compile time (just like raw array)
    // -> Does NOT decay to pointer when passed to function
    // -> Has helper methods: .at(), .front(), .back(), .data(), .size()
    // -> Works perfectly with ALL STL algorithms

    std::array<int, 100> ids = {};  // all 100 elements = 0

    // ============================================================
    // SECTION 2: TEMPLATE SYNTAX EXPLAINED
    // ============================================================

    // std::array<int, 100>
    //             ^    ^
    //             |    |
    //           TYPE  SIZE
    //
    // Both TYPE and SIZE must be known at COMPILE TIME
    // Size must be a constexpr / literal, NOT a runtime variable
    //
    // int n = 100;
    // std::array<int, n> ids;   // ERROR - n is runtime variable
    //
    // constexpr int n = 100;
    // std::array<int, n> ids;   // VALID - n is compile time

    // ============================================================
    // SECTION 3: ids[100000] vs ids.at(100000)
    // ============================================================

    // ids[100000] = 9;
    // -> operator[] -> NO bounds checking
    // -> Accesses memory way beyond array -> UNDEFINED BEHAVIOR
    // -> Program may crash, corrupt memory, or silently do nothing
    // -> C++ trusts you to stay in bounds (performance reason)

    // ids.at(1000000) = 9;
    // -> .at() -> WITH bounds checking
    // -> If index >= size -> throws std::out_of_range EXCEPTION
    // -> Program throws exception, you can catch it with try/catch
    // -> Safer but SLIGHTLY slower (one extra comparison per access)

    // INTERVIEW: When to use [] vs .at()?
    //   -> [] in performance-critical code where you are SURE of bounds
    //   -> .at() during development/debugging or when safety matters

    // ============================================================
    // SECTION 4: ALL std::array METHODS EXPLAINED
    // ============================================================

    std::iota(std::begin(ids), std::end(ids), 10);

    // .at(index) -> bounds-checked access, throws if out of range
    ids.at(99) = 9;          // valid, index 99 is last element (0-99)
    // ids.at(100) = 9;      // throws std::out_of_range -> index 100 DNE

    // .front() -> returns FIRST element (ids[0])
    std::cout << ids.front() << std::endl;  // prints 10

    // .back() -> returns LAST element (ids[99])
    std::cout << ids.back() << std::endl;   // prints 9 (we set it above)

    // .size() -> returns number of elements
    std::cout << ids.size() << std::endl;   // prints 100

    // .data() -> returns RAW POINTER to underlying array
    int* raw = ids.data();                  // same as &ids[0]
    std::cout << raw[0] << std::endl;       // prints 10
    // USE CASE: when you need to pass std::array to a C function
    // that expects a raw int* pointer

    // .empty() -> returns true if size is 0
    std::cout << ids.empty() << std::endl;  // prints 0 (false, has 100 elements)

    // .fill(value) -> sets ALL elements to given value
    // ids.fill(0);  // resets everything back to 0

    // ============================================================
    // SECTION 5: MEMORY - std::array vs raw array
    // ============================================================

    // BOTH live on the STACK (not heap)
    // BOTH have same memory layout (contiguous)
    // BOTH have same SIZE in memory
    //
    // sizeof(int ids[100])          = 400 bytes
    // sizeof(std::array<int,100>)   = 400 bytes  <- identical!
    //
    // std::array is a ZERO COST ABSTRACTION
    // -> Gives you safety + methods with NO extra memory or runtime cost
    // -> This is a core C++ philosophy: "pay only for what you use"

    // ============================================================
    // SECTION 6: WHY std::array DOES NOT DECAY
    // ============================================================

    // Raw array DECAYS to pointer when passed to function:
    // void foo(int arr[]) { sizeof(arr); }  // sizeof gives 8 (pointer size!)
    // void foo(int* arr)  { sizeof(arr); }  // same thing, 8 bytes
    //
    // std::array does NOT decay:
    // void foo(std::array<int,100> arr) { arr.size(); }  // still 100!
    //
    // INTERVIEW: What is array decay?
    //   -> When a raw array is passed to a function, it loses its
    //      size information and becomes a raw pointer.
    //      std::array solves this problem completely.

    // ============================================================
    // SECTION 7: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: What is the difference between raw array and std::array?
    //     -> Raw array: C-style, decays to pointer, no bounds check,
    //                   no methods, size lost when passed to functions
    //        std::array: C++ wrapper, no decay, .at() bounds check,
    //                    helper methods, same memory cost (zero overhead)

    // Q2: What does .at() do differently from []?
    //     -> .at() checks bounds and throws std::out_of_range if exceeded
    //        [] does no checking -> undefined behavior if out of bounds

    // Q3: What is zero cost abstraction?
    //     -> A C++ feature that provides safety/convenience
    //        with NO additional runtime or memory cost
    //        std::array is a perfect example.

    // Q4: What is sizeof(std::array<int,100>)?
    //     -> 400 bytes. Same as raw int[100].
    //        No extra memory overhead.

    // Q5: Can std::array size be a runtime variable?
    //     -> NO. Size must be a compile-time constant (constexpr or literal)
    //        For runtime size -> use std::vector instead

    // Q6: What is .data() used for?
    //     -> Returns raw pointer to underlying array.
    //        Used when interfacing with C APIs or legacy code
    //        that expects a raw pointer.

    // Q7: Where is std::array stored? Stack or Heap?
    //     -> STACK (just like raw array)
    //        For heap allocation -> use std::vector

    // Q8: What exception does .at() throw?
    //     -> std::out_of_range (defined in <stdexcept>)
    //        Can be caught with: catch(const std::out_of_range& e)

    for (int i = 0; i < 100; i++) {
        std::cout << ids[i] << std::endl;
    }

    return 0;
}
// ```

// **The single most important interview point here:**

// > `std::array` is a **zero cost abstraction** — same memory, same speed as raw array, but you get `.at()`, `.size()`, no pointer decay, and full STL compatibility. There is **no reason** to use raw arrays in modern C++.

// **Quick cheat sheet:**

// | | `int arr[100]` | `std::array<int,100>` | `std::vector<int>` |
// |---|---|---|---|
// | Size fixed? | ✅ | ✅ | ❌ dynamic |
// | Stack/Heap | Stack | Stack | Heap |
// | Bounds check | ❌ | ✅ `.at()` | ✅ `.at()` |
// | Decays to pointer | ✅ (bad) | ❌ | ❌ |
// | Size known in function | ❌ | ✅ | ✅ |

//Array-data stored comtigiously




// ============================================================
// ARRAYS & WHY YOUR CODE PRINTS NOTHING - INTERVIEW NOTES
// ============================================================

// #include <iostream>

// int main() {

//     int ids[10];

//     return 0;              // <-- PROGRAM EXITS HERE
//                            // Everything after return 0 is DEAD CODE
//                            // Compiler may even warn: "unreachable code"

//     std::cout << "the val:" << ids[2];  // NEVER REACHED
// }

// ============================================================
// THAT IS THE ONLY REASON - return 0 is BEFORE cout
// ============================================================

// Fix: move return 0; to the END

// int main() {
//     int ids[10];
//     std::cout << "the val:" << ids[2];  // now this runs
//     return 0;                            // THEN exit
// }

// ============================================================
// BUT WAIT - ids[2] would print GARBAGE, here is why
// ============================================================

// int ids[10];  -> declared but NOT initialized
//
// In C++, local arrays are NOT zero-initialized by default
// They contain whatever garbage bytes were already on the stack
//
// ids[0] = ???  (random garbage)
// ids[1] = ???  (random garbage)
// ids[2] = ???  (random garbage)  <- this is what cout would print
//
// Output could be: "the val: -858993460"  or any random number
// This is UNDEFINED BEHAVIOR - reading uninitialized memory

// HOW TO PROPERLY INITIALIZE:

// Option 1: Initialize all to zero
//     int ids[10] = {};          // all 10 elements = 0
//     int ids[10] = {0};         // same thing

// Option 2: Initialize with values
//     int ids[10] = {1,2,3,4,5,6,7,8,9,10};

// Option 3: Initialize first few, rest become 0
//     int ids[10] = {1, 2, 3};   // ids[0]=1, ids[1]=2, ids[2]=3
//                                 // ids[3] to ids[9] = 0 automatically

// ============================================================
// SECTION 2: WHAT IS AN ARRAY?
// ============================================================

// Array = fixed size collection of SAME TYPE elements
//         stored CONTIGUOUSLY in memory (your own comment was right!)
//
// int ids[10];
//
// MEMORY LAYOUT (each int = 4 bytes):
//
// Address:  100  104  108  112  116  120  124  128  132  136
//           [0]  [1]  [2]  [3]  [4]  [5]  [6]  [7]  [8]  [9]
//
// Contiguous = no gaps between elements
// ids[0] starts at address 100
// ids[1] starts at address 104 (100 + sizeof(int))
// ids[2] starts at address 108 (100 + 2*sizeof(int))
//
// Formula: address of ids[i] = base_address + (i * sizeof(type))
// This is why array indexing is O(1) - direct math, no searching

// ============================================================
// SECTION 3: ARRAY NAME IS A POINTER
// ============================================================

// int ids[10];
// 'ids' by itself = address of first element = &ids[0]
//
// ids[2] is exactly same as *(ids + 2)
//   -> go to address (ids + 2*4), read the int there
//
// INTERVIEW: What does array name represent?
//   -> It decays to a pointer to the first element
//      ids == &ids[0]  -> both give same address

// ============================================================
// SECTION 4: INTERVIEW QUESTIONS
// ============================================================

// Q1: Why are arrays contiguous in memory?
//     -> Enables O(1) random access using index arithmetic.
//        address = base + index * element_size
//        If not contiguous, this formula would not work.

// Q2: What is the default value of an uninitialized local array?
//     -> GARBAGE values (undefined behavior to read them)
//        Global arrays ARE zero-initialized by default.

// Q3: What is the difference between global and local array initialization?
//     -> Global:  int arr[10];  -> all zeros guaranteed
//        Local:   int arr[10];  -> garbage, undefined behavior to read

// Q4: What happens if you access ids[15] in an array of size 10?
//     -> Out of bounds access -> UNDEFINED BEHAVIOR
//        C++ does NOT check bounds (unlike Java/Python)
//        Could crash, could read garbage, could corrupt memory
//        This is a common source of security vulnerabilities (buffer overflow)

// Q5: What is sizeof(ids) for int ids[10]?
//     -> 40 bytes (10 elements x 4 bytes each)
//        sizeof(ids) / sizeof(ids[0]) = 10  <- classic way to get array length

// Q6: Can array size be changed after declaration?
//     -> NO. Arrays are FIXED size in C++.
//        Use std::vector if you need dynamic sizing.

// Q7: What is the difference between array and pointer?
//     -> Array: has fixed size, sizeof gives total size
//        Pointer: just stores an address, sizeof gives 8 bytes (on 64-bit)
//        Array name DECAYS to pointer when passed to a function
//        which is why you must pass size separately to functions

// Q8: What is a buffer overflow?
//     -> Writing beyond array bounds, overwriting adjacent memory
//        Classic security vulnerability in C/C++
//        Example: ids[15] = 999;  on int ids[10] -> buffer overflow
// ```

// **Your actual bug in one line:**

// > `return 0;` was **before** `cout` — program exited before printing anything. Dead code after `return` never runs.

// **Bonus thing to remember:**

// > Local array = **garbage values** by default. Global array = **zero** by default. This difference **always comes up** in interviews.


// ```cpp
// // ============================================================
// // std::iota, std::begin, std::end - INTERVIEW NOTES
// // ============================================================

// #include <iostream>
// #include <numeric>   // for std::iota
// #include <iterator>  // for std::begin, std::end

// int main() {

//     int ids[10] = {};   // all 10 elements initialized to 0

//     // ============================================================
//     // SECTION 1: std::iota - WHAT DOES IT DO?
//     // ============================================================

//     // std::iota(begin, end, startValue)
//     //
//     // It fills a range with SEQUENTIALLY INCREASING values
//     // starting from startValue
//     //
//     // std::iota(std::begin(ids), std::end(ids), 10);
//     //
//     // ids[0]  = 10
//     // ids[1]  = 11
//     // ids[2]  = 12
//     // ids[3]  = 13
//     // ids[4]  = 14
//     // ids[5]  = 15
//     // ids[6]  = 16
//     // ids[7]  = 17
//     // ids[8]  = 18
//     // ids[9]  = 19
//     //
//     // Name comes from Greek letter iota (ι)
//     // Mathematically means "sequence of integers"

//     std::iota(std::begin(ids), std::end(ids), 10);

//     // ============================================================
//     // SECTION 2: std::begin and std::end - WHAT ARE THEY?
//     // ============================================================

//     // std::begin(ids) -> returns pointer to FIRST element (ids[0])
//     // std::end(ids)   -> returns pointer to ONE PAST LAST element
//     //                    (points AFTER ids[9], not AT ids[9])
//     //
//     // MEMORY VISUAL:
//     //
//     // index:  [0] [1] [2] [3] [4] [5] [6] [7] [8] [9]  [?]
//     //          ^                                          ^
//     //          |                                          |
//     //        begin()                                    end()
//     //                                            (one past last)
//     //
//     // INTERVIEW: Why does end() point ONE PAST the last element?
//     //   -> So that begin() == end() means the range is EMPTY
//     //   -> Allows standard loop: for(auto it=begin; it!=end; it++)
//     //   -> Dereferencing end() is UNDEFINED BEHAVIOR, it is a sentinel

//     // ============================================================
//     // SECTION 3: std::begin/end vs ids/ids+10
//     // ============================================================

//     // These two lines are IDENTICAL in effect:
//     //
//     // std::iota(std::begin(ids), std::end(ids), 10);  // modern, safe
//     // std::iota(ids, ids + 10, 10);                   // old style, raw pointer
//     //
//     // std::begin/end is PREFERRED because:
//     // -> Works with arrays, vectors, lists - any container
//     // -> ids+10 only works if you manually know the size (error prone)
//     // -> More readable, more generic

//     // ============================================================
//     // SECTION 4: THE FOR LOOP
//     // ============================================================

//     // for (int i = 0; i < 10; i++)
//     //     std::cout << ids[i] << std::endl;
//     //
//     // prints: 10 11 12 13 14 15 16 17 18 19 (each on new line)
//     //
//     // Modern C++ alternative - range based for loop:
//     // for (int val : ids) {
//     //     std::cout << val << std::endl;
//     // }
//     // Same output, cleaner syntax, no index needed
//     // INTERVIEW: What is a range-based for loop?
//     //   -> Syntactic sugar introduced in C++11
//     //      Internally uses begin() and end() behind the scenes

//     for (int i = 0; i < 10; i++) {
//         std::cout << ids[i] << std::endl;
//     }

//     // ============================================================
//     // SECTION 5: WHAT IS A RANGE IN C++?
//     // ============================================================

//     // Range = anything that has a begin and end
//     //
//     // [begin ........... end)
//     //  ^                  ^
//     //  inclusive          exclusive (one past last)
//     //
//     // This is called a HALF-OPEN RANGE
//     // Written mathematically as [begin, end)
//     //
//     // WHY half-open?
//     // -> end - begin = exact count of elements (10 - 0 = 10)
//     // -> Empty range: begin == end (no special case needed)
//     // -> Loops naturally: while(begin != end) { process; begin++; }

//     // ============================================================
//     // SECTION 6: HEADERS EXPLAINED
//     // ============================================================

//     // #include <numeric>
//     //   -> contains number-related algorithms
//     //   -> std::iota, std::accumulate, std::partial_sum etc.

//     // #include <iterator>
//     //   -> contains std::begin, std::end
//     //   -> also std::advance, std::distance, std::next etc.

//     // NOTE: in practice std::begin/end also come with <array>
//     // and many other headers. <iterator> makes it explicit.

//     // ============================================================
//     // SECTION 7: INTERVIEW QUESTIONS
//     // ============================================================

//     // Q1: What does std::iota do?
//     //     -> Fills a range with sequentially increasing values
//     //        starting from a given value.

//     // Q2: What header is std::iota in?
//     //     -> <numeric>

//     // Q3: Why does std::end point one past the last element?
//     //     -> To represent an empty range when begin == end,
//     //        and to allow standard half-open range iteration.
//     //        Dereferencing end() is undefined behavior.

//     // Q4: What is the difference between std::begin(arr) and arr?
//     //     -> For raw arrays, same result (both give pointer to first element)
//     //        But std::begin works generically with ALL containers.
//     //        arr alone only works as pointer for raw arrays.

//     // Q5: What is a half-open range?
//     //     -> [begin, end) -> begin is included, end is excluded
//     //        Standard convention in C++ STL for all algorithms.

//     // Q6: Can std::iota work with floats?
//     //     -> YES. float ids[5] = {};
//     //        std::iota(begin(ids), end(ids), 1.5f);
//     //        -> 1.5, 2.5, 3.5, 4.5, 5.5
//     //        Works with any type that supports operator++

//     // Q7: What is std::accumulate? (related to <numeric>)
//     //     -> Sums up all elements in a range
//     //        int sum = std::accumulate(begin(ids), end(ids), 0);
//     //        -> 10+11+12...+19 = 145
//     //        Third argument is the INITIAL value

//     return 0;
// }
// ```

// **The one thing interviewers love to ask about this:**

// > "Why does `end()` point **one past** the last element?"

// Answer: so `begin == end` naturally means **empty range**, and `end - begin` gives **exact count** — no off-by-one math needed. This half-open `[begin, end)` convention is used **everywhere** in STL.