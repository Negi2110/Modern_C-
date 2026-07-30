#include <iostream>
#include <span>
#include <array>
#include <vector>
#include <algorithm>

// ============================================================
// std::span - COMPLETE NOTES (C++20)
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::span = a non-owning VIEW into a contiguous sequence of elements
// "describes an object that can refer to a contiguous sequence
//  with the first element at position zero"
//
// YOUR COMMENT nails it:
// "span: pointer to first memory location + size of particular element"
// "pass around data structures in a non-owning way"
// "view into a container, does not own the elements"
//
// THINK OF IT AS:
// string_view : std::string  ==  std::span : std::vector/array
//
// INTERNAL STRUCTURE (like string_view):
// struct span<T> {
//     T* data;       // pointer to first element
//     size_t size;   // number of elements
// };
// sizeof(span) = 16 bytes (pointer + size) always
//
// WORKS WITH ANY CONTIGUOUS CONTAINER:
// -> std::array     (contiguous, stack)
// -> std::vector    (contiguous, heap)
// -> C-style array  (contiguous, stack)
// -> raw pointer + size
//
// DOES NOT WORK WITH:
// -> std::list      (not contiguous)
// -> std::deque     (not contiguous)
// -> std::map/set   (not contiguous)

// ============================================================
// SECTION 2: STATIC vs DYNAMIC EXTENT
// ============================================================

// TWO FORMS of span:
//
// STATIC EXTENT: std::span<int, 4>
// -> size known at COMPILE TIME (template parameter)
// -> span.extent = 4 (the compile-time size)
// -> slightly more optimizable (compiler knows exact size)
// -> more restrictive (can only accept size-4 sequences)
//
// DYNAMIC EXTENT: std::span<int>  (default, most common)
// -> same as: std::span<int, std::dynamic_extent>
// -> size known at RUNTIME only
// -> span.extent = std::dynamic_extent (= SIZE_MAX = very large number)
// -> more flexible (accepts any size)
//
// YOUR CODE:
// std::array<int, 4> arr = {1, 2, 3, 4};
// std::span mySpan{arr};          // CTAD deduces span<int, 4> (STATIC!)
// mySpan.extent                   // = 4 (compile-time size)
//
// PrintIntData(arr)               // array<int,4> -> span<int> (DYNAMIC)
// param.extent                    // = dynamic_extent (runtime size)

// ============================================================
// SECTION 3: CONSTRUCTION
// ============================================================

// FROM std::array:
// std::array<int,4> arr = {1,2,3,4};
// std::span<int> s1(arr);              // dynamic extent, size=4
// std::span<int, 4> s2(arr);           // static extent 4
// std::span s3{arr};                   // CTAD: deduces span<int,4>
//
// FROM std::vector:
// std::vector<int> v = {1,2,3,4,5};
// std::span<int> s4(v);                // whole vector
// std::span<int> s5(v.data(), 3);      // first 3 elements
// std::span<int> s6(v.begin(), 3);     // YOUR CODE: first 3 elements
//
// FROM C-style array:
// int arr[5] = {1,2,3,4,5};
// std::span<int> s7(arr, 5);           // pointer + size
// std::span<int> s8(arr);              // CTAD: deduces span<int,5>
//
// FROM raw pointer:
// int* ptr = new int[5]{1,2,3,4,5};
// std::span<int> s9(ptr, 5);           // must provide size!

// ============================================================
// SECTION 4: IMPORTANT METHODS
// ============================================================

// SIZE:
// s.size()          -> number of elements               O(1)
// s.size_bytes()    -> total bytes (size * sizeof(T))   O(1)
// s.empty()         -> true if size == 0                O(1)
// s.extent          -> compile-time size or dynamic_extent
//
// ELEMENT ACCESS:
// s[i]              -> element at i (NO bounds check)   O(1)
// s.front()         -> first element                    O(1)
// s.back()          -> last element                     O(1)
// s.data()          -> raw T* pointer                   O(1)
//
// SUBVIEWS (most powerful feature - all O(1), no copies!):
// s.first(3)        -> span of first 3 elements
// s.last(3)         -> span of last 3 elements
// s.subspan(1, 3)   -> span starting at index 1, length 3
// s.subspan(2)      -> span from index 2 to end
//
// ITERATORS (works with ALL STL algorithms):
// s.begin(), s.end()
// s.rbegin(), s.rend()

// ============================================================
// SECTION 5: YOUR CODE EXPLAINED
// ============================================================

