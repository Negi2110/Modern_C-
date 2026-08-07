#include <iostream>
#include <string>
#include <map>
#include <ostream>

// ============================================================
// std::map - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::map<Key, Value, Compare> =
// sorted associative container storing KEY-VALUE pairs
// YOUR COMMENT: "associative and ordered container for storing keys and values"
// "implemented as Red-Black Tree"
// "logarithmic performance for insertion, removal, and updates"
//
// KEY PROPERTIES:
// -> keys are UNIQUE (no duplicate keys)
// -> keys are SORTED (by default: ascending with operator<)
// -> each key maps to EXACTLY ONE value
// -> O(log n) for all operations (tree height = O(log n))
// -> iterating gives pairs in SORTED KEY ORDER automatically
//
// INTERNAL: each node stores std::pair<const Key, Value>
//           Key is CONST (you can't change a key after insertion!)

// ============================================================
// SECTION 2: YOUR QUESTIONS ANSWERED
// ============================================================

// Q: "What is structured binding and does it come after C++17?"
// -> C++17 feature: auto [a, b] = some_pair_or_struct;
//    unpacks multiple values into named variables
//    const auto [it, success] = mymap.insert(...);
//    -> it = iterator to inserted/existing element
//    -> success = bool: true if inserted, false if key existed
//    Cleaner than: auto result = ...; result.first; result.second;

// Q: "What if we use auto instead of verbose iterator type?"
// -> auto it = mymap.begin(); is IDENTICAL in performance
//    compiler deduces the type at compile time (zero runtime cost)
//    BENEFITS of auto:
//    -> shorter code (less typing)
//    -> if you change map type, auto still works
//    -> less error-prone (can't accidentally write wrong type)
//    PREFER auto always for iterators in modern C++

// Q: "Which is better - range-based for or iterator loop?"
// -> PREFER range-based for almost always:
//    -> shorter, more readable
//    -> less error-prone (no manual ++it, no begin/end)
//    -> same performance (compiler generates identical code)
//    -> with structured bindings [key,val]: even cleaner
//    USE iterator loop when:
//    -> need to erase during iteration
//    -> need to call insert() during iteration
//    -> need position-based operations (advance, next, prev)

// ============================================================
// SECTION 3: CONSTRUCTION AND INITIALIZATION
// ============================================================

// std::map<string, int> m;                    // empty
// std::map<string, int> m = {{"a",1},{"b",2}};// initializer list
// std::map<string, int> m{{"a",1},{"b",2}};   // same
// std::map<string, int> m2(m);                // copy
//
// TYPE ALIAS (your code):
// using StringEdge = std::map<std::string, Edge>;
// -> then use StringEdge::iterator instead of full type
// -> same readability benefit as auto

// ============================================================
// SECTION 4: IMPORTANT METHODS WITH COMPLEXITY
// ============================================================

// INSERT / UPDATE:
// m[key] = value         -> insert OR update               O(log n)
//                           IF key exists: OVERWRITES value
//                           IF key new: inserts new pair
//                           CREATES default value if key didn't exist!
//                           -> m["edge1"] creates Edge() first time!
//
// m.insert({key, val})   -> insert ONLY if key not present  O(log n)
//                           returns pair<iterator, bool>
//                           bool = true if inserted
//
// m.insert_or_assign(k,v)-> insert OR overwrite (C++17)     O(log n)
//                           unlike m[k]=v: doesn't default-construct
//
// m.emplace(key, val)    -> in-place construct               O(log n)
// m.try_emplace(k,args)  -> insert only if not present (C++17) O(log n)
//
// ACCESS:
// m[key]                 -> reference (creates if missing!)  O(log n)
// m.at(key)              -> reference, throws if missing     O(log n)
//                           YOUR CODE: mymap.at("edge1")
//                           PREFER at() over [] for READ access!
//                           [] creates entries accidentally
//
// SEARCH:
// m.find(key)            -> iterator or end()                O(log n)
// m.contains(key)        -> bool (C++20)                     O(log n)
// m.count(key)           -> 0 or 1 (unique keys)             O(log n)
//
// REMOVE:
// m.erase(key)           -> remove by key                    O(log n)
// m.erase(it)            -> remove by iterator               O(1) amort
// m.erase(it1, it2)      -> remove range                     O(n)
// m.clear()              -> remove all                       O(n)
//
// SIZE:
// m.size()               -> element count                    O(1)
// m.empty()              -> true if empty                    O(1)
//
// RANGE QUERIES:
// m.lower_bound(key)     -> first element >= key             O(log n)
// m.upper_bound(key)     -> first element > key              O(log n)
// m.equal_range(key)     -> pair of above                    O(log n)

