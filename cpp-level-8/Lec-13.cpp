#include <iostream>
#include <deque>
#include <algorithm>
#include <cstdio>

// ============================================================
// std::deque - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::deque = Double Ended Queue
// YOUR COMMENTS nail it:
// "similar to vector but more efficient in insertion and deletion
//  at BOTH front AND back"
// "indexed sequence container - fast insert/delete at front AND back"
// "FIFO - first in first out"
//
// INTERNAL IMPLEMENTATION:
// NOT a simple array like vector
// NOT a linked list
// A DYNAMIC ARRAY OF FIXED-SIZE CHUNKS (blocks/pages)
//
// VISUAL:
// [block0: 1,2,3,4] [block1: 5,6,7,8] [block2: 9,10,11,12]
//        ^                                           ^
//      front                                       back
//
// Central "map" array stores pointers to each block:
// map: [ptr0] [ptr1] [ptr2] ...
//        |      |      |
//      block0 block1 block2
//
// WHY THIS DESIGN?
// -> push_front: fill from end of block0 backwards (no shifting!)
// -> push_back:  fill next slot in last block (no shifting!)
// -> random access: calculate which block + which offset = O(1)
// -> NO single contiguous memory = can grow without copying everything

// ============================================================
// SECTION 2: deque vs vector vs list
// ============================================================

// | Operation           | deque   | vector  | list    |
// |---------------------|---------|---------|---------|
// | push_back           | O(1)*   | O(1)*   | O(1)    |
// | push_front          | O(1)*   | O(n)!   | O(1)    |
// | pop_back            | O(1)    | O(1)    | O(1)    |
// | pop_front           | O(1)    | O(n)!   | O(1)    |
// | Random access [i]   | O(1)    | O(1)    | NO      |
// | Insert at middle    | O(n)    | O(n)    | O(1)**  |
// | Contiguous memory   | NO      | YES     | NO      |
// | Cache friendly      | OK      | BEST    | WORST   |
// | Iterator invalidated| on insert| on realloc| NEVER|
//
// *amortized  **with iterator at position
//
// USE deque WHEN:
// -> need O(1) push/pop at BOTH ends (vector only O(1) at back)
// -> need random access (list doesn't have this)
// -> implementing queue (FIFO), sliding window
// -> std::stack and std::queue use deque internally by default!
//
// USE vector WHEN:
// -> only need fast back operations
// -> need contiguous memory (.data() for C API)
// -> best cache performance

// ============================================================
// SECTION 3: CONSTRUCTION
// ============================================================

// std::deque<int> d;               // empty
// std::deque<int> d{1,2,3,4};      // initializer list
// std::deque<int> d(5, 0);          // 5 zeros
// std::deque<int> d2(d);            // copy
// std::deque<int> d3(v.begin(), v.end()); // from range

// ============================================================
// SECTION 4: IMPORTANT METHODS WITH COMPLEXITY
// ============================================================

// ADD/REMOVE AT ENDS (all O(1) amortized):
// d.push_back(val)    -> add to BACK                    O(1)*
// d.push_front(val)   -> add to FRONT                   O(1)*
// d.pop_back()        -> remove from BACK               O(1)
// d.pop_front()       -> remove from FRONT              O(1)
// d.emplace_back(args)-> construct in place at BACK     O(1)*
// d.emplace_front(args)->construct in place at FRONT    O(1)*
//
// INSERT/ERASE AT MIDDLE (O(n) - shifts elements):
// d.insert(it, val)   -> insert before iterator         O(n)
// d.erase(it)         -> erase at iterator              O(n)
// d.erase(it1, it2)   -> erase range                    O(n)
//
// ACCESS:
// d[i]                -> random access NO bounds check  O(1)
// d.at(i)             -> random access WITH bounds check O(1)
// d.front()           -> first element                  O(1)
// d.back()            -> last element                   O(1)
//
// SIZE:
// d.size()            -> element count                  O(1)
// d.empty()           -> true if empty                  O(1)
// d.max_size()        -> theoretical max                O(1)
// d.resize(n)         -> change size                    O(n)
// d.clear()           -> remove all elements            O(n)
// d.shrink_to_fit()   -> release unused blocks          O(n)
//
// NOTE: deque has NO reserve() or capacity()!
// -> because memory is chunked, not single contiguous block

