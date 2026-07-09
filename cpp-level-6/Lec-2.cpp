// ============================================================
// UNIONS - COMPLETE NOTES - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS A UNION?
// ============================================================

// union = a special type where ALL members SHARE the same memory location
//
// struct: each member has its OWN separate memory
// struct S { int i; short s; float f; };
// sizeof(S) = 4 + 2 + 2(pad) + 4 = 12 bytes
// [iiii][ss__][ffff]  <- separate storage for each
//
// union: ALL members OVERLAP at the SAME address
// union U { int i; short s; float f; };
// sizeof(U) = max(4, 2, 4) = 4 bytes (size of LARGEST member)
// [iiii]  <- all members share THIS memory
// [ss]    <- s occupies first 2 bytes of same memory
// [ffff]  <- f occupies all 4 bytes of same memory
//
// ONLY ONE member can hold a VALID value at any time!
// writing to i makes s and f UNDEFINED to read
// writing to s makes i and f UNDEFINED to read

// ============================================================
// SECTION 2: YOUR UNION
// ============================================================

union U
{
    int   i;   // 4 bytes (alignment: 4)
    short s;   // 2 bytes (alignment: 2)
    // float f; // 4 bytes (alignment: 4)
    //
    // sizeof(U) = max(sizeof(int), sizeof(short)) = max(4, 2) = 4 bytes
    // alignment of U = alignment of largest alignment member = 4
    //
    // MEMORY LAYOUT (all at address 0):
    // Byte: 0    1    2    3
    //       [         i         ]  <- int uses all 4 bytes
    //       [    s    ]            <- short uses first 2 bytes only
    //
    // myUnion.i = 2003 (0x000007D3 in hex):
    // Byte 0: 0xD3
    // Byte 1: 0x07
    // Byte 2: 0x00
    // Byte 3: 0x00
    //
    // myUnion.s after setting i = 2003:
    // reads bytes 0-1: 0xD3, 0x07 = 2003 (same value! fits in short)
    // but if i = 100000 (too big for short!):
    // i = 100000 = 0x000186A0
    // s reads only bytes 0-1: 0x86A0 = -31072 (garbage for short context!)

    // YOUR COMMENT about methods:
    // void printi() { std::cout << i << std::endl; }
    //
    // C++ UNIONS CAN HAVE:
    // -> member functions (methods)  <- C++ feature (not in C)
    // -> constructors / destructors  <- C++ feature
    // -> access specifiers (public/private) <- C++ feature
    //
    // C UNIONS CANNOT HAVE:
    // -> methods
    // -> constructors
    // -> access specifiers
    // -> member types with constructors (non-trivial types)
    //
    // THIS IS THE DIFFERENCE BETWEEN C AND C++ UNIONS
    // your comment: "it differs in C++ and C"
};

// ============================================================
// SECTION 3: YOUR QUESTION - "WHEN DO WE ACTUALLY USE THEM?"
// ============================================================

// USE CASE 1: MEMORY-EFFICIENT TYPE VARIANT (most common)
// When you need to store ONE OF several types but not all simultaneously:
//
// union Variant {
//     int    i;
//     float  f;
//     double d;
// };
// // only using ONE at a time -> no wasted memory
// // without union: struct would use 4+4+8 = 16 bytes
// // with union: only max(4,4,8) = 8 bytes!

// USE CASE 2: TYPE PUNNING (inspecting bytes of a value)
// See the raw bytes of a float:
// union FloatInspector {
//     float f;
//     unsigned char bytes[4];
//     unsigned int bits;
// };
// FloatInspector fi;
// fi.f = 3.14f;
// // now inspect fi.bytes[0..3] to see IEEE 754 representation
// // fi.bits to see as unsigned int
// // (technically undefined behavior in C++ but widely used in practice)

// USE CASE 3: NETWORK PROTOCOL / HARDWARE REGISTERS
// union Register {
//     uint32_t raw;        // full 32-bit register
//     struct {
//         uint8_t low;     // low byte
//         uint8_t high;    // high byte
//         uint16_t upper;  // upper 16 bits
//     };
// };
// DIS protocol PDUs, hardware registers in embedded = union territory
// YOUR ISSST/DIS work: PDU headers often use unions for this!

