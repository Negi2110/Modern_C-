#include <iostream>
#include <vector>
#include <list>
#include <forward_list>
#include <array>

// ============================================================
// STL RANGE ACCESS - NON-MEMBER FUNCTIONS - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT ARE NON-MEMBER RANGE ACCESS FUNCTIONS?
// ============================================================

// C++11 added FREE FUNCTIONS (non-member) for range access:
// std::begin(c)   instead of c.begin()
// std::end(c)     instead of c.end()
// std::size(c)    instead of c.size()
// std::data(c)    instead of c.data()
// std::empty(c)   instead of c.empty()
//
// WHY NON-MEMBER VERSIONS?
// -> WORKS WITH C-STYLE ARRAYS (which have no member functions!)
// -> int arr[5] = {1,2,3,4,5};
//    arr.begin()         // COMPILE ERROR: arrays have no methods
//    std::begin(arr)     // WORKS! (C++11)
//    std::size(arr)      // WORKS! returns 5
//    std::data(arr)      // WORKS! returns pointer to arr[0]
//
// -> Same code works for BOTH STL containers AND raw arrays:
//    template<typename Container>
//    void print(const Container& c) {
//        for (auto it = begin(c); it != end(c); ++it)
//            cout << *it << ",";
//    }
//    print(vec);    // vector - calls vec.begin()
//    print(arr);    // C array - calls std::begin(arr) specialization
//    print(list);   // list  - calls list.begin()

// ============================================================
// SECTION 2: ALL NON-MEMBER RANGE ACCESS FUNCTIONS
// ============================================================

// begin(c)   -> iterator to first element         (modifiable)
// end(c)     -> iterator past last element        (modifiable)
// cbegin(c)  -> CONST iterator to first           (read-only!)
// cend(c)    -> CONST iterator past last          (read-only!)
// rbegin(c)  -> REVERSE iterator to LAST element  (modifiable)
// rend(c)    -> REVERSE iterator before first     (modifiable)
// crbegin(c) -> CONST REVERSE iterator to last    (read-only!)
// crend(c)   -> CONST REVERSE past first          (read-only!)
// size(c)    -> number of elements                (C++17)
// empty(c)   -> true if empty                     (C++17)
// data(c)    -> raw pointer to underlying array   (C++17)
//
// NAMING PATTERN:
// c = const version (read-only)
// r = reverse version (backward traversal)
// cr = const AND reverse

// ============================================================
// SECTION 3: YOUR COMMENT - cbegin vs begin
// ============================================================

// YOUR COMMENT: "read only more because of 'c-begin' = const"
// CORRECT!
//
// begin(c)  -> iterator (can READ and WRITE)
// cbegin(c) -> const_iterator (can READ only, CANNOT write)
//
// EXAMPLE:
// auto it  = begin(container);    // vector<int>::iterator
// *it = 99;                        // OK: can modify
//
// auto cit = cbegin(container);   // vector<int>::const_iterator
// *cit = 99;                       // COMPILE ERROR: read-only!
// std::cout << *cit;               // OK: can read
//
// WHY USE cbegin?
// -> communicates INTENT: "I only need to read, not modify"
// -> compiler enforces it (can't accidentally modify)
// -> same as function taking const& instead of &

// ============================================================
// SECTION 4: YOUR COMMENT - rbegin (reverse iterator)
// ============================================================

// YOUR COMMENT: "reverse iterator points to the last one"
// CORRECT!
//
// vector {1, 2, 3, 4, 5, 6}
//         ^                ^
//      begin()          rbegin() <- points to 6 (last element)
//      cbegin()         crbegin() <- const version of same
//
// begin():  1, 2, 3, 4, 5, 6   (forward, ++ goes right)
// rbegin(): 6, 5, 4, 3, 2, 1   (reverse, ++ goes LEFT!)
//
// CONFUSING PART: ++ on REVERSE ITERATOR moves BACKWARD:
// auto rit = rbegin(vec);  // points to 6
// rit++;                    // now points to 5 (moved left!)
// rit++;                    // now points to 4
//
// YOUR CODE: crbegin = CONST + REVERSE
// -> starts at last element (6)
// -> ++ moves toward first element
// -> cannot modify elements (*it = 7 would COMPILE ERROR)

// ============================================================
// SECTION 5: YOUR CODE TRACE
// ============================================================

