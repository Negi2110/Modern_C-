#include <iostream>
#include <unordered_map>
#include <string>

// ============================================================
// std::unordered_multimap - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: THE FOUR MAP VARIANTS - COMPLETE PICTURE
// ============================================================

//                    UNIQUE KEYS        DUPLICATE KEYS
// SORTED (RB-Tree)   std::map           std::multimap
// UNSORTED (Hash)    std::unordered_map std::unordered_multimap ← THIS

// unordered_multimap = hash table + allows duplicate keys
// "unordered" = no sorted order (hash-based buckets)
// "multi"     = same key can appear multiple times
//
// YOUR COMMENT: "unordered_multimap needs:
//                1. hash function
//                2. operator=="
// CORRECT - same requirements as unordered_map for custom types

// ============================================================
// SECTION 2: COMPARISON OF ALL FOUR MAPS
// ============================================================

// | Feature              | map      | multimap | u_map    | u_multimap|
// |----------------------|----------|----------|----------|-----------|
// | Sorted               | YES      | YES      | NO       | NO        |
// | Duplicate keys       | NO       | YES      | NO       | YES       |
// | Find                 | O(log n) | O(log n) | O(1) avg | O(1) avg  |
// | Insert               | O(log n) | O(log n) | O(1) avg | O(1) avg  |
// | count(key)           | 0 or 1   | 0,1,2... | 0 or 1   | 0,1,2...  |
// | operator[]           | YES      | NO       | YES      | NO        |
// | at()                 | YES      | NO       | YES      | NO        |
// | equal_range          | YES      | YES      | YES      | YES       |
// | lower/upper_bound    | YES      | YES      | NO       | NO        |
//
// USE unordered_multimap WHEN:
// -> need FAST O(1) average operations
// -> need DUPLICATE KEYS (multiple values per key)
// -> order doesn't matter
// -> phone book: name -> multiple numbers (fast lookup)
// -> cache: request_hash -> multiple cached responses

// ============================================================
// SECTION 3: NO operator[] OR at() - WHY?
// ============================================================

// SAME REASON as multimap:
// mymap["s4"] = e1;  // WHICH s4? there are 3 of them!
//                    // AMBIGUOUS -> not provided
//
// ALTERNATIVES for inserting:
// mymap.insert({"s4", e1});  // always adds (even if s4 exists)
//
// ALTERNATIVES for accessing:
// mymap.find("s4")           // first occurrence only
// mymap.equal_range("s4")    // ALL occurrences (use this!)
// mymap.count("s4")          // how many s4 entries exist

// ============================================================
// SECTION 4: CUSTOM TYPE SETUP (same as unordered_map)
// ============================================================

struct Edge
{
    unsigned long v1;
    unsigned long v2;

    Edge() { v1 = -1; v2 = -1; }
    Edge(unsigned long _v1, unsigned long _v2) : v1(_v1), v2(_v2) {}

    bool operator==(const Edge& rhs) const {
        return (v1 == rhs.v1 && v2 == rhs.v2);
    }
    friend std::ostream& operator<<(std::ostream& os, const Edge& e);
};

std::ostream& operator<<(std::ostream& os, const Edge& e) {
    os << e.v1 << "," << e.v2;
    return os;
}

struct myhash {
    std::size_t operator()(const Edge& e) const noexcept {
        return e.v1 + e.v2;  // simple but causes collisions
    }
};

// ============================================================
// SECTION 5: IMPORTANT METHODS WITH COMPLEXITY
// ============================================================

// INSERT (always succeeds - no uniqueness check):
// mm.insert({key, val})      -> ALWAYS inserts               O(1) avg
//                               returns just iterator (no bool!)
//                               because it ALWAYS succeeds
//
// SEARCH:
// mm.find(key)               -> FIRST occurrence only         O(1) avg
// mm.count(key)              -> ACTUAL count of key           O(1) avg + count
// mm.contains(key)           -> bool (C++20)                  O(1) avg
// mm.equal_range(key)        -> pair{first, past_last}        O(1) avg
//                               YOUR CODE: range = mymap.equal_range("s4")
//                               USE THIS for all duplicates!
//
// NO lower_bound or upper_bound! (unordered = no range queries)
//
// ERASE:
// mm.erase(key)              -> removes ALL entries with key  O(count) avg
//                               YOUR CODE: mymap.erase("s1") removes ALL s1
// mm.erase(it)               -> removes ONE specific entry    O(1)
//                               YOUR CODE: mymap.erase(begin) removes one
//
// SIZE:
// mm.size()                  -> total ALL entries             O(1)
// mm.empty()                 -> true if empty                 O(1)
//
// HASH TABLE:
// mm.bucket_count()          -> number of buckets             O(1)
// mm.load_factor()           -> size/bucket_count             O(1)
// mm.reserve(n)              -> pre-allocate for n elements   O(n)

