#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <iterator>

// ============================================================
// std::unordered_set - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::unordered_set<T> = hash table storing UNIQUE elements
// YOUR COMMENT:
// "unsorted data structure storing unique elements"
// "average time CONSTANT lookup, insertion, removal"
//
// INTERNAL STRUCTURE - HASH TABLE WITH BUCKETS:
//
// bucket[0]: -> [element] -> [element] -> nullptr  (chain)
// bucket[1]: -> nullptr
// bucket[2]: -> [element] -> nullptr
// bucket[3]: -> [element] -> [element] -> nullptr
// ...
// bucket[n]: -> nullptr
//
// HOW IT WORKS:
// 1. hash(key) -> generates a number
// 2. number % bucket_count -> which bucket
// 3. scan that bucket's chain for exact match
//
// PERFORMANCE:
// BEST/AVERAGE: O(1) - element goes straight to bucket (empty bucket)
// WORST CASE:   O(n) - ALL elements in same bucket (hash collision)
// -> worst case very rare with good hash function
// -> std::hash<int> rarely causes collisions

// ============================================================
// SECTION 2: set vs unordered_set
// ============================================================

// | Feature              | set           | unordered_set  |
// |----------------------|---------------|----------------|
// | Order                | sorted        | NO order       |
// | Implementation       | Red-Black Tree| Hash Table     |
// | Find                 | O(log n)      | O(1) avg       |
// | Insert               | O(log n)      | O(1) avg       |
// | Delete               | O(log n)      | O(1) avg       |
// | lower/upper_bound    | YES           | NO             |
// | Sorted iteration     | YES           | NO             |
// | Memory               | tree nodes    | buckets+chains |
// | Hash function needed | NO            | YES            |
// | Custom types         | need operator<| need hash<T>   |
//
// USE unordered_set WHEN:
// -> only need existence check (most common use case)
// -> order doesn't matter
// -> want fastest average O(1) operations
// -> large datasets where O(log n) vs O(1) matters
//
// USE set WHEN:
// -> need sorted order
// -> need range queries (lower_bound/upper_bound)
// -> need guaranteed O(log n) (no hash collision risk)

// ============================================================
// SECTION 3: HASH TABLE INTERNALS - bucket_count, load_factor
// ============================================================

// BUCKET COUNT:
// -> number of "slots" in the hash table
// -> set1.bucket_count() = YOUR CODE prints this
// -> initially: enough buckets for current elements
// -> grows when load_factor exceeds max_load_factor
//
// LOAD FACTOR:
// -> load_factor = size / bucket_count
// -> YOUR CODE: set1.load_factor()
// -> measures how "full" the hash table is
// -> low load_factor: mostly empty buckets (fast, wastes memory)
// -> high load_factor: crowded buckets (more collisions, slower)
// -> default max_load_factor = 1.0 (rehash when size == bucket_count)
//
// REHASHING:
// -> when load_factor > max_load_factor:
//    1. allocate MORE buckets (~2x)
//    2. RE-HASH ALL elements into new buckets
//    3. ALL ITERATORS INVALIDATED
// -> similar to vector reallocation
//
// EXAMPLE with 6 elements:
// bucket_count = 7 (typically prime number)
// load_factor = 6/7 = 0.857
// close to 1.0 -> will rehash on next insert!
//
// CONTROL:
// set1.reserve(n)          // pre-allocate for n elements (avoids rehash)
//                          // YOUR CODE has this commented out
// set1.max_load_factor(2.0)// allow more crowding before rehash
// set1.rehash(n)           // set at least n buckets directly

// ============================================================
// SECTION 4: generate_n + inserter - YOUR CODE
// ============================================================

