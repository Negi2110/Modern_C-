// ============================================================
// C++ PRIMITIVE DATA TYPES - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <string>
#include <climits>   // for INT_MAX, LLONG_MAX etc.
#include <cfloat>    // for FLT_DIG, DBL_DIG etc.

// ============================================================
// SECTION 1: FUNDAMENTAL (PRIMITIVE) DATA TYPES IN C++
// ============================================================

// C++ has 7 fundamental types:
// 1. int       - whole numbers
// 2. float     - single precision decimal
// 3. double    - double precision decimal
// 4. char      - single character
// 5. bool      - true / false
// 6. void      - represents absence of type
// 7. wchar_t   - wide character (for unicode)

// ============================================================
// SECTION 2: TYPE SIZES & RANGES (64-bit system)
// ============================================================

// | Type        | Size    | Range / Notes                          |
// |-------------|---------|----------------------------------------|
// | bool        | 1 byte  | true (1) or false (0)                  |
// | char        | 1 byte  | -128 to 127 (signed)                   |
// | int         | 4 bytes | -2,147,483,648 to 2,147,483,647        |
// | long long   | 8 bytes | -9.2 * 10^18 to 9.2 * 10^18           |
// | float       | 4 bytes | ~6-7 significant decimal digits        |
// | double      | 8 bytes | ~15-16 significant decimal digits      |
// | long double | 16bytes | ~18-19 significant decimal digits      |

// ============================================================
// SECTION 3: CODE EXAMPLES WITH EXPLANATIONS
// ============================================================

// --- int64_t x = 423245235423543253; ---
// int64_t = guaranteed 64-bit signed integer (from <cstdint>)
// INTERVIEW: Why use int64_t over long long?
//   -> int64_t is portable. long long is "at least 32 bits"
//      but int64_t is EXACTLY 64 bits on every platform.

// --- bool x = true; ---
// Size: 1 byte (not 1 bit!) - reason: CPU addresses memory in bytes
// INTERVIEW: Why does bool take 1 byte and not 1 bit?
//   -> Memory is byte-addressable, CPU cannot address individual bits.
// INTERVIEW: What does sizeof(bool) return? -> 1

// --- float x = 3.14f; ---
// 'f' suffix is MANDATORY to tell compiler it's a float literal
// Without 'f', the literal 3.14 is treated as double by default
// INTERVIEW: What happens if you write float x = 3.14; (no f)?
//   -> Compiler does an implicit narrowing conversion (double -> float)
//      May cause precision loss warning.

// --- float x = 3.142342135432543254; (print 5 digits) ---
// float only has ~6-7 significant digits of precision
// Use: std::cout << std::fixed << std::setprecision(5) << x;
// INTERVIEW: What is the difference between precision and accuracy?
//   -> Precision = how many digits stored. Accuracy = how close to true value.

// --- double x = 3.142342135432543254; ---
// double has ~15-16 significant digits (more precise than float)
// Default floating point literal type in C++ is double
// INTERVIEW: When to use float vs double?
//   -> Use float when memory/performance matters (GPU, large arrays).
//      Use double for scientific calculations needing high precision.

// --- char x = 'abc'; ---
// WRONG! char holds exactly ONE character.
// 'abc' is a multi-character literal -> implementation-defined behavior
// INTERVIEW: What is the difference between 'a' and "a"?
//   -> 'a' is char (1 byte). "a" is const char* (2 bytes: 'a' + '\0')

// --- const char* x = "abc"; ---
// String literal stored in READ-ONLY memory segment
// const char* = pointer to constant characters
// INTERVIEW: Why is it const char* and not char*?
//   -> String literals are stored in read-only memory.
//      Modifying them causes UNDEFINED BEHAVIOR / crash.
// INTERVIEW: What is the difference between const char* and char* const?
//   -> const char* : pointer to constant data (data can't change)
//      char* const  : constant pointer (address can't change, data can)
//      const char* const : both locked

// ============================================================
// SECTION 4: std::string - NOT a primitive type
// ============================================================

// std::string is a CLASS defined in <string> header
// It is part of STL (Standard Template Library)
// Internally manages a heap-allocated char array

// sizeof(std::string) = 32 bytes (on most 64-bit systems)
// This is the SIZE OF THE OBJECT, not the string content!
// It stores: pointer to data + size + capacity (3 x 8 bytes = 24-32)

// INTERVIEW: What is the difference between std::string and const char*?
//   -> const char* : raw pointer, no bounds checking, manual memory
//      std::string  : safe, dynamic size, has methods (.length(), .find() etc.)

// INTERVIEW: Is std::string null-terminated?
//   -> Internally yes (.c_str() returns null-terminated), but std::string
//      itself tracks length separately, so it CAN store embedded '\0'.

