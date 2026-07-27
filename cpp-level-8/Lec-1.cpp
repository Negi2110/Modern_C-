// ============================================================
// STL - STANDARD TEMPLATE LIBRARY - COMPLETE OVERVIEW NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS THE STL?
// ============================================================

// STL = Standard Template Library
// Three core components working together:
//
// 1. CONTAINERS  = data structures that store elements
// 2. ALGORITHMS  = operations performed ON those elements
// 3. ITERATORS   = the GLUE between containers and algorithms
//
// WHY this separation?
// -> std::sort doesn't need to know if it's sorting a vector or deque
// -> std::find works on any container (array, list, set, map)
// -> NEW container + EXISTING algorithms = instant compatibility
// -> NEW algorithm + EXISTING containers = instant compatibility
// -> "write once, works everywhere" via iterator abstraction
//
// YOUR COMMENT: "a pair of iterators makes a range"
// -> MOST algorithms take: (begin, end) = a RANGE of elements
// -> begin = iterator to FIRST element
// -> end   = iterator to ONE PAST LAST element [begin, end)
// -> half-open range [begin, end) = standard C++ convention

// ============================================================
// SECTION 2: CONTAINERS - COMPLETE OVERVIEW
// ============================================================

// ---- SEQUENCE CONTAINERS (order you insert = order stored) ----

// std::vector<T>
// -> dynamic array (contiguous memory)
// -> O(1) random access by index
// -> O(1) amortized push_back (end)
// -> O(n) insert/delete in middle (must shift elements)
// -> MOST USED container in C++ (default choice)
// -> USE WHEN: need indexing, mostly add/remove at end
//
// std::array<T, N>
// -> fixed-size array (stack allocated)
// -> O(1) random access
// -> size known at compile time (template parameter)
// -> no heap allocation = fastest option
// -> USE WHEN: size fixed at compile time, performance critical
//
// std::deque<T>
// -> double-ended queue
// -> O(1) push_front AND push_back (unlike vector)
// -> O(1) random access (but slower than vector)
// -> not contiguous memory (chunked)
// -> USE WHEN: need fast insertion at BOTH ends
//
// std::list<T>
// -> doubly-linked list
// -> O(1) insert/delete ANYWHERE (with iterator)
// -> O(n) random access (must traverse from head)
// -> no contiguous memory = cache unfriendly
// -> USE WHEN: frequent insert/delete in middle, rarely index
//
// std::forward_list<T>
// -> singly-linked list (only forward traversal)
// -> less memory than list (one pointer per node)
// -> USE WHEN: memory critical, only need forward traversal

// ---- ASSOCIATIVE CONTAINERS (sorted by key) ----

// std::set<T>
// -> sorted unique elements (no duplicates)
// -> O(log n) insert, find, delete
// -> internally: Red-Black Tree
// -> USE WHEN: need sorted unique collection, fast lookup
//
// std::multiset<T>
// -> sorted, allows duplicates
// -> same as set but multiple same values allowed
//
// std::map<Key, Value>
// -> sorted key-value pairs, unique keys
// -> O(log n) insert, find, delete
// -> internally: Red-Black Tree
// -> USE WHEN: dictionary/lookup table, need sorted keys
//
// std::multimap<Key, Value>
// -> sorted, multiple values per key

// ---- UNORDERED CONTAINERS (hash-based, C++11) ----

// std::unordered_set<T>
// -> unsorted unique elements
// -> O(1) average insert, find, delete (hash table)
// -> O(n) worst case (hash collision)
// -> USE WHEN: fast lookup, don't need sorted order
//
// std::unordered_map<Key, Value>
// -> unsorted key-value pairs, unique keys
// -> O(1) average operations
// -> USE WHEN: fast dictionary/lookup, order doesn't matter

// ---- CONTAINER ADAPTORS (built on other containers) ----

// std::stack<T>
// -> LIFO (Last In First Out)
// -> push(), pop(), top()
// -> built on deque by default
//
// std::queue<T>
// -> FIFO (First In First Out)
// -> push(), pop(), front(), back()
// -> built on deque by default
//
// std::priority_queue<T>
// -> always gives you LARGEST element first (max-heap by default)
// -> push(), pop(), top()
// -> USE WHEN: always need highest priority element

// ============================================================
// SECTION 3: ITERATORS - THE GLUE
// ============================================================

