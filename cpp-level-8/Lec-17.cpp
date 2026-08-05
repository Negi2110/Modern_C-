#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <iterator>

// ============================================================
// std::unordered_multiset - COMPLETE NOTES (C++11)
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::unordered_multiset<T, Hash> =
// hash table that allows DUPLICATE elements, NO ordering
//
// THE FOUR UNORDERED/ORDERED VARIANTS:
// set                -> sorted,   unique
// multiset           -> sorted,   duplicates allowed
// unordered_set      -> unsorted, unique
// unordered_multiset -> unsorted, duplicates allowed   <- THIS ONE
//
// WHEN TO USE unordered_multiset:
// -> need to store duplicates (like multiset)
// -> don't need sorted order (like unordered_set)
// -> want O(1) average operations (like unordered_set)
// -> need to count frequency with O(1) average lookup
//
// | Feature           | multiset  | unordered_multiset |
// |-------------------|-----------|--------------------|
// | Sorted            | YES       | NO                 |
// | Duplicates        | YES       | YES                |
// | Find              | O(log n)  | O(1) avg           |
// | Insert            | O(log n)  | O(1) avg           |
// | count(val)        | O(log n)  | O(1) avg           |
// | Hash function     | NO        | YES (required!)    |
// | operator==        | NO        | YES (required!)    |

// ============================================================
// SECTION 2: CUSTOM TYPE REQUIREMENTS
// ============================================================

// For unordered containers with custom type T, you need:
//
// REQUIREMENT 1: operator==
// -> tells the container: "are these two elements the same?"
// -> used to resolve hash collisions
// -> WITHOUT IT: compiler error
//
// REQUIREMENT 2: hash function
// -> tells the container: "which bucket does this go in?"
// -> provides: size_t operator()(const T&) const noexcept
// -> WITHOUT IT: compiler error (no std::hash<Custom> by default)
//
// set/multiset only need operator< (for tree ordering)
// unordered_set/multiset need operator== AND hash

struct Custom
{
    int field1;
    int field2;

    Custom(int _field1, int _field2) : field1(_field1), field2(_field2) {}

    // REQUIREMENT 1: equality comparison
    bool operator==(const Custom& rhs) const
    {
        return (field1 == rhs.field1 && field2 == rhs.field2);
        // TWO Custom objects are EQUAL only if BOTH fields match
        // c1(2,7) == c2(2,7) -> true
        // c1(1,8) == c2(2,7) -> false (even though sum is same!)
    }

    void print() const
    {
        std::cout << "(" << field1 << "," << field2 << ")="
                  << field1 + field2 << '\n';
    }
};

// ============================================================
// SECTION 3: CUSTOM HASH FUNCTION
// ============================================================

// REQUIREMENT 2: hash function struct
struct hash
{
    // constexpr = can be evaluated at compile time
    // noexcept  = guarantees no exceptions (hash should never throw)
    // const     = doesn't modify the hash struct's state
    constexpr std::size_t operator()(const Custom& c) const noexcept
    {
        return c.field1 + c.field2;
        // hash = sum of both fields
        // Custom(1,6) -> hash = 7
        // Custom(2,7) -> hash = 9
        // Custom(3,6) -> hash = 9  <- COLLISION with (2,7)!
        //                             operator== resolves which is which
    }
};

// WHAT MAKES A GOOD HASH FUNCTION?
// 1. CONSISTENT: same input ALWAYS gives same output
// 2. FAST: O(1) computation
// 3. GOOD DISTRIBUTION: spreads elements across many buckets
//    (minimizes collisions)
//
// YOUR HASH (field1 + field2) IS A POOR HASH BECAUSE:
// Custom(1,9) and Custom(9,1) and Custom(5,5) all hash to 10
// -> all go to same bucket -> linked list traversal -> O(n)!
//
// BETTER HASH - combine fields to reduce collisions:
// return std::hash<int>{}(c.field1) ^ (std::hash<int>{}(c.field2) << 1);
// -> XOR shifts prevent (1,9) and (9,1) from colliding

// ============================================================
// SECTION 4: GENERATORS - gen AND gen_duplicates
// ============================================================

Custom gen()
{
    static int i = 0;
    static int j = 0;
    return Custom(++i, ++j);
    // calls: (1,1), (2,2), (3,3), (4,4), (5,5)
}