// INTERVIEW: What does sizeof("abc") return?
//   -> 4 (3 chars + 1 null terminator '\0')

// INTERVIEW: What does sizeof(std::string) return?
//   -> Platform-dependent, typically 24 or 32 bytes (NOT the string length!)

// ============================================================
// SECTION 5: TYPE MODIFIERS
// ============================================================

// signed / unsigned : change the range of integer types
// short / long      : change the size

// unsigned int  -> 0 to 4,294,967,295 (double the positive range)
// short int     -> 2 bytes, range: -32768 to 32767
// long long int -> 8 bytes

// INTERVIEW: What happens when unsigned int overflows?
//   -> It WRAPS AROUND to 0 (defined behavior for unsigned)
//      For signed int overflow -> UNDEFINED BEHAVIOR in C++

// ============================================================
// SECTION 6: KEY INTERVIEW QUESTIONS SUMMARY
// ============================================================

// Q1: What are the primitive/fundamental data types in C++?
//     -> bool, char, int, float, double, void, wchar_t

// Q2: Is std::string a primitive type?
//     -> NO. It is a class from STL. Primitive types are built into the language.

// Q3: What is the default type of a floating-point literal like 3.14?
//     -> double. Use 'f' suffix for float: 3.14f

// Q4: What is sizeof() operator? Is it a function?
//     -> It's a COMPILE-TIME operator (not a function).
//        Returns size in bytes. Evaluated at compile time, not runtime.

// Q5: Can sizeof() be applied to a type and a variable both?
//     -> YES. sizeof(int) and sizeof(x) both work.

// Q6: Why does bool take 1 byte and not 1 bit?
//     -> CPU is byte-addressable. Smallest addressable unit is 1 byte.

// Q7: What is the difference between int and int32_t?
//     -> int is "at least 16 bits" (usually 32). int32_t is EXACTLY 32 bits.
//        Use int32_t for portability in system/embedded programming.

// Q8: What is undefined behavior (UB)? Give an example.
//     -> Behavior the C++ standard does not define. Compiler can do anything.
//        Example: signed integer overflow, dereferencing null pointer,
//                 accessing array out of bounds.

// Q9: What is the difference between declaration and definition?
//     -> Declaration: tells compiler the type exists (extern int x;)
//        Definition: allocates memory (int x = 5;)

// Q10: What is a literal? What are different types?
//      -> A fixed value written directly in code.
//         Integer literal: 42, Floating: 3.14, Char: 'a',
//         String: "hello", Bool: true/false

int main() {
    // Your original code - now you understand WHY each line works:

    // int64_t x = 423245235423543253;  // exactly 64-bit, portable
    // bool x = true;                   // 1 byte, not 1 bit
    // float x = 3.14f;                 // 'f' suffix = float literal
    // double x = 3.142342135432543254; // higher precision than float
    // char x = 'a';                    // single char only!
    // const char* x = "abc";           // pointer to read-only memory

    std::string x = "abc"; // NOT a primitive - it's a class
    std::cout << "The value of x is: " << x << std::endl;
    std::cout << sizeof(std::string) << std::endl; // prints 32 (object size, NOT string length!)

    return 0;
}

// Yes, exactly — double IS more precise. That's the whole point of the tradeoff:

// ```cpp
// // ============================================================
// // FLOAT vs DOUBLE - The Real Tradeoff Explained
// // ============================================================

// // You are RIGHT - double is MORE precise. Always.
// // So why would anyone ever use float? -> PERFORMANCE & MEMORY

// // ============================================================
// // PRECISION COMPARISON
// // ============================================================

// // float  -> 4 bytes -> ~6-7  significant digits
// // double -> 8 bytes -> ~15-16 significant digits

// // Example:
// // float  x = 3.142342135432543254f;
// // stored as -> 3.142342  (cuts off after ~7 digits, rest is GARBAGE)

// // double x = 3.142342135432543254;
// // stored as -> 3.142342135432543  (keeps ~15-16 digits, much better)

// // ============================================================
// // SO WHY USE FLOAT AT ALL?
// // ============================================================

// // REASON 1: MEMORY
// // -----------------
// // float  = 4 bytes
// // double = 8 bytes  <- double the memory!
// //
// // Imagine a game with 1 million particles on screen:
// // float  -> 1,000,000 x 4 bytes =  4 MB
// // double -> 1,000,000 x 8 bytes =  8 MB  <- wastes 4MB extra
// //
// // In GPU programming (CUDA, OpenGL, shaders) you have
// // thousands of such arrays -> memory saving becomes HUGE

// // REASON 2: SPEED (CPU/GPU)
// // --------------------------
// // Modern GPUs are optimized for float (32-bit) operations
// // float  math -> runs 2x to 8x FASTER on most GPUs
// // double math -> slower, some GPUs don't even support it natively
// //
// // Game engines (Unity, Unreal) use float by default for this reason