int main()
{
    std::vector<int> container{1, 2, 3, 4, 5, 6};
    //  Indices:                0  1  2  3  4  5

    // crbegin = CONST REVERSE iterator to LAST element
    auto it = crbegin(container);
    // it points to: 6 (index 5, last element)
    // type: vector<int>::const_reverse_iterator

    std::cout << *it << '\n';  // 6 (last element)
    it++;                       // moves BACKWARD (to index 4)
    std::cout << *it << '\n';  // 5
    it++;                       // moves BACKWARD (to index 3)
    std::cout << *it << '\n';  // 4
    it++;                       // moves BACKWARD (to index 2)
    std::cout << *it << '\n';  // 3
    // OUTPUT: 6, 5, 4, 3

    // *it = 7;  // COMPILE ERROR! crbegin = const, cannot modify

    std::cout << "Size: " << size(container) << '\n';
    // std::size(container) = 6
    // C++17 non-member function - same as container.size()
    // WORKS ON: STL containers AND C arrays!

    // data() = pointer to underlying raw array
    auto mydata = data(container);
    // mydata = int* pointing to container's first element
    // same as container.data() or &container[0]
    // ONLY works for CONTIGUOUS containers (vector, array, string)
    // NOT for list, deque (non-contiguous memory)

    for (size_t i = 0; i < size(container); i++) {
        std::cout << mydata[i] << '\n';  // raw pointer array access
    }
    // prints: 1,2,3,4,5,6
    // mydata[i] = same as container[i] (same memory!)

    return 0;
}

// ============================================================
// SECTION 6: WHERE NON-MEMBER FUNCTIONS SHINE - C ARRAY
// ============================================================

// int arr[5] = {1, 2, 3, 4, 5};
//
// arr.begin()  // COMPILE ERROR - arrays have no methods!
// arr.size()   // COMPILE ERROR
// arr.data()   // COMPILE ERROR
//
// std::begin(arr)  // OK! returns int* (pointer to first element)
// std::end(arr)    // OK! returns int* (pointer PAST last = arr+5)
// std::size(arr)   // OK! returns 5 (compile-time constant!)
// std::data(arr)   // OK! returns int* (= arr itself)
//
// GENERIC FUNCTION that works for BOTH:
// template<typename Container>
// void printAll(const Container& c) {
//     for (auto it = std::cbegin(c); it != std::cend(c); ++it)
//         std::cout << *it << " ";
// }
//
// printAll(vec);        // works with vector
// int arr[5]{1,2,3,4,5};
// printAll(arr);        // works with C array!
// printAll(myList);     // works with list!

// ============================================================
// SECTION 7: COMPLETE ITERATOR VARIANT TABLE
// ============================================================

// Function    | Const? | Direction | Starts at
// ------------|--------|-----------|----------
// begin()     | NO     | forward   | first element
// end()       | NO     | forward   | past last
// cbegin()    | YES    | forward   | first element
// cend()      | YES    | forward   | past last
// rbegin()    | NO     | reverse   | LAST element
// rend()      | NO     | reverse   | before first
// crbegin()   | YES    | reverse   | LAST element ← YOUR CODE
// crend()     | YES    | reverse   | before first
//
// VISUAL for {1,2,3,4,5,6}:
//
//   begin()                         end()
//   cbegin()                        cend()
//     ↓                               ↓
//    [1] [2] [3] [4] [5] [6]   (gap)
//                               ↑
//                              rend()
//                            crend()
//                                [6] [5] [4] [3] [2] [1]
//                                 ↑
//                              rbegin()
//                             crbegin() ← YOUR CODE starts here

// ============================================================
// SECTION 8: data() AND size() - NON-MEMBER VERSIONS
// ============================================================

// std::data(c):
// -> returns pointer to underlying array (for contiguous containers)
// -> vector<int>: returns int* to first element (same as &c[0])
// -> string:      returns const char* (same as .c_str())
// -> C array:     returns pointer to first element (same as arr itself)
// -> NO for: list, deque (not contiguous - no single underlying array)
//
// USE CASE: pass vector to C API expecting raw pointer
// std::vector<int> v{1,2,3,4,5};
// c_function(std::data(v), std::size(v));  // clean C interop
// // vs older style: c_function(v.data(), v.size());  // same result
//
// std::size(c):
// -> C++17
// -> for STL containers: calls c.size()
// -> for C arrays: returns compile-time array size (constexpr!)
//    int arr[5]; std::size(arr) = 5 (known at compile time!)
//    vs sizeof(arr)/sizeof(arr[0]) = old way to get C array size
//
// std::empty(c):
// -> C++17
// -> for STL containers: calls c.empty()
// -> for C arrays: returns true if size == 0 (rarely useful for arrays)
// -> for initializer_list: checks size

