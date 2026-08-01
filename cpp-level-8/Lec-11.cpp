#include <iostream>
#include <list>
#include <algorithm>

// ============================================================
// std::list - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::list = doubly linked list
// YOUR COMMENT: "node1 <-> node2 <-> node3 <-> node4"
// each node has: data + pointer to NEXT + pointer to PREV
//
// INTERNAL STRUCTURE per node:
// struct Node {
//     T data;
//     Node* prev;
//     Node* next;
// };
//
// list itself stores: head pointer + tail pointer + size
//
// KEY PROPERTY: elements are NOT contiguous in memory
// -> each node is a SEPARATE heap allocation
// -> jumping to element i = O(n) must traverse from head
// -> NO random access: list[i] does NOT exist!
// -> BUT: insert/erase ANYWHERE with an iterator = O(1)
//    (just re-link pointers, no shifting like vector)

// ============================================================
// SECTION 2: list vs vector vs array
// ============================================================

// | Operation           | list    | vector  | array   |
// |---------------------|---------|---------|---------|
// | Random access [i]   | NO      | O(1)    | O(1)    |
// | Insert at front     | O(1)    | O(n)    | NO      |
// | Insert at back      | O(1)    | O(1)*   | NO      |
// | Insert at middle    | O(1)**  | O(n)    | NO      |
// | Erase at front      | O(1)    | O(n)    | NO      |
// | Erase at back       | O(1)    | O(1)    | NO      |
// | Erase at middle     | O(1)**  | O(n)    | NO      |
// | Search              | O(n)    | O(n)    | O(n)    |
// | Memory              | non-cont| contiguous| contiguous|
// | Cache friendly      | NO      | YES     | YES     |
// | Iterator invalidated| NEVER   | on realloc| never  |
//
// *amortized  **with iterator already at position
//
// WHEN TO USE list:
// -> frequent insert/erase in MIDDLE of container
// -> iterators must NEVER be invalidated (list guarantees this!)
// -> order-preserving operations (merge, splice, sort)
// -> implementing LRU cache (move-to-front in O(1))
//
// WHEN NOT TO USE list:
// -> need random access (use vector)
// -> cache performance matters (use vector - list = cache miss per node)
// -> just need front/back operations (use deque)
// -> small dataset (vector usually wins due to cache)

// ============================================================
// SECTION 3: CONSTRUCTION / INITIALIZATION
// ============================================================

// std::list<int> l;                    // empty list
// std::list<int> l = {1,2,3,4};        // initializer list
// std::list<int> l{1,2,3,4};           // same
// std::list<int> l(5, 0);              // 5 elements all = 0
// std::list<int> l2(l);               // copy
// std::list<int> l3(v.begin(),v.end()); // from any iterator range

// ============================================================
// SECTION 4: IMPORTANT METHODS WITH COMPLEXITY
// ============================================================

// ADD/REMOVE (ALL O(1)):
// l.push_back(val)   -> add to end                      O(1)
// l.push_front(val)  -> add to front                    O(1)
// l.pop_back()       -> remove from end                 O(1)
// l.pop_front()      -> remove from front               O(1)
// l.insert(it, val)  -> insert BEFORE iterator pos      O(1)
//                       YOUR COMMENT: "insert value before position"
// l.erase(it)        -> remove at iterator pos          O(1)
// l.erase(it1,it2)   -> remove range [it1,it2)          O(n)
// l.clear()          -> remove all elements             O(n)
//
// SIZE:
// l.size()           -> element count                   O(1)
// l.empty()          -> true if empty                   O(1)
// l.max_size()       -> theoretical max                 O(1)
//
// ACCESS (no operator[] or at()!):
// l.front()          -> first element                   O(1)
// l.back()           -> last element                    O(1)
//
// LIST-SPECIFIC OPERATIONS (not in vector!):
// l.sort()           -> sort in place (merge sort)      O(n log n)
// l.reverse()        -> reverse order                   O(n)
// l.remove(val)      -> remove ALL elements == val      O(n)
// l.remove_if(pred)  -> remove ALL matching predicate   O(n)
// l.unique()         -> remove consecutive duplicates   O(n)
//                       (call sort() first for full dedup)
// l.merge(l2)        -> merge TWO SORTED lists          O(n)
//                       l2 becomes empty after merge!
// l.splice(pos,l2)   -> transfer ALL of l2 into l       O(1)
// l.splice(pos,l2,it)-> transfer ONE element            O(1)
// l.splice(pos,l2,f,e)-> transfer range [f,e)           O(1)
//                       YOUR CODE: transfer partial list3

// ============================================================
// SECTION 5: ITERATOR NAVIGATION (critical for list!)
// ============================================================

