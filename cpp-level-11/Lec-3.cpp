#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cctype>

// ============================================================
// std::equal - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS std::equal?
// ============================================================

// std::equal compares TWO RANGES element by element
// returns TRUE if ALL corresponding elements are equal
// returns FALSE as soon as ANY mismatch found (short-circuit)
//
// YOUR COMMENT: "return true if range [first1,last1] is equal to
//                range [first2, first2+(last1-first1)]"
//
// TWO SIGNATURES:
//
// 3-ITERATOR VERSION (C++11 - DANGEROUS):
// equal(first1, last1, first2)
// -> compares [first1,last1) with [first2, first2+n)
// -> ASSUMES range2 is at least as long as range1
// -> UNDEFINED BEHAVIOR if range2 is shorter!
//
// 4-ITERATOR VERSION (C++14 - SAFE):
// equal(first1, last1, first2, last2)
// -> explicitly bounds BOTH ranges
// -> returns false if sizes differ (no UB!)
// -> PREFER THIS VERSION!
//
// WITH PREDICATE:
// equal(first1, last1, first2, last2, pred)
// -> pred(a, b) returns true if elements are "equal"
// -> replaces default == comparison
// -> YOUR test3 bug uses this for case-insensitive compare

// ============================================================
// SECTION 2: test0 - BASIC EQUAL (CORRECT)
// ============================================================

void test0()
{
    std::vector<int> v1{1, 2, 3};
    std::vector<int> v2{1, 2, 3};

    bool result = std::equal(v1.begin(), v1.end(),
                             v2.begin(), v2.end());
    // compares: 1==1, 2==2, 3==3 → ALL equal
    // returns: TRUE

    std::cout << "test0 - is Equal? " << result << '\n';  // 1

    // HOW IT WORKS INTERNALLY:
    // for each position i:
    //   if v1[i] != v2[i]: return false
    // return true (all matched)
}

// ============================================================
// SECTION 3: test1 - PARTIAL RANGE COMPARISON (BUG ANALYSIS)
// ============================================================

void test1()
{
    std::vector<int> v1{2, 3};
    std::vector<int> v2{1, 2, 3};

    bool result = std::equal(v1.begin() + 1, v1.end(),
                             v2.begin(), v2.end());
    // v1 range: v1.begin()+1 to v1.end() = {3} (just one element!)
    // v2 range: v2.begin() to v2.end()   = {1, 2, 3}
    //
    // SIZES ARE DIFFERENT: range1 has 1 element, range2 has 3 elements
    // 4-iterator version: sizes differ → returns FALSE immediately
    //
    // WHAT PROBABLY INTENDED:
    // Compare {3} with {3} (last element of v2)?
    // Should be: equal(v1.begin()+1, v1.end(), v2.begin()+2, v2.end())
    //            equal({3}, {3}) → TRUE

    std::cout << "test1 - is Equal? " << result << '\n';  // 0 (false)

    // CORRECT VERSION (what was probably meant):
    bool intended = std::equal(v1.begin(), v1.end(),
                               v2.begin() + 1, v2.end());
    // range1: {2, 3}
    // range2: {2, 3} (last two elements of v2)
    // 2==2, 3==3 → TRUE
    std::cout << "test1 corrected - is Equal? " << intended << '\n'; // 1
}

// ============================================================
// SECTION 4: test2 - WITH PREDICATE (BUG ANALYSIS)
// ============================================================

void test2()
{
    std::vector<int> v1{2, 3};
    std::vector<int> v2{1, 2, 3};

    bool result = std::equal(v1.begin() + 1, v1.end(),
                             v2.begin(), v2.end(),
                             [](int i, int j) { return i == j; });
    // SAME SIZE ISSUE as test1!
    // range1: {3} (1 element)
    // range2: {1,2,3} (3 elements)
    // Size mismatch → FALSE (even with custom predicate)
    //
    // Predicate [](int i, int j){ return i == j; }
    // is IDENTICAL to the default behavior (no benefit here)
    // Useful predicates: case-insensitive, absolute value, tolerance

    std::cout << "test2 - is Equal? " << result << '\n';  // 0

    // PREDICATE EXAMPLES THAT ACTUALLY ADD VALUE:
    // Case-insensitive: [](char a, char b){ return tolower(a)==tolower(b); }
    // Abs equal:        [](int a, int b){ return abs(a)==abs(b); }
    // Float tolerance:  [](float a, float b){ return abs(a-b) < 0.001f; }
    // Mod equal:        [](int a, int b){ return (a%10)==(b%10); }
}

// ============================================================
// SECTION 5: test3 - CASE INSENSITIVE STRING COMPARE (BUG!)
// ============================================================