// ============================================================
// SECTION 5: operator[] DANGER - IMPORTANT
// ============================================================

// m["edge1"] = Edge(1,1);  // OK: assignment after access
//
// BUT:
// std::cout << m["edge99"];  // DANGER!
// -> "edge99" doesn't exist
// -> operator[] DEFAULT CONSTRUCTS Edge() and inserts it!
// -> m now has edge99 with garbage values
// -> size increased by 1 accidentally!
//
// YOUR CODE SHOWS THIS:
// mymap["edge1"] = Edge(0, 0);  // first: creates with Edge(0,0)
// mymap["edge1"] = Edge(1, 1);  // second: OVERWRITES with Edge(1,1)
// -> [] allows easy overwrite (by design)
//
// SAFE ALTERNATIVE for reading:
// m.at("edge99")       // throws std::out_of_range (no accidental create)
// m.find("edge99")     // returns end() (no accidental create)
// m.contains("edge99") // bool (no accidental create)
//
// RULE: use [] only for INSERT/UPDATE
//       use at() or find() for READ

// ============================================================
// SECTION 6: FOUR ITERATION STYLES (your code shows all four)
// ============================================================

// STYLE 1: Classic iterator (verbose):
// for (std::map<std::string, Edge>::iterator it = mymap.begin();
//      it != mymap.end(); ++it) {
//     it->first   // key
//     it->second  // value
// }

// STYLE 2: Auto iterator (better):
// for (auto it = mymap.begin(); it != mymap.end(); ++it) {
//     it->first   // key
//     it->second  // value
// }

// STYLE 3: Range-based for (preferred):
// for (const auto& it : mymap) {
//     it.first   // key (dot not arrow!)
//     it.second  // value
// }

// STYLE 4: Range-based + structured binding (BEST, C++17):
// for (const auto& [key, val] : mymap) {
//     key  // key directly
//     val  // value directly
// }
// -> cleanest, most readable, compiler generates identical code

// ============================================================
// SECTION 7: CUSTOM KEY TYPE WITH COMPARATOR
// ============================================================

struct Edge
{
    unsigned long v1;
    unsigned long v2;

    Edge() { v1 = -1; v2 = -1; }  // default constructor
    Edge(unsigned long _v1, unsigned long _v2) : v1(_v1), v2(_v2) {}

    // operator<< for printing (friend = can access privates)
    friend std::ostream& operator<<(std::ostream& os, const Edge& e);
};

std::ostream& operator<<(std::ostream& os, const Edge& e)
{
    os << e.v1 << "," << e.v2;
    return os;
}

// COMPARATOR for Edge as KEY:
struct EdgeCompare
{
    bool operator()(const Edge& lhs, const Edge& rhs) const
    {
        return lhs.v1 + lhs.v2 < rhs.v1 + rhs.v2;
        // sorts edges by sum of vertices
        // e(0,8)=8 < e(8,8)=16
    }
};
// std::map<Edge, string, EdgeCompare> mymap2;
// -> uses EdgeCompare to sort Edge keys
// -> WITHOUT EdgeCompare: needs Edge::operator< (which doesn't exist here)

