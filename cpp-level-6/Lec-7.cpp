// ============================================================
// auto KEYWORD - COMPLETE NOTES - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <vector>
#include <string>

// ============================================================
// SECTION 1: WHAT IS auto?
// ============================================================

// auto = "compiler, figure out the type for me from the initializer"
// TYPE DEDUCTION at compile time (NOT runtime!)
// auto is NOT dynamic typing (like Python)
// type is FIXED at compile time - just deduced, not written by you
//
// auto x = 5;       // x is int    (compiler deduces from literal 5)
// auto y = 5.0f;    // y is float  (compiler deduces from 5.0f)
// auto z = "hello"; // z is const char* (NOT std::string!)
// auto s = std::string("hello"); // s is std::string (explicit)
//
// ONCE DEDUCED: type is FIXED, cannot change
// auto x = 5;
// x = "hello"; // ERROR: cannot assign const char* to int
//              // auto made x an int, cannot reassign to different type

// ============================================================
// SECTION 2: YOUR MAIN EXAMPLE - ITERATOR WITH auto
// ============================================================

int main()
{
    std::vector<int> ints;
    ints.push_back(1);
    ints.push_back(2);
    ints.push_back(3);

    // WITHOUT auto (verbose):
    // for (std::vector<int>::iterator it = ints.begin();
    //      it != ints.end(); it++)
    // -> std::vector<int>::iterator is LONG to write
    // -> if you change vector<int> to vector<double>:
    //    must update iterator type in EVERY loop!
    //
    // WITH auto (clean):
    for (auto it = ints.begin(); it != ints.end(); it++)
    //   ^^^^
    //   compiler sees ints.begin() returns std::vector<int>::iterator
    //   deduces: it is std::vector<int>::iterator
    //   YOU don't write the type, compiler figures it out
    {
        std::cout << *it << std::endl;
        // *it = dereference iterator = the actual int value
        // prints: 1, 2, 3
    }
    // if you change vector<int> to vector<double>:
    // auto STILL WORKS (deduces vector<double>::iterator automatically)
    // WITHOUT auto: must manually update the iterator type everywhere!

    // ============================================================
    // SECTION 3: YOUR QUESTION - WHY auto SHOULDN'T ALWAYS BE USED
    // ============================================================

    // auto value = "mike";
    //      ^^^^^   ^^^^^^
    //      deduced  string literal in C++
    //      as       = const char* (NOT std::string!)
    //      const char*
    //
    // YOUR COMMENT: "can't do value.size() if it points to string"
    // EXACTLY RIGHT!
    //
    // const char* has NO .size() method
    // const char* has NO .find() method
    // const char* has NO .substr() method
    // const char* is a C-style pointer, not a C++ string object!
    //
    // value.size();    // COMPILER ERROR: const char* has no member size()
    // value.length();  // COMPILER ERROR: same
    // value.find('k'); // COMPILER ERROR: same
    //
    // FIX if you want std::string:
    // auto value = std::string("mike");  // explicitly construct string
    // std::string value = "mike";        // explicit type (clearest!)
    // using namespace std::string_literals;
    // auto value = "mike"s;              // 's' suffix = std::string literal

    // auto value = "mike";
    // std::cout << value << std::endl;
    // -> this WORKS (const char* can be printed with cout)
    // -> but value.size() would FAIL
    // -> this is the DANGER of auto with string literals!

    return 0;
}

// ============================================================
// SECTION 4: WHEN auto IS GREAT
// ============================================================

// 1. LONG/COMPLEX TYPES (your iterator example):
// auto it = myMap.begin();
// // vs: std::map<std::string, std::vector<int>>::iterator it = ...
// -> auto saves massive typing, less error-prone

// 2. TEMPLATE RETURN TYPES:
// auto result = someTemplateFunction(x, y);
// // vs: having to know/write the exact return type

// 3. RANGE-BASED FOR (you already use this!):
// for (auto& element : myVector) { ... }
// for (const auto& element : myVector) { ... }

// 4. LAMBDA TYPES (impossible to write manually):
// auto myLambda = [](int x) { return x * 2; };
// // the type of a lambda is unique and UNNAMED
// // CANNOT write the type manually - must use auto!

// 5. STRUCTURED BINDINGS (C++17):
// auto [key, value] = myPair;
// // decomposes pair/tuple into named variables

// ============================================================
// SECTION 5: WHEN auto IS DANGEROUS / SHOULD BE AVOIDED
// ============================================================

// DANGER 1: STRING LITERAL (your example)
// auto s = "hello";   // const char*, NOT std::string
// s.size();           // ERROR! surprises people expecting std::string

// DANGER 2: HIDING IMPORTANT TYPE INFORMATION
// auto x = getResult();  // what type is x?? reader doesn't know!
// int x = getResult();   // obvious: x is an int
// -> auto hides type intent, makes code harder to read sometimes

// DANGER 3: PROXY TYPES (std::vector<bool>)
// std::vector<bool> bools = {true, false, true};
// auto val = bools[0];  // val is NOT bool!
//                        // it's std::vector<bool>::reference (proxy type!)
// bool val = bools[0];  // correct! forces conversion to actual bool

