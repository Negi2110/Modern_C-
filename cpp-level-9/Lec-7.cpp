#include <iostream>
#include <queue>
#include <list>
#include <deque>
#include <vector>

// ============================================================
// std::queue - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::queue<T> = FIFO (First In First Out) container adaptor
// YOUR COMMENT: "first in first out, as opposed to stack which is LIFO"
//
// REAL WORLD ANALOGY:
// Queue at a bank: first person in line = first served
// Printer queue: first document sent = first printed
// BFS traversal: first node discovered = first explored
// OS process scheduler: first process queued = first executed
//
// YOUR QUESTION: "is it deque based?"
// YES! Default underlying container = std::deque
// But can use others:
// std::queue<int>               -> deque (DEFAULT)
// std::queue<int, std::list<int>> -> list  (YOUR CODE!)
// std::queue<int, std::deque<int>>-> deque (explicit)
// CANNOT use vector (no pop_front, which queue needs for pop())
//
// WHY DEQUE BY DEFAULT?
// -> O(1) push_back  (for queue::push)
// -> O(1) pop_front  (for queue::pop)
// -> vector has no O(1) pop_front (would need O(n) shift)
// -> list works but worse cache performance than deque

// ============================================================
// SECTION 2: stack vs queue - THE KEY DIFFERENCE
// ============================================================

// STACK (LIFO):
// push -> adds to TOP
// pop  -> removes from TOP
// [1][2][3] push 4 -> [1][2][3][4]
//                                 ^ top
// pop -> removes 4 (last in = first out)
//
// QUEUE (FIFO):
// push -> adds to BACK
// pop  -> removes from FRONT
// [1][2][3] push 4 -> [1][2][3][4]
//  ^ front           ^ front    ^ back
// pop -> removes 1 (first in = first out)
//
// VISUAL:
// Stack: add/remove SAME end (top)
// Queue: add ONE end (back), remove OTHER end (front)

// ============================================================
// SECTION 3: METHODS (ALL O(1))
// ============================================================

// push(val)   -> add to BACK of queue                 O(1) amort
// pop()       -> REMOVE from FRONT                    O(1)
//                returns VOID (same design as stack!)
//                YOUR COMMENT: "pop does not return deleted item"
// front()     -> peek at FRONT element (NOT removed)  O(1)
// back()      -> peek at BACK element  (NOT removed)  O(1)
// empty()     -> true if size==0                      O(1)
// size()      -> number of elements                   O(1)
// swap(other) -> swap with another queue              O(1)
//
// THAT'S IT - only 6 methods!
// NO: find, sort, begin, end, [], at, insert, erase
// Queue restricts access to enforce FIFO discipline

// ============================================================
// SECTION 4: YOUR QUESTION - pop() DOESN'T RETURN VALUE
// ============================================================

// YOUR COMMENT: "in queue pop does not return delete item like
//                other STL"
//
// SAME DESIGN AS STACK - same reason:
// EXCEPTION SAFETY: if pop() returned T and copy constructor throws:
// -> element removed from queue
// -> copy failed -> value LOST FOREVER
//
// CORRECT PATTERN:
// int val = q.front(); // copy FIRST (if throws, queue unchanged)
// q.pop();              // THEN remove (no copy possible, safe)
//
// NEVER do:
// q.pop();              // value gone forever!
//                       // can't recover it

// ============================================================
// SECTION 5: CONSTRUCTION
// ============================================================

// std::queue<int> q;               // empty queue
// std::queue<int> q({1,2,3});      // from initializer_list
//                                   -> 1 = FRONT, 3 = BACK
// std::queue<int> q2(q);           // copy
// std::queue<int,std::list<int>> q3({1,2,3}); // list-based (YOUR CODE)
//
// YOUR CODE: std::queue<int, std::list<int>> q({-3,-2,1});
// -> -3 = FRONT (first in, first out)
// -> 1  = BACK
// -> uses std::list as underlying container

// ============================================================
// SECTION 6: main() TRACE
// ============================================================

