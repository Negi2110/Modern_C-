// ============================================================
// std::string - METHODS, DSA PATTERNS - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <string>

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::string = std::basic_string<char>
// Dynamic array of chars with null terminator MANAGED FOR YOU
// Unlike const char* / char* - handles memory automatically
//
// KEY DIFFERENCE:
// const char* s = "mike";  // pointer to read-only memory, NO methods
// char* s = "mike";        // pointer to chars, manual memory, NO methods
// std::string s = "mike";  // OBJECT with methods, auto memory management
//
// "mike" in code = string LITERAL = const char[5] = {'m','i','k','e','\0'}
// YOUR COMMENT: "array of characters ending with null terminating character"

// ============================================================
// SECTION 2: CONSTRUCTION / INITIALIZATION
// ============================================================

void construction_examples()
{
    std::string s1;                    // empty string ""
    std::string s2("mike");            // from const char*
    std::string s3 = "mike";          // same (implicit conversion)
    std::string s4(s2);               // copy constructor
    std::string s5(5, 'a');           // "aaaaa" - n copies of char
    std::string s6(s2, 1, 2);        // substr from index 1, length 2 = "ik"
    std::string s7 = "mike";         // string literal (C++14, needs using namespace std::literals)

    // FROM const char* (for C interop):
    const char* cstr = "hello";
    std::string s8(cstr);             // const char* -> string
    std::string s9(cstr, 3);         // first 3 chars = "hel"
}

// ============================================================
// SECTION 3: SIZE AND CAPACITY METHODS
// ============================================================

void size_capacity()
{
    std::string s("mike");

    // SIZE / LENGTH - identical, use .size() (consistent with STL)
    s.size();      // 4 - number of chars (NOT counting '\0')
    s.length();    // 4 - same as size() (historical alias)
    s.empty();     // false - true if size() == 0
                   // prefer empty() over size() == 0 for clarity

    // CAPACITY - internal buffer management
    s.capacity();  // 15 typically (SSO: Small String Optimization)
                   // SSO = strings ≤ 15 chars stored ON STACK (no heap!)
                   // "mike" (4 chars) = stored in-place, capacity = 15
                   // YOUR COMMENT: "15 - that was what allocated for string"

    s.reserve(100); // pre-allocate capacity for 100 chars
                    // USE IN DSA: before building long string in loop
                    // avoids repeated reallocation!

    s.shrink_to_fit(); // YOUR COMMENT: "non-binding request to reduce capacity to size"
                       // compiler may IGNORE this - implementation defined
                       // useful after: remove lots of chars, want memory back

    s.max_size();  // maximum possible size (platform dependent, ~2^63)

    // CLEAR
    s.clear();     // removes all chars, size=0 but capacity unchanged
}

// ============================================================
// SECTION 4: ACCESS METHODS
// ============================================================

void access_methods()
{
    std::string s("mike");

    // ELEMENT ACCESS:
    s[0];          // 'm' - NO bounds check (UB if out of range)
    s.at(0);       // 'm' - WITH bounds check (throws std::out_of_range)
    s.front();     // 'm' - first char
    s.back();      // 'e' - last char

    // C INTEROP:
    s.c_str();     // const char* - null-terminated C string
                   // YOUR COMMENT: "non-modifiable standard C character array"
                   // USE: passing to C functions (printf, fopen etc.)

    s.data();      // const char* (C++11: same as c_str())
                   // C++17: returns char* (writable!)
                   // YOUR COMMENT: "non-modifiable standard C char array version"
                   // data() pre-C++17 = NOT guaranteed null-terminated!

    // ADDRESS:
    // std::cout << &s; // prints address of string OBJECT (not the chars)
    // std::cout << s.data(); // prints the actual char data
}

// ============================================================
// SECTION 5: MODIFICATION - MOST USED IN DSA
// ============================================================

void modification_methods()
{
    std::string s("mike");

    // APPEND (most common):
    s += " shah";           // append string  -> "mike shah"
    s += '!';               // append char    -> "mike shah!"
    s.append(" hello");     // same as +=     -> "mike shah! hello"
    s.push_back('?');       // append one char

    // REMOVE:
    s.pop_back();           // remove last char (like vector)
    s.erase(0, 4);          // erase 4 chars starting at index 0
    s.erase(s.begin());     // erase using iterator
    s.clear();              // erase everything

    // INSERT:
    s = "hello";
    s.insert(5, " world");  // insert at index 5 -> "hello world"
    s.insert(s.begin(), 'H'); // insert at iterator position

    // REPLACE:
    s.replace(0, 5, "goodbye"); // replace chars [0,5) with "goodbye"

    // RESIZE:
    s.resize(3);    // truncate to 3 chars
    s.resize(10, 'x'); // extend to 10 chars, fill with 'x'
}

// ============================================================
// SECTION 6: SEARCH - CRITICAL FOR DSA
// ============================================================

