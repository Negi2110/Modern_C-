// ============================================================
// WHAT CAN GO WRONG WITH C++ CASTING - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <utility>  // for std::cmp_greater (C++20)

// ============================================================
// SECTION 1: THE SIGNED/UNSIGNED COMPARISON BUG
// ============================================================

// THIS IS A CLASSIC C++ BUG THAT TRIPS EVERYONE:
//
// int i = -2;
// unsigned int u = 1;
// if (i > u)  -> prints "huh?" even though -2 < 1 mathematically!
//
// WHY??
//
// COMPARISON RULE:
// When you compare signed and unsigned in C++:
// -> the SIGNED value is IMPLICITLY CONVERTED to UNSIGNED
// -> -2 as unsigned int = ???
//
// -2 in binary (two's complement, 32 bits):
// 1111 1111 1111 1111 1111 1111 1111 1110
//
// These SAME bits as UNSIGNED int:
// = 4,294,967,294  (which is 2^32 - 2)
//
// So comparison becomes:
// 4,294,967,294 > 1 -> TRUE -> "huh?" printed!
// -2 secretly became 4 BILLION when compared to unsigned!
//
// THIS IS UNDEFINED/IMPLEMENTATION-DEFINED BEHAVIOR
// (technically implementation-defined for unsigned conversion,
//  but the result is always this "wrap around" on common platforms)

// ============================================================
// SECTION 2: WHY COMPILER WARNS (YOUR COMMENT)
// ============================================================

// g++ -std=c++20 -Wall -> enables ALL warnings
// -> "comparison of integer expressions of different signedness:
//    'int' and 'unsigned int' [-Wsign-compare]"
//
// WITHOUT -Wall: no warning, silent bug!
// WITH -Wall: compiler catches it
//
// THIS IS WHY you should always compile with warnings enabled:
// g++ -std=c++20 -Wall -Wextra -Wpedantic file.cpp
//
// In Qt projects: add to CMakeLists.txt:
// target_compile_options(MyApp PRIVATE -Wall -Wextra)

// ============================================================
// SECTION 3: THE FIXES
// ============================================================

int main()
{
    // ---- ORIGINAL (safe) EXAMPLE ----
    int result = 6;
    unsigned char c = result;
    std::cout << (int)c << std::endl;  // 6: fits, no issue

    int i = -2;
    unsigned int u = 1;

    // ---- THE BUG ----
    // if (i > u)  // WRONG: -2 converted to 4294967294 -> 4294967294 > 1 = true!
    // {
    //     std::cout << "huh?" << std::endl;  // INCORRECTLY prints!
    // }

    // ---- FIX 1: std::cmp_greater (C++20) - YOUR CODE ----
    if (std::cmp_greater(i, u))
    // std::cmp_greater does MATHEMATICALLY CORRECT signed/unsigned comparison
    // it knows i=-2 and u=1 and correctly says -2 is NOT > 1
    // returns false -> nothing printed -> CORRECT behavior!
    {
        std::cout << "should not print\n";  // correctly skipped
    }

    // ---- FIX 2: cast to larger signed type ----
    // if ((long long)i > (long long)u)
    // cast BOTH to signed long long (can hold all unsigned int values)
    // -2 > 1 -> false -> correct!
    // long long range: -9 quintillion to +9 quintillion
    // can hold unsigned int max (4 billion) without overflow

    // ---- FIX 3: explicit cast to unsigned (if you know i >= 0) ----
    // if ((unsigned int)i > u)  // only safe if i is guaranteed non-negative!
    // -> for i = -2: still wrong! becomes 4294967294
    // -> ONLY safe when you KNOW i won't be negative

    // ---- FIX 4: compare as signed (cast u to int) ----
    // if (i > (int)u)  // cast u to int
    // -2 > 1 -> false -> correct
    // BUT: dangerous if u > INT_MAX (would wrap to negative!)
    // safe only when u is known to fit in int

    return 0;
}

// ============================================================
// SECTION 4: std::cmp_* FAMILY (C++20) - COMPLETE LIST
// ============================================================

