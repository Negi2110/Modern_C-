#include <iostream>
#include <set>
#include <algorithm>

// ============================================================
// std::set - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::set<T> = sorted collection of UNIQUE elements
// YOUR COMMENTS:
// "associative container that contains a sorted set of unique objects"
// "ordered data structure for adding unique elements"
// "implemented as Red-Black Tree (rb-tree)"
// "key is to use this structure to CHECK FOR EXISTENCE of data"
//
// RED-BLACK TREE PROPERTIES:
// -> self-balancing binary search tree
// -> height always O(log n) - guaranteed balanced
// -> every operation: O(log n) worst case (no worst-case like hash)
// -> elements stored IN SORTED ORDER always
// -> in-order traversal = ascending order automatically
//
// INTERNAL STRUCTURE (each node):
// struct Node {
//     T value;
//     Node* left;
//     Node* right;
//     Node* parent;
//     Color color;    // RED or BLACK
// };
//
// KEY GUARANTEE: sorted + unique + O(log n) operations

// ============================================================
// SECTION 2: set vs unordered_set vs vector (when to use what)
// ============================================================

// | Feature              | set       | unordered_set | vector    |
// |----------------------|-----------|---------------|-----------|
// | Sorted               | YES       | NO            | depends   |
// | Unique elements      | YES       | YES           | NO        |
// | Find                 | O(log n)  | O(1) avg      | O(n)      |
// | Insert               | O(log n)  | O(1) avg      | O(1) amort|
// | Delete               | O(log n)  | O(1) avg      | O(n)      |
// | Memory               | tree nodes| hash table    | contiguous|
// | Iterator invalidated | NEVER     | on rehash     | on realloc|
// | Ordered iteration    | YES       | NO            | depends   |
// | Lower/upper bound    | O(log n)  | NO            | O(log n)* |
//
// *only if sorted
//
// USE set WHEN:
// -> need SORTED unique elements
// -> need range queries (lower_bound, upper_bound)
// -> need ordered iteration
// -> need GUARANTEED O(log n) (no hash collision worst case)
//
// USE unordered_set WHEN:
// -> only need existence check (contains/find)
// -> order doesn't matter
// -> want FASTER average O(1) operations

// ============================================================
// SECTION 3: CONSTRUCTION
// ============================================================

// std::set<int> s;                    // empty set
// std::set<int> s{1,2,3,4};           // initializer list (dupes removed)
// std::set<int> s{1,1,2,3,4,4};       // YOUR CODE - {1,2,3,4} stored
// std::set<int> s2(s);                // copy
// std::set<int> s3(v.begin(),v.end()); // from range (removes dupes)
//
// CUSTOM COMPARATOR (sort in different order):
// std::set<int, std::greater<int>> descSet; // descending order
// std::set<string, [](string a, string b){ return a.size() < b.size(); }> // by length

// ============================================================
// SECTION 4: IMPORTANT METHODS WITH COMPLEXITY
// ============================================================

// INSERT:
// s.insert(val)          -> insert val, returns pair<iterator,bool>  O(log n)
//                           pair.second = true if inserted (new)
//                           pair.second = false if already existed
// s.emplace(args)        -> construct in-place                        O(log n)
//
// REMOVE:
// s.erase(val)           -> remove by VALUE                           O(log n)
// s.erase(it)            -> remove by ITERATOR                        O(1) amort
// s.erase(it1, it2)      -> remove range [it1, it2)                  O(n)
// s.clear()              -> remove all                                O(n)
//
// SEARCH (THE MAIN PURPOSE):
// s.find(val)            -> iterator to val, or end() if not found    O(log n)
// s.contains(val)        -> bool (C++20) - CLEANEST way              O(log n)
// s.count(val)           -> 0 or 1 (always! unique elements)          O(log n)
//
// YOUR QUESTION: "why count when we have only unique numbers?"
// -> count() in SET always returns 0 or 1 (by design)
// -> exists for INTERFACE CONSISTENCY with multiset
//    (multiset allows dupes, count() returns how many)
// -> prefer contains() (C++20) or find() != end() for clarity
// -> count(val) == 1 means "exists", count(val) == 0 means "not found"
//
// RANGE QUERIES (unique to ordered containers!):
// s.lower_bound(val)     -> iterator to FIRST element >= val         O(log n)
// s.upper_bound(val)     -> iterator to FIRST element > val          O(log n)
//
// SIZE:
// s.size()               -> element count                            O(1)
// s.empty()              -> true if empty                            O(1)
// s.max_size()           -> theoretical max                          O(1)
//
// MERGE:
// s1.merge(s2)           -> move elements from s2 into s1            O(n log n)
//                           elements already in s1 stay in s2
//                           YOUR CODE: non-duplicate elements moved

