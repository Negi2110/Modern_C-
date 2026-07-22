// ============================================================
// mutable KEYWORD - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <vector>

// ============================================================
// SECTION 1: THE PROBLEM - const member function can't modify members
// ============================================================

// YOU ALREADY KNOW: trailing const on member function
// void ChangeW(float newW) const
//                           ^^^^
// "I promise this function will NOT modify any member variable"
//
// BUT sometimes you have a member that LOGICALLY should be changeable
// even on a "const" object:
// -> cache values (computed once, stored for reuse)
// -> mutex for thread synchronization
// -> last-access timestamp
// -> homogeneous coordinate w (your example!)
//
// CONFLICT:
// "The function doesn't change the LOGICAL state of the object"
// "But it needs to change one PHYSICAL member variable"
//
// SOLUTION: mutable = "this member is exempt from const rules"

// ============================================================
// SECTION 2: YOUR CODE EXPLAINED
// ============================================================

struct Point3f
{
public:
    explicit Point3f(float _x, float _y, float _z) : x(_x), y(_y), w(1.0f)
    //                                                                ^
    //                                                                NOTE: z not initialized!
    //                                                                z = GARBAGE VALUE
    //                                                                only x, y, w initialized
    {
    }

    void ChangeW(const float newW) const
    //                              ^^^^
    //                              const function: promises no modifications
    {
        // x = 1.0f;  // COMPILER ERROR: x is regular member
        //             // const function cannot modify regular members

        w = newW;      // OK! w is mutable - exempt from const restriction
        //              // even inside a const function, mutable CAN be modified
    }

    float x, y, z;      // REGULAR members: const function CANNOT modify
    mutable float w;     // MUTABLE member:  const function CAN modify
    //      ^^^^^
    //      "this member is always writable, even through const"
};

// ============================================================
// SECTION 3: mutable RULES
// ============================================================

// mutable applies to:
// 1. NON-STATIC data members (your case: mutable float w)
// 2. LAMBDA captures (mutable keyword on lambda - different use)
//
// mutable CANNOT apply to:
// -> static members (already class-level, no per-object const)
// -> const members (contradictory)
// -> references (references can't be reseated anyway)
//
// THE KEY RULE:
// mutable = "this member does NOT contribute to the object's
//             observable/logical state"
// -> users of the class should not notice it changes
// -> changing it doesn't affect the VALUE the object represents
//
// Point3f represents a 3D point (x, y, z)
// w = homogeneous coordinate = technical/internal detail
// changing w doesn't change the POINT being represented
// -> making w mutable is semantically reasonable

// ============================================================
// SECTION 4: BUG IN YOUR CODE - z NOT INITIALIZED
// ============================================================

// explicit Point3f(float _x, float _y, float _z) : x(_x), y(_y), w(1.0f)
//                                      ^^                           ^
//                                      parameter received           w initialized
//                                      but NEVER USED!              
//
// z is declared: float x, y, z;
// z is NEVER initialized in initializer list or constructor body!
// -> z = GARBAGE VALUE (whatever was on the stack)
// -> p.z will print some random number
//
// FIX:
// : x(_x), y(_y), z(_z), w(1.0f)
//                  ^^^^^
//                  add z(_z) to initialize it!
//
// ALSO: w is initialized to 1.0f in the list (good, default homogeneous coord)
// but _z parameter is received and ignored (likely a typo/bug)

// ============================================================
// SECTION 5: main() TRACE
// ============================================================

int main()
{
    Point3f p{1.0f, 2.0f, 3.0f};
    // calls: Point3f(1.0f, 2.0f, 3.0f)
    // _x=1.0f, _y=2.0f, _z=3.0f (but _z IGNORED!)
    // initializer list: x(1.0f), y(2.0f), w(1.0f)
    // z = ??? (uninitialized garbage)
    //
    // p.x = 1.0f
    // p.y = 2.0f
    // p.z = ??? (garbage - likely 0 in debug, random in release)
    // p.w = 1.0f (default homogeneous coordinate)

    p.ChangeW(5.0f);
    // const function called on non-const object (OK - non-const CAN call const)
    // w = 5.0f  (mutable allows this even though function is const)
    // x, y, z unchanged
    //
    // p.w = 5.0f now

    std::cout << p.x << '\n'  // 1
              << p.y << '\n'  // 2
              << p.z << '\n'  // ??? garbage (bug!)
              << p.w << '\n'; // 5  (changed by ChangeW via mutable)

    return 0;
}

// EXPECTED OUTPUT (with bug):
// 1
// 2
// 0         <- or garbage (z never initialized)
// 5         <- correctly changed via mutable

// EXPECTED OUTPUT (after fixing z(_z)):
// 1
// 2
// 3
// 5

// ============================================================
// SECTION 6: REAL WORLD mutable USE CASES
// ============================================================

// USE CASE 1: CACHE (most common)
// class ExpensiveComputation {
//     mutable float m_cachedResult{-1.0f};
//     mutable bool  m_cacheValid{false};
// public:
//     float GetResult() const {
//         if (!m_cacheValid) {
//             m_cachedResult = /* expensive computation */;
//             m_cacheValid = true;  // mutable: OK in const function!
//         }
//         return m_cachedResult;
//     }
// };
// -> object is logically const (result doesn't change)
// -> but physically stores computed value for speed
// -> mutable allows cache update inside const function

