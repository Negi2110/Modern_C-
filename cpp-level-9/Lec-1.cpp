#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <functional>
#include <tuple>

// ============================================================
// std::pair - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS IT?
// ============================================================

// std::pair<T1, T2> = bundles exactly TWO values of potentially
//                     different types into one object
//
// YOUR COMMENT: "special case of std::tuple"
// std::pair  = tuple with exactly 2 elements
// std::tuple = pair generalized to N elements
//
// INTERNAL STRUCTURE (as your template shows):
// template<typename First, typename Second>
// struct pair {
//     First  first;   // first element
//     Second second;  // second element
// };
//
// MOST COMMON USE CASES:
// 1. KEY-VALUE pairs (especially as map return types)
// 2. Returning TWO values from a function
// 3. Storing coordinate pairs (x,y)
// 4. Returning iterator + bool from insert()

// ============================================================
// SECTION 2: CONSTRUCTION / INITIALIZATION
// ============================================================

// std::pair<int, string> p(1, "mike");       // constructor
// std::pair<int, string> p = {1, "mike"};    // brace init
// std::pair<int, string> p{1, "mike"};       // uniform init
// auto p = std::make_pair(1, "mike");        // type deduction
// auto p = std::make_pair(std::ref(x), 1.0f); // with reference
//
// TYPE ALIAS (your code):
using student_t = std::pair<int, std::string>;
// -> cleaner than writing std::pair<int,string> everywhere
// -> like a typedef but with using (preferred modern C++)

// ============================================================
// SECTION 3: ACCESSING ELEMENTS
// ============================================================

// THREE WAYS:
// p.first       -> first element  (member access)
// p.second      -> second element (member access)
// std::get<0>(p)-> first element  (index-based, like tuple)
// std::get<1>(p)-> second element (index-based)
// std::get<T>(p)-> element of type T (TYPE-based, YOUR CODE)
//                  std::get<float>(p) -> gets the float member
//                  only works if type is UNIQUE in the pair
//
// C++17 STRUCTURED BINDINGS (most modern, cleanest):
// auto [key, value] = p;
// auto [id, name] = student;
// for (auto [id, name] : students) { cout << id << name; }

// ============================================================
// SECTION 4: std::ref() - YOUR CODE EXPLAINS THIS WELL
// ============================================================

// std::make_pair(value, 10.0f)        -> copies value
// std::make_pair(std::ref(value), 10.0f) -> stores REFERENCE to value
//
// WITHOUT std::ref (copy):
// long value = 100L;
// auto p = std::make_pair(value, 10.0f);  // p.first = copy of 100
// value = 150L;
// p.first == 100  // STILL 100! copy is independent
//
// WITH std::ref (reference):
// auto p = std::make_pair(std::ref(value), 10.0f);
// value = 150L;
// p.first == 150  // UPDATED! it's a reference!
//
// YOUR CODE:
// long value = 100L;
// auto newpair = std::make_pair(std::ref(value), 10.0f);
// value = 150L;
// std::cout << newpair.first;  // prints 150 (not 100!)
//
// WHY? std::ref wraps value in std::reference_wrapper<long>
// -> pair stores the wrapper, not a copy
// -> accessing .first goes through the wrapper to original value
//
// TYPE of newpair = std::pair<std::reference_wrapper<long>, float>
//
// USE std::ref WHEN:
// -> you want pair to "see" changes to the original variable
// -> passing pairs to functions that update via the pair
// -> avoiding expensive copies of large objects in pairs

// ============================================================
// SECTION 5: COMPARISON OPERATORS
// ============================================================

// pair supports: ==, !=, <, >, <=, >=
//
// EQUALITY: both first AND second must match
// {1,"mike"} == {1,"mike"}  -> true
// {1,"mike"} == {1,"john"}  -> false (second differs)
// {2,"mike"} == {1,"mike"}  -> false (first differs)
//
// LESS THAN: lexicographic (first compared first, then second)
// {1,"b"} < {2,"a"}  -> true  (1 < 2, first decides)
// {1,"a"} < {1,"b"}  -> true  (first equal, "a" < "b" decides)
// {1,"b"} < {1,"a"}  -> false ("b" > "a")
//
// WHY THIS MATTERS:
// std::map uses < to sort pairs
// std::sort on vector<pair> sorts by first then second automatically

