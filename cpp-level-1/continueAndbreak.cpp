
// ============================================================
// break vs continue - NESTED LOOPS - INTERVIEW NOTES
// ============================================================

#include <iostream>

int main()
{
    // ============================================================
    // SECTION 1: WHAT break AND continue DO
    // ============================================================

    // break;
    // -> EXITS the loop immediately
    // -> No more iterations, jumps OUT of the loop body
    // -> In nested loops -> only exits the INNERMOST loop
    //    outer loop continues normally

    // continue;
    // -> SKIPS rest of current iteration
    // -> Goes back to loop condition check / increment
    // -> Does NOT exit the loop, just skips THIS iteration

    // ============================================================
    // SECTION 2: YOUR CODE - WHAT ACTUALLY HAPPENS
    // ============================================================

    for (int x = 0; x < 10; x++)
    {
        for (int i = 0; i < 10; i++)
        {
            break;                                          // hits immediately
            std::cout << "start of loop" << std::endl;    // NEVER runs
            std::cout << "i is:" << i << std::endl;       // NEVER runs
        }
        std::cout << "x is:" << x << std::endl;           // ALWAYS runs
    }

    // EXECUTION TRACE:
    //
    // x=0: inner loop starts -> i=0 -> break -> inner loop DEAD
    //      prints "x is: 0"
    // x=1: inner loop starts -> i=0 -> break -> inner loop DEAD
    //      prints "x is: 1"
    // x=2: inner loop starts -> i=0 -> break -> inner loop DEAD
    //      prints "x is: 2"
    // ... same for x=3 to x=9
    //
    // OUTPUT:
    // x is: 0
    // x is: 1
    // x is: 2
    // x is: 3
    // x is: 4
    // x is: 5
    // x is: 6
    // x is: 7
    // x is: 8
    // x is: 9
    //
    // KEY POINT: break only killed the INNER loop
    //            outer loop ran all 10 iterations normally

    // ============================================================
    // SECTION 3: IF continue WAS USED INSTEAD OF break
    // ============================================================

    // for (int x = 0; x < 10; x++)
    // {
    //     for (int i = 0; i < 10; i++)
    //     {
    //         continue;                                    // skips rest of THIS iteration
    //         std::cout << "start of loop" << std::endl;  // NEVER runs
    //         std::cout << "i is:" << i << std::endl;     // NEVER runs
    //     }
    //     std::cout << "x is:" << x << std::endl;         // runs 10 times
    // }
    //
    // EXECUTION TRACE with continue:
    //
    // x=0: i=0 -> continue -> skip to i=1
    //      i=1 -> continue -> skip to i=2
    //      ... all 10 iterations of inner loop run but skip their body
    //      prints "x is: 0"
    // x=1: same thing
    // ...
    //
    // OUTPUT: same as break in this case!
    //   x is: 0
    //   x is: 1
    //   ... to x is: 9
    //
    // BUT the difference is:
    // break    -> inner loop runs 1 time  (exits after i=0)
    // continue -> inner loop runs 10 times (skips body each time)
    // Same output here because the skipped code was after break/continue
    // Performance is different though!

    // ============================================================
    // SECTION 4: BREAK vs CONTINUE - CLEAR EXAMPLE
    // ============================================================

    // BREAK example - find first even number and stop:
    // for (int i = 0; i < 10; i++) {
    //     if (i % 2 == 0) {
    //         std::cout << "found: " << i << std::endl;
    //         break;        // stop searching, we found it
    //     }
    // }
    // output: found: 0
    // loop ran only 1 time

    // CONTINUE example - skip even numbers, print odds only:
    // for (int i = 0; i < 10; i++) {
    //     if (i % 2 == 0) {
    //         continue;     // skip even, go to next iteration
    //     }
    //     std::cout << i << std::endl;
    // }
    // output: 1, 3, 5, 7, 9
    // loop ran 10 times, but only printed 5 times

    // ============================================================
    // SECTION 5: NESTED LOOPS - break ONLY EXITS INNERMOST
    // ============================================================

    // for (int x = 0; x < 3; x++) {
    //     for (int i = 0; i < 3; i++) {
    //         if (i == 1) break;          // exits inner loop only
    //         std::cout << x << "," << i << std::endl;
    //     }
    // }
    // output:
    // 0,0    <- i=1 hit break, inner loop exits
    // 1,0    <- outer loop continues, x=1
    // 2,0    <- outer loop continues, x=2
    //
    // INTERVIEW: How do you break out of ALL nested loops?
    // Option 1: use a flag variable
    //   bool done = false;
    //   for (int x = 0; x < 10 && !done; x++) {
    //       for (int i = 0; i < 10; i++) {
    //           if (someCondition) { done = true; break; }
    //       }
    //   }
    //
    // Option 2: goto (valid but discouraged in modern C++)
    //   for (int x = 0; x < 10; x++) {
    //       for (int i = 0; i < 10; i++) {
    //           if (someCondition) goto done;
    //       }
    //   }
    //   done:
    //
    // Option 3: put loops inside a function, use return
    //   void search() {
    //       for (int x = 0; x < 10; x++) {
    //           for (int i = 0; i < 10; i++) {
    //               if (someCondition) return;  // exits function = exits all loops
    //           }
    //       }
    //   }

    // ============================================================
    // SECTION 6: DEAD CODE - important concept
    // ============================================================

    // Any code AFTER break or continue in same block = DEAD CODE
    // It will NEVER execute
    // Compiler may warn: "unreachable code"
    //
    // for (int i = 0; i < 10; i++) {
    //     break;
    //     std::cout << i;    // DEAD CODE - never reached
    // }
    //
    // Same as last session where return 0; was before cout
    // Lesson: break/continue/return all create dead code below them

    // ============================================================
    // SECTION 7: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: What is the difference between break and continue?
    //     -> break:    exits the loop entirely, no more iterations
    //        continue: skips rest of current iteration, loop continues

    // Q2: In nested loops, which loop does break exit?
    //     -> Only the INNERMOST loop where break is placed
    //        Outer loops are unaffected

    // Q3: How do you exit all nested loops at once?
    //     -> 3 ways: flag variable, goto label, or wrap in function + return
    //        Most preferred in modern C++: flag variable or function + return

    // Q4: What is dead code? Give an example.
    //     -> Code that can never be executed.
    //        break; followed by any statement in same block.
    //        return 0; before cout (from last session!)
    //        Compiler usually warns about unreachable code.

    // Q5: What does continue do in a while loop?
    //     -> Skips to the CONDITION CHECK (not the top of body)
    //        int i = 0;
    //        while (i < 5) {
    //            i++;
    //            if (i == 3) continue;  // skips cout for i=3
    //            std::cout << i;
    //        }
    //        output: 1 2 4 5

    // Q6: What is an infinite loop and when is it used?
    //     -> for(;;) or while(true) - no exit condition
    //        Used in: game loops, server loops, embedded systems
    //        Always needs break or return inside to exit

    // Q7: What happens if break is used outside a loop?
    //     -> COMPILER ERROR - break is only valid inside
    //        for, while, do-while, and switch statements

    return 0;
}
// ```

// **The one visual that makes break vs continue stick forever:**

// ```
// BREAK:                          CONTINUE:
// for (i = 0; i < 5; i++) {      for (i = 0; i < 5; i++) {
//     if (i == 2) break;              if (i == 2) continue;
//     print(i);                       print(i);
// }                               }

// i=0 -> print 0                  i=0 -> print 0
// i=1 -> print 1                  i=1 -> print 1
// i=2 -> STOP, exit loop          i=2 -> SKIP, go to i=3
//                                  i=3 -> print 3
// output: 0 1                      i=4 -> print 4

//                                  output: 0 1 3 4
// ```

// > `break` = **walk out of the room**
// > `continue` = **skip your turn but stay in the game**