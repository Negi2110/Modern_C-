// ============================================================
// std::move, MOVE SEMANTICS, static_cast<&&> - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <string>
#include <utility>  // for std::move

// ============================================================
// SECTION 1: THE PROBLEM - WHY COPIES ARE EXPENSIVE
// ============================================================

// std::string myString = "long long long string...";
// std::string s2 = myString;
//
// WHAT HAPPENS INTERNALLY (copy):
//
// myString on heap:  [ptr] -> ["long long long string..."]
//                                        |
//                              char by char COPY
//                                        |
// s2 on heap:        [ptr] -> ["long long long string..."]
//
// TWO separate heap allocations
// every character copied one by one
// for a 1MB string -> 1MB of work just to duplicate!
//
// SAME PROBLEM with function calls:
// void process(std::string s) { }  // copy on every call!
// process(myString);               // entire string copied into s
//
// YOUR COMMENT IS SPOT ON:
// "I want it like the resource I'm sending just ASSIGN THAT to memory"
// = move semantics! steal the pointer, don't copy the data

// ============================================================
// SECTION 2: YOUR MAIN QUESTION - WHY static_cast<std::string&&> WORKS SAME AS std::move
// ============================================================

// THIS IS THE KEY INSIGHT:
// std::move does NOT actually move anything!
// std::move is literally just a CAST
//
// std::move implementation in the standard library:
// template<typename T>
// std::remove_reference_t<T>&& move(T&& t) noexcept
// {
//     return static_cast<std::remove_reference_t<T>&&>(t);
// }
//
// IT IS LITERALLY static_cast<T&&> UNDER THE HOOD
// nothing more, nothing less
//
// SO:
// newValue = std::move(myString);
// newValue = static_cast<std::string&&>(myString);
// IDENTICAL - same assembly generated, same behavior
//
// std::move just says to the compiler:
// "treat myString as an RVALUE (temporary)"
// "it is safe to STEAL its resources"
// the actual stealing happens in std::string's MOVE ASSIGNMENT OPERATOR
// not in std::move itself!

// ============================================================
// SECTION 3: WHAT ACTUALLY HAPPENS DURING MOVE
// ============================================================

int main()
{
    std::string myString = "copy construct me";
    std::string newValue;

    std::cout << "myString:  " << myString << std::endl;
    // prints: copy construct me

    std::cout << "newValue:  " << newValue << std::endl;
    // prints: (empty string)

    // BEFORE std::move:
    // STACK:           HEAP:
    // myString:
    //  [ptr]     -->   ["copy construct me"]
    //  [size=17]
    //  [cap=17]
    //
    // newValue:
    //  [ptr]     -->   nullptr (empty)
    //  [size=0]
    //  [cap=0]

    newValue = std::move(myString);
    // std::move(myString) = static_cast<std::string&&>(myString)
    // = "treat myString as rvalue, safe to steal from"
    //
    // std::string's MOVE ASSIGNMENT OPERATOR then runs:
    // STEP 1: newValue steals myString's internal pointer
    //         newValue.ptr = myString.ptr  (pointer copy = 8 bytes only!)
    // STEP 2: newValue takes myString's size and capacity
    // STEP 3: myString's pointer set to nullptr (or empty state)
    //         myString.ptr = nullptr
    //         myString.size = 0
    //
    // AFTER std::move:
    // STACK:           HEAP:
    // myString:
    //  [ptr]     -->   nullptr (GUTTED - pointer stolen!)
    //  [size=0]
    //  [cap=0]
    //
    // newValue:
    //  [ptr]     -->   ["copy construct me"]  (SAME heap memory!)
    //  [size=17]
    //  [cap=17]
    //
    // NO new heap allocation!
    // NO character by character copy!
    // Just pointer reassignment = O(1) regardless of string size!

    // COPY vs MOVE comparison:
    // newValue = myString;             // COPY
    //   -> new heap allocation (17 bytes)
    //   -> copy 17 chars one by one
    //   -> myString untouched, still valid with its own data
    //   -> O(n) where n = string length
    //
    // newValue = std::move(myString);  // MOVE
    //   -> NO new heap allocation
    //   -> just pointer swap (8 bytes)
    //   -> myString gutted (valid but empty)
    //   -> O(1) regardless of string size!

    std::cout << "myString: " << myString << std::endl;
    // prints: (empty or unspecified but VALID state)
    // myString still EXISTS as a variable on stack
    // but its HEAP DATA was stolen by newValue
    // standard guarantees: valid but unspecified state
    // safe to: reassign, destroy, check .empty()
    // NOT safe to: assume it has any specific value

    std::cout << "newValue: " << newValue << std::endl;
    // prints: copy construct me
    // newValue now owns the heap data

    return 0;
}

// ============================================================
// SECTION 4: COPY vs MOVE - CONSTRUCTOR AND ASSIGNMENT
// ============================================================

