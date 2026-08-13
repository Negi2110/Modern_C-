#include <iostream>
#include <queue>
#include <vector>
#include <functional>

// ============================================================
// std::priority_queue - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::priority_queue<T, Container, Compare> =
// container adaptor that always gives you the HIGHEST priority element
// YOUR COMMENT: "takes the highest (by default) element first"
//
// NOT true FIFO/LIFO - ordering by PRIORITY (value)
// push() -> adds element in correct priority position
// top()  -> gives HIGHEST priority element
// pop()  -> removes HIGHEST priority element
//
// IMPLEMENTED AS: BINARY HEAP (max-heap by default)
// -> complete binary tree where parent >= children
// -> top() is ALWAYS the maximum (O(1))
// -> push/pop maintain heap property (O(log n))
//
// DEFAULT: MAX HEAP (largest value on top)
// std::priority_queue<int> pq;
// push(32), push(33), push(31)
// top() = 33 (largest)
// pop order: 33, 32, 31

// ============================================================
// SECTION 2: YOUR QUESTION - MAX HEAP vs MIN HEAP - HOW?
// ============================================================

// HOW HEAP WORKS INTERNALLY:
//
// MAX HEAP (default): parent >= both children
// push(31), push(33), push(32):
//       33           <- always max at root
//      /  \
//    31    32
// top() = 33
//
// AFTER pop() (33 removed, heap restructured):
//       32
//      /
//    31
// top() = 32
//
// MIN HEAP: parent <= both children (smallest at root)
//       31           <- always min at root
//      /  \
//    33    32
// top() = 31
//
// THE COMPARATOR CONTROLS WHICH IS TOP:
// comparator(a, b) returns true -> b has HIGHER priority than a
// -> "a should come BEFORE b in removal order" = false
// -> b gets removed first
//
// MAX HEAP: use std::less<T> (default)
//   less(a,b) = a < b = true -> b has higher priority
//   -> larger values removed first
//
// MIN HEAP: use std::greater<T>
//   greater(a,b) = a > b = true -> b has higher priority
//   -> smaller values removed first

// ============================================================
// SECTION 3: YOUR QUESTION - HOW DOES EdgeCompare WORK?
// ============================================================

struct Edge
{
    int v1, v2;
    Edge(int _v1, int _v2) : v1(_v1), v2(_v2) {}
    friend std::ostream& operator<<(std::ostream& os, const Edge& e);
};

std::ostream& operator<<(std::ostream& os, const Edge& e) {
    os << e.v1 << "," << e.v2;
    return os;
}

struct EdgeCompare
{
    // YOUR QUESTION: "explain how this function is working"
    //
    // RULE: comparator(a, b) returns true
    //       means: "b has HIGHER priority than a"
    //       means: "b should be at the TOP (popped first)"
    //
    bool operator()(const Edge& e1, const Edge& e2) const
    {
        int e1Sum = e1.v1 + e1.v2;  // e1 total weight
        int e2Sum = e2.v1 + e2.v2;  // e2 total weight

        return e1Sum < e2Sum;
        // e1Sum < e2Sum returns TRUE
        // -> e2 has HIGHER priority than e1
        // -> e2 comes out first
        // -> LARGER sum = HIGHER priority = MAX HEAP BY SUM
    }
};

// TRACE WITH YOUR DATA:
// e1(1,1) sum=2, e2(2,2) sum=4, e3(3,2) sum=5
//
// compare(e1,e2): 2<4=true  -> e2 higher priority than e1
// compare(e1,e3): 2<5=true  -> e3 higher priority than e1
// compare(e2,e3): 4<5=true  -> e3 higher priority than e2
//
// HEAP ORDER: e3(sum=5) at top, then e2(sum=4), then e1(sum=2)
// pop order:  e3, e2, e1 (highest sum first = MAX HEAP)

// ============================================================
// SECTION 4: HOW TO MAKE MIN HEAP FOR CUSTOM TYPE
// ============================================================

// YOUR QUESTION: "how to implement max/min heap compare func
//                for custom data type?"
//
// CURRENT (MAX HEAP - larger sum first):
// return e1Sum < e2Sum;
// -> "e1 is LESS THAN e2" -> e2 wins -> larger wins = MAX HEAP
//
// FOR MIN HEAP (smaller sum first):
// return e1Sum > e2Sum;
// -> "e1 is GREATER THAN e2" -> e2 wins -> smaller wins = MIN HEAP
//
// MEMORY TRICK:
// same as sort comparator:
// sort ascending (small first)  -> return a < b  -> for priority_queue: MIN HEAP? NO!
// IT'S THE OPPOSITE:
// priority_queue pops the element that LOSES comparison
// return a < b means a loses -> b pops first -> MAX heap
// return a > b means a loses -> b pops first -> MIN heap
//
// SIMPLER RULE:
// "what comes out FIRST should LOSE the comparison"
// MAX heap: smaller loses (return a < b)
// MIN heap: larger loses  (return a > b)