// list has BIDIRECTIONAL iterators (NOT random access)
// -> can go forward (++) and backward (--)
// -> CANNOT do: it + 3, it - 2, it[3] (random access)
//
// TO ADVANCE: use std::next() or std::advance()
//
// std::next(it, n):
// -> returns NEW iterator n steps ahead
// -> does NOT modify original it
// -> YOUR CODE: it = std::next(it, myList.size()/2)
//
// std::advance(it, n):
// -> MODIFIES it in place (moves it n steps)
// -> no return value
// -> YOUR CODE: std::advance(it, myList.size()/2)
//
// std::prev(it, n):
// -> returns NEW iterator n steps backward
//
// DIFFERENCE:
// auto it2 = std::next(it, 3);  // it unchanged, it2 = it+3
// std::advance(it, 3);          // it itself moved to it+3
//
// BOTH are O(n) for list (must traverse node by node)
// For vector/array: O(1) (just pointer arithmetic)

// ============================================================
// SECTION 6: splice - MOST POWERFUL LIST FEATURE
// ============================================================

// splice = transfer elements from one list to another
// O(1) for moving ANY number of elements!
// (just re-linking pointers, no copying data)
//
// THREE FORMS:
//
// FORM 1: transfer ENTIRE list
// myList.splice(pos, list2);
// -> ALL of list2 moved into myList before pos
// -> list2 becomes EMPTY
//
// FORM 2: transfer ONE element
// myList.splice(pos, list2, it);
// -> element at it in list2 moved to pos in myList
//
// FORM 3: transfer RANGE (your code)
// myList.splice(pos, list2, first, last);
// -> elements [first, last) in list2 moved to pos in myList
// -> list3_iter points to 3rd element (index 2)
// -> moves [list3_iter, end(list3)) = just "35"
// -> list3 becomes {15, 25}

// ============================================================
// SECTION 7: merge - MERGING TWO SORTED LISTS
// ============================================================

// l1.merge(l2):
// -> BOTH lists must be sorted ascending FIRST
// -> merges l2 INTO l1 in sorted order
// -> l2 becomes EMPTY after merge
// -> O(n+m) where n,m are sizes of both lists
//
// YOUR CODE:
// myList.reverse() called before merge
// -> because myList was sorted descending, reverse makes it ascending
// list2 = {2,4,6,8,10} already sorted ascending
// merge combines them: {1,2,2,3,4,4,6,8,9,10,12}

// ============================================================
// SECTION 8: remove vs remove_if vs erase
// ============================================================

// l.remove(val):
// -> removes ALL elements equal to val
// -> O(n) traversal
// -> l.remove(0) removes all zeros
//
// l.remove_if(predicate):
// -> removes ALL elements where predicate returns true
// -> YOUR CODE: remove_if([](int n){ return n < 1; })
// -> removes all elements less than 1 (removes 0)
//
// l.erase(it):
// -> removes ONE specific element at iterator position
// -> O(1) (but need O(n) to FIND the position first)
//
// l.unique():
// -> removes CONSECUTIVE duplicates only!
// -> {1,1,2,2,1,1} -> {1,2,1} (only consecutive ones removed)
// -> sort() FIRST for complete deduplication:
// l.sort();   // {1,1,1,2,2}
// l.unique(); // {1,2}

// ============================================================
// SECTION 9: DSA USE CASES
// ============================================================

// PATTERN 1: LRU Cache (most famous list use case)
// Most recently used -> front
// Least recently used -> back
// On access: splice element to front O(1)
// On eviction: remove from back O(1)
// std::list<int> lruList;
// std::unordered_map<int, std::list<int>::iterator> cache;
// // move to front: lruList.splice(lruList.begin(), lruList, cache[key])
//
// PATTERN 2: Task scheduler (insert by priority)
// std::list<Task> tasks;
// // find insertion point: O(n)
// // insert: O(1) with iterator
//
// PATTERN 3: When iterators must survive insert/erase
// std::list<int> l = {1,2,3,4,5};
// auto it = std::next(l.begin(), 2); // points to 3
// l.push_back(99);   // it still valid! (unlike vector)
// l.erase(l.begin()); // it still valid! (points to 3)
// *it;               // still 3, iterator never invalidated
//
// PATTERN 4: Merge sort on linked list (classic interview)
// list.sort() uses merge sort internally (no random access needed)
// merge sort = natural fit for linked lists

// ============================================================
// SECTION 10: REVERSE ITERATION
// ============================================================

// YOUR CODE: rbegin/rend for reverse traversal
//
// for (std::list<int>::reverse_iterator it = myList.rbegin();
//      it != myList.rend(); ++it) {
//     std::cout << *it;
// }
//
// MODERN (prefer auto):
// for (auto it = myList.rbegin(); it != myList.rend(); ++it) {
//     std::cout << *it;
// }
//
// OR range-based reverse (C++20):
// for (const auto& e : std::views::reverse(myList)) {
//     std::cout << e;
// }

// ============================================================
// SECTION 11: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::list and what is it implemented as?
//     -> Doubly linked list. Each node has data + prev + next pointers.
//        Non-contiguous memory. O(1) insert/erase with iterator.
//        No random access (no operator[]).

// Q2: When would you choose list over vector?
//     -> Frequent insert/erase in the MIDDLE of the container.
//        When you need iterator stability (never invalidated).
//        LRU cache, task schedulers, any move-to-front pattern.
//        In practice: vector usually wins due to cache efficiency.

