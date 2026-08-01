#include <iostream>
#include <forward_list>
#include <algorithm>
#include <numeric>

// ============================================================
// std::forward_list - COMPLETE NOTES (C++11)
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::forward_list = singly linked list
// YOUR COMMENT: "forward_lists are very lightweight linked lists
//                designed primarily to push_front and occupy
//                minimal amount of memory for a linked data structure"
//
// INTERNAL STRUCTURE per node:
// struct Node {
//     T data;
//     Node* next;   // only ONE pointer (vs list which has prev AND next)
// };
//
// forward_list stores: ONLY head pointer (not even size or tail!)
//
// forward_list vs list:
// list:         [prev|data|next] <-> [prev|data|next]  // 2 pointers per node
// forward_list: [data|next] -> [data|next]             // 1 pointer per node
//
// MEMORY SAVINGS: 1 pointer per node = 8 bytes saved per element
// For 1 million elements: 8MB saved vs std::list
//
// TRADE-OFFS:
// -> NO backward traversal (one directional - YOUR COMMENT)
// -> NO size() method (would need O(n) traversal, by design omitted)
//    YOUR COMMENT: "max_size not size"
// -> NO push_back (no tail pointer - would need O(n) to find end)
// -> NO back() method
// -> insert/erase uses "after" variants (insert_after, erase_after)
//    because you need previous node to link, and we only go forward

// ============================================================
// SECTION 2: forward_list vs list vs vector
// ============================================================

// | Feature             | forward_list | list    | vector  |
// |---------------------|--------------|---------|---------|
// | Direction           | forward only | both    | both    |
// | Memory per node     | 1 pointer    | 2 ptrs  | N/A     |
// | size()              | NO (use dist)| YES     | YES     |
// | push_front          | O(1)         | O(1)    | O(n)    |
// | push_back           | NO (O(n))    | O(1)    | O(1)*   |
// | insert at middle    | O(1)**       | O(1)**  | O(n)    |
// | Random access [i]   | NO           | NO      | YES     |
// | Reverse iterate     | NO           | YES     | YES     |
// | Memory overhead     | LOWEST       | medium  | lowest  |
//   (per element)
//
// *amortized **with iterator at position
//
// USE forward_list WHEN:
// -> memory is critically constrained (embedded/IoT)
// -> only need push_front and forward traversal
// -> want absolute minimum overhead linked list
// -> implementing stack-like behavior

// ============================================================
// SECTION 3: CONSTRUCTION
// ============================================================

// std::forward_list<int> fl;              // empty
// std::forward_list<int> fl{1,2,3,4};    // initializer list
// std::forward_list<int> fl(5, 0);        // 5 zeros
// std::forward_list<int> fl2(fl);         // copy
// std::forward_list<int> fl3(v.begin(),v.end()); // from range

// ============================================================
// SECTION 4: IMPORTANT METHODS
// ============================================================

// ADD/REMOVE - ALL "AFTER" VARIANTS:
// fl.push_front(val)         -> add to FRONT                  O(1)
// fl.pop_front()             -> remove from FRONT             O(1)
// fl.insert_after(it, val)   -> insert AFTER iterator pos     O(1)
// fl.erase_after(it)         -> erase element AFTER it        O(1)
// fl.erase_after(it1, it2)   -> erase range (it1, it2)        O(n)
// fl.clear()                 -> remove all elements           O(n)
//
// WHY "after" and not "before"?
// -> to erase/insert at position, you need to UPDATE the previous node
// -> in singly linked list, you can only reach previous by traversal
// -> so API is designed around "what comes after THIS node"
//
// SPECIAL ITERATOR:
// fl.before_begin()          -> iterator to node BEFORE head
//                               used to insert/erase at front via insert_after
//                               YOUR CODE uses this for splice_after
//
// SIZE (no size() method!):
// fl.empty()                 -> true if empty                 O(1)
// fl.max_size()              -> theoretical max               O(1)
// std::distance(fl.begin(), fl.end()) -> count elements       O(n)
//
// ACCESS:
// fl.front()                 -> first element                 O(1)
// NO back(), NO operator[], NO at()!
//
// LIST-SPECIFIC:
// fl.sort()                  -> sort in place                 O(n log n)
// fl.reverse()               -> reverse order                 O(n)
// fl.remove(val)             -> remove all == val             O(n)
// fl.remove_if(pred)         -> remove all matching           O(n)
// fl.unique()                -> remove consecutive dupes      O(n)
// fl.merge(fl2)              -> merge two sorted lists        O(n)
// fl.splice_after(pos,fl2)   -> transfer elements             O(1)

// ============================================================
// SECTION 5: YOUR QUESTION - const_iterator vs iterator
// ============================================================

