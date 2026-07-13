// ============================================================
// C++ CASTING - TYPES, CONVERSIONS - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <iomanip>

// ============================================================
// SECTION 1: WHAT IS A CAST / CONVERSION?
// ============================================================

// Cast = telling the compiler "treat this value as a DIFFERENT TYPE"
//
// TWO TYPES OF CONVERSION:
//
// IMPLICIT (automatic, compiler does it):
// int x = 5;
// float y = x;   // compiler automatically converts int to float
//                // no data loss (float can hold all int values)
//                // called WIDENING / PROMOTION (safe)
//
// EXPLICIT (you tell compiler to do it):
// float x = 3.14f;
// int y = (int)x;  // you explicitly say "treat as int"
//                   // truncates to 3 (loses .14)
//                   // called NARROWING (potentially unsafe)

// ============================================================
// SECTION 2: YOUR COMMENTED CODE EXPLAINED
// ============================================================

// EXAMPLE 1: INTEGER DIVISION TRUNCATION
// std::cout << 7 / 5;          // prints 1 (integer division, no decimal)
// std::cout << (float)7 / 5;   // prints 1.4 (float division)
// -> (float)7 converts 7 to 7.0f
// -> 7.0f / 5 -> 5 promoted to float automatically -> 1.4f
// -> YOUR COMMENT: "5 will be promoted to float so we have equivalents"
// -> this is IMPLICIT CONVERSION of 5 triggered by the float on left

// EXAMPLE 2: int to short (data loss)
// int result = 500000;
// short c = result;             // implicit narrowing - WARNING!
// std::cout << (short)result;  // explicit C-style cast
//
// int   = 4 bytes = max 2,147,483,647
// short = 2 bytes = max 32,767
// 500000 > 32,767 -> DATA LOSS, wraps around -> garbage value
// YOUR COMMENT: "int has 4 bytes, short has 2 bytes, we can lose data and sign"

// ============================================================
// SECTION 3: YOUR ACTUAL CODE - int to unsigned char
// ============================================================

int main()
{
    int result = 65;
    // ASCII value 65 = 'A'

    unsigned char c = result;
    // IMPLICIT conversion: int(65) -> unsigned char
    // int    = 4 bytes = value 65 = 0x00000041
    // unsigned char = 1 byte = takes LOWEST byte = 0x41 = 65
    // 65 fits in unsigned char (range 0-255) -> no data loss here!
    // c = 65

    std::cout << (int)c << std::endl;
    // WITHOUT cast: cout << c would print 'A'! (char interpreted as character)
    // WITH (int)c: cout << 65 (numeric value printed)
    // YOUR COMMENT: "we did not say anything" = without cast, 'A' printed
    // WITH (int) cast: forces cout to treat c as integer, prints 65
    //
    // prints: 65

    // WHY does cout print 'A' for char without cast?
    // cout has OVERLOADED << for different types:
    // cout << (char)65   -> uses char overload -> prints 'A'
    // cout << (int)65    -> uses int overload  -> prints 65
    // same VALUE, different TYPE = different output!
    // This is why you need (int) to force numeric display

    return 0;
}

// ============================================================
// SECTION 4: SIGNED vs UNSIGNED - YOUR QUESTION
// ============================================================

// SIGNED (default for int, short, char):
// -> can hold NEGATIVE and POSITIVE values
// -> one bit used for SIGN (positive/negative)
// -> int:   -2,147,483,648 to +2,147,483,647  (4 bytes)
// -> short: -32,768        to +32,767          (2 bytes)
// -> char:  -128           to +127             (1 byte)
//
// UNSIGNED:
// -> ONLY positive values (0 and above)
// -> NO sign bit -> all bits used for magnitude
// -> double the positive range!
// -> unsigned int:   0 to 4,294,967,295        (4 bytes)
// -> unsigned short: 0 to 65,535               (2 bytes)
// -> unsigned char:  0 to 255                  (1 byte)
//
// EXAMPLE - why unsigned matters:
// int result = 50000;
// short c = result;          // 50000 > 32767 -> OVERFLOW -> garbage (-15536)
// unsigned short d = result; // 50000 < 65535 -> FITS -> 50000 correct!
//
// YOUR COMMENT: "unsigned short will work for result = 50000"
// BECAUSE: 50000 fits in unsigned short (0-65535) but NOT in short (-32768 to 32767)

