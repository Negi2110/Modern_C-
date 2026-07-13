
// ============================================================
// API DESIGN - PIMPL IDIOM vs STRUCT OPTION - INTERVIEW NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS API DESIGN?
// ============================================================

// API = Application Programming Interface
// The PUBLIC FACE of your code - what users of your class SEE and USE
//
// GOOD API DESIGN means:
// -> hiding implementation details (encapsulation)
// -> stable interface (users don't recompile when internals change)
// -> clear, easy to use correctly
// -> hard to use INCORRECTLY
//
// TWO COMMON PATTERNS for a Character class:
// 1. PIMPL IDIOM  -> maximum hiding, stable ABI, more complex
// 2. STRUCT OPTION -> simple, fast, less hiding

// ============================================================
// SECTION 2: PATTERN 1 - PIMPL IDIOM
// ============================================================

// character.hpp (PUBLIC - users see this):
// ----------------------------------------
// class Character {
// public:
//     Character(std::string name);
//     ~Character();
//
//     std::string getName()    const;
//     float       getSpeed()   const;
//     float       getAgility() const;
//     void        setSpeed(float speed);
//     void        setAgility(float agility);
//
// private:
//     struct CharacterImpl;                     // forward declaration only
//     std::unique_ptr<CharacterImpl> m_pImpl;   // opaque pointer
// };
//
// character.cpp (PRIVATE - users never see this):
// ------------------------------------------------
// struct Character::CharacterImpl {
//     std::string m_name;     // TRULY hidden - not visible in header
//     float       m_speed;
//     float       m_agility;
// };
//
// Character::Character(std::string name)
//     : m_pImpl(std::make_unique<CharacterImpl>())
// {
//     m_pImpl->m_name    = name;
//     m_pImpl->m_speed   = 1.0f;
//     m_pImpl->m_agility = 1.0f;
// }
// Character::~Character() = default;  // must be in .cpp (unique_ptr + incomplete type)
//
// std::string Character::getName()    const { return m_pImpl->m_name; }
// float       Character::getSpeed()   const { return m_pImpl->m_speed; }
// float       Character::getAgility() const { return m_pImpl->m_agility; }
// void        Character::setSpeed(float s)   { m_pImpl->m_speed = s; }
// void        Character::setAgility(float a) { m_pImpl->m_agility = a; }

// PIMPL BENEFITS:
// ✅ TRUE information hiding (members not visible in header AT ALL)
// ✅ COMPILATION FIREWALL
//    add/remove/change private members -> character.hpp NEVER changes
//    -> only character.cpp recompiles
//    -> users of Character: ZERO recompilation
// ✅ BINARY COMPATIBILITY (ABI stability)
//    ship new character.cpp DLL -> users don't recompile
// ✅ Reduced header dependencies
//    character.hpp doesn't need to include <string>, <vector> etc.
//    those inclusions stay in character.cpp
//
// PIMPL COSTS:
// ❌ One extra heap allocation per Character object
// ❌ One extra pointer dereference for every member access
// ❌ More code to write (boilerplate)
// ❌ Harder to debug (members not directly visible)
// ❌ Copy constructor/assignment need careful implementation

// ============================================================
// SECTION 3: PATTERN 2 - STRUCT OPTION
// ============================================================

// character.hpp (PUBLIC - users see everything):
// -----------------------------------------------
// struct Character {
//     std::string m_name;
//     float       m_speed    = 1.0f;
//     float       m_agility  = 1.0f;
// };
//
// OR with private + getters (slightly more encapsulated):
// -------------------------------------------------------
// class Character {
// public:
//     Character(std::string name)
//         : m_name(name), m_speed(1.0f), m_agility(1.0f) {}
//
//     std::string getName()    const { return m_name; }
//     float       getSpeed()   const { return m_speed; }
//     float       getAgility() const { return m_agility; }
//     void        setSpeed(float s)   { m_speed = s; }
//     void        setAgility(float a) { m_agility = a; }
//
// private:
//     std::string m_name;
//     float       m_speed;
//     float       m_agility;
// };
//
// character.cpp:
// (implementations if not inline, or empty if all inline above)

