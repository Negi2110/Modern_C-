#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

// ============================================================
// STL SEARCHING ALGORITHMS - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT ARE NON-MODIFYING ALGORITHMS?
// ============================================================

// YOUR COMMENT: "these algorithms do not modify the data structure,
//                but typically take a pair of iterators and a value
//                or range to search for"
//
// NON-MODIFYING ALGORITHMS:
// -> read elements but NEVER change them
// -> work on ANY range (vector, list, array, etc.)
// -> take: [begin, end) range + search target
// -> return: iterator to found element OR end() if not found
//
// FOUR ALGORITHMS IN YOUR CODE:
// std::find         -> find EXACT VALUE
// std::find_if      -> find element matching PREDICATE (condition)
// std::search       -> find SUBSEQUENCE inside a range
// std::adjacent_find-> find CONSECUTIVE DUPLICATE elements

// ============================================================
// SECTION 2: std::find - EXACT VALUE SEARCH
// ============================================================

void findExample()
{
    std::vector<int> v{1, 3, 5, 7, 9};

    // BASIC find:
    auto result = std::find(v.begin(), v.end(), 7);
    // searches range [begin, end) for value 7
    // returns iterator to FIRST occurrence
    // returns end() if NOT found
    // complexity: O(n) linear search

    if (result != v.end()) {
        std::cout << "Found: " << *result << '\n';       // 7
        std::cout << "Index: " << std::distance(v.begin(), result) << '\n'; // 3
    }

    // CONST iterator version (for read-only access):
    std::vector<int>::const_iterator cResult =
        std::find(v.cbegin(), v.cend(), 7);
    // cbegin/cend = const iterators (can't modify through them)

    // YOUR COMMENTED CODE:
    // std::find(v.begin(), v.end() + 2, 3)
    // WARNING: v.end() + 2 = UNDEFINED BEHAVIOR!
    // end() already points past the last element
    // going beyond end() = invalid iterator!
    // CORRECT: v.begin() + 2 (search only first 2 elements)
    auto partial = std::find(v.begin(), v.begin() + 2, 3);
    // searches only {1, 3} (first 2 elements)
    if (partial != v.begin() + 2) {
        std::cout << "Found in first 2: " << *partial << '\n'; // 3
    }

    // NOT FOUND case:
    auto notFound = std::find(v.begin(), v.end(), 99);
    if (notFound == v.end()) {
        std::cout << "99 not found\n";  // prints this
    }
}

// ============================================================
// SECTION 3: std::find_if - PREDICATE-BASED SEARCH
// ============================================================

void findIfExample()
{
    std::vector<int> v{1, 3, 12, 5, 7, 9, 10};

    // FIND FIRST EVEN NUMBER:
    auto is_even = [](int i) { return i % 2 == 0; };
    auto result = std::find_if(v.begin(), v.end(), is_even);

    if (result != v.end()) {
        std::cout << "First even: " << *result << '\n';  // 12
    }

    // FIND FIRST > 6:
    auto result2 = std::find_if(v.begin(), v.end(),
        [](int i) { return i > 6; });
    // returns iterator to 7 (first element > 6)

    // FIND_IF_NOT: find first element NOT matching predicate
    auto result3 = std::find_if_not(v.begin(), v.end(), is_even);
    // returns iterator to 1 (first ODD number)
    std::cout << "First odd: " << *result3 << '\n';  // 1

    // REAL USE CASES:
    // Find first expired item:
    // find_if(items.begin(), items.end(),
    //         [](const Item& i){ return i.isExpired(); });
    //
    // Find first disconnected entity in DIS:
    // find_if(entities.begin(), entities.end(),
    //         [](const Entity& e){ return e.isTimedOut(); });
}

// ============================================================
// SECTION 4: std::search - SUBSEQUENCE SEARCH
// ============================================================

