#include <iostream>
#include <string>
#include <map>
#include <utility>

// ============================================================
// std::multimap - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::multimap<Key, Value> = sorted associative container
// SAME as std::map BUT allows DUPLICATE KEYS
//
// YOUR COMMENT: "diff between map is we can have duplicates"
// "red-black tree (logarithmic operations)"
//
// map:      each key is UNIQUE -> "mike" can only appear ONCE
// multimap: keys can REPEAT  -> "mike" can appear MANY times
//
// YOUR EXAMPLE:
// "mike" -> 2
// "mike" -> 7
// "mike" -> 100
// "mike" -> 101   ALL valid in multimap!
//
// REAL WORLD USE CASES:
// -> phone book: same name, multiple numbers
// -> YOUR COMMENT: "5/1123" date -> multiple events same day
// -> DNS: one domain -> multiple IP addresses
// -> Student grades: one student -> multiple test scores
// -> Index: one word -> multiple page numbers (like a book index)

// ============================================================
// SECTION 2: YOUR QUESTION - HOW ARE ENTRIES SORTED?
// ============================================================

// "how entries get sorted during insertion in multimap
//  based on string or int?"
//
// ANSWER: sorted by KEY only (first element = string)
// -> values (int) have NO effect on ordering
// -> keys sorted using operator< (for string: lexicographic/alphabetical)
//
// YOUR INSERTIONS:
// {"mike",2}, {"mike",7}, {"mike",100}, {"tomoki",107},
// {"mike",101}, {"mike",102}, {"nick",107}, {"mike",103}
//
// STORED ORDER (sorted by KEY string):
// "mike",2      <- mike comes before nick, tomoki alphabetically
// "mike",7
// "mike",100
// "mike",101
// "mike",102
// "mike",103
// "nick",107    <- n comes after m
// "tomoki",107  <- t comes after n
//
// WITHIN SAME KEY (multiple "mike" entries):
// ORDER = ORDER OF INSERTION (stable within same key)
// -> {mike,2} inserted first -> appears first among mikes
// -> {mike,7} inserted second -> appears second
// -> etc.
//
// ALPHABETICAL ORDER: strings compared char by char
// "mike" < "nick" < "tomoki"  (m < n < t)

// ============================================================
// SECTION 3: map vs multimap - KEY DIFFERENCE
// ============================================================

// | Feature            | map                | multimap           |
// |--------------------|--------------------|--------------------|
// | Duplicate keys     | NO                 | YES                |
// | operator[]         | YES (creates entry)| NO! (ambiguous)    |
// | at()               | YES                | NO! (ambiguous)    |
// | find(key)          | unique result      | first occurrence   |
// | count(key)         | 0 or 1             | 0, 1, 2, 3...      |
// | insert()           | pair<it,bool>      | just iterator      |
// | erase(key)         | removes 1          | removes ALL        |
// | erase(iterator)    | removes 1          | removes 1          |
//
// WHY NO operator[] in multimap?
// m["mike"] = 5;  <- which "mike"? there are 6 of them!
// AMBIGUOUS -> not provided by design
// Use insert() to add, find()/equal_range() to access

// ============================================================
// SECTION 4: IMPORTANT METHODS WITH COMPLEXITY
// ============================================================

// INSERT:
// mm.insert({key, val})      -> always inserts (no uniqueness check) O(log n)
//                               returns JUST iterator (no bool!)
//                               because insertion ALWAYS succeeds
//
// SEARCH:
// mm.find(key)               -> iterator to FIRST occurrence O(log n)
// mm.count(key)              -> total count of key           O(log n + count)
// mm.contains(key)           -> bool (C++20)                 O(log n)
// mm.lower_bound(key)        -> first element >= key         O(log n)
// mm.upper_bound(key)        -> first element > key          O(log n)
// mm.equal_range(key)        -> pair{lower_bound, upper_bound} O(log n)
//                               YOUR CODE: start_end = equal_range("mike")
//                               MOST EFFICIENT: single tree traversal!
//
// ERASE:
// mm.erase(key)              -> removes ALL entries with key  O(log n + count)
//                               YOUR CODE (commented): erase("mike") = removes all mikes
// mm.erase(iterator)         -> removes ONE specific entry    O(1) amort
//                               YOUR CODE: mymultimap.erase(f) = removes first mike
//
// SIZE:
// mm.size()                  -> total element count (all entries) O(1)
// mm.empty()                 -> true if empty                O(1)