// Iterator = object that points to an element in a container
// behaves LIKE a pointer: dereference (*it), advance (++it), compare (it != end)
//
// FIVE ITERATOR CATEGORIES (from weakest to strongest):
//
// 1. INPUT ITERATOR      = read once, forward only (e.g., istream_iterator)
// 2. OUTPUT ITERATOR     = write once, forward only (e.g., ostream_iterator)
// 3. FORWARD ITERATOR    = read/write, forward only (e.g., forward_list)
// 4. BIDIRECTIONAL       = forward AND backward (e.g., list, map, set)
// 5. RANDOM ACCESS       = jump anywhere O(1) (e.g., vector, array, deque)
//
// STRONGER iterator can do everything weaker one can + more
// Algorithms specify which MINIMUM category they need:
// std::sort requires RANDOM ACCESS (needs O(1) jumping)
// std::find needs only INPUT (just forward reading)
//
// ITERATOR OPERATIONS:
// *it         = dereference (get element)
// ++it        = advance to next (prefix, preferred)
// it++        = advance (postfix, creates copy - less efficient)
// it--        = go back (bidirectional and above)
// it + n      = jump forward n steps (random access only)
// it[n]       = same as *(it+n) (random access only)
// it1 - it2   = distance between iterators (random access only)
// it1 == it2  = compare positions
// it1 != it2  = compare positions

// ============================================================
// SECTION 4: HOW THEY ALL FIT TOGETHER
// ============================================================

// ALGORITHM + ITERATOR + CONTAINER = STL pattern:
//
// std::vector<int> v = {5, 3, 1, 4, 2};
//
// [CONTAINER]        [ITERATOR RANGE]     [ALGORITHM]
// std::vector<int>   v.begin(), v.end()   std::sort
//
// std::sort(v.begin(), v.end());
// -> sort doesn't know it's a vector
// -> only knows: "I have iterators, I can dereference and swap"
// -> works IDENTICALLY with any random-access container
//
// std::sort(arr, arr+5);         // raw array (pointers ARE iterators!)
// std::sort(v.begin(), v.end()); // vector
// std::sort(d.begin(), d.end()); // deque
// -> SAME algorithm, different containers, just works

// ============================================================
// SECTION 5: COMMON ALGORITHMS
// ============================================================

// #include <algorithm>

// SEARCHING:
// std::find(begin, end, value)         -> iterator to element or end
// std::find_if(begin, end, predicate)  -> first matching predicate
// std::binary_search(begin, end, val)  -> bool (requires sorted!)
// std::count(begin, end, value)        -> count occurrences
// std::count_if(begin, end, pred)      -> count matching predicate

// SORTING:
// std::sort(begin, end)                -> O(n log n), random access needed
// std::sort(begin, end, comparator)    -> custom order
// std::stable_sort(begin, end)         -> preserves equal element order
// std::partial_sort(begin, mid, end)   -> sorts first N elements

// TRANSFORMATION:
// std::transform(begin, end, out, op)  -> apply op, store in out
// std::for_each(begin, end, func)      -> call func on each element
// std::fill(begin, end, value)         -> fill range with value
// std::copy(begin, end, out)           -> copy range to out

// REORDERING:
// std::reverse(begin, end)             -> reverse the range
// std::rotate(begin, middle, end)      -> rotate elements
// std::shuffle(begin, end, rng)        -> random shuffle

// REMOVAL:
// std::remove(begin, end, value)       -> moves non-matching to front
//                                         returns new logical end
// std::unique(begin, end)              -> removes consecutive duplicates
// (erase-remove idiom for actual removal from container)

// NUMERIC (#include <numeric>):
// std::accumulate(begin, end, init)    -> sum (or custom op)
// std::iota(begin, end, start)         -> fill with incrementing values
// std::inner_product(...)              -> dot product

// ============================================================
// SECTION 6: RANGES (C++20) - THE EVOLUTION
// ============================================================

// C++20 Ranges = modernized STL
// -> algorithms take containers directly (not begin/end pairs)
// -> views (lazy evaluation, no copies)
// -> pipe syntax for chaining operations
//
// OLD STL style:
// std::sort(v.begin(), v.end());
// std::for_each(v.begin(), v.end(), print);
//
// C++20 RANGES style:
// std::ranges::sort(v);              // no begin/end needed!
// std::ranges::for_each(v, print);   // cleaner
//
// VIEWS (lazy, no copies):
// auto evens = v | std::views::filter([](int x){ return x%2==0; })
//                | std::views::transform([](int x){ return x*2; });
// -> NO computation yet (lazy)
// -> only computes when you iterate!

// ============================================================
// SECTION 7: CONTAINER CHOICE DECISION GUIDE
// ============================================================