// STRUCT OPTION BENEFITS:
// ✅ Simple - less code to write
// ✅ Fast - no extra heap allocation, no pointer dereference
// ✅ Easier to debug (all members visible in debugger directly)
// ✅ Works well when: small team, internal code, performance critical
// ✅ No rule-of-five complexity (no raw pointer to manage)
// ✅ Compiler can optimize better (inlining, no indirection)
//
// STRUCT OPTION COSTS:
// ❌ Implementation VISIBLE in header (private but still declared there)
// ❌ Change any private member -> ALL including files recompile
// ❌ No ABI stability (change member type -> recompile everything)
// ❌ Header includes can leak to users (must include <string> in header)

// ============================================================
// SECTION 4: SIDE-BY-SIDE USAGE - FROM USER'S PERSPECTIVE
// ============================================================

// USAGE IS IDENTICAL from the outside:
//
// Character hero("Arthur");
// hero.setSpeed(5.0f);
// std::cout << hero.getName() << std::endl;
// std::cout << hero.getSpeed() << std::endl;
//
// Both patterns: same call syntax, same behavior
// Difference is ENTIRELY internal and affects:
// -> compile times
// -> binary compatibility
// -> memory layout
// -> maintainability

// ============================================================
// SECTION 5: MEMORY LAYOUT COMPARISON
// ============================================================

// STRUCT OPTION (Character object):
// STACK:
// [m_name ptr+size+cap (24 bytes)]
// [m_speed (4 bytes)             ]
// [m_agility (4 bytes)           ]
// [m_name heap data              ] -> on heap via string
// sizeof(Character) = ~32 bytes
// ONE allocation (string's internal heap)
// Direct member access: character.m_speed (no indirection)
//
// PIMPL (Character object):
// STACK:
// [m_pImpl pointer (8 bytes)]   -> points to heap
// HEAP (CharacterImpl):
// [m_name ptr+size+cap (24 bytes)]
// [m_speed (4 bytes)             ]
// [m_agility (4 bytes)           ]
// [m_name heap data              ] -> also on heap
// sizeof(Character) = 8 bytes (just the pointer!)
// TWO heap allocations (pImpl + string's internal)
// Indirect member access: m_pImpl->m_speed (one extra dereference)

// ============================================================
// SECTION 6: DECISION GUIDE - WHICH TO USE?
// ============================================================

// USE PIMPL WHEN:
// -> Building a LIBRARY distributed to external users
// -> ABI stability required (can update .dll/.so without recompile)
// -> Large project where recompile times are a real problem
// -> Hiding proprietary implementation details from clients
// -> Qt convention (ALL Qt classes use d-pointer = PIMPL)
// -> Your ISSST library if distributed to DRDO/other teams!
//
// USE STRUCT OPTION WHEN:
// -> Internal code (only your team uses it)
// -> Performance critical (no extra indirection)
// -> Small/simple class (not worth the boilerplate)
// -> Prototyping/early development
// -> Simple data classes (Point2D, Color, Vec3)
// -> Game entities that need tight memory layout (cache performance)

// ============================================================
// SECTION 7: THE Qt d-POINTER (PIMPL IN PRACTICE)
// ============================================================

// Qt uses PIMPL everywhere via "d-pointer" convention:
// Every QObject-derived class:
//
// class QWidget : public QObject {
//     Q_DECLARE_PRIVATE(QWidget)  // macro sets up pimpl
//     QWidgetPrivate* d_ptr;      // THE pimpl pointer
// public:
//     // public API only
// };
//
// class QWidgetPrivate {
//     // ALL private data here (hidden in Qt's source)
//     QRect geometry;
//     QFont font;
//     bool visible;
//     // hundreds of members...
// };
//
// WHY Qt chose PIMPL for EVERYTHING:
// -> Qt is a LIBRARY used by millions of developers
// -> Qt can add/remove private members between Qt versions
// -> Your compiled Qt5.12 app can load Qt5.15 DLL without recompile
// -> This is BINARY COMPATIBILITY - only possible with PIMPL!
//
// IN YOUR ISSST WORK:
// If TDF plugin API uses PIMPL -> O2I can update ISSST internals
// without requiring CABS DRDO to recompile their integration code
// -> THIS is the practical value of PIMPL in defence software!