// YOUR COMMENT: "why use const_iterator not iterator?"
//
// FUNCTION: void printList(const std::forward_list<int>& list)
//                           ^^^^^
//                           const reference parameter
//
// const forward_list& means: the list cannot be modified
// -> list.begin() on a CONST list returns CONST_ITERATOR
// -> const_iterator: can READ elements, CANNOT WRITE
// -> regular iterator: can READ and WRITE elements
//
// const container -> can only give you const_iterator
// non-const container -> can give either
//
// USING iterator on const list = COMPILER ERROR:
// std::forward_list<int>::iterator it = list.begin();
// // ERROR: cannot convert const_iterator to iterator!
//
// USING const_iterator on const list = CORRECT:
// std::forward_list<int>::const_iterator it = list.begin();
// *it = 5;  // ERROR: can't write through const_iterator (correct!)
//
// MODERN (prefer auto - compiler picks correct type):
// for (const auto& e : list) { }  // cleanest, always correct
// auto it = list.cbegin();        // explicit const_iterator
//
// RULE: if function takes const container& -> use const_iterator
//       or just use range-based for (compiler handles it)

// ============================================================
// SECTION 6: before_begin() - THE UNIQUE forward_list ITERATOR
// ============================================================

// before_begin() = iterator to "virtual node" before the first element
// exists ONLY in forward_list (and list has equivalent)
//
// WHY NEEDED?
// insert_after needs a "previous" node to link from
// to insert at the very FRONT, you need insert_after(before_begin())
//
// VISUAL:
// [before_begin] -> [1] -> [2] -> [3] -> nullptr
//                    ^head
//
// insert_after(before_begin(), 0):
// [before_begin] -> [0] -> [1] -> [2] -> [3] -> nullptr
//
// YOUR CODE:
// myList.splice_after(myList.before_begin(), list3,
//                     list3.before_begin(), list3_end)
// -> insert elements from list3 AFTER before_begin() = at the FRONT

// ============================================================
// SECTION 7: push_back WORKAROUND (your code)
// ============================================================

void push_back(std::forward_list<int>& list, int val)
{
    // forward_list has NO push_back (no tail pointer)!
    // WORKAROUND: find last element manually
    auto pos = begin(list);
    int distance = std::distance(begin(list), end(list));
    // distance = O(n) traversal! expensive!

    std::advance(pos, distance - 1);
    // advance to last element position O(n)

    list.insert_after(pos, val);
    // insert val AFTER the last element O(1)
    // TOTAL: O(n) - this is why forward_list has no push_back!
}
// YOUR COMMENT: "Warning: you might just want to use std::list"
// CORRECT! If you need push_back, use list not forward_list
// forward_list = push_front ONLY efficient operation at ends

// ============================================================
// SECTION 8: splice_after - TRANSFER ELEMENTS
// ============================================================

// splice_after = transfer elements between forward_lists
// similar to list::splice but uses "after" positions
//
// FORM 1: transfer ENTIRE list
// fl.splice_after(pos, fl2);
// -> all of fl2 inserted after pos in fl
// -> fl2 becomes empty
//
// FORM 2: transfer ONE element
// fl.splice_after(pos, fl2, it);
// -> element AFTER it in fl2 moved to after pos in fl
//
// FORM 3: transfer RANGE (your code)
// fl.splice_after(pos, fl2, first, last);
// -> elements (first, last) moved to after pos in fl
//    (EXCLUSIVE range - first and last not included!)
//
// YOUR CODE:
// list3 = {66, 77, 88, 99}
// list3_iter = begin(list3) -> points to 66
// list3_end = std::next(list3_iter, 3) -> points to 99
// splice_after(before_begin(), list3, before_begin(), list3_end)
// -> transfers (before_begin, list3_end) exclusive = {66, 77, 88}
// -> inserts at front of myList
// -> list3 remains with {99}

// ============================================================
// SECTION 9: DSA USE CASES
// ============================================================

// USE forward_list WHEN:
//
// PATTERN 1: Memory-critical stack implementation
// std::forward_list<T> stack;
// stack.push_front(val);   // push  O(1)
// stack.front();           // peek  O(1)
// stack.pop_front();       // pop   O(1)
// -> uses LESS memory than std::stack (which uses deque internally)
//
// PATTERN 2: Adjacency list for sparse graphs (memory critical)
// std::vector<std::forward_list<int>> adj(n);
// adj[u].push_front(v);  // add edge O(1)
// -> saves 8 bytes per edge vs std::list (1 pointer vs 2)
// -> for graphs with millions of edges: significant savings
//
// PATTERN 3: Bucket in hash table (chaining collision resolution)
// std::vector<std::forward_list<pair<K,V>>> buckets(size);
// buckets[hash(key)].push_front({key, value});
// -> forward_list is the classic choice for hash buckets
// -> only need push_front and traverse (both forward_list O(1)/O(n))
//
// PATTERN 4: History/undo stack (most recent first)
// std::forward_list<Action> history;
// history.push_front(action);  // add most recent
// history.front();             // peek latest
// history.pop_front();         // undo latest

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::forward_list and how does it differ from std::list?
//     -> Singly linked list (one pointer per node vs two).
//        No size(), no push_back, no back(), no reverse iteration.
//        Uses insert_after/erase_after API.
//        Lower memory per element - designed for minimal overhead.

