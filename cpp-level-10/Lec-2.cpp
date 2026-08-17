#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <forward_list>

// ============================================================
// ITERATOR CATEGORIES - CPPREFERENCE TABLE + NOTES
// ============================================================

// ============================================================
// SECTION 1: THE TABLE EXPLAINED (from your screenshot)
// ============================================================

// The table shows WHAT each iterator category REQUIRES:
//
// LegacyOutputIterator:
// -> write: YES (Required)
// -> without multiple passes: YES (single pass write only)
// -> use: ostream_iterator, back_inserter
//
// LegacyInputIterator:
// -> read: YES
// -> without multiple passes: YES (read each element ONCE)
// -> use: istream_iterator (reading from cin once)
//
// LegacyForwardIterator:
// -> read: YES
// -> with multiple passes: YES (can iterate same data again)
// -> use: forward_list::iterator, unordered_map::iterator
//    (can traverse multiple times, only forward)
//
// LegacyBidirectionalIterator:
// -> everything Forward has PLUS
// -> decrement: YES (--, go backward)
// -> use: list::iterator, map::iterator, set::iterator
//
// LegacyRandomAccessIterator:
// -> everything Bidirectional has PLUS
// -> random access: YES (it+n, it[n], it1-it2, <, >, <=, >=)
// -> use: vector::iterator, deque::iterator, array::iterator
//
// LegacyContiguousIterator[1] (C++17):
// -> everything RandomAccess has PLUS
// -> contiguous storage: YES (elements PHYSICALLY adjacent in memory)
// -> use: vector::iterator, array::iterator, string::iterator
// -> NOT deque (deque has chunked memory, not fully contiguous)
// -> allows pointer arithmetic on the ACTUAL memory

// ============================================================
// SECTION 2: WHICH CONTAINER GIVES WHICH ITERATOR
// ============================================================

// Container          | Iterator Category      | Key restrictions
// -------------------|------------------------|------------------
// vector<T>          | Contiguous             | Full (best!)
// array<T,N>         | Contiguous             | Full
// string             | Contiguous             | Full
// deque<T>           | RandomAccess           | No contiguous
// list<T>            | Bidirectional          | No +n, no []
// map<K,V>           | Bidirectional          | No +n, no []
// set<T>             | Bidirectional          | No +n, no []
// forward_list<T>    | Forward                | No --, no +n
// unordered_map<K,V> | Forward                | No --, no +n
// unordered_set<T>   | Forward                | No --, no +n

// ============================================================
// SECTION 3: YOUR CODE - forward_list ITERATOR LIMITATIONS
// ============================================================

int main()
{
    // ---- YOUR CODE: forward_list (FORWARD ITERATOR only) ----
    std::forward_list<int> container{1, 2, 3, 4, 5, 6};
    // forward_list = singly linked list
    // iterator category = LegacyForwardIterator
    // ALLOWED: *, ++, ==, != (and multiple passes)
    // NOT ALLOWED: --, +=, [], +n, it1-it2

    auto it = container.begin();

    std::cout << *it << '\n';  // 1 (dereference - OK for Forward)
    it++;                       // advance forward - OK for Forward
    std::cout << *it << '\n';  // 2

    // it--;   // COMPILER ERROR for forward_list!
    //         // requires Bidirectional iterator
    //         // forward_list has no prev pointer (singly linked)
    //         // use list<T> instead if you need --

    // it += 3; // COMPILER ERROR for forward_list!
    //           // requires RandomAccess iterator
    //           // only vector, deque, array support this

    std::cout << *it << '\n';  // 2 (still at same position)
    std::cout << *it << '\n';  // 2

    // ---- COMMENTED CODE: vector (CONTIGUOUS ITERATOR) ----
    // std::vector<int> vec{1, 2, 3, 4, 5, 6};
    // auto it = vec.begin();
    // *it = 5;        // WRITE: OK (not const)
    // cout << *it;    // READ: OK
    // it++;           // advance: OK
    // it--;           // decrement: OK (Bidirectional+)
    // it += 3;        // jump: OK (RandomAccess+)
    // cout << *it;    // prints element at new position
    //
    // A[3] == *(A + 3) - YOUR COMMENT:
    // pointer arithmetic works same way as random access iterators
    // vec.begin() + 3 == &vec[3] (for contiguous iterators!)

    return 0;
}

// ============================================================
// SECTION 4: WHAT EACH CATEGORY CAN DO - CODE EXAMPLES
// ============================================================

// INPUT ITERATOR (read once, forward only):
// std::istream_iterator<int> it(std::cin);
// int val = *it;    // read
// ++it;             // advance
// // can't go back, can't re-read