int main()
{
    std::queue<int, std::list<int>> q({-3, -2, 1});
    // FRONT [-3][-2][1] BACK
    // using std::list (YOUR CODE - not default deque)

    q.push(1);   // FRONT [-3][-2][1][1] BACK
    q.push(2);   // FRONT [-3][-2][1][1][2] BACK
    q.push(3);   // FRONT [-3][-2][1][1][2][3] BACK
    q.push(4);   // FRONT [-3][-2][1][1][2][3][4] BACK

    std::cout << "back:  " << q.back()  << '\n';  // 4 (last pushed)
    std::cout << "size:  " << q.size()  << '\n';  // 7

    // DRAIN THE QUEUE (FIFO order):
    while (!q.empty())
    {
        std::cout << "front: " << q.front() << '\n';
        q.pop();    // removes from FRONT (not back!)
    }
    // PRINTS IN ORDER: -3, -2, 1, 1, 2, 3, 4
    // (exact order they were added - FIFO!)
    // CONTRAST with stack: would print 4, 3, 2, 1, 1, -2, -3

    return 0;
}

// ============================================================
// SECTION 7: UNDERLYING CONTAINER COMPARISON
// ============================================================

// std::queue<int>                      -> deque (DEFAULT)
// std::queue<int, std::deque<int>>     -> same as default
// std::queue<int, std::list<int>>      -> YOUR CODE
// std::queue<int, std::vector<int>>    -> COMPILE ERROR!
//                                          vector has no pop_front
//
// WHY LIST WORKS:
// queue needs: push_back (for push), pop_front (for pop),
//              front(), back()
// list has all of these as O(1)
//
// DEQUE vs LIST for queue:
// deque: contiguous blocks, better cache performance (DEFAULT choice)
// list:  linked nodes, O(1) guaranteed but cache unfriendly
//        each node = heap allocation (slower in practice)
//
// IN PRACTICE: default deque queue is almost always the right choice

// ============================================================
// SECTION 8: DSA PATTERNS - WHERE QUEUE IS ESSENTIAL
// ============================================================

// PATTERN 1: BFS (Breadth First Search) - THE classic queue use
// std::queue<int> bfs;
// bfs.push(start);
// visited[start] = true;
// while (!bfs.empty()) {
//     int node = bfs.front(); bfs.pop();
//     for (int neighbor : adj[node]) {
//         if (!visited[neighbor]) {
//             visited[neighbor] = true;
//             bfs.push(neighbor);
//         }
//     }
// }
// -> finds SHORTEST PATH in unweighted graph

// PATTERN 2: Level-order traversal (binary tree)
// std::queue<TreeNode*> q;
// q.push(root);
// while (!q.empty()) {
//     int levelSize = q.size();       // nodes at current level
//     for (int i = 0; i < levelSize; i++) {
//         TreeNode* node = q.front(); q.pop();
//         if (node->left)  q.push(node->left);
//         if (node->right) q.push(node->right);
//     }
//     // process one level at a time
// }

// PATTERN 3: Sliding window with queue (monotonic queue)
// std::deque<int> dq;  // stores indices
// for (int i = 0; i < n; i++) {
//     while (!dq.empty() && dq.front() < i-k+1) dq.pop_front();
//     while (!dq.empty() && nums[dq.back()] < nums[i]) dq.pop_back();
//     dq.push_back(i);
//     if (i >= k-1) result.push_back(nums[dq.front()]);
// }

// PATTERN 4: Task scheduling / Round Robin
// std::queue<Task> taskQueue;
// taskQueue.push(task1);
// taskQueue.push(task2);
// while (!taskQueue.empty()) {
//     Task t = taskQueue.front(); taskQueue.pop();
//     execute(t);
//     if (!t.done()) taskQueue.push(t); // re-add if not finished
// }

// PATTERN 5: Producer-Consumer (multithreading)
// std::queue<int> buffer;
// std::mutex mtx;
// Producer: { lock_guard l(mtx); buffer.push(item); }
// Consumer: { lock_guard l(mtx); item=buffer.front(); buffer.pop(); }