// ============================================================
// SECTION 8: insert() RETURN VALUE - STRUCTURED BINDING
// ============================================================

// const auto [it, success] = mymap.insert(std::pair("edge3", Edge(3,3)));
//
// insert() returns: std::pair<iterator, bool>
// iterator -> points to the element (inserted OR existing)
// bool     -> true if NEWLY inserted, false if key ALREADY EXISTED
//
// C++17 STRUCTURED BINDING unpacks this:
// [it, success] -> it = iterator, success = bool
//
// it->first  = "edge3" (the key)
// it->second = Edge(3,3) (the value)
// success    = true (was inserted)
//
// IF KEY EXISTS:
// auto [it2, ok] = mymap.insert({"edge3", Edge(99,99)});
// ok = false (edge3 already in map!)
// it2->second = Edge(3,3) (ORIGINAL value, NOT updated!)
// -> insert() NEVER overwrites, use [] or insert_or_assign for that

// ============================================================
// SECTION 9: DSA PATTERNS WITH map
// ============================================================

// PATTERN 1: Frequency count (most common)
// std::map<char, int> freq;
// for (char c : str) freq[c]++;  // [] creates with 0 if missing
// // sorted by char: freq['a'], freq['b']...

// PATTERN 2: Ordered lookup table
// std::map<int, string> levels{{1,"easy"},{2,"medium"},{3,"hard"}};
// levels[2] = "medium";  // O(log n) lookup

// PATTERN 3: Graph adjacency with weights
// std::map<int, std::map<int, int>> adj;
// adj[u][v] = weight;  // edge u->v with weight
// adj[u][v]            // O(log n) lookup

// PATTERN 4: Range query (sorted keys)
// std::map<int, string> m{{1,"a"},{3,"c"},{5,"e"},{7,"g"}};
// auto lo = m.lower_bound(3);   // >= 3
// auto hi = m.upper_bound(5);   // > 5
// for (auto it=lo; it!=hi; ++it) // {3,"c"},{5,"e"}

// PATTERN 5: Check and insert pattern
// if (!m.contains(key)) {
//     m[key] = value;
// }
// OR cleaner:
// auto [it, inserted] = m.insert({key, value});
// if (inserted) { /* handle new insertion */ }

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::map and how is it implemented?
//     -> Sorted key-value container. Red-Black Tree internally.
//        Unique keys, sorted by key. O(log n) all operations.

// Q2: What is the danger of using operator[] for reading?
//     -> If key doesn't exist, [] DEFAULT CONSTRUCTS and INSERTS it!
//        m["missing"] creates an entry. Use at() (throws) or
//        find() (returns end()) for safe read access.

// Q3: What does insert() return and how does structured binding help?
//     -> pair<iterator, bool>. iterator = position, bool = was it new?
//        auto [it, ok] = m.insert({k,v}); cleaner than .first/.second

// Q4: What is the difference between m[key]=val and m.insert({key,val})?
//     -> m[key]=val: ALWAYS sets value (overwrites if exists)
//        m.insert(): only inserts if key NOT present (never overwrites)
//        insert_or_assign(): inserts or overwrites (C++17, like [])

// Q5: Which iteration style is preferred for map?
//     -> Range-based for with structured binding (C++17):
//        for (const auto& [key, val] : m) { }
//        Same performance, most readable.

// Q6: How do you use a custom type as a map key?
//     -> Option 1: define operator< in the type.
//        Option 2: provide comparator as third template param:
//        std::map<Edge, string, EdgeCompare> m;

// Q7: How is std::map different from std::unordered_map?
//     -> map: sorted (RB-Tree), O(log n) guaranteed, supports range queries.
//        unordered_map: unsorted (hash), O(1) avg, no range queries.
//        Use map when you need order; unordered_map for faster lookup.

