#include <iostream>
#include <string>
#include <string_view>  // C++17

// ============================================================
// std::string_view - WHAT IT IS, WHEN TO USE - INTERVIEW NOTES
// ============================================================

// ============================================================
// SECTION 1: THE PROBLEM string_view SOLVES
// ============================================================

// BEFORE string_view - what happens with const string&?
//
// void Print(const std::string& s) { std::cout << s; }
//
// Print("hello");           // const char* -> std::string
//                           // ALLOCATION HAPPENS! "hello" copied to heap
//                           // temporary std::string created just to call Print
//
// const char* cstr = "hello";
// Print(cstr);              // SAME: allocation + copy into std::string
//
// std::string str = "hello";
// Print(str);               // OK: no copy (const ref), but still std::string
//
// PROBLEM: passing string literals or const char* to const string&
// forces a HEAP ALLOCATION every time!
// For a logging function called 10000 times/second -> 10000 allocations!

// ============================================================
// SECTION 2: WHAT IS string_view?
// ============================================================

// YOUR COMMENT shows the internal structure:
// class string_view {
//     const char* data;    // pointer to the EXISTING string data
//     size_t length;       // how many chars to look at
// };
//
// string_view = a READ-ONLY WINDOW into existing string data
// -> does NOT own the data (no allocation, no copy!)
// -> just a pointer + length (16 bytes on 64-bit)
// -> like a "slice" or "view" into a string
//
// sizeof(std::string)      = 32 bytes (ptr + size + capacity)
// sizeof(std::string_view) = 16 bytes (ptr + length ONLY)
// YOUR CODE confirms this: "string:32, string_view:16"

// ============================================================
// SECTION 3: HOW IT ACCEPTS ALL STRING TYPES
// ============================================================

// void Print(std::string_view param) accepts:
//
// 1. std::string (no copy - view into string's buffer)
//    std::string str = "mike";
//    Print(str);             // sview.data = str.data(), no allocation
//
// 2. const char* (no copy - view into literal)
//    const char* s = "hello";
//    Print(s);               // sview.data = s, no allocation
//
// 3. String literal (no copy - view into read-only segment)
//    Print("hello");         // sview.data = "hello" in data segment
//
// 4. char array (no copy)
//    char arr[] = "hello";
//    Print(arr);
//
// ALL of the above: ZERO ALLOCATION, ZERO COPY
// This is why string_view exists!

// ============================================================
// SECTION 4: YOUR CODE EXPLAINED
// ============================================================

void PrintString(std::string_view param)
// NOT const std::string& (which would force allocation for const char*)
// std::string_view = accepts ANYTHING, zero copy, zero allocation
{
    // param += "some more text";  // COMPILER ERROR! string_view is READ-ONLY
    //                               cannot modify the underlying data
    std::cout << param << std::endl;
}

int main()
{
    // const char* passed to string_view: ZERO allocation (just sets pointer)
    const char* s = "this is some really long string ...";
    PrintString(s);
    // string_view internally: { data=s, length=35 }
    // no heap allocation at all!

    // std::string passed to string_view: ZERO copy
    std::string str = "mike";
    std::string_view sview = str;
    // sview.data = str.data() (points INTO str's buffer)
    // sview.length = 4
    // NO new allocation, sview just LOOKS AT str's memory

    std::cout << "string:"      << sizeof(str)   << std::endl;  // 32
    std::cout << "string_view:" << sizeof(sview) << std::endl;  // 16

    return 0;
}

// ============================================================
// SECTION 5: string_view METHODS (subset of string)
// ============================================================

// string_view has MOST READ methods of string (no modifying ones):
//
// sview.size()           -> number of chars
// sview.length()         -> same as size()
// sview.empty()          -> true if empty
// sview.data()           -> const char* (NOT guaranteed null-terminated!)
// sview[0]               -> char at index (no bounds check)
// sview.at(0)            -> char at index (with bounds check)
// sview.front()          -> first char
// sview.back()           -> last char
// sview.find("hello")    -> index or string_view::npos
// sview.substr(0, 4)     -> ANOTHER string_view (no allocation!)
// sview.starts_with("hi")-> bool (C++20)
// sview.ends_with("lo")  -> bool (C++20)
// sview.contains("ell")  -> bool (C++23)
//
// MISSING (because read-only, no ownership):
// NO push_back, pop_back
// NO append, +=
// NO insert, erase, replace
// NO reserve, resize, clear
// NO c_str() (not guaranteed null-terminated!)

// UNIQUE TO string_view (not in string):
// sview.remove_prefix(3)  -> moves start pointer forward 3 (like ltrim)
// sview.remove_suffix(3)  -> shrinks length by 3 (like rtrim)

// ============================================================
// SECTION 6: CRITICAL DANGER - LIFETIME
// ============================================================

