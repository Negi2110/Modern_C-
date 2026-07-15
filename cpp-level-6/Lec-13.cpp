// ============================================================
// reinterpret_cast - RAW MEMORY / SERIALIZATION - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <cstring>  // for std::memcpy

// ============================================================
// SECTION 1: WHAT IS reinterpret_cast?
// ============================================================

// reinterpret_cast = "treat these EXACT BITS as a completely different type"
// NO conversion happens (unlike static_cast which converts)
// NO runtime check (unlike dynamic_cast which checks vtable)
// Just: "these bytes? pretend they are THIS type now"
//
// float pi = 3.14f;
// (int)(pi)                        -> CONVERTS: float 3.14 -> int 3
//                                     changes the VALUE (truncation)
// *reinterpret_cast<int*>(&pi)     -> REINTERPRETS: same bytes read as int
//                                     IEEE 754 float bits read as int
//                                     result = 1078523331 (not 3!)
//                                     NO change to value, just HOW we read it
//
// ANALOGY:
// (int)(pi) = translating a document from French to English (new content)
// reinterpret_cast = using the French document but calling it English
//                    (same bytes, different interpretation)

// ============================================================
// SECTION 2: YOUR QUESTION - WHY 16 BYTES NOT 14?
// ============================================================

class GameState
{
public:
    int  level;          // 4 bytes  (offset 0)
    int  health;         // 4 bytes  (offset 4)
    int  points;         // 4 bytes  (offset 8)
    bool GameCompleted;  // 1 byte   (offset 12)
    bool BossDefeated;   // 1 byte   (offset 13)
    //                   // 2 bytes PADDING (offset 14-15)
};
// TOTAL = 16 bytes (not 14!)
//
// WHY 2 bytes padding at end?
// ALIGNMENT RULE: sizeof(struct) must be multiple of its LARGEST member's alignment
// largest member = int = 4 bytes alignment
// actual data = 4+4+4+1+1 = 14 bytes
// 14 is NOT a multiple of 4
// next multiple of 4 = 16
// compiler adds 2 bytes of TAIL PADDING to make it 16
//
// WHY alignment matters?
// If you have array: GameState arr[2];
// arr[0] ends at byte 13 (if 14 bytes)
// arr[1].level starts at byte 14 -> NOT 4-byte aligned -> CPU problem!
// With padding: arr[0] ends at 15, arr[1].level starts at 16 -> aligned!
//
// MEMORY LAYOUT (16 bytes):
// Byte: 0  1  2  3    4  5  6  7    8  9  10 11   12   13   14  15
//      [level int ][health int ][points int ][GC][BD][pad][pad]
//       level=66    health=100   points=999  F    F   --   --

// ============================================================
// SECTION 3: pi EXAMPLE - SAME BITS, DIFFERENT INTERPRETATION
// ============================================================

// float pi = 3.14f;
//
// 3.14f in IEEE 754 binary (32 bits):
// 0 10000000 10010001111010111000011
// = 0x4048F5C3 in hex
// = 1078523331 in decimal (as unsigned int)
//
// std::cout << (int)(pi)
// -> CONVERSION: 3.14f -> 3 (truncates decimal)
// -> changes the value
//
// std::cout << *reinterpret_cast<int*>(&pi)
// -> takes ADDRESS of pi (float*)
// -> reinterpret_cast: "treat this float* as int*"
// -> dereference: read those 4 bytes AS IF they were an int
// -> prints 1078523331 (the raw bit pattern of 3.14f read as int)
// -> value NOT changed, interpretation changed
//
// &pi == reinterpret_cast<float*>(&pi) ?
// YES! same address, just different type label
// YOUR COMMENT: "is &pi same?" -> yes, same address

// ============================================================
// SECTION 4: THE MAIN USE CASE - SERIALIZATION
// ============================================================