// C++20 added safe comparison functions in <utility>:
//
// std::cmp_equal(a, b)         -> a == b (mathematically correct)
// std::cmp_not_equal(a, b)     -> a != b
// std::cmp_less(a, b)          -> a < b
// std::cmp_greater(a, b)       -> a > b  (your code uses this!)
// std::cmp_less_equal(a, b)    -> a <= b
// std::cmp_greater_equal(a, b) -> a >= b
//
// ALL handle signed/unsigned comparison CORRECTLY
// -2 and 1u: std::cmp_greater(-2, 1u) = false (correctly!)
//
// std::in_range<T>(value)  -> checks if value fits in type T
// std::in_range<int>(4294967294u) -> false (doesn't fit in int)
// std::in_range<unsigned>(5) -> true

// ============================================================
// SECTION 5: ALL THE THINGS THAT CAN GO WRONG WITH CASTING
// ============================================================

// PROBLEM 1: SIGNED/UNSIGNED COMPARISON (your main example)
// int i = -2; unsigned u = 1; i > u -> TRUE (wrong!)
// Fix: std::cmp_greater, or cast both to larger signed type

// PROBLEM 2: NARROWING CONVERSION (data loss)
// int x = 500000; short s = x;  // 500000 doesn't fit in short
// s = -15536 (wraps around!) -> SILENT data corruption
// Fix: check range first, use static_assert for compile-time sizes
// int x = 50000; assert(x <= 32767); short s = x;

// PROBLEM 3: FLOAT TO INT TRUNCATION
// float f = 3.9f;
// int i = (int)f;  // i = 3 (truncates, NOT rounds!)
// might expect 4, get 3 -> logic error
// Fix: use (int)std::round(f) if rounding is intended

// PROBLEM 4: OVERFLOW ON UNSIGNED SUBTRACTION
// unsigned int u = 0;
// u - 1;  // = 4294967295! (wraps around, no negative for unsigned)
// Fix: check before subtracting: if (u > 0) u--;

// PROBLEM 5: CHAR PRINTED AS CHARACTER NOT NUMBER
// unsigned char c = 65;
// std::cout << c;      // prints 'A' (might expect 65)
// std::cout << (int)c; // prints 65
// Fix: always cast to int when printing numeric char values

// PROBLEM 6: POINTER CAST TO WRONG TYPE
// int x = 65;
// char* p = (char*)&x;  // reinterpret int bytes as chars
// *p = 100;             // modifies low byte of x!
// -> undefined behavior, platform-dependent results
// Fix: don't cast pointers between unrelated types

// PROBLEM 7: double to float PRECISION LOSS
// double d = 3.141592653589793;
// float f = (float)d;  // f = 3.1415927 (loses digits)
// if (f == d) -> FALSE! comparison after conversion
// Fix: use appropriate precision for calculations

// ============================================================
// SECTION 6: SIGNED/UNSIGNED - THE MEMORY VISUAL
// ============================================================

// 32-bit binary: 1111 1111 1111 1111 1111 1111 1111 1110
//
// INTERPRETED AS signed int (two's complement):
// = -2
// (highest bit = 1 means negative, value = -(2^32 - value) = -2)
//
// INTERPRETED AS unsigned int:
// = 4,294,967,294
// (all bits = magnitude, no sign bit)
//
// SAME BITS, DIFFERENT INTERPRETATION!
// The cast doesn't CHANGE the bits, just changes how they're READ
//
// This is the fundamental danger: when signed is SILENTLY converted
// to unsigned for comparison, the BITS stay the same but the
// MEANING changes completely -> wrong comparison result

// ============================================================
// SECTION 7: WHY THIS MATTERS IN DIS/ISSST WORK
// ============================================================