// ============================================================
// SECTION 5: YOUR QUESTION - count() vs find() vs contains()
// ============================================================

// THREE WAYS to check existence in set:
//
// 1. count(val) - returns 0 or 1 for set:
//    if (s.count(val)) { /* exists */ }
//    DRAWBACK: name implies counting, misleading for set
//
// 2. find(val) - returns iterator:
//    if (s.find(val) != s.end()) { /* exists */ }
//    BENEFIT: you get the iterator (can use for erase!)
//    DRAWBACK: more verbose
//
// 3. contains(val) - C++20, returns bool:
//    if (s.contains(val)) { /* exists */ }
//    BEST: clearest intent, most readable
//    YOUR CODE uses this!
//
// PERFORMANCE: all three are O(log n) for set

// ============================================================
// SECTION 6: lower_bound and upper_bound EXPLAINED
// ============================================================

// s2 = {4, 6, 8, 10, 12, 14}
//
// s2.lower_bound(8):
// -> first element >= 8
// -> points to 8 (inclusive)
//
// s2.upper_bound(14):
// -> first element > 14
// -> points to end() (14 is the last, nothing > 14)
//
// YOUR LOOP: for (auto it = lower; it != higher; ++it)
// -> iterates from 8 up to (not including) element past 14
// -> prints: 8, 10, 12, 14
//
// s2.erase(lower, higher):
// -> erases [lower, higher) = [8, end())
// -> removes 8, 10, 12, 14
// -> s2 becomes {4, 6}
//
// COMMON PATTERN - query range [a, b]:
// auto lo = s.lower_bound(a);   // >= a
// auto hi = s.upper_bound(b);   // > b (so <= b is included)
// for (auto it = lo; it != hi; ++it) { *it; }
// -> all elements in [a, b] inclusive

// ============================================================
// SECTION 7: merge() EXPLAINED
// ============================================================

// s1.merge(s2):
// -> moves elements from s2 into s1
// -> if element ALREADY IN s1: stays in s2 (not moved!)
// -> if element NOT IN s1: moved to s1, removed from s2
// -> O(n log n)
//
// YOUR CODE:
// s1 after erase: {1,2,3,4,21,76}    (removed -12)
// s2 after erase: {4,6}              (removed 8,10,12,14)
//
// s1.merge(s2):
// -> 4 already in s1 -> stays in s2
// -> 6 not in s1 -> moved to s1
// -> s1 = {1,2,3,4,6,21,76}
// -> s2 = {4}                        (4 couldn't merge)

// ============================================================
// SECTION 8: insert() RETURN VALUE - IMPORTANT!
// ============================================================

// s.insert(val) returns std::pair<iterator, bool>
// iterator = points to the element (existing or newly inserted)
// bool = true if NEWLY inserted, false if ALREADY EXISTED
//
// std::set<int> s{1,2,3};
// auto [it, inserted] = s.insert(2);  // C++17 structured binding
// if (inserted) { std::cout << "new element"; }
// else { std::cout << "already existed"; }
//
// USE CASE: insert and check in one operation (no double lookup):
// if (s.insert(val).second) {
//     // successfully inserted, process new element
// }

// ============================================================
// SECTION 9: DSA PATTERNS
// ============================================================

// PATTERN 1: Remove duplicates while keeping sorted order
// std::vector<int> v = {3,1,4,1,5,9,2,6,5,3,5};
// std::set<int> s(v.begin(), v.end());
// // s = {1,2,3,4,5,6,9} - sorted unique
// std::vector<int> result(s.begin(), s.end());

// PATTERN 2: Check existence O(log n)
// std::set<string> visited;
// if (!visited.count(node)) {     // not visited
//     visited.insert(node);
//     // process node
// }

// PATTERN 3: Find kth smallest (ordered property)
// std::set<int> s{5,3,8,1,9,2};
// auto it = s.begin();
// std::advance(it, k-1);          // O(k) advance
// int kth = *it;

// PATTERN 4: Count elements in range [lo, hi]
// int count = std::distance(s.lower_bound(lo), s.upper_bound(hi));

// PATTERN 5: Nearest smaller/larger element
// std::set<int> s{1,3,5,7,9};
// auto it = s.lower_bound(4);     // first >= 4 = points to 5
// if (it != s.begin()) {
//     --it;                        // largest element < 4 = 3
// }

// PATTERN 6: Scheduling / intervals (sorted by start time)
// std::set<pair<int,int>> intervals;  // {start, end}
// intervals.insert({3,7});
// intervals.insert({1,5});
// // automatically sorted by start time!

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::set and what data structure is it implemented with?
//     -> Sorted collection of unique elements.
//        Implemented as a Red-Black Tree (self-balancing BST).
//        All operations O(log n) guaranteed.