// Q2: Why is there no size() in forward_list?
//     -> By design. Calling size() would require O(n) traversal.
//        Storing size separately would add per-container overhead.
//        Use std::distance(fl.begin(), fl.end()) if needed (O(n)).

// Q3: Why does forward_list use insert_after instead of insert?
//     -> Singly linked: to insert before position X, you need X's
//        predecessor to update its next pointer.
//        In singly linked list, you can't go backward to find predecessor.
//        Solution: work with "after" semantics - insert after a known node.

// Q4: What is before_begin() and why does it exist?
//     -> Iterator to virtual node before the first element.
//        Needed for insert_after at the front position.
//        insert_after(before_begin(), val) = insert at front.
//        Also used in splice_after for front transfers.

// Q5: Why use const_iterator in printList?
//     -> Function takes const forward_list& parameter.
//        const container only yields const_iterators from begin().
//        const_iterator allows reading but not writing.
//        Better: use auto or range-for (compiler handles it).

// Q6: Why is push_back O(n) for forward_list?
//     -> forward_list stores only head pointer (no tail).
//        To insert at end: must traverse entire list to find last node.
//        Then insert_after last node = O(1).
//        Total: O(n) traversal + O(1) insert = O(n).

// Q7: When would you choose forward_list over list?
//     -> Memory critical environment (embedded, IoT).
//        Only need push_front and forward traversal.
//        Hash table bucket implementation.
//        Stack implementation with minimal overhead.

// Q8: What does unique() do in forward_list?
//     -> Removes CONSECUTIVE duplicate elements.
//        {1,1,2,2,1,1} -> {1,2,1} (keeps first of each consecutive group).
//        Must sort() first for complete deduplication.

void printList(const std::forward_list<int>& list)
{
    std::cout << "===================\n";
    // MODERN: range-based for (compiler picks const_iterator automatically)
    for (const auto& e : list) {
        std::cout << e << " ";
    }
    std::cout << '\n';
}

int main()
{
    std::forward_list<int> myList{1, 2, 3, 4};
    myList.push_front(0);       // O(1) - the FAST operation!
    printList(myList);          // 0 1 2 3 4

    push_back(myList, 5);       // O(n) - workaround
    push_back(myList, 6);
    printList(myList);          // 0 1 2 3 4 5 6

    myList.push_front(-7);
    myList.push_front(-5);
    myList.sort();
    printList(myList);          // -7 -5 0 1 2 3 4 5 6

    std::forward_list<int> list2{-2, 0, 3, 4, 5};
    myList.merge(list2);        // both must be sorted!
    printList(myList);          // merged sorted list
    printList(list2);           // empty after merge

    myList.unique();            // remove consecutive duplicates
    printList(myList);          // 0s merged -> one 0 remains

    // splice_after range transfer
    std::forward_list<int> list3{66, 77, 88, 99};
    auto list3_iter = begin(list3);
    auto list3_end  = std::next(list3_iter, 3); // points to 99

    // transfer (before_begin, list3_end) exclusive = {66,77,88}
    // insert at front of myList (after before_begin)
    myList.splice_after(myList.before_begin(), list3,
                        list3.before_begin(), list3_end);

    printList(myList);  // 66 77 88 at front
    printList(list3);   // {99} remains

    return 0;
}
// ```

// **forward_list vs list - memory:**

// ```
// std::list node:         std::forward_list node:
// [prev*][data][next*]    [data][next*]
//   8B    4B    8B          4B    8B
// = 20 bytes per node    = 12 bytes per node

// 1 million int elements:
// list:         ~20MB
// forward_list: ~12MB   ← 8MB saved!
// ```

// **The "after" API - why it exists:**

// ```
// forward_list: [1] -> [2] -> [3] -> nullptr
//                       ^
//              want to insert 9 here (before 2)

// Problem: I'm AT node 2, I need to update node 1's next pointer
//          But I can't go BACKWARD to node 1!

// Solution: work from node 1's perspective
//          "insert AFTER node 1" = same result
//          insert_after(iterator_to_1, 9)

// [1] -> [9] -> [2] -> [3]  ✅
// ```

// > **The interview answer**: forward_list is the right choice when you need a linked list with **absolute minimum memory** — one pointer per node instead of two. The entire API (insert_after, erase_after, before_begin, no size(), no push_back) flows from that single design decision. Use it for hash table buckets, memory-critical stacks, or graph adjacency lists where every byte matters.