// Two operations, two versions each:
//
// COPY CONSTRUCTOR:
// std::string s2 = s1;          // copy construct
// std::string s2(s1);           // same thing
// -> new heap allocation + data copied
// -> s1 unchanged
//
// MOVE CONSTRUCTOR:
// std::string s2 = std::move(s1); // move construct
// std::string s2(std::move(s1));  // same thing
// -> NO new heap allocation
// -> s1 gutted (empty/valid)
//
// COPY ASSIGNMENT:
// s2 = s1;               // copy assign
// -> s2's old data freed
// -> new allocation + copy from s1
//
// MOVE ASSIGNMENT:
// s2 = std::move(s1);    // move assign
// -> s2's old data freed
// -> s2 steals s1's pointer
// -> s1 gutted

// ============================================================
// SECTION 5: WHEN MOVE HAPPENS AUTOMATICALLY (no std::move needed)
// ============================================================

// C++ automatically moves (no std::move needed) when:
//
// 1. Returning local variable from function:
//    std::string createString() {
//        std::string result = "hello";
//        return result;   // automatically MOVED (RVO/NRVO)
//    }
//    std::string s = createString();  // no copy!
//
// 2. Passing a temporary directly:
//    process(std::string("hello"));  // temporary -> auto moved into param
//
// 3. Inserting temporary into container:
//    vec.push_back(std::string("hello"));  // auto moved

// std::move needed when:
// -> named lvalue you want to transfer (your case)
// -> you are DONE with myString and want newValue to own it

// ============================================================
// SECTION 6: THE RULE OF FIVE - INTERVIEW ESSENTIAL
// ============================================================

// If your class manages heap memory, you need ALL FIVE:
// 1. Destructor              -> free heap on destroy
// 2. Copy constructor        -> deep copy heap data
// 3. Copy assignment         -> deep copy heap data
// 4. Move constructor        -> steal heap pointer
// 5. Move assignment         -> steal heap pointer
//
// std::string implements all 5
// YOUR custom classes managing heap must also implement all 5
//
// class MyString {
//     char* data;
//     size_t size;
// public:
//     ~MyString()                            { delete[] data; }
//     MyString(const MyString& other)        { /* deep copy */ }
//     MyString& operator=(const MyString& o) { /* deep copy */ }
//     MyString(MyString&& other) noexcept    { /* steal ptr */ }
//     MyString& operator=(MyString&& other)  { /* steal ptr */ }
// };

// ============================================================
// SECTION 7: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does std::move actually do?
//     -> NOTHING at runtime. It's just a cast to rvalue reference.
//        static_cast<T&&>(x) under the hood.
//        Signals compiler "treat this as temporary, steal from it".
//        Actual stealing done by move constructor/assignment operator.

// Q2: Why is std::move same as static_cast<std::string&&>?
//     -> Because std::move IS static_cast<T&&> internally.
//        It just has a cleaner name and works generically with
//        any type via template. Both produce identical assembly.

// Q3: What state is myString in after std::move(myString)?
//     -> Valid but unspecified state (C++ standard guarantee).
//        Typically empty string (ptr=nullptr, size=0).
//        Safe to reassign or destroy, NOT safe to read value.

// Q4: What is the performance difference between copy and move?
//     -> Copy: O(n) - allocates new heap memory, copies n chars.
//        Move: O(1) - just swaps 3 pointers (ptr, size, capacity).
//        For 1MB string: copy = 1MB work, move = 24 bytes work.

// Q5: When does C++ automatically use move instead of copy?
//     -> When source is a temporary (rvalue):
//        return local_variable; (RVO/NRVO)
//        vec.push_back(std::string("hi")); (temporary)
//        Use std::move explicitly for named lvalues you're done with.

// Q6: What is the Rule of Five?
//     -> If class manages heap memory, implement all 5:
//        destructor, copy ctor, copy assign, move ctor, move assign.
//        Missing move operations = copies used instead (inefficient).

// Q7: Can you use myString after std::move(myString)?
//     -> Technically yes (valid state), but value is unspecified.
//        Best practice: don't use it. Reassign first if needed.
//        myString = "new value";  // safe to use again after reassign

// Q8: What is move semantics used for in Qt?
//     -> QString, QByteArray, QVector all support move semantics.
//        Returning large Qt containers from functions is efficient.
//        Qt 5.2+ added move constructors to most container classes.
// ```

// **The single most important insight:**

// ```cpp
// // std::move SOURCE CODE (simplified):
// template<typename T>
// T&& move(T& x) {
//     return static_cast<T&&>(x);  // literally just a cast!
// }

// // So these are IDENTICAL:
// newValue = std::move(myString);
// newValue = static_cast<std::string&&>(myString);
// ```

// > `std::move` does **zero work** at runtime. It's a compile-time cast that says *"this is a temporary, steal from it"*. The **actual stealing** (pointer swap) happens inside `std::string`'s move assignment operator. The name `std::move` is misleading — `std::steal` or `std::enable_move` would be more accurate.