// std::generate_n(output_iterator, count, generator_function)
// -> calls generator_function count times
// -> writes each result via output_iterator
//
// YOUR GENERATOR:
int gen()
{
    static int i = 0;  // static: persists between calls (like Singleton!)
    return i++;        // returns 0,1,2,3,4,5,6,7,8,9 on successive calls
}
// -> gen() called 10 times = generates 0,1,2,3,4,5,6,7,8,9
//
// std::inserter(set2, set2.begin()):
// -> creates an insert_iterator for set2
// -> each write to this iterator calls set2.insert()
// -> perfect for filling sets from algorithms
// -> position hint (set2.begin()) ignored by unordered_set
//    but required by the inserter syntax
//
// RESULT: set2 = {0,1,2,3,4,5,6,7,8,9} (order may vary in print)
//
// OTHER WAYS to use generate_n:
// std::generate_n(std::back_inserter(vector), 10, gen); // for vectors
// std::generate_n(std::front_inserter(list), 10, gen);  // for list

// ============================================================
// SECTION 5: IMPORTANT METHODS WITH COMPLEXITY
// ============================================================

// INSERT:
// s.insert(val)          -> insert, returns pair<it,bool>  O(1) avg
// s.emplace(args)        -> in-place construct + insert    O(1) avg
//
// REMOVE:
// s.erase(val)           -> remove by VALUE                O(1) avg
//                           returns 1 if removed, 0 if not found
//                           YOUR COMMENT: "return 1 if removed 0 if not"
// s.erase(it)            -> remove by ITERATOR             O(1)
// s.erase(it1, it2)      -> remove range                   O(n)
// s.clear()              -> remove all                     O(n)
//
// SEARCH:
// s.find(val)            -> iterator or end()              O(1) avg
// s.contains(val)        -> bool (C++20)                   O(1) avg
// s.count(val)           -> 0 or 1                         O(1) avg
//
// SIZE:
// s.size()               -> element count                  O(1)
// s.empty()              -> true if empty                  O(1)
//
// HASH TABLE SPECIFIC:
// s.bucket_count()       -> number of buckets              O(1)
// s.load_factor()        -> size / bucket_count            O(1)
// s.max_load_factor()    -> get max before rehash (=1.0)   O(1)
// s.max_load_factor(f)   -> set max load factor            O(1)
// s.reserve(n)           -> reserve for n elements         O(n)
// s.rehash(n)            -> set bucket count >= n          O(n)
// s.bucket(val)          -> which bucket val is in         O(1)
// s.bucket_size(n)       -> elements in bucket n           O(n)
//
// MERGE:
// s1.merge(s2)           -> move unique elements s2->s1   O(n) avg

// ============================================================
// SECTION 6: YOUR CODE EXPLAINED
// ============================================================

void printUnsortedSet(const std::unordered_set<int>& set1)
{
    std::cout << "============================================\n";
    // order is UNPREDICTABLE - depends on hash function + bucket layout
    // you might see: 5,3,1,6,2,4 or any other order
    for (const auto& e : set1) {  // cleaner than const_iterator
        std::cout << e << ",";
    }
    std::cout << '\n';
}

