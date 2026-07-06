// ============================================================
// static KEYWORD - ALL USES - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS static LOCAL VARIABLE?
// ============================================================

// static local variable:
// -> allocated in the BINARY (data segment), NOT on the stack
// -> initialized ONLY ONCE (first time the function is called)
// -> PERSISTS between function calls (survives scope exit)
// -> destroyed only when the PROGRAM ends (not when function returns)
//
// YOUR COMMENT IS EXACT:
// "it doesn't live in the stack or in heap,
//  it is allocated in the binary"
//
// THREE MEMORY REGIONS (recap):
// STACK  -> local variables, function frames (auto cleanup)
// HEAP   -> new/delete (manual cleanup)
// DATA SEGMENT (binary) -> global variables, static variables
//                          (exist for entire program lifetime)

void foo()
{
    static int s_variable = 0;
    // FIRST CALL:  s_variable created in data segment, set to 0
    // SECOND CALL: this line SKIPPED (already initialized)
    //              s_variable still has value from last call!
    // -> initialization happens ONCE, storage persists FOREVER

    s_variable += 1;
    // increments the SAME variable every call
    // not a new variable each time (unlike local int x = 0)

    std::cout << s_variable << std::endl;
}

// ============================================================
// SECTION 2: CONTRAST - LOCAL vs STATIC LOCAL vs HEAP
// ============================================================

// void foo_local() {
//     int x = 0;      // STACK - born every call, dies every return
//     x += 1;
//     std::cout << x; // always prints 1 (new x each time)
// }
//
// void foo_static() {
//     static int x = 0; // DATA SEGMENT - born once, lives forever
//     x += 1;
//     std::cout << x; // prints 1, 2, 3, 4... (same x each time)
// }
//
// void foo_heap() {
//     int* p = new int(0); // HEAP - born every call
//     *p += 1;
//     std::cout << *p;     // always prints 1 (new allocation each time)
//     delete p;            // must manually free
// }

// ============================================================
// SECTION 3: MAIN() TRACE
// ============================================================

int main()
{
    for (int i = 0; i < 10; i++)
    {
        foo();
    }
    // OUTPUT:
    // 1   <- i=0: s_variable was 0, += 1 = 1
    // 2   <- i=1: s_variable was 1, += 1 = 2
    // 3   <- i=2: s_variable was 2, += 1 = 3
    // 4
    // 5
    // 6
    // 7
    // 8
    // 9
    // 10  <- i=9: s_variable was 9, += 1 = 10
    //
    // IF it were int (not static): would print 1 ten times
    // BECAUSE static: prints 1 through 10 (accumulating)

    return 0;
}

// ============================================================
// SECTION 4: ALL FOUR USES OF static IN C++
// ============================================================

// USE 1: STATIC LOCAL VARIABLE (your code above)
// -> persists between calls
// -> initialized once
// -> useful for: counters, caches, lazy initialization

// USE 2: STATIC GLOBAL VARIABLE (file scope)
// static int g_count = 0;  // at file level (outside any function)
// -> limits visibility to THIS FILE ONLY
// -> other .cpp files cannot see/access it (internal linkage)
// -> prevents name collisions across files
// -> without static: any .cpp file could access it (external linkage)

// USE 3: STATIC MEMBER VARIABLE (class level)
// class MyClass {
//     static int s_count;  // shared by ALL instances
// };
// int MyClass::s_count = 0;  // must define outside class
//
// -> ONE copy shared across ALL objects of the class
// -> not per-object (like regular members)
// -> exists even if NO objects created
// -> access: MyClass::s_count (no object needed)

// USE 4: STATIC MEMBER FUNCTION (class level)
// class MyClass {
//     static void printCount() {
//         std::cout << s_count;
//     }
// };
// MyClass::printCount();  // call without any object
//
// -> no 'this' pointer (not called on any specific object)
// -> can only access STATIC members (no instance members!)
// -> access: MyClass::printCount() (no object needed)

// ============================================================
// SECTION 5: STATIC MEMBER VARIABLE - IMPORTANT EXAMPLE
// ============================================================

// class Person {
// public:
//     Person() { s_count++; }
//     ~Person() { s_count--; }
//     static int getCount() { return s_count; }
// private:
//     static int s_count;  // HOW MANY Person objects exist right now?
// };
// int Person::s_count = 0;  // initialize outside class (mandatory!)
//
// int main() {
//     std::cout << Person::getCount();  // 0 (no objects yet)
//     Person p1;
//     Person p2;
//     std::cout << Person::getCount();  // 2
//     {
//         Person p3;
//         std::cout << Person::getCount();  // 3
//     }  // p3 destroyed
//     std::cout << Person::getCount();  // 2 again
// }
//
// CLASSIC use case: counting instances, shared state, singleton