// ============================================================
// SECTION 5: THREE WAYS TO ACCESS DUPLICATE KEYS
// ============================================================

// METHOD 1: find() + manual iteration
// auto it = mm.find("mike");
// while (it != mm.end() && it->first == "mike") {
//     cout << it->second;
//     ++it;
// }
// PROBLEM: must manually check key hasn't changed (verbose)

// METHOD 2: lower_bound + upper_bound (YOUR CODE)
// auto first = mm.lower_bound("mike");  // first "mike" O(log n)
// auto last  = mm.upper_bound("mike");  // past last "mike" O(log n)
// for (; first != last; ++first) {      // iterate range
//     cout << first->second;
// }
// TWO separate tree traversals (less efficient)

// METHOD 3: equal_range (YOUR CODE - most efficient)
// auto [lo, hi] = mm.equal_range("mike");  // ONE tree traversal O(log n)
// for (auto it = lo; it != hi; ++it) {
//     cout << it->second;
// }
// BEST: single O(log n) operation returns BOTH bounds
// C++17 structured binding makes it even cleaner!

// ============================================================
// SECTION 6: main() TRACE
// ============================================================

int main()
{
    using stringIntMultiMap = std::multimap<std::string, int>;

    std::multimap<std::string, int> mymultimap = {{"mike",2}, {"mike",7}};
    // stored: {mike,2}, {mike,7}

    mymultimap.insert(std::pair{"mike",   100});
    mymultimap.insert(std::pair{"tomoki", 107});
    mymultimap.insert(std::pair{"mike",   101});
    mymultimap.insert(std::pair{"mike",   102});
    mymultimap.insert(std::pair{"nick",   107});
    mymultimap.insert(std::pair{"mike",   103});

    // SORTED STATE:
    // mike:2, mike:7, mike:100, mike:101, mike:102, mike:103
    // nick:107
    // tomoki:107

    // FIND (returns iterator to FIRST "mike"):
    stringIntMultiMap::iterator f = mymultimap.find("mike");
    // f points to {mike, 2} (first inserted "mike")

    std::cout << mymultimap.count("mike") << '\n';
    // count = 6 (six mike entries)

    mymultimap.erase(f);
    // erases ONLY the first mike (the one f points to = {mike,2})
    // NOT all mikes!

    std::cout << mymultimap.count("mike") << '\n';
    // count = 5 (one mike was removed)

    // mymultimap.erase("mike");  // would remove ALL mikes!

    // ITERATE ALL (sorted order):
    for (const auto& elements : mymultimap) {
        std::cout << elements.first << "," << elements.second << '\n';
    }
    // mike,7   mike,100  mike,101  mike,102  mike,103
    // nick,107
    // tomoki,107

    std::cout << "=========================\n";

    // METHOD 2: lower/upper bound (two traversals):
    auto firstEntry = mymultimap.lower_bound("mike"); // first >= "mike"
    auto lastEntry  = mymultimap.upper_bound("mike"); // first > "mike"
    for (; firstEntry != lastEntry; ++firstEntry) {
        std::cout << firstEntry->first << ","
                  << firstEntry->second << '\n';
    }
    // mike,7  mike,100  mike,101  mike,102  mike,103

    // METHOD 3: equal_range (ONE traversal - MORE EFFICIENT):
    auto start_end = mymultimap.equal_range("mike");
    // start_end.first  = lower_bound("mike")
    // start_end.second = upper_bound("mike")
    // single O(log n) call vs two O(log n) calls above

    for (auto it = start_end.first; it != start_end.second; ++it) {
        std::cout << it->first << "," << it->second << '\n';
    }
    // same output as above

    // C++17 STRUCTURED BINDING version (cleaner):
    auto [lo, hi] = mymultimap.equal_range("mike");
    for (auto it = lo; it != hi; ++it) {
        std::cout << it->second << '\n';  // just the values
    }

    return 0;
}

// ============================================================
// SECTION 7: DSA PATTERNS WITH MULTIMAP
// ============================================================