// ============================================================
// SECTION 6: main() TRACE
// ============================================================

int main()
{
    // CUSTOM KEY unordered_multimap:
    std::unordered_multimap<Edge, std::string, myhash> mymap2;
    Edge edgeTest(100, 100);
    mymap2.insert({edgeTest, "edgeTest"});

    Edge e1(1, 2);
    Edge e2(5, 4);
    Edge e3(3, 3);

    using StringEdge = std::unordered_multimap<std::string, Edge>;

    StringEdge mymap{{"s1",e1}, {"s2",e2}, {"s3",e3}};
    // stored in hash buckets, order unpredictable
    // all unique keys so far, but multimap allows duplicates

    mymap.insert({"s4", e1});  // new key s4
    // s4 is new, inserts fine
    // NOW: s1,s2,s3,s4 (all once)

    mymap.erase("s1");
    // removes ALL entries with key "s1" (just one here)
    // returns number of removed entries (1)

    StringEdge::iterator begin = mymap.begin();
    mymap.erase(begin);
    // removes ONE entry at begin iterator
    // which key? UNPREDICTABLE (hash order)!
    // could be s2, s3, or s4

    Edge e4(99, 100);

    // INSERT DUPLICATES (the multimap feature!):
    mymap.insert({"s2", e4});    // s2 appears TWICE now
    mymap.insert({"s3", e4});    // s3 appears TWICE now
    mymap.insert({"s4", e3});    // s4 appears TWICE now
    mymap.insert({"s4", e2});    // s4 appears THREE times
    mymap.insert({"s4", e4});    // s4 appears FOUR times
    // (minus any that were erased by erase(begin))

    std::cout << "number of s4: " << mymap.count("s4") << '\n';
    // count("s4") = actual number of s4 entries
    // depends on whether begin erased s4 or not

    // ACCESS ALL s4 ENTRIES with equal_range:
    auto range = mymap.equal_range("s4");
    auto start = range.first;    // iterator to first s4
    auto end   = range.second;   // iterator past last s4

    for (; start != end; start++) {
        std::cout << start->first << "|" << start->second << '\n';
    }
    // prints ALL s4 entries (in bucket order, not insertion order)

    // C++17 STRUCTURED BINDING VERSION (cleaner):
    // auto [lo, hi] = mymap.equal_range("s4");
    // for (auto it = lo; it != hi; ++it) {
    //     cout << it->first << "|" << it->second;
    // }

    mymap.erase("s1");  // "s1" already erased, returns 0 (safe!)

    // EXISTENCE CHECKS:
    if (mymap.contains("s5")) {       // C++20
        std::cout << "s5 exists\n";   // won't print (s5 not inserted)
    }

    if (mymap.find("s6") != mymap.end()) {
        std::cout << "We contain s6\n"; // won't print
    }

    // ITERATE ALL (order unpredictable):
    for (const auto& [key, value] : mymap) {
        std::cout << key << "|" << value << '\n';
    }
    // might see: s4|99,100  s4|5,4  s2|1,2  s2|99,100 etc.
    // NOT alphabetical! hash order

    return 0;
}

// ============================================================
// SECTION 7: erase() BEHAVIOR COMPARISON
// ============================================================

// unordered_multimap<string, int> mm;
// mm.insert({"a",1}); mm.insert({"a",2}); mm.insert({"a",3});
// mm.insert({"b",4});
//
// mm.erase("a"):
// -> removes ALL three "a" entries
// -> mm now only has {"b",4}
// -> returns 3 (count removed)
//
// mm.erase(mm.find("a")):
// -> find("a") = iterator to ONE "a" (whichever hash places first)
// -> erase(iterator) removes ONLY THAT ONE
// -> mm still has two "a" entries + "b"
// -> returns void (not count!)
//
// CHOOSE BASED ON INTENT:
// remove ALL with key:  erase(key)
// remove ONE specific:  erase(find(key)) or erase(equal_range.first)

// ============================================================
// SECTION 8: equal_range - THE ESSENTIAL METHOD
// ============================================================

// equal_range("s4") returns pair<iterator, iterator>
// first  = iterator to FIRST s4 entry
// second = iterator to element AFTER LAST s4 entry
//
// VISUAL:
// buckets: [...] [s4|e1] [s4|e2] [s4|e4] [...] [s2|e2] [...]
//                  ^                       ^
//              range.first            range.second
//
// NOTE: within equal_range, order of duplicates is NOT guaranteed
// (hash bucket order, not insertion order)
//
// WHY prefer equal_range over find() loop:
// find() + manual: must check key != "s4" each iteration
// equal_range:     clean range, no check needed
//
// EFFICIENCY: O(1) avg single call vs find() O(1) + manual traverse