// string_view does NOT own data -> underlying data MUST outlive the view!
//
// DANGER 1: dangling string_view from temporary
// std::string_view bad()
// {
//     std::string temp = "hello";     // temp on stack
//     return std::string_view(temp);  // DANGLING! temp destroyed on return
// }                                   // sview now points to freed memory
// auto sv = bad();
// std::cout << sv;  // UNDEFINED BEHAVIOR - crash or garbage
//
// DANGER 2: string modification invalidates view
// std::string str = "hello";
// std::string_view sv = str;          // sv points into str's buffer
// str += " world world world world";  // str REALLOCATES (new buffer)!
// std::cout << sv;    // DANGLING! sv points to OLD freed buffer
//
// RULE: string_view is ONLY safe when you know the source string
//       will NOT be modified or destroyed while the view exists
//
// SAFE: as a FUNCTION PARAMETER (source outlives the function call)
// UNSAFE: stored as a MEMBER VARIABLE (source might go out of scope)

// ============================================================
// SECTION 7: NO c_str() - IMPORTANT DIFFERENCE
// ============================================================

// std::string has c_str() -> guaranteed null-terminated
// std::string_view has data() -> NOT guaranteed null-terminated!
//
// "hello" in string_view might point to middle of a larger string:
// std::string s = "hello world";
// std::string_view sv = s;
// sv = sv.substr(6, 5);          // sv = "world" (view into middle)
// sv.data()                      // points to 'w' in "world\0" (OK here)
// BUT: sv.data() after remove_suffix(2) -> "wor" (no \0 after 'r'!)
//
// NEVER pass string_view::data() to a C function expecting null-terminated!
// Convert to string first: std::string(sv).c_str()

// ============================================================
// SECTION 8: WHEN TO USE WHAT
// ============================================================

// USE std::string WHEN:
// -> you need to OWN and STORE the string
// -> you need to MODIFY the string
// -> you need to pass to C API (.c_str())
// -> lifetime management is complex
// -> member variables that store strings
//
// void storeString(std::string s);          // stores own copy
// class Foo { std::string m_name; };        // owns the string
//
// USE std::string_view WHEN:
// -> function parameter that only READS the string
// -> you know the source will outlive the view
// -> working with substrings without allocation
// -> parsing large strings (lots of substr operations)
//
// void read(std::string_view sv);          // read-only, zero copy
// std::string_view prefix = sv.substr(0,3);// no allocation!
//
// USE const std::string& WHEN:
// -> function needs to call string-specific methods (.c_str() etc.)
// -> API compatibility with code that always uses string
// -> C++14 or earlier (no string_view)
//
// DECISION FLOWCHART:
// Read only? YES -> string_view
// Need to modify? YES -> string (by value or &)
// Storing as member? YES -> string
// Passing to C API? YES -> string (then .c_str())

// ============================================================
// SECTION 9: PERFORMANCE COMPARISON
// ============================================================

// void foo_bad(const std::string& s)   // forces allocation for literals
// void foo_good(std::string_view sv)   // zero allocation for anything
//
// Benchmark:
// foo_bad("hello") called 1M times:
//   -> 1M heap allocations + 1M heap frees
//   -> slow, cache unfriendly
//
// foo_good("hello") called 1M times:
//   -> 0 heap allocations
//   -> just copies 16 bytes (pointer + length)
//   -> much faster
//
// FOR LOGGING / HOT PATH CODE:
// Always prefer string_view for read-only string parameters!
// In ISSST: PDU field name logging, entity type printing etc.

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::string_view?
//     -> A non-owning read-only view into existing string data.
//        Contains just a pointer and length (16 bytes).
//        No allocation, no copy. C++17 feature.

// Q2: What is the sizeof string vs string_view?
//     -> std::string:      32 bytes (pointer + size + capacity)
//        std::string_view: 16 bytes (pointer + length only)

// Q3: Why use string_view instead of const string&?
//     -> const string& forces allocation when passing string literals
//        or const char*. string_view accepts all string types with
//        ZERO allocation and ZERO copy.

// Q4: What is the main danger of string_view?
//     -> Dangling views. If the underlying string is destroyed or
//        reallocated (e.g., string += causes reallocation), the
//        string_view becomes a dangling pointer -> UB.

// Q5: Does string_view have c_str()?
//     -> NO. string_view::data() is NOT guaranteed null-terminated.
//        Never pass data() to C functions expecting null-terminated strings.
//        Convert: std::string(sv).c_str() if you need null-termination.

// Q6: Can you modify data through string_view?
//     -> NO. string_view is always read-only (const char* internally).
//        No push_back, +=, insert, erase etc.

// Q7: When is it UNSAFE to use string_view?
//     -> As a class member variable (source might go out of scope).
//        As a return value from a function (source destroyed on return).
//        After modifying the source string (reallocation invalidates view).

// Q8: What does remove_prefix/remove_suffix do?
//     -> remove_prefix(n): moves the start pointer forward n positions
//        remove_suffix(n): shrinks the length by n
//        Both are O(1), no allocation.
//        Useful for parsing: skip leading/trailing chars without copying.
// ```

// **The key visual:**

// ```
// std::string str = "mike";

// MEMORY:                         OBJECT:
// str:  [ptr][size=4][cap=32]     32 bytes on stack
//        |
//        v
//       heap: ['m','i','k','e','\0']

// std::string_view sview = str;

// sview: [ptr][length=4]          16 bytes on stack
//         |
//         v (points to SAME heap data as str - no copy!)
//        heap: ['m','i','k','e','\0']
// ```

// **The one rule:**

// ```
// Function only READS a string? → string_view   (zero allocation)
// Function STORES or MODIFIES?  → std::string   (owns the data)
// ```