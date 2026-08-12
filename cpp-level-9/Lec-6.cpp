#include <iostream>
#include <stack>
#include <vector>
#include <deque>
#include <list>

// ============================================================
// std::stack - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::stack<T> = LIFO (Last In First Out) container adaptor
// "Last thing pushed = first thing popped"
//
// YOUR COMMENT: "push-add, pop-remove last, top-peek at top"
//
// REAL WORLD ANALOGY:
// Stack of plates: add to top, remove from top
// Call stack: last function called = first to return
// Undo history: last action = first to undo
// Bracket matching: last opened = first to close
//
// CONTAINER ADAPTOR - NOT a new data structure:
// stack is built ON TOP of another container
// it restricts the interface to ONLY stack operations
// YOUR QUESTION: "can stack be implemented through deque and array?"
// YES! Three underlying containers:
// std::stack<int>              -> uses deque (DEFAULT)
// std::stack<int, std::vector<int>>  -> uses vector
// std::stack<int, std::list<int>>    -> uses list
//
// WHY DEQUE BY DEFAULT?
// -> deque has O(1) push_back AND pop_back (like vector)
// -> deque has NO reallocation (unlike vector which copies on resize)
// -> stack uses push_back, pop_back, back from underlying container

// ============================================================
// SECTION 2: METHODS (ALL O(1)!)
// ============================================================

// push(val)    -> add element to TOP                    O(1) amort
// pop()        -> REMOVE element from TOP               O(1)
//                 IMPORTANT: returns void! (no return value)
//                 must call top() THEN pop() separately
// top()        -> PEEK at top element (returns reference) O(1)
//                 does NOT remove it
// empty()      -> YOUR QUESTION: "what does empty() return?"
//                 returns BOOL: true if size==0, false otherwise
// size()       -> number of elements                    O(1)
// swap(other)  -> swap contents with another stack      O(1)
//
// THAT'S IT - only 5 methods!
// NO: find, sort, begin, end, [], at, insert, erase
// Stack INTENTIONALLY restricts access to enforce LIFO discipline

// ============================================================
// SECTION 3: THE pop() DESIGN - IMPORTANT INTERVIEW QUESTION
// ============================================================

// WHY does pop() NOT return the value?
//
// EXCEPTION SAFETY REASON:
// If pop() returned T, and the copy constructor of T throws:
// -> element is removed from stack (pop happened)
// -> but copy failed (value lost forever!)
// -> stack lost data with no recovery
//
// SOLUTION: separate top() and pop()
// T val = myStack.top();  // get a copy FIRST (if this throws, stack unchanged)
// myStack.pop();           // THEN remove (no copy, can't throw)
//
// YOUR CODE SHOWS THE CORRECT PATTERN:
// int holdontoTopItem = myStack.top();  // copy top value
// myStack.pop();                         // then remove

// ============================================================
// SECTION 4: CONSTRUCTION
// ============================================================

// std::stack<int> s;               // empty stack
// std::stack<int> s({1,2,3,4});    // from initializer_list
//                                   -> 4 is at TOP (last inserted)
// std::stack<int> s2(s);           // copy
// std::stack<int, vector<int>> s3; // using vector as underlying container
//
// YOUR CODE: std::stack<int> myStack({-3,-2,-1,0});
// -> pushed in order: -3, -2, -1, 0
// -> TOP = 0 (last inserted = top of stack)
// -> BOTTOM = -3

// ============================================================
// SECTION 5: YOUR QUESTION - empty() RETURN VALUE
// ============================================================

// empty() returns BOOL:
// true  = stack is empty (size == 0)
// false = stack has elements
//
// COMMON USAGE PATTERN (your printStack uses this):
// while (!stack.empty()) {
//     process(stack.top());
//     stack.pop();
// }
// -> keep going while NOT empty
// -> empty() is the standard loop termination condition

// ============================================================
// SECTION 6: printStack - PASS BY VALUE (IMPORTANT!)
// ============================================================

