#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <iterator>
#include <list>
#include <array>

// ============================================================
// ITERATORS IN STL - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS AN ITERATOR?
// ============================================================

// Iterator = an object that points to an element in a container
// YOUR COMMENT: "uniform way to move through data structures"
// "behavioral design pattern describing how to move through a collection"
// "show intent for how to move through a data structure more clearly"
//
// WHY ITERATORS EXIST:
// -> each container stores data differently (array, tree, hash, linked list)
// -> WITHOUT iterators: different code to traverse each container type
// -> WITH iterators: SAME syntax for ALL containers
//
// POINTER ANALOGY:
// iterator behaves LIKE a pointer:
// *it     = dereference (get element)
// ++it    = advance to next element
// it != end = check if done
// it == other = compare positions
//
// RANGE-BASED FOR LOOP uses iterators BEHIND THE SCENES:
// for (auto& e : vec) { }
// compiler generates:
// for (auto it = vec.begin(); it != vec.end(); ++it) { auto& e = *it; }

// ============================================================
// SECTION 2: FIVE ITERATOR CATEGORIES
// ============================================================

// ITERATOR HIERARCHY (each extends the previous):
//
// 1. INPUT ITERATOR (read forward once)
//    -> dereference (read), ++, ==, !=
//    -> example: istream_iterator, single-pass reading
//
// 2. OUTPUT ITERATOR (write forward once)
//    -> dereference (write), ++
//    -> example: ostream_iterator, back_inserter
//
// 3. FORWARD ITERATOR (read/write forward, multi-pass)
//    -> all of above + can go through data multiple times
//    -> example: forward_list::iterator, unordered_map::iterator
//
// 4. BIDIRECTIONAL ITERATOR (forward + backward)
//    -> all of above + -- (decrement)
//    -> example: list::iterator, map::iterator, set::iterator
//
// 5. RANDOM ACCESS ITERATOR (jump anywhere O(1))
//    -> all of above + [], +n, -n, it1-it2, <, >, <=, >=
//    -> example: vector::iterator, array::iterator, deque::iterator
//    -> MOST POWERFUL - supports all operations
//
// WHY MATTERS FOR ALGORITHMS:
// std::sort requires RANDOM ACCESS (needs jumping)
// std::find needs only INPUT (just forward reading)
// std::reverse needs BIDIRECTIONAL (needs --)
// -> passing wrong iterator type = COMPILE ERROR (not runtime!)

// ============================================================
// SECTION 3: KEY ITERATOR FUNCTIONS
// ============================================================

// std::advance(it, n)   -> move it forward by n steps         O(n) for list
//                          O(1) for random access (vector/array)
//                          MODIFIES it in place (no return value)
//
// std::next(it, n)      -> returns NEW iterator n steps ahead O(n)
//                          does NOT modify it
//                          it = std::next(it, 3); // common pattern
//
// std::prev(it, n)      -> returns NEW iterator n steps back  O(n)
//                          requires bidirectional iterator
//
// std::distance(it1,it2)-> count elements between iterators   O(n) for list
//                          O(1) for random access
//                          YOUR CODE: std::distance(it, vec.end())
//                          = how many elements remain

// ============================================================
// SECTION 4: ITERATOR TYPES FOR EACH CONTAINER
// ============================================================

// vector<T>:        iterator, const_iterator, reverse_iterator
// array<T,N>:       iterator, const_iterator, reverse_iterator
// deque<T>:         iterator, const_iterator, reverse_iterator
// list<T>:          iterator, const_iterator, reverse_iterator (NO random access)
// forward_list<T>:  iterator, const_iterator (NO reverse, NO --)
// set<T>:           const_iterator (can't change keys!)
// map<K,V>:         iterator gives pair<const K, V>
// unordered_set:    const_iterator
// unordered_map:    iterator gives pair<const K, V>
//
// CONST ITERATORS:
// vec.cbegin(), vec.cend()   -> const_iterator (can't modify elements)
// vec.begin() on const vec   -> also gives const_iterator automatically

// ============================================================
// SECTION 5: YOUR CODE - FOUR LOOP STYLES COMPARED
// ============================================================

