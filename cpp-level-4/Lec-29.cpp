// ============================================================
// DATA LAYOUT, ALIGNMENT, PADDING - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <string>

// ============================================================
// SECTION 1: WHAT IS MEMORY ALIGNMENT?
// ============================================================

// ALIGNMENT = CPUs require certain data types to be stored at
// memory addresses that are MULTIPLES of their size
//
// WHY? CPU fetches data in fixed-size chunks (words)
// If data is misaligned -> CPU needs TWO fetches instead of one
// -> performance penalty
// -> some architectures CRASH on misaligned access
//
// ALIGNMENT RULES (64-bit system):
// bool   = 1 byte  -> can be at ANY address (multiple of 1)
// short  = 2 bytes -> must be at address that is multiple of 2
// int    = 4 bytes -> must be at address that is multiple of 4
// float  = 4 bytes -> must be at address that is multiple of 4
// double = 8 bytes -> must be at address that is multiple of 8
// pointer= 8 bytes -> must be at address that is multiple of 8
//
// PADDING = bytes the compiler INSERTS between members to
//           satisfy alignment requirements
// -> you don't write them, compiler adds them silently
// -> wasted space, but required for correctness/performance

// ============================================================
// SECTION 2: YOUR ORIGINAL LAYOUT - 12 BYTES (BAD ORDER)
// ============================================================

struct Gamestate_Original    // YOUR CODE - members in this order:
{
    bool checkapoint;        // 1 byte  (alignment: 1)
    short numberOfaplayers;  // 2 bytes (alignment: 2)
    float score;             // 4 bytes (alignment: 4)
};
// MEMORY LAYOUT:
//
// Address: 0    1    2    3    4    5    6    7    8    9   10   11
//          [bool][PAD][short short][PAD PAD][float float float float]
//           ^     ^    ^               ^    ^
//           |     |    |               |    |
//         check 1byte  numberOfPlayers 2pad  score
//         1byte padding for short     padding for float
//         (short needs addr%2==0)     (float needs addr%2==0)
//
// WAIT - let me re-read your actual struct order:
// YOUR ACTUAL ORDER: float, short, bool
// Let me redo:

// ============================================================
// SECTION 3: YOUR ACTUAL CODE LAYOUT
// ============================================================

struct Gamestate
{
    float score;             // 4 bytes (alignment: 4) -> address 0-3
    short numberOfaplayers;  // 2 bytes (alignment: 2) -> address 4-5
    bool checkapoint;        // 1 byte  (alignment: 1) -> address 6
};
// MEMORY LAYOUT (float first):
//
// Address: 0    1    2    3    4    5    6    7
//         [float float float float][short short][bool][PAD]
//          ^                        ^             ^     ^
//          score (4 bytes)          nPlayers     check  1 byte padding
//                                   (2 bytes)    (1 byte) to round to 8
//
// TOTAL = 4 + 2 + 1 + 1(padding) = 8 bytes
//
// WHY padding at end?
// -> struct size must be multiple of its LARGEST member's alignment
// -> largest alignment = float = 4 bytes
// -> current size without padding = 7 bytes
// -> 7 is NOT multiple of 4... wait let me recalculate:
//    4+2+1 = 7, round up to multiple of 4 = 8
//    1 byte of tail padding added
// -> sizeof(Gamestate) = 8

// ============================================================
// SECTION 4: YOUR COMMENT - IF double score INSTEAD OF float
// ============================================================

// YOUR COMMENT: "what if we do double score now whole will become 16"
//
// struct Gamestate_double {
//     double score;            // 8 bytes (alignment: 8)
//     short numberOfaplayers;  // 2 bytes
//     bool checkapoint;        // 1 byte
// };
//
// MEMORY LAYOUT:
// Address: 0-7:    [double double double double double double double double]
// Address: 8-9:    [short short]
// Address: 10:     [bool]
// Address: 11-15:  [PAD PAD PAD PAD PAD]  <- 5 bytes padding!
//
// WHY 5 bytes padding at end?
// -> largest member = double = 8 byte alignment
// -> struct size must be multiple of 8
// -> 8 + 2 + 1 = 11 bytes
// -> next multiple of 8 = 16
// -> 5 bytes of tail padding added
// -> sizeof = 16 bytes!
//
// YOUR COMMENT: "max data size becomes the block"
// CORRECT! the struct's alignment = alignment of its LARGEST member
// double forces 8-byte alignment -> struct must be multiple of 8 -> 16