void test3()
{
    std::string s1 = "hello";
    std::string s2 = "HELLO";

    // YOUR CODE - BUG: predicate just uses == (case SENSITIVE!)
    bool result_buggy = std::equal(s1.begin(), s1.end(),
                                   s2.begin(), s2.end(),
                                   [](char i, char j) {
                                       return i == j;  // 'h' != 'H' → FALSE
                                   });
    std::cout << "test3 buggy (case sensitive): "
              << result_buggy << '\n';  // 0 (false) - not actually insensitive!

    // FIXED: truly case-insensitive comparison
    bool result_fixed = std::equal(s1.begin(), s1.end(),
                                   s2.begin(), s2.end(),
                                   [](char i, char j) {
                                       return std::tolower(i) == std::tolower(j);
                                       // 'h'==tolower('H')=true ✅
                                   });
    std::cout << "test3 fixed (case insensitive): "
              << result_fixed << '\n';  // 1 (true) ✅
}

// ============================================================
// SECTION 6: ALL VARIATIONS WITH EXAMPLES
// ============================================================

void allVariations()
{
    // VARIATION 1: 3-iterator (UNSAFE - only if range2 >= range1)
    std::vector<int> v1{1, 2, 3};
    std::vector<int> v2{1, 2, 3, 4, 5};  // longer
    bool r1 = std::equal(v1.begin(), v1.end(), v2.begin());
    // compares {1,2,3} with first 3 of v2 = {1,2,3} → TRUE
    // safe here because v2 is longer, but ALWAYS prefer 4-iterator!
    std::cout << "3-iterator: " << r1 << '\n';  // 1

    // VARIATION 2: 4-iterator (SAFE - C++14)
    bool r2 = std::equal(v1.begin(), v1.end(),
                         v2.begin(), v2.end());
    // different sizes (3 vs 5) → FALSE immediately
    std::cout << "4-iterator different sizes: " << r2 << '\n';  // 0

    // VARIATION 3: custom predicate - absolute value
    std::vector<int> pos{1, 2, 3};
    std::vector<int> neg{-1, -2, -3};
    bool r3 = std::equal(pos.begin(), pos.end(),
                         neg.begin(), neg.end(),
                         [](int a, int b) {
                             return std::abs(a) == std::abs(b);
                         });
    std::cout << "abs equal: " << r3 << '\n';  // 1 (true)

    // VARIATION 4: float comparison with tolerance
    std::vector<float> f1{1.0f, 2.0f, 3.0f};
    std::vector<float> f2{1.001f, 1.999f, 3.0f};
    bool r4 = std::equal(f1.begin(), f1.end(),
                         f2.begin(), f2.end(),
                         [](float a, float b) {
                             return std::abs(a - b) < 0.01f;
                         });
    std::cout << "float tolerance: " << r4 << '\n';  // 1 (within 0.01)

    // VARIATION 5: partial range comparison
    std::vector<int> v3{1, 2, 3, 4, 5};
    std::vector<int> v4{3, 4, 5};
    // does v3 end with v4?
    bool r5 = std::equal(v4.begin(), v4.end(),
                         v3.end() - v4.size(), v3.end());
    std::cout << "ends with: " << r5 << '\n';  // 1 (true)
}

// ============================================================
// SECTION 7: equal vs mismatch vs search
// ============================================================

// std::equal:
// -> are ranges the same? returns BOOL
// -> O(n) linear
// -> use: "are these two containers identical?"

// std::mismatch:
// -> WHERE do they differ? returns PAIR of iterators
// -> O(n) linear
// -> use: "show me the first difference"

// std::search:
// -> is range2 a SUBSEQUENCE of range1? returns iterator
// -> O(n*m) quadratic
// -> use: "does range1 contain range2 as a sub-sequence?"

void compareAlgorithms()
{
    std::vector<int> v1{1, 2, 3, 4, 5};
    std::vector<int> v2{1, 2, 3, 4, 5};
    std::vector<int> v3{1, 2, 9, 4, 5};
    std::vector<int> sub{2, 3, 4};

    // equal: same or different?
    std::cout << "equal v1,v2: "
              << std::equal(v1.begin(), v1.end(), v2.begin(), v2.end())
              << '\n';  // 1

    // mismatch: find where they differ
    auto [it1, it3] = std::mismatch(v1.begin(), v1.end(), v3.begin());
    if (it1 != v1.end())
        std::cout << "mismatch at index "
                  << std::distance(v1.begin(), it1)
                  << ": " << *it1 << " vs " << *it3 << '\n';  // index 2: 3 vs 9

    // search: find subsequence
    auto found = std::search(v1.begin(), v1.end(), sub.begin(), sub.end());
    if (found != v1.end())
        std::cout << "sub found at index "
                  << std::distance(v1.begin(), found) << '\n';  // 1
}

// ============================================================
// SECTION 8: ISSST/DIS USE CASES
// ============================================================