// ============================================================
// SECTION 6: IMPORTANT METHODS AND FREE FUNCTIONS
// ============================================================

// CREATION:
// std::make_pair(a, b)       -> create pair with type deduction
// std::make_pair(ref(a), b)  -> create pair with reference

// ACCESS:
// p.first                    -> first element
// p.second                   -> second element
// std::get<0>(p)             -> first (tuple-style)
// std::get<1>(p)             -> second (tuple-style)
// std::get<T>(p)             -> by type (C++14)

// SWAP:
// p1.swap(p2)                -> swap contents
// std::swap(p1, p2)          -> same

// C++17 DEDUCTION:
// std::pair p(1, "hello");   // CTAD: pair<int, const char*>
// no need for make_pair in C++17!

// ============================================================
// SECTION 7: PAIR WITH MAP - PREVIEW (since you mentioned it)
// ============================================================

// std::map<int, string> m;
// m.insert(std::make_pair(1, "alice"));
//
// Iterating map gives pairs:
// for (const auto& [key, val] : m) { // C++17
//     cout << key << " -> " << val;
// }
//
// m.find(key) returns iterator to pair<const Key, Value>
// if (auto it = m.find(1); it != m.end()) {
//     it->first;   // key
//     it->second;  // value
// }
//
// m.insert() returns pair<iterator, bool>:
// auto [it, inserted] = m.insert({1, "alice"});
// // inserted = true if new, false if key already existed

// ============================================================
// SECTION 8: RETURNING MULTIPLE VALUES WITH PAIR
// ============================================================

// Classic use: return two values from function
// std::pair<bool, int> findIndex(vector<int>& v, int target) {
//     for (int i = 0; i < v.size(); i++)
//         if (v[i] == target) return {true, i};  // found
//     return {false, -1};                          // not found
// }
//
// auto [found, index] = findIndex(v, 42);  // C++17
// if (found) cout << "at index " << index;
//
// Before C++17 (or for more than 2 values):
// auto result = findIndex(v, 42);
// if (result.first) cout << "at index " << result.second;

// ============================================================
// SECTION 9: DSA PATTERNS WITH PAIR
// ============================================================

// PATTERN 1: Sorting by second element
// vector<pair<string, int>> scores{{"alice",90},{"bob",85}};
// std::sort(scores.begin(), scores.end(),
//     [](const auto& a, const auto& b){ return a.second > b.second; });
// -> sorted by score descending

// PATTERN 2: Priority queue of pairs (by first = priority)
// std::priority_queue<pair<int,int>> pq;
// pq.push({priority, node});
// auto [pri, node] = pq.top();
// -> natural pair comparison: highest first first

// PATTERN 3: Dijkstra's algorithm
// using State = pair<int,int>;  // {distance, node}
// priority_queue<State, vector<State>, greater<State>> pq;
// pq.push({0, source});  // {distance, node}

// PATTERN 4: Coordinate pairs for grid problems
// using Point = pair<int,int>;  // {row, col}
// vector<Point> dirs{{0,1},{0,-1},{1,0},{-1,0}};
// for (auto [dr, dc] : dirs) {  // C++17
//     int nr = r + dr, nc = c + dc;
// }

// PATTERN 5: Interval problems
// vector<pair<int,int>> intervals{{1,3},{2,6},{8,10}};
// sort(intervals.begin(), intervals.end()); // sort by start
// // pair< naturally sorts by first then second

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is std::pair and when do you use it?
//     -> A struct bundling exactly two values of potentially
//        different types. Used for key-value pairs, returning
//        two values from a function, map iteration, insert results.

// Q2: What is the difference between make_pair and constructor?
//     -> Constructor: must specify types explicitly: pair<int,string>(1,"a")
//        make_pair: deduces types automatically: make_pair(1, "a")
//        C++17 CTAD: pair p(1, "a") also works now.