// ============================================================
// SECTION 6: STATIC LOCAL - THREAD SAFETY (important for interviews)
// ============================================================

// C++11 GUARANTEES: static local initialization is THREAD SAFE
// (called "magic statics" or "Meyers singleton")
//
// void foo() {
//     static int x = expensive_computation();
//     // computed ONCE, even in multithreaded programs
//     // C++11: compiler inserts locking around initialization
// }
//
// SINGLETON PATTERN using static local:
// class Singleton {
// public:
//     static Singleton& getInstance() {
//         static Singleton instance;  // created once, thread-safe (C++11)
//         return instance;
//     }
// private:
//     Singleton() {}  // private constructor
// };
// Singleton& s = Singleton::getInstance();  // always same object

// ============================================================
// SECTION 7: MEMORY COMPARISON TABLE
// ============================================================

// | Variable Type      | Where stored   | Lifetime          | Scope      |
// |--------------------|----------------|-------------------|------------|
// | local int x        | Stack          | function call     | local      |
// | static local       | Data segment   | entire program    | local      |
// | int* p = new int   | Heap (data)    | until delete      | pointer local|
// | global int g       | Data segment   | entire program    | file/global|
// | static global      | Data segment   | entire program    | THIS FILE  |
// | static member      | Data segment   | entire program    | class      |

// ============================================================
// SECTION 8: s_ PREFIX CONVENTION
// ============================================================

// static int s_variable;
//             ^
//             s_ = "static" prefix convention
//
// SAME idea as m_ prefix for member variables:
// m_ = member variable
// s_ = static variable
// g_ = global variable
//
// Makes it immediately clear when reading code:
// "this variable persists, it's not a fresh local each call"

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does static mean for a local variable?
//     -> Variable is stored in the data segment (not stack).
//        Initialized ONCE (first call), persists between function calls,
//        destroyed when the program exits.

// Q2: Where is a static local variable stored?
//     -> Data segment of the binary (neither stack nor heap).
//        Same segment as global variables.
//        YOUR COMMENT was exactly right.

// Q3: What are the four uses of the static keyword in C++?
//     -> 1. Static local variable: persists between calls
//        2. Static global variable: file-scope only (internal linkage)
//        3. Static member variable: shared across all class instances
//        4. Static member function: no 'this', accessible without object

// Q4: What is the difference between static local and global variable?
//     -> Both live in data segment, both persist entire program.
//        Global: accessible from anywhere (any file if extern).
//        Static local: accessible ONLY inside its function (local scope).
//        Static local = global lifetime + local scope.

// Q5: Can a static member function access non-static members?
//     -> NO. Static functions have no 'this' pointer.
//        They don't operate on any specific object.
//        Can only access other static members.

// Q6: Is static local variable initialization thread-safe?
//     -> YES in C++11 and later (magic statics).
//        Compiler inserts thread-safe locking around initialization.
//        This is used for the Meyers singleton pattern.

// Q7: What happens if foo() with static int x=0 is called 10 times?
//     -> x is initialized to 0 ONLY on the first call.
//        Each subsequent call skips initialization, uses current value.
//        x accumulates: prints 1,2,3,...10 (as in your code).

// Q8: When would you use a static member variable?
//     -> When data should be SHARED across ALL instances of a class.
//        Examples: instance counter, shared configuration, singleton.
//        Must be defined OUTSIDE the class body in a .cpp file.
// ```

// **The four uses of `static` - memorize this:**

// ```cpp
// // USE 1: static LOCAL variable
// void foo() {
//     static int x = 0;  // persists between calls, lives in data segment
//     x++;               // accumulates: 1, 2, 3, 4...
// }

// // USE 2: static GLOBAL variable (file scope only)
// static int g_count = 0;  // only THIS .cpp file can see it

// // USE 3: static MEMBER variable (shared across all objects)
// class Foo {
//     static int s_count;  // ONE copy for ALL Foo objects
// };

// // USE 4: static MEMBER function (no 'this', no object needed)
// class Foo {
//     static void print() { }  // call as Foo::print()
// };
// ```

// **Memory location summary:**

// ```
// int x = 0;          → STACK    (dies when function returns)
// new int(0)          → HEAP     (dies when you delete it)
// static int x = 0;  → DATA SEGMENT (dies when program ends)
// ```

// > **Interview one-liner**: static local variable = **global lifetime + local scope**. It lives as long as the program but can only be *seen* inside its function. This combination makes it perfect for lazy initialization and the Singleton pattern.