// ============================================================
// SECTION 5: WHY THE TWO LAYOUTS DIFFER (your question)
// ============================================================

// BAD ORDER (your original comment shows this was the old order):
struct Gamestate_BadOrder
{
    bool checkapoint;        // 1 byte  -> address 0
                             // [PAD PAD PAD] -> 3 bytes padding!
                             // (float at address 4 needs alignment 4)
    float score;             // 4 bytes -> address 4-7
    short numberOfaplayers;  // 2 bytes -> address 8-9
                             // [PAD PAD] -> 2 bytes tail padding
                             // (struct size must be multiple of 4)
};
// TOTAL = 1 + 3(pad) + 4 + 2 + 2(pad) = 12 bytes!
//
// GOOD ORDER (your actual struct):
// float, short, bool -> 8 bytes (as shown in section 3)
//
// SAME DATA, DIFFERENT ORDER, DIFFERENT SIZE:
// Bad order:  12 bytes
// Good order:  8 bytes
// SAVINGS: 4 bytes per struct!
// -> 1000 Gamestate objects: 4000 bytes saved just by reordering!

// ============================================================
// SECTION 6: THE GOLDEN RULE FOR OPTIMAL LAYOUT
// ============================================================

// RULE: arrange members from LARGEST to SMALLEST alignment
//       (biggest first, smallest last)
//
// WHY? biggest members set the alignment requirement
//      placing them first means less padding needed
//
// OPTIMAL ORDER for common types:
// 1. double / pointer (8 bytes)
// 2. int / float      (4 bytes)
// 3. short            (2 bytes)
// 4. char / bool      (1 byte)
//
// EXAMPLE: worst vs best layout:
//
// WORST:
// struct Bad {
//     bool a;    // 1 + 3 pad = 4
//     int b;     // 4
//     bool c;    // 1 + 3 pad = 4
//     int d;     // 4
// };             // total = 16 bytes!
//
// BEST:
// struct Good {
//     int b;     // 4
//     int d;     // 4
//     bool a;    // 1
//     bool c;    // 1 + 2 pad = 4
// };             // total = 12 bytes!
//
// Even better:
// struct Best {
//     int b;     // 4
//     int d;     // 4
//     bool a;    // 1
//     bool c;    // 1
//                // 2 pad at end
// };             // total = 12 bytes (same, but cleaner grouping)

// ============================================================
// SECTION 7: main() TRACE
// ============================================================

int main()
{
    Gamestate gs;

    std::cout << sizeof(gs.checkapoint)      << std::endl;  // 1
    std::cout << sizeof(gs.numberOfaplayers) << std::endl;  // 2
    std::cout << sizeof(gs.score)            << std::endl;  // 4
    std::cout << sizeof(gs)                  << std::endl;  // 8 (not 7!)
    // sizeof(gs) = 8, NOT 4+2+1=7
    // compiler added 1 byte tail padding to make it multiple of 4

    return 0;
}

// ============================================================
// SECTION 8: YOUR LOOP COMMENT - WHY LAYOUT MATTERS
// ============================================================

// for (int i = 0; i < 1000; i++) {
//     Gamestate* gs = new Gamestate;
// }
//
// 1000 Gamestate objects:
// Bad layout  (12 bytes): 12,000 bytes used
// Good layout  (8 bytes):  8,000 bytes used  <- 4000 bytes saved!
//
// At scale (game with 100,000 entities):
// Bad:  1,200,000 bytes = 1.2 MB
// Good:   800,000 bytes = 0.8 MB  <- 400KB saved!
//
// For cache performance: smaller struct = more fit in CPU cache
// -> fewer cache misses -> better performance
// -> THIS is why data layout matters in game dev / embedded / DRDO