// Q2: Why does set::count() return only 0 or 1?
//     -> Because set only stores UNIQUE elements.
//        count() exists for interface consistency with multiset.
//        Prefer contains() (C++20) or find() != end() for clarity.

// Q3: What is the difference between lower_bound and upper_bound?
//     -> lower_bound(val): first element >= val (inclusive).
//        upper_bound(val): first element >  val (exclusive).
//        Together: all elements in range [a,b] = [lower_bound(a), upper_bound(b))

// Q4: What happens when you merge() and the element already exists?
//     -> It stays in the source set (not moved, not duplicated).
//        merge() only moves elements that DON'T already exist in target.
//        Source set ends up with only the "collision" elements.

// Q5: Are iterators to set elements ever invalidated?
//     -> NEVER. Unlike vector, inserting or erasing in set
//        doesn't invalidate any existing iterators (tree re-balancing
//        doesn't move nodes, just updates pointers).

// Q6: What does insert() return in set?
//     -> pair<iterator, bool>. Iterator points to the element.
//        bool = true if newly inserted, false if already existed.
//        Useful for insert-and-check in one O(log n) operation.

// Q7: When would you choose set over unordered_set?
//     -> Need sorted order, ordered iteration.
//        Need lower_bound/upper_bound range queries.
//        Need GUARANTEED O(log n) (no hash collision worst case).
//        Smaller datasets where tree overhead doesn't matter.

// Q8: What is the time complexity of iterating through a set?
//     -> O(n) total for full traversal.
//        Elements visited in SORTED (ascending) order automatically.
//        This is the in-order traversal of the underlying BST.

void printSet(const std::set<int>& s)
{
    for (const auto& e : s) {       // cleaner than const_iterator
        std::cout << e << '\n';
    }
}

int main()
{
    // dupes removed automatically, stored sorted
    std::set<int> s1{1, 1, 2, 3, 4, 4}; // stored as {1,2,3,4}
    s1.insert(21);
    s1.insert(-12);
    s1.insert(76);
    printSet(s1);  // -12 1 2 3 4 21 76 (sorted!)

    // count() in set = 0 or 1 only
    std::cout << "count(4): " << s1.count(4) << '\n';  // 1

    // find + erase pattern:
    auto it = s1.find(-12);
    if (it != s1.end()) {
        std::cout << "Found -12, erasing\n";
        s1.erase(it);              // O(1) erase with iterator
    }

    // C++20 contains() - cleanest existence check
    std::cout << "Contains 2: " << s1.contains(2) << '\n';  // 1

    // lower_bound / upper_bound range query:
    std::set<int> s2{4, 6, 8, 10, 12, 14};
    auto lower = s2.lower_bound(8);    // points to 8 (>=8)
    auto higher = s2.upper_bound(14);  // points to end() (>14)

    std::cout << "Range [8,14]: ";
    for (auto it = lower; it != higher; ++it) {
        std::cout << *it << ",";       // 8,10,12,14
    }
    std::cout << '\n';

    s2.erase(lower, higher);           // removes 8,10,12,14
    printSet(s2);                      // 4 6

    // merge: moves elements not already in s1
    s1.merge(s2);
    printSet(s1);  // s2's unique elements added to s1
    printSet(s2);  // s2 has only elements that collided with s1

    // insert return value:
    auto [insertIt, wasInserted] = s1.insert(999);
    std::cout << "999 inserted: " << wasInserted << '\n';  // 1 (true)
    auto [dupIt, dupInserted] = s1.insert(999);
    std::cout << "999 again: "   << dupInserted  << '\n';  // 0 (false)

    return 0;
}
// ```

// **The lower_bound / upper_bound visual:**

// ```
// s2 = {4, 6, 8, 10, 12, 14}

// lower_bound(8)  = iterator to 8  (first >= 8)
// upper_bound(14) = iterator to end (first > 14)

// range [lower, higher):
// {4, 6, [8, 10, 12, 14]}
//         ^              ^
//       lower          higher(=end)

// Prints: 8, 10, 12, 14
// Erase removes: 8, 10, 12, 14
// s2 becomes: {4, 6}
// ```

// **count() vs find() vs contains():**

// ```cpp
// // All O(log n), pick by C++ version and what you need:
// s.count(val)          // C++98: returns 0 or 1 (misleading name for set)
// s.find(val) != s.end()// C++98: verbose but gives you the iterator
// s.contains(val)       // C++20: cleanest, most readable ← PREFER THIS
// ```

// > **The interview answer on when to use set**: "use `set` when you need **sorted unique elements** and range queries like 'find all elements between 10 and 20'. If you only need fast existence checks and order doesn't matter, `unordered_set` is faster at O(1) average. The killer feature of `set` is `lower_bound`/`upper_bound` — no other container gives you `O(log n)` range queries on dynamic data."