// PATTERN 6: Number of islands BFS
// std::queue<pair<int,int>> q;  // {row, col}
// q.push({r, c});
// while (!q.empty()) {
//     auto [row, col] = q.front(); q.pop();
//     for (auto [dr,dc] : directions) {
//         int nr=row+dr, nc=col+dc;
//         if (valid(nr,nc) && grid[nr][nc]=='1') {
//             grid[nr][nc]='0';
//             q.push({nr,nc});
//         }
//     }
// }

// ============================================================
// SECTION 9: QUEUE vs STACK vs DEQUE - WHEN TO USE
// ============================================================

// USE stack WHEN:
// -> LIFO order needed
// -> DFS traversal
// -> undo/redo
// -> balanced parentheses
// -> call stack simulation
//
// USE queue WHEN:
// -> FIFO order needed
// -> BFS traversal
// -> level-order processing
// -> task scheduling
// -> first-come-first-served
//
// USE deque WHEN:
// -> need BOTH ends: push/pop front AND back
// -> sliding window problems
// -> need indexed access (deque has operator[])
// -> more flexible than both stack and queue

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::queue and what is it FIFO?
//     -> Container adaptor for FIFO access.
//        push() adds to BACK, pop() removes from FRONT.
//        First element pushed = first element received.
//        Default underlying container: std::deque.

// Q2: What underlying containers can queue use?
//     -> deque (default), list. NOT vector (no pop_front).
//        queue requires: push_back, pop_front, front(), back().
//        vector lacks O(1) pop_front so it doesn't work.

// Q3: Why does pop() not return the value?
//     -> Exception safety. If return T and copy throws:
//        element is removed but value is lost forever.
//        Use front() to get value, THEN pop() to remove.

// Q4: What is the difference between front() and back()?
//     -> front(): element that will be popped NEXT (oldest).
//        back(): element most recently pushed (newest).
//        pop() removes from front, push() adds to back.

// Q5: What is the most important DSA use of queue?
//     -> BFS (Breadth First Search).
//        Processes nodes level by level (shortest path).
//        Queue ensures first discovered = first processed = FIFO.

// Q6: How do you print a queue without destroying it?
//     -> Pass by value (copy the queue):
//        void print(std::queue<int> copy) { while(!copy.empty()) { ... } }
//        Same trick as for stack.

// Q7: What is the difference between std::queue and std::deque?
//     -> queue: adaptor restricts to FIFO (push back, pop front only).
//        deque: full container with access at both ends AND random access.
//        queue is BUILT ON TOP of deque by default.

// Q8: Can you implement a stack using two queues?
//     -> YES. Push: push to q1. Pop: move all but last to q2,
//        pop last from q1, swap q1 and q2.
//        Push O(1), Pop O(n).
//        OR: Push O(n) by rotating, Pop O(1).
// ```

// **Queue vs Stack - the one visual:**

// ```
// STACK (LIFO):              QUEUE (FIFO):
// push(1) push(2) push(3)   push(1) push(2) push(3)

//     [3] ← TOP               FRONT [1][2][3] BACK
//     [2]                           ↑         ↑
//     [1] ← BOTTOM              pop here  push here

// pop() gives: 3,2,1         pop() gives: 1,2,3
// (last in first out)        (first in first out)
// ```

// **The standard drain pattern:**

// ```cpp
// // SAFE pattern (front then pop):
// while (!q.empty()) {
//     int val = q.front();  // peek first
//     q.pop();              // then remove
//     process(val);
// }

// // WRONG (pop returns void - can't do this):
// // int val = q.pop();  // COMPILE ERROR: void return!
// ```

// > **The BFS connection**: queue is THE data structure for BFS. The reason BFS finds the **shortest path** in unweighted graphs is directly because of FIFO order — nodes are processed in the order they were discovered, which means closer nodes (fewer hops) are always processed before farther nodes. If you used a stack instead, you'd get DFS. The choice of queue vs stack literally determines BFS vs DFS.