// ============================================================
// SECTION 5: THREE WAYS TO CREATE PRIORITY QUEUES
// ============================================================

// FOR int (and basic types):
//
// MAX HEAP (default):
// std::priority_queue<int> maxPQ;
// push: 32,33,31 -> top: 33 (max first)
//
// MIN HEAP:
// std::priority_queue<int, std::vector<int>, std::greater<int>> minPQ;
// push: 32,33,31 -> top: 31 (min first)
//
// FOR CUSTOM TYPES:
//
// MAX HEAP by sum:
// std::priority_queue<Edge, std::vector<Edge>, EdgeCompare> maxEdge;
// (return e1Sum < e2Sum in comparator)
//
// MIN HEAP by sum:
// struct EdgeMinCompare {
//     bool operator()(const Edge& e1, const Edge& e2) const {
//         return (e1.v1+e1.v2) > (e2.v1+e2.v2);  // > for min heap
//     }
// };
// std::priority_queue<Edge, std::vector<Edge>, EdgeMinCompare> minEdge;

// ============================================================
// SECTION 6: METHODS (template: T, Container, Compare)
// ============================================================

// push(val)  -> insert with correct priority              O(log n)
// pop()      -> REMOVE highest priority element           O(log n)
//               returns VOID (same as stack/queue!)
// top()      -> PEEK at highest priority (not removed)    O(1)
// empty()    -> true if empty                             O(1)
// size()     -> number of elements                        O(1)
//
// NO: find, sort, begin, end, [], at
// Can use deque or vector as underlying container (YOUR CODE uses deque)
// DEFAULT container = std::vector

// ============================================================
// SECTION 7: main() TRACE
// ============================================================

int main()
{
    // CUSTOM priority_queue with deque (YOUR CODE):
    std::priority_queue<Edge, std::deque<Edge>, EdgeCompare> edges;

    Edge e1(1, 1);  // sum = 2
    Edge e2(2, 2);  // sum = 4
    Edge e3(3, 2);  // sum = 5

    edges.push(e1);   // heap: [e1]
    edges.push(e2);   // heap: [e2, e1] (e2 sum=4 > e1 sum=2)
    edges.push(e3);   // heap: [e3, e1, e2] (e3 sum=5 = max)

    while (!edges.empty()) {
        Edge topEdge = edges.top();
        std::cout << "Top: " << topEdge << '\n';
        edges.pop();
    }
    // OUTPUT:
    // Top: 3,2   (sum=5, highest)
    // Top: 2,2   (sum=4, second)
    // Top: 1,1   (sum=2, lowest)

    // BASIC INT EXAMPLE (your commented code):
    // std::priority_queue<int> pQueue;  // max heap
    // pQueue.push(32);
    // pQueue.push(33);
    // pQueue.push(31);
    // top order: 33, 32, 31 (max first)

    return 0;
}

// ============================================================
// SECTION 8: HEAP OPERATIONS VISUALIZED
// ============================================================

// PUSH into max heap:
// Start: empty
// push(31):    31         (single element)
// push(33):    33         (33>31, becomes root)
//             /
//           31
// push(32):    33
//             /  \
//           31    32      (32<33, placed as child)
//
// POP from max heap:
// top() = 33 (root)
// pop():
// 1. remove root (33)
// 2. move last element (32) to root
// 3. "sift down" to restore heap:
//       32               31
//      /          ->    /
//    31                32   WRONG, let me redo:
//                       Actually: 32 >= 31, so:
//       32               stays as root (32>31)
//      /
//    31
// Result: top() = 32

// ============================================================
// SECTION 9: DSA PATTERNS WITH PRIORITY QUEUE
// ============================================================

// PATTERN 1: Dijkstra's shortest path
// using State = pair<int,int>;  // {distance, node}
// priority_queue<State, vector<State>, greater<State>> pq; // MIN heap
// pq.push({0, source});
// while (!pq.empty()) {
//     auto [dist, node] = pq.top(); pq.pop();
//     for (auto [neighbor, weight] : adj[node]) {
//         if (dist + weight < distances[neighbor]) {
//             distances[neighbor] = dist + weight;
//             pq.push({distances[neighbor], neighbor});
//         }
//     }
// }

// PATTERN 2: K largest elements
// priority_queue<int, vector<int>, greater<int>> minHeap; // SIZE k min heap
// for (int n : nums) {
//     minHeap.push(n);
//     if (minHeap.size() > k) minHeap.pop(); // remove smallest
// }
// top() = kth largest!

// PATTERN 3: Merge K sorted arrays
// using T = pair<int, pair<int,int>>;  // {value, {arrayIdx, elemIdx}}
// priority_queue<T, vector<T>, greater<T>> pq; // min heap
// for each array: push first element
// while (!pq.empty()) { take min, push next from same array }

