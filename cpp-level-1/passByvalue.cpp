
// ============================================================
// PASS BY VALUE - COPIES, STACK FRAMES - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: PASS BY VALUE - WHAT HAPPENS
// ============================================================

// When you call setValue(x):
// -> C++ makes a COPY of x
// -> That copy is given to arg
// -> arg and x are TWO DIFFERENT variables
// -> TWO DIFFERENT memory addresses
// -> Modifying arg has ZERO effect on x

void setValue(int arg)
{
    std::cout << "Args address is :" << &arg << std::endl;
    // &arg will be DIFFERENT from &x in main()
    // proof that arg is a separate copy in a separate stack frame

    arg = 999;  // modifies the COPY only
                // x in main() is completely unaffected
}

int main()
{
    int x = 42;

    std::cout << "x's value is:" << x << std::endl;
    // prints: x's value is: 42

    std::cout << "x's addr is:" << &x << std::endl;
    // prints something like: x's addr is: 0x61ff14
    // this is x's address in main()'s stack frame

    setValue(x);
    // x is COPIED into arg
    // arg lives in setValue()'s stack frame
    // &arg != &x  (different addresses, different stack frames)
    // arg = 999 happens inside setValue()
    // setValue() stack frame is then DESTROYED after call ends

    std::cout << "x's value is:" << x << std::endl;
    // prints: x's value is: 42   <- UNCHANGED
    // proof that pass by value never touches original

    std::cout << "x's addr is:" << &x << std::endl;
    // prints SAME address as before: 0x61ff14
    // x never moved, never changed

    return 0;
}

// ============================================================
// SECTION 2: STACK FRAME VISUAL
// ============================================================

// STEP 1: main() starts
//
// STACK:
// |__________________________|
// | main() frame             |
// |   x = 42  (addr:0x14)   |
// |__________________________|
//
// STEP 2: setValue(x) is called - NEW frame pushed
//
// STACK:
// |__________________________|
// | setValue() frame         | <- new frame on top
// |   arg = 42 (addr:0x04)  |    copy of x, DIFFERENT address
// |__________________________|
// | main() frame             |
// |   x = 42  (addr:0x14)   |    original, untouched
// |__________________________|
//
// STEP 3: arg = 999 runs
//
// STACK:
// |__________________________|
// | setValue() frame         |
// |   arg = 999 (addr:0x04) | <- arg changed, x unaffected
// |__________________________|
// | main() frame             |
// |   x = 42  (addr:0x14)   | <- still 42!
// |__________________________|
//
// STEP 4: setValue() returns - its frame DESTROYED
//
// STACK:
// |__________________________|
// | main() frame             | <- only main remains
// |   x = 42  (addr:0x14)   |    x = 42, unchanged
// |__________________________|

// ============================================================
// SECTION 3: THREE WAYS TO PASS TO A FUNCTION
// ============================================================

// WAY 1: PASS BY VALUE (your current code)
// void setValue(int arg)
// -> copy made, original safe
// -> changes to arg do NOT affect x
// -> costs memory (copy of entire variable)
// -> good for small types (int, float, char)

// WAY 2: PASS BY POINTER
// void setValue(int* arg)
// {
//     *arg = 999;   // dereference pointer -> modifies ORIGINAL
// }
// setValue(&x);     // pass ADDRESS of x
//
// -> no copy of x made (only address copied = 8 bytes)
// -> can be null (must check before dereferencing)
// -> modifies original directly
// -> C style, still used in C++ for optional parameters

// WAY 3: PASS BY REFERENCE  (most common in modern C++)
// void setValue(int& arg)
// {
//     arg = 999;    // arg IS x, modifies original directly
// }
// setValue(x);      // no & needed at call site, looks like by value
//
// -> no copy made
// -> cannot be null (always valid)
// -> cleaner syntax than pointer
// -> modifies original directly

// ============================================================
// SECTION 4: PROOF WITH ADDRESSES
// ============================================================

// Pass by value output:
// x's addr is:    0x61ff14   <- main's stack
// Args address is: 0x61ff04  <- setValue's stack, DIFFERENT!
// x's value is:   42         <- unchanged

// Pass by reference output:
// x's addr is:    0x61ff14   <- main's stack
// Args address is: 0x61ff14  <- SAME address! arg IS x
// x's value is:   999        <- changed!

// ============================================================
// SECTION 5: WHEN PASS BY VALUE IS EXPENSIVE
// ============================================================

// For int (4 bytes) -> copy is cheap, fine to pass by value
//
// For large objects -> copy is EXPENSIVE:
// void print(std::string s)  // copies entire string -> bad!
// void print(std::string& s) // no copy, reference -> good
// void print(const std::string& s) // no copy + read only -> best

// Rule of thumb:
// primitive types (int, float, char, bool) -> pass by value  (cheap copy)
// large objects (string, vector, struct)   -> pass by const& (no copy)
// need to modify original                  -> pass by &       (reference)
// need optional/nullable                   -> pass by *       (pointer)

// ============================================================
// SECTION 6: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is pass by value?
//     -> A copy of the argument is made and given to the function.
//        Modifying the parameter does NOT affect the original variable.

// Q2: How do you prove pass by value makes a copy?
//     -> Print &x in main() and &arg in function.
//        They will be DIFFERENT addresses -> different variables.

// Q3: What is the difference between pass by value and pass by reference?
//     -> By value:     copy made, original safe, costs memory
//        By reference: no copy, arg IS original, changes affect caller

// Q4: When does the copy get destroyed in pass by value?
//     -> When the function returns, its stack frame is destroyed.
//        The copy (arg) is destroyed with it.

// Q5: What is the cost of pass by value for large objects?
//     -> Entire object is copied onto the stack.
//        For std::string, std::vector -> very expensive.
//        Use const& instead for read-only large objects.

// Q6: Can pass by value ever modify the original?
//     -> NO. By definition, it works on a copy.
//        Only pass by reference or pass by pointer can modify original.

// Q7: What happens to arg after setValue() returns?
//     -> setValue()'s stack frame is popped.
//        arg is destroyed. Memory is reclaimed by stack pointer moving up.

// Q8: What is the difference between pass by pointer and pass by reference?
//     -> Pointer: can be null, needs * to dereference, & at call site
//        Reference: cannot be null, cleaner syntax, no & at call site
//        Both allow modifying the original variable.


// **The address proof is the killer answer in interviews:**

// ```cpp
// // Run this and show the interviewer:
// std::cout << &x;    // 0x61ff14  <- main's stack
// // inside setValue:
// std::cout << &arg;  // 0x61ff04  <- different! proof it's a copy
// ```

// > Different address = different variable = different stack frame.
// > Same address = same variable = reference/pointer.

// **Quick rule for which to use:**

// ```
// small type, read only   ->  pass by value      (int, float)
// large type, read only   ->  const reference    (const string&)
// any type, need to modify ->  reference         (int&)
// nullable / optional      ->  pointer           (int*)
// ```