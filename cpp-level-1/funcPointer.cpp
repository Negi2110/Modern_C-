// ============================================================
// FUNCTION POINTERS, typedef, std::function - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <functional>  // for std::function

// ============================================================
// SECTION 1: WHAT IS A FUNCTION POINTER?
// ============================================================

// Every function lives in CODE SEGMENT of memory
// just like variables have addresses, FUNCTIONS have addresses too
// (you saw this: (void*)&foo in the address session)
//
// Function pointer = variable that stores address of a function
// allows you to:
// 1. store a function in a variable
// 2. pass a function to another function
// 3. decide WHICH function to call at RUNTIME (your code does this!)
// 4. store different functions in arrays/containers

// ============================================================
// SECTION 2: THREE WAYS TO DECLARE FUNCTION POINTER
// ============================================================

// WAY 1: RAW function pointer syntax (ugly but foundational)
// int (*op)(int, int);
//  ^    ^    ^    ^
//  |    |    |____|
//  |    |    parameter types
//  |    |
//  |    name of pointer variable
//  |
//  return type
//
// read it: "op is a pointer to a function that
//           takes (int,int) and returns int"
//
// The parentheses around *op are MANDATORY
// int *op(int,int) = completely different!
//                   (function that returns int*)

// WAY 2: typedef (your commented code)
// typedef int (*PFnIntegerOperations)(int, int);
// now PFnIntegerOperations is a TYPE ALIAS
// PFnIntegerOperations op;  <- cleaner, readable
//
// READ typedef right to left:
// typedef int (*PFnIntegerOperations)(int,int)
//         |         |
//         |         new type name
//         |
//         what the type IS (pointer to function int(int,int))
//
// Naming convention: PFn = Pointer to Function (common in C/Qt/WinAPI)

// WAY 3: std::function (modern C++11 - your actual code)
// std::function<int(int, int)> op;
//               |   |    |
//               |   |____|
//               |   parameter types
//               return type
//
// CLEANEST syntax, most flexible
// can store: regular functions, lambdas, member functions, functors
// slight overhead vs raw pointer (uses type erasure internally)

// ============================================================
// SECTION 3: THE ACTUAL FUNCTIONS
// ============================================================

int add(int x, int y)
{
    return x + y;
    // lives at some address in CODE SEGMENT
    // e.g. 0x401550
}

int multiply(int x, int y)
{
    return x * y;
    // lives at different address in CODE SEGMENT
    // e.g. 0x401570
}

// ============================================================
// SECTION 4: RUNTIME POLYMORPHISM WITH FUNCTION POINTERS
// ============================================================

int main()
{
    // std::function<int(int,int)> op;
    // op is currently EMPTY (uninitialized std::function)
    // calling op() now = throws std::bad_function_call exception

    std::function<int(int, int)> op;

    int n;
    std::cout << "1 for add or 2 for multiply" << std::endl;
    std::cin >> n;

    if (n == 1)
    {
        op = add;
        // op now stores ADDRESS of add function
        // op = 0x401550 (address of add in code segment)
    }
    else if (n == 2)
    {
        op = multiply;
        // op now stores ADDRESS of multiply function
        // op = 0x401570
    }
    // KEY POINT:
    // which function op calls is decided at RUNTIME
    // based on user input - compiler has no idea which one!
    // this is RUNTIME POLYMORPHISM via function pointer

    int x, y;
    std::cin >> x;
    std::cin >> y;

    std::cout << "Operations: " << op(x, y) << std::endl;
    // op(x, y) -> goes to address stored in op
    //          -> executes the function there
    //          -> returns result
    // if n==1: calls add(x,y)
    // if n==2: calls multiply(x,y)

    return 0;
}

// ============================================================
// SECTION 5: EQUIVALENT CODE - ALL THREE SYNTAXES SHOWN
// ============================================================

// --- RAW POINTER SYNTAX ---
// int (*op)(int, int);          // declare
// op = add;                     // assign (no & needed, function decays)
// op = &add;                    // also valid with &
// int result = op(3, 4);        // call -> 7
// int result = (*op)(3, 4);     // also valid (explicit dereference)

// --- TYPEDEF SYNTAX ---
// typedef int (*PFnOp)(int, int);
// PFnOp op;                     // declare (cleaner!)
// op = add;
// int result = op(3, 4);        // call -> 7

// --- std::function SYNTAX (modern, recommended) ---
// std::function<int(int,int)> op;
// op = add;                     // assign
// op = multiply;                // reassign (flexible!)
// int result = op(3, 4);        // call -> 7

// ============================================================
// SECTION 6: FUNCTION POINTER ARRAY - POWERFUL PATTERN
// ============================================================