int main()
{
    float pi = 3.14f;

    std::cout << &pi << std::endl;
    // prints address of pi (e.g., 0x7ffee4b3c8ac)

    std::cout << *reinterpret_cast<int*>(&pi) << std::endl;
    // reads pi's bytes AS IF they were an int
    // prints: 1078523331 (raw IEEE 754 bits of 3.14f)

    std::cout << *reinterpret_cast<float*>(&pi) << std::endl;
    // reads pi's bytes AS IF they were a float
    // prints: 3.14 (same as just cout << pi)
    // pointless here but shows the cast mechanism

    // ============================================================
    // SECTION 5: GAMESTATE SERIALIZATION - THE REAL EXAMPLE
    // ============================================================

    GameState gs = {66, 100, 999, false, false};
    // gs.level = 66, health = 100, points = 999
    // GameCompleted = false (0), BossDefeated = false (0)

    char bagOfBytes[sizeof(GameState)];
    // allocates 16 bytes of raw char storage on stack
    // simulates: reading from disk, network, binary file
    // char = 1 byte each -> perfect for raw byte manipulation

    std::memcpy(bagOfBytes, &gs, sizeof(GameState));
    // memcpy = memory copy (from <cstring>)
    // copies sizeof(GameState) = 16 bytes
    // FROM: &gs (source address)
    // TO:   bagOfBytes (destination address)
    // byte by byte, no interpretation
    //
    // YOUR QUESTION: "why and when use memcpy?"
    // USE memcpy WHEN:
    // -> copying raw bytes between buffers (serialization/deserialization)
    // -> copying to/from network/disk/file buffers
    // -> faster than element-by-element copy for large data
    // -> must copy data of TRIVIALLY COPYABLE types (POD types)
    // -> cannot use = operator (types don't match, e.g., struct to char[])
    //
    // DO NOT use memcpy WHEN:
    // -> type has constructors/destructors (std::string, std::vector)
    // -> objects have pointer members (will copy pointer, not data!)
    // -> types are not trivially copyable

    // ---- READING INDIVIDUAL MEMBERS (ugly way) ----
    std::cout << *(bagOfBytes) << std::endl;
    // reads first byte as char -> prints 'B' (ASCII 66 = 'B' = level!)

    std::cout << *((int*)bagOfBytes) << std::endl;
    // C-style cast: treat bagOfBytes as int*
    // reads first 4 bytes as int -> prints 66 (level)
    // YOUR COMMENT: "looks ugly, that's why we use reinterpret_cast"

    // ---- READING WITH reinterpret_cast (better named cast) ----
    std::cout << *reinterpret_cast<int*>(bagOfBytes) << std::endl;
    // same as above but EXPLICIT: "I know these bytes are an int"
    // prints: 66 (level)

    std::cout << *reinterpret_cast<int*>(bagOfBytes + sizeof(int)) << std::endl;
    // bagOfBytes + 4 = skip past first int (level)
    // read next 4 bytes as int
    // prints: 100 (health)

    std::cout << *reinterpret_cast<int*>(bagOfBytes + 2*sizeof(int)) << std::endl;
    // bagOfBytes + 8 = skip past level and health
    // read next 4 bytes as int
    // prints: 999 (points)

    std::cout << *reinterpret_cast<bool*>(bagOfBytes + 3*sizeof(int)) << std::endl;
    // bagOfBytes + 12 = skip past 3 ints
    // read 1 byte as bool
    // prints: 0 (false = GameCompleted)

    std::cout << *reinterpret_cast<bool*>(bagOfBytes + 3*sizeof(int) + sizeof(bool)) << std::endl;
    // bagOfBytes + 13 = skip past 3 ints + 1 bool
    // read 1 byte as bool
    // prints: 0 (false = BossDefeated)

    // ---- BETTER WAY - cast entire buffer to GameState* ----
    GameState gs2 = *reinterpret_cast<GameState*>(bagOfBytes);
    // reinterpret_cast<GameState*>(bagOfBytes):
    //   treat bagOfBytes pointer AS IF it points to a GameState
    // * dereference: copy that GameState into gs2
    // -> SINGLE OPERATION to reconstruct entire struct from bytes!
    //
    // YOUR COMMENT: "better approach because we can access data in a
    // more structured way rather than accessing each member individually
    // using offsets"
    // EXACTLY RIGHT:
    // -> no manual offset calculation
    // -> compiler handles member layout automatically
    // -> readable: gs2.level not *(ptr + 0)
    // -> type-safe member access (not raw byte arithmetic)
    // -> if GameState changes (add member), offset code BREAKS
    //    but gs2.level STILL WORKS automatically

    std::cout << gs2.level         << std::endl;  // 66
    std::cout << gs2.health        << std::endl;  // 100
    std::cout << gs2.points        << std::endl;  // 999
    std::cout << gs2.GameCompleted << std::endl;  // 0
    std::cout << gs2.BossDefeated  << std::endl;  // 0

    return 0;
}

