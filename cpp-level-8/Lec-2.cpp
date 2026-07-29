#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

// ============================================================
// std::string vs const char* vs char* - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: THE THREE STRING TYPES
// ============================================================

// TYPE 1: String literal
// "hello" -> stored in READ-ONLY memory (data segment)
// type is const char[6] (includes \0)
// CANNOT modify

// TYPE 2: const char*
// const char* s = "hello";
// -> pointer to read-only chars
// -> NO methods (.size() etc dont exist)
// -> NO memory management
// -> CAN reassign pointer: s = "world"  OK
// -> CANNOT modify chars: s[0] = 'H'   UNDEFINED BEHAVIOR

// TYPE 3: char*
// char s[] = "hello";              // array on stack, MODIFIABLE
// char* s = new char[6];           // heap, manual delete needed
// -> CAN modify: s[0] = 'H'        OK
// -> NO methods
// -> manual memory management
// -> easy to forget null terminator = buffer overflow danger

// TYPE 4: std::string (DEFAULT CHOICE)
// std::string s = "hello";
// -> OBJECT with full API
// -> AUTO memory management
// -> MODIFIABLE
// -> ALL methods: size(), find(), substr() etc.
// -> SSO: short strings (<=15 chars) stored on STACK (no heap!)

// ============================================================
// SECTION 2: COMPARISON TABLE
// ============================================================

// Feature              const char*    char*          std::string
// Auto memory          NO             NO             YES
// Modifiable chars     NO             YES            YES
// .size() .find()      NO             NO             YES
// Null terminated      must trust     must manage    YES guaranteed
// Pass to C API        YES directly   YES directly   YES via .c_str()
// Comparison ==        compares ADDR  compares ADDR  compares CONTENT
// Concatenation        NO             manual         YES via +=

// ============================================================
// SECTION 3: THE COMPARISON TRAP
// ============================================================

// const char* a = "hello";
// const char* b = "hello";
// a == b   -> compares ADDRESSES not content! WRONG!

// std::string x = "hello";
// std::string y = "hello";
// x == y   -> compares CONTENT correctly

// ============================================================
// SECTION 4: CONSTRUCTION / INITIALIZATION
// ============================================================

// std::string s1;                   // empty string ""
// std::string s2("mike");           // from const char*
// std::string s3 = "mike";          // same (implicit conversion)
// std::string s4(s2);               // copy constructor
// std::string s5(5, 'a');           // "aaaaa" - n copies of char
// std::string s6(s2, 1, 2);         // substr from index 1, length 2 = "ik"
// std::string s7 = "mike"s;         // string literal suffix (C++14)

// FROM const char*:
// const char* cstr = "hello";
// std::string s8(cstr);             // const char* -> string (automatic)
// std::string s9(cstr, 3);          // first 3 chars = "hel"

// ============================================================
// SECTION 5: SIZE AND CAPACITY
// ============================================================

// s.size()        -> number of chars (NOT counting '\0')
// s.length()      -> identical to size() (historical alias)
// s.empty()       -> true if size() == 0 (prefer over size()==0)
// s.capacity()    -> how many chars can be stored without reallocation
//                    SSO: typically 15 for short strings (stack stored!)
// s.reserve(100)  -> pre-allocate space for 100 chars
//                    USE IN DSA: before building long string in loop
//                    avoids repeated reallocation = much faster!
// s.shrink_to_fit()-> non-binding request to reduce capacity to size
//                    implementation may ignore it
// s.max_size()    -> maximum possible size (~2^63 on 64-bit)
// s.clear()       -> removes all chars, size=0 but capacity unchanged

// ============================================================
// SECTION 6: ELEMENT ACCESS
// ============================================================

// s[0]            -> first char, NO bounds check (UB if out of range)
// s.at(0)         -> first char, WITH bounds check (throws out_of_range)
// s.front()       -> first char (same as s[0])
// s.back()        -> last char
// s.c_str()       -> const char* null-terminated (for C APIs)
//                    ALWAYS use this when passing to C functions
// s.data()        -> const char* C++11, char* C++17 (writable)

// ============================================================
// SECTION 7: MODIFICATION
// ============================================================

// s += " shah"    -> append string (PREFER OVER + in loops!)
// s += '!'        -> append single char
// s.append(" hi") -> same as +=
// s.push_back('?')-> append one char
// s.pop_back()    -> remove last char O(1)
// s.erase(0, 4)   -> erase 4 chars starting at index 0
// s.clear()       -> erase everything
// s.insert(5, " world")  -> insert string at index 5
// s.replace(0, 5, "bye") -> replace chars [0,5) with "bye"
// s.resize(3)     -> truncate to 3 chars
// s.resize(10,'x')-> extend to 10, fill new chars with 'x'