// USE CASE 2: MUTEX (thread safety)
// class ThreadSafeCounter {
//     mutable std::mutex m_mutex;  // mutable: lock/unlock in const functions
//     int m_count{0};
// public:
//     int GetCount() const {
//         std::lock_guard<std::mutex> lock(m_mutex);  // modifies mutex state
//         return m_count;  // but reading count is "logically const"
//     }
// };
// -> mutex itself changes state when locked/unlocked
// -> but the COUNT being read is "logically const"
// -> mutable mutex = standard pattern in thread-safe const getters

// USE CASE 3: LAZY INITIALIZATION
// class HeavyResource {
//     mutable std::unique_ptr<Resource> m_resource;
//     mutable bool m_loaded{false};
// public:
//     const Resource& Get() const {
//         if (!m_loaded) {
//             m_resource = std::make_unique<Resource>();  // mutable!
//             m_loaded = true;
//         }
//         return *m_resource;
//     }
// };

// USE CASE 4: HOMOGENEOUS COORDINATES (your case)
// In 3D graphics: Point3f has x,y,z as the logical state
// w = homogeneous coordinate (technical detail for matrix math)
// changing w doesn't change which 3D point is represented
// -> making it mutable makes sense conceptually

// ============================================================
// SECTION 7: mutable IN LAMBDAS (different use)
// ============================================================

// In lambdas, mutable means something different:
// allow modification of VALUE-CAPTURED variables
//
// int count = 0;
// auto counter = [count]() mutable {  // mutable on lambda
//     count++;                          // OK: modifying the copy
//     return count;
// };
// counter();  // returns 1 (copy incremented)
// counter();  // returns 2 (copy incremented again)
// count;      // still 0! (original unchanged, copy was modified)
//
// WITHOUT mutable on lambda:
// auto counter = [count]() {
//     count++;  // COMPILER ERROR: value captures are const by default
// };

// ============================================================
// SECTION 8: const vs mutable - THE COMPLETE PICTURE
// ============================================================

// CONST MEMBER FUNCTION:
// void foo() const
// -> promises: "I will not change the LOGICAL state of this object"
// -> effect: all members become read-only INSIDE this function
//
// MUTABLE MEMBER VARIABLE:
// mutable int m_cache
// -> says: "this does NOT contribute to the logical state"
// -> effect: exempt from const restriction - ALWAYS writable
//
// TOGETHER:
// const function + mutable member = "logically const, physically may update cache"
// perfect for: caching, logging, mutex, lazy init, homogeneous coords

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does mutable mean in C++?
//     -> A member variable marked mutable can be modified even inside
//        const member functions. It's exempt from the const restriction.
//        Used for members that don't contribute to the object's logical state.

// Q2: What is the difference between mutable and const?
//     -> const (on function): promises no member variables will be modified.
//        mutable (on member): this specific member is exempt from that promise.
//        They work together: const function can modify only mutable members.

// Q3: When would you use mutable?
//     -> Caching computed values (cache updated inside const getter).
//        Mutex for thread-safe const functions (mutex state changes on lock).
//        Lazy initialization (resource created on first access, const accessor).
//        Logging/debugging (log calls inside const functions).

// Q4: What is wrong with the Point3f code?
//     -> z is declared but never initialized in the constructor initializer list.
//        Parameter _z is received but ignored.
//        p.z will contain a garbage value.
//        Fix: add z(_z) to the initializer list.

// Q5: Can mutable apply to any type of member?
//     -> Only non-static data members. Cannot be: static, const, reference.
//        Also used as keyword on lambda to allow modifying value captures.

// Q6: Is using mutable a design smell?
//     -> Potentially, if overused. Should only be used for members that
//        genuinely don't affect the object's logical state (caches, mutexes).
//        Abusing mutable to bypass const correctness is poor design.

// Q7: Can a const object's mutable member be modified directly?
//     -> YES. Even a const Point3f cp has a modifiable w.
//        const Point3f cp{1,2,3}; cp.w = 5.0f; // OK via mutable!

// Q8: What is the mutable keyword on a lambda?
//     -> Different meaning: allows value-captured variables to be modified
//        inside the lambda body (they are const by default without mutable).
//        Does NOT affect original variables - only the captured copies.
// ```

// **mutable in one picture:**

// ```cpp
// struct Point3f {
//     float x, y, z;      // const function: CANNOT modify these
//     mutable float w;     // const function: CAN modify this

//     void ChangeW(float v) const {
//         // x = 1.0f;   // ERROR: regular member
//         w = v;          // OK: mutable member
//     }
// };
// ```

// **The bug to flag immediately:**

// ```cpp
// explicit Point3f(float _x, float _y, float _z) : x(_x), y(_y), w(1.0f)
// //                                     ^^                    missing z(_z)!
// // _z received, never used → z is garbage
// // Fix: : x(_x), y(_y), z(_z), w(1.0f)
// ```

// > **The semantic rule for mutable**: ask *"if this member changes, does the object represent a different value?"* If **no** (cache, mutex, log counter, homogeneous `w`) → `mutable` is appropriate. If **yes** (the actual data the object models) → do NOT use mutable, fix your const correctness instead.