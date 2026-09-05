#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdint>
// ============================================================
// std::mismatch - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS std::mismatch?
// ============================================================

// std::mismatch compares TWO ranges element by element
// -> finds the FIRST position where they DIFFER
// -> returns a std::pair of iterators:
//    result.first  = iterator into range 1 at mismatch point
//    result.second = iterator into range 2 at mismatch point
//
// IF NO MISMATCH (ranges are equal):
// -> result.first  = end of range 1
// -> result.second = corresponding position in range 2
//
// SIGNATURE:
// std::pair<It1, It2> std::mismatch(first1, last1, first2)
// std::pair<It1, It2> std::mismatch(first1, last1, first2, last2) // C++14
// std::pair<It1, It2> std::mismatch(first1, last1, first2, pred)  // custom compare
//
// COMPLEXITY: O(n) - linear scan until mismatch found

// ============================================================
// SECTION 2: BUG IN YOUR CODE + FIX
// ============================================================

// YOUR CODE HAS A BUG:
// auto result1 = std::mismatch(v1.begin(), v1.end(), v2.begin());
// if (*result1.first == *result1.second)  ← CRASH when NO mismatch!
// {
//     std::cout << "no mismatch found\n";
// }
// std::cout << *result1.first << std::endl;   ← CRASH: derefs end()!
// std::cout << *result1.second << std::endl;  ← CRASH: derefs past end()!
//
// WHEN v1 == v2 (mismatchTest(v1, v2)):
// result1.first  = v1.end()  (past-the-end iterator)
// *result1.first = UNDEFINED BEHAVIOR! (dereferencing end())
//
// RULE: ALWAYS CHECK if result == end() before dereferencing!

void mismatchTestFixed(const std::vector<int>& v1,
                       const std::vector<int>& v2)
{
    if (v1.size() != v2.size()) {
        std::cout << "Different sizes!\n";
        return;
    }

    auto [it1, it2] = std::mismatch(v1.begin(), v1.end(), v2.begin());
    // C++17 structured binding - cleaner than result1.first/second

    // CRITICAL: check for end() BEFORE dereferencing!
    if (it1 == v1.end()) {
        // reached end without mismatch = vectors are EQUAL
        std::cout << "No mismatch: vectors are identical\n";
        return;
    }

    // FOUND a mismatch - safe to dereference:
    std::cout << "Mismatch at index: "
              << std::distance(v1.begin(), it1) << '\n';
    std::cout << "v1 value: " << *it1  << '\n';
    std::cout << "v2 value: " << *it2  << '\n';
}

// ============================================================
// SECTION 3: WHAT mismatch RETURNS - ALL CASES
// ============================================================

void allCasesExample()
{
    // CASE 1: IDENTICAL vectors
    std::vector<int> a{1, 2, 3, 4};
    std::vector<int> b{1, 2, 3, 4};
    auto [ia, ib] = std::mismatch(a.begin(), a.end(), b.begin());
    // ia == a.end()   (no mismatch found)
    // ib == b.end()   (corresponding position)
    std::cout << "Case 1 - equal: "
              << (ia == a.end() ? "identical" : "mismatch") << '\n';

    // CASE 2: MISMATCH at index 1
    std::vector<int> c{1, 17, 3, 4};  // YOUR v3
    auto [ia2, ic] = std::mismatch(a.begin(), a.end(), c.begin());
    // ia2 points to a[1] (value 2)
    // ic  points to c[1] (value 17)
    std::cout << "Case 2 - mismatch at index: "
              << std::distance(a.begin(), ia2) << '\n';  // 1
    std::cout << "  a[1]=" << *ia2 << " c[1]=" << *ic << '\n'; // 2 vs 17

    // CASE 3: MISMATCH at first element
    std::vector<int> d{99, 2, 3, 4};
    auto [ia3, id] = std::mismatch(a.begin(), a.end(), d.begin());
    // ia3 points to a[0] (value 1)
    // id  points to d[0] (value 99)
    std::cout << "Case 3 - mismatch at index: "
              << std::distance(a.begin(), ia3) << '\n';  // 0

    // CASE 4: C++14 FOUR-ITERATOR version (safer for different sizes)
    std::vector<int> shorter{1, 2};
    auto [ia4, is] = std::mismatch(a.begin(), a.end(),
                                   shorter.begin(), shorter.end());
    // stops at end of SHORTER range
    // ia4 points to a[2] (value 3 - where shorter ran out)
    // is  == shorter.end()
}

// ============================================================
// SECTION 4: WITH CUSTOM PREDICATE
// ============================================================

