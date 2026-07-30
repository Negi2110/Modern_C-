#include <iostream>
#include <vector>
#include <span>
#include <algorithm>
#include <numeric>

// ============================================================
// std::vector - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::vector<T> = dynamic array (heap allocated, auto-resizing)
// YOUR COMMENT: "dynamic = heap allocated"
//
// INTERNAL STRUCTURE (your comment shows this):
// class vector {
//     T*     m_data;    // pointer to heap array
//     size_t size;      // number of elements currently stored
//     size_t capacity;  // number of elements allocated (>= size)
// };
//
// KEY CONCEPT: size vs capacity
// size     = how many elements ARE IN the vector right now
// capacity = how many elements can fit WITHOUT reallocation
//
// GROWTH STRATEGY (amortized O(1) push_back):
// when size == capacity and you push_back:
// 1. allocate NEW buffer (typically 2x capacity)
// 2. COPY all elements to new buffer
// 3. DELETE old buffer
// 4. add new element
// -> expensive but rare -> amortized O(1) overall
//
// YOUR push_back example:
// push_back(1) -> size=1, capacity=1
// push_back(2) -> size=2, capacity=2 (reallocate! 1->2)
// push_back(3) -> size=3, capacity=4 (reallocate! 2->4)
// push_back(4) -> size=4, capacity=4
// push_back(5) -> size=5, capacity=8 (reallocate! 4->8)
// This is WHY reserve() matters for performance

// ============================================================
// SECTION 2: CONSTRUCTION / INITIALIZATION
// ============================================================

// std::vector<int> v;                    // empty, size=0, capacity=0
// std::vector<int> v(5);                 // 5 elements, all 0
// std::vector<int> v(5, 42);             // 5 elements, all 42
// std::vector<int> v = {1,2,3,4,5};      // initializer list
// std::vector<int> v{1,2,3,4,5};         // same (uniform init)
// std::vector v{1,2,3,4};               // CTAD: deduces vector<int>
//                                         YOUR CODE uses this!
// std::vector<int> v2(v);               // copy constructor
// std::vector<int> v3(v.begin(), v.end()); // from iterator range
// std::vector<int> v4(v.begin(), v.begin()+3); // first 3 elements

// ============================================================
// SECTION 3: IMPORTANT METHODS WITH COMPLEXITY
// ============================================================

// SIZE AND CAPACITY:
// v.size()          -> current element count              O(1)
// v.capacity()      -> allocated space (>= size)         O(1)
// v.empty()         -> true if size == 0                 O(1)
// v.max_size()      -> theoretical max elements          O(1)
// v.reserve(n)      -> pre-allocate for n elements       O(n)
//                      capacity >= n after this call
//                      does NOT change size!
// v.shrink_to_fit() -> request capacity = size           O(n)
//                      non-binding (impl may ignore)
//                      YOUR CODE: after shrink_to_fit,
//                      capacity drops from 8 to 5
// v.resize(n)       -> change size to n                  O(n)
//                      if n > size: add default elements
//                      if n < size: remove elements
// v.clear()         -> remove all elements, size=0       O(n)
//                      capacity UNCHANGED after clear!

// ELEMENT ACCESS:
// v[i]              -> no bounds check, UB if bad index  O(1)
// v.at(i)           -> bounds check, throws out_of_range O(1)
// v.front()         -> first element                     O(1)
// v.back()          -> last element                      O(1)
// v.data()          -> raw T* pointer (for C API)        O(1)

// ADD/REMOVE:
// v.push_back(val)  -> add to END                        O(1) amortized
// v.pop_back()      -> remove from END                   O(1)
// v.emplace_back(args) -> construct in-place at END      O(1) amortized
//                         FASTER than push_back for complex objects!
// v.insert(it, val) -> insert at iterator position       O(n) shifts!
// v.erase(it)       -> remove at iterator position       O(n) shifts!
// v.erase(it1,it2)  -> remove range [it1, it2)          O(n)
//                      YOUR COMMENT: "erase is linear time operation"

// ============================================================
// SECTION 4: WHY reserve() IS CRITICAL FOR PERFORMANCE
// ============================================================

