#include <iostream>
#include <unordered_map>
#include <string>

// ============================================================
// std::unordered_map - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::unordered_map<Key, Value, Hash, KeyEqual> =
// hash table storing UNIQUE key-value pairs, NO ordering
//
// YOUR COMMENT: "offers average-case constant time insertion,
//                lookup, and deletion"
// "elements are NOT sorted, organized into BUCKETS"
// "underlying data structure is a hash table"
//
// map vs unordered_map:
// map:          Red-Black Tree, O(log n), SORTED keys
// unordered_map: Hash Table,   O(1) avg, NO order
//
// INTERNAL: same bucket structure as unordered_set
// bucket[hash(key) % bucket_count] -> chain of {key,value} pairs

// ============================================================
// SECTION 2: map vs unordered_map - WHEN TO USE WHICH
// ============================================================

// | Feature              | map        | unordered_map    |
// |----------------------|------------|------------------|
// | Order                | sorted     | NO order         |
// | Implementation       | RB-Tree    | Hash Table       |
// | Find                 | O(log n)   | O(1) avg         |
// | Insert               | O(log n)   | O(1) avg         |
// | Delete               | O(log n)   | O(1) avg         |
// | Worst case           | O(log n)   | O(n) collisions  |
// | lower/upper_bound    | YES        | NO               |
// | Sorted iteration     | YES        | NO               |
// | Memory               | tree nodes | buckets+chains   |
// | Custom key           | need <     | need == AND hash |
//
// USE unordered_map WHEN:
// -> frequent lookup/insert/delete (O(1) avg vs O(log n))
// -> order doesn't matter
// -> key is string, int, or type with good hash
// -> most DSA problems (frequency count, memoization, caching)
//
// USE map WHEN:
// -> need sorted iteration
// -> need range queries (lower_bound/upper_bound)
// -> need guaranteed O(log n) (no hash collision risk)

// ============================================================
// SECTION 3: CUSTOM KEY TYPE REQUIREMENTS
// ============================================================

struct Edge
{
    unsigned long v1;
    unsigned long v2;

    Edge() { v1 = -1; v2 = -1; }
    Edge(unsigned long _v1, unsigned long _v2) : v1(_v1), v2(_v2) {}

    // REQUIREMENT 1: operator== (for collision resolution)
    // "are two keys the SAME key?"
    bool operator==(const Edge& rhs) const {
        return (v1 == rhs.v1 && v2 == rhs.v2);
    }

    friend std::ostream& operator<<(std::ostream& os, const Edge& e);
};

std::ostream& operator<<(std::ostream& os, const Edge& e)
{
    os << e.v1 << "," << e.v2;
    return os;
}

// REQUIREMENT 2: hash function
// "which bucket does this key go in?"
struct myhash
{
    std::size_t operator()(const Edge& e) const noexcept
    {
        return e.v1 + e.v2;
        // SIMPLE but not great: Edge(1,9) and Edge(9,1) hash to 10
        //                        causes collisions -> O(n) worst case
        //
        // BETTER hash (XOR-shift):
        // size_t h1 = std::hash<unsigned long>{}(e.v1);
        // size_t h2 = std::hash<unsigned long>{}(e.v2);
        // return h1 ^ (h2 << 1);
    }
};
// std::unordered_map<Edge, string, myhash> mymap2;
//                    ^KEY  ^VALUE  ^HASH

// ============================================================
// SECTION 4: IMPORTANT METHODS WITH COMPLEXITY
// ============================================================

// INSERT / UPDATE:
// m[key] = value         -> insert OR update               O(1) avg
//                           CREATES default if key missing!
//                           same danger as map::operator[]
// m.insert({key, val})   -> insert if key NOT present      O(1) avg
//                           returns pair<iterator, bool>
// m.insert_or_assign(k,v)-> insert OR overwrite (C++17)    O(1) avg
// m.emplace(key, val)    -> in-place construct              O(1) avg
// m.try_emplace(k, args) -> insert if not present (C++17)  O(1) avg
//
// ACCESS:
// m[key]                 -> reference (CREATES if missing!) O(1) avg
// m.at(key)              -> reference, throws if missing    O(1) avg
//
// SEARCH:
// m.find(key)            -> iterator or end()               O(1) avg
// m.contains(key)        -> bool (C++20)                    O(1) avg
// m.count(key)           -> 0 or 1                          O(1) avg
//
// REMOVE:
// m.erase(key)           -> remove by key, returns 0 or 1  O(1) avg
// m.erase(it)            -> remove by iterator              O(1)
// m.erase(it1, it2)      -> remove range                    O(n)
// m.clear()              -> remove all                      O(n)
//
// SIZE:
// m.size()               -> element count                   O(1)
// m.empty()              -> true if empty                   O(1)
//
// HASH TABLE STATS:
// m.bucket_count()       -> number of buckets               O(1)
// m.load_factor()        -> size / bucket_count             O(1)
// m.max_load_factor()    -> threshold before rehash (1.0)   O(1)
// m.reserve(n)           -> prepare for n elements          O(n)
// m.rehash(n)            -> set bucket count                O(n)