void printStack(std::stack<int> copy)
//              ^^^^^^^^^^^^^^^
//              PASS BY VALUE = creates a COPY of the stack
//              so popping from 'copy' doesn't affect the original!
{
    std::cout << "=====================\n";
    while (!copy.empty())           // loop until copy is empty
    {
        std::cout << "Result: " << copy.top() << '\n';
        copy.pop();                  // removes from copy, not original
    }
    // copy destroyed here (local), original stack unchanged
}
// WHY BY VALUE? Because popping elements to print would
// DESTROY the original stack! By copying, we consume the
// copy instead. This is the standard stack printing trick.

// ============================================================
// SECTION 7: main() TRACE
// ============================================================

int main()
{
    std::stack<int> myStack({-3, -2, -1, 0});
    // Stack state (top to bottom): 0, -1, -2, -3
    //                               ^TOP

    myStack.push(1);  // top: 1
    myStack.push(2);  // top: 2
    myStack.push(3);  // top: 3
    // Stack (top to bottom): 3, 2, 1, 0, -1, -2, -3

    std::cout << "top: " << myStack.top()  << '\n';  // 3
    std::cout << "size: "<< myStack.size() << '\n';  // 7

    int holdontoTopItem = myStack.top();  // holdontoTopItem = 3
    std::cout << "removed: " << holdontoTopItem << '\n';  // 3
    myStack.pop();   // removes 3, top is now 2

    std::cout << "top: " << myStack.top() << '\n';  // 2

    // empty() answer: returns bool
    std::cout << "empty? " << myStack.empty() << '\n';  // 0 (false)

    printStack(myStack);
    // prints: 2, 1, 0, -1, -2, -3 (top to bottom)
    // original myStack UNCHANGED (passed by value to printStack)

    return 0;
}

// ============================================================
// SECTION 8: UNDERLYING CONTAINER COMPARISON
// ============================================================

// std::stack<int>                     -> deque (default)
// std::stack<int, std::deque<int>>    -> same
// std::stack<int, std::vector<int>>   -> vector
// std::stack<int, std::list<int>>     -> list
//
// PERFORMANCE:
// vector-based: contiguous memory, best cache performance
//               reallocation can be O(n) occasionally
// deque-based:  chunked memory, no reallocation (default)
//               slightly worse cache than vector
// list-based:   linked nodes, heap alloc per push, cache worst
//               constant time always (no reallocation ever)
//
// IN PRACTICE: default deque is fine for most use cases
// For maximum performance in hot loops: vector-based stack

// ============================================================
// SECTION 9: DSA PATTERNS - WHERE STACK IS ESSENTIAL
// ============================================================

// PATTERN 1: Balanced Parentheses / Bracket Matching
// string s = "({[]})";
// std::stack<char> st;
// for (char c : s) {
//     if (c=='(' || c=='{' || c=='[') st.push(c);
//     else {
//         if (st.empty()) return false;
//         char top = st.top(); st.pop();
//         if (c==')' && top!='(') return false;
//         if (c=='}' && top!='{') return false;
//         if (c==']' && top!='[') return false;
//     }
// }
// return st.empty();

// PATTERN 2: DFS (Depth First Search) - iterative
// std::stack<int> dfs;
// dfs.push(start);
// while (!dfs.empty()) {
//     int node = dfs.top(); dfs.pop();
//     for (int neighbor : adj[node])
//         if (!visited[neighbor]) dfs.push(neighbor);
// }

// PATTERN 3: Valid Parentheses / Min Stack
// Keep track of minimum at each level:
// std::stack<pair<int,int>> minStack;  // {value, minSoFar}
// minStack.push({val, min(val, minStack.top().second)});
// getMin() = minStack.top().second;

// PATTERN 4: Evaluate expression (Reverse Polish Notation)
// "2 3 + 4 *" = (2+3)*4 = 20
// std::stack<int> eval;
// for (token) {
//     if (isNumber) eval.push(n);
//     else { int b=top,pop; int a=top,pop; eval.push(a op b); }
// }
// return eval.top();