// PATTERN 4: Find median (two heaps)
// priority_queue<int> maxHeap;              // left half (max at top)
// priority_queue<int,vector<int>,greater<int>> minHeap; // right half (min at top)
// median = maxHeap.top() (or average of both tops)

// PATTERN 5: Task scheduler / CPU scheduling
// using Task = pair<int, string>;  // {priority, taskName}
// priority_queue<Task> scheduler;
// scheduler.push({5, "high priority"});
// scheduler.push({1, "low priority"});
// auto [pri, task] = scheduler.top(); // always highest priority

// PATTERN 6: Prim's MST (minimum spanning tree)
// using Edge = pair<int,int>;  // {weight, vertex}
// priority_queue<Edge, vector<Edge>, greater<Edge>> pq; // min heap
// pq.push({0, start});
// while (!pq.empty()) {
//     auto [weight, u] = pq.top(); pq.pop();
//     if (visited[u]) continue;
//     mstCost += weight; visited[u] = true;
//     for (auto [v, w] : adj[u]) if (!visited[v]) pq.push({w, v});
// }

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::priority_queue and how does it work internally?
//     -> Container adaptor that always provides the highest priority
//        element. Implemented as a binary heap.
//        Max-heap by default: largest element always at top.
//        push/pop: O(log n). top: O(1).

// Q2: How do you create a min-heap vs max-heap?
//     -> Max (default): priority_queue<int>
//        Min: priority_queue<int, vector<int>, greater<int>>
//        Custom: write comparator returning true when first arg
//        should have LOWER priority than second.

// Q3: How does the custom comparator work?
//     -> comp(a, b) returns true means b has HIGHER priority.
//        return a < b: larger b wins = MAX HEAP.
//        return a > b: smaller b wins = MIN HEAP.
//        "What comes out first should LOSE the comparison."

// Q4: What is the time complexity of push and pop?
//     -> Both O(log n) - must maintain heap property.
//        top() is O(1) - just return root.

// Q5: Why use pair<int,int> with priority_queue in Dijkstra?
//     -> {distance, node} pair. pair comparison is lexicographic:
//        compares first element (distance) first.
//        With greater<pair>: smallest distance = highest priority.
//        Natural way to implement distance-based priority.

// Q6: What is the difference between priority_queue and multiset?
//     -> priority_queue: O(log n) push/pop, O(1) top, no iteration.
//        multiset: O(log n) all ops, full iteration, erase specific.
//        priority_queue is faster for push/pop (heap vs tree).
//        multiset when you need to remove specific elements.

// Q7: Can you use deque as underlying container for priority_queue?
//     -> YES (YOUR CODE does this). Both vector and deque work.
//        vector is default (better cache performance usually).
//        deque avoids reallocation but has worse cache behavior.

// Q8: What are the two-heap technique and what problem does it solve?
//     -> Maintain two heaps: max-heap for lower half, min-heap for upper.
//        Max-heap top = median candidate (or one of two for even count).
//        Used for: Find Median from Data Stream (O(log n) insert, O(1) median).
// ```

// **Max heap vs min heap - the comparator rule:**

// ```cpp
// // MAX HEAP (largest first):
// return e1Sum < e2Sum;   // e1 "loses" when smaller → larger wins

// // MIN HEAP (smallest first):
// return e1Sum > e2Sum;   // e1 "loses" when larger → smaller wins

// // MEMORY TRICK:
// // "The element that should come out LAST loses the comparison"
// // MAX heap → small values lose → return a < b
// // MIN heap → large values lose → return a > b
// ```

// **Heap visualization:**

// ```
// push(31), push(33), push(32) into MAX HEAP:

//        33          ← always max at root (O(1) access)
//       /  \
//     31    32

// pop(): removes 33, restructures:
//        32
//       /
//     31

// top() returns: 33, then 32, then 31
// ```

// **The two most important DSA uses:**

// ```cpp
// // DIJKSTRA (min heap - shortest distance first):
// priority_queue<pair<int,int>, vector<pair<int,int>>,
//                greater<pair<int,int>>> pq;
// pq.push({0, source});  // {distance, node}

// // K LARGEST (min heap of size k):
// priority_queue<int, vector<int>, greater<int>> minHeap;
// for (int n : nums) {
//     minHeap.push(n);
//     if (minHeap.size() > k) minHeap.pop();
// }
// // minHeap.top() = kth largest element!
// ```

// > **The key insight for interviews**: `priority_queue` comparator works **opposite** to `sort` comparator. In `sort`, `comp(a,b)=true` means "a comes before b in output". In `priority_queue`, `comp(a,b)=true` means "b has higher priority than a" (b comes out first). This reversal trips up most candidates — the safest approach is to remember: **return `a < b` for max-heap, return `a > b` for min-heap**.