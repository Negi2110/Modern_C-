// ============================================================
// INTEGER/FLOAT SUFFIXES AND DIGIT SEPARATORS - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: YOUR QUESTION - WHY 'f' SUFFIX FOR FLOAT?
// ============================================================

// DEFAULT type of floating-point literals in C++:
// 3.14      -> double  (no suffix = double by default)
// 3.14f     -> float   ('f' suffix)
// 3.14l     -> long double ('l' or 'L' suffix)
//
// WHY does this matter?
//
// float pi = 3.141526;       // implicit narrowing: double -> float
//                             // compiler might warn: "narrowing conversion"
//                             // loses precision (double has more digits than float)
//
// float pi = 3.141526f;      // correct: float literal -> float variable
//                             // no conversion needed, exact type match
//
// IF YOU DON'T WRITE 'f':
// float pi = 3.141526;
// STEP 1: 3.141526 created as DOUBLE (15-16 significant digits)
// STEP 2: double CONVERTED to float (6-7 significant digits)
// STEP 3: precision LOST silently (or with warning)
//
// {} initialization would catch this:
// float pi{3.141526};   // COMPILER ERROR: narrowing conversion!
// float pi{3.141526f};  // OK: float literal -> float variable
// -> another reason to use {} !

// ============================================================
// SECTION 2: auto WITH FLOATING POINT
// ============================================================

// auto pi = 3.141526;   // YOUR CODE: pi deduced as DOUBLE
//                        // because literal has no suffix = double default
//
// auto pi = 3.141526f;  // pi deduced as FLOAT
// auto pi = 3.141526l;  // pi deduced as LONG DOUBLE
//
// PROOF:
// auto x = 3.14;    std::cout << sizeof(x) << "\n";  // 8 (double)
// auto x = 3.14f;   std::cout << sizeof(x) << "\n";  // 4 (float)
// auto x = 3.14l;   std::cout << sizeof(x) << "\n";  // 16 (long double)

// ============================================================
// SECTION 3: ALL FLOATING POINT SUFFIXES
// ============================================================

// NO suffix    -> double  (default)
// f or F       -> float
// l or L       -> long double
//
// 3.14         // double
// 3.14f        // float
// 3.14F        // float (same as lowercase f)
// 3.14l        // long double
// 3.14L        // long double (prefer uppercase L - lowercase l looks like 1!)
//
// SIZES (64-bit system):
// float       = 4 bytes  (~6-7  significant decimal digits)
// double      = 8 bytes  (~15-16 significant decimal digits)
// long double = 16 bytes (~18-19 significant decimal digits)

// ============================================================
// SECTION 4: INTEGER SUFFIXES
// ============================================================

// NO suffix        -> int
// u or U           -> unsigned int
// l or L           -> long
// ll or LL         -> long long
// ul or UL         -> unsigned long
// ull or ULL       -> unsigned long long
//
// YOUR CODE: auto mi = 1'000'000'0ul;
// -> 10000000 with suffix ul = unsigned long
//
// EXAMPLES:
// 42           // int
// 42u          // unsigned int
// 42l          // long
// 42ll         // long long
// 42ul         // unsigned long
// 42ull        // unsigned long long
// 42ULL        // unsigned long long (uppercase preferred for clarity)
//
// WHY do we need these?
// int max = 2,147,483,647
// auto x = 2147483648;    // ERROR or overflow: too big for int!
// auto x = 2147483648u;   // OK: unsigned int (max 4,294,967,295)
// auto x = 2147483648ll;  // OK: long long (max 9.2 quintillion)

// ============================================================
// SECTION 5: DIGIT SEPARATOR ' (C++14)
// ============================================================

// YOUR CODE: auto mi = 1'000'000'0ul;
//
// Single quote ' = DIGIT SEPARATOR (C++14 feature)
// -> purely for HUMAN READABILITY
// -> compiler COMPLETELY ignores the quotes
// -> 1'000'000'0 = 10000000 (same value)
//
// BEFORE C++14 (hard to read):
// auto x = 1000000000;    // is this a billion? hard to tell!
// auto x = 0xDEADBEEF;    // hard to count hex digits
//
// WITH C++14 digit separator:
// auto x = 1'000'000'000;  // clearly 1 billion
// auto x = 0xDEAD'BEEF;    // clearly 0xDEAD and 0xBEEF
// auto x = 0b1010'0101;    // binary, clearly 4+4 bits
//
// CAN BE PLACED ANYWHERE (between any two digits):
// 1'0'0'0   = 1000    (weird but valid)
// 1'000'000 = 1000000 (conventional)
// 1000'000  = 1000000 (also valid)
//
// CANNOT be at start or end:
// '1000     // ERROR: starts with '
// 1000'     // ERROR: ends with '
// 1''000    // ERROR: consecutive quotes

// ============================================================
// SECTION 6: YOUR ACTUAL CODE
// ============================================================