// ============================================================
// SECTION 5: main() TRACE
// ============================================================

int main()
{
    // CUSTOM KEY (Edge) unordered_map:
    std::unordered_map<Edge, std::string, myhash> mymap2;
    Edge edgeTest(100, 100);
    mymap2.insert({edgeTest, "edgeTest"});
    // hash(Edge(100,100)) = 200 -> bucket 200 % bucket_count

    Edge e1(1, 2);
    Edge e2(5, 4);
    Edge e3(3, 3);

    // TYPE ALIAS for cleaner code:
    using StringEdge = std::unordered_map<std::string, Edge>;

    StringEdge myMap = {{"s1",e1}, {"s2",e2}, {"s3",e3}};
    // stored in buckets: ORDER NOT GUARANTEED
    // NOT alphabetical like map!

    myMap.insert({"s4", e1});
    // insert succeeds (s4 is new key)
    // if s4 existed: insert would FAIL (keep old value)
    // vs myMap["s4"] = e1 which would OVERWRITE

    // ERASE by key:
    myMap.erase("s1");         // removes {s1, e1}, returns 1
    // myMap.erase("s99");     // key not found, returns 0 (no error!)

    // ERASE by iterator:
    StringEdge::iterator begin = myMap.begin();
    myMap.erase(begin);
    // removes whatever is at begin (order unpredictable!)
    // could be s2, s3, s4 - not guaranteed!

    // operator[] - insert OR update:
    myMap["s2"] = Edge(99, 100);  // UPDATE existing s2
    myMap["s5"] = Edge(99, 100);  // INSERT new s5
    // if s5 didn't exist: creates it with Edge(99,100)

    // EXISTENCE CHECK (C++20):
    if (myMap.contains("s5")) {
        std::cout << "we contain s5\n";
    }

    // EXISTENCE CHECK (pre-C++20):
    if (myMap.find("s6") != myMap.end()) {
        std::cout << "we contain s6\n";  // won't print (s6 not in map)
    }

    // ITERATION - structured binding (C++17, BEST):
    for (const auto& [key, value] : myMap) {
        std::cout << key << "|" << value << '\n';
    }
    // ORDER IS UNPREDICTABLE (hash-based, not alphabetical!)

    // ALTERNATIVES (all equivalent performance):
    // for (const auto& pair : myMap) { pair.first; pair.second; }
    // for (StringEdge::iterator it = myMap.begin(); ...)

    // HASH TABLE STATS:
    std::cout << "bucket_count: " << myMap.bucket_count() << '\n';
    std::cout << "load_factor:  " << myMap.load_factor()  << '\n';
    // load_factor = size / bucket_count
    // when > 1.0: rehash triggered (doubles buckets, re-inserts all)

    return 0;
}

// ============================================================
// SECTION 6: OPERATOR[] DANGER (same as map!)
// ============================================================

// SAFE READ:
// m.at("s6")       // throws out_of_range if missing
// m.find("s6")     // returns end() if missing
// m.contains("s6") // returns false if missing
//
// DANGEROUS READ:
// m["s6"]          // CREATES {s6, Edge()} if missing!
//                  // Edge() = {-1, -1} inserted silently
//                  // size increases by 1 accidentally
//
// SAFE WRITE:
// m["s2"] = Edge(99,100);  // OK: intended to create/overwrite
// m.insert_or_assign("s2", Edge(99,100)); // C++17, clearer intent
//
// RULE: use [] ONLY for intentional create/update
//       use at() / find() / contains() for reading

// ============================================================
// SECTION 7: DSA PATTERNS - THE MOST COMMON USE CASES
// ============================================================

// PATTERN 1: Frequency count (MOST COMMON DSA pattern)
// std::unordered_map<char, int> freq;
// for (char c : str) freq[c]++;    // O(n) total
// freq['a']  // how many 'a's

// PATTERN 2: Two Sum
// std::unordered_map<int, int> seen; // {value, index}
// for (int i = 0; i < nums.size(); i++) {
//     int complement = target - nums[i];
//     if (seen.count(complement))
//         return {seen[complement], i};
//     seen[nums[i]] = i;
// }

// PATTERN 3: Memoization (DP top-down)
// std::unordered_map<int, long long> memo;
// long long fib(int n) {
//     if (n <= 1) return n;
//     if (memo.count(n)) return memo[n];  // O(1) lookup
//     return memo[n] = fib(n-1) + fib(n-2);
// }