// Need fast random access by INDEX?
//   -> std::vector (99% of cases) or std::array (fixed size)
//
// Need fast insert/delete at BOTH ENDS?
//   -> std::deque
//
// Need fast insert/delete ANYWHERE, no random access?
//   -> std::list (rarely - cache unfriendly, prefer vector usually)
//
// Need UNIQUE SORTED elements?
//   -> std::set
//
// Need KEY-VALUE mapping with SORTED keys?
//   -> std::map
//
// Need KEY-VALUE mapping, ORDER doesn't matter?
//   -> std::unordered_map (faster O(1) vs O(log n))
//
// Need LIFO (stack behavior)?
//   -> std::stack
//
// Need FIFO (queue behavior)?
//   -> std::queue
//
// Need always get HIGHEST PRIORITY element?
//   -> std::priority_queue

// ============================================================
// SECTION 8: YOUR ISSST/DIS CONTEXT
// ============================================================

// Likely containers in DIS simulation:
//
// Entity tracking:
// std::unordered_map<EntityID, EntityState>  // fast lookup by ID O(1)
// std::map<EntityID, EntityState>            // sorted, ordered iteration
//
// PDU buffer:
// std::vector<uint8_t>                       // raw bytes, contiguous
// std::deque<PDU>                            // queue of incoming PDUs
//
// Active exercises:
// std::set<uint16_t>                         // unique exercise IDs
// std::unordered_set<uint16_t>               // faster if order not needed
//
// Event queue (priority-based):
// std::priority_queue<SimEvent>              // highest priority event first

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What are the three components of the STL?
//     -> Containers (data structures), Algorithms (operations),
//        Iterators (abstraction for accessing containers, glue between them).

// Q2: What is a range in STL?
//     -> A pair of iterators [begin, end) defining a sequence of elements.
//        Half-open: begin points to first element, end points one past last.
//        All STL algorithms operate on ranges.

// Q3: What is the difference between std::vector and std::list?
//     -> vector: contiguous memory, O(1) random access, O(n) insert middle.
//        list: linked nodes, O(1) insert anywhere (with iterator), O(n) access.
//        vector is almost always preferred due to cache efficiency.

// Q4: What is the difference between std::map and std::unordered_map?
//     -> map: sorted by key (Red-Black Tree), O(log n) operations.
//        unordered_map: unsorted (hash table), O(1) average operations.
//        Use map when you need sorted iteration or O(log n) is fine.
//        Use unordered_map when you need fastest possible lookup.

// Q5: Why do most STL algorithms take begin/end instead of the container?
//     -> Separation of concerns. Algorithms don't need to know the
//        container type, only that they have iterators. Same algorithm
//        works on any container providing compatible iterators.

// Q6: What iterator category does std::sort require?
//     -> Random Access Iterator (can jump to any position in O(1)).
//        This is why sort works on vector/array/deque but NOT on list.
//        std::list has its own sort() member function instead.

// Q7: What is the erase-remove idiom?
//     -> std::remove doesn't actually remove (moves elements to end).
//        Must combine: v.erase(std::remove(v.begin(),v.end(),val), v.end())
//        remove returns iterator to new logical end, erase deletes the rest.

// Q8: What is the default container for std::stack and std::queue?
//     -> Both use std::deque by default.
//        stack<int> = stack<int, deque<int>> internally.
//        Can customize: stack<int, vector<int>> to use vector instead.

#include <iostream>
int main()
{
    // STL overview lesson - no direct code to run
    // See examples throughout the notes above
    return 0;
}
// ```

// **The STL triangle - how all three work together:**

// ```
// CONTAINERS          ITERATORS           ALGORITHMS
// (store data)  ←──→  (the glue)  ←──→  (process data)

// vector<int> v = {3,1,2};
//               ↓
// std::sort(v.begin(), v.end());
//               ↑              ↑
//          iterator        iterator
//          to start        to end

// Algorithm doesn't know it's a vector.
// Container doesn't know it's being sorted.
// Iterator is the ONLY interface between them.
// ```

// **Container decision in 10 seconds:**

// ```
// Default choice?          → std::vector
// Fixed size?              → std::array
// Fast both-end insert?    → std::deque
// Key-value, sorted?       → std::map
// Key-value, fast lookup?  → std::unordered_map
// Unique sorted values?    → std::set
// LIFO?                    → std::stack
// FIFO?                    → std::queue
// Always get max/min?      → std::priority_queue
// ```

// > **The interview soundbite**: *"The STL's power comes from the fact that containers, algorithms, and iterators are completely independent. std::sort works on a vector, a deque, a raw array — anything with random access iterators — without knowing what it's sorting. This is generic programming: write the algorithm once, use it everywhere."*