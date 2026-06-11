// ============================================================
// MEMORY ADDRESSES, STACK, void* CASTING - INTERVIEW NOTES
// ============================================================

#include <iostream>

void foo()
{
    // x = 72;  -> ERROR: x is in main()'s stack frame
                //  foo() has its own separate stack frame
                //  cannot access another function's local variables
}

// ============================================================
// SECTION 1: YOUR MAIN QUESTION - WHY void* FOR char AND FUNCTION?
// ============================================================

// For int and float:
// std::cout << &x  -> works fine, prints address like 0x61ff08
// std::cout << &y  -> works fine, prints address like 0x61ff04
//
// For char:
// std::cout << &a  -> WRONG OUTPUT - prints "abc..." (string!)
//                     NOT the address you want
//
// WHY? Because cout has a SPECIAL RULE:
// cout << char*  -> treats it as a C-STRING and prints characters
//                   until it hits '\0' (null terminator)
// This is by design - so you can do: cout << "hello" (char array)
//
// std::cout << &a  // &a is char* -> cout thinks it's a string!
//                  // prints garbage characters until random '\0' found
//                  // UNDEFINED BEHAVIOR - not what you want
//
// FIX: cast to void*
// std::cout << (void*)&a  // now cout sees void* -> prints address
//                         // void* has no special treatment in cout
//
// SAME for function pointer:
// std::cout << &foo   // function pointer -> cout doesn't know how to print it
//                     // COMPILER ERROR or garbage
// std::cout << (void*)&foo  // cast to void* -> prints address cleanly