// WITHOUT reserve (BAD for large data):
// std::vector<long> v;
// for (size_t i = 0; i < 500000; i++) {
//     v.push_back(i);   // reallocates ~20 times! (1,2,4,8,16...500000)
//                        // each reallocation: copy ALL existing elements
//                        // total work: O(n log n) copies
// }
//
// WITH reserve (GOOD - your code):
// std::vector<long> v;
// v.reserve(500000);    // ONE allocation upfront
// for (size_t i = 0; i < 500000; i++) {
//     v.push_back(i);   // NEVER reallocates (capacity already 500000)
//                        // total work: O(n) only
// }
//
// YOUR COMMENT: "reserve or allocate capacity = 500000
//                8*500000 = 4,000,000 bytes = 4MB allocated"
// (8 bytes per long on 64-bit system)
//
// RULE: if you know the final size, ALWAYS reserve() first!

// ============================================================
// SECTION 5: ITERATORS - THREE LOOP STYLES
// ============================================================

// STYLE 1: Index-based
// for (int i = 0; i < v.size(); i++) { v[i]; }
// -> works, but i should be size_t to avoid signed/unsigned warning
// -> for (size_t i = 0; i < v.size(); i++)

// STYLE 2: Iterator (your code)
// for (std::vector<int>::iterator it = v.begin(); it != v.end(); it++)
// { *it; }
// -> verbose type, prefer auto:
// for (auto it = v.begin(); it != v.end(); ++it) { *it; }
// -> note: prefer ++it over it++ (it++ creates a copy!)

// STYLE 3: Range-based for (MOST COMMON)
// for (const auto& elem : v) { elem; }  // read only
// for (auto& elem : v) { elem = 0; }   // modify
// for (auto elem : v) { }              // copy each element (expensive!)

// ============================================================
// SECTION 6: ERASE DANGERS (your comment warns about this)
// ============================================================

// DANGER: erasing while iterating:
// for (auto elem : myVector) {
//     myVector.erase(myVector.begin()); // UB! invalidates iterators!
//     std::cout << elem;
// }
// -> YOUR COMMENTED CODE: "what not to do"
// -> modifying vector while range-for iterates = UNDEFINED BEHAVIOR
// -> iterator invalidation!
//
// CORRECT WAY to remove elements: ERASE-REMOVE IDIOM
// std::vector<int> v = {1,2,3,4,5,2,2};
// // remove all 2s:
// v.erase(std::remove(v.begin(), v.end(), 2), v.end());
// // std::remove: moves non-2s to front, returns iterator to "junk" part
// // v.erase: actually removes the junk part
//
// CORRECT WAY to erase by index:
// v.erase(v.begin() + 2);          // erase element at index 2
// v.erase(v.begin(), v.begin()+3); // erase first 3 elements
//
// CORRECT iterator erase (must use returned iterator!):
// for (auto it = v.begin(); it != v.end(); ) {
//     if (*it == 2) it = v.erase(it); // erase returns next valid it
//     else ++it;
// }

// ============================================================
// SECTION 7: push_back vs emplace_back
// ============================================================

// push_back: constructs THEN copies/moves into vector
// emplace_back: constructs DIRECTLY in the vector (no copy/move)
//
// struct Point { int x, y; Point(int x, int y){} };
//
// v.push_back(Point(1, 2));    // constructs Point, then moves it
// v.emplace_back(1, 2);        // constructs Point DIRECTLY inside vector
//                               // passes (1,2) to Point constructor
//                               // one less move operation!
//
// FOR primitives (int, float): push_back == emplace_back (no difference)
// FOR objects: prefer emplace_back (avoids extra move/copy)

// ============================================================
// SECTION 8: SPAN AND C API INTEROP (your code)
// ============================================================

void PrintData(std::span<long> param)
// std::span<long> accepts vector<long> directly
// NO COPY: span just views vector's internal buffer
// -> prefer span over const vector& for read functions
// -> span also accepts C arrays, array<long,N> etc.
// -> more flexible API
{
    for (const auto& e : param) {
        std::cout << e << " ";
    }
}

void CStyleApi(long* data, size_t size)
// legacy C function expecting raw pointer
{
    // use data[i] or pointer arithmetic
}