// Q3: What is the difference between std::next and std::advance?
//     -> std::next(it,n): returns NEW iterator, original unchanged.
//        std::advance(it,n): modifies it IN PLACE, no return value.
//        Both O(n) for list (must traverse nodes).
//        Both O(1) for vector/array (pointer arithmetic).

// Q4: What does splice do and why is it O(1)?
//     -> Transfers elements from one list to another by re-linking
//        pointers. No data is copied or moved, just pointer updates.
//        Even moving N elements is O(1) (or O(1) for range splice).

// Q5: What is the difference between remove, remove_if, and erase?
//     -> remove(val): remove all elements equal to val O(n).
//        remove_if(pred): remove all matching predicate O(n).
//        erase(it): remove specific element at iterator O(1).
//        erase(it1,it2): remove range O(distance).

// Q6: What does merge() require and what happens to the source list?
//     -> Both lists must be SORTED (ascending by default).
//        Source list (l2) becomes EMPTY after merge.
//        Result is merged into the calling list in sorted order.

// Q7: Are iterators invalidated after insert/erase in list?
//     -> NEVER for list (unlike vector).
//        Inserting/erasing only re-links pointers of affected nodes.
//        All other iterators remain valid. This is a key advantage.

// Q8: What is unique() and when do you need sort() first?
//     -> unique() removes CONSECUTIVE duplicates only.
//        {1,1,2,1,1} -> {1,2,1} (non-consecutive kept).
//        Must sort() first for FULL deduplication:
//        sort() -> {1,1,1,2,2}, unique() -> {1,2}.

void printList(const std::list<int>& list)
{
    std::cout << "========================\n";
    for (const auto& e : list) {
        std::cout << e << ",";
    }
    std::cout << '\n';
}

int main()
{
    std::list<int> myList;
    myList.push_back(1);
    myList.push_back(2);
    myList.push_back(3);
    myList.push_back(4);
    printList(myList);  // 1,2,3,4

    // insert(pos, val) = insert BEFORE pos
    myList.insert(begin(myList), 0);   // insert 0 at front
    myList.insert(end(myList),   12);  // insert 12 at end
    printList(myList);  // 0,1,2,3,4,12

    // std::next: get iterator N steps ahead (non-modifying)
    auto it = cbegin(myList);
    it = std::next(it, myList.size() / 2); // move to middle
    myList.insert(it, 9);                  // insert 9 at middle
    printList(myList);  // 0,1,2,9,3,4,12

    // std::advance: move iterator IN PLACE
    it = cbegin(myList);
    std::advance(it, myList.size() / 2);
    std::cout << "middle is: " << *it << '\n';

    // sort and reverse
    myList.sort();
    myList.reverse();
    printList(myList);  // descending

    // remove_if: remove all elements < 1 (removes 0)
    myList.remove_if([](int n){ return n < 1; });
    printList(myList);

    // reverse iteration
    for (auto rit = myList.rbegin(); rit != myList.rend(); ++rit) {
        std::cout << "reverse: " << *rit << '\n';
    }

    // merge: both lists must be sorted ascending!
    myList.reverse(); // make ascending for merge
    std::list<int> list2{2, 4, 6, 8, 10};
    myList.merge(list2);        // list2 becomes empty!
    printList(myList);
    std::cout << "list2 size after merge: " << list2.size() << '\n'; // 0

    // splice: transfer RANGE from list3 into myList
    std::list<int> list3{15, 25, 35};
    auto list3_iter = list3.begin();
    std::advance(list3_iter, 2); // points to 35 (index 2)

    // transfer [list3_iter, end) = {35} into end of myList
    myList.splice(end(myList), list3, list3_iter, end(list3));
    printList(myList);           // myList now has 35 at end
    printList(list3);            // list3 = {15, 25} (35 was moved)

    return 0;
}
// ```

// **The key visual - why list beats vector for middle insert:**

// ```
// VECTOR insert at middle (O(n)):
// [1][2][3][4][5]
// insert 9 at index 2:
// [1][2][_][3][4][5]  ← must SHIFT 3,4,5 right
// [1][2][9][3][4][5]  ← then write 9
// cost: shifted 3 elements

// LIST insert at middle (O(1)) - with iterator:
// 1 <-> 2 <-> 3 <-> 4 <-> 5
// just re-link 2 and 3 to point to new node 9:
// 1 <-> 2 <-> 9 <-> 3 <-> 4 <-> 5
// cost: updated 4 pointers only, regardless of list size!
// ```

// **splice in one picture:**

// ```
// myList: [1,2,3,4]
// list3:  [15,25,35]
//             ^
//         list3_iter (points to 35)

// myList.splice(end(myList), list3, list3_iter, end(list3))

// RESULT:
// myList: [1,2,3,4,35]    ← 35 MOVED (no copy)
// list3:  [15,25]          ← 35 is gone
// ```

// > **DSA golden rule**: list's `O(1)` insert/erase is only useful when you **already have an iterator** to the position. Finding the position is still `O(n)`. So list wins when: (1) you keep iterators around, (2) you need splice/merge, or (3) you're implementing structures like LRU cache where you always operate on known positions (front/back or stored iterators).