// PATTERN 1: Group by key (most common use)
// std::multimap<int, string> byScore;
// byScore.insert({90, "alice"});
// byScore.insert({90, "bob"});    // same score, different student
// byScore.insert({85, "charlie"});
// // iterate: grouped and sorted by score

// PATTERN 2: Event calendar (date -> events)
// std::multimap<string, string> calendar;
// calendar.insert({"2024-01-15", "Meeting"});
// calendar.insert({"2024-01-15", "Lunch"});  // same date!
// calendar.insert({"2024-01-16", "Conference"});
// auto [start, end] = calendar.equal_range("2024-01-15");
// // all events on Jan 15

// PATTERN 3: Inverted index (word -> page numbers)
// std::multimap<string, int> index;
// index.insert({"algorithm", 5});
// index.insert({"algorithm", 23});  // appears on two pages
// index.insert({"algorithm", 67});
// auto pages = index.equal_range("algorithm"); // all pages

// PATTERN 4: Top K frequent elements
// std::multimap<int, string> freqMap; // {count, word}
// for (auto& [word, count] : wordFreq)
//     freqMap.insert({count, word});
// // last k entries = top k (sorted by frequency)

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between map and multimap?
//     -> map: unique keys, has operator[] and at().
//        multimap: duplicate keys allowed, NO operator[] or at().
//        Both sorted by key, both O(log n) operations.

// Q2: Why doesn't multimap have operator[]?
//     -> Ambiguous: if "mike" appears 6 times, which one does
//        m["mike"] refer to? Cannot be resolved, so not provided.
//        Use find() or equal_range() to access specific entries.

// Q3: How are entries sorted in multimap with duplicate keys?
//     -> Sorted by KEY using operator< (for string: alphabetical).
//        Within the SAME KEY: entries appear in INSERTION ORDER (stable).
//        Values have no effect on ordering.

// Q4: What does erase(key) do in multimap?
//     -> Removes ALL entries with that key.
//        To remove ONE specific entry: use erase(iterator).
//        find() to get iterator to first occurrence, then erase(it).

// Q5: What is the difference between the three "mike" access methods?
//     -> find() + manual: verbose, must manually check key hasn't changed.
//        lower_bound + upper_bound: two separate O(log n) tree traversals.
//        equal_range: SINGLE O(log n) traversal, returns both bounds.
//        equal_range is most efficient and recommended.

// Q6: What does insert() return in multimap vs map?
//     -> map:      pair<iterator, bool> (bool = was it new?)
//        multimap: just iterator (always succeeds, no uniqueness check)
//        Because multimap ALWAYS inserts successfully.

// Q7: What does count() return in multimap?
//     -> Actual count of entries with that key: 0, 1, 2, 3...
//        Unlike map where count() returns only 0 or 1.

// Q8: When would you use multimap over multiple maps?
//     -> When you need SORTED access to all values of a key AND
//        grouped iteration over all entries of a key efficiently.
//        Multiple maps require maintaining synchronization manually.
//        Multimap keeps everything in one sorted structure.
// ```

// **The sorting answer to your question:**

// ```
// Insertions:
// {"mike",2}, {"mike",7}, {"tomoki",107},
// {"mike",101}, {"nick",107}, {"mike",103}

// SORTED BY KEY (string, alphabetical):
// mike   → 2      ← m comes before n and t
// mike   → 7      ← insertion order preserved within same key
// mike   → 101
// mike   → 103
// nick   → 107    ← n comes after m
// tomoki → 107    ← t comes after n

// RULE: sorted by KEY (string), values don't affect order
//       within same key: insertion order preserved
// ```

// **Three ways to get all "mike" entries:**

// ```cpp
// // METHOD 1: find + manual (verbose):
// auto it = mm.find("mike");
// while (it != mm.end() && it->first == "mike") { ++it; }

// // METHOD 2: lower + upper bound (2 tree traversals):
// auto lo = mm.lower_bound("mike");   // O(log n)
// auto hi = mm.upper_bound("mike");   // O(log n) again!

// // METHOD 3: equal_range (1 tree traversal - BEST):
// auto [lo, hi] = mm.equal_range("mike");  // O(log n) ONCE
// ```

// > **The one rule for multimap**: always use `equal_range()` to access all entries of a key — it's a single O(log n) operation that returns both the start and end iterators. Using `lower_bound()` + `upper_bound()` separately does two tree traversals for the same result.