int main()
{
    auto pi = 3.141526;
    // no suffix -> DOUBLE
    // pi is double, 8 bytes
    // ~15 significant digits stored correctly
    //
    // to make it float: auto pi = 3.141526f;
    // to be explicit:   double pi = 3.141526;

    auto mi = 1'000'000'0ul;
    // digit separators: 1'000'000'0 = 10000000
    // suffix ul = unsigned long
    // mi = 10,000,000 as unsigned long
    //
    // NOTE: unusual placement of separator (before last 0)
    // more readable would be: 10'000'000ul

    return 0;
}

// ============================================================
// SECTION 7: BINARY AND HEX LITERALS (C++14)
// ============================================================

// C++14 also added binary literals:
// 0b prefix = binary
// 0x prefix = hexadecimal (existed since C)
//
// 0b1010        = 10 in decimal
// 0b1111'0000   = 240 in decimal (digit separator helps!)
// 0xFF          = 255 in decimal
// 0xDEAD'BEEF   = 3,735,928,559 in decimal
//
// COMBINED with suffixes:
// 0b1010u        // unsigned int binary
// 0xFFFFull      // unsigned long long hex

// ============================================================
// SECTION 8: COMPLETE SUFFIX TABLE
// ============================================================

// FLOATING POINT:
// | Suffix    | Type        | Size    | Precision        |
// |-----------|-------------|---------|------------------|
// | (none)    | double      | 8 bytes | ~15-16 digits    |
// | f or F    | float       | 4 bytes | ~6-7 digits      |
// | l or L    | long double | 16 bytes| ~18-19 digits    |

// INTEGER:
// | Suffix    | Type                | Typical Size |
// |-----------|---------------------|--------------|
// | (none)    | int                 | 4 bytes      |
// | u or U    | unsigned int        | 4 bytes      |
// | l or L    | long                | 4 or 8 bytes |
// | ll or LL  | long long           | 8 bytes      |
// | ul or UL  | unsigned long       | 4 or 8 bytes |
// | ull or ULL| unsigned long long  | 8 bytes      |

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the default type of a floating-point literal?
//     -> double. 3.14 is double, not float.
//        Use 'f' suffix for float: 3.14f.
//        Use 'L' suffix for long double: 3.14L.

// Q2: Why write 3.14f instead of just 3.14 for a float variable?
//     -> 3.14 is double, assigning to float = narrowing conversion.
//        Compiler may warn. {} initialization would error.
//        3.14f creates a float literal directly, no conversion needed.

// Q3: What does auto pi = 3.14 deduce?
//     -> double (no suffix = double default).
//        auto pi = 3.14f -> float.
//        auto pi = 3.14L -> long double.

// Q4: What is the digit separator in C++14?
//     -> Single quote ' placed between digits for readability.
//        1'000'000 = 1000000. Compiler ignores the quotes completely.
//        Can be used in decimal, hex (0xDEAD'BEEF), and binary (0b1010'0101).

// Q5: What does the 'ul' suffix mean in 10000000ul?
//     -> unsigned long. u = unsigned, l = long.
//        Combined: unsigned long integer literal.
//        Other combinations: ull = unsigned long long, ll = long long.

// Q6: Why might you need 'ull' suffix?
//     -> When a value exceeds int or long range.
//        auto x = 10000000000; might overflow int.
//        auto x = 10000000000ull; guarantees unsigned long long.

// Q7: What does 0b1010 mean?
//     -> Binary literal (C++14). 0b prefix = binary.
//        1010 in binary = 10 in decimal.
//        Can use digit separator: 0b1010'0101 for clarity.

// Q8: What happens if you write float pi = 3.14 without 'f'?
//     -> 3.14 is double. Implicit narrowing conversion: double -> float.
//        Loses some precision (double has more significant digits).
//        Compiler may warn with -Wall (-Wnarrowing).
//        float pi{3.14} would be a COMPILE ERROR (narrowing in {}).
// ```

// **Suffix quick reference - the most tested:**

// ```cpp
// // FLOATING POINT:
// 3.14      // double  ← DEFAULT (no suffix)
// 3.14f     // float   ← needs 'f'
// 3.14L     // long double

// // INTEGER:
// 42        // int     ← DEFAULT
// 42u       // unsigned int
// 42ll      // long long
// 42ull     // unsigned long long  ← most common for large values

// // DIGIT SEPARATOR (C++14, readability only):
// 1'000'000         // = 1000000
// 0xDEAD'BEEF       // hex with separator
// 0b1010'0101       // binary with separator
// ```

// **The float/double trap with `auto`:**

// ```cpp
// auto x = 3.14;    // x is DOUBLE (8 bytes)  ← default!
// auto x = 3.14f;   // x is FLOAT  (4 bytes)
// auto x = 3.14L;   // x is LONG DOUBLE (16 bytes)

// float pi = 3.14;  // compiles but: double → float conversion (precision loss)
// float pi = 3.14f; // correct: float literal → float variable (no conversion)
// float pi{3.14};   // COMPILE ERROR: narrowing refused by {}
// ```

// > **The key rule to remember**: C++ defaults floating-point literals to `double` because it's the "natural" precision for scientific computation. You **opt into** `float` (less precision, faster/smaller) or `long double` (more precision) with explicit suffixes. Without the `f` suffix, you're silently doing a double-to-float conversion every time.