// ============================================================
// SECTION 9: MEMBER vs NON-MEMBER COMPARISON
// ============================================================

// For STL containers, IDENTICAL behavior:
// c.begin()   == std::begin(c)      // same iterator
// c.end()     == std::end(c)
// c.size()    == std::size(c)       // C++17
// c.data()    == std::data(c)       // C++17
// c.empty()   == std::empty(c)      // C++17
//
// For C-STYLE ARRAYS, only non-member works:
// arr.begin()   // ERROR
// std::begin(arr) // OK
//
// PREFER NON-MEMBER when writing GENERIC code (templates)
// MEMBER is fine for specific container code

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: Why do non-member begin/end/size functions exist?
//     -> To work with C-style arrays which have no member functions.
//        Also enables generic template code that works with
//        both STL containers and C arrays using same syntax.

// Q2: What is the difference between begin() and cbegin()?
//     -> begin(): returns mutable iterator (can read AND write elements).
//        cbegin(): returns const_iterator (can ONLY read, not modify).
//        Use cbegin() when you only need to read (communicates intent).

// Q3: What does crbegin() do and what does ++ do on it?
//     -> Returns const reverse_iterator pointing to LAST element.
//        ++ moves TOWARD first element (backward through container).
//        const = cannot modify elements. reverse = starts from end.

// Q4: What does std::data() return and when can you use it?
//     -> Raw pointer to underlying contiguous memory.
//        Only for contiguous containers: vector, array, string.
//        NOT for list, deque (non-contiguous).
//        Used for C API interop needing raw pointer.

// Q5: What does std::size() return for a C-style array?
//     -> Compile-time size of the array (constexpr).
//        std::size(int arr[5]) = 5.
//        Cleaner than sizeof(arr)/sizeof(arr[0]) old idiom.

// Q6: How does a reverse iterator's ++ work?
//     -> Moves toward first element (backward).
//        rbegin() points to last, rbegin()++ points to second-to-last.
//        Internally wraps a forward iterator and decrements it.
//        Confusing: ++ looks like forward but moves backward.

// Q7: When would you use crbegin() specifically?
//     -> When you need to iterate BACKWARD through a container
//        in READ-ONLY mode. Common for: checking palindromes,
//        printing in reverse without modifying, binary search from end.

// Q8: Can you use std::size() on a std::forward_list?
//     -> NO! forward_list has no .size() method (by design - would
//        require O(n) traversal and forward_list avoids storing size).
//        std::size() calls .size() internally, so it fails too.
//        Use std::distance(fl.begin(), fl.end()) instead O(n).
// ```

// **All eight iterator access functions visualized:**

// ```
// container: {1, 2, 3, 4, 5, 6}

// FORWARD:
// begin()/cbegin()  →  1  2  3  4  5  6  ←  end()/cend()
//                      ↑                      (past last)
//                   starts here, ++ goes RIGHT

// REVERSE:
// rend()/crend()   ←  6  5  4  3  2  1  ←  rbegin()/crbegin()
// (before first)                              ↑ starts here
//                      ++ goes LEFT (toward rend)
// ```

// **Non-member vs C array:**

// ```cpp
// int arr[5] = {1,2,3,4,5};
// std::vector<int> vec{1,2,3,4,5};

// // C array (no methods):           // vector (has methods):
// std::begin(arr)  // ✅ works       vec.begin()  // ✅ works
// arr.begin()      // ❌ ERROR       std::begin(vec) // ✅ also works
// std::size(arr)   // ✅ = 5        vec.size()   // ✅ = 5
// arr.size()       // ❌ ERROR      std::size(vec)  // ✅ = 5
// ```

// > **The design insight**: non-member `begin/end/size/data` exist to make C-style arrays first-class citizens in generic code. Without them, you'd need template specializations for arrays vs containers. With them, `template<typename C> void process(C& c)` works identically for `vector`, `array`, `list`, and even `int arr[5]` — same syntax, correct behavior for each.