int main()
{
    std::vector<int> vec{1, 2, 3, 4, 5, 6};

    // STYLE 1: Raw index loop
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << ",";
    }
    // 1,2,3,4,5,6
    // -> explicit index, best when you NEED the index
    // -> only works for random access containers (vector, array)
    // -> doesn't work for list, set, map

    std::cout << '\n';

    // STYLE 2: Iterator with step (YOUR CODE - every 2nd element)
    for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); it += 2) {
        std::cout << *it << ",\n";
    }
    // prints: 1, 3, 5 (every other element)
    // it += 2 = random access jump! only works for vector/array/deque
    // PREFER: auto it = vec.begin(); (shorter, same type)

    std::cout << '\n';

    // STYLE 3: std::advance + std::distance (YOUR CODE)
    auto it = vec.begin();
    for (std::advance(it, 2); it != vec.end(); it++) {
        // std::advance(it, 2) = skip first 2 elements
        // it now points to element at index 2 (value=3)
        std::cout << "remaining: " << std::distance(it, vec.end()) << '\n';
        std::cout << *it << ",\n";
    }
    // std::advance(it, 2) -> it points to 3 (index 2)
    // std::distance(it, vec.end()) -> elements from it to end
    //   first iteration: 4 remaining (3,4,5,6)
    //   second:          3 remaining (4,5,6)
    //   etc.
    //
    // NOTE: std::advance is in the for() INIT section (runs once!)
    // it starts at index 2, increments normally after

    std::cout << '\n';

    // STYLE 4: Range-based for (CLEANEST - PREFER THIS)
    for (auto& e : vec) {
        std::cout << e << ",";
    }
    // -> compiler generates iterator loop automatically
    // -> works on ANY container with begin()/end()
    // -> const auto& for read-only (efficient, no copy)
    // -> auto& for modifying elements

    std::cout << '\n';

    // UNORDERED_MAP iteration:
    std::unordered_map<std::string, int> umap{{"one",1},{"two",2},{"Three",3}};

    for (auto& e : umap) {
        std::cout << e.first << " - " << e.second << '\n';
        // e is pair<const string, int>
        // e.first = key, e.second = value
    }
    // ORDER UNPREDICTABLE (hash-based)
    // for map (ordered): would print in sorted key order

    // C++17 STRUCTURED BINDING (even cleaner):
    for (const auto& [key, value] : umap) {
        std::cout << key << " - " << value << '\n';
    }

    return 0;
}

// ============================================================
// SECTION 6: ITERATOR OPERATIONS BY CATEGORY
// ============================================================

// AVAILABLE OPERATIONS:
// +-----------------+-------+--------+-------------+-----------+--------+
// | Operation       | Input | Output | Forward     | Bidir     | Random |
// +-----------------+-------+--------+-------------+-----------+--------+
// | *it (read)      | YES   | NO     | YES         | YES       | YES    |
// | *it (write)     | NO    | YES    | YES         | YES       | YES    |
// | ++it (pre)      | YES   | YES    | YES         | YES       | YES    |
// | it++ (post)     | YES   | YES    | YES         | YES       | YES    |
// | --it            | NO    | NO     | NO          | YES       | YES    |
// | it + n          | NO    | NO     | NO          | NO        | YES    |
// | it - n          | NO    | NO     | NO          | NO        | YES    |
// | it[n]           | NO    | NO     | NO          | NO        | YES    |
// | it1 - it2       | NO    | NO     | NO          | NO        | YES    |
// | <, >, <=, >=    | NO    | NO     | NO          | NO        | YES    |
// +-----------------+-------+--------+-------------+-----------+--------+

// ============================================================
// SECTION 7: REVERSE ITERATORS
// ============================================================

// Traverse backwards:
// vec.rbegin() -> last element
// vec.rend()   -> before first element (sentinel)
//
// for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
//     cout << *it << ",";  // prints: 6,5,4,3,2,1
// }
//
// NOTE: ++it on REVERSE iterator moves BACKWARDS (confusing but correct!)
// reverse_iterator wraps the underlying iterator and flips direction

// ============================================================
// SECTION 8: ITERATOR INVALIDATION (critical for safety)
// ============================================================

// OPERATIONS THAT INVALIDATE ITERATORS:
//
// vector: any reallocation (push_back when full, insert, resize)
//         -> ALL iterators invalid after reallocation
//
// deque: insert at middle -> all invalid
//        insert at front/back -> all invalid
//        erase at middle -> all invalid
//
// list: erase only invalidates the erased element's iterator
//       all others remain valid (never rehashes)
//
// map/set: erase only invalidates the erased element's iterator
//
// unordered_map/set: rehash invalidates ALL iterators
//
// SAFE PATTERN for erasing during iteration:
// for (auto it = vec.begin(); it != vec.end(); ) {
//     if (shouldErase(*it)) it = vec.erase(it); // erase returns next valid it
//     else ++it;
// }

