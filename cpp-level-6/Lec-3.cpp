// ============================================================
// std::variant - TYPE-SAFE TAGGED UNION - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <variant>  // C++17

// ============================================================
// SECTION 1: WHAT IS std::variant?
// ============================================================

// std::variant = type-safe union (C++17)
// stores ONE value that can be ONE OF several types
// ALWAYS knows WHICH type is currently stored (the "tag")
//
// union U { int i; float f; };
// -> no tracking of which member is active
// -> reading wrong member = UNDEFINED BEHAVIOR silently
// -> NO safety at all
//
// std::variant<int, float>:
// -> ALWAYS tracks which type is active (index())
// -> accessing wrong type = throws std::bad_variant_access
// -> SAFE, type-checked access
//
// YOUR COMMENT FROM THE VIDEO explains it perfectly:
// "variant is not implemented as a struct, still uses a union
//  but with an EXTRA DATA MEMBER for keeping track of the
//  active alternative index"
// -> internally: union (for the data) + int (for the tag/index)
// -> THIS is why sizeof(variant) > sizeof(union)

// ============================================================
// SECTION 2: SIZEOF PROOF - UNION vs VARIANT
// ============================================================

union U
{
    int   i;    // 4 bytes
    short s;    // 2 bytes
};
// sizeof(U) = max(4,2) = 4 bytes
// NO tag/index stored

// std::variant<int, float>:
// sizeof = max(sizeof(int), sizeof(float)) + sizeof(index)
//        = max(4, 4)                       + 4
//        = 4                               + 4
//        = 8 bytes
//
// YOUR COMMENT explains exactly:
// "union size (4) + fixed width 32-bit integer tracker (4) = 8"
//
// std::variant<std::string, std::string>:
// sizeof = sizeof(string) + sizeof(index) = 24 + 4 = 28 bytes
// (on your machine - implementation defined)

// ============================================================
// SECTION 3: CREATING AND USING variant
// ============================================================

int main()
{
    std::cout << "U size:    " << sizeof(U)    << std::endl;  // 4
    // just the union (no index stored)

    std::variant<int, float> data;
    std::cout << "data size: " << sizeof(data) << std::endl;  // 8
    // union (4) + index tracker (4) = 8

    // DEFAULT CONSTRUCTION:
    // data is default-initialized to FIRST TYPE (int) = 0
    // data.index() = 0 (index of int in the type list)

    data = 7.14f;
    // ASSIGNS a float -> data now holds float 7.14f
    // index updated to 1 (float is at position 1 in <int,float>)

    std::cout << data.index() << std::endl;
    // index() = current active type's position in the type list
    // <int, float>: int=0, float=1
    // prints: 1  (because we stored a float)

    // ============================================================
    // SECTION 4: ACCESSING variant - THREE WAYS
    // ============================================================

    // WAY 1: std::get<Type>(data) - throws if wrong type
    // std::cout << std::get<float>(data) << std::endl;
    // -> returns float reference if data holds float
    // -> throws std::bad_variant_access if data holds something else
    // -> your commented-out line is this approach
    //
    // std::cout << std::get<int>(data);  // THROWS! data holds float

    // WAY 2: std::get_if<Type>(&data) - returns nullptr if wrong type
    if (auto attempt = std::get_if<float>(&data))
    //                  ^^^^^^^^^^^^^^^^^^^
    //                  returns float* if data holds float
    //                  returns nullptr if data holds something else
    //                  SAFE: no exception, just null check
    {
        std::cout << "we found a float: " << *attempt << std::endl;
        // attempt is float* -> dereference to get value
        // prints: we found a float: 7.14
    }
    // else: attempt is nullptr, if block skipped

    // WAY 3: std::visit - handle ALL possible types (most complete)
    // std::visit([](auto value) {
    //     std::cout << "value is: " << value << std::endl;
    // }, data);
    // -> lambda called with WHATEVER type is stored
    // -> auto deduces to float here (since data holds float)
    // -> forces you to handle ALL cases (compile error if you miss one)

    return 0;
}

// ============================================================
// SECTION 5: COMPLETE API OVERVIEW
// ============================================================

// CONSTRUCTION:
// std::variant<int,float,string> v;   // default: first type (int=0)
// std::variant<int,float,string> v = 42;       // holds int
// std::variant<int,float,string> v = 3.14f;    // holds float
// std::variant<int,float,string> v = "hello"s; // holds string

// ASSIGNMENT:
// v = 7;      // now holds int (index=0)
// v = 3.14f;  // now holds float (index=1)
// v = "hi"s;  // now holds string (index=2)

// CHECKING:
// v.index()                              -> 0,1,2 (which type)
// std::holds_alternative<int>(v)         -> true/false
// std::holds_alternative<float>(v)       -> true/false

// ACCESSING:
// std::get<int>(v)         -> int& (throws if not int)
// std::get<0>(v)           -> get by INDEX (same as get<int>)
// std::get_if<int>(&v)     -> int* or nullptr (safe, no throw)
// std::get_if<0>(&v)       -> same by index

// VISITING:
// std::visit(callable, v)  -> calls callable with the stored value

// ============================================================
// SECTION 6: std::visit - THE POWERFUL WAY
// ============================================================

