// ============================================================
// COMMENTING PHILOSOPHY - "COMMENT THE WHY, NOT THE WHAT"
// ============================================================

#include <iostream>
#include <vector>

// ============================================================
// SECTION 1: THE CORE RULE
// ============================================================

// THE GOLDEN RULE OF COMMENTS:
// CODE tells you WHAT is happening (the compiler reads this)
// COMMENTS tell you WHY it is happening (humans need this)
//
// BAD COMMENT: describes WHAT the code already shows
// GOOD COMMENT: explains WHY this decision was made
//
// YOUR CODE shows both:
//
// BAD (what):
// "im shrinking my vector to its capacity" <- code already shows this!
// "im printing out the capacity of the vector" <- obvious from the code!
//
// GOOD (why):
// "im shrinking my vector because I pre-allocate for each level
//  in my game, about 50 elements, because that's the empirical result
//  we had. so I try to optimise and fit to my actual vector size
//  because I promise to never allocate again"
// <- explains the BUSINESS/DESIGN REASON behind the decision

// ============================================================
// SECTION 2: BAD vs GOOD COMMENTS - SIDE BY SIDE
// ============================================================

// BAD: restates the code (noise, adds nothing):
// i++;                    // increment i
// v.push_back(val);       // push val to vector
// if (x > 0)             // check if x is greater than zero
// myVector.shrink_to_fit();// shrink the vector

// GOOD: explains the reason/context (adds value):
// i++;  // skip the header row (row 0 is always column names)
//
// v.push_back(val);
// // buffer fills up to MAX_ENTITIES (empirically measured as 50)
// // pre-allocating avoids reallocation during combat loops
//
// if (x > 0)
// // x can be negative due to floating point error from GPU readback
// // discard negative values as they indicate sensor noise
//
// myVector.shrink_to_fit();
// // release unused capacity after level load completes
// // level assets are fixed after this point, no more push_back
// // measured: saves ~2MB per level on low-end devices

// ============================================================
// SECTION 3: WHEN TO COMMENT (and when NOT to)
// ============================================================

// COMMENT WHEN:
// -> the WHY is not obvious from the code
// -> you made a non-obvious design decision
// -> there's a bug/hack/workaround with a reason
// -> explaining a formula or algorithm
// -> describing the PURPOSE of a struct/class
// -> warning future developers about side effects

// DON'T COMMENT WHEN:
// -> the code is self-explanatory
// -> the comment just repeats the code in English
// -> good naming already explains it

// SELF-DOCUMENTING CODE > COMMENTS:
// Bad:
// int d = 86400; // seconds in a day
// Good:
// constexpr int SECONDS_PER_DAY = 86400;
// (no comment needed - name explains it!)

// ============================================================
// SECTION 4: CLASS/STRUCT COMMENTS - THE "WHY WAS THIS CREATED"
// ============================================================

// YOUR CODE shows this with the block comment for Vertex:
// /*
//  * vertices and its intended...
//  */
//
// GOOD struct/class comment answers:
// 1. WHY does this type exist?
// 2. WHAT is it intended to represent?
// 3. ANY constraints or invariants?
// 4. HOW is it used (brief example if complex)?

/*
 * Vertex: 2D position in world space for a game entity.
 * Used exclusively by the renderer for mesh construction.
 * Coordinates are in meters relative to world origin (0,0).
 * Created per-level, destroyed when level unloads.
 * NOTE: x,y are NOT screen coordinates (see Camera::worldToScreen)
 */
struct Vertex
{
    float x, y;
};

// ============================================================
// SECTION 5: COMMENTING BUGS, HACKS, WORKAROUNDS
// ============================================================

// These MUST be commented - the code cannot explain them:

// TODO: remove this when DIS protocol v7 is adopted (ticket #142)
// currently clamping to 32767 because DRDO's legacy receiver
// overflows on larger entity IDs - confirmed in field test 2024-01

// HACK: sleeping 10ms here because the Qt socket buffer doesn't
// flush synchronously. Proper fix is to use QSocketNotifier
// but that requires refactoring PDUSender (deferred to Q3)

// FIXME: this calculation is wrong for entities at longitude > 170
// causes geocentric conversion error (confirmed in DRDO demo crash)
// see eulerToHeading() for the related bug

// ============================================================
// SECTION 6: YOUR CODE WITH CORRECTED COMMENTS
// ============================================================

int main()
{
    std::vector<int> myVector{1, 2, 3};

    // Initial capacity (WHAT - no comment needed, obvious from code)
    std::cout << "capacity: " << myVector.capacity() << std::endl;

    myVector.push_back(4);
    std::cout << "capacity: " << myVector.capacity() << std::endl;

    // Release unused heap memory after pre-allocation phase.
    // We pre-allocate generously at level start to avoid reallocation
    // during gameplay (measured: reallocation causes 2ms frame spikes).
    // After all entities are loaded, we shrink because no more
    // entities will be added until the next level load.
    myVector.shrink_to_fit();
    std::cout << "capacity: " << myVector.capacity() << std::endl;

    // Remove the sentinel/header entity at index 0.
    // Index 0 is reserved for the "null entity" placeholder,
    // which is only needed during initialization.
    myVector.erase(myVector.begin());

    std::cout << "capacity: " << myVector.capacity() << std::endl;

    for (size_t i = 0; i < myVector.size(); i++)
    {
        std::cout << myVector[i] << std::endl;
    }

    return 0;
}

// ============================================================
// SECTION 7: COMMENT STYLES IN C++
// ============================================================

// SINGLE LINE: for brief why-comments inline
// int timeout = 5000; // 5 seconds - minimum for DRDO network latency

/* BLOCK: for struct/class/function descriptions */

/**
 * DOXYGEN style: for public API documentation
 * @param param  description
 * @return       description
 * @note         important caveat
 */

// TODO:   known future work
// FIXME:  known bug that needs fixing
// HACK:   workaround, explain why
// NOTE:   important information for reader
// WARN:   danger, explain the risk

// ============================================================
// SECTION 8: INTERVIEW / REAL WORLD RULES
// ============================================================

// Q1: What is the difference between good and bad comments?
//     -> Good comments explain WHY a decision was made.
//        Bad comments describe WHAT the code already shows.
//        "shrink_to_fit()" needs no comment to explain what it does.
//        It needs a comment to explain WHY you're calling it here.

// Q2: When should you NOT write a comment?
//     -> When the code is already self-explanatory.
//        Better fix: improve naming so no comment is needed.
//        constexpr int MAX_ENTITIES = 50 beats "// max 50 entities"

// Q3: What should a class/struct comment explain?
//     -> Why the type was created, what it represents,
//        any invariants/constraints, and how it fits in the system.
//        NOT: a list of its member variables (the code shows that).

// Q4: What are TODO/FIXME/HACK comments for?
//     -> Marking intentional technical debt with context.
//        Must include: what the issue is, why it exists now,
//        and ideally when/how it will be resolved.
//        Without the why: future developers can't judge priority.
// ```

// **The rule in one line:**

// ```
// Code  = WHAT    (compiler reads this, should be self-evident)
// Comment = WHY   (humans need this, cannot be seen from the code)

// "shrink_to_fit()"          ← WHAT (no comment needed)
// "// releasing memory after level load, no more entities added"  ← WHY (valuable!)
// ```

// > **The real test**: if your comment can be deleted and the code still makes complete sense — delete the comment. If deleting it loses information a future developer *needs* — keep it. Comments should survive that test.