void search_methods()
{
    std::string s("hello world hello");

    // FIND - returns index or std::string::npos if not found
    s.find("hello");         // 0  - first occurrence
    s.find("hello", 1);      // 12 - search starting from index 1
    s.find('o');             // 4  - find char
    s.rfind("hello");        // 12 - LAST occurrence (reverse find)

    // CHECKING npos (YOUR CODE):
    if (s.find("z") == std::string::npos) {
        std::cout << "not found\n";  // npos = size_t max value
    }
    // npos = (size_t)-1 = largest possible size_t value
    // NEVER compare find() result to -1! use npos

    // FIND variants:
    s.find_first_of("aeiou");    // find first vowel (any of these chars)
    s.find_last_of("aeiou");     // find last vowel
    s.find_first_not_of("aeiou"); // find first non-vowel
    s.find_last_not_of(" \t\n"); // find last non-whitespace (useful for trimming!)

    // CONTAINS (C++23):
    // s.contains("hello");  // true - cleaner than find != npos

    // STARTS WITH / ENDS WITH (C++20):
    // s.starts_with("hello");  // true
    // s.ends_with("hello");    // true
}

// ============================================================
// SECTION 7: SUBSTRING AND COMPARISON
// ============================================================

void substr_compare()
{
    std::string s("hello world");

    // SUBSTR - creates NEW string (allocates memory!)
    s.substr(6);       // "world"         - from index 6 to end
    s.substr(6, 3);    // "wor"           - from index 6, length 3
    s.substr(0, 5);    // "hello"         - first 5 chars

    // COMPARE:
    s.compare("hello world");  // 0 if equal, <0 if less, >0 if greater
    s == "hello world";        // true (prefer == for equality)
    s < "hello z";             // lexicographic comparison (for sorting)
}

// ============================================================
// SECTION 8: ITERATION
// ============================================================

void iteration_methods()
{
    std::string s("mike");

    // RANGE-BASED FOR (most common):
    for (auto element : s) {         // COPY each char
        std::cout << element;
    }
    for (const auto& ch : s) {       // const ref (prefer for read-only)
        std::cout << ch;
    }
    for (auto& ch : s) {             // reference (to modify)
        ch = std::toupper(ch);        // UPPER CASE each char
    }

    // ITERATOR:
    for (auto it = s.begin(); it != s.end(); ++it) {
        std::cout << *it;
    }

    // REVERSE ITERATION:
    for (auto it = s.rbegin(); it != s.rend(); ++it) {
        std::cout << *it;  // prints: ekim
    }
}

// ============================================================
// SECTION 9: C INTEROP - extern "C" and const char*
// ============================================================

// YOUR CODE: extern "C" { }
// extern "C" = "compile what's inside as C code (no name mangling)"
// USE WHEN: calling C functions from C++, or exposing C++ as C API
// -> DIS protocol C libraries (like OpenDIS) need this!
// -> YOUR ISSST: calling any C-based DIS library functions

void PrintString(const std::string& s)
{
    std::cout << s << std::endl;
    // YOUR COMMENT: "keep in mind passing string by const reference if not changing"
    // const string& = no copy (efficient) + read-only (safe)
    // for large strings: ALWAYS use const string& not string (by value)
}

// C function expecting const char*:
void c_function(const char* str) { }

void interop_example()
{
    std::string s = "hello";
    c_function(s.c_str());    // string -> const char* (safe)
    c_function(s.data());     // C++17: same as c_str()

    // NEVER do: c_function(&s[0]) on older code (not guaranteed null-terminated)
    // c_str() is ALWAYS guaranteed null-terminated
}

// ============================================================
// SECTION 10: DSA PATTERNS WITH STRING
// ============================================================

void dsa_patterns()
{
    // PATTERN 1: Build string efficiently (reserve first!)
    std::string result;
    result.reserve(1000);          // pre-allocate, avoid reallocation
    for (int i = 0; i < 100; i++) {
        result += "hello";         // no reallocation (reserved space)
    }

    // PATTERN 2: Check palindrome
    std::string s = "racecar";
    bool isPalin = (s == std::string(s.rbegin(), s.rend()));

    // PATTERN 3: Reverse string
    std::reverse(s.begin(), s.end());

    // PATTERN 4: Count char frequency
    std::string word = "hello";
    int freq[26] = {};
    for (char c : word) {
        freq[c - 'a']++;    // 'a'=0, 'b'=1, ..., 'z'=25
    }

    // PATTERN 5: Trim whitespace
    auto start = s.find_first_not_of(" \t\n");
    auto end = s.find_last_not_of(" \t\n");
    if (start != std::string::npos) {
        s = s.substr(start, end - start + 1);
    }

    // PATTERN 6: Split by delimiter
    std::string data = "a,b,c,d";
    std::vector<std::string> tokens;
    size_t pos = 0, found;
    while ((found = data.find(',', pos)) != std::string::npos) {
        tokens.push_back(data.substr(pos, found - pos));
        pos = found + 1;
    }
    tokens.push_back(data.substr(pos));  // last token

    // PATTERN 7: Convert to/from int
    std::string num = "42";
    int n = std::stoi(num);           // string -> int
    long l = std::stol(num);          // string -> long
    double d = std::stod("3.14");     // string -> double
    std::string back = std::to_string(42);  // int -> string

    // PATTERN 8: String to uppercase/lowercase
    std::string upper = "hello";
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    // "HELLO"
}