// ============================================================
// SECTION 9: ITERATOR INVALIDATION - CRITICAL
// ============================================================

// Any operation that causes REALLOCATION invalidates ALL iterators:
// -> push_back (if size == capacity)
// -> insert
// -> emplace_back (if size == capacity)
// -> reserve (if new capacity > old)
// -> resize (if growing)
//
// Operations that DON'T invalidate (if no reallocation):
// -> pop_back (only invalidates end() and the removed element)
// -> erase (invalidates from erased point onwards)
//
// RULE: never store iterators to vector across operations
//       that might invalidate them!
// auto it = v.begin();
// v.push_back(5);       // MIGHT reallocate!
// *it;                  // DANGLING ITERATOR if reallocation happened

// ============================================================
// SECTION 10: DSA PATTERNS
// ============================================================

// PATTERN 1: Dynamic array with known size (reserve!)
// std::vector<int> dp(n+1, 0);      // DP table
// std::vector<bool> visited(n, false); // graph visited

// PATTERN 2: Stack (LIFO)
// std::vector<int> stk;
// stk.push_back(val);   // push
// stk.back();           // peek top
// stk.pop_back();       // pop

// PATTERN 3: Adjacency list for graphs
// std::vector<std::vector<int>> adj(n);
// adj[u].push_back(v);
// adj[v].push_back(u);

// PATTERN 4: Sorting + binary search
// std::sort(v.begin(), v.end());
// std::binary_search(v.begin(), v.end(), target); // O(log n)
// auto it = std::lower_bound(v.begin(), v.end(), target);

// PATTERN 5: Two pointers on sorted vector
// int left = 0, right = v.size()-1;
// while (left < right) {
//     int sum = v[left] + v[right];
//     if (sum == target) return {left, right};
//     else if (sum < target) left++;
//     else right--;
// }

// PATTERN 6: Flatten 2D to 1D
// std::vector<int> matrix(rows * cols, 0);
// matrix[row * cols + col] = val;  // access [row][col]

// PATTERN 7: Remove duplicates
// std::sort(v.begin(), v.end());
// v.erase(std::unique(v.begin(), v.end()), v.end());

// PATTERN 8: Prefix sums
// std::vector<int> prefix(v.size()+1, 0);
// for (int i = 0; i < v.size(); i++)
//     prefix[i+1] = prefix[i] + v[i];
// // sum of range [l,r] = prefix[r+1] - prefix[l]

// ============================================================
// SECTION 11: COMPLEXITY CHEAT SHEET
// ============================================================

// push_back(val)       O(1) amortized    O(n) worst (reallocation)
// pop_back()           O(1)
// emplace_back(args)   O(1) amortized
// insert at end        O(1) amortized
// insert at middle     O(n)              must shift elements
// erase at end         O(1)
// erase at middle      O(n)              must shift elements
// operator[]           O(1)
// at()                 O(1)
// front() back()       O(1)
// size() capacity()    O(1)
// reserve(n)           O(n)              may copy all elements
// clear()              O(n)              destroys elements
// sort (algorithm)     O(n log n)
// find (linear)        O(n)
// binary_search        O(log n)          requires sorted!

// ============================================================
// SECTION 12: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::vector and how does it resize?
//     -> Dynamic heap-allocated array. When full (size==capacity),
//        allocates ~2x new buffer, copies all elements, frees old.
//        Amortized O(1) push_back. size = elements stored,
//        capacity = elements that fit without reallocation.

// Q2: What is the difference between size() and capacity()?
//     -> size(): actual element count.
//        capacity(): allocated space (>= size).
//        push_back increases size. Reallocation increases capacity.
//        reserve() increases ONLY capacity, not size.

// Q3: Why use reserve() before a loop?
//     -> Prevents repeated reallocation. Without reserve: O(n log n)
//        total copies. With reserve: O(n) total. For 500000 elements:
//        ~20 reallocations without vs 1 with reserve.

// Q4: What is iterator invalidation in vector?
//     -> Any reallocation (push_back when full, insert, reserve)
//        invalidates ALL existing iterators and pointers.
//        Always re-obtain iterators after potentially growing operations.