// Store multiple functions in an array:
// int (*operations[2])(int,int) = {add, multiply};
// operations[0](3,4) -> add(3,4)      = 7
// operations[1](3,4) -> multiply(3,4) = 12
//
// with typedef (cleaner):
// typedef int (*PFnOp)(int,int);
// PFnOp operations[] = {add, multiply};
//
// REAL USE CASE: jump tables, command patterns, dispatch tables
// instead of long if-else chains:
//
// if (n==1) add(x,y);
// else if (n==2) multiply(x,y);
// else if (n==3) subtract(x,y);
// ...
//
// becomes:
// PFnOp ops[] = {add, multiply, subtract};
// ops[n-1](x, y);   // one line, fast, extensible

// ============================================================
// SECTION 7: std::function vs RAW POINTER
// ============================================================

// | Feature              | int(*op)(int,int) | std::function      |
// |----------------------|-------------------|--------------------|
// | Syntax               | ugly              | clean              |
// | Stores lambdas       | NO                | YES                |
// | Stores member func   | NO                | YES (with bind)    |
// | Stores functors      | NO                | YES                |
// | Overhead             | zero              | small (type erase) |
// | Can be null          | YES (nullptr)     | YES (empty)        |
// | Null check           | if(op)            | if(op)             |
// | C compatibility      | YES               | NO                 |
//
// WHEN to use which:
// raw pointer: C interop, performance-critical, simple callbacks
// std::function: modern C++, lambdas, flexible callbacks, Qt-like signals

// ============================================================
// SECTION 8: LAMBDA WITH std::function (preview)
// ============================================================

// std::function can store LAMBDAS - raw pointer CANNOT:
//
// std::function<int(int,int)> op;
// op = [](int x, int y) { return x + y; };  // lambda!
// op(3, 4);  // 7
//
// lambda = anonymous function defined inline
// HUGELY used in modern C++ and Qt (connect, algorithms)
// raw function pointer CANNOT store lambdas with captures:
//
// int offset = 10;
// op = [offset](int x, int y) { return x + y + offset; }; // capture!
// int (*rawOp)(int,int) = [offset](...){...}; // ERROR - has capture!
// std::function handles this, raw pointer cannot

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a function pointer?
//     -> A variable that stores the memory address of a function.
//        Allows selecting and calling functions at runtime.

// Q2: What is the syntax for raw function pointer to int(int,int)?
//     -> int (*op)(int, int);
//        Parentheses around *op are mandatory.
//        Without them: int *op(int,int) = function returning int*

// Q3: What is typedef and why use it with function pointers?
//     -> typedef creates a type alias.
//        typedef int (*PFnOp)(int,int) makes PFnOp a readable type.
//        Hides the ugly pointer syntax behind a clean name.

// Q4: What is std::function and how is it different from raw pointer?
//     -> std::function is a wrapper from <functional>.
//        Can store functions, lambdas, member functions, functors.
//        Raw pointer can only store plain functions.
//        std::function has small overhead (type erasure).

// Q5: What is runtime polymorphism via function pointers?
//     -> Which function is called is decided at RUNTIME, not compile time.
//        Your code: user picks add or multiply at runtime via n.
//        Compiler cannot know which function op will call.

// Q6: Can you store a lambda in a raw function pointer?
//     -> Only if lambda has NO captures.
//        [](){} -> can convert to raw pointer
//        [x](){} -> has capture, CANNOT convert to raw pointer
//        Use std::function for lambdas with captures.

// Q7: What is a jump table / dispatch table?
//     -> Array of function pointers.
//        Replaces long if-else chains with array index lookup.
//        O(1) dispatch vs O(n) if-else chain.
//        Used in virtual function tables (vtable) internally.

// Q8: What happens if you call an empty std::function?
//     -> Throws std::bad_function_call exception.
//        Always check: if(op) { op(x,y); } before calling.
//        Raw null pointer call = undefined behavior (crash).
//```

// **The 3 syntaxes side by side - most asked in interviews:**

// ```cpp
// // 1. RAW (C style)
// int (*op)(int, int) = add;       // ugly but zero overhead

// // 2. typedef (C style cleaner)
// typedef int (*PFnOp)(int, int);
// PFnOp op = add;                  // readable, same as raw

// // 3. std::function (modern C++)
// std::function<int(int,int)> op = add;  // cleanest, stores lambdas too
// ```

// > The **key insight** interviewers want: function pointers enable **runtime polymorphism without inheritance** — you decide which function to call based on runtime data, not compile-time type. This is the foundation of **callbacks, signals/slots in Qt, and strategy pattern**.





Great question. Honest answer:

