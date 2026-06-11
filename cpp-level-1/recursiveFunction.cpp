// ============================================================
// RECURSION, STACK OVERFLOW, BASE CASE - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS RECURSION?
// ============================================================

// Recursion = a function that calls ITSELF
// Every recursive function needs TWO things:
//
// 1. BASE CASE    -> condition where recursion STOPS
//                   without this = infinite calls = stack overflow
//
// 2. RECURSIVE CASE -> function calls itself with SMALLER input
//                      must move TOWARDS the base case every time

int countdown(int n) {

    // BASE CASE - stops recursion
    if (n == 0) {
        std::cout << "0...blastoff!" << std::endl;
        return 0;
    }

    // RECURSIVE CASE - moves towards base case (n gets smaller each time)
    std::cout << n << std::endl;
    return countdown(n - 1);  // calls itself with n-1
}

// ============================================================
// SECTION 2: WHAT HAPPENS WHEN countdown(5) IS CALLED
// ============================================================

// CALL STACK grows like this (each call adds a frame):
//
// countdown(5) -> prints 5, calls countdown(4)
//   countdown(4) -> prints 4, calls countdown(3)
//     countdown(3) -> prints 3, calls countdown(2)
//       countdown(2) -> prints 2, calls countdown(1)
//         countdown(1) -> prints 1, calls countdown(0)
//           countdown(0) -> prints "0...blastoff!", returns 0
//         countdown(1) gets 0 back, returns 0
//       countdown(2) gets 0 back, returns 0
//     countdown(3) gets 0 back, returns 0
//   countdown(4) gets 0 back, returns 0
// countdown(5) gets 0 back, returns 0
//
// OUTPUT:
// 5
// 4
// 3
// 2
// 1
// 0...blastoff!
//
// STACK VISUAL (grows downward):
//
// |  countdown(0)  | <- top, last in, first out
// |  countdown(1)  |
// |  countdown(2)  |
// |  countdown(3)  |
// |  countdown(4)  |
// |  countdown(5)  |
// |  main()        | <- bottom, was here first
// |________________|
//
// After base case hit, stack UNWINDS back up (LIFO order)

// ============================================================
// SECTION 3: STACK OVERFLOW - countdown(5000000)
// ============================================================

// countdown(5000000) -> countdown(4999999) -> ... 5 million frames!
//
// Each stack frame uses memory (local variables + return address)
// Stack has a FIXED SIZE (typically 1MB - 8MB on most systems)
//
// 5,000,000 frames x ~32 bytes each = ~160MB needed
// Stack only has ~1-8MB available
// Result: STACK OVERFLOW -> program CRASHES
//
// INTERVIEW: What is a stack overflow?
//   -> When the call stack runs out of memory
//      Caused by: too deep recursion or infinite recursion
//      OS sends SIGSEGV (segmentation fault) -> program crashes

// ============================================================
// SECTION 4: WHAT IF BASE CASE IS MISSING?
// ============================================================

// int countdown(int n) {
//     std::cout << n << std::endl;
//     return countdown(n - 1);  // no base case!
// }
//
// countdown(10) -> countdown(9) -> ... -> countdown(0)
// -> countdown(-1) -> countdown(-2) -> FOREVER
// -> stack overflow guaranteed
//
// INTERVIEW: What happens if base case is missing in recursion?
//   -> Infinite recursion -> stack overflow -> program crash

// ============================================================
// SECTION 5: RECURSION vs ITERATION
// ============================================================

// RECURSIVE countdown:
// int countdown(int n) {
//     if (n == 0) { std::cout << "blastoff!"; return 0; }
//     std::cout << n << std::endl;
//     return countdown(n-1);
// }

// ITERATIVE countdown (same result, no stack risk):
// void countdown(int n) {
//     for (int i = n; i > 0; i--) {
//         std::cout << i << std::endl;
//     }
//     std::cout << "blastoff!" << std::endl;
// }
//
// | Feature        | Recursion          | Iteration        |
// |----------------|--------------------|------------------|
// | Stack usage    | O(n) - one frame   | O(1) - no frames |
// |                | per call           | added            |
// | Readability    | cleaner for some   | cleaner for      |
// |                | problems (trees)   | simple loops     |
// | Risk           | stack overflow     | infinite loop    |
// | Speed          | slower (function   | faster (no       |
// |                | call overhead)     | call overhead)   |
// | Best for       | trees, graphs,     | simple counting, |
// |                | divide & conquer   | linear problems  |