// WHY += not + in loops:
// s + "hello"  -> creates NEW temporary string each time O(n) + alloc
// s += "hello" -> appends in place, amortized O(1), no new string
// In a 1000 iteration loop: + is O(n^2), += is O(n)

// ============================================================
// SECTION 8: SEARCH - CRITICAL FOR DSA
// ============================================================

// s.find("hello")     -> index of first occurrence, or string::npos
// s.find("hello", 1)  -> search starting from index 1
// s.find('o')         -> find char, returns index or npos
// s.rfind("hello")    -> LAST occurrence (reverse find)
//
// ALWAYS check npos:
// if (s.find("z") == std::string::npos) { // not found }
// npos = (size_t)-1 = largest possible size_t value
// NEVER compare to -1, always use string::npos
//
// s.find_first_of("aeiou")     -> first char that is any of these
// s.find_last_of("aeiou")      -> last char that is any of these
// s.find_first_not_of("aeiou") -> first char NOT in this set
// s.find_last_not_of(" \t\n")  -> last non-whitespace (trim use!)
//
// C++20:
// s.starts_with("hello")       -> bool
// s.ends_with("world")         -> bool
// C++23:
// s.contains("ello")           -> bool (cleaner than find != npos)

// ============================================================
// SECTION 9: SUBSTRING AND COMPARISON
// ============================================================

// s.substr(6)       -> from index 6 to end (creates NEW string O(n)!)
// s.substr(6, 3)    -> from index 6, length 3
// s.substr(0, 5)    -> first 5 chars
// s.compare("str")  -> 0 equal, <0 less, >0 greater
// s == "str"        -> bool content comparison (prefer over compare)
// s < "str"         -> lexicographic (for sorting strings)

// ============================================================
// SECTION 10: ITERATION
// ============================================================

// for (const auto& ch : s)      -> read each char (prefer)
// for (auto& ch : s)            -> modify each char
// for (auto it = s.begin(); it != s.end(); ++it) -> iterator style
// for (auto it = s.rbegin(); it != s.rend(); ++it) -> reverse

// ============================================================
// SECTION 11: CONVERSION
// ============================================================

// string -> int:    std::stoi("42")     = 42
// string -> long:   std::stol("42")     = 42L
// string -> double: std::stod("3.14")   = 3.14
// string -> float:  std::stof("3.14")   = 3.14f
// int -> string:    std::to_string(42)  = "42"
// All stoi/stod throw: std::invalid_argument or std::out_of_range

// string -> const char* (EXPLICIT):
// std::string s = "hello";
// const char* p = s.c_str();   // OK must be explicit!
// const char* p = s;           // COMPILER ERROR

// const char* -> string (IMPLICIT):
// const char* cstr = "hello";
// std::string s = cstr;        // OK automatic

// ============================================================
// SECTION 12: DSA PATTERNS
// ============================================================

// PATTERN 1: Build string efficiently
// std::string result;
// result.reserve(1000);          // pre-allocate FIRST
// for (int i = 0; i < 100; i++)
//     result += "hello";         // no reallocation

// PATTERN 2: Check palindrome
// std::string s = "racecar";
// bool isPalin = (s == std::string(s.rbegin(), s.rend()));

// PATTERN 3: Reverse string
// std::reverse(s.begin(), s.end());

// PATTERN 4: Char frequency count (most common in DSA)
// std::string word = "hello";
// int freq[26] = {};
// for (char c : word)
//     freq[c - 'a']++;    // 'a'=0, 'b'=1, ..., 'z'=25

// PATTERN 5: Trim whitespace
// auto start = s.find_first_not_of(" \t\n");
// auto end   = s.find_last_not_of(" \t\n");
// if (start != std::string::npos)
//     s = s.substr(start, end - start + 1);

// PATTERN 6: Split by delimiter
// std::vector<std::string> tokens;
// size_t pos = 0, found;
// while ((found = data.find(',', pos)) != std::string::npos) {
//     tokens.push_back(data.substr(pos, found - pos));
//     pos = found + 1;
// }
// tokens.push_back(data.substr(pos)); // last token

// PATTERN 7: To uppercase/lowercase
// std::transform(s.begin(), s.end(), s.begin(), ::toupper);
// std::transform(s.begin(), s.end(), s.begin(), ::tolower);

// ============================================================
// SECTION 13: COMPLEXITY CHEAT SHEET
// ============================================================

// operator[]       O(1)      no bounds check
// at()             O(1)      with bounds check (throws)
// size() empty()   O(1)      cached
// push_back()      O(1) avg  may reallocate
// pop_back()       O(1)
// insert at end    O(1) avg
// insert at middle O(n)      must shift chars
// erase at end     O(1)
// erase at middle  O(n)      must shift chars
// find()           O(n*m)    n=string len, m=pattern len
// substr()         O(n)      allocates new string!
// == compare       O(n)
// +=               O(1) avg  prefer over + for building strings
// + concat         O(n)      creates new string every time