// ============================================================
// SECTION 6: WHEN IS reinterpret_cast ACTUALLY USED?
// ============================================================

// USE CASE 1: SERIALIZATION / DESERIALIZATION (your example)
// Save GameState to file:
// file.write(reinterpret_cast<char*>(&gs), sizeof(GameState));
// Load GameState from file:
// file.read(reinterpret_cast<char*>(&gs2), sizeof(GameState));
// -> converts struct to raw bytes for file I/O
// -> reads raw bytes back into struct

// USE CASE 2: NETWORK PROTOCOL BUFFERS (relevant to DIS/ISSST!)
// DIS PDU received as char buffer from network socket:
// char buffer[144];
// recvfrom(socket, buffer, 144, 0, ...);
// EntityStatePDU* pdu = reinterpret_cast<EntityStatePDU*>(buffer);
// pdu->entityID.site  // access structured data
// pdu->entityLocation // direct access without offset math
// -> THIS IS EXACTLY HOW DIS packets are read in practice!

// USE CASE 3: HARDWARE REGISTER ACCESS (embedded systems)
// volatile uint32_t* GPIO_PORT = reinterpret_cast<uint32_t*>(0x40020000);
// *GPIO_PORT = 0x01;  // write to hardware address
// -> hardware registers have fixed memory addresses
// -> must use reinterpret_cast to treat integer address as pointer

// USE CASE 4: TYPE PUNNING (inspecting float bits)
// float f = 3.14f;
// uint32_t bits = *reinterpret_cast<uint32_t*>(&f);
// -> see IEEE 754 representation of float
// -> used in fast inverse square root (Quake III algorithm!)
// NOTE: std::bit_cast<uint32_t>(f) is the C++20 safe alternative!

// ============================================================
// SECTION 7: DANGERS AND RULES
// ============================================================

// RULE 1: ONLY safe to reinterpret_cast back to original type
// float f = 3.14f;
// int* iptr = reinterpret_cast<int*>(&f);
// float* fptr = reinterpret_cast<float*>(iptr);  // OK: back to float
// *fptr == f  // true: round-trip is safe
// *iptr       // accessing as int = technically UB in C++
//              (but universally supported in practice)

// RULE 2: STRICT ALIASING (C++ standard rule)
// float f = 3.14f;
// int* ip = reinterpret_cast<int*>(&f);
// *ip = 5;  // UNDEFINED BEHAVIOR! (violates strict aliasing rule)
//            // compiler assumes float* and int* never alias
//            // can produce wrong results with optimization!
//
// SAFE alternatives:
// std::memcpy(&intVal, &f, sizeof(float));  // safe copy
// std::bit_cast<int>(f);                    // C++20 safe type pun

// RULE 3: ALIGNMENT
// char buf[4];
// int* ip = reinterpret_cast<int*>(buf);  // potentially misaligned!
// *ip = 5;  // UB if buf not 4-byte aligned
// std::memcpy is safe even for misaligned access

// RULE 4: ONLY for TRIVIALLY COPYABLE types
// GameState: int, bool members -> trivially copyable -> safe
// std::string member -> NOT trivially copyable -> UNSAFE to memcpy/reinterpret_cast!

// ============================================================
// SECTION 8: memcpy vs reinterpret_cast
// ============================================================