// FORWARD ITERATOR (forward_list):
// forward_list<int> fl{1,2,3};
// auto it = fl.begin();
// *it = 5;          // write OK (mutable)
// int x = *it;      // read OK
// ++it;             // advance OK
// it++;             // post-increment OK
// // it--;          // ERROR: no --
// // it += 2;       // ERROR: no +=
// // it - other;    // ERROR: no distance

// BIDIRECTIONAL ITERATOR (list, map, set):
// list<int> l{1,2,3,4,5};
// auto it = l.begin();
// *it = 5;          // write OK
// ++it; --it;       // both directions OK
// // it += 2;       // ERROR: no random jump
// // it[2];         // ERROR: no subscript

// RANDOM ACCESS ITERATOR (vector, deque, array):
// vector<int> v{1,2,3,4,5};
// auto it = v.begin();
// *it = 5;          // write OK
// ++it; --it;       // both directions OK
// it += 3;          // jump ahead 3 OK
// it -= 2;          // jump back 2 OK
// it[2];            // subscript OK = *(it+2)
// it - v.begin();   // distance OK
// it > v.begin();   // comparison OK

// CONTIGUOUS ITERATOR (vector, array, string - C++17):
// vector<int> v{1,2,3};
// auto it = v.begin();
// // everything RandomAccess PLUS:
// &(*it) == &v[0];  // elements are physically adjacent
// int* ptr = v.data(); // can get raw pointer to contiguous block
// std::memcpy(dest, v.data(), v.size()*sizeof(int)); // memcpy safe!

// ============================================================
// SECTION 5: WHY CATEGORY MATTERS FOR ALGORITHMS
// ============================================================

// std::sort requires RANDOM ACCESS:
// std::sort(vec.begin(), vec.end()); // OK: vector is RandomAccess
// std::sort(lst.begin(), lst.end()); // COMPILE ERROR: list is Bidirectional
// -> list has .sort() member instead!

// std::reverse requires BIDIRECTIONAL:
// std::reverse(vec.begin(), vec.end()); // OK: vector is Bidirectional+
// std::reverse(fl.begin(), fl.end());   // COMPILE ERROR: forward_list is Forward only
// -> use std::forward_list::reverse() member instead

// std::find needs only INPUT:
// std::find(vec.begin(), vec.end(), val);    // OK
// std::find(lst.begin(), lst.end(), val);    // OK
// std::find(fl.begin(), fl.end(), val);      // OK
// -> forward iterator is enough for linear search!

// std::distance:
// vector<int> v{1,2,3,4,5};
// std::distance(v.begin(), v.end()); // O(1): v.end()-v.begin() (RandomAccess)
//
// list<int> l{1,2,3,4,5};
// std::distance(l.begin(), l.end()); // O(n): must traverse (Bidirectional)

// ============================================================
// SECTION 6: CONTIGUOUS ITERATOR - THE C++17 ADDITION
// ============================================================

// LegacyContiguousIterator[1] = new in C++17
// WHY NEEDED?
// -> RandomAccess didn't guarantee PHYSICAL adjacency in memory
// -> deque: RandomAccess but NOT contiguous (chunked blocks)
// -> vector, array: RandomAccess AND contiguous
//
// PRACTICAL DIFFERENCE:
// With contiguous guarantee, you can:
// int* ptr = &(*it);        // take address and do pointer arithmetic
// memcpy(dest, &v[0], n);   // safe C API call (contiguous = predictable layout)
// SIMD operations           // process multiple elements at once
//
// deque CANNOT do this safely (chunked memory, gaps between blocks)
// vector CAN do this (all elements in one contiguous block)
//
// THIS IS WHY:
// std::span works with vector/array but NOT deque
// C API functions expect contiguous pointers (vector.data() is safe)

// ============================================================
// SECTION 7: MULTIPLE PASSES - FORWARD vs INPUT
// ============================================================

// YOUR TABLE: "without multiple passes" vs "with multiple passes"
//
// WITHOUT MULTIPLE PASSES (Input/Output):
// std::istream_iterator<int> it(std::cin);
// int a = *it; ++it;   // read first element
// // can't go back to read first element again!
// // it is now "consumed" - reading from a stream is one-shot
//
// WITH MULTIPLE PASSES (Forward and above):
// std::forward_list<int> fl{1,2,3};
// auto it = fl.begin();
// int a = *it;  // read 1
// it = fl.begin();  // CAN reset and read again!
// int b = *it;  // read 1 again (multiple passes OK)
// -> Forward iterator guarantees you can iterate the SAME range multiple times

// ============================================================
// SECTION 8: PRACTICAL SUMMARY
// ============================================================