// USE CASE 4: TAGGED UNION / DISCRIMINATED UNION
// Store different types with a "tag" saying which is active:
// enum class Type { INT, FLOAT, STRING };
// struct Value {
//     Type tag;        // which member is active
//     union {
//         int    i;
//         float  f;
//         char   s[32];
//     };
// };
// This is essentially what std::variant<int,float,string> does internally!

// USE CASE 5: ANONYMOUS UNIONS (common in graphics/math)
// union Vec3 {
//     struct { float x, y, z; };  // named access
//     float data[3];               // array access
// };
// Vec3 v;
// v.x = 1.0f; v.y = 2.0f; v.z = 3.0f;
// v.data[0] == v.x  // true! same memory
// Used heavily in: GLM, DirectXMath, Eigen etc.

// ============================================================
// SECTION 4: ANONYMOUS UNION IN C++ - SYNTACTIC SUGAR
// ============================================================

// NAMED union (your code):
// union U { int i; short s; };
// U myUnion;
// myUnion.i = 5;  // must use myUnion.member

// ANONYMOUS union (no name):
// union { int i; short s; };
// i = 5;  // access DIRECTLY without union name!
// s       // also directly accessible
//
// Anonymous unions commonly used INSIDE a struct:
// struct Pixel {
//     union {
//         uint32_t rgba;      // as one 32-bit value
//         struct {
//             uint8_t r, g, b, a;  // as 4 separate bytes
//         };
//     };
// };
// Pixel p;
// p.r = 255; p.g = 0; p.b = 0; p.a = 255;  // set red
// std::cout << p.rgba;  // see as combined 32-bit value

// ============================================================
// SECTION 5: std::variant - MODERN C++ REPLACEMENT (C++17)
// ============================================================

// C++17 introduced std::variant as TYPE-SAFE alternative to union:
//
// std::variant<int, float, std::string> v;
// v = 42;           // stores int
// v = 3.14f;        // stores float (replaces int)
// v = "hello";      // stores string (replaces float)
//
// std::get<int>(v)  // safely get int (throws if not int)
// std::holds_alternative<int>(v)  // check which type is stored
//
// UNION vs std::variant:
// union:     no type safety (undefined behavior if wrong member read)
//            no destructor called for complex types
//            faster, no overhead
// variant:   type-safe (exception on wrong access)
//            correctly destructs held type
//            small overhead (stores type tag internally)
//
// PREFER std::variant in modern C++ for safety
// Use union for: performance-critical, hardware, type-punning, C interop

// ============================================================
// SECTION 6: DANGERS OF UNION
// ============================================================

// DANGER 1: Reading wrong member = UNDEFINED BEHAVIOR
// U u;
// u.i = 100000;
// std::cout << u.s;  // UB! s was not set, reads garbage bytes
//                     // (some compilers define this behavior,
//                      C standard allows "type punning" via union
//                      C++ standard is stricter - technically UB)

// DANGER 2: Complex types in unions (pre-C++11 restriction)
// union Bad {
//     std::string s;  // has constructor/destructor!
//     int i;
// };
// Bad b;
// b.s = "hello";      // constructs std::string
// b.i = 5;            // DOESN'T destruct the string! -> resource leak
// // C++11 allows this but YOU must manually construct/destruct:
// new(&b.s) std::string("hello");    // placement new
// b.s.~string();                      // explicit destructor call
// -> std::variant handles this automatically -> use variant!

// DANGER 3: Platform-dependent byte order (endianness)
// union FloatBytes { float f; uint8_t b[4]; };
// FloatBytes fb;
// fb.f = 3.14f;
// fb.b[0]  // might be MSB or LSB depending on CPU architecture!
// x86 = little-endian, ARM = usually little-endian
// network protocols = big-endian (must consider in DIS/ISSST!)

