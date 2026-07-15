// ============================================================
// FUNCTORS AS COMPARATORS - std::sort - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <vector>
#include <algorithm>

// ============================================================
// SECTION 1: TWO WAYS TO DEFINE SORTING BEHAVIOR
// ============================================================

// YOUR CODE shows BOTH approaches:
//
// WAY 1: operator< INSIDE the class (member comparison)
// -> defines "natural" ordering for the type
// -> std::sort uses this BY DEFAULT (when no comparator given)
// -> sorts by mHealth (less = earlier in sorted order)
//
// WAY 2: EXTERNAL FUNCTOR (GoblinComparator)
// -> CUSTOM comparator passed to std::sort as third argument
// -> overrides the natural ordering
// -> sorts by mStrength instead of mHealth
// -> FUNCTOR IS THE KEY: it's a callable object with state capability

struct Goblin
{
    int mHealth;
    int mStrength;

    Goblin(int h, int s) : mHealth(h), mStrength(s) {}

    // WAY 1: member operator< (natural ordering)
    bool operator<(const Goblin& rhs)
    // ^                         ^
    // |                         rhs = right hand side (other goblin)
    // this = left hand side (current goblin)
    // "is THIS goblin less than RHS goblin?"
    {
        return this->mHealth < rhs.mHealth;
        // sort by health: lowest health comes first
        // (this->mHealth is same as just mHealth, 'this->' optional)
    }
    // USED BY: std::sort(begin, end)  <- no third argument
    // USED BY: < operator in general (comparisons, priority_queue etc.)
};

// WAY 2: EXTERNAL COMPARATOR FUNCTOR
struct GoblinComparator
{
    bool operator()(const Goblin& lhs, const Goblin& rhs)
    //               ^                   ^
    //               left hand side      right hand side
    //               (both explicit - no 'this' like in member operator<)
    {
        return lhs.mStrength < rhs.mStrength;
        // sort by strength: lowest strength comes first
    }
    // USED BY: std::sort(begin, end, GoblinComparator())
    //          the third argument overrides the default operator
};

// ============================================================
// SECTION 2: HOW std::sort USES THE COMPARATOR
// ============================================================

// std::sort internally does something like:
// template<typename Iterator, typename Compare>
// void sort(Iterator first, Iterator last, Compare comp) {
//     // ... sorting algorithm ...
//     if (comp(element_a, element_b)) {  // calls your functor!
//         // a comes before b
//     }
// }
//
// GoblinComparator() creates a TEMPORARY functor object
// std::sort stores this object and calls operator() during sorting
// comp(goblin1, goblin2) -> GoblinComparator::operator()(goblin1, goblin2)
//                        -> goblin1.mStrength < goblin2.mStrength

// ============================================================
// SECTION 3: main() TRACE
// ============================================================

int main()
{
    std::vector<Goblin> goblins{
        Goblin(5,   205),   // health=5,   strength=205
        Goblin(3,   25),    // health=3,   strength=25
        Goblin(100, 1),     // health=100, strength=1
        Goblin(1,   11)     // health=1,   strength=11
    };

    // BEFORE SORT:
    // health:   5,   3,  100,  1
    // strength: 205, 25,   1, 11

    std::sort(goblins.begin(), goblins.end(), GoblinComparator());
    // GoblinComparator() = creates temporary functor instance
    // sorts by mStrength ASCENDING (lowest strength first)
    //
    // SORTED BY STRENGTH:
    // strength: 1,  11,  25, 205
    // health:   100, 1,   3,   5
    //
    // NOTE: operator< (mHealth) is IGNORED here
    // The third argument OVERRIDES the default comparison

    for (auto g : goblins)
    {
        std::cout << g.mHealth << std::endl;
        // prints health values in order of STRENGTH (not health!)
    }

    return 0;
}

// OUTPUT:
// 100   <- strength=1  (weakest, comes first)
// 1     <- strength=11
// 3     <- strength=25
// 5     <- strength=205 (strongest, comes last)

// ============================================================
// SECTION 4: THREE WAYS TO SORT - COMPARISON
// ============================================================

// WAY 1: DEFAULT (uses operator<) - sorts by health
// std::sort(goblins.begin(), goblins.end());
// -> uses Goblin::operator<(const Goblin& rhs)
// -> sorts by mHealth ascending
// -> output health: 1, 3, 5, 100