// ============================================================
// SECTION 5: YOUR CODE EXPLAINED
// ============================================================

void printDeque(const std::deque<int>& data)
{
    std::for_each(std::begin(data), std::end(data),
        [](const int& n) {
            std::puts(std::to_string(n).c_str());
            // std::puts = C-style fast print (faster than cout for single strings)
            // to_string converts int -> string
            // .c_str() converts string -> const char* for puts
            // equivalent to: std::cout << n << '\n'; (but slightly faster)
        });
    std::cout << "====================================\n";
}

int main()
{
    std::deque<int> deque1{1, 2, 3, 4};
    printDeque(deque1);  // 1 2 3 4

    // RANDOM ACCESS (like vector):
    std::cout << deque1[0]     << '\n';  // 1 - no bounds check
    std::cout << deque1.at(0)  << '\n';  // 1 - with bounds check

    // ITERATOR + INSERT:
    auto it = std::begin(deque1);
    std::cout << *it << '\n';           // 1 (first element)

    it = deque1.insert(it, 10);
    // insert 10 BEFORE it (before first element = at front)
    // insert returns iterator to the NEWLY inserted element
    // it now points to 10
    printDeque(deque1);  // 10 1 2 3 4

    // ADVANCE ITERATOR AND INSERT AT MIDDLE:
    std::deque<int>::iterator it2 = std::begin(deque1);
    std::advance(it2, 2);              // move to index 2 (element '1')
    deque1.insert(it2, 20);            // insert 20 before index 2
    printDeque(deque1);  // 10 1 20 2 3 4
                         // wait: after inserting 10 at front:
                         // {10,1,2,3,4}, advance 2 -> points to '2'
                         // insert 20 before '2': {10,1,20,2,3,4}

    // INSERT BEFORE LAST ELEMENT:
    auto end = std::end(deque1);
    std::advance(end, -1);             // move back 1 from end
    deque1.insert(end, 30);            // insert 30 before last element
    printDeque(deque1);  // 10 1 20 2 3 30 4

    // PUSH BACK:
    deque1.push_back(40);
    printDeque(deque1);  // 10 1 20 2 3 30 4 40

    return 0;
}

// ============================================================
// SECTION 6: ITERATOR INVALIDATION - IMPORTANT!
// ============================================================

// deque iterators are MORE FRAGILE than list iterators:
//
// push_front or push_back:
// -> ALL iterators INVALIDATED (block map may expand)
//
// insert at middle:
// -> ALL iterators INVALIDATED
//
// pop_front or pop_back:
// -> only iterators to erased element invalidated
// -> others remain valid
//
// CONTRAST:
// vector: all iterators invalid on reallocation
// list: iterators NEVER invalidated
// deque: invalidated on any insert, even at ends!
//
// RULE: re-obtain iterators after any insert operation
// YOUR CODE: it = deque1.insert(it, 10); <- catches returned iterator!

// ============================================================
// SECTION 7: WHY std::stack AND std::queue USE deque
// ============================================================

// std::stack<T> (LIFO):
// -> default underlying container = std::deque
// -> uses: push_back (push), pop_back (pop), back (top)
// -> could also use vector or list
//
// std::queue<T> (FIFO):
// -> default underlying container = std::deque
// -> uses: push_back (enqueue), pop_front (dequeue)
// -> CANNOT use vector (no O(1) pop_front)
// -> COULD use list but deque is more cache-friendly
//
// THIS is why deque exists: the perfect FIFO container
// O(1) push_back + O(1) pop_front = ideal queue operations

// ============================================================
// SECTION 8: DSA PATTERNS
// ============================================================

