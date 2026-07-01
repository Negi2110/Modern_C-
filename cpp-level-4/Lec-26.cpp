// ============================================================
// VALUE INITIALIZATION, ZERO INITIALIZATION, = default - NOTES
// ============================================================

#include <iostream>
#include <string>

// ============================================================
// SECTION 1: WHAT IS VALUE INITIALIZATION?
// ============================================================

// Value initialization = initializing an object with {}
// (empty curly braces, no arguments)
//
// WHAT IT DOES:
// -> for fundamental types (int, float, pointer): sets to ZERO
// -> for std::string: sets to empty string ""
// -> for structs/classes: initializes each member recursively
//
// TWO WAYS to trigger it:
// Entity e{};       // value initialization - guaranteed zero
// Entity e = {};    // same thing
//
// WITHOUT {}:
// Entity e;         // default initialization
//                   // fundamental members = GARBAGE (random values)
//                   // std::string still = "" (it has its own constructor)

// ============================================================
// SECTION 2: THE INTERACTION WITH = default
// ============================================================

struct Entity
{
    // OPTION 1: User-defined constructor
    // Entity() : name("mike"), x(0), y(0), collection(nullptr) { }
    // -> if you write YOUR OWN constructor:
    //    Entity e{};  still calls YOUR constructor (values set by you)
    //    Entity e;    also calls YOUR constructor (values set by you)
    //    -> {} makes no difference here, YOUR ctor runs either way

    Entity() = default;
    // -> tells compiler: "generate the default constructor automatically"
    // -> BUT crucially: it PRESERVES value-initialization behavior
    //
    // = default + {} = ZERO INITIALIZATION of all members:
    // name       = ""        (std::string default)
    // collection = nullptr   (pointer zeroed)
    // x          = 0         (int zeroed)
    // y          = 0         (int zeroed)
    //
    // YOUR COMMENT IS EXACTLY RIGHT:
    // "Entity e{} with = default will also zero init everything"
    // "but with constructor declared (user-defined body),
    //  {} will again let generate random values if you don't
    //  explicitly set them in the constructor"
    //
    // TECHNICALLY MORE PRECISE:
    // with USER-DEFINED constructor:
    //   Entity e{};   -> calls YOUR constructor body
    //                    whatever YOUR constructor sets = those values
    //                    whatever YOUR constructor DOESN'T set = garbage!
    // with = default:
    //   Entity e{};   -> zero-initializes ALL members first,
    //                    THEN runs the defaulted constructor (which is empty)
    //                    -> ALL members guaranteed zero/empty

    std::string name;    // class type - always default constructed ("") regardless
    int* collection;     // pointer - garbage without {}, nullptr with {}
    int x;               // int     - garbage without {}, 0 with {}
    int y;               // int     - garbage without {}, 0 with {}
};

// ============================================================
// SECTION 3: vec3 - AGGREGATE TYPE (no constructor at all)
// ============================================================

struct vec3
{
    float x, y;   // NO constructor written at all (not even = default)
                  // vec3 is an AGGREGATE type
};
// AGGREGATE = struct/class with:
// -> no user-declared constructors
// -> no private or protected non-static data members
// -> no base classes (in C++11/14)
// -> no virtual functions
//
// AGGREGATE INITIALIZATION:
// vec3 v{1.0f, 2.0f};  -> x=1.0, y=2.0  (direct member assignment)
// vec3 v{};            -> x=0.0, y=0.0  (zero initialized)
// vec3 v;              -> x=???, y=???  (GARBAGE - junk values)
//
// YOUR COMMENT: "if we don't use {} it will print junk vals"
// EXACTLY RIGHT for aggregate types!

// ============================================================
// SECTION 4: COMPLETE RULES - WHEN DO YOU GET ZERO vs GARBAGE?
// ============================================================

// | Declaration    | Has user ctor? | Members zero'd? |
// |----------------|----------------|-----------------|
// | Entity e;      | NO (=default)  | NO  - garbage   |
// | Entity e{};    | NO (=default)  | YES - zero      |
// | Entity e;      | YES (own body) | DEPENDS on body |
// | Entity e{};    | YES (own body) | DEPENDS on body |
// | vec3 v;        | NO (aggregate) | NO  - garbage   |
// | vec3 v{};      | NO (aggregate) | YES - zero      |
//
// GOLDEN RULE: {} = "give me zero/empty values for anything
//               you don't have an explicit initializer for"

// ============================================================
// SECTION 5: main() TRACE
// ============================================================