// std::visit forces you to handle EVERY possible type:
//
// std::variant<int, float, std::string> v = 3.14f;
//
// std::visit([](auto&& value) {
//     using T = std::decay_t<decltype(value)>;
//     if constexpr (std::is_same_v<T, int>)
//         std::cout << "int: " << value;
//     else if constexpr (std::is_same_v<T, float>)
//         std::cout << "float: " << value;
//     else
//         std::cout << "string: " << value;
// }, v);
//
// OR with overloaded lambdas (pattern matching style):
// std::visit(overloaded{
//     [](int i)         { std::cout << "int: " << i; },
//     [](float f)       { std::cout << "float: " << f; },
//     [](std::string s) { std::cout << "string: " << s; }
// }, v);
//
// COMPILER ERROR if you forget to handle any type!
// This is the "sum type" your comment mentions - you MUST handle ALL alternatives

// ============================================================
// SECTION 7: YOUR COMMENT - SUM TYPE EXPLAINED
// ============================================================

// YOUR COMMENT: "variant/tagged union is basically a SUM TYPE with type safety"
//
// SUM TYPE (from functional programming / type theory):
// A type that can be ONE OF several alternatives
// Size (number of possible values) = SUM of all alternative values
//
// int has 2^32 possible values
// float has 2^32 possible values
// variant<int,float> has 2^32 + 2^32 possible values (sum!)
//
// CONTRAST with PRODUCT TYPE (struct/pair):
// struct { int i; float f; }
// possible values = 2^32 * 2^32 (product - both exist simultaneously!)
//
// variant = "either int OR float" (sum - ONE exists at a time)
// struct  = "int AND float" (product - both exist simultaneously)
//
// "type safety" = variant ENFORCES which alternative is active
// unlike union which lets you read any member (UB if wrong)

// ============================================================
// SECTION 8: MEMORY LAYOUT - INTERNALS
// ============================================================

// std::variant<int, float> internally (conceptual):
// struct variant_internal {
//     union {
//         int   i_storage;
//         float f_storage;
//     };               // 4 bytes (union of the alternatives)
//     int index;       // 4 bytes (which alternative is active: 0=int, 1=float)
// };
// total = 8 bytes (matches sizeof(data) = 8 in your code)
//
// YOUR COMMENT EXAMPLE:
// variant<string, string>:
// union { string s1; string s2; } = 24 bytes (only one string stored)
// int index = 4 bytes
// total = 28 bytes
// (legal to have same type twice: index distinguishes them)

// ============================================================
// SECTION 9: UNION vs VARIANT DECISION GUIDE
// ============================================================

// USE union WHEN:
// -> C interoperability required
// -> Performance critical, cannot afford any overhead
// -> Type punning (inspecting raw bytes) is the goal
// -> Hardware/embedded register access
// -> DIS PDU byte-level manipulation

// USE std::variant WHEN:
// -> Type safety is important (most cases!)
// -> Code readability and maintainability matter
// -> Need to handle multiple types polymorphically
// -> Replace error-prone tagged unions with built-in safety
// -> Modern C++ (C++17+) code

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::variant and how does it differ from union?
//     -> variant stores one of several types (like union) but TRACKS
//        which type is active (tagged union). Accessing wrong type
//        throws exception instead of undefined behavior. Type-safe.

// Q2: Why is sizeof(variant<int,float>) > sizeof(union{int;float;})?
//     -> variant stores an extra index/tag member to track the active type.
//        union: 4 bytes (max member). variant: 4 + 4(index) = 8 bytes.

// Q3: What does index() return?
//     -> Zero-based index of the currently held type in the type list.
//        variant<int,float>: int=0, float=1.
//        After data=7.14f: index()=1.

// Q4: What is the difference between std::get and std::get_if?
//     -> get<T>:    returns reference, THROWS std::bad_variant_access if wrong type.
//        get_if<T>: returns POINTER (T* or nullptr), never throws. Null = wrong type.
//        Use get_if when you're not sure which type is stored.

// Q5: What is std::visit?
//     -> Applies a callable to the stored value. Handles ALL possible
//        types. Compiler error if any type is unhandled. Equivalent to
//        pattern matching in functional languages.

// Q6: What is a sum type?
//     -> A type that can be ONE OF several alternatives (not all at once).
//        Number of possible values = SUM of all alternatives' values.
//        std::variant is C++'s sum type. Contrast with struct (product type
//        where all members exist simultaneously).

// Q7: Can variant hold the same type twice?
//     -> YES. variant<string, string> is legal.
//        Both alternatives are distinguishable via index (0 or 1).
//        Access by type would be ambiguous; must use index: get<0>(v).

// Q8: What exception does variant throw on wrong access?
//     -> std::bad_variant_access (from <variant>).
//        Thrown by std::get<T>(v) when v doesn't hold type T.
//        std::get_if<T>(&v) never throws, returns nullptr instead.
// ```

// **union vs variant - the critical difference:**

// ```cpp
// union U { int i; float f; };
// U u;
// u.i = 42;
// std::cout << u.f;  // SILENT undefined behavior - no error!
//                    // reads garbage, program might crash later

// std::variant<int,float> v;
// v = 42;
// std::cout << std::get<float>(v);  // throws std::bad_variant_access
//                                    // LOUD failure - you know immediately!
// ```

// **sizeof breakdown (your comment explained):**

// ```
// std::variant<int, float>:
// [  union: max(int,float) = 4 bytes  ] + [ index tracker: 4 bytes ] = 8 bytes

// std::variant<string, string>:
// [  union: sizeof(string) = 24 bytes ] + [ index tracker: 4 bytes ] = 28 bytes
// ```

// > **The comment from the video nails it**: `std::variant` is a `union` + an integer index under the hood. The index is what makes it "tagged" — it's always checking "am I an `int` right now or a `float`?" before giving you access. Raw `union` has no such check, which is why it's faster but dangerous. `std::variant` trades a few bytes and a few nanoseconds for **guaranteed safety** — the correct trade-off for application code.