// // REASON 3: CACHE EFFICIENCY
// // ---------------------------
// // Smaller data = more values fit in CPU cache
// // More cache hits = faster program overall
// // float arrays are cache-friendlier than double arrays

// // ============================================================
// // SIMPLE DECISION RULE
// // ============================================================

// // Need HIGH PRECISION?
// //   -> Scientific calc, financial data, physics simulation
// //   -> Use DOUBLE (or long double)

// // Need HIGH PERFORMANCE / LESS MEMORY?
// //   -> Game dev, GPU shaders, ML model weights, graphics
// //   -> Use FLOAT (precision loss is acceptable there)

// // ============================================================
// // INTERVIEW QUESTION THAT TRIPS PEOPLE UP
// // ============================================================

// // Q: "Is double always better than float?"
// //
// // WRONG answer: "Yes, double is better"
// // RIGHT answer:
// //   "double is more PRECISE, but float is faster and uses less memory.
// //    The best choice depends on the use case.
// //    For graphics/GPU -> float.
// //    For scientific/financial calculations -> double."

// // This shows you understand tradeoffs, not just facts.
// // Interviewers LOVE tradeoff-based thinking.
// ```

// **One line to remember it forever:**

// > `double` = more precision → use when **correctness** matters
// > `float` = less memory, faster → use when **performance** matters

// The GPU/game example is the killer answer in interviews because it shows real-world awareness, not just textbook knowledge.
// Great catch — your thinking is 100% logical. Let me clear this up:

// ```cpp
// // ============================================================
// // WHY does double -> float cause precision loss WARNING?
// // ============================================================

// // Your logic: "double is more precise, so storing double into
// // float should be fine... right?"
// //
// // WRONG - and here is exactly WHY:

// // ============================================================
// // THE REAL PROBLEM - It's a NARROWING CONVERSION
// // ============================================================

// // double has 15-16 digits of precision  (8 bytes)
// // float  has  6-7 digits of precision   (4 bytes)
// //
// // When you write:
// //    float x = 3.14;
// //
// // What actually happens step by step:
// //
// // STEP 1: Compiler sees "3.14" -> creates a DOUBLE in memory
// //         double value = 3.14000000000000012434...  (15 digits stored)
// //
// // STEP 2: Now tries to SQUEEZE that double into a float variable
// //         float can only hold 6-7 digits
// //         So it CUTS OFF the remaining digits
// //
// // STEP 3: float x = 3.14000  (rest is LOST forever)
// //
// // Data went from BIGGER bucket -> SMALLER bucket
// // That lost data = "precision loss"
// // Compiler warns you: "hey, you are losing data here!"

// // ============================================================
// // ANALOGY TO MAKE IT CRYSTAL CLEAR
// // ============================================================

// // Think of it like this:
// //
// // double = a JUG that holds 1 litre
// // float  = a GLASS that holds 500ml
// //
// // float x = 3.14;
// // means -> fill the JUG fully (double literal)
// //       -> now pour it into the GLASS (float variable)
// //       -> 500ml SPILLS and is LOST
// //
// // Compiler sees the spill and warns you!
// //
// // The warning is NOT about double being less precise.
// // The warning is about LOSING the extra precision during conversion.

// // ============================================================
// // CONTRAST: The OTHER direction has NO warning
// // ============================================================

// // double x = 3.14f;   -> NO WARNING
// //
// // Why? Because:
// // float  = GLASS  (500ml)
// // double = JUG    (1 litre)
// //
// // Pouring GLASS into JUG -> nothing spills, no data lost
// // Compiler is happy, no warning.
// //
// // This is called WIDENING CONVERSION -> always safe
// // float -> double = widening  -> safe, no warning
// // double -> float = narrowing -> unsafe, WARNING!

// // ============================================================
// // INTERVIEW QUESTION
// // ============================================================

// // Q: What is implicit narrowing conversion? Give an example.
// //
// // A: When a larger data type is automatically converted to a
// //    smaller one, potentially losing data.
// //    Example: float x = 3.14; (double literal -> float variable)
// //    Compiler may warn because digits are silently dropped.

// // Q: What is the fix?
// //    Option 1 -> Add 'f' suffix:   float x = 3.14f;
// //                now literal is already float, no conversion needed
// //
// //    Option 2 -> Explicit cast:    float x = (float)3.14;
// //                you are TELLING the compiler "yes I know, do it anyway"
// //                warning goes away because you made it intentional
// ```

// **The key insight in one line:**

// > The warning is not about double being imprecise — it's about **data being lost** when a bigger type is forced into a smaller one.

// Your confusion was actually very smart thinking — you just needed to see that the **direction of conversion** is what matters, not which type is "better."