// ============================================================
// SECTION 11: COMPLEXITY CHEAT SHEET
// ============================================================

// | Operation          | Complexity | Notes                        |
// |--------------------|------------|------------------------------|
// | operator[]         | O(1)       | no bounds check              |
// | at()               | O(1)       | with bounds check            |
// | size() / length()  | O(1)       | cached value                 |
// | empty()            | O(1)       | prefer over size()==0        |
// | push_back()        | O(1) amort | may reallocate               |
// | pop_back()         | O(1)       |                              |
// | insert() at end    | O(1) amort |                              |
// | insert() at middle | O(n)       | must shift chars             |
// | erase() at end     | O(1)       |                              |
// | erase() at middle  | O(n)       | must shift chars             |
// | find()             | O(n*m)     | n=string len, m=pattern len  |
// | substr()           | O(n)       | allocates new string!        |
// | compare() / ==     | O(n)       |                              |
// | +=(string)         | O(n) amort | prefer over + for building   |
// | + (concat)         | O(n)       | creates new string (costly!) |

// ============================================================
// SECTION 12: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between std::string, const char*, and char*?
//     -> char*: raw pointer to chars, manual memory, no methods.
//        const char*: pointer to read-only chars (string literals).
//        std::string: object, auto memory management, full API.

// Q2: What is std::string::npos?
//     -> Maximum value of size_t ((size_t)-1). Returned by find()
//        when element not found. Always check == npos, never == -1.

// Q3: What is Small String Optimization (SSO)?
//     -> std::string stores short strings (≤15-22 chars, impl defined)
//        directly in the object (stack) without heap allocation.
//        "mike" (4 chars) = no heap, capacity = 15 on most compilers.

// Q4: What is the difference between c_str() and data()?
//     -> Both return const char*. c_str() ALWAYS null-terminated.
//        Pre-C++17: data() not guaranteed null-terminated.
//        C++17: data() also null-terminated and returns char* (writable).
//        Use c_str() for C API interop to be safe.

// Q5: Why use reserve() before building a string in a loop?
//     -> Without reserve: each append may trigger reallocation (copy all)
//        = O(n²) total. With reserve: only one allocation, O(n) total.

// Q6: What is the complexity of string concatenation with + vs +=?
//     -> + creates a NEW temporary string each time: O(n) + allocation.
//        += appends in place (amortized O(1)): no new string created.
//        In loops: ALWAYS use += not +. Build final with += then assign.

// Q7: How do you convert between string and int?
//     -> string to int: std::stoi(), stol(), stoll(), stod(), stof()
//        int to string: std::to_string()
//        These throw std::invalid_argument or std::out_of_range on failure.

// Q8: When would you use string_view over string? (C++17)
//     -> std::string_view = non-owning view into a string (no copy!)
//        void foo(std::string_view sv) - accepts string, const char*, literals
//        without any allocation. Read-only. Use for function parameters
//        when you only need to READ and not store the string.

// int main()
// {
//     std::string s("mike");
//     // Demonstrates SSO: "mike" = 4 chars, capacity = 15 (no heap!)

//     PrintString(s);          // const ref: efficient, no copy

//     // Find demo:
//     if (s.find("z") == std::string::npos) {
//         std::cout << "did not find z\n";
//     }
//     std::cout << s.find('i') << std::endl;  // prints index: 1

//     // Modification:
//     s[0] = 'M';              // direct access, no bounds check
//     std::cout << s << '\n';  // "Mike"

//     return 0;
// }
// ```

// **Quick method reference - the ones that matter most:**

// ```cpp
// std::string s = "hello world";

// // MUST KNOW:
// s.size()              // 11
// s.empty()             // false
// s.find("world")       // 6
// s.find("xyz")         // string::npos  ← always check this!
// s.substr(6, 5)        // "world"  ← creates new string (O(n)!)
// s += " again"         // append   ← prefer over +
// s.c_str()             // const char* for C APIs
// s[0]                  // 'h' no check  vs  s.at(0) with check
// std::to_string(42)    // int → string
// std::stoi("42")       // string → int
// ```

// **DSA golden rules for strings:**

// ```
// Building string in loop?     → reserve() first
// Searching substring?         → find() returns npos if not found
// Need char frequency?         → freq[c - 'a']++
// Passing to function?         → const string& (never by value)
// Need C API?                  → .c_str() (always null-terminated)
// Avoid + in loops             → use += instead (avoids O(n²))
// ```