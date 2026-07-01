// ============================================================
// INITIALIZER PRECEDENCE - THREE LEVELS - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <string>

// ============================================================
// SECTION 1: THE THREE LEVELS OF INITIALIZATION - PRIORITY ORDER
// ============================================================

// C++ has THREE places where you can initialize a member variable:
//
// LEVEL 1 (LOWEST priority):  Value initialization via {}
//   Entity e{};  -> would zero everything IF nothing else overrides
//
// LEVEL 2 (MIDDLE priority):  In-class member initializer (NSDMI)
//   int x{1};    -> default value baked into class definition
//
// LEVEL 3 (HIGHEST priority): Constructor member initializer list
//   Entity() : x{2} { }   -> overrides in-class initializer
//
// PRECEDENCE (highest wins):
// Constructor initializer list  >  In-class initializer  >  Value init {}
//
// YOUR COMMENT IS EXACTLY RIGHT:
// "init list will override the x{1} down"
// -> x{1} (in-class) is overridden by x{2} (initializer list)

// ============================================================
// SECTION 2: THE CODE EXPLAINED LAYER BY LAYER
// ============================================================

struct Entity
{
    // LEVEL 3 (WINS): Constructor initializer list
    Entity() : name{}, x{2}, y{4}, collection(nullptr)
    //          ^        ^     ^      ^
    //          |        |     |      |
    //       empty    x=2    y=4   nullptr
    //       string
    // This OVERRIDES the in-class initializers below
    // x{1} and y{7} are IGNORED because this list takes priority
    {
        // constructor body (empty here)
        // at this point: name="", x=2, y=4, collection=nullptr
    }

    // LEVEL 2 (MIDDLE): In-class member initializers (NSDMI)
    // Non-Static Data Member Initializers = C++11 feature
    std::string name;        // no in-class init -> default constructed ("")
    int x{1};                // would be 1 IF no constructor list overrides it
    int y{7};                // would be 7 IF no constructor list overrides it
    int* collection{nullptr};// would be nullptr either way (same value here)

    // NOTE: x{4.2} would be a COMPILER ERROR
    // {} prevents narrowing: 4.2 (double) cannot narrow to int
    // () would allow it silently: int x(4.2) -> x=4 (truncated)
    // {} catches this at compile time -> SAFER
};

// ============================================================
// SECTION 3: PRECEDENCE VISUAL - COMPLETE PICTURE
// ============================================================

//                    WHO SETS x?
//
// Entity e;       -> in-class init runs:  x = 1
// Entity e{};     -> in-class init runs:  x = 1  (NOT zero! in-class wins over value-init)
// Entity e{};     -> WAIT: but Entity has a constructor!
//                    constructor initializer list runs: x = 2
//                    in-class x{1} is IGNORED
//                    e.x = 2
//
// FULL PRIORITY CHAIN FOR e.x:
//
// Value init {}         -> would give 0
//       OVERRIDDEN BY
// In-class init x{1}    -> would give 1
//       OVERRIDDEN BY
// Ctor init list x{2}   -> gives 2  <- FINAL VALUE
//
// SO: e.x = 2

// ============================================================
// SECTION 4: YOUR COMMENT - "Entity e{} with x{1} -> 1 is taken not zero"
// ============================================================

// YOUR COMMENT refers to a class WITHOUT a constructor:
// struct Entity {
//     int x{1};    // in-class initializer
//                  // NO constructor written
// };
//
// Entity e{};  -> x = 1  (NOT 0!)
// -> {} (value init) tries to zero everything
// -> BUT in-class initializer x{1} is MORE SPECIFIC than {}
// -> in-class init WINS over value init
// -> x = 1
//
// CONTRAST: without in-class init:
// struct Entity { int x; };  // no in-class init, no constructor
// Entity e{};  -> x = 0  (value init wins, nothing else overrides it)
// Entity e;    -> x = ??? (garbage, default init, nothing runs)
//
// THIS IS THE HIERARCHY:
// {} alone         = x gets 0
// {} + x{1}        = x gets 1  (in-class overrides value-init)
// {} + x{1} + list = x gets 2  (ctor list overrides in-class)

// ============================================================
// SECTION 5: NARROWING PREVENTION WITH {}
// ============================================================

// YOUR COMMENT: "x{4.2} not possible don't get narrowed down"
// CORRECT!
//
// int x{4.2};    // COMPILER ERROR
//                // "narrowing conversion of 4.2 from double to int"
//                // {} refuses to silently lose the .2
//
// int x(4.2);    // OK but BAD - x = 4 (silently loses .2)
// int x = 4.2;   // OK but BAD - x = 4 (silently loses .2)
//
// SAME for constructor init list:
// Entity() : x{4.2} { }   // COMPILER ERROR - narrowing in {}
// Entity() : x(4.2) { }   // OK but BAD    - x = 4 silently
//
// {} is consistently STRICTER everywhere:
// -> in-class initializers: int x{1};  refuses narrowing
// -> ctor init list: : x{2}  refuses narrowing
// -> local variables: int n{4.2}  refuses narrowing
// -> value init: Entity e{} refuses narrowing for aggregate members