Custom gen_duplicates()
{
    static int i = 0;  // SEPARATE static from gen()!
    static int j = 0;  // each function has its OWN statics
    return Custom(++i, ++j);
    // also generates: (1,1), (2,2), (3,3), (4,4), (5,5)
    // -> EXACT DUPLICATES of gen() output!
}

// WHY ARE THEY DUPLICATES?
// Both generators use static int starting at 0
// Both increment before returning
// gen()            -> (1,1),(2,2),(3,3),(4,4),(5,5)
// gen_duplicates() -> (1,1),(2,2),(3,3),(4,4),(5,5)
//
// operator== check: (1,1)==(1,1) -> true -> ARE duplicates!
// unordered_multiset ALLOWS these, unlike unordered_set which rejects them

// ============================================================
// SECTION 5: BUCKET ITERATION - UNIQUE TO UNORDERED CONTAINERS
// ============================================================

void PrintUnorderedBuckets(const std::unordered_multiset<Custom, hash>& b)
{
    for (size_t i = 0; i < b.bucket_count(); ++i)
    {
        std::cout << "======= bucket " << i << " ======\n";

        // BUCKET ITERATORS: b.begin(i) and b.end(i)
        // -> iterate elements WITHIN a specific bucket
        // -> different from b.begin() / b.end() (iterates ALL elements)
        auto it  = b.begin(i);   // first element in bucket i
        auto end = b.end(i);     // past last element in bucket i

        while (it != end) {
            it->print();
            ++it;
        }
        // bucket with multiple elements = COLLISION
        // elements in same bucket: same hash value, but != (not equal)
        // OR: same hash AND equal (true duplicates in multiset!)
    }
}

// ============================================================
// SECTION 6: main() TRACE
// ============================================================

int main()
{
    std::unordered_multiset<Custom, hash> o_multi;
    //                                    ^^^^^
    //                                    our custom hash struct
    //                                    (no operator< needed, just hash + ==)

    // INSERT 5 unique elements: (1,1),(2,2),(3,3),(4,4),(5,5)
    std::generate_n(std::inserter(o_multi, o_multi.begin()), 5, gen);

    // INSERT 5 DUPLICATE elements: (1,1),(2,2),(3,3),(4,4),(5,5)
    std::generate_n(std::inserter(o_multi, o_multi.begin()), 5, gen_duplicates);
    // unordered_set would REJECT these (already present)
    // unordered_multiset ACCEPTS them (allows duplicates)

    // PRINT ALL ELEMENTS (order unpredictable):
    std::for_each(o_multi.cbegin(), o_multi.cend(),
        [](const Custom& c) { c.print(); });

    // STATS:
    std::cout << "size:         " << o_multi.size()         << '\n';
    // = 10 (5 original + 5 duplicates)

    std::cout << "bucket_count: " << o_multi.bucket_count() << '\n';
    // impl defined, typically prime >= element count
    // default max_load_factor = 1.0
    // 10 elements -> at least 10 buckets to stay at load <= 1.0

    std::cout << "load_factor:  " << o_multi.load_factor()  << '\n';
    // = size / bucket_count
    // if bucket_count = 11: 10/11 = 0.909...

    PrintUnorderedBuckets(o_multi);
    // shows which elements ended up in which bucket
    // duplicates (same hash AND same ==) appear TOGETHER in same bucket

    return 0;
}

// ============================================================
// SECTION 7: HOW DUPLICATES WORK IN HASH TABLE
// ============================================================

// INSERTION of (1,1) twice:
//
// STEP 1: hash((1,1)) = 1+1 = 2 -> bucket[2]
// STEP 2: scan bucket[2] for existing (1,1)
//         -> for unordered_SET: found! reject insertion
//         -> for unordered_MULTISET: allow regardless, insert again!
//
// BUCKET AFTER BOTH INSERTIONS:
// bucket[2]: [(1,1)] -> [(1,1)]   <- linked chain!
//
// WHEN SEARCHING for (1,1):
// hash -> bucket[2], scan chain
// find first (1,1) -> return it
// find() always returns first occurrence
// count() traverses all to count matches

// ============================================================
// SECTION 8: COLLISION vs DUPLICATE
// ============================================================