// Q8: What happens when you iterate a map - in what order?
//     -> Always SORTED KEY ORDER (ascending by default).
//        This is the in-order traversal of the underlying RB-Tree.
//        Guaranteed by the standard.

int main()
{
    std::map<std::string, Edge> mymap;
    using StringEdge = std::map<std::string, Edge>;

    // [] creates entry with default Edge() if not present:
    mymap["edge1"] = Edge(0, 0);   // insert edge1
    mymap["edge7"] = Edge(7, 7);   // insert edge7
    mymap["edge1"] = Edge(1, 1);   // OVERWRITE edge1 ([] overwrites)

    // at() for safe read (throws if missing):
    std::cout << mymap.at("edge1") << '\n';  // 1,1

    // insert() - only inserts if key not present:
    mymap.insert(std::pair("edge2", Edge(2L, 2L)));
    std::cout << mymap.at("edge2") << '\n';  // 2,2

    // structured binding from insert():
    const auto [it, success] = mymap.insert(std::pair("edge3", Edge(3,3)));
    std::cout << "edge3 added: " << success << '\n';  // 1 (true)
    std::cout << it->first << " success:" << success << '\n';

    // STYLE 1: verbose iterator
    for (std::map<std::string, Edge>::iterator it = mymap.begin();
         it != mymap.end(); ++it) {
        std::cout << it->first << ":" << it->second << '\n';
    }

    // STYLE 2: using type alias
    for (StringEdge::iterator it = mymap.begin(); it != mymap.end(); ++it) {
        std::cout << it->first << ":" << it->second << '\n';
    }

    // STYLE 3: range-based for
    for (const auto& it : mymap) {
        std::cout << it.first << ":" << it.second << '\n';
    }

    // erase by key:
    if (mymap.contains("edge3")) {
        mymap.erase("edge3");
    }

    // STYLE 4: structured binding (BEST - C++17):
    for (const auto& [key, val] : mymap) {
        std::cout << key << ":" << val.v1 << "," << val.v2 << '\n';
    }
    // Output is SORTED by key: edge1, edge2, edge7

    // CUSTOM KEY TYPE with comparator:
    std::map<Edge, std::string, EdgeCompare> mymap2;
    Edge e1(8, 8);  // sum = 16
    Edge e2(0, 8);  // sum = 8

    mymap2[e1] = "test";
    mymap2[e2] = "test2";

    // Printed in order of sum: e2(sum=8) before e1(sum=16)
    for (const auto& [key, val] : mymap2) {
        std::cout << key << ":" << val << '\n';
    }
    // 0,8:test2
    // 8,8:test

    return 0;
}
// ```

// **The operator[] trap:**

// ```cpp
// std::map<string, int> m;

// // WRITE - fine:
// m["key"] = 42;         // inserts or overwrites

// // READ - DANGEROUS:
// cout << m["missing"];   // INSERTS "missing" with value 0!
//                         // m.size() just increased by 1 accidentally!

// // SAFE READ:
// m.at("missing")         // throws std::out_of_range
// m.find("missing")       // returns end() (no insertion)
// m.contains("missing")   // returns false (no insertion)
// ```

// **Four iteration styles:**

// ```cpp
// // VERBOSE (avoid):
// for (std::map<string,Edge>::iterator it = m.begin(); it != m.end(); ++it)

// // AUTO (better):
// for (auto it = m.begin(); it != m.end(); ++it)

// // RANGE-BASED (good):
// for (const auto& [key, val] : m)   // ← PREFER THIS (C++17)

// // All produce SORTED output by key. Same performance.
// ```

// > **The map vs unordered_map decision**: if you need to iterate keys in order, use `lower_bound`/`upper_bound` range queries, or need **guaranteed O(log n)** (no hash collision risk) → `map`. If you only need fast lookup and order doesn't matter → `unordered_map` at O(1) average. In practice, `unordered_map` wins for most DSA problems; `map` wins when sorted order matters.