// ============================================================
// SECTION 9: OUTPUT ITERATORS (std::inserter etc.)
// ============================================================

// From the generate_n session: output iterators for writing to containers
//
// std::back_inserter(v)     -> calls push_back() each write
// std::front_inserter(l)    -> calls push_front() (list/deque only)
// std::inserter(s, s.begin()) -> calls insert() (for set, map)
//
// USAGE:
// std::copy(v1.begin(), v1.end(), std::back_inserter(v2));
// std::generate_n(std::inserter(s, s.begin()), 10, gen);

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is an iterator and why does STL use them?
//     -> Object pointing to container element with pointer-like syntax.
//        Provides UNIFORM traversal across all container types.
//        Decouples algorithms from containers (sort works on any
//        random-access container without knowing container type).

// Q2: What are the five iterator categories in order?
//     -> Input, Output, Forward, Bidirectional, Random Access.
//        Each extends the previous (Random Access = most powerful).
//        Algorithms specify minimum required category.

// Q3: What is the difference between advance() and next()?
//     -> advance(it, n): modifies it IN PLACE, no return value.
//        next(it, n): returns NEW iterator, it UNCHANGED.
//        advance: for(advance(it,2); ...) = skip first 2 (YOUR CODE)
//        next: auto mid = next(v.begin(), v.size()/2);

// Q4: Why does std::sort require random access iterators?
//     -> sort needs to jump to arbitrary positions in O(1).
//        "pick element at index n" = requires it + n.
//        list has bidirectional only -> std::sort fails on list.
//        list has its own .sort() member function instead.

// Q5: What is iterator invalidation and when does it happen?
//     -> Operations that make existing iterators invalid (dangling).
//        vector: any reallocation invalidates all iterators.
//        list/set/map: only the erased element's iterator invalid.
//        Always re-obtain iterators after potentially invalidating ops.

// Q6: How does range-based for loop work internally?
//     -> Compiler generates:
//        auto&& range = vec;
//        auto it = range.begin();
//        auto end = range.end();
//        for (; it != end; ++it) { auto& e = *it; ... }
//        Works on any type with begin()/end() (including arrays).

// Q7: What is std::distance and what is its complexity?
//     -> Returns number of elements between two iterators.
//        Random access: O(1) (it2 - it1 arithmetic).
//        Others (list, map): O(n) (must traverse step by step).
//        Use sparingly on non-random-access containers.

// Q8: What is the difference between it++ and ++it?
//     -> ++it: pre-increment, advances and returns new position.
//        it++: post-increment, copies current position, then advances.
//        it++ creates a COPY (more expensive for complex iterators).
//        ALWAYS prefer ++it in for loops (no copy, same result).
// ```

// **The five iterator categories:**

// ```
// INPUT          OUTPUT         FORWARD        BIDIRECTIONAL  RANDOM ACCESS
// read once      write once     read/write     + go back      + jump anywhere
// forward only   forward only   multi-pass     (--)           ([], +n, -n, <)

// istream_iter   ostream_iter   forward_list   list           vector
//                back_inserter  unordered_map  map, set       array, deque
// ```

// **advance() vs next() - the key difference:**

// ```cpp
// auto it = vec.begin();  // points to element 0

// std::advance(it, 2);    // it ITSELF moved to element 2
//                         // no return value

// auto it2 = std::next(it, 2);  // NEW iterator 2 ahead of it
//                                 // it UNCHANGED, it2 is new
// ```

// **The four loop styles:**

// ```cpp
// // 1. Index (need the index):
// for (size_t i = 0; i < vec.size(); ++i) { vec[i]; }

// // 2. Iterator (need position for advance/insert/erase):
// for (auto it = vec.begin(); it != vec.end(); ++it) { *it; }

// // 3. Range-based (read/modify, cleanest):
// for (const auto& e : vec) { e; }          // read
// for (auto& e : vec) { e = newVal; }       // modify

// // 4. Structured binding (map/pair):
// for (const auto& [key, val] : umap) { }   // C++17
// ```

// > **The interview one-liner**: iterators are the **glue** between containers and algorithms in STL — they provide a pointer-like interface so that `std::sort`, `std::find`, and `std::copy` work on ANY compatible container without knowing the container's internal structure. The five categories (input → random access) define what operations each algorithm can use, which is why `std::sort` works on vector but not list.