// COLLISION: two DIFFERENT elements with same hash
// Custom(1,9): hash = 10
// Custom(9,1): hash = 10  <- same hash!
// operator==: (1,9)==(9,1) -> FALSE (they're different)
// -> stored in same bucket, different elements
//
// DUPLICATE: two IDENTICAL elements
// Custom(5,5): hash = 10
// Custom(5,5): hash = 10  <- same hash!
// operator==: (5,5)==(5,5) -> TRUE (they're same)
// -> unordered_SET: REJECT (already exists)
// -> unordered_MULTISET: ACCEPT (duplicates allowed)
//
// VISUAL:
// bucket[10]: [(1,9)] -> [(9,1)] -> [(5,5)] -> [(5,5)] -> nullptr
//              ^collision^          ^--- duplicates ---^

// ============================================================
// SECTION 9: IMPORTANT METHODS
// ============================================================

// Same as unordered_set PLUS:
// ms.count(val)         -> actual count of val O(1) avg
// ms.equal_range(val)   -> range of all val occurrences O(1) avg
//
// ms.erase(val)         -> removes ALL occurrences O(count)
// ms.erase(it)          -> removes ONE at iterator O(1)
//
// BUCKET API (for debugging/understanding):
// ms.bucket_count()     -> total buckets
// ms.bucket(val)        -> which bucket val would go to
// ms.bucket_size(i)     -> elements in bucket i
// ms.begin(i)           -> iterator to start of bucket i
// ms.end(i)             -> iterator to end of bucket i
// ms.load_factor()      -> size / bucket_count
// ms.max_load_factor()  -> threshold before rehash (default 1.0)
// ms.reserve(n)         -> hint: prepare for n elements

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does unordered_multiset provide over unordered_set?
//     -> Allows duplicate elements. unordered_set rejects re-insertions.
//        unordered_multiset stores them all. count() returns actual
//        frequency, not just 0 or 1.

// Q2: What two things must a custom type provide for unordered_multiset?
//     -> 1. operator==: for equality check (collision resolution + lookup)
//        2. Hash function: size_t operator()(const T&) const noexcept
//           Either as std::hash<T> specialization or struct passed as
//           template parameter.

// Q3: What is the difference between a hash collision and a duplicate?
//     -> Collision: different elements with same hash value.
//           hash(a) == hash(b) but a != b
//           Both stored, in same bucket chain.
//        Duplicate: identical elements (same hash AND operator==).
//           multiset accepts both, set only accepts the first.

// Q4: What should a good hash function guarantee?
//     -> 1. Consistency: same input always gives same output.
//        2. Speed: O(1) computation.
//        3. Distribution: spreads elements uniformly across buckets.
//        4. noexcept: hash functions should never throw exceptions.

// Q5: How does begin(i) differ from begin()?
//     -> begin(): iterator over ALL elements in the container.
//        begin(i): iterator over elements in BUCKET i only.
//        Used to inspect specific buckets (debugging, understanding
//        distribution, checking for clustering/collisions).

// Q6: Why are gen() and gen_duplicates() generating the same values?
//     -> Each function has its OWN static variables starting at 0.
//        Both increment before returning: (1,1),(2,2)...
//        operator== sees them as equal -> they ARE duplicates.

// Q7: What happens to load_factor after reserve()?
//     -> reserve(n) ensures bucket_count >= n/max_load_factor.
//        Prevents rehashing for up to n elements.
//        load_factor decreases (more buckets, same elements).

// Q8: When would you use unordered_multiset over multiset?
//     -> When order doesn't matter and you want O(1) avg instead of
//        O(log n). Frequency counting on large unsorted datasets.
//        If you need sorted order or range queries: use multiset.
// ```

// **The four set variants - complete picture:**

// ```
//                     UNIQUE          DUPLICATES ALLOWED
// SORTED          std::set           std::multiset
// UNSORTED        std::unordered_set std::unordered_multiset ← THIS
// ```

// **Collision vs Duplicate in hash table:**

// ```
// hash(1,9) = 10    hash(9,1) = 10    hash(5,5) = 10
// (1,9) != (9,1)    (5,5) == (5,5)

// bucket[10]: [(1,9)] → [(9,1)] → [(5,5)] → [(5,5)]
//              ←collision→         ←duplicates→

// unordered_set:      would have: (1,9), (9,1), (5,5)  [one of each]
// unordered_multiset: would have: (1,9), (9,1), (5,5), (5,5) [both (5,5)s]
// ```

// > **The custom type recipe**: for any custom type in unordered containers: (1) `operator==` to define what "same element" means, (2) hash function to define "which bucket" — these two together replace the `operator<` that `set`/`multiset` needs. The XOR-shift hash (`hash(a) ^ (hash(b) << 1)`) is the standard way to combine multiple fields.