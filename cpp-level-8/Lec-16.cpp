#include <iostream>
#include <set>
#include <algorithm>

// ============================================================
// std::multiset - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::multiset<T> = sorted collection that ALLOWS DUPLICATES
// YOUR COMMENT: "similar to set but DOES allow duplicates"
// "uses Red-Black Tree for O(log n) operations"
//
// set:      {1,2,3,4} - inserts 1 twice -> still {1,2,3,4}
// multiset: {1,2,3,4} - inserts 1 twice -> {1,1,2,3,4}
//
// YOUR QUESTION: "in set, multiset, or unordered_set are numbers
//                stored in order automatically?"
//
// ANSWER:
// std::set          -> YES, always SORTED ascending
// std::multiset     -> YES, always SORTED ascending (with duplicates)
// std::unordered_set-> NO,  order is UNPREDICTABLE (hash-based)
//
// ALL THREE maintain this automatically on every insert/erase
// You never need to call sort() on them

// ============================================================
// SECTION 2: set vs multiset vs unordered_multiset
// ============================================================

// | Feature              | set      | multiset  | unordered_multiset |
// |----------------------|----------|-----------|--------------------|
// | Duplicates           | NO       | YES       | YES                |
// | Sorted               | YES      | YES       | NO                 |
// | Implementation       | RB-Tree  | RB-Tree   | Hash Table         |
// | Find                 | O(log n) | O(log n)  | O(1) avg           |
// | Insert               | O(log n) | O(log n)  | O(1) avg           |
// | count(val)           | 0 or 1   | 0,1,2,... | 0,1,2,...          |
// | erase(val)           | removes 1| removes ALL| removes ALL       |
// | erase(iterator)      | removes 1| removes 1 | removes 1          |
//
// USE multiset WHEN:
// -> need sorted ORDER with DUPLICATES
// -> need count() to return actual frequency
// -> need range queries (lower_bound/upper_bound) on non-unique data
// -> priority queue-like behavior but need iteration over all elements
// -> scheduling: multiple events at same priority

// ============================================================
// SECTION 3: CUSTOM COMPARATOR - TWO APPROACHES
// ============================================================

// APPROACH 1: operator< INSIDE the class (your code)
class Custom
{
public:
    int field1;
    int field2;

    Custom(int _field1, int _field2) : field1(_field1), field2(_field2) {}

    // operator< defines NATURAL ORDERING for the type
    // multiset uses this when no comparator template param given
    // MUST be const (doesn't modify the object)
    bool operator<(const Custom& rhs) const
    {
        int lhs_sum = field1 + field2;
        int rhs_sum = rhs.field1 + rhs.field2;
        return lhs_sum < rhs_sum;
        // sorted by SUM of field1+field2
    }

    void print() const
    {
        std::cout << "(" << field1 << "," << field2 << ")="
                  << field1 + field2 << '\n';
    }
};

// APPROACH 2: External comparator struct (your code)
// USE WHEN: cannot modify the class (third-party library, legacy code)
struct compare
{
    constexpr bool operator()(const Custom& lhs, const Custom& rhs) const
    //                                                               ^^^^^
    //                                                               MUST be const!
    //                                                               comparison can't modify state
    {
        int lhs_sum = lhs.field1 + lhs.field2;
        int rhs_sum = rhs.field1 + rhs.field2;  // BUG IN YOUR CODE!
        // YOUR CODE: rhs.field1 + lhs.field2   <- uses lhs.field2, not rhs!
        // CORRECT:   rhs.field1 + rhs.field2
        return lhs_sum < rhs_sum;
    }
};
// USE: std::multiset<Custom, compare> multi;
// -> compare::operator() called instead of Custom::operator

// ============================================================
// SECTION 4: IMPORTANT METHODS WITH COMPLEXITY
// ============================================================

// INSERT:
// ms.insert(val)         -> insert (duplicates allowed!)     O(log n)
// ms.emplace(args)       -> construct in place               O(log n)
//
// REMOVE (KEY DIFFERENCE from set!):
// ms.erase(val)          -> removes ALL copies of val!       O(log n + count)
//                           YOUR COMMENTED CODE: s.erase(2) removes all 2s
// ms.erase(it)           -> removes ONLY ONE (at iterator)   O(1) amort
// ms.erase(it1, it2)     -> removes range [it1, it2)         O(n)
//
// SEARCH:
// ms.find(val)           -> iterator to FIRST occurrence     O(log n)
// ms.count(val)          -> ACTUAL COUNT of val              O(log n + count)
//                           DIFFERENT from set! returns 0,1,2,3...
// ms.contains(val)       -> bool (C++20)                     O(log n)
// ms.lower_bound(val)    -> first element >= val             O(log n)
// ms.upper_bound(val)    -> first element > val              O(log n)
// ms.equal_range(val)    -> pair<lower, upper> both at once  O(log n)
//
// SIZE:
// ms.size()              -> total element count              O(1)
// ms.empty()             -> true if empty                    O(1)