// ============================================================
// SECTION 9: alignof AND alignas - ADVANCED (good to know)
// ============================================================

// alignof(T) -> returns alignment requirement of type T
// std::cout << alignof(float) << std::endl;   // 4
// std::cout << alignof(double) << std::endl;  // 8
// std::cout << alignof(bool) << std::endl;    // 1
//
// alignas(N) -> force a member or type to have alignment N
// struct alignas(16) SimdVector {
//     float x, y, z, w;
// };
// -> ensures the struct starts at 16-byte boundary
// -> required for SIMD (SSE/AVX) operations in game/graphics engines
//
// #pragma pack(1) -> force NO padding (dangerous but sometimes needed
//                    for binary file formats, network protocols)
// #pragma pack(1)
// struct NetworkPacket { bool flag; int data; }; // 5 bytes, no padding
// #pragma pack()  // restore default

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is memory alignment and why does it exist?
//     -> CPU requires data to be stored at addresses that are multiples
//        of the data's size. Misaligned access requires multiple CPU
//        reads or causes hardware exceptions. Compiler adds padding
//        to enforce alignment automatically.

// Q2: What is padding in a struct?
//     -> Extra bytes inserted by the compiler between or after members
//        to satisfy alignment requirements. Invisible to programmer,
//        but increases sizeof the struct.

// Q3: Why is the Gamestate struct 8 bytes and not 7?
//     -> float=4, short=2, bool=1 = 7 bytes of actual data.
//        But struct size must be a multiple of its largest alignment
//        (float=4). Next multiple of 4 after 7 is 8.
//        1 byte of tail padding added -> sizeof = 8.

// Q4: How do you minimize struct size?
//     -> Order members from largest to smallest alignment.
//        Groups same-sized members together.
//        Avoids padding gaps between members.

// Q5: What happens if you change float to double in Gamestate?
//     -> double requires 8-byte alignment. struct size becomes
//        multiple of 8. 8+2+1=11, rounds up to 16.
//        sizeof increases from 8 to 16 bytes (double the size!).

// Q6: Why does member ORDER matter for struct size?
//     -> bool, float, short: 1+3pad+4+2+2pad = 12 bytes
//        float, short, bool: 4+2+1+1pad      =  8 bytes
//        Same data, same members, DIFFERENT ORDER = different size.

// Q7: What is the struct's alignment (alignof the struct itself)?
//     -> Equal to the alignment of its LARGEST aligned member.
//        Gamestate with float: alignof = 4.
//        Gamestate with double: alignof = 8.

// Q8: Why does this matter in embedded/defence systems (like ISSST)?
//     -> Memory is limited, cache efficiency critical.
//        Sensor data structures (radar configs, PDUs) used in millions
//        of iterations - layout affects both memory and cache performance.
//        Network protocol structs (DIS PDUs) often require exact layout
//        (#pragma pack) to match wire format.
// ```

// **The visual that explains everything:**

// ```
// BAD ORDER (bool, float, short) = 12 bytes:
// [bool][PAD][PAD][PAD][float float float float][short short][PAD][PAD]
//   1     3                  4                      2           2
// = 12 bytes WASTED 5 bytes on padding!

// GOOD ORDER (float, short, bool) = 8 bytes:
// [float float float float][short short][bool][PAD]
//           4                    2         1    1
// = 8 bytes  only 1 byte wasted!
// ```

// **The golden rule:**

// ```
// Arrange struct members:
// BIGGEST first  →  double/pointer (8 bytes)
//                →  int/float      (4 bytes)
//                →  short          (2 bytes)
// SMALLEST last  →  char/bool      (1 byte)
// ```

// > In ISSST/DIS work this matters doubly — radar data structures and PDU packets iterated at high frequency benefit enormously from cache-friendly layout, and DIS wire-format structs sometimes need `#pragma pack(1)` to match the exact byte layout the protocol expects.