// DANGER 4: NARROWING/UNEXPECTED TYPES
// auto x = 3.14;   // double (not float, even though you might want float)
// auto x = 3.14f;  // float (explicit 'f' suffix needed)
// auto x = 5/2;    // int 2 (not float 2.5! integer division)

// DANGER 5: REFERENCE VS COPY AMBIGUITY
// auto x = someExpression;        // COPY (might be expensive!)
// auto& x = someExpression;       // reference (efficient, modifies original)
// const auto& x = someExpression; // const reference (efficient, read-only)
// -> easy to forget & and make expensive copies

// ============================================================
// SECTION 6: auto DEDUCTION RULES - WHAT IT STRIPS
// ============================================================

// auto STRIPS:
// -> top-level const:  const int x = 5; auto y = x;  -> y is int (not const int!)
// -> references:       int& r = x;      auto y = r;  -> y is int (not int&!)
// -> volatile
//
// PRESERVES:
// -> pointer-ness: int* p; auto q = p; -> q is int*
// -> const-ness of what's pointed to: const int* p; auto q = p; -> const int*
//
// TO KEEP const/ref with auto:
// auto x = val;          // copy, strips const
// auto& x = val;         // reference, keeps const if val is const
// const auto& x = val;   // const reference (most common for read-only)
// auto* x = &val;        // pointer (keeps pointed-to const)

// ============================================================
// SECTION 7: AAA STYLE - ALMOST ALWAYS AUTO
// ============================================================

// Herb Sutter's "AAA style" = Almost Always Auto
// Some developers use auto everywhere possible:
//
// auto x = int{5};          // int
// auto y = float{3.14f};    // float
// auto z = std::string{"hi"}; // std::string
//
// ARGUMENTS FOR AAA:
// -> never accidentally declare wrong type
// -> changes to return type propagate automatically
// -> forces explicit initialization (auto x; -> ERROR, must initialize)
//
// ARGUMENTS AGAINST AAA:
// -> reduces code readability (what type IS this?)
// -> proxy type pitfalls
// -> string literal trap (yours)
//
// BALANCE: use auto when type is OBVIOUS or VERY LONG
//          use explicit type when type MATTERS for readability

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is auto in C++?
//     -> Keyword that lets the compiler deduce the type of a variable
//        from its initializer. Type is still fixed at compile time -
//        auto is NOT dynamic typing.

// Q2: What type does auto value = "hello" deduce?
//     -> const char* (C-style string pointer), NOT std::string.
//        Common gotcha! To get std::string:
//        auto value = std::string("hello") or auto value = "hello"s (C++14).

// Q3: Why is auto useful for iterators?
//     -> Saves writing long types like std::vector<int>::iterator.
//        If container type changes, auto adapts automatically.
//        Reduces code maintenance burden.

// Q4: What does auto strip during type deduction?
//     -> Top-level const and references are stripped.
//        const int x = 5; auto y = x; -> y is int (not const int).
//        int& r = x; auto y = r; -> y is int (not int&).
//        Use const auto& to preserve both.

// Q5: What is the proxy type problem with auto?
//     -> std::vector<bool>::operator[] returns a proxy reference type,
//        not bool. auto val = bools[0] gives proxy type, not bool.
//        Can cause subtle bugs. Use bool val = bools[0] explicitly.

// Q6: When should you NOT use auto?
//     -> When type is important for readability.
//        With string literals (gets const char* not std::string).
//        With proxy types (vector<bool>).
//        When you need specific numeric type (double vs float).

// Q7: Can you declare auto without initializer?
//     -> NO. auto x; is a COMPILE ERROR.
//        auto REQUIRES an initializer to deduce from.
//        This is actually a benefit: forces initialization!

// Q8: What is the difference between auto, auto&, and const auto&?
//     -> auto:        copy, strips const and ref (might be expensive)
//        auto&:       reference, efficient, can modify, keeps type
//        const auto&: const reference, efficient, read-only
//        Use const auto& for range-for over large collections.
// ```

// **The string literal trap - most important gotcha:**

// ```cpp
// auto value = "mike";           // const char* ← NOT std::string!
// value.size();                  // ERROR: const char* has no .size()

// // FIXES:
// std::string value = "mike";           // explicit type (clearest)
// auto value = std::string("mike");     // explicit construction
// auto value = "mike"s;                 // 's' suffix = std::string (C++14)
// ```

// **auto deduction rules:**

// ```cpp
// const int x = 5;
// auto  y = x;   // y is int        (const STRIPPED)
// auto& z = x;   // z is const int& (const KEPT via reference)

// int arr[] = {1,2,3};
// auto  a = arr;  // a is int*      (array DECAYS to pointer)
// auto& b = arr;  // b is int(&)[3] (no decay, keeps array type)
// ```

// > **The balanced answer for interviews**: use `auto` when the type is **obvious** from context (iterators, lambdas, complex template types) or when the exact type **doesn't matter** for understanding. Avoid it when the type **communicates intent** (prefer `std::string` over `auto` for strings, `bool` over `auto` for `vector<bool>` elements) or when it **hides important information** from the reader.