```cpp
// ============================================================
// FUNCTION POINTER OVERHEAD - RAW CALL vs POINTER CALL
// ============================================================

// ============================================================
// SECTION 1: DIRECT CALL vs POINTER CALL - PERFORMANCE
// ============================================================

// DIRECT FUNCTION CALL:
// add(3, 4);
// -> compiler KNOWS at compile time which function to call
// -> generates: CALL 0x401550  (hardcoded address in assembly)
// -> can be INLINED by compiler (function body copy-pasted at call site)
// -> zero overhead, fastest possible

// RAW FUNCTION POINTER CALL:
// int (*op)(int,int) = add;
// op(3, 4);
// -> compiler does NOT know at compile time which function op holds
// -> generates: CALL [op]  (go to address stored in op = indirect call)
// -> CANNOT be inlined (compiler doesn't know what's there)
// -> one extra memory read to get address from op
// -> tiny overhead vs direct call

// std::function CALL:
// std::function<int(int,int)> op = add;
// op(3,4);
// -> uses TYPE ERASURE internally (virtual-call-like mechanism)
// -> heap allocation possible for large callables
// -> cannot be inlined
// -> MOST overhead of the three

// ============================================================
// SECTION 2: ACTUAL NUMBERS - IS IT WORTH WORRYING?
// ============================================================

// Direct call:          ~1 CPU cycle
// Raw pointer call:     ~1-3 CPU cycles  (indirect jump)
// std::function call:   ~5-20 CPU cycles (type erasure overhead)
//
// For context:
// CPU runs at 3-4 GHz = 3-4 BILLION cycles per second
// difference between direct and pointer call = NANOSECONDS
//
// SO:
// calling op(x,y) vs add(x,y) 1000 times -> difference = MICROSECONDS
// calling op(x,y) vs add(x,y) 1 time      -> difference = NANOSECONDS
//
// In 99% of real code -> YOU WILL NEVER NOTICE THE DIFFERENCE
// Only matters in:
// -> tight inner loops running MILLIONS of times per second
// -> real-time systems (audio processing, game physics, embedded)
// -> high frequency trading (nanoseconds matter)

// ============================================================
// SECTION 3: MEMORY USAGE
// ============================================================

// DIRECT CALL:
// no extra memory, address baked into instruction

// RAW POINTER:
// int (*op)(int,int);
// sizeof(op) = 8 bytes  (just stores one address)
// no heap allocation
// lives on stack

// std::function:
// sizeof(std::function<int(int,int)>) = 32 bytes (on most systems)
// for SMALL callables (plain functions, small lambdas):
//   -> stores inline, no heap allocation (small buffer optimization)
// for LARGE callables (lambdas capturing many variables):
//   -> heap allocation! (similar to std::string SSO)
//
// MEMORY:
// direct call   = 0 extra bytes
// raw pointer   = 8 bytes (stack)
// std::function = 32 bytes (stack) + possible heap

// ============================================================
// SECTION 4: SO WHY USE FUNCTION POINTERS AT ALL?
// ============================================================

// BENEFIT IS NOT PERFORMANCE
// BENEFIT IS FLEXIBILITY AND DESIGN

// 1. RUNTIME DECISION (your code):
//    you CANNOT do this with direct calls:
//    add(x,y);       // hardcoded at compile time
//    multiply(x,y);  // hardcoded at compile time
//
//    with pointer: op = add OR multiply based on user input
//    decision made at RUNTIME - impossible without pointer

// 2. CALLBACKS:
//    pass behavior INTO a function
//    void sort(int* arr, int n, bool(*compare)(int,int))
//    caller decides HOW to sort by passing different compare functions
//    library doesn't need to know compare logic in advance

// 3. PLUGIN SYSTEMS:
//    load functions from DLL/SO at runtime
//    void* fn = dlsym(handle, "processData");
//    raw pointer = only way to call dynamically loaded functions

// 4. REPLACING LONG if-else:
//    // instead of:
//    if(op==1) add(x,y);
//    else if(op==2) multiply(x,y);
//    else if(op==3) subtract(x,y);
//    // ... 20 more cases
//
//    // use jump table:
//    PFnOp ops[] = {add, multiply, subtract};
//    ops[op-1](x,y);  // O(1), cleaner, extensible

// ============================================================
// SECTION 5: SIMPLE RULE TO REMEMBER
// ============================================================

// USE DIRECT CALL when:
// -> you always call the same function
// -> maximum performance needed
// -> compiler can optimize/inline

// USE RAW POINTER when:
// -> need to decide function at runtime
// -> C interop / callbacks to C APIs
// -> performance-critical inner loops needing dynamic dispatch
// -> plugin/DLL systems

// USE std::function when:
// -> need to store lambdas (especially with captures)
// -> flexibility > raw performance
// -> storing in containers (vector of callbacks)
// -> Qt-like signal/slot systems, event handlers
// ```

// **Bottom line in 3 lines:**

// > Direct call = fastest, but **hardcoded** at compile time — no flexibility
// > Raw pointer = tiny overhead (~1-3 cycles), but **runtime flexibility**
// > std::function = most overhead (5-20 cycles + possible heap), but **stores anything** (lambdas, captures, functors)

// The benefit of function pointers is **never performance** — it's always **flexibility**. You pay a tiny cost to gain the ability to decide **which function to call at runtime**. That tradeoff is almost always worth it.