// PATTERN 1: BFS Queue (most common DSA use)
// std::deque<int> bfsQueue;
// bfsQueue.push_back(startNode);     // enqueue
// while (!bfsQueue.empty()) {
//     int node = bfsQueue.front();   // peek front
//     bfsQueue.pop_front();           // dequeue
//     for (auto neighbor : adj[node])
//         bfsQueue.push_back(neighbor); // enqueue neighbors
// }
//
// PATTERN 2: Sliding Window Maximum (classic deque problem)
// Keep indices of useful elements in deque
// std::deque<int> dq;  // stores indices
// for (int i = 0; i < n; i++) {
//     // remove elements outside window
//     while (!dq.empty() && dq.front() < i - k + 1)
//         dq.pop_front();
//     // remove smaller elements (useless for max)
//     while (!dq.empty() && nums[dq.back()] < nums[i])
//         dq.pop_back();
//     dq.push_back(i);
//     if (i >= k-1) result.push_back(nums[dq.front()]);
// }
//
// PATTERN 3: Work-Stealing Queue (multithreading)
// Thread pushes/pops from back (LIFO for cache efficiency)
// Other threads steal from front (FIFO)
// deque supports BOTH efficiently!
//
// PATTERN 4: Palindrome check
// std::deque<char> dq(str.begin(), str.end());
// while (dq.size() > 1) {
//     if (dq.front() != dq.back()) return false;
//     dq.pop_front();
//     dq.pop_back();
// }
// return true;

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::deque and how is it implemented?
//     -> Double-ended queue. Implemented as a dynamic array of
//        fixed-size memory blocks with a "map" of pointers to blocks.
//        Allows O(1) push/pop at both front and back, O(1) random access.

// Q2: What is the difference between deque and vector?
//     -> vector: single contiguous buffer, O(n) push_front.
//        deque: chunked memory, O(1) push_front AND push_back.
//        deque has no reserve(), no capacity(), no .data().
//        vector is more cache-friendly for sequential access.

// Q3: Why can't deque have reserve() or capacity()?
//     -> deque uses multiple fixed-size blocks, not one buffer.
//        There's no single "capacity" to report or reserve.
//        Each block is allocated independently as needed.

// Q4: What are the iterator invalidation rules for deque?
//     -> Any insert invalidates ALL iterators.
//        pop_front/pop_back only invalidate the removed element's iterator.
//        More fragile than list (never invalid) but similar to vector.

// Q5: Why do std::stack and std::queue use deque by default?
//     -> queue needs O(1) push_back + O(1) pop_front = deque is perfect.
//        stack needs O(1) push_back + O(1) pop_back = vector also works.
//        deque is the default because it handles both patterns efficiently.

// Q6: What is the sliding window maximum problem and why use deque?
//     -> Find max in each window of size k in an array.
//        Deque stores indices of POTENTIALLY useful elements.
//        Pop front when index leaves window (too old).
//        Pop back when new element is larger (previous useless).
//        O(n) solution because each element pushed/popped at most once.

// Q7: When should you NOT use deque?
//     -> When you need .data() for C API (deque not contiguous).
//        When sequential access performance is critical (vector better).
//        When you only need back operations (vector simpler).

// Q8: What does insert() return in deque?
//     -> An iterator to the NEWLY INSERTED element.
//        Critical to capture: it = d.insert(it, val)
//        Because insert invalidates all iterators - old it is now invalid!
//        The returned iterator is the only valid way to continue.
// ```

// **The deque memory model - why it beats vector at front:**

// ```
// VECTOR push_front(10):
// [10][1][2][3][4]   ← must SHIFT 1,2,3,4 right = O(n)

// DEQUE push_front(10):
// [block0: _,_,10] [block1: 1,2,3,4]
//                             ↑
//          just fills block0 from right to left = O(1)!
// ```

// **The sliding window maximum - deque's killer DSA pattern:**

// ```
// nums = [1,3,-1,-3,5,3,6,7], k=3

// i=0: dq=[0]                    (index of 1)
// i=1: dq=[1]                    (3>1, pop 0, push 1)
// i=2: dq=[1,2], max=nums[1]=3   (window complete)
// i=3: dq=[1,2,3], max=nums[1]=3 (-3 < all, just push)
// i=4: dq=[4], max=nums[4]=5     (5>all, pop 1,2,3, push 4)
// ...
// Result: [3,3,5,5,6,7]  ← O(n) using deque!
// ```

// > **The interview soundbite**: deque = "vector with O(1) front operations." Use it when you need **both ends efficient** — the classic use is BFS queue and sliding window problems. The chunked memory design trades some cache efficiency for the ability to grow at the front without copying everything.