// ============================================================
// SECTION 6: TAIL RECURSION - important optimization
// ============================================================

// Your countdown IS tail recursive:
// return countdown(n-1);
// -> recursive call is the LAST thing the function does
// -> no work done after the recursive call returns
//
// Tail recursive = compiler CAN optimize it into a loop
// (called Tail Call Optimization - TCO)
// With TCO: no new stack frame created each call
// -> no stack overflow even for countdown(5000000)
//
// BUT: C++ standard does NOT guarantee TCO
//      gcc/clang DO optimize it with -O2 flag
//      without optimization flag -> still overflows
//
// INTERVIEW: What is tail recursion?
//   -> Recursive call is the last operation in the function
//      Compiler can convert it to iteration (TCO)
//      Avoids stack overflow in languages that guarantee TCO (Haskell, Scala)
//      C++ does not guarantee it but often does it with optimizations

// ============================================================
// SECTION 7: FAMOUS RECURSION EXAMPLES
// ============================================================

// FACTORIAL:
// int factorial(int n) {
//     if (n <= 1) return 1;        // base case
//     return n * factorial(n-1);   // recursive case
// }
// factorial(5) = 5 * 4 * 3 * 2 * 1 = 120

// FIBONACCI:
// int fib(int n) {
//     if (n <= 1) return n;           // base case
//     return fib(n-1) + fib(n-2);    // two recursive calls!
// }
// WARNING: this is O(2^n) - exponential time - very slow for large n
// fib(50) makes ~2^50 = 1 quadrillion calls!
// Fix: use memoization or iteration

// BINARY SEARCH (divide and conquer):
// int search(int arr[], int low, int high, int target) {
//     if (low > high) return -1;              // base case: not found
//     int mid = (low + high) / 2;
//     if (arr[mid] == target) return mid;     // base case: found
//     if (arr[mid] < target)
//         return search(arr, mid+1, high, target); // right half
//     return search(arr, low, mid-1, target);      // left half
// }

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What are the two required parts of a recursive function?
//     -> Base case (stopping condition) and
//        recursive case (calls itself with smaller input)

// Q2: What is a stack overflow?
//     -> Call stack runs out of memory due to too many
//        nested function calls (deep or infinite recursion)

// Q3: What is the difference between recursion and iteration?
//     -> Recursion: function calls itself, uses call stack O(n) space
//        Iteration: uses loops, O(1) stack space, generally faster

// Q4: What is tail recursion?
//     -> Recursive call is the LAST operation in the function
//        Compiler may optimize to avoid extra stack frames (TCO)

// Q5: When should you use recursion over iteration?
//     -> When problem is naturally recursive: trees, graphs,
//        divide and conquer, file system traversal
//        Avoid for simple linear problems (use loop instead)

// Q6: What is the time complexity of this countdown?
//     -> O(n) - one function call per number from n to 0

// Q7: What is the space complexity of this countdown?
//     -> O(n) - n stack frames created simultaneously

// Q8: Why does countdown(5000000) crash but countdown(10) works?
//     -> Stack size is limited (~1-8MB).
//        countdown(10) needs 10 frames (~320 bytes) - fine
//        countdown(5000000) needs 5M frames (~160MB) - stack overflow

int main() {
    // countdown(5000000);  // STACK OVERFLOW - do not run
    countdown(10);          // safe, prints 10 9 8 ... 1 blastoff!
    return 0;
}
// ```

// **The interview question that trips everyone:**

// > "What is the space complexity of recursion vs iteration for this problem?"
// >
// > Recursion = **O(n)** space — n frames sitting on stack simultaneously
// > Iteration = **O(1)** space — no stack frames, just one loop variable

// > And always remember: **missing base case = infinite recursion = stack overflow = crash**. Same guarantee, every time.