// CONTAINER CHOICE affects WHAT YOU CAN DO with iterators:
//
// Need it--?        -> Use list, map, set (not forward_list!)
// Need it+=n?       -> Use vector, array, deque (not list!)
// Need memcpy/span? -> Use vector, array (not deque, not list)
// Need fast find?   -> Use unordered containers (hash O(1))
// Need sorted?      -> Use map, set (tree-based, sorted iteration)
//
// ALWAYS prefer:
// ++it over it++    (avoids copy for complex iterators)
// cbegin/cend       (const iterators when only reading)
// range-based for   (compiler picks correct iterator automatically)
// auto it           (less typing, correct type always)

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a LegacyContiguousIterator and how is it different
//     from LegacyRandomAccessIterator?
//     -> Contiguous adds the guarantee that elements are PHYSICALLY
//        adjacent in memory (like a C array). RandomAccess allows
//        O(1) jumping but doesn't guarantee physical adjacency.
//        vector, array = Contiguous. deque = RandomAccess but NOT Contiguous.

// Q2: Why does forward_list not support -- operator?
//     -> Singly linked list: each node has only a 'next' pointer.
//        No 'prev' pointer means going backward requires traversing
//        from head = O(n). C++ chose not to include it to enforce
//        that iterators are O(1). Use std::list for bidirectional.

// Q3: What does "multiple passes" mean in iterator categories?
//     -> Can iterate the same range more than once without reinitializing.
//        Input iterator (like istream): once read, can't go back.
//        Forward and above: can reset to begin() and re-traverse.

// Q4: Why can't std::sort work on list but works on vector?
//     -> sort requires RandomAccess (needs it+n, it1-it2).
//        list provides only Bidirectional (no +n, no distance comparison).
//        list has .sort() member function that uses merge sort (O(n log n)
//        without random access) internally.

// Q5: What is the advantage of Contiguous over RandomAccess?
//     -> Can use with C APIs expecting raw pointers (memcpy, etc.).
//        Enables SIMD/vectorization (compiler can process multiple elements).
//        std::span works with Contiguous containers (vector/array).
//        Allows safe data() pointer arithmetic.

// Q6: Which containers provide each iterator category?
//     -> Contiguous:    vector, array, string
//        RandomAccess:  deque + all Contiguous
//        Bidirectional: list, map, set, multimap, multiset + above
//        Forward:       forward_list, unordered_map/set + above
//        Input/Output:  stream iterators (single pass)

// Q7: What happens if you use the wrong iterator category?
//     -> COMPILE ERROR (not runtime). Template constraints catch it.
//        std::sort(list.begin(), list.end()) = compile error.
//        Safer than C-style arrays where wrong pointer arithmetic
//        gives UB without any error.

// Q8: Why is ++it preferred over it++ in for loops?
//     -> it++ creates a COPY of the iterator, advances it, returns copy.
//        ++it advances in place, returns reference to it.
//        For complex iterators: extra copy = unnecessary allocation.
//        For simple iterators (raw pointer): compiler optimizes away.
//        Always write ++it as good habit.
// ```

// **The table from your screenshot - simplified:**

// ```
// Category           | write | read | -- | +n | contiguous
// -------------------|-------|------|----|----|----------
// OutputIterator     |  ✅   |      |    |    |
// InputIterator      |       |  ✅  |    |    |
// ForwardIterator    |  ✅   |  ✅  |    |    |   ← forward_list, unordered_map
// BidirectionalIter  |  ✅   |  ✅  | ✅ |    |   ← list, map, set
// RandomAccessIter   |  ✅   |  ✅  | ✅ | ✅ |   ← deque
// ContiguousIter     |  ✅   |  ✅  | ✅ | ✅ | ✅ ← vector, array, string
// ```

// **Your forward_list code - what works and what doesn't:**

// ```cpp
// std::forward_list<int> fl{1,2,3,4,5};
// auto it = fl.begin();

// *it;        // ✅ read
// *it = 5;    // ✅ write
// ++it;       // ✅ advance
// it++;       // ✅ post-increment
// it != fl.end(); // ✅ compare

// it--;       // ❌ COMPILE ERROR (no prev pointer in singly-linked)
// it += 3;    // ❌ COMPILE ERROR (no random jump)
// it[2];      // ❌ COMPILE ERROR (no subscript)
// it - fl.begin(); // ❌ COMPILE ERROR (no distance)
// ```

// > **The key insight**: iterator categories are a **compile-time contract**. If you write `std::sort(list.begin(), list.end())`, it fails at compile time because the compiler checks that `list::iterator` satisfies `RandomAccess`. This is safer than runtime failures — the wrong algorithm choice is caught before your program even runs.