// ============================================================
// SECTION 5: FOUR C++ CAST OPERATORS
// ============================================================

// C has ONE cast style: (type)value  <- C-style cast (your code uses this)
// C++ has FOUR specific cast operators (safer, more expressive):

// 1. static_cast<T>(value)
// -> most common, safe compile-time cast
// -> replaces most C-style casts
// -> checked by compiler (won't cast unrelated types)
//
// float f = static_cast<float>(7) / 5;  // int -> float
// int i = static_cast<int>(3.14f);      // float -> int (truncation, but explicit)
// (int)c  equivalent to  static_cast<int>(c)

// 2. dynamic_cast<T>(ptr)
// -> for POLYMORPHISM (inheritance + virtual functions)
// -> safely casts base pointer to derived pointer
// -> returns nullptr if cast invalid (at runtime!)
// -> requires RTTI (Run-Time Type Information)
//
// Base* b = new Derived();
// Derived* d = dynamic_cast<Derived*>(b);  // safe downcast
// if (d) { d->derivedMethod(); }           // nullptr check

// 3. const_cast<T>(value)
// -> adds or REMOVES const qualifier
// -> use when you must pass to legacy C API that lacks const
// -> almost always a design smell
//
// const int x = 5;
// int* p = const_cast<int*>(&x);  // removes const (dangerous!)
// // modifying *p is undefined behavior if x was originally const!

// 4. reinterpret_cast<T>(value)
// -> DANGEROUS low-level bit reinterpretation
// -> no conversion, just "pretend these bits are this type"
// -> use for: pointer to integer, hardware registers, type punning
//
// int x = 65;
// char* p = reinterpret_cast<char*>(&x);  // treat int bytes as chars
// // p[0] = low byte of x (implementation defined)

// ============================================================
// SECTION 6: C-STYLE CAST vs C++ CAST
// ============================================================

// C-STYLE: (type)value
// (int)c          -> can mean static_cast, reinterpret_cast, or const_cast
//                    compiler PICKS one silently
//                    dangerous: you might get reinterpret_cast by accident!
//
// C++ STYLE: static_cast<type>(value)
// static_cast<int>(c)  -> EXACTLY a static_cast, nothing else
//                         compiler verifies it's appropriate
//                         intent is EXPLICIT
//
// PREFER C++ style in modern code:
// -> more explicit (reader knows exactly what kind of cast)
// -> compiler can reject inappropriate casts
// -> easier to search in code (find "reinterpret_cast" vs finding all "(type)")
//
// YOUR CODE uses C-style (int)c -> fine for simple numeric conversions
// but in production Qt/C++ code: prefer static_cast<int>(c)

// ============================================================
// SECTION 7: ASCII AND char - WHY 65 = 'A'
// ============================================================

// ASCII (American Standard Code for Information Interchange):
// maps integers 0-127 to characters
//
// 65 = 'A'    66 = 'B'  ...  90 = 'Z'
// 97 = 'a'    98 = 'b'  ... 122 = 'z'
// 48 = '0'    49 = '1'  ...  57 = '9'
// 32 = ' ' (space)
//
// char in C++ = small integer that happens to be printed as character
// char c = 65;  // stored as number 65
// cout << c;    // prints 'A' (cout interprets char as character)
// cout << (int)c; // prints 65 (cout interprets as number)
//
// THIS IS WHY you saw (void*)&a in the pointer session!
// cout << &c -> treats char* as C-string -> tries to print "A..." until '\0'
// cout << (void*)&c -> treats as void* -> prints the address

// ============================================================
// SECTION 8: OVERFLOW AND WRAPPING EXPLAINED
// ============================================================