void searchExample()
{
    std::vector<int> v{1, 2, 3, 4, 5};
    std::vector<int> s{2, 3, 4};  // sequence to find inside v

    auto result = std::search(v.begin(), v.end(),
                              s.begin(), s.end());
    // searches for the ENTIRE sequence {2,3,4} inside v
    // returns iterator to START of where sequence was found
    // returns v.end() if sequence not found
    // complexity: O(n*m) where n=v.size(), m=s.size()

    if (result != v.end()) {
        std::cout << "Sequence found starting at: "
                  << *result << '\n';  // 2 (start of {2,3,4})
        std::cout << "At index: "
                  << std::distance(v.begin(), result) << '\n';  // 1
    }

    // SEARCH WITH PREDICATE (C++17):
    // auto result2 = std::search(v.begin(), v.end(),
    //                            s.begin(), s.end(),
    //                            [](int a, int b){ return a == b; });

    // DIFFERENCE: find vs search:
    // std::find   -> searches for a SINGLE VALUE
    // std::search -> searches for a SEQUENCE OF VALUES
    //
    // find({2,3,4}) in {1,2,3,4,5}: finds FIRST occurrence of value 2,3,or 4
    // search({2,3,4}) in {1,2,3,4,5}: finds WHERE the sequence 2,3,4 appears

    // STRING SEARCHING (search works on strings too!):
    std::string text = "Hello World Hello";
    std::string pattern = "World";
    auto strResult = std::search(text.begin(), text.end(),
                                 pattern.begin(), pattern.end());
    if (strResult != text.end()) {
        std::cout << "Found at position: "
                  << std::distance(text.begin(), strResult) << '\n'; // 6
    }

    // SEQUENCE NOT FOUND:
    std::vector<int> notThere{9, 9, 9};
    auto notFound = std::search(v.begin(), v.end(),
                                notThere.begin(), notThere.end());
    if (notFound == v.end()) {
        std::cout << "Sequence {9,9,9} not found\n";
    }
}

// ============================================================
// SECTION 5: std::adjacent_find - YOUR CODE
// ============================================================

void adjacentFindExample()
{
    std::vector<int> v{1, 2, 2, 3, 4, 5, 5, 6, 7, 8};

    auto result = std::adjacent_find(v.begin(), v.end());
    // finds first pair of CONSECUTIVE EQUAL elements
    // v = {1, 2, [2, 2], 3, 4, [5, 5], 6, 7, 8}
    //              ^^^
    //              first adjacent pair found here!
    // returns iterator to FIRST element of the pair
    // returns end() if no adjacent pair found

    if (result != v.end()) {
        std::cout << "Found adjacent pair: " << *result << '\n';
        // prints: 2 (the first of the pair {2,2})

        std::cout << "At index: "
                  << std::distance(v.begin(), result) << '\n';
        // prints: 1 (index of first '2' in the pair)

        // Both elements:
        std::cout << "Pair: " << *result
                  << " and " << *std::next(result) << '\n';
        // prints: "Pair: 2 and 2"
    }

    // FIND ALL ADJACENT PAIRS:
    std::cout << "All adjacent pairs in v:\n";
    auto it = v.begin();
    while (true) {
        it = std::adjacent_find(it, v.end());
        if (it == v.end()) break;
        std::cout << "  Pair " << *it
                  << " at index " << std::distance(v.begin(), it) << '\n';
        ++it;  // advance past this pair to find next
    }
    // Output:
    // Pair 2 at index 1
    // Pair 5 at index 5

    // WITH PREDICATE (find adjacent elements where second > first by 2):
    std::vector<int> v2{1, 3, 5, 8, 9, 12};
    auto result2 = std::adjacent_find(v2.begin(), v2.end(),
        [](int a, int b) { return (b - a) > 2; });
    // finds first pair where gap > 2
    // {5, 8}: 8-5=3 > 2 → found!
    if (result2 != v2.end()) {
        std::cout << "Gap > 2 between: "
                  << *result2 << " and "
                  << *std::next(result2) << '\n';  // 5 and 8
    }

    // NO ADJACENT PAIR:
    std::vector<int> unique{1, 2, 3, 4, 5};
    if (std::adjacent_find(unique.begin(), unique.end()) == unique.end()) {
        std::cout << "No duplicates found\n";
    }
}

// ============================================================
// SECTION 6: COMPLEXITY SUMMARY
// ============================================================

// Algorithm         | Complexity | What it finds
// ------------------|------------|---------------------------
// std::find         | O(n)       | First element == value
// std::find_if      | O(n)       | First element where pred(e)==true
// std::find_if_not  | O(n)       | First element where pred(e)==false
// std::search       | O(n*m)     | First occurrence of subsequence
// std::adjacent_find| O(n)       | First consecutive equal pair

// ============================================================
// SECTION 7: REAL DIS/ISSST USE CASES
// ============================================================