int main()
{
    std::unordered_set<int> set1{1, 2, 3, 4, 5, 6};
    // stored in hash table, order not guaranteed
    // all unique (already unique here)
     
    // GENERATE_N example:
    std::unordered_set<int> set2;
    std::generate_n(std::inserter(set2, set2.begin()), 10, gen);
    // gen() called 10 times: 0,1,2,3,4,5,6,7,8,9
    // inserter calls set2.insert() for each
    // set2 = {0,1,2,3,4,5,6,7,8,9}
    //set1.merge(set2);
    // ERASE by value - returns 0 or 1:
    if (set1.erase(19) == 1) {
        std::cout << "we removed 19\n";
    }
    // 19 not in set1 -> erase returns 0 -> if block skipped
    // no exception thrown (unlike vector.at() out of bounds)

    // HASH TABLE STATS:
    std::cout << "bucket_count: " << set1.bucket_count() << '\n';
    // typically 7 or 11 (prime) for 6 elements
    // actual value = implementation defined

    std::cout << "load_factor: " << set1.load_factor() << '\n';
    // load_factor = 6 / bucket_count
    // if bucket_count=7: 0.857...
    // if bucket_count=11: 0.545...

    // INSERT with existence check:
    if (!set1.contains(12)) {  // O(1) check
        set1.insert(12);       // O(1) insert
    }
    // better way (single O(1) operation):
    // set1.insert(12); // just insert - duplicates auto-rejected!
    // insert() returns {iterator, false} if already exists
    // no need to check contains() first for sets

    // BUCKET INSPECTION (your commented code):
    // for (size_t i = 0; i < set1.bucket_count(); i++) {
    //     std::cout << "bucket[" << i << "] size: "
    //               << set1.bucket_size(i) << '\n';
    // }
    // shows how many elements in each bucket
    // most buckets: 0 or 1 (good distribution)
    // bucket with many elements: hash collision happened

    printUnsortedSet(set1);
    // ORDER NOT GUARANTEED - could print: 12,5,3,1,6,2,4 or anything
    printUnsortedSet(set2);
    return 0;
}

// ============================================================
// SECTION 7: CUSTOM HASH FOR CUSTOM TYPES
// ============================================================

// For built-in types: std::hash<int>, std::hash<string> etc. built-in
// For custom types: MUST provide hash function
//
// struct Point { int x, y; };
//
// OPTION 1: Specialize std::hash
// template<>
// struct std::hash<Point> {
//     size_t operator()(const Point& p) const {
//         size_t hx = std::hash<int>{}(p.x);
//         size_t hy = std::hash<int>{}(p.y);
//         return hx ^ (hy << 1);  // combine hashes
//     }
// };
// std::unordered_set<Point> pointSet;
//
// OPTION 2: Custom hash struct passed as template param
// struct PointHash {
//     size_t operator()(const Point& p) const {
//         return std::hash<int>{}(p.x) ^ std::hash<int>{}(p.y);
//     }
// };
// std::unordered_set<Point, PointHash> pointSet;

// ============================================================
// SECTION 8: DSA PATTERNS
// ============================================================

// PATTERN 1: Two Sum - O(n) solution
// std::unordered_set<int> seen;
// for (int num : nums) {
//     if (seen.count(target - num)) return true;  // found pair!
//     seen.insert(num);
// }

// PATTERN 2: Remove duplicates preserving order
// std::unordered_set<int> seen;
// std::vector<int> result;
// for (int n : v) {
//     if (seen.insert(n).second) {  // .second = true if new
//         result.push_back(n);
//     }
// }

// PATTERN 3: Find intersection of two arrays
// std::unordered_set<int> s(v1.begin(), v1.end());
// std::vector<int> intersection;
// for (int n : v2) {
//     if (s.count(n)) intersection.push_back(n);
// }

// PATTERN 4: Check if array has duplicates
// std::unordered_set<int> s(v.begin(), v.end());
// bool hasDuplicates = (s.size() != v.size());

// PATTERN 5: Longest consecutive sequence O(n)
// std::unordered_set<int> s(nums.begin(), nums.end());
// int longest = 0;
// for (int n : s) {
//     if (!s.count(n-1)) {           // n is sequence start
//         int len = 1;
//         while (s.count(n + len)) len++;
//         longest = std::max(longest, len);
//     }
// }

// PATTERN 6: Visited nodes in graph traversal
// std::unordered_set<int> visited;
// void dfs(int node) {
//     if (visited.count(node)) return;  // already visited
//     visited.insert(node);
//     for (int neighbor : adj[node]) dfs(neighbor);
// }

// ============================================================
// SECTION 9: std::inserter and other output iterators
// ============================================================