void predicateExample()
{
    std::vector<int> v1{1, 2, 3, 4};
    std::vector<int> v2{1, -2, 3, 4};  // -2 vs 2

    // DEFAULT: finds mismatch where elements are NOT equal (!=)
    auto [it1_default, it2_default] =
        std::mismatch(v1.begin(), v1.end(), v2.begin());
    std::cout << "Default mismatch at: "
              << std::distance(v1.begin(), it1_default) << '\n';  // 1

    // CUSTOM: ignore sign, find mismatch where ABS values differ
    auto absEqual = [](int a, int b) {
        return std::abs(a) == std::abs(b);  // predicate: what counts as "equal"
    };
    auto [it1_abs, it2_abs] =
        std::mismatch(v1.begin(), v1.end(), v2.begin(), absEqual);
    // 1==1 ✓, |2|==|-2| ✓, 3==3 ✓, 4==4 ✓ → no mismatch!
    if (it1_abs == v1.end()) {
        std::cout << "Abs-equal: no mismatch (sign ignored)\n";
    }
}

// ============================================================
// SECTION 5: mismatch vs equal
// ============================================================

// std::equal: returns BOOL (are ranges equal?)
// std::mismatch: returns WHERE they differ (more info)
//
// std::equal(v1.begin(), v1.end(), v2.begin())
// -> true if all elements equal, false otherwise
// -> O(n) same complexity
// -> USE when: you only need "same or different?"
//
// std::mismatch(v1.begin(), v1.end(), v2.begin())
// -> returns pair of iterators to first difference
// -> O(n) same complexity
// -> USE when: you need to know WHERE and WHAT the difference is

void equalVsMismatch()
{
    std::vector<int> v1{1, 2, 3};
    std::vector<int> v2{1, 2, 4};

    // equal: just true/false
    bool same = std::equal(v1.begin(), v1.end(), v2.begin());
    std::cout << "equal: " << same << '\n';  // 0 (false)

    // mismatch: where + what
    auto [it1, it2] = std::mismatch(v1.begin(), v1.end(), v2.begin());
    if (it1 != v1.end()) {
        std::cout << "differ at index " << std::distance(v1.begin(), it1)
                  << ": " << *it1 << " vs " << *it2 << '\n';  // 2: 3 vs 4
    }
}

// ============================================================
// SECTION 6: REAL USE CASES
// ============================================================

// USE CASE 1: Find first version difference
void versionDiff()
{
    std::vector<int> v1{1, 2, 3};   // version 1.2.3
    std::vector<int> v2{1, 2, 5};   // version 1.2.5

    auto [it1, it2] = std::mismatch(v1.begin(), v1.end(), v2.begin());
    if (it1 != v1.end()) {
        int component = std::distance(v1.begin(), it1);
        std::string labels[] = {"major", "minor", "patch"};
        std::cout << "Versions differ at " << labels[component]
                  << ": " << *it1 << " vs " << *it2 << '\n';
    }
}

// USE CASE 2: Find first byte difference in binary data (DIS PDU diff)
void pduDiff()
{
    std::vector<uint8_t> pdu1{0x01, 0x01, 0x00, 0x58, 0xFF, 0x01};
    std::vector<uint8_t> pdu2{0x01, 0x01, 0x00, 0x58, 0xFF, 0x02};
    // identical except last byte (PDU type field changed)

    auto [it1, it2] = std::mismatch(pdu1.begin(), pdu1.end(), pdu2.begin());
    if (it1 != pdu1.end()) {
        std::cout << "PDUs differ at byte "
                  << std::distance(pdu1.begin(), it1) << '\n';  // 5
        std::cout << "0x" << std::hex << (int)*it1
                  << " vs 0x" << (int)*it2 << std::dec << '\n';
    }
}

// USE CASE 3: Longest common prefix (string or vector)
void longestCommonPrefix()
{
    std::vector<std::string> words1{"flower", "flow", "flight"};
    // find common prefix between "flower" and "flow":
    std::string s1 = "flower";
    std::string s2 = "flow";

    auto [it1, it2] = std::mismatch(s1.begin(), s1.end(), s2.begin());
    std::string prefix(s1.begin(), it1);
    std::cout << "Common prefix: " << prefix << '\n';  // "flow"
}

// USE CASE 4: Check if sequence STARTS WITH another (C++17 workaround)
bool startsWith(const std::vector<int>& data, const std::vector<int>& prefix)
{
    if (prefix.size() > data.size()) return false;
    auto [it1, it2] = std::mismatch(prefix.begin(), prefix.end(),
                                     data.begin());
    return it1 == prefix.end();  // no mismatch in prefix = starts with!
}

// ============================================================
// SECTION 7: YOUR CODE TRACE
// ============================================================