// ============================================================
// SECTION 7: main() TRACE
// ============================================================

int main()
{
    U myUnion;
    myUnion.i = 2003;
    // writes 2003 to the 4-byte storage
    // both i and s "contain" this data
    // but only i is guaranteed valid to read
    //
    // myUnion.i = 2003 -> i is ACTIVE member
    // myUnion.s        -> reading inactive member = technically UB
    //                     in practice: reads first 2 bytes
    //                     2003 fits in short (max 32767)
    //                     so would read 2003 on little-endian

    // std::cout << myUnion.i << std::endl;  // 2003 (valid)
    // std::cout << myUnion.s << std::endl;  // 2003 (works on little-endian)
    //                                          but technically UB in C++

    // sizeof output (if uncommented):
    // sizeof(int)   = 4
    // sizeof(short) = 2
    // sizeof(float) = 4
    // sizeof(myUnion) = 4  (= max(4,2) = 4)

    return 0;
}

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a union and how does it differ from a struct?
//     -> Union: all members share the SAME memory location.
//        Size = size of LARGEST member. Only ONE member valid at a time.
//        Struct: each member has SEPARATE memory. Size = sum + padding.

// Q2: What is sizeof(union U {int i; short s; float f;})?
//     -> max(sizeof(int), sizeof(short), sizeof(float)) = max(4,2,4) = 4 bytes.
//        Size = largest member's size (plus any alignment padding).

// Q3: What is the difference between C unions and C++ unions?
//     -> C++ unions can have: member functions, constructors, destructors,
//        access specifiers (public/private). C unions cannot.
//        C++ unions can contain types with constructors (with manual
//        construction/destruction). C unions: trivial types only.

// Q4: When would you use a union?
//     -> Type punning (inspect bytes of a value), memory efficiency
//        (store one of several types), hardware register access,
//        network protocol PDU fields, anonymous union for math vectors.
//        Modern C++: prefer std::variant for type safety.

// Q5: Is reading a different union member than you wrote undefined behavior?
//     -> In C++: technically yes (UB). In C: allowed for type punning.
//        In practice: most compilers on common platforms produce expected
//        results. For guaranteed behavior: use memcpy or std::bit_cast (C++20).

// Q6: What is an anonymous union?
//     -> A union without a name. Members accessed directly without
//        union name prefix. Often used inside structs for overlapping
//        access (rgba as uint32 or as r,g,b,a bytes separately).

// Q7: What is std::variant and how does it relate to union?
//     -> Type-safe tagged union (C++17). Stores one of several types,
//        tracks which type is active, throws on wrong access, correctly
//        destructs held type. Prefer over raw union in modern C++.

// Q8: How is union used in DIS/network protocols?
//     -> PDU headers often have fields that can be interpreted different
//        ways. Union lets you read raw bytes AND structured fields from
//        same memory. Also used for big-endian/little-endian conversion
//        by writing raw bytes and reading as integer (or vice versa).
// ```

// **union vs struct - the one visual that explains everything:**

// ```
// struct S { int i; short s; float f; };    union U { int i; short s; float f; };

// Byte: 0  1  2  3  4  5  6  7  8  9  10  11   Byte: 0  1  2  3
//      [i  i  i  i][s  s  _  _][f  f  f  f ]        [i  i  i  i]
//                                                      [s  s      ]  ← overlaps!
// sizeof = 12 bytes                                    [f  f  f  f]  ← overlaps!

//                                                 sizeof = 4 bytes (max member)
// ```

// **When to use each:**

// ```
// union    → hardware, DIS PDUs, type punning, anonymous math vectors, C interop
// variant  → type-safe "one of these types" in modern C++ code
// struct   → multiple fields all valid simultaneously (normal use case)
// ```

// > **The ISSST/DIS connection**: DIS PDU headers have fields where the same bits mean different things depending on protocol version or PDU type. Unions let you define **overlapping interpretations of the same bytes** — reading the raw `uint32_t` for quick parsing OR accessing individual fields by name. This is exactly the pattern used in embedded systems and network protocol code.