// OUTPUT ITERATORS - used with algorithms to insert into containers:
//
// std::back_inserter(container)
// -> calls push_back() on each write
// -> works with: vector, deque, list
//
// std::front_inserter(container)
// -> calls push_front() on each write
// -> works with: list, deque (NOT vector - no push_front)
//
// std::inserter(container, position)
// -> calls insert(position, val) on each write
// -> works with: ANY container with insert()
// -> position hint (ignored by unordered containers)
// -> REQUIRED for set and unordered_set (no push_back!)
//
// USAGE EXAMPLES:
// std::copy(v.begin(), v.end(), std::back_inserter(result));
// std::transform(v.begin(), v.end(), std::inserter(s, s.begin()), op);
// std::generate_n(std::inserter(s, s.begin()), 10, gen); // YOUR CODE

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::unordered_set and how does it work?
//     -> Hash table storing unique elements. Uses hash(key) % bucket_count
//        to find the bucket, then linear scan of that bucket.
//        Average O(1) operations. No guaranteed order.

// Q2: What is load_factor and why does it matter?
//     -> size / bucket_count. Measures hash table density.
//        High load = more collisions = slower.
//        Default max = 1.0. When exceeded: rehash (double buckets,
//        re-insert all elements, all iterators invalidated).

// Q3: What does erase() return for unordered_set?
//     -> Number of elements removed: 1 if found and removed, 0 if not found.
//        Never throws. Safe to call with non-existent key.

// Q4: When would you use reserve() for unordered_set?
//     -> When you know the final size upfront. Prevents rehashing.
//        set.reserve(1000) ensures table handles 1000 elements
//        without rehashing. Like vector::reserve for performance.

// Q5: What is std::inserter and when do you need it?
//     -> Output iterator that calls insert() on each write.
//        Required for algorithms writing to sets (no push_back).
//        generate_n, copy, transform use it to write into sets.

// Q6: What is std::generate_n and how does it work?
//     -> Calls a generator function N times, writes results to
//        output iterator. gen() with static counter produces
//        sequential values. Combined with inserter fills a set.

// Q7: What is the worst-case complexity of unordered_set?
//     -> O(n) if ALL elements hash to the same bucket (all collide).
//        In practice near-impossible with good hash function.
//        If predictable collision is a concern: use set (O(log n) guaranteed).

// Q8: How do you use unordered_set with a custom type?
//     -> Must provide: operator== for equality, and std::hash<T>
//        specialization (or custom hash struct as template param).
// //        Combine member hashes with XOR or boost::hash_combine.
// ```

// **The hash table visual:**

// ```
// set1 = {1,2,3,4,5,6}, bucket_count = 7

// hash(1) % 7 = 1  ->  bucket[1]: [1]
// hash(2) % 7 = 2  ->  bucket[2]: [2]
// hash(3) % 7 = 3  ->  bucket[3]: [3]
// hash(4) % 7 = 4  ->  bucket[4]: [4]
// hash(5) % 7 = 5  ->  bucket[5]: [5]
// hash(6) % 7 = 6  ->  bucket[6]: [6]
//                       bucket[0]: empty

// load_factor = 6/7 = 0.857 ← close to 1.0, rehash coming soon!

// FIND(5): hash(5)%7=5 → check bucket[5] → found! O(1)
// INSERT(12): hash(12)%7=5 → bucket[5]: [5]->[12] (collision chain)
// ```

// **Three existence check patterns:**

// ```cpp
// // All O(1) average for unordered_set:
// s.count(val)           // 0 or 1 (C++98, misleading name)
// s.find(val) != s.end() // iterator check (C++98)
// s.contains(val)        // bool (C++20) ← PREFER THIS

// // INSERT without pre-checking (most efficient):
// auto [it, inserted] = s.insert(val);
// if (inserted) { /* new element */ }
// // ONE operation vs contains() + insert() = TWO operations
// ```

// > **The DSA golden rule**: `unordered_set` is your **O(1) membership test** structure. The most common interview patterns — two sum, detecting duplicates, finding intersections, tracking visited nodes — all reduce to "have I seen this value before?" which is exactly what `unordered_set` answers in O(1). When order matters or you need range queries, switch to `set`.