// USE CASE 1: Compare two DIS PDU buffers
// bool pdusIdentical(const std::vector<uint8_t>& pdu1,
//                    const std::vector<uint8_t>& pdu2)
// {
//     return std::equal(pdu1.begin(), pdu1.end(),
//                       pdu2.begin(), pdu2.end());
// }
// // Use: detect if entity state changed (only send if different)
//
// USE CASE 2: Compare entity positions with tolerance
// bool positionsMatch(const std::vector<float>& pos1,
//                     const std::vector<float>& pos2)
// {
//     return std::equal(pos1.begin(), pos1.end(),
//                       pos2.begin(), pos2.end(),
//                       [](float a, float b){
//                           return std::abs(a-b) < 0.01f; // 1cm tolerance
//                       });
// }
//
// USE CASE 3: Check if exercise configuration matches expected
// bool configMatches(const std::vector<int>& loaded,
//                    const std::vector<int>& expected)
// {
//     return std::equal(loaded.begin(), loaded.end(),
//                       expected.begin(), expected.end());
// }

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does std::equal return?
//     -> bool: true if ALL corresponding elements are equal.
//        Short-circuits: returns false as soon as first mismatch found.

// Q2: What is the difference between 3-iterator and 4-iterator equal?
//     -> 3-iterator: equal(f1, l1, f2) - assumes range2 is long enough.
//        Undefined behavior if range2 is shorter than range1!
//        4-iterator: equal(f1, l1, f2, l2) - explicitly bounds both.
//        Returns false if sizes differ. ALWAYS prefer 4-iterator (C++14).

// Q3: What is the bug in test3?
//     -> Predicate uses i==j (case sensitive!)
//        'h' != 'H' so result is false even though intent was case-insensitive.
//        Fix: return std::tolower(i) == std::tolower(j);

// Q4: How do you use equal for case-insensitive string comparison?
//     -> std::equal(s1.begin(), s1.end(), s2.begin(), s2.end(),
//            [](char a, char b){ return std::tolower(a)==std::tolower(b); });

// Q5: What is the time complexity of std::equal?
//     -> O(n) where n = length of shorter range.
//        Best case O(1): sizes differ → false immediately.
//        Or: first elements differ → false immediately.

// Q6: How does equal differ from mismatch?
//     -> equal: returns bool (yes/no are they the same).
//        mismatch: returns iterators to WHERE they differ.
//        Use equal for "are they identical?", mismatch for "show me the diff".

// Q7: How do you check if a vector starts with another vector?
//     -> std::equal(prefix.begin(), prefix.end(),
//                   data.begin(), data.begin() + prefix.size())
//        Only compare [0, prefix.size()) of data.

// Q8: Can std::equal compare containers of different types?
//     -> YES! With predicate version.
//        vector<int> vs vector<double>:
//        equal(v.begin(), v.end(), d.begin(), d.end(),
//              [](int a, double b){ return a == b; });

int main()
{
    std::cout << "=== test0 (basic equal) ===\n";
    test0();  // 1 (true)

    std::cout << "\n=== test1 (partial range) ===\n";
    test1();  // 0 (false) - size mismatch

    std::cout << "\n=== test2 (predicate, same bug) ===\n";
    test2();  // 0 (false) - size mismatch

    std::cout << "\n=== test3 (case insensitive - BUGGY) ===\n";
    test3();  // 0 buggy, 1 fixed

    std::cout << "\n=== all variations ===\n";
    allVariations();

    std::cout << "\n=== algorithm comparison ===\n";
    compareAlgorithms();

    return 0;
}
// ```

// **Your test3 bug - the predicate does nothing different:**

// ```cpp
// // BUGGY predicate (same as default ==):
// [](char i, char j) { return i == j; }
// // 'h' == 'H' → false! (ASCII: 'h'=104, 'H'=72)
// // Result: false (NOT case-insensitive at all!)

// // FIXED predicate:
// [](char i, char j) { return std::tolower(i) == std::tolower(j); }
// // tolower('h')=h == tolower('H')=h → true ✅
// // Result: true (correctly case-insensitive)
// ```

// **All four test results explained:**

// ```
// test0: {1,2,3} vs {1,2,3}         → true  ✅ (identical)
// test1: {3} vs {1,2,3}             → false ✅ (size 1 ≠ size 3)
// test2: {3} vs {1,2,3} + predicate → false ✅ (size mismatch)
// test3: "hello" vs "HELLO" + ==    → false ❌ (BUG: should be true!)
// test3: "hello" vs "HELLO"+tolower → true  ✅ (fixed)
// ```

// > **The two rules for `std::equal`**: (1) always use the **4-iterator version** (`f1, l1, f2, l2`) to avoid undefined behavior on shorter ranges, and (2) when writing a custom predicate, make sure it actually does something different from `==` — otherwise you're just writing the default behavior with extra code. The case-insensitive string comparison is the canonical example where a predicate genuinely adds value.