// Q3: What does std::ref do in make_pair?
//     -> Wraps variable in reference_wrapper so pair stores a
//        reference, not a copy. Changes to original variable
//        are reflected through pair.first/second.

// Q4: How does pair comparison (< operator) work?
//     -> Lexicographic: compares first elements first.
//        If first elements equal, compares second elements.
//        This is why sorting vector<pair> works naturally.

// Q5: What is std::get<T> and when do you use it over .first/.second?
//     -> std::get<int>(p) gets element by TYPE.
//        Useful in generic/template code where you don't know
//        if it's first or second. Type must be unique in the pair.
//        More consistent with tuple access pattern.

// Q6: What is the return type of map::insert()?
//     -> pair<iterator, bool>. Iterator to the element.
//        bool = true if newly inserted, false if key existed.
//        auto [it, inserted] = m.insert({key, val});

// Q7: What is a structured binding and how does it work with pair?
//     -> C++17 feature: auto [a, b] = p; creates a and b as
//        references/copies of p.first and p.second.
//        Cleaner than p.first/p.second everywhere.

// Q8: How do you sort a vector<pair> by the second element?
//     -> Default sort sorts by first, then second.
//        Custom: sort(v.begin(), v.end(),
//                [](const auto& a, const auto& b){
//                    return a.second < b.second; });

int main()
{
    // TYPE ALIAS for cleaner code:
    std::vector<student_t> students;

    for (int i = 0; i < 5; i++) {
        std::pair<int, std::string> student(i, "mike");
        students.push_back(student);
    }

    // ACCESS via .first and .second:
    for (const auto& [id, name] : students) {  // C++17 structured binding
        std::cout << id << "," << name << '\n';
    }
    // OR old style:
    // for (auto e : students) { cout << e.first << "," << e.second; }

    // std::ref - reference in pair:
    long value = 100L;
    auto newpair = std::make_pair(std::ref(value), 10.0f);
    // newpair.first = reference to value (not a copy!)

    value = 150L;
    std::cout << newpair.first << '\n';    // 150! (sees the update)
    std::cout << std::get<0>(newpair) << '\n';    // 150 (same, index access)
    std::cout << std::get<float>(newpair) << '\n';// 10  (type-based access)

    // WITHOUT std::ref (copy):
    long x = 100L;
    auto copypair = std::make_pair(x, 10.0f); // COPIES x
    x = 150L;
    std::cout << copypair.first << '\n';   // still 100! (copy)

    // C++17 CTAD (no make_pair needed):
    std::pair p(42, std::string("hello")); // deduces pair<int, string>

    // Structured binding:
    auto [num, str] = p;
    std::cout << num << " " << str << '\n';

    return 0;
}
// ```

// **std::ref vs copy - the key visual:**

// ```cpp
// long value = 100L;

// // COPY:
// auto p1 = std::make_pair(value, 1.0f);
// value = 150L;
// p1.first == 100  // ← still 100, copy is independent

// // REFERENCE:
// auto p2 = std::make_pair(std::ref(value), 1.0f);
// value = 150L;
// p2.first == 150  // ← sees 150, it's a reference!
// ```

// **Three access styles:**

// ```cpp
// std::pair<int, float> p(42, 3.14f);

// p.first           // 42    ← most common
// p.second          // 3.14f ← most common
// std::get<0>(p)    // 42    ← tuple-style index
// std::get<1>(p)    // 3.14f ← tuple-style index
// std::get<float>(p)// 3.14f ← type-based (C++14, type must be unique)

// auto [i, f] = p;  // 42, 3.14f ← C++17 structured binding (CLEANEST)
// ```

// > **The DSA one-liner**: `std::pair<int,int>` is the backbone of graph algorithms — `{distance, node}` in Dijkstra, `{priority, task}` in scheduling, `{row, col}` in grid BFS. The natural lexicographic comparison means `priority_queue<pair<int,int>>` automatically gives you the **smallest distance first** with `greater<pair<int,int>>`, making it perfect for shortest path algorithms.