// WAY 2: FUNCTOR (your code) - sorts by strength
// std::sort(goblins.begin(), goblins.end(), GoblinComparator());
// -> uses GoblinComparator::operator()
// -> sorts by mStrength ascending
// -> output health: 100, 1, 3, 5

// WAY 3: LAMBDA (modern equivalent of functor)
// std::sort(goblins.begin(), goblins.end(),
//     [](const Goblin& a, const Goblin& b) {
//         return a.mStrength < b.mStrength;
//     });
// -> IDENTICAL result to WAY 2 (functor)
// -> lambda IS a functor internally (compiler generates GoblinComparator-like struct)
// -> preferred in modern C++ for simple one-off comparisons
// -> use functor when comparator is REUSED or has STATE

// WAY 4: SORT DESCENDING (largest strength first)
// std::sort(goblins.begin(), goblins.end(),
//     [](const Goblin& a, const Goblin& b) {
//         return a.mStrength > b.mStrength;  // > instead of 
//     });
// OR with functor:
// struct GoblinComparatorDesc {
//     bool operator()(const Goblin& lhs, const Goblin& rhs) {
//         return lhs.mStrength > rhs.mStrength;  // > flips order
//     }
// };

// ============================================================
// SECTION 5: COMPARATOR RULES - STRICT WEAK ORDERING
// ============================================================

// std::sort requires: STRICT WEAK ORDERING
// Your comparator MUST satisfy:
//
// 1. IRREFLEXIVITY: comp(a, a) must be false
//    lhs.mStrength < rhs.mStrength
//    same goblin vs itself: s < s = false ✓
//
// 2. ASYMMETRY: if comp(a,b) is true, comp(b,a) must be false
//    if a.strength < b.strength -> b.strength < a.strength is false ✓
//
// 3. TRANSITIVITY: if comp(a,b) and comp(b,c), then comp(a,c)
//    if a < b and b < c then a < c ✓ (standard < is transitive)
//
// VIOLATING THESE = UNDEFINED BEHAVIOR in std::sort!
// BAD COMPARATOR example:
// return lhs.mStrength <= rhs.mStrength;  // WRONG! not irreflexive
//                                           // a <= a is TRUE (violates rule 1)
// -> std::sort may crash or produce garbage

// ============================================================
// SECTION 6: STATEFUL COMPARATOR - WHERE FUNCTOR BEATS LAMBDA
// ============================================================

// Functor WITH STATE (lambda with capture does this too but functor is clearer):
//
// struct GoblinComparatorByField {
//     enum Field { HEALTH, STRENGTH };
//     Field m_sortField;    // STATE: which field to sort by
//
//     GoblinComparatorByField(Field f) : m_sortField(f) {}
//
//     bool operator()(const Goblin& lhs, const Goblin& rhs) {
//         if (m_sortField == HEALTH)
//             return lhs.mHealth < rhs.mHealth;
//         else
//             return lhs.mStrength < rhs.mStrength;
//     }
// };
//
// // sort by health:
// std::sort(goblins.begin(), goblins.end(),
//     GoblinComparatorByField(GoblinComparatorByField::HEALTH));
//
// // sort by strength:
// std::sort(goblins.begin(), goblins.end(),
//     GoblinComparatorByField(GoblinComparatorByField::STRENGTH));
//
// SAME comparator type, DIFFERENT BEHAVIOR based on constructor argument
// This is the POWER of stateful functors!

// ============================================================
// SECTION 7: MEMBER operator< vs EXTERNAL COMPARATOR
// ============================================================

// MEMBER operator<:
// -> defines the "natural" ordering of the type
// -> used by: std::sort (default), std::min, std::max,
//             std::set (default), std::map (default), < in if statements
// -> only ONE natural ordering per class
// -> const-correctness: should be const!
//    bool operator<(const Goblin& rhs) const { ... }
//                                       ^^^^^ missing in your code!
//
// EXTERNAL COMPARATOR (functor/lambda):
// -> custom ordering for specific use case
// -> can have MULTIPLE comparators for same type
// -> used when: std::sort with third arg, std::set<T,Comp>,
//               std::map<K,V,Comp>, priority_queue with custom order
// -> doesn't modify the class at all
// -> PREFERRED for: one-off sorts, multiple orderings, complex logic