// struct Entity { uint32_t id; float x,y,z; bool active; };
// std::vector<Entity> entities;
//
// FIND specific entity by ID:
// auto it = std::find_if(entities.begin(), entities.end(),
//     [targetID](const Entity& e){ return e.id == targetID; });
// if (it != entities.end()) { /* found entity */ }
//
// FIND first inactive entity (for reuse):
// auto slot = std::find_if(entities.begin(), entities.end(),
//     [](const Entity& e){ return !e.active; });
//
// FIND two entities in same position (collision detection):
// auto collision = std::adjacent_find(entities.begin(), entities.end(),
//     [](const Entity& a, const Entity& b){
//         return std::abs(a.x - b.x) < 1.0f &&
//                std::abs(a.y - b.y) < 1.0f;
//     });
//
// SEARCH for specific PDU sequence in replay buffer:
// std::vector<uint8_t> pduBuffer = getReplayBuffer();
// std::vector<uint8_t> signature = {0x01, 0x01, 0x00}; // PDU header
// auto pduStart = std::search(pduBuffer.begin(), pduBuffer.end(),
//                             signature.begin(), signature.end());

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between std::find and std::find_if?
//     -> find: searches for exact VALUE equality (uses ==).
//        find_if: searches for element satisfying a PREDICATE (lambda/functor).
//        find_if is more flexible (any condition, not just equality).

// Q2: What does std::search do differently from std::find?
//     -> find: searches for a SINGLE element/value.
//        search: searches for a SUBSEQUENCE (multiple consecutive elements).
//        search({2,3,4}) finds where the pattern {2,3,4} appears.

// Q3: What does std::adjacent_find return?
//     -> Iterator to the FIRST element of the first consecutive
//        pair of equal elements. Returns end() if no adjacent duplicates.
//        With predicate: first pair where pred(a, b) == true.

// Q4: All searching algorithms return end() when not found - why?
//     -> Standard convention: end() = "not found" sentinel.
//        Always check: if (result != container.end()) before using *result.
//        Dereferencing end() = undefined behavior!

// Q5: Can you use std::find on a std::map?
//     -> Yes but it's O(n) - finds first element (pair<key,value>) == target.
//        Better: use map.find(key) which is O(log n) for maps!
//        Use std::find only on sequential containers (vector, list, deque).

// Q6: How do you find ALL occurrences (not just first)?
//     -> Loop with find/find_if, advance iterator past each found element:
//        auto it = begin;
//        while ((it = std::find(it, end, val)) != end) {
//            process(*it); ++it;
//        }

// Q7: What is find_if_not?
//     -> Finds first element where predicate is FALSE.
//        find_if_not(v.begin(), v.end(), is_even)
//        = find first ODD number.
//        Equivalent to: find_if with negated predicate.

// Q8: Why use std::search over std::string::find for strings?
//     -> std::search works on ANY container type, not just strings.
//        std::string::find works only on strings.
//        std::search supports custom comparison predicates (case-insensitive etc.)
//        In practice for strings: string::find is simpler and faster.

int main()
{
    // YOUR CODE: adjacent_find
    std::vector<int> v{1, 2, 2, 3, 4, 5, 5, 6, 7, 8};
    auto result = std::adjacent_find(v.begin(), v.end());

    if (result != v.end()) {
        std::cout << "Found adjacent pair: " << *result << '\n';  // 2
        std::cout << "At index: "
                  << std::distance(v.begin(), result) << '\n';    // 1
    }

    // Run all examples:
    std::cout << "\n--- find ---\n";       findExample();
    std::cout << "\n--- find_if ---\n";    findIfExample();
    std::cout << "\n--- search ---\n";     searchExample();
    std::cout << "\n--- adjacent_find ---\n"; adjacentFindExample();

    return 0;
}
// ```

// **The four algorithms compared:**

// ```
// v = {1, 2, 2, 3, 4, 5, 5, 6}

// std::find(v, 5):          → points to index 5 (first '5')
//                             finds single VALUE

// std::find_if(v, is_even): → points to index 1 (first even = 2)
//                             finds first element matching CONDITION

// std::search(v, {3,4,5}):  → points to index 3 (where {3,4,5} starts)
//                             finds SUBSEQUENCE of elements

// std::adjacent_find(v):    → points to index 1 (first {2,2} pair)
//                             finds CONSECUTIVE EQUAL elements
// ```

// **The NOT FOUND check - always required:**

// ```cpp
// auto it = std::find(v.begin(), v.end(), 99);

// if (it == v.end()) {
//     // NOT FOUND - do NOT dereference it!
//     // *it = UNDEFINED BEHAVIOR!
// } else {
//     // FOUND - safe to use *it
//     std::cout << *it;
// }
// ```

// > **The practical rule**: in your DIS simulation, use `std::find_if` constantly — "find the entity with this ID", "find the first timed-out entity", "find the first unoccupied slot". It's O(n) but for typical simulation sizes (hundreds to thousands of entities), this is fast enough. If you need O(log n) lookup by key, use `std::map` or `std::unordered_map` instead.