int main()
{
    Entity e{};
    // = default + {} -> VALUE INITIALIZATION
    // name       = ""       (empty string)
    // x          = 0
    // y          = 0
    // collection = nullptr  (0x0)

    std::cout << e.name       << std::endl;  // prints: (empty line)
    std::cout << e.x          << std::endl;  // prints: 0
    std::cout << e.y          << std::endl;  // prints: 0
    std::cout << e.collection << std::endl;  // prints: 0 (nullptr address)

    vec3 v{};
    // aggregate + {} -> VALUE INITIALIZATION
    // x = 0.0f
    // y = 0.0f
    // WITHOUT {}: vec3 v;  -> x=???, y=??? (random garbage floats)

    std::cout << v.x << std::endl;  // prints: 0
    std::cout << v.y << std::endl;  // prints: 0

    return 0;
}

// ============================================================
// SECTION 6: WHY THIS MATTERS - THE BUG IT PREVENTS
// ============================================================

// COMMON BUG:
// struct Player { int health; int score; };
// Player p;                     // UNINITIALIZED
// if (p.health > 0) { ... }     // health is GARBAGE! could be anything
// -> undefined behavior, unpredictable crashes
//
// FIX:
// Player p{};                   // health=0, score=0 guaranteed
// if (p.health > 0) { ... }     // safe, predictably false
//
// IN PRODUCTION CODE:
// ALWAYS use {} when creating objects if you want guaranteed
// zero/empty initialization. It costs nothing at runtime and
// eliminates a whole class of bugs.

// ============================================================
// SECTION 7: = default vs = delete - QUICK RECAP
// ============================================================

// = default -> "generate the compiler's version of this function"
//              preserves value-initialization behavior with {}
//              same as not writing a constructor at all, BUT
//              more explicit (reader sees you made a deliberate choice)
//              also: writing = default re-enables the default ctor
//              even when other constructors are present

// = delete  -> "this function is FORBIDDEN"
//              any attempt to call it = compiler error
//              used to: prevent copying, prevent certain conversions

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is value initialization in C++?
//     -> Initializing an object with {} (empty braces).
//        Guarantees that fundamental types are set to zero,
//        pointers to nullptr, and class types are default constructed.

// Q2: What is the difference between Entity e; and Entity e{};?
//     -> Entity e;    default initialization - fundamental members
//                     may contain garbage values (undefined behavior to read)
//        Entity e{};  value initialization - fundamental members
//                     guaranteed to be zero/nullptr/empty

// Q3: What does = default do to value initialization behavior?
//     -> Preserves it. A class with = default constructor will
//        still zero-initialize its fundamental members when
//        constructed with {}. A user-defined constructor body
//        loses this guarantee for members not explicitly set.

// Q4: What is an aggregate type?
//     -> A struct/class with no user-declared constructors,
//        no private non-static members, no base classes, no
//        virtual functions. Supports aggregate initialization
//        with {} and value-initialization (zeroing) with {}.

// Q5: Why does vec3 v; print garbage but vec3 v{}; prints 0?
//     -> vec3 is an aggregate with no constructor.
//        v;   = default initialization = no initialization = garbage
//        v{}  = value initialization = zero initialization = 0.0f

// Q6: When should you always use {} when creating objects?
//     -> When you want guaranteed zero/empty initialization of
//        all members. Use {} by default as a safety habit,
//        especially for structs with fundamental type members.

// Q7: If a class has a user-defined constructor that sets x=5
//     but not y, what is y after Entity e{}?
//     -> If the constructor has a body (user-defined), {} calls
//        that constructor. y is whatever the constructor leaves it
//        (garbage if not set). The "zero guarantee" of {} only
//        applies when the constructor is = default or not declared.

// Q8: What is zero initialization vs value initialization?
//     -> Zero initialization:  sets everything to zero/null/empty
//                               (happens as part of value init for
//                                types without constructors)
//        Value initialization: broader term - for class types with
//                               constructors, calls the constructor;
//                               for others, zero initializes.
//        In practice: using {} always triggers value initialization
//        which ensures zeroing for fundamental types.
// ```

// **The one rule to always use in practice:**

// ```cpp
// Entity e;    // DANGEROUS - x, y, collection = random garbage
// Entity e{};  // SAFE      - x=0, y=0, collection=nullptr, name=""
// ```

// **The = default interaction - the tricky part:**

// ```cpp
// // With = default:
// Entity e{};   // zero initializes ALL members first -> safe

// // With user-defined constructor:
// Entity() { }  // body does nothing explicitly
// Entity e{};   // calls YOUR constructor
//               // x, y, collection still garbage
//               // (= default zero-init guarantee is GONE!)
// ```

// > **Interview one-liner**: always use `{}` when creating objects — it costs nothing and eliminates an entire category of "reading garbage value" bugs. The only time `{}` doesn't guarantee zeros is when you have a **user-defined constructor body** that doesn't initialize certain members — that becomes your responsibility.