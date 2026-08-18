#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <forward_list>
#include <array>
#include <map>
#include <unordered_map>
#include <set>

// ============================================================
// ITERATOR INVALIDATION - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS ITERATOR INVALIDATION?
// ============================================================

// Iterator invalidation = an operation makes existing iterators
// DANGLING (point to freed/moved/invalid memory)
//
// YOUR SCREENSHOT: "Read-only methods NEVER invalidate iterators
// or references. Methods which MODIFY contents may invalidate."
//
// USING INVALIDATED ITERATOR = UNDEFINED BEHAVIOR
// -> crash, garbage values, silent data corruption
// -> no compile error, no runtime error guaranteed
// -> the hardest class of bug to debug
//
// TWO THINGS can be invalidated:
// 1. ITERATORS: the iterator objects themselves
// 2. REFERENCES/POINTERS: raw references/pointers to elements

// ============================================================
// SECTION 2: THE TABLE - CONTAINER BY CONTAINER
// ============================================================

// ---- ARRAY (std::array) ----
// After INSERTION: N/A (fixed size, can't insert!)
// After ERASURE:   N/A (fixed size, can't erase!)
// -> array never invalidates anything (no insert/erase)
// -> safest container for iterator stability

// ---- VECTOR ----
// After INSERTION:
//   iterators: NO  (invalid if reallocation happened!)
//   references: YES (valid IF capacity didn't change)
//   CONDITIONAL: "Insertion changed capacity"
//   -> push_back when size < capacity: references valid, iterators INVALID?
//      Actually: if no reallocation, elements before insert point stay valid
//      if reallocation: ALL iterators AND references invalid
//
// After ERASURE:
//   iterators: YES, except erased element(s) and AFTER them
//   references: YES, except erased element(s)
//   -> erasing middle element: everything before = still valid
//      everything at or after erased point = INVALID (elements shifted)
//
// KEY RULE FOR VECTOR: ANY push_back that causes reallocation
// invalidates ALL iterators and references!
// Use reserve() to prevent this.

// ---- DEQUE ----
// After INSERTION:
//   Inserting at FRONT or BACK: iterators NO, references YES
//   Inserting in MIDDLE:        iterators NO, references NO
//   -> deque invalidates iterators on any insert (blocks may reorganize)
//   -> but references/pointers to elements usually survive front/back insert
//
// After ERASURE:
//   Erasing at FRONT or BACK: "Yes, except erased element(s)"
//   Erasing in MIDDLE:        all invalid

// ---- LIST (std::list - doubly linked) ----
// After INSERTION: iterators YES, references YES (always valid!)
// After ERASURE:   "Yes, except erased element(s)"
// -> BEST for iterator stability!
// -> inserting/erasing just re-links pointers
// -> no element moves, no memory reallocation
// -> stored iterators SURVIVE insert/erase (except the erased one)

// ---- FORWARD_LIST (singly linked) ----
// Same as list:
// After INSERTION: YES (valid)
// After ERASURE:   "Yes, except erased element(s)"
// -> same stability guarantees as list

// ---- ASSOCIATIVE CONTAINERS (set, multiset, map, multimap) ----
// After INSERTION: YES (all valid!)
// After ERASURE:   "Yes, except erased element(s)"
// -> Red-Black Tree: insertion just adds a node (no elements move)
// -> existing iterators point to same nodes -> still valid
// -> SAME stability as list!

// ---- UNORDERED ASSOCIATIVE (unordered_set, unordered_map etc.) ----
// After INSERTION:
//   iterators: NO (if rehash occurs - "Insertion changed capacity")
//   references: YES (elements don't move even on rehash!)
//   -> rehash reorganizes buckets but elements stay in place
//   -> iterator path through buckets changes -> iterators invalid
//   -> but &element still points to same element -> refs valid
//
// After ERASURE:
//   "Yes, except erased element(s)" AND "No rehash"
//   -> without rehash: only erased element's iterator invalid
//   -> all others valid

// ============================================================
// SECTION 3: THE SAFEST ERASE-DURING-ITERATION PATTERN
// ============================================================

// WRONG (undefined behavior):
// vector<int> v = {1,2,3,4,5};
// for (auto it = v.begin(); it != v.end(); ++it) {
//     if (*it == 3) v.erase(it);  // it is NOW INVALID!
//     // ++it on invalid iterator = UB
// }

// CORRECT: use the return value of erase():
void safeEraseVector()
{
    std::vector<int> v = {1, 2, 3, 4, 5, 3, 6};
    for (auto it = v.begin(); it != v.end(); ) {
        if (*it == 3) {
            it = v.erase(it);  // erase returns NEXT VALID iterator
        } else {
            ++it;              // only advance if NOT erasing
        }
    }
    // v = {1,2,4,5,6} - all 3s removed
}