// ============================================================
// SECTION 5: THE KEY ERASE DISTINCTION
// ============================================================

// std::multiset<int> ms{1, 2, 2, 2, 3};
//
// ms.erase(2):
// -> removes ALL 2s
// -> ms becomes {1, 3}
//
// auto it = ms.find(2);
// ms.erase(it):
// -> removes ONLY the FIRST 2
// -> ms becomes {1, 2, 2, 3}
//
// YOUR COMMENTED CODE shows this correctly:
// s.erase(it)   -> removes one occurrence
// s.erase(key)  -> removes all occurrences
//
// TO REMOVE EXACTLY N COPIES:
// auto it = ms.lower_bound(val);
// std::advance(it, n);           // skip n elements
// ms.erase(ms.lower_bound(val), it); // erase exactly n

// ============================================================
// SECTION 6: equal_range - FINDING ALL DUPLICATES
// ============================================================

// ms.equal_range(val) returns pair<iterator, iterator>
// = [lower_bound(val), upper_bound(val))
// = range containing ALL occurrences of val
//
// std::multiset<int> ms{1,2,2,2,3,4};
// auto [lo, hi] = ms.equal_range(2);  // C++17
// // lo points to first 2
// // hi points to element after last 2 (= 3)
//
// for (auto it = lo; it != hi; ++it) {
//     std::cout << *it << " ";  // prints: 2 2 2
// }
//
// COUNT via distance:
// int count = std::distance(lo, hi);  // = 3

// ============================================================
// SECTION 7: YOUR CODE TRACE
// ============================================================

int main()
{
    // ---- COMMENTED SECTION EXPLAINED ----
    // std::multiset<int> s{1, 1, 2, 2, 2, 3, 4, 5, 1, 1};
    // stored: {1,1,1,1,2,2,2,3,4,5}  <- sorted, duplicates kept
    //
    // s.count(2)           -> 3 (three 2s!)
    // s.find(2)            -> iterator to FIRST 2
    // s.erase(it)          -> removes ONLY that one 2
    // s.count(2)           -> now 2
    // s.erase(2)           -> removes ALL remaining 2s
    // s.count(2)           -> now 0

    // ---- CUSTOM TYPE WITH EXTERNAL COMPARATOR ----
    std::multiset<Custom, compare> multi;
    // uses compare::operator() for ordering
    // sorted by (field1 + field2) sum

    Custom c0(1, 6);   // sum = 7
    Custom c1(2, 7);   // sum = 9
    Custom c2(2, 7);   // sum = 9 (DUPLICATE sum -> allowed in multiset!)
    Custom c3(9, -5);  // sum = 4

    multi.insert(c0);
    multi.insert(c1);
    multi.insert(c2);
    multi.insert(c3);

    // SORTED BY SUM:
    // c3(9,-5)=4 < c0(1,6)=7 < c1(2,7)=9 == c2(2,7)=9

    std::for_each(multi.cbegin(), multi.cend(),
        [](const Custom& c) { c.print(); });
    // OUTPUT (sorted by sum):
    // (9,-5)=4
    // (1,6)=7
    // (2,7)=9
    // (2,7)=9   <- duplicate allowed!

    return 0;
}

// ============================================================
// SECTION 8: DSA PATTERNS WITH MULTISET
// ============================================================

// PATTERN 1: Median maintenance (classic interview problem)
// Keep two multisets: lower half (max at back) and upper half (min at front)
// std::multiset<int> lower, upper;
// insert n -> rebalance so sizes differ by at most 1
// median = *lower.rbegin() or average of both
//
// PATTERN 2: Count frequency while keeping sorted order
// std::multiset<int> ms(v.begin(), v.end());
// ms.count(key)  // frequency of key
// // iterate for sorted traversal with all occurrences
//
// PATTERN 3: Sliding window minimum/maximum
// std::multiset<int> window;
// for (int i = 0; i < n; i++) {
//     window.insert(nums[i]);
//     if (i >= k) window.erase(window.find(nums[i-k])); // erase ONE
//     if (i >= k-1) result.push_back(*window.begin());  // min
// }
//
// PATTERN 4: k closest elements
// std::multiset<int> ms(v.begin(), v.end());
// auto it = ms.lower_bound(target);
// // check elements before and after it to find k closest
//
// PATTERN 5: Schedule tasks by priority (duplicates allowed)
// struct Task { int priority; string name; };
// std::multiset<Task> schedule; // sorted by priority
// // multiple tasks CAN have same priority (vs set which would reject)