// PATTERN 4: Group anagrams
// std::unordered_map<string, vector<string>> groups;
// for (string& s : strs) {
//     string key = s; sort(key.begin(), key.end());
//     groups[key].push_back(s);
// }

// PATTERN 5: Graph adjacency list
// std::unordered_map<int, vector<int>> adj;
// adj[u].push_back(v);   // O(1) avg to add edge
// adj[u]                 // O(1) avg to get neighbors

// PATTERN 6: Cache / LRU Cache base
// std::unordered_map<int, int> cache;
// if (cache.count(key)) return cache[key]; // O(1) hit
// cache[key] = compute(key);               // O(1) store

// PATTERN 7: Count distinct elements
// std::unordered_map<int, bool> seen;
// for (int n : nums) seen[n] = true;
// return seen.size();  // number of distinct elements

// ============================================================
// SECTION 8: PERFORMANCE TIPS
// ============================================================

// TIP 1: reserve() to avoid rehashing
// std::unordered_map<string, int> m;
// m.reserve(10000);   // pre-allocate for 10000 elements
// for (...) m.insert(...);  // no rehashing!
//
// TIP 2: avoid operator[] for reads (prevents accidental inserts)
//
// TIP 3: use emplace() over insert() for complex value types
// m.emplace("key", Edge(1,2));  // constructs Edge directly in map
//                                // vs insert: constructs then moves
//
// TIP 4: count() before accessing in pre-C++20 code:
// if (m.count(key)) { m.at(key); } // safe access pattern
//
// TIP 5: string keys - consider string_view for lookups (C++20)
// m.find(string_view) avoids creating temporary string for lookup

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the main difference between map and unordered_map?
//     -> map: Red-Black Tree, O(log n), sorted keys.
//        unordered_map: Hash Table, O(1) avg, no ordering.
//        Choose based on whether you need sorted order.

// Q2: What does operator[] do when key doesn't exist?
//     -> DEFAULT CONSTRUCTS and INSERTS the value type.
//        m["missing"] inserts {missing, Value()} silently.
//        Use at(), find(), contains() for safe reads.

// Q3: What two things must a custom key type provide?
//     -> 1. operator==: for equality check (collision resolution)
//        2. Hash function: std::hash<T> specialization or custom
//           hash struct as third template parameter.

// Q4: What is the worst-case complexity of unordered_map?
//     -> O(n) if all keys hash to same bucket (all collide).
//        Rare in practice with good hash function.
//        Use map if guaranteed O(log n) is required.

// Q5: What is load_factor and when does rehashing occur?
//     -> load_factor = size / bucket_count.
//        When load_factor > max_load_factor (default 1.0):
//        rehash: ~2x buckets allocated, all elements re-inserted.
//        All iterators invalidated. Use reserve() to prevent.

// Q6: What is the return type of insert() in unordered_map?
//     -> pair<iterator, bool>. Same as map.
//        bool = true if newly inserted, false if key existed.
//        iterator points to the element (existing or new).

// Q7: Can you use unordered_map in interviews instead of map?
//     -> YES and often PREFERRED for O(1) vs O(log n).
//        Most frequency count, lookup, memoization problems:
//        unordered_map is faster and simpler.

// Q8: What is the difference between find() and contains()?
//     -> find(): returns iterator (use when you need the value too).
//        contains(): returns bool only (C++20, clearer intent).
//        Same O(1) avg complexity. Use contains() for existence only.
// ```

// **map vs unordered_map - the decision:**

// ```
// Need SORTED keys?          → map
// Need RANGE queries?        → map (lower_bound/upper_bound)
// Need O(1) avg lookup?      → unordered_map
// DSA problem (freq/memo)?   → unordered_map (almost always)
// Custom key type?           → map needs <, unordered_map needs == + hash
// ```

// **The most common DSA patterns:**

// ```cpp
// // FREQUENCY COUNT (most used pattern):
// unordered_map<char, int> freq;
// for (char c : str) freq[c]++;  // O(n) total

// // TWO SUM (second most used):
// unordered_map<int, int> seen;  // {value -> index}
// if (seen.count(target - nums[i])) return answer;
// seen[nums[i]] = i;

// // MEMOIZATION:
// unordered_map<int, long long> memo;
// if (memo.count(n)) return memo[n];  // O(1) cache hit
// ```

// > **The one-liner**: `unordered_map` is `map` with O(1) average operations instead of O(log n), at the cost of losing sorted order and needing a hash function. In DSA problems, reach for `unordered_map` by default — you get the same key-value interface at faster speed. Switch to `map` only when the problem requires sorted keys or range queries.