// PATTERN 5: Undo/Redo system
// std::stack<Action> undoStack;
// std::stack<Action> redoStack;
// doAction(a): undoStack.push(a), redoStack={}
// undo(): move undoStack.top() to redoStack
// redo(): move redoStack.top() to undoStack

// PATTERN 6: Next Greater Element
// std::stack<int> st;
// for (int i = n-1; i >= 0; i--) {
//     while (!st.empty() && st.top() <= nums[i]) st.pop();
//     result[i] = st.empty() ? -1 : st.top();
//     st.push(nums[i]);
// }

// PATTERN 7: Inorder traversal without recursion
// std::stack<TreeNode*> st;
// TreeNode* curr = root;
// while (curr || !st.empty()) {
//     while (curr) { st.push(curr); curr = curr->left; }
//     curr = st.top(); st.pop();
//     visit(curr);
//     curr = curr->right;
// }

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::stack and what underlying containers can it use?
//     -> LIFO container adaptor. Default: deque.
//        Can use: vector, list, deque as underlying container.
//        vector: best cache, deque: no reallocation, list: always O(1).

// Q2: Why does pop() return void instead of the popped value?
//     -> Exception safety. If return T and copy constructor throws:
//        element is removed but value is lost (unrecoverable).
//        Solution: top() to copy, THEN pop() to remove separately.

// Q3: What does empty() return?
//     -> bool: true if stack is empty (size==0), false otherwise.
//        Standard loop pattern: while(!s.empty()) { top(); pop(); }

// Q4: Why pass stack by value to print it?
//     -> To print, you pop all elements (destructive operation).
//        Passing by value creates a copy, so the original is preserved.
//        This is the standard trick for printing stacks.

// Q5: What is the difference between top() and pop()?
//     -> top(): returns reference to top element, does NOT remove.
//        pop(): removes top element, returns VOID (no value).
//        Must use both: val = top(); pop();

// Q6: What DSA problems commonly use a stack?
//     -> Balanced parentheses, DFS iterative, expression evaluation,
//        next greater element, undo/redo, inorder traversal, min stack.

// Q7: What is a monotonic stack?
//     -> Stack that maintains elements in sorted order (increasing or
//        decreasing). Used for next greater/smaller element problems.
//        Pop elements that violate the monotonic property during push.

// Q8: Can you implement a queue using two stacks?
//     -> YES. Push to stack1. Pop from stack2.
//        When stack2 empty: move all stack1 to stack2 (reverse order).
//        Enqueue: O(1). Dequeue: O(1) amortized.
// ```

// **Stack state trace:**

// ```
// After std::stack<int> myStack({-3,-2,-1,0}):
// TOP →  [0]
//        [-1]
//        [-2]
// BOTTOM [-3]

// After push(1), push(2), push(3):
// TOP →  [3]   ← top() returns 3
//        [2]
//        [1]
//        [0]
//        [-1]
//        [-2]
// BOTTOM [-3]   size() = 7

// After pop():
// TOP →  [2]   ← new top
//        [1]
//        [0]
//        [-1]
//        [-2]
// BOTTOM [-3]
// ```

// **The pop() return void - the why:**

// ```cpp
// // BROKEN (hypothetical):
// T val = myStack.pop();  // what if T copy throws here?
//                         // element is GONE but val construction FAILED
//                         // data lost forever!

// // SAFE (actual design):
// T val = myStack.top();  // copy first - if throws, stack unchanged
// myStack.pop();          // then remove - no copy, no throw possible
// ```

// > **The DSA golden rule**: when you see **"last in first out"**, **"undo"**, **"backtracking"**, **"matching pairs"**, or **"DFS"** in a problem — think stack first. The six patterns above (balanced brackets, DFS, min stack, RPN, next greater element, tree traversal) cover the vast majority of stack-based interview problems.