// ============================================================
// SECTION 9: STRICT WEAK ORDERING - COMPARATOR RULES
// ============================================================

// YOUR compare struct must satisfy STRICT WEAK ORDERING:
// (same rules as std::sort comparator)
//
// 1. IRREFLEXIVITY: comp(a,a) = false
//    lhs_sum < rhs_sum where both equal: false ✓
//
// 2. ASYMMETRY: if comp(a,b) then !comp(b,a)
//    if a_sum < b_sum then b_sum < a_sum is false ✓
//
// 3. TRANSITIVITY: if comp(a,b) and comp(b,c) then comp(a,c) ✓
//
// BUG IN YOUR compare STRUCT:
// rhs_sum = rhs.field1 + lhs.field2;   // uses lhs.field2 by mistake!
// SHOULD BE:
// rhs_sum = rhs.field1 + rhs.field2;   // both from rhs
// This bug causes undefined behavior (violates strict weak ordering)!

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the key difference between set and multiset?
//     -> set: unique elements only, insert duplicate = rejected.
//        multiset: allows duplicates, count() returns actual frequency.
//        Both: sorted (Red-Black Tree), same O(log n) operations.

// Q2: How do you remove ONE occurrence vs ALL in multiset?
//     -> ONE:  ms.erase(ms.find(val))  - erase at iterator position
//        ALL:  ms.erase(val)           - erase by value removes all copies

// Q3: Are elements in set/multiset/unordered_set sorted?
//     -> set: YES, sorted ascending automatically
//        multiset: YES, sorted ascending with duplicates
//        unordered_set: NO, order is unpredictable (hash-based)

// Q4: What does count() return in multiset vs set?
//     -> set: 0 or 1 only (unique elements)
//        multiset: 0, 1, 2, 3... (actual frequency of the element)

// Q5: What is equal_range() and when is it useful?
//     -> Returns pair<lower_bound, upper_bound> = range of all
//        occurrences of a value. Useful to find/iterate/count
//        all duplicates of a specific value in O(log n).

// Q6: What are the requirements for a custom comparator?
//     -> Must satisfy Strict Weak Ordering:
//        irreflexivity (comp(a,a)=false), asymmetry, transitivity.
//        Must be const (can't modify state).
//        constexpr preferred for compile-time optimization.

// Q7: What is the sliding window minimum problem using multiset?
//     -> Maintain a multiset of window elements.
//        Add new element: insert(nums[i]).
//        Remove old element: erase(find(nums[i-k])) - ONE occurrence!
//        Minimum: *window.begin() (first = smallest in sorted multiset).

// Q8: When would you choose multiset over priority_queue?
//     -> multiset: can iterate ALL elements, supports lower/upper_bound,
//        can remove specific elements by value.
//        priority_queue: only access top element, faster O(1) top access,
//        less memory overhead. If you need more than just top: multiset.
// ```

// **The erase trap - most common interview mistake:**

// ```cpp
// std::multiset<int> ms{1, 2, 2, 2, 3};

// ms.erase(2);               // removes ALL 2s -> {1, 3}
// ms.erase(ms.find(2));      // removes ONE 2  -> {1, 2, 2, 3}

// // RULE: erase(value) = ALL copies
// //       erase(iterator) = ONE element
// ```

// **The three sorted/unsorted containers:**

// ```
// std::set<int>           s{3,1,2,1,3};  → stored: {1,2,3}     SORTED, UNIQUE
// std::multiset<int>      m{3,1,2,1,3};  → stored: {1,1,2,3,3} SORTED, DUPLICATES
// std::unordered_set<int> u{3,1,2,1,3};  → stored: {3,2,1}     UNSORTED, UNIQUE
//                                                    (any order)
// ```

// **The bug to fix in your comparator:**

// ```cpp
// // YOUR CODE (bug):
// int rhs_sum = rhs.field1 + lhs.field2;  // lhs.field2 ← WRONG!

// // CORRECT:
// int rhs_sum = rhs.field1 + rhs.field2;  // both from rhs ← RIGHT

// // Bug violates Strict Weak Ordering → undefined behavior in multiset!
// ```

// > **DSA golden rule for multiset**: it's the go-to structure for the **sliding window minimum/maximum** and **median maintenance** problems — situations where you need a sorted dynamic collection that supports fast insert, delete of ONE specific occurrence, and instant access to min/max. The `erase(find(val))` pattern to remove exactly one occurrence is the key idiom to memorize.