// DIS protocol uses MANY unsigned types:
// uint8_t, uint16_t, uint32_t for PDU fields
// Entity IDs, site IDs, protocol version = all unsigned
//
// DANGEROUS CODE in ISSST context:
// int16_t signedField = receivedData;  // signed
// uint16_t pduField = header.length;   // unsigned from PDU
// if (signedField > pduField) { ... }  // signed/unsigned bug!
//
// COULD CAUSE:
// -> wrong branch taken in PDU processing
// -> incorrect entity state updates
// -> simulation desync
//
// FIX: always use matching types for DIS fields
// or use std::cmp_* for mixed comparisons
// or explicit casts with range checking

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What happens when you compare signed int -2 with unsigned int 1?
//     -> The signed int is IMPLICITLY converted to unsigned.
//        -2 as unsigned int = 4,294,967,294 (wraps around).
//        4,294,967,294 > 1 = TRUE, even though -2 < 1 mathematically.
//        This is the signed/unsigned comparison bug.

// Q2: How does std::cmp_greater fix this?
//     -> C++20 function from <utility> that performs mathematically
//        correct mixed signed/unsigned comparison. Internally handles
//        the sign difference without implicit conversion.
//        std::cmp_greater(-2, 1u) = false (correct!).

// Q3: How can you detect signed/unsigned comparison bugs?
//     -> Compile with -Wall (enables -Wsign-compare warning).
//        "comparison of integer expressions of different signedness"
//        Static analysis tools (clang-tidy, cppcheck).
//        C++20: use std::cmp_* functions to fix them.

// Q4: What is two's complement?
//     -> The way signed integers are stored in binary.
//        Negative numbers: flip all bits of positive, add 1.
//        -2: flip bits of 2 (00...10) = 11...01, add 1 = 11...10.
//        This means same bit pattern reads as -2 (signed) or
//        4294967294 (unsigned) on 32-bit systems.

// Q5: What other implicit conversions can cause bugs?
//     -> float to int (truncation not rounding)
//        int to short (overflow/wrapping)
//        unsigned subtraction going below 0 (wraps to huge number)
//        char printed as character not number

// Q6: What compiler flag enables signed/unsigned warnings?
//     -> -Wall (enables most warnings including -Wsign-compare)
//        or -Wsign-compare specifically.
//        Also: -Wextra and -Wpedantic for even more warnings.
//        In CMake: target_compile_options(target PRIVATE -Wall -Wextra)

// Q7: Is (unsigned int)(-2) == 4294967294 guaranteed?
//     -> YES for unsigned conversion: the C++ standard guarantees
//        that converting a negative signed int to unsigned wraps around
//        (modulo 2^N). This is well-defined (unlike signed overflow!).

// Q8: What is the safest way to compare int and size_t?
//     -> C++20: std::cmp_less(intVal, sizeVal) etc.
//        Pre-C++20: cast int to ptrdiff_t (signed size type)
//        and compare, checking int >= 0 first.
//        OR: use explicit checks: if (intVal >= 0 && (size_t)intVal < sizeVal)
// ```

// **The signed/unsigned trap - the exact mechanism:**

// ```cpp
// int i = -2;         // bits: 1111...1110
// unsigned int u = 1; // bits: 0000...0001

// Comparison: i > u
// Compiler converts i to unsigned (implicit!)
// -2 as unsigned = 4,294,967,294
// 4,294,967,294 > 1 = TRUE  ← WRONG!

// // Fix (C++20):
// std::cmp_greater(i, u)  // = false  ← CORRECT!
// ```

// **All the things that go wrong:**

// ```
// CAST DANGER              EXAMPLE                    RESULT
// ─────────────────────────────────────────────────────────────────
// signed > unsigned        -2 > 1u                    TRUE (wrong!)
// narrowing               int(500000) → short          -15536
// float truncation        (int)3.9f                    3 (not 4!)
// unsigned underflow       0u - 1                      4294967295
// char as ASCII            cout << char(65)             'A' (not 65)
// ```

// > **The -Wall habit**: always compile with `-Wall -Wextra` during development. The signed/unsigned comparison bug is **silent without warnings** — your code compiles, runs, and produces wrong answers with no indication anything is wrong. `-Wall` turns this silent bug into an immediate compiler warning. In your ISSST CMakeLists.txt, add this to `target_compile_options`.