// ============================================================
// SECTION 9: DSA PATTERNS
// ============================================================

// PATTERN 1: Fast frequency count with all occurrences
// std::unordered_multimap<int, int> mm;
// for (int n : nums) mm.insert({n, index++});
// auto [lo,hi] = mm.equal_range(target);
// int count = mm.count(target);  // O(1) avg

// PATTERN 2: Grouping without sorting (vs multimap which sorts)
// std::unordered_multimap<string, int> groups;
// groups.insert({"even", 2}); groups.insert({"even", 4});
// groups.insert({"odd", 1});  groups.insert({"odd", 3});
// auto [lo,hi] = groups.equal_range("even"); // O(1) get all evens

// PATTERN 3: Cache with multiple responses per key
// std::unordered_multimap<string, Response> cache;
// cache.insert({url, response});
// auto cnt = cache.count(url);  // O(1) how many cached for this url

// PATTERN 4: Inverted index (fast word lookup)
// std::unordered_multimap<string, int> wordIndex; // {word, lineNum}
// wordIndex.insert({"hello", 5});
// wordIndex.insert({"hello", 23}); // same word, different line
// auto [lo,hi] = wordIndex.equal_range("hello"); // all line numbers

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What are the four map variants and when to use each?
//     -> map:               sorted, unique keys, O(log n)
//        multimap:          sorted, duplicate keys, O(log n)
//        unordered_map:     unsorted, unique keys, O(1) avg
//        unordered_multimap:unsorted, duplicate keys, O(1) avg
//        Choose sorted vs unsorted based on order needs.
//        Choose map vs multi based on key uniqueness needs.

// Q2: Why does unordered_multimap have no operator[] or at()?
//     -> Same key can have multiple values.
//        m["s4"] is ambiguous when "s4" has 3 entries.
//        Use equal_range("s4") to get all values for a key.

// Q3: What does erase(key) do in unordered_multimap?
//     -> Removes ALL entries with that key.
//        Returns count of removed entries.
//        erase(iterator) removes only ONE specific entry.

// Q4: What does insert() return in unordered_multimap?
//     -> Just an iterator (no bool!).
//        Insertion ALWAYS succeeds (no uniqueness check).
//        Unlike unordered_map where bool indicates if key was new.

// Q5: How do you access all values for a given key?
//     -> equal_range(key) returns pair<iterator, iterator>.
//        Iterate [first, second) to see all values.
//        Single O(1) avg call for both bounds.

// Q6: Is iteration order guaranteed in unordered_multimap?
//     -> NO. Neither overall order nor duplicate order is guaranteed.
//        Depends on hash function and bucket layout.
//        If order matters: use multimap instead.

// Q7: What is the difference between find() and equal_range()?
//     -> find(): returns iterator to ONE occurrence (first found).
//        equal_range(): returns range covering ALL occurrences.
//        Use equal_range when you need to process all duplicates.

// Q8: When would you use unordered_multimap over multimap?
//     -> Need faster O(1) avg lookup vs O(log n).
//        Order doesn't matter.
//        Large datasets where tree overhead is too slow.
// ```

// **All four maps - the complete picture:**

// ```
//              UNIQUE KEYS              DUPLICATE KEYS
// SORTED       std::map                 std::multimap
//              O(log n), sorted keys    O(log n), sorted + dupes

// UNSORTED     std::unordered_map       std::unordered_multimap
//              O(1) avg, no order       O(1) avg, no order, dupes allowed
// ```

// **equal_range - the only safe way to get all duplicates:**

// ```cpp
// // s4 has 3 entries in the map
// auto [lo, hi] = mymap.equal_range("s4");

// // lo → [s4|e1] → [s4|e2] → [s4|e4] → hi
// //        first duplicate              past-last

// for (auto it = lo; it != hi; ++it) {
//     cout << it->second;  // sees ALL three s4 values
// }

// // DON'T use find() + loop - must manually check key didn't change
// // DO use equal_range - clean bounds, no extra check needed
// ```

// > **The final map summary**: the four map variants form a 2x2 grid — sorted vs unsorted, unique vs duplicate. `unordered_multimap` occupies the "fastest + most flexible" corner — O(1) operations AND duplicate keys — at the cost of no ordering and needing `equal_range()` instead of `operator[]`. Choose it when you need to associate multiple values with the same key and need fast lookup.