// mismatchTest(v1={1,2,3,4}, v2={1,2,3,4}):
// mismatch → it1=v1.end(), it2=v2.end()
// YOUR CODE: *result1.first == *result1.second
//   *v1.end() = UNDEFINED BEHAVIOR!
// FIX: check it1 == v1.end() FIRST
//
// mismatchTest(v1={1,2,3,4}, v3={1,17,3,4}):
// element 0: 1==1 ✓
// element 1: 2!=17 ← MISMATCH HERE
// it1 → v1[1]=2, it2 → v3[1]=17
// YOUR CODE: *result1.first=2, *result1.second=17 (works for this case)
// But previous call with equal vectors causes UB!

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does std::mismatch return?
//     -> std::pair<It1, It2>: iterators to the first mismatch position
//        in both ranges. If no mismatch: first iterator = end of range 1.

// Q2: What is the bug in the original code?
//     -> Dereferencing result.first when it equals end() (no mismatch case).
//        Dereferencing end() = undefined behavior (crash or garbage).
//        Fix: always check if (it1 == v1.end()) before *it1.

// Q3: What is the difference between mismatch and equal?
//     -> equal: returns bool (same or different?).
//        mismatch: returns WHERE and WHAT the difference is.
//        mismatch gives more information at same O(n) cost.

// Q4: What does the custom predicate parameter do for mismatch?
//     -> Replaces the default equality check (==).
//        pred(a, b) returns true if elements are "equal" (match).
//        Useful for: case-insensitive compare, absolute value, fuzzy match.

// Q5: When should you use the 4-iterator version of mismatch?
//     -> When ranges might have different sizes.
//        mismatch(f1, l1, f2, l2): stops at end of SHORTER range.
//        3-iterator version: undefined behavior if range2 is shorter!

// Q6: How do you find how many elements match at the start?
//     -> auto [it1, it2] = mismatch(v1.begin(), v1.end(), v2.begin());
//        int matchCount = std::distance(v1.begin(), it1);
//        This is the length of the common prefix.

// Q7: Can mismatch work on strings?
//     -> YES. std::string has begin()/end() iterators.
//        std::mismatch(s1.begin(), s1.end(), s2.begin())
//        Useful for: finding where two strings first differ,
//        longest common prefix, starts-with check.

// Q8: What is a practical use in your DIS simulation?
//     -> Compare two versions of an EntityState PDU:
//        mismatch(oldPDU.begin(), oldPDU.end(), newPDU.begin())
//        → find exactly which bytes changed
//        → only update changed fields (efficient differential update)

int main()
{
    std::vector v1{1, 2, 3, 4};
    std::vector v2{1, 2, 3, 4};
    std::vector v3{1, 17, 3, 4};

    std::cout << "=== v1 vs v2 ===\n";
    mismatchTestFixed(v1, v2);

    std::cout << "\n=== v1 vs v3 ===\n";
    mismatchTestFixed(v1, v3);

    std::cout << "\n=== All cases ===\n";
    allCasesExample();

    std::cout << "\n=== Predicate ===\n";
    predicateExample();

    std::cout << "\n=== Version diff ===\n";
    versionDiff();

    std::cout << "\n=== Starts with ===\n";
    std::vector<int> data{1, 2, 3, 4, 5};
    std::vector<int> prefix{1, 2, 3};
    std::cout << "startsWith: " << startsWith(data, prefix) << '\n'; // 1

    return 0;
}
// ```

// **The mismatch return values:**

// ```
// v1 = {1, 2, 3, 4}
// v2 = {1, 2, 3, 4}   → it1 = v1.end(),  it2 = v2.end()  (no mismatch)
// v3 = {1, 17, 3, 4}  → it1 = &v1[1]=2,  it2 = &v3[1]=17 (mismatch at index 1)

// ALWAYS check before dereferencing:
// if (it1 == v1.end()) → identical (no mismatch)
// else                  → *it1 and *it2 are safe to read
// ```

// **The bug visualized:**

// ```cpp
// // v1 == v2 (identical):
// auto [it1, it2] = mismatch(v1.begin(), v1.end(), v2.begin());
// // it1 = v1.end() (past the last element!)

// *it1  // CRASH! dereferencing end() = undefined behavior

// // FIX:
// if (it1 == v1.end()) { cout << "identical\n"; return; }
// cout << *it1;  // now safe ✅
// ```

// > **The one rule**: `std::mismatch` always returns `end()` in the "no mismatch" case — that's how it signals equality. Dereferencing `end()` is always undefined behavior. The pattern is: `auto [it1, it2] = mismatch(...); if (it1 == v1.end()) { /* equal */ } else { /* use *it1 and *it2 safely */ }`. This same pattern applies to `std::find`, `std::search`, and all STL algorithms that return iterators.