// BOTH copy raw bytes, but differently:
//
// memcpy: copies BYTES from one location to another
//   memcpy(dest, src, size) -> bytes moved, both interpreted as THEIR OWN types
//   safe for misaligned data
//   works at byte level
//
// reinterpret_cast: changes how you INTERPRET existing bytes
//   no bytes moved, no copying
//   same address, different type label
//   alignment must be respected!
//
// SERIALIZATION PATTERN (DIS/file/network):
// WRITE: memcpy(buffer, &gs, sizeof(gs))
//        -> copy struct bytes into raw buffer
//
// READ:  GameState gs2 = *reinterpret_cast<GameState*>(buffer)
//        -> treat buffer as GameState, copy it out
//        OR: memcpy(&gs2, buffer, sizeof(gs2))
//        -> both work for trivially copyable types

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does reinterpret_cast do?
//     -> Reinterprets the raw bits/bytes of a value as a completely
//        different type. No conversion, no runtime check.
//        "Treat these exact bytes as this other type."

// Q2: What is the difference between (int)pi and *reinterpret_cast<int*>(&pi)?
//     -> (int)pi: CONVERTS float 3.14 to int 3 (truncation, value changes).
//        reinterpret_cast: reads same bytes AS int (1078523331).
//        Value unchanged, interpretation changed.

// Q3: Why is sizeof(GameState) 16 and not 14?
//     -> Struct alignment: size must be multiple of largest member's
//        alignment (int = 4 bytes). 14 not divisible by 4, so compiler
//        adds 2 bytes of tail padding -> 16 bytes.

// Q4: When would you use reinterpret_cast in real code?
//     -> Serialization (struct to char buffer for file/network I/O).
//        Network protocol parsing (cast received bytes to struct).
//        Hardware register access (cast fixed address to pointer).
//        DIS PDU processing (cast socket buffer to PDU struct).

// Q5: What is memcpy and when should you use it?
//     -> Copies N bytes from source to destination at byte level.
//        Use for: raw buffer copies, serialization, data between
//        incompatible types. Only for trivially copyable types.
//        Do NOT use for types with constructors/pointer members.

// Q6: Why is the "better way" (casting entire struct) preferred?
//     -> Single operation instead of manual offset arithmetic.
//        Compiler handles struct layout automatically.
//        Readable (gs2.level not *(ptr+0)).
//        Resilient to struct changes (adding member doesn't break code).

// Q7: What types are safe to use with reinterpret_cast / memcpy?
//     -> Trivially copyable types: no constructors, no virtual functions,
//        no pointer members, no reference members. int, float, bool,
//        plain structs/classes with only such members. NOT std::string,
//        std::vector, or anything with heap-managed resources.

// Q8: What is the C++20 alternative to type punning with reinterpret_cast?
//     -> std::bit_cast<T>(value) from <bit>.
//        Safely reinterprets value as type T.
//        Well-defined (no strict aliasing violation).
//        Requires sizeof(source) == sizeof(T).
//        std::bit_cast<int>(3.14f) = 1078523331 (safe and defined!).
// ```

// **reinterpret_cast in one picture:**

// ```
// float pi = 3.14f;
// Memory: [01000000 01001000 11110101 11000011]  (4 bytes)

// (int)(pi):                    *reinterpret_cast<int*>(&pi):
// CONVERTS: 3.14f → 3           REINTERPRETS: same bytes → 1078523331
// VALUE changes                 VALUE doesn't change, MEANING changes
// ```

// **The serialization pattern (DIS/ISSST connection):**

// ```cpp
// // WRITE to network/disk:
// char buffer[sizeof(GameState)];
// std::memcpy(buffer, &gs, sizeof(GameState));  // struct → bytes
// send(socket, buffer, sizeof(GameState), 0);   // send raw bytes

// // READ from network/disk:
// char buffer[sizeof(GameState)];
// recv(socket, buffer, sizeof(GameState), 0);   // receive raw bytes
// GameState gs2 = *reinterpret_cast<GameState*>(buffer); // bytes → struct
// // gs2.level, gs2.health etc. all accessible immediately!
// ```

// > **The DIS connection**: every DIS PDU received over UDP arrives as a `char[]` buffer. `reinterpret_cast<EntityStatePDU*>(buffer)` is **exactly** how you parse it — same pattern as your GameState example, just with a DIS-specific struct. This is why understanding `reinterpret_cast` + `memcpy` + struct alignment is directly applicable to your ISSST work.