// ============================================================
// SECTION 8: BUG IN YOUR CODE - MISSING const
// ============================================================

// bool operator<(const Goblin& rhs)           // your code
// bool operator<(const Goblin& rhs) const     // correct!
//                                    ^^^^^
//
// WITHOUT const:
// -> cannot call on const Goblin objects
// -> cannot use in std::sort with const references
// -> violates const correctness
//
// SAME for comparator functor:
// bool operator()(const Goblin& lhs, const Goblin& rhs)           // your code
// bool operator()(const Goblin& lhs, const Goblin& rhs) const     // correct!
//                                                        ^^^^^
// std::sort requires the comparator's operator() to be const
// (because sort stores it and calls it on a const comparator)

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a comparator functor?
//     -> A struct/class with operator() that takes two values and returns
//        bool. Passed to std::sort as third argument to define custom
//        ordering. Overrides the default operator< comparison.

// Q2: What is the difference between operator< and an external comparator?
//     -> operator<: defines natural/default ordering for the type.
//        One per class. Used when no third argument given to std::sort.
//        External comparator: custom ordering for specific use case.
//        Multiple comparators can exist for the same type.

// Q3: What rules must a comparator satisfy for std::sort?
//     -> Strict weak ordering: irreflexivity (comp(a,a)=false),
//        asymmetry (comp(a,b) → !comp(b,a)), transitivity.
//        Using <= instead of < violates irreflexivity → undefined behavior!

// Q4: What is GoblinComparator() in the sort call?
//     -> Creates a TEMPORARY instance of the GoblinComparator functor.
//        std::sort takes this object and calls operator() during sorting.
//        Parentheses = constructor call (default constructor here).

// Q5: What is the output and why?
//     -> Sorts by mStrength ascending: strength 1,11,25,205.
//        Prints mHealth in that order: 100, 1, 3, 5.
//        operator< (mHealth) is completely ignored.

// Q6: What is the modern C++ equivalent of a comparator functor?
//     -> A lambda: [](const Goblin& a, const Goblin& b) { return a.mStrength < b.mStrength; }
//        Functors preferred when: reused across multiple call sites,
//        need state that changes between constructions.

// Q7: What bug exists in the code?
//     -> Missing const on both operator< and comparator's operator().
//        bool operator<(const Goblin& rhs) should be const.
//        bool operator()(const Goblin& lhs, const Goblin& rhs) should be const.
//        Without const: can't sort const containers, violates const correctness.

// Q8: Can the same type have multiple sort orders?
//     -> YES via multiple comparator functors (or lambdas).
//        GoblinSortByHealth, GoblinSortByStrength, GoblinSortByName.
//        std::set<Goblin, GoblinSortByStrength> uses strength for ordering.
//        Only ONE operator< per class, but unlimited external comparators.
// ```

// **The three sort approaches side by side:**

// ```cpp
// std::vector<Goblin> goblins = { ... };

// // DEFAULT (uses operator<, sorts by health):
// std::sort(goblins.begin(), goblins.end());
// // output health: 1, 3, 5, 100

// // FUNCTOR (sorts by strength):
// std::sort(goblins.begin(), goblins.end(), GoblinComparator());
// // output health: 100, 1, 3, 5

// // LAMBDA (same as functor, modern style):
// std::sort(goblins.begin(), goblins.end(),
//     [](const Goblin& a, const Goblin& b) { return a.mStrength < b.mStrength; });
// // output health: 100, 1, 3, 5
// ```

// **The output explained:**

// ```
// BEFORE SORT:           AFTER SORT (by strength↑):
// health | strength      strength | health
//    5   |   205            1     |  100   ← weakest first
//    3   |    25           11     |    1
//  100   |     1           25     |    3
//    1   |    11          205     |    5   ← strongest last

// Printing health: 100, 1, 3, 5
// ```

// > **The bug to flag in interviews**: both `operator<` and the comparator's `operator()` are missing `const`. This means you can't sort `const` containers and violates const correctness. Every comparator and `operator<` should be `const` — it's one of the most common C++ review comments in code interviews.