// int result = 500000;
// short c = result;
//
// 500000 in binary (32 bits):
// 0000 0000 0000 0111 1010 0001 0010 0000
//                    ^^^^^^^^^^^^^^^^^^^ <- these 16 bits go into short
//           ^^^^^^^^^^^^                 <- these 16 bits LOST!
//
// short only takes LOWEST 16 bits:
// 1010 0001 0010 0000 = -24288 (in signed short, high bit = sign!)
// -> DATA LOSS and SIGN CHANGE!
//
// FOR 65:
// 65 in binary (32 bits): 0000 0000 0000 0000 0000 0000 0100 0001
// unsigned char takes LOWEST 8 bits: 0100 0001 = 65
// 65 < 255 -> FITS -> NO data loss!

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between implicit and explicit conversion?
//     -> Implicit: compiler does it automatically (widening/promotion).
//        int to float, char to int. Safe, no data loss.
//        Explicit: programmer requests it (cast). May lose data.
//        (int)3.14f, static_cast<float>(7).

// Q2: Why does (float)7 / 5 give 1.4 but 7 / 5 gives 1?
//     -> 7/5 with both ints = integer division = 1 (truncates).
//        (float)7 converts 7 to 7.0f, then 7.0f/5: int 5 is
//        implicitly promoted to float, result is 1.4f.

// Q3: What are the four C++ cast operators?
//     -> static_cast:       safe compile-time numeric/related type cast
//        dynamic_cast:      safe runtime polymorphic downcast
//        const_cast:        add/remove const qualifier
//        reinterpret_cast:  dangerous low-level bit reinterpretation

// Q4: Why prefer static_cast over C-style cast?
//     -> C-style can mean any of the four C++ casts silently.
//        static_cast is explicit about intent, compiler verifies
//        appropriateness, easier to find in code review/search.

// Q5: What is the difference between signed and unsigned?
//     -> Signed: uses one bit for sign, range includes negatives.
//        int: -2B to +2B, char: -128 to +127.
//        Unsigned: all bits for magnitude, 0 to positive max.
//        unsigned int: 0 to 4B, unsigned char: 0 to 255.

// Q6: Why does cout print 'A' for char(65) but 65 for int(65)?
//     -> cout's << operator is overloaded for different types.
//        char overload: interprets as ASCII character -> 'A'.
//        int overload:  interprets as number -> 65.
//        Cast to int forces the int overload.

// Q7: What happens when 500000 is assigned to short?
//     -> Overflow/narrowing. short max is 32767. 500000 > 32767.
//        Only lowest 16 bits of 500000 are kept.
//        Result is implementation-defined, typically wraps around
//        to a negative value (-15536 or similar). Data loss!

// Q8: When is unsigned better than signed?
//     -> When value is NEVER negative (sizes, counts, indices).
//        Double the positive range (50000 fits in unsigned short,
//        not in signed short). Size_t, uint32_t for protocol fields.
//        In DIS PDUs: many fields are unsigned (exercise ID, entity ID).
// ```

// **The key cast comparison:**

// ```cpp
// // C-STYLE (avoid in modern C++):
// (int)c           // what KIND of cast? unclear, compiler picks

// // C++ STYLE (prefer):
// static_cast<int>(c)      // safe numeric conversion, explicit
// dynamic_cast<Derived*>(b) // safe polymorphic downcast
// const_cast<int*>(&x)     // removing const (use carefully)
// reinterpret_cast<char*>(&x) // raw bit reinterpretation (dangerous)
// ```

// **The char print trap:**

// ```cpp
// unsigned char c = 65;
// std::cout << c;          // prints 'A'  ← char overload, ASCII!
// std::cout << (int)c;     // prints 65   ← int overload, numeric!
// // Same value, DIFFERENT TYPE = different cout behavior
// ```

// > **The ISSST/DIS connection**: DIS protocol PDU fields have specific widths — `uint8_t`, `uint16_t`, `uint32_t`. Understanding signed vs unsigned and casting is **critical** when packing/unpacking PDU fields. A signed-to-unsigned mismatch in a PDU header can corrupt the entire packet and break simulation communication.