void PrintIntData(const std::span<int>& param)
// std::span<int> = dynamic extent (no size in template param)
// const& = don't copy the span itself (span is already just ptr+size
//          so copying is cheap, but const& is still good practice)
// NOTE: span is already a view (non-owning), no double-view needed
// Often written as: void PrintIntData(std::span<int> param) (by value is fine!)
{
    if (param.extent == std::dynamic_extent)
    // extent = static member, compile-time constant
    // span<int>: extent = dynamic_extent (runtime size)
    // span<int,4>: extent = 4 (compile-time known)
    {
        std::cout << "dynamic: " << std::endl;
        // prints when called with: vector, span(begin,3)
    }
    else
    {
        std::cout << "static " << std::endl;
        // prints when called with: array (if static extent preserved)
        // BUT: array<int,4> -> span<int> loses static info!
        // -> this else branch won't actually execute here
    }

    for (auto& elem : param)
    {
        std::cout << elem << std::endl;
    }
}

int main()
{
    // ---- SPAN FROM ARRAY ----
    std::array<int, 4> arr = {1, 2, 3, 4};
    std::span mySpan{arr};
    // CTAD deduces: std::span<int, 4> (STATIC extent!)
    // mySpan.data() = arr.data() (points INTO arr, no copy)

    std::cout << mySpan.extent << std::endl;
    // prints: 4 (static extent because CTAD deduced span<int,4>)

    PrintIntData(arr);
    // arr (array<int,4>) -> converted to span<int> (DYNAMIC)
    // inside: extent == dynamic_extent -> prints "dynamic:"
    // prints: 1 2 3 4

    // ---- SPAN FROM VECTOR (partial!) ----
    std::vector<int> myVector = {1, 2, 3, 4, 5, 6, 7};
    PrintIntData(std::span(myVector.begin(), 3));
    // span(begin_iterator, count) = first 3 elements only
    // span points to {1, 2, 3} - no copy of vector data!
    // dynamic extent: prints "dynamic:"
    // prints: 1 2 3
    // (elements 4,5,6,7 completely ignored - just a view into first 3)

    return 0;
}

// ============================================================
// SECTION 6: SUBSPAN - THE KILLER FEATURE
// ============================================================

// The most powerful thing about span is subspan operations
// ALL return a NEW SPAN (just ptr+size), ZERO allocation, ZERO copy

// std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};
// std::span<int> s(v);
//
// s.first(3)          -> view of {1,2,3}
// s.last(3)           -> view of {8,9,10}
// s.subspan(2, 4)     -> view of {3,4,5,6}
// s.subspan(5)        -> view of {6,7,8,9,10}
//
// WHY POWERFUL: process parts of arrays without copying!
// void processChunk(std::span<int> chunk) { }
// processChunk(s.subspan(0, 5));    // first half
// processChunk(s.subspan(5));       // second half
// -> split work across threads without memcpy

// ============================================================
// SECTION 7: span vs string_view COMPARISON
// ============================================================

// | Feature          | string_view          | span<T>              |
// |------------------|----------------------|----------------------|
// | What it views    | char sequences       | any type T           |
// | Owning?          | NO                   | NO                   |
// | Read-only?       | YES (const char*)    | NO (can modify T!)   |
// | sizeof           | 16 bytes             | 16 bytes             |
// | Works with       | string,char*,literal | array,vector,C-array |
// | Subrange         | substr()             | subspan/first/last   |
// | Null-terminated? | c_str() not safe     | N/A                  |
//
// IMPORTANT: span<int> is NOT const by default!
// void foo(std::span<int> s) { s[0] = 99; }  // MODIFIES original data!
// void foo(std::span<const int> s) { s[0]=99; }// COMPILE ERROR (read-only)
//
// For READ-ONLY: use std::span<const int>
// For MODIFIABLE: use std::span<int>

// ============================================================
// SECTION 8: DSA USE CASES
// ============================================================