// ============================================================
// SECTION 8: HYBRID APPROACH
// ============================================================

// Sometimes best to combine:
// - Simple POD structs for data (no hiding needed)
// - PIMPL for classes with complex implementation
// - Virtual interface for maximum flexibility
//
// VIRTUAL INTERFACE (most decoupled):
// class ICharacter {
// public:
//     virtual ~ICharacter() = default;
//     virtual std::string getName()    const = 0;
//     virtual float       getSpeed()   const = 0;
//     virtual float       getAgility() const = 0;
//     virtual void        setSpeed(float s)   = 0;
//     virtual void        setAgility(float a) = 0;
//
//     static std::unique_ptr<ICharacter> create(std::string name);
// };
//
// -> users only see the pure interface (ICharacter)
// -> implementation completely hidden
// -> can swap implementations at runtime (different AI behaviors)
// -> but: virtual call overhead, no value semantics

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the PIMPL idiom?
//     -> Pointer to Implementation. Moves all private data into a
//        separate struct defined in the .cpp file. Class header
//        contains only a pointer to that struct. Provides compilation
//        firewall and true information hiding.

// Q2: What is the main advantage of PIMPL over struct option?
//     -> Compilation firewall: changing private implementation doesn't
//        require recompiling files that include the header.
//        Also provides ABI stability for shared libraries.

// Q3: What is the main disadvantage of PIMPL?
//     -> Extra heap allocation per object and extra pointer dereference
//        for every member access. More boilerplate code to write.
//        Harder to debug.

// Q4: When would you choose struct option over PIMPL?
//     -> Internal code, performance-critical paths, simple data classes,
//        small teams where recompile time is not an issue.

// Q5: What is ABI stability and why does PIMPL provide it?
//     -> ABI (Application Binary Interface) stability means compiled
//        code can work with a new version of a library without
//        recompiling. PIMPL provides it because the class size in
//        the header (just one pointer = 8 bytes) never changes even
//        when implementation changes.

// Q6: How does Qt use PIMPL?
//     -> Qt uses "d-pointer" (d_ptr) in every class. QWidgetPrivate
//        contains all Qt's private data. Allows Qt to add features
//        between versions without breaking compiled applications.
//        Accessed via Q_D() macro inside Qt source.

// Q7: What is the sizeof difference between PIMPL and struct option?
//     -> PIMPL: sizeof = 8 bytes (just the unique_ptr/pointer).
//        Struct: sizeof = sum of all members + padding.
//        Objects are smaller with PIMPL but data still on heap.

// Q8: Why must PIMPL destructor be defined in .cpp, not the header?
//     -> unique_ptr<CharacterImpl> destructor needs CharacterImpl to
//        be a complete type. In the header, CharacterImpl is only
//        forward-declared (incomplete). Defining ~Character() in
//        .cpp where CharacterImpl is fully defined solves this.
// ```

// **Decision tree - which pattern to choose:**

// ```
// Is this a PUBLIC LIBRARY / DISTRIBUTED TO EXTERNAL USERS?
// ├── YES → PIMPL
// │         (ABI stability, compilation firewall, true hiding)
// │
// └── NO (internal code)
//     ├── Performance critical / game entity / tight memory?
//     │   └── STRUCT OPTION (no indirection overhead)
//     │
//     ├── Complex class / many private members / large team?
//     │   └── PIMPL (reduce recompile cascades)
//     │
//     └── Simple data class / POD-like?
//         └── STRUCT OPTION (simple wins)
// ```

// **Memory layout comparison:**

// ```
// STRUCT:  Character object
// [m_name: 24][m_speed: 4][m_agility: 4]  ← all together, direct access

// PIMPL:   Character object
// [m_pImpl: 8] ──→ heap: [m_name: 24][m_speed: 4][m_agility: 4]
//                          ↑ one extra indirection for every access
// ```

// > **The ISSST angle**: if ISSST's sensor/radar APIs are distributed to other DRDO teams as compiled `.so` libraries, **PIMPL is mandatory** — otherwise adding a new radar parameter would force every team to recompile their integration. This is exactly why Qt chose PIMPL for their entire framework, and why it's the right choice for any C++ library with external users.