// CORRECT for list: same pattern (erase returns next valid)
void safeEraseList()
{
    std::list<int> l = {1, 2, 3, 4, 5, 3, 6};
    for (auto it = l.begin(); it != l.end(); ) {
        if (*it == 3) {
            it = l.erase(it);  // valid for list too
        } else {
            ++it;
        }
    }
}

// C++20: std::erase and std::erase_if (cleanest):
// std::erase_if(v, [](int x){ return x == 3; });

// ============================================================
// SECTION 4: CODE EXAMPLES - EACH CONTAINER
// ============================================================

void vectorInvalidation()
{
    std::vector<int> v = {1, 2, 3, 4, 5};

    auto it = v.begin();        // points to 1
    auto ref = &v[0];           // reference to first element

    // CASE 1: push_back WITH capacity (no reallocation)
    v.reserve(10);              // ensure capacity first
    auto it2 = v.begin();
    auto ref2 = &v[0];
    v.push_back(6);             // no reallocation
    // it2: may be invalid (implementation-defined)
    // ref2: valid (no reallocation, element didn't move)

    // CASE 2: push_back WITHOUT capacity (may reallocate)
    std::vector<int> v2 = {1,2,3};
    auto it3 = v2.begin();      // points to 1
    v2.push_back(4);            // might reallocate! all new memory
    // it3 is now DANGLING if reallocation happened!
    // DO NOT use it3 after this!

    // CASE 3: erase invalidates only from erased point onwards
    std::vector<int> v3 = {1, 2, 3, 4, 5};
    auto before = v3.begin();   // points to 1
    auto erased = v3.begin() + 2; // points to 3
    auto after  = v3.begin() + 3; // points to 4
    v3.erase(erased);           // removes 3, shifts 4,5 left
    // before (1): still valid (unchanged)
    // erased (was 3): INVALID (element gone)
    // after  (was 4): INVALID (element shifted left)
}

void listInvalidation()
{
    std::list<int> l = {1, 2, 3, 4, 5};
    auto it1 = l.begin();       // points to 1
    auto it2 = std::next(l.begin(), 2); // points to 3
    auto it3 = l.end();

    l.push_back(6);             // insert at back
    // ALL iterators still VALID (just linked a new node)
    // it1 still points to 1 ✓
    // it2 still points to 3 ✓

    l.insert(std::next(l.begin()), 99); // insert after 1
    // ALL existing iterators still VALID
    // it1 still points to 1 ✓
    // it2 still points to 3 ✓ (didn't move)

    l.erase(it2);               // erase the node at it2 (3)
    // it1 (1): still valid ✓
    // it2 (3): INVALID - that node was deleted
    // it3 (end): still valid ✓
}

void mapInvalidation()
{
    std::map<int, std::string> m = {{1,"a"},{2,"b"},{3,"c"}};
    auto it1 = m.begin();           // points to {1,"a"}
    auto it2 = std::next(m.begin()); // points to {2,"b"}

    m.insert({4, "d"});             // insert new node
    // it1 still valid ✓  (tree just added a node)
    // it2 still valid ✓

    m.erase(2);                     // erase key 2
    // it1 (key 1): still valid ✓
    // it2 (key 2): INVALID - that node deleted
}

void unorderedMapInvalidation()
{
    std::unordered_map<int, std::string> m;
    m.reserve(10);                  // prevent rehash
    m.insert({1, "a"});
    m.insert({2, "b"});

    auto it = m.begin();            // valid iterator

    m.insert({3, "c"});             // no rehash (reserved)
    // it: MIGHT be invalid even without rehash (unordered containers
    //     reorganize bucket chains on insert)
    // &m[1]: still valid (element didn't move in memory)

    // SAFE: rehash happened -> all iterators invalid but refs valid
    m.insert({100,"x"});            // force rehash if needed
    // it is now POTENTIALLY INVALID
    // but m[1] reference is still valid (element in same place)
}

// ============================================================
// SECTION 5: SUMMARY TABLE IN CODE
// ============================================================

// OPERATION          | array | vector | deque | list | map/set | unordered
// -------------------|-------|--------|-------|------|---------|----------
// insert() - iters   | N/A   | NO*    | NO    | YES  | YES     | NO**
// insert() - refs    | N/A   | YES*   | YES†  | YES  | YES     | YES
// erase()  - iters   | N/A   | after  | all   | only | only    | only
// erase()  - refs    | N/A   | after  | all†  | only | only    | only
//
// * vector: YES if no reallocation, NO if reallocation
// † deque: YES for front/back operations, NO for middle
// ** unordered: NO if rehash, YES if no rehash
// "after" = elements after erased point invalid
// "only" = only the erased element's iterator invalid

// ============================================================
// SECTION 6: REAL PATTERNS IN ISSST/DIS CONTEXT
// ============================================================