// USE span WHEN:
// -> function needs to work on contiguous data of ANY container type
// -> want to pass a SLICE of an array/vector (subspan)
// -> replacing (T* ptr, int size) C-style function signatures
// -> processing chunks of data (divide and conquer)
//
// PATTERN 1: Replace T*, size pairs (most common)
// OLD: void process(int* data, int size) { }
// NEW: void process(std::span<int> data) { }
// -> safer, has .size(), works with array/vector/C-array
//
// PATTERN 2: Process subranges
// void mergeSort(std::span<int> data) {
//     if (data.size() <= 1) return;
//     size_t mid = data.size() / 2;
//     mergeSort(data.first(mid));       // left half
//     mergeSort(data.last(data.size()-mid)); // right half
// }
//
// PATTERN 3: Sliding window
// void slidingWindow(std::span<int> data, int k) {
//     for (size_t i = 0; i <= data.size()-k; i++) {
//         auto window = data.subspan(i, k);  // view of k elements
//         // process window - no copy!
//     }
// }
//
// PATTERN 4: Multi-dimensional views
// std::vector<int> matrix(9); // 3x3 flattened
// std::span<int> row0 = std::span(matrix).subspan(0, 3);
// std::span<int> row1 = std::span(matrix).subspan(3, 3);
// std::span<int> row2 = std::span(matrix).subspan(6, 3);

// ============================================================
// SECTION 9: DANGER - LIFETIME (same as string_view)
// ============================================================

// span does NOT own data -> source MUST outlive the span!
//
// DANGER 1: vector reallocation
// std::vector<int> v = {1,2,3};
// std::span<int> s(v);
// v.push_back(4);    // MAY reallocate vector's buffer!
// s[0];              // DANGLING! s points to old freed buffer
//
// DANGER 2: local variable goes out of scope
// std::span<int> bad() {
//     std::array<int,3> arr = {1,2,3};
//     return std::span(arr);  // DANGLING! arr destroyed on return
// }
//
// RULE: span is SAFE as a function parameter
//       span is DANGEROUS stored as member variable or returned

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::span and when was it introduced?
//     -> C++20. A non-owning view into a contiguous sequence.
//        Like string_view but for any type T.
//        Contains just a pointer and size (16 bytes).

// Q2: What is the difference between static and dynamic extent?
//     -> Static: span<int, 4> - size known at compile time.
//        Dynamic: span<int> - size known at runtime.
//        span.extent = compile-time size or dynamic_extent (SIZE_MAX).

// Q3: What containers can span view?
//     -> Only CONTIGUOUS containers: vector, array, C-style array,
//        raw pointer + size. NOT list, deque, map, set.

// Q4: Is span<int> read-only like string_view?
//     -> NO! span<int> allows MODIFICATION of the viewed data.
//        For read-only: use span<const int>.
//        This is a key difference from string_view.

// Q5: What are first(), last(), and subspan()?
//     -> Return new spans (views) into parts of the original.
//        All O(1), zero allocation, zero copy.
//        first(n): first n elements, last(n): last n elements.
//        subspan(offset, count): elements starting at offset.

// Q6: What is the danger with span and vector?
//     -> If vector reallocates (push_back causes growth), span
//        becomes a dangling pointer to freed memory. UB!
//        Never store span of a vector that might grow.

// Q7: How does span replace C-style (T*, size) function params?
//     -> void foo(int* ptr, int size) -> void foo(std::span<int> s)
//        span bundles pointer and size together, has bounds safety,
//        works with array/vector/C-array automatically.

// Q8: What is sizeof(span)?
//     -> Always 16 bytes (pointer + size), regardless of T or N.
//        Same as sizeof(string_view).
//        Cheap to copy by value (often preferred over const span&).
// ```

// **span vs string_view - the one difference that trips people:**

// ```cpp
// std::string_view sv = "hello";
// sv[0] = 'H';  // COMPILE ERROR: string_view is ALWAYS read-only

// std::vector<int> v = {1,2,3};
// std::span<int> s(v);
// s[0] = 99;    // WORKS: span<int> allows modification!
// // v[0] is now 99 (modifies the original!)

// std::span<const int> cs(v);
// cs[0] = 99;   // COMPILE ERROR: span<const int> is read-only
// ```

// **The subspan power - zero copy slicing:**

// ```cpp
// std::vector<int> data = {1,2,3,4,5,6,7,8,9,10};
// std::span<int> s(data);

// s.first(3)         // view {1,2,3}       ← no copy!
// s.last(3)          // view {8,9,10}      ← no copy!
// s.subspan(2, 4)    // view {3,4,5,6}     ← no copy!

// // Divide and conquer without any allocation:
// mergeSort(s.first(5));   // left half
// mergeSort(s.last(5));    // right half
// ```

// > **The replacement rule**: anywhere you have `(T* ptr, int size)` as function parameters in C or old C++ code, replace with `std::span<T>`. You get the pointer and size bundled together, iterator support, and subspan operations — all with zero overhead. It's the modern C++ way to say "give me a view into some contiguous data, I don't care what container it came from."