// ============================================================
// SECTION 6: vec3 - AGGREGATE, NO CONSTRUCTOR
// ============================================================

struct vec3
{
    float x, y, z;   // no in-class init, no constructor
};
// vec3 v{};
// -> value initialization (no constructor, no in-class init)
// -> x=0.0f, y=0.0f, z=0.0f  (all zero)
//
// vec3 v;
// -> default initialization (no constructor)
// -> x=???, y=???, z=???  (GARBAGE)
//
// KEY DIFFERENCE FROM Entity:
// Entity has a constructor -> {} calls the constructor
// vec3 has NO constructor  -> {} directly zero-initializes members

// ============================================================
// SECTION 7: main() TRACE
// ============================================================

int main()
{
    vec3 v{};
    // no constructor, no in-class init
    // {} -> value init -> zero init
    // v.x=0, v.y=0, v.z=0

    Entity e{};
    // {} -> calls Entity() constructor (it exists!)
    // constructor initializer list runs: name{}, x{2}, y{4}, collection(nullptr)
    // in-class x{1} and y{7} are OVERRIDDEN by the list
    // FINAL VALUES: name="", x=2, y=4, collection=nullptr

    std::cout << e.name       << std::endl;  // prints: (empty)
    std::cout << e.x          << std::endl;  // prints: 2  (NOT 1, NOT 0!)
    std::cout << e.y          << std::endl;  // prints: 4  (NOT 7, NOT 0!)
    std::cout << e.collection << std::endl;  // prints: 0  (nullptr)

    return 0;
}

// ============================================================
// SECTION 8: COMPLETE PRECEDENCE TABLE - MEMORIZE THIS
// ============================================================

// | Initialization Type          | Syntax              | Priority |
// |-------------------------------|---------------------|----------|
// | Value initialization          | Entity e{}          | LOWEST   |
// | In-class member initializer   | int x{1};           | MIDDLE   |
// | Constructor initializer list  | Entity() : x{2} { } | HIGHEST  |
//
// EACH LEVEL OVERRIDES ALL LOWER LEVELS
// HIGHEST LEVEL ALWAYS WINS

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What are the three levels of member initialization in C++
//     and what is their priority order?
//     -> 1. Value init {}       (lowest  - zeroes everything)
//        2. In-class init x{1}  (middle  - overrides value init)
//        3. Ctor init list x{2} (highest - overrides in-class init)

// Q2: What are NSDMI (Non-Static Data Member Initializers)?
//     -> In-class default values for member variables:
//        int x{1};  or  int x = 1;  inside the class definition.
//        C++11 feature. Used when no constructor init list overrides.

// Q3: If a class has both int x{1} and Entity():x{2}{}, what is x?
//     -> 2. Constructor initializer list always wins over in-class
//        initializer. In-class init is ignored when ctor list sets x.

// Q4: Why does int x{4.2} cause a compiler error?
//     -> Narrowing conversion. {} refuses to silently truncate
//        4.2 (double) to int. Use int x(4.2) or int x = 4.2 if
//        you explicitly want truncation (but prefer avoiding it).

// Q5: Entity e{}; with in-class int x{1} - is x 0 or 1?
//     -> 1. In-class initializer overrides value initialization.
//        {} only gives 0 when there's no in-class init AND
//        no constructor init list AND no user-defined constructor.

// Q6: What is the difference between Entity e; and Entity e{}
//     when Entity has a user-defined constructor?
//     -> Both call the constructor. The {} vs () vs nothing
//        distinction only matters when there is NO user-defined
//        constructor to call.

// Q7: Can you use both () and {} in constructor initializer lists?
//     -> YES. : x{2} and : x(2) are both valid for initializing
//        member x with value 2. {} additionally prevents narrowing.
//        Prefer {} for consistency and safety.

// Q8: In what order are members initialized in the constructor?
//     -> DECLARATION ORDER in the class body, regardless of the
//        order written in the initializer list. Always write the
//        list in the same order as declarations to avoid confusion.
// ```

// **The complete precedence in one picture:**

// ```
// WHAT SETS e.x ?

// {}  value init  →  would give  0
//       ↑ overridden by
// x{1} in-class   →  would give  1
//       ↑ overridden by
// : x{2} ctor list →  gives      2  ← FINAL WINS

// e.x == 2
// ```

// **The narrowing rule is consistent everywhere `{}` appears:**

// ```cpp
// int x{4.2};             // ERROR - in-class
// Entity() : x{4.2} { }  // ERROR - ctor list
// int n{4.2};             // ERROR - local variable
// Entity e{4.2};          // ERROR - aggregate init

// // {} is ALWAYS strict. () is ALWAYS permissive.
// // Use {} everywhere for safety.
// ```