// Q5: When should you use emplace_back over push_back?
//     -> For complex objects: emplace_back constructs directly in vector
//        (no temp object). push_back constructs then moves/copies.
//        For primitives (int, float): no difference.

// Q6: What is the erase-remove idiom?
//     -> std::remove moves matching elements to end, returns new end.
//        v.erase(std::remove(v.begin(),v.end(),val), v.end())
//        Two-step because std::remove doesn't actually erase from container.

// Q7: What happens to capacity after clear()?
//     -> Size becomes 0 but capacity is UNCHANGED.
//        Memory is NOT freed. Use shrink_to_fit() after clear()
//        if you want to free the memory.

// Q8: vector vs span vs const vector& for function parameters?
//     -> const vector&: only accepts vector, reference semantics.
//        span<T>: accepts vector, array, C-array, any contiguous data.
//        span<T>: more flexible and modern. Prefer span for read/write.
//        span<const T>: read-only view, most flexible parameter type.

int main()
{
    // CTAD: deduces vector<int>
    std::vector myVector{1, 2, 3, 4};
    myVector.push_back(5);

    std::cout << "size:     " << myVector.size()     << '\n'; // 5
    std::cout << "capacity: " << myVector.capacity() << '\n'; // 8 (2x growth)

    myVector.shrink_to_fit();
    std::cout << "after shrink_to_fit capacity: "
              << myVector.capacity() << '\n'; // 5 (matches size)

    // INDEX LOOP
    for (size_t i = 0; i < myVector.size(); i++) {
        std::cout << myVector[i] << " ";
    }
    std::cout << '\n';

    // ITERATOR LOOP
    for (auto it = myVector.begin(); it != myVector.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << '\n';

    // RANGE-BASED (prefer this)
    for (const auto& elem : myVector) {
        std::cout << elem << " ";
    }
    std::cout << '\n';

    // ERASE first element
    myVector.erase(myVector.begin());
    std::cout << "after erase front: ";
    for (const auto& e : myVector) std::cout << e << " ";
    std::cout << '\n'; // 2 3 4 5

    // RESERVE before large insert
    std::vector<long> bigVector;
    bigVector.reserve(500000); // ONE allocation: 8*500000 = 4MB
    for (size_t i = 0; i < 500000; i++) {
        bigVector.push_back(static_cast<long>(i));
    }
    std::cout << "bigVector size: " << bigVector.size() << '\n';

    // SPAN interop
    PrintData(bigVector);    // span accepts vector directly

    // C API interop
    CStyleApi(bigVector.data(), bigVector.size());

    // ERASE-REMOVE idiom
    std::vector<int> v = {1,2,3,2,4,2,5};
    v.erase(std::remove(v.begin(), v.end(), 2), v.end());
    std::cout << "after removing 2s: ";
    for (const auto& e : v) std::cout << e << " ";
    std::cout << '\n'; // 1 3 4 5

    return 0;
}
// ```

// **size vs capacity - the visual:**

// ```
// std::vector<int> v;
// v.push_back(1);  // size=1, cap=1  [1]
// v.push_back(2);  // size=2, cap=2  [1,2]         ← reallocate!
// v.push_back(3);  // size=3, cap=4  [1,2,3,_]     ← reallocate!
// v.push_back(4);  // size=4, cap=4  [1,2,3,4]
// v.push_back(5);  // size=5, cap=8  [1,2,3,4,5,_,_,_] ← reallocate!

// v.reserve(8);    // size=5, cap=8  (no reallocation now!)
// v.shrink_to_fit();// size=5, cap=5 (frees unused capacity)
// ```

// **The reserve() impact:**

// ```
// WITHOUT reserve(), 500000 push_backs:
// ~20 reallocations, each copies everything
// Total copies: 1+2+4+8...+500000 ≈ 1,000,000 extra copies

// WITH reserve(500000):
// 1 allocation, 0 reallocations
// Total copies: 0 extra
// ```

// > **DSA vector golden rules**: use `reserve()` when size is known, use `emplace_back` for objects, use erase-remove idiom to delete elements, never erase inside range-for, always treat iterators as invalidated after any push_back.