int main()
{
    // ============================================================
    // SECTION 2: STACK LAYOUT - WHERE VARIABLES LIVE
    // ============================================================

    int x = 42;
    float y = 72;
    char a = 'a';
    signed char b = 'b';
    unsigned char c = 'c';

    // All these variables live on main()'s STACK FRAME
    // Stack grows DOWNWARD in memory (high address -> low address)
    //
    // MEMORY LAYOUT (typical, addresses decrease downward):
    //
    // High address
    // |  main() return address  |
    // |-------------------------|
    // |  x = 42   (4 bytes)     |  &x = 0x61ff14  (example)
    // |-------------------------|
    // |  y = 72.0 (4 bytes)     |  &y = 0x61ff10
    // |-------------------------|
    // |  a = 'a'  (1 byte)      |  &a = 0x61ff0f
    // |-------------------------|
    // |  b = 'b'  (1 byte)      |  &b = 0x61ff0e
    // |-------------------------|
    // |  c = 'c'  (1 byte)      |  &c = 0x61ff0d
    // |-------------------------|
    // Low address
    //
    // NOTICE: each variable is at a DIFFERENT address
    // sizeof(x) = 4, so x takes 4 bytes of stack space

    // ============================================================
    // SECTION 3: PRINTING ADDRESSES
    // ============================================================

    std::cout << "x:" << &x << std::endl;
    // &x = address of x = pointer to int (int*)
    // cout handles int* fine -> prints hex address
    // example output: x:0x61ff14

    std::cout << "size of x:" << sizeof(x) << std::endl;
    // sizeof(x) = 4 bytes (int on 64-bit system)
    // sizeof is compile time operator, not a function

    std::cout << "y:" << &y << std::endl;
    // &y = pointer to float (float*)
    // cout handles float* fine -> prints hex address

    std::cout << "a:" << (void*)&(a) << std::endl;
    // &a = pointer to char (char*)
    // WITHOUT cast: cout treats char* as C-string -> prints 'a' + garbage
    // WITH (void*) cast: cout treats as raw address -> prints hex address
    // CORRECT output: a:0x61ff0f

    std::cout << "b:" << (void*)&(b) << std::endl;
    // &b = pointer to signed char (signed char*)
    // same issue as char -> need void* cast

    std::cout << "c:" << (void*)&(c) << std::endl;
    // &c = pointer to unsigned char
    // same issue -> need void* cast

    std::cout << "foo:" << (void*)&foo << std::endl;
    // &foo = function pointer
    // function pointers are DIFFERENT from data pointers
    // they point to CODE SEGMENT, not stack/heap
    // cout cannot print function pointer directly -> need void* cast
    // output: foo:0x401550  (some address in code segment)

    std::cout << "main:" << (void*)&main << std::endl;
    // &main = address of main function in CODE SEGMENT
    // also needs void* cast
    // output: main:0x401560 (different from foo, both in code segment)

    // ============================================================
    // SECTION 4: THREE MEMORY SEGMENTS
    // ============================================================

    // Your program's memory is divided into segments:
    //
    // HIGH ADDRESS
    // |--------------------------|
    // |    STACK                 | <- local variables (x, y, a, b, c)
    // |    grows DOWNWARD        |    function call frames
    // |          |               |    fast, automatic, limited size
    // |          v               |
    // |--------------------------|
    // |    (gap)                 |
    // |          ^               |
    // |          |               |
    // |    HEAP                  | <- dynamic memory (new, malloc)
    // |    grows UPWARD          |    manual management, large, slower
    // |--------------------------|
    // |    DATA SEGMENT          | <- global variables, static variables
    // |--------------------------|
    // |    CODE SEGMENT (TEXT)   | <- your compiled functions live here
    // |    foo(), main()         |    read-only, loaded at program start
    // |--------------------------|
    // LOW ADDRESS
    //
    // THAT IS WHY:
    // &x, &y, &a   -> stack addresses   (high, similar to each other)
    // &foo, &main  -> code addresses    (low,  far from stack addresses)

    // ============================================================
    // SECTION 5: WHY foo() CANNOT ACCESS x
    // ============================================================

    // Each function has its OWN stack frame
    // foo()'s stack frame and main()'s stack frame are SEPARATE
    //
    // main() frame:  [x=42][y=72][a='a'][b='b'][c='c']
    // foo()  frame:  []  <- empty, no local variables
    //
    // foo() has NO WAY to reach main()'s x
    // unless you PASS it as a parameter or use a pointer
    //
    // void foo(int* ptr) { *ptr = 72; }  // now foo CAN modify x
    // foo(&x);                            // pass address of x to foo

    // ============================================================
    // SECTION 6: void* - WHAT IS IT?
    // ============================================================

    // void* = "pointer to anything"
    // It is a RAW ADDRESS with no type information
    //
    // int*   -> pointer to int    (knows data is 4 bytes)
    // float* -> pointer to float  (knows data is 4 bytes, IEEE 754)
    // char*  -> pointer to char   (cout treats as C-string!)
    // void*  -> raw address only  (no type, no special treatment)
    //
    // void* ptr = &x;   // valid - any pointer can become void*
    // *ptr = 10;        // ERROR - cannot dereference void*
    //                   // compiler doesn't know the type/size
    // *(int*)ptr = 10;  // VALID - cast back to int* first
    //
    // INTERVIEW: What is void*?
    //   -> A generic pointer that holds any address but carries
    //      no type information. Cannot be dereferenced directly.
    //      Must be cast to a typed pointer before use.
    //      Used in C APIs (malloc returns void*)

    // ============================================================
    // SECTION 7: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: Why do we cast char* to void* when printing with cout?
    //     -> cout has special overload for char* that treats it
    //        as a C-string and prints characters, not the address.
    //        void* cast bypasses this, printing the raw address.

    // Q2: What is the difference between stack and heap?
    //     -> Stack: automatic, fast, limited size, local variables,
    //               managed by compiler (push/pop on function calls)
    //        Heap:  manual, slower, large, dynamic allocation (new/delete)
    //               programmer manages lifetime

    // Q3: Where do function definitions (code) live in memory?
    //     -> Code/Text segment. Read-only, loaded at program start.
    //        &foo gives you the address in code segment.

    // Q4: Why does stack grow downward?
    //     -> Historical/architectural convention on x86.
    //        Stack pointer starts high, decrements as frames are pushed.
    //        Heap grows upward from low addresses toward stack.

    // Q5: Can foo() access main()'s local variable x?
    //     -> NOT directly. Each function has its own stack frame.
    //        Only way: pass x by pointer/reference to foo().

    // Q6: What is a function pointer?
    //     -> A pointer that holds the address of a function
    //        in the code segment.
    //        void (*fptr)() = &foo;  // pointer to foo
    //        fptr();                 // calls foo through pointer
    //        Used in callbacks, Qt signals/slots internally.

    // Q7: What does & operator do?
    //     -> Address-of operator. Returns the memory address
    //        of a variable or function.
    //        &x   -> address of variable x (stack)
    //        &foo -> address of function foo (code segment)

    // Q8: What is sizeof() and when is it evaluated?
    //     -> Returns size in bytes of a type or variable.
    //        Evaluated at COMPILE TIME (not runtime).
    //        sizeof(int) = 4, sizeof(char) = 1, sizeof(double) = 8

    return 0;
}
// ```

// **The one thing that surprises everyone:**

// ```cpp
// char a = 'a';
// cout << &a;          // prints "abc..." garbage  <- NOT the address!
// cout << (void*)&a;   // prints 0x61ff0f          <- actual address
// ```

// > `cout` was designed so `cout << "hello"` works (char* = string).
// > That same "feature" **breaks** address printing for `char*`.
// > `void*` cast strips the type → cout has no choice but to print the raw address.