// In DIS simulation code, iterator invalidation matters when:
//
// SCENARIO 1: Removing disconnected entities during simulation tick
// std::unordered_map<EntityID, EntityState> entities;
// for (auto it = entities.begin(); it != entities.end(); ) {
//     if (it->second.isTimedOut()) {
//         it = entities.erase(it);  // safe: erase returns next
//     } else {
//         ++it;
//     }
// }
//
// SCENARIO 2: Adding new entities during iteration (UNSAFE!)
// for (auto& [id, state] : entities) {
//     if (state.spawnsChild) {
//         entities.insert({newId, newState}); // INVALIDATES all iters!
//         // UB: the range-based for's iterator is now invalid
//     }
// }
// FIX: collect new entities separately, insert after loop finishes
// std::vector<pair<ID, State>> toAdd;
// for (...) { if (...) toAdd.push_back({id, state}); }
// for (auto& [id, state] : toAdd) entities.insert({id, state});

// ============================================================
// SECTION 7: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is iterator invalidation?
//     -> When an operation makes existing iterators point to
//        invalid/freed/moved memory. Using them after = UB.
//        Read-only operations never invalidate. Modifying ops may.

// Q2: Which container has the BEST iterator stability?
//     -> std::list (and map/set). Insertion/erasure only affects
//        the erased element's iterator. All others remain valid.
//        Linked structures: no elements move on insert/erase.

// Q3: Why does vector invalidate iterators on push_back?
//     -> push_back may trigger REALLOCATION: allocate new buffer,
//        copy/move all elements, free old buffer.
//        All old iterators point to freed memory = dangling.
//        Fix: reserve() before loop to prevent reallocation.

// Q4: How do you safely erase elements while iterating a vector?
//     -> Use erase() return value: it = v.erase(it);
//        erase() returns iterator to the next valid element.
//        Only increment if NOT erasing: else ++it.

// Q5: Why do unordered containers invalidate iterators on rehash?
//     -> Rehash reorganizes bucket layout. Iterator encodes
//        bucket position + chain position. After rehash, same
//        element is in a different bucket -> old iterator invalid.
//        But the element's ADDRESS doesn't change -> refs still valid.

// Q6: Does list invalidate iterators on erase?
//     -> Only the ERASED element's iterator. All others remain valid.
//        This is why list is preferred when you need to erase
//        during iteration without losing other positions.

// Q7: Which iterator is always invalid and should never be dereferenced?
//     -> end() iterator. Points past the last element (sentinel).
//        Used for comparison only: it != container.end().
//        *end() = undefined behavior.

// Q8: What is the safest way to erase all matching elements?
//     -> C++20: std::erase_if(container, predicate);
//        Pre-C++20: erase-remove idiom for vector:
//        v.erase(std::remove_if(v.begin(),v.end(),pred), v.end());
//        For list/map: iterator loop with it = erase(it) pattern.

// int main()
// {
//     safeEraseVector();
//     safeEraseList();
//     vectorInvalidation();
//     listInvalidation();
//     mapInvalidation();
//     unorderedMapInvalidation();
//     return 0;
// }
// ```

// **The table in one picture:**

// ```
// After INSERT:         iterators valid?    references valid?
// array                 N/A                 N/A
// vector                NO (if realloc)     YES (if no realloc)
// deque                 NO                  YES (front/back)
// list                  YES ✅              YES ✅
// map/set               YES ✅              YES ✅
// unordered_*           NO (if rehash)      YES ✅

// After ERASE:
// array                 N/A                 N/A
// vector                only before point   only before point
// deque                 all invalid         all invalid (middle)
// list                  only erased         only erased ✅
// map/set               only erased         only erased ✅
// unordered_*           only erased         only erased
// ```

// **The safe erase pattern:**

// ```cpp
// // WRONG (UB - it is invalidated after erase):
// for (auto it = v.begin(); it != v.end(); ++it) {
//     if (*it == 3) v.erase(it);  // it now dangling!
//     // ++it on dangling iterator = crash
// }

// // CORRECT (erase returns next valid iterator):
// for (auto it = v.begin(); it != v.end(); ) {
//     if (*it == 3) it = v.erase(it);  // it updated to next valid
//     else          ++it;               // only advance if not erasing
// }

// // CLEANEST (C++20):
// std::erase_if(v, [](int x){ return x == 3; });
// ```

// > **The interview answer**: `list`, `map`, `set`, and their multi-variants have the **best iterator stability** — only the erased element's iterator is invalidated. `vector` is the most dangerous — any `push_back` that causes reallocation invalidates **everything**. The fix is `reserve()` before the loop. For `unordered` containers, rehash invalidates iterators but not references/pointers to elements (elements don't move, just bucket positions change).