// ============================================================
// SECTION 14: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between std::string, const char*, and char*?
//     -> char*: raw pointer, manual memory, no methods
//        const char*: pointer to read-only chars, no methods
//        std::string: object, auto memory, full API, SSO optimization

// Q2: What is std::string::npos?
//     -> Maximum value of size_t ((size_t)-1)
//        Returned by find() when element not found
//        Always check == npos, NEVER compare to -1

// Q3: What is Small String Optimization (SSO)?
//     -> Short strings (<=15 chars on most compilers) stored
//        directly in the string object on the stack
//        No heap allocation for "mike" (4 chars)
//        capacity = 15 even for a 4-char string

// Q4: Why use += not + when building strings in a loop?
//     -> + creates a NEW temporary string each iteration: O(n^2) total
//        += appends in place amortized O(1): O(n) total
//        Also reserve() first if you know the final size

// Q5: What does c_str() do and when do you use it?
//     -> Returns const char* that is always null-terminated
//        Use ONLY when passing std::string to C API functions
//        The pointer is invalidated if string is modified!

// Q6: What is the difference between s[0] and s.at(0)?
//     -> s[0]: no bounds check, UB if out of range (faster)
//        s.at(0): bounds check, throws std::out_of_range (safer)
//        Use at() when index might be invalid

// Q7: What is std::string_view (C++17)?
//     -> Non-owning read-only view into a string
//        void foo(std::string_view sv) accepts string, const char*,
//        and string literals WITHOUT any allocation or copy
//        Use for function parameters where you only need to read

// Q8: How do you convert between string and numeric types?
//     -> string to int:    std::stoi("42")
//        string to double: std::stod("3.14")
//        int to string:    std::to_string(42)
//        These throw invalid_argument/out_of_range on failure

int main()
{
    // ---- BASIC USAGE ----
    std::string s("mike");
    std::cout << "string: "    << s          << '\n';
    std::cout << "size: "      << s.size()   << '\n';
    std::cout << "capacity: "  << s.capacity()<< '\n';
    std::cout << "empty: "     << s.empty()  << '\n';

    // ---- ACCESS ----
    std::cout << "front: "     << s.front()  << '\n';
    std::cout << "back: "      << s.back()   << '\n';
    std::cout << "s[0]: "      << s[0]       << '\n';
    std::cout << "c_str: "     << s.c_str()  << '\n';

    // ---- MODIFICATION ----
    s += " shah";
    std::cout << "after +=: "  << s          << '\n';
    s[0] = 'M';
    std::cout << "after s[0]='M': " << s     << '\n';

    // ---- SEARCH ----
    std::string sentence = "hello world hello";
    size_t pos = sentence.find("hello");
    std::cout << "first hello at: " << pos   << '\n';
    std::cout << "last hello at: "  << sentence.rfind("hello") << '\n';

    if (sentence.find("xyz") == std::string::npos) {
        std::cout << "xyz not found (npos)\n";
    }

    // ---- SUBSTR ----
    std::cout << "substr: " << sentence.substr(6, 5) << '\n'; // "world"

    // ---- ITERATION ----
    std::string word = "hello";
    std::cout << "chars: ";
    for (const auto& ch : word) {
        std::cout << ch << " ";
    }
    std::cout << '\n';

    // ---- CONVERSION ----
    std::cout << "stoi: "       << std::stoi("42")      << '\n';
    std::cout << "to_string: "  << std::to_string(100)  << '\n';

    // ---- DSA PATTERN: char frequency ----
    int freq[26] = {};
    for (char c : word) {
        freq[c - 'a']++;
    }
    std::cout << "freq of l: " << freq['l' - 'a'] << '\n'; // 2

    // ---- DSA PATTERN: reverse ----
    std::string rev = word;
    std::reverse(rev.begin(), rev.end());
    std::cout << "reversed: " << rev << '\n'; // "olleh"

    // ---- DSA PATTERN: uppercase ----
    std::string up = word;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    std::cout << "uppercase: " << up << '\n'; // "HELLO"

    // ---- DSA PATTERN: split by delimiter ----
    std::string data = "a,b,c,d";
    std::vector<std::string> tokens;
    size_t start = 0, found;
    while ((found = data.find(',', start)) != std::string::npos) {
        tokens.push_back(data.substr(start, found - start));
        start = found + 1;
    }
    tokens.push_back(data.substr(start));
    std::cout << "tokens: ";
    for (const auto& t : tokens) std::cout << t << " ";
    std::cout << '\n';

    return 0;
}