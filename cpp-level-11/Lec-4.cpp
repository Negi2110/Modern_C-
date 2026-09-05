#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cctype>
#include <cstdint>
// ============================================================
// std::lexicographical_compare - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS LEXICOGRAPHICAL ORDERING?
// ============================================================

// LEXICOGRAPHICAL = dictionary order
// -> compare element by element from LEFT to RIGHT
// -> first DIFFERENCE decides the result
// -> if all elements equal but lengths differ: shorter < longer
//
// DICTIONARY EXAMPLE:
// "apple" < "application" (apple is shorter, prefix matches)
// "apple" < "banana"      (a < b at position 0)
// "cat"   < "dog"         (c < d at position 0)
// "abc"   < "abd"         (a==a, b==b, c < d at position 2)
//
// IT'S LIKE SORTING IN A REAL DICTIONARY:
// "Apple" comes before "microsoft"?
// 'A'=65 vs 'm'=109 (ASCII) → A < m → YES, Apple comes first!
// BUT: 'A'=65 vs 'a'=97 → uppercase < lowercase!
// -> "Apple" < "apple" (uppercase A < lowercase a)
// -> "Microsoft" < "apple" (uppercase M < lowercase a!)
//
// std::lexicographical_compare returns:
// TRUE  if range1 < range2 (range1 comes FIRST in dictionary)
// FALSE if range1 >= range2 (range1 comes AFTER or is equal)

// ============================================================
// SECTION 2: SIGNATURE AND VERSIONS
// ============================================================

// VERSION 1: Default (uses operator<):
// bool lexicographical_compare(It1 f1, It1 l1, It2 f2, It2 l2)
//
// VERSION 2: Custom comparator:
// bool lexicographical_compare(It1 f1, It1 l1, It2 f2, It2 l2, Compare comp)
// comp(a, b) returns true if a < b
//
// RETURNS:
// true  = range1 is LEXICOGRAPHICALLY LESS THAN range2
// false = range1 is GREATER THAN or EQUAL TO range2
//
// COMPLEXITY: O(min(n,m)) where n,m = sizes of ranges

// ============================================================
// SECTION 3: test0 - STRING COMPARISON
// ============================================================

void test0()
{
    std::string s1 = "Apple";
    std::string s2 = "microsoft";

    bool result = std::lexicographical_compare(
        s1.begin(), s1.end(),
        s2.begin(), s2.end());

    std::cout << "Apple before microsoft? " << result << '\n';

    // HOW IT WORKS STEP BY STEP:
    // Position 0: 'A' vs 'm'
    // ASCII: 'A'=65, 'm'=109
    // 65 < 109 → TRUE! (A comes before m)
    // → result = true (Apple < microsoft in ASCII order)
    //
    // IMPORTANT NOTE: 'A'=65, 'a'=97 (uppercase < lowercase)
    // "Apple"     < "apple"     (A < a)
    // "Microsoft" < "apple"     (M=77 < a=97)
    // "apple"     < "microsoft" (a=97 < m=109)
    // This is NOT case-insensitive!

    // WHAT IF SAME UP TO ONE STRING'S END?
    std::string shorter = "App";
    std::string longer  = "Apple";
    bool r2 = std::lexicographical_compare(
        shorter.begin(), shorter.end(),
        longer.begin(),  longer.end());
    std::cout << "App before Apple? " << r2 << '\n';  // TRUE
    // A==A, p==p, p==p → shorter runs out → shorter < longer
}

// ============================================================
// SECTION 4: test1 - VECTOR COMPARISON
// ============================================================

void test1()
{
    std::vector v1{2, 4, 3, 8, 12};
    std::vector v2{2, 4, 4, 8};

    bool result = std::lexicographical_compare(
        v1.begin(), v1.end(),
        v2.begin(), v2.end());

    std::cout << "v1 before v2 lexicographically? " << result << '\n';

    // HOW IT WORKS STEP BY STEP:
    // Position 0: v1[0]=2 vs v2[0]=2 → EQUAL, continue
    // Position 1: v1[1]=4 vs v2[1]=4 → EQUAL, continue
    // Position 2: v1[2]=3 vs v2[2]=4 → 3 < 4 → TRUE!
    //
    // RESULT: true (v1 < v2, v1 comes "before" v2)
    //
    // NOTE: v1 has MORE elements (5 vs 4) but that doesn't matter
    // The FIRST DIFFERENCE at position 2 decides: 3 < 4 → v1 < v2

    // ANOTHER EXAMPLE: equal prefix, different length
    std::vector<int> a{1, 2, 3};
    std::vector<int> b{1, 2, 3, 4};  // longer
    bool r2 = std::lexicographical_compare(
        a.begin(), a.end(),
        b.begin(), b.end());
    std::cout << "{1,2,3} before {1,2,3,4}? " << r2 << '\n';  // TRUE
    // 1==1, 2==2, 3==3 → a runs out → shorter < longer = TRUE

    // EQUAL SEQUENCES:
    std::vector<int> c{1, 2, 3};
    std::vector<int> d{1, 2, 3};
    bool r3 = std::lexicographical_compare(
        c.begin(), c.end(),
        d.begin(), d.end());
    std::cout << "{1,2,3} before {1,2,3}? " << r3 << '\n';  // FALSE (equal!)
}

// ============================================================
// SECTION 5: THE COMPARISON ALGORITHM LOGIC
// ============================================================

// INTERNAL LOGIC (what the algorithm does):
// for (auto i1=f1, i2=f2; i1!=l1 && i2!=l2; ++i1, ++i2) {
//     if (*i1 < *i2) return true;   // range1 element is smaller → range1 wins
//     if (*i2 < *i1) return false;  // range2 element is smaller → range2 wins
//     // if equal: continue to next element
// }
// // Reached end of at least one range:
// return (i1 == l1) && (i2 != l2);
// // true only if range1 ran out BUT range2 still has elements
// // (shorter prefix < longer = true)
// // if both ran out: they're equal → return false

// DECISION TABLE:
// Element comparison   │ Returns
// ─────────────────────┼──────────────────
// range1[i] < range2[i]│ true  immediately
// range1[i] > range2[i]│ false immediately
// range1[i] == range2[i]│ continue...
// range1 exhausted first│ true  (shorter < longer)
// range2 exhausted first│ false (longer > shorter)
// Both exhausted (equal)│ false (equal, not less)

// ============================================================
// SECTION 6: CASE-INSENSITIVE VERSION (custom comparator)
// ============================================================

void caseInsensitiveExample()
{
    std::string s1 = "Apple";
    std::string s2 = "apple";

    // DEFAULT (case-sensitive): 'A'=65 < 'a'=97 → Apple < apple
    bool caseSensitive = std::lexicographical_compare(
        s1.begin(), s1.end(),
        s2.begin(), s2.end());
    std::cout << "Case-sensitive: Apple < apple? "
              << caseSensitive << '\n';  // 1 (true)

    // CUSTOM (case-insensitive): tolower both before comparing
    auto caseInsensitiveCmp = [](char a, char b) {
        return std::tolower(a) < std::tolower(b);
    };

    bool caseInsensitive = std::lexicographical_compare(
        s1.begin(), s1.end(),
        s2.begin(), s2.end(),
        caseInsensitiveCmp);
    std::cout << "Case-insensitive: Apple < apple? "
              << caseInsensitive << '\n';  // 0 (false - they're equal!)

    // ANOTHER EXAMPLE:
    std::string s3 = "banana";
    std::string s4 = "CHERRY";
    bool r = std::lexicographical_compare(
        s3.begin(), s3.end(),
        s4.begin(), s4.end(),
        caseInsensitiveCmp);
    std::cout << "banana < CHERRY (case-insensitive)? " << r << '\n';
    // tolower: 'b' < 'c' → true (banana comes before cherry)
}

// ============================================================
// SECTION 7: ABSOLUTE VALUE COMPARATOR (for vectors)
// ============================================================

void absComparatorExample()
{
    std::vector<int> v1{-5, 2, -3};
    std::vector<int> v2{-4, 2, -3};

    // DEFAULT: -5 < -4 → true
    bool r1 = std::lexicographical_compare(
        v1.begin(), v1.end(),
        v2.begin(), v2.end());
    std::cout << "Default: " << r1 << '\n';  // 1 (true: -5 < -4)

    // ABSOLUTE VALUE: compare by magnitude
    auto absCmp = [](int a, int b) {
        return std::abs(a) < std::abs(b);
    };
    bool r2 = std::lexicographical_compare(
        v1.begin(), v1.end(),
        v2.begin(), v2.end(),
        absCmp);
    std::cout << "Abs value: " << r2 << '\n';
    // abs(-5)=5 > abs(-4)=4 → false! (|-5| > |-4|)
}

// ============================================================
// SECTION 8: PRACTICAL USE CASES
// ============================================================

// USE CASE 1: Sorting strings (std::sort uses this internally)
void sortingExample()
{
    std::vector<std::string> words{"banana", "apple", "cherry", "date"};

    std::sort(words.begin(), words.end());
    // std::sort uses operator< which uses lexicographical comparison!
    // After: {apple, banana, cherry, date}

    for (const auto& w : words)
        std::cout << w << " ";
    std::cout << '\n';
}

// USE CASE 2: Custom sort order
void customSortExample()
{
    std::vector<std::string> words{"Banana", "apple", "Cherry", "date"};

    // Case-insensitive sort:
    std::sort(words.begin(), words.end(),
        [](const std::string& a, const std::string& b) {
            return std::lexicographical_compare(
                a.begin(), a.end(),
                b.begin(), b.end(),
                [](char x, char y) {
                    return std::tolower(x) < std::tolower(y);
                });
        });
    // After: {apple, Banana, Cherry, date} (case-insensitive order)

    for (const auto& w : words)
        std::cout << w << " ";
    std::cout << '\n';
}

// USE CASE 3: Version string comparison
void versionExample()
{
    std::string v1 = "1.2.3";
    std::string v2 = "1.2.10";  // PROBLEM: '1' < '3' lexicographically!

    bool result = std::lexicographical_compare(
        v1.begin(), v1.end(),
        v2.begin(), v2.end());
    std::cout << "1.2.3 < 1.2.10 (lex)? " << result << '\n';
    // FALSE! '3' > '1' at the last component
    // For version comparison: need to parse numbers properly
    // lexicographical_compare is NOT suitable for version numbers!
    // Use semantic version parsing instead
}

// USE CASE 4: DIS PDU sequence ordering
void disExample()
{
    // Compare DIS PDU identifiers lexicographically
    // Entity ID: {site, application, entity}
    std::vector<uint16_t> entity1{1, 2, 5};   // site 1, app 2, entity 5
    std::vector<uint16_t> entity2{1, 2, 10};  // site 1, app 2, entity 10

    bool result = std::lexicographical_compare(
        entity1.begin(), entity1.end(),
        entity2.begin(), entity2.end());
    std::cout << "Entity (1,2,5) before (1,2,10)? " << result << '\n';
    // 1==1, 2==2, 5 < 10 → TRUE: entity1 < entity2
}

// ============================================================
// SECTION 9: THREE-WAY COMPARISON (C++20)
// ============================================================

// C++20: std::lexicographical_compare_three_way
// Returns: strong_ordering / weak_ordering / partial_ordering
//
// < 0: range1 is less than range2
// = 0: ranges are equal
// > 0: range1 is greater than range2
//
// EXAMPLE:
// auto result = std::lexicographical_compare_three_way(
//     s1.begin(), s1.end(),
//     s2.begin(), s2.end());
// if (result < 0) cout << "s1 < s2";
// if (result == 0) cout << "s1 == s2";
// if (result > 0) cout << "s1 > s2";
//
// BENEFIT over lexicographical_compare:
// -> Tells you LESS THAN, EQUAL, or GREATER in one call
// -> vs having to call lexicographical_compare TWICE for full 3-way

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What does std::lexicographical_compare do?
//     -> Compares two ranges in dictionary order.
//        Compares element by element left to right.
//        Returns true if range1 < range2 (range1 comes first).
//        First difference determines result; shorter < longer if prefix matches.

// Q2: What does it return when ranges are equal?
//     -> FALSE. "less than" comparison: equal is NOT less than.
//        Same as: 5 < 5 = false.
//        If you need equality: combine with reverse call or use equal().

// Q3: What is the ASCII ordering trap with strings?
//     -> Uppercase letters < lowercase (A=65 < a=97).
//        "Apple" < "apple" (A < a).
//        "Zebra" < "ant"   (Z=90 < a=97).
//        Use custom comparator with tolower() for case-insensitive.

// Q4: What happens when ranges have different lengths?
//     -> Shorter range < longer range IF they share the same prefix.
//        "App" < "Apple" (prefix matches, shorter ran out first).
//        "App" < "Appz" would be different (z vs nothing).

// Q5: How do you make it case-insensitive?
//     -> Use custom comparator:
//        [](char a, char b){ return tolower(a) < tolower(b); }
//        Pass as 5th argument to lexicographical_compare.

// Q6: When should you NOT use lexicographical_compare?
//     -> Version numbers: "1.2.10" < "1.2.9" lexicographically (wrong!).
//        Numeric strings: "10" < "9" lexicographically (wrong!).
//        Use it for: string sorting, multi-key tuple comparison,
//        generic sequence ordering.

// Q7: What is the difference between lexicographical_compare and equal?
//     -> equal: returns bool (are they the same?).
//        lexicographical_compare: returns bool (is range1 LESS THAN range2?).
//        For full 3-way: call compare twice or use C++20 three_way version.

// Q8: How does std::string's operator< relate to lexicographical_compare?
//     -> std::string::operator< IS lexicographical comparison!
//        s1 < s2 is equivalent to:
//        lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end())
//        std::sort on strings uses this automatically.

int main()
{
    std::cout << "=== test0: String comparison ===\n";
    test0();
    // "Apple comes before microsoft? 1 (true)
    // 'A'=65 < 'm'=109 → decided at first character

    std::cout << "\n=== test1: Vector comparison ===\n";
    test1();
    // v1={2,4,3,8,12} vs v2={2,4,4,8}
    // 2==2, 4==4, 3<4 → TRUE at position 2

    std::cout << "\n=== Case-insensitive ===\n";
    caseInsensitiveExample();

    std::cout << "\n=== Sorting ===\n";
    sortingExample();

    std::cout << "\n=== Custom sort ===\n";
    customSortExample();

    return 0;
}
// ```

// **The decision algorithm step by step:**

// ```
// v1 = {2, 4, 3, 8, 12}
// v2 = {2, 4, 4, 8}

// Step 0: v1[0]=2 vs v2[0]=2 → EQUAL, continue
// Step 1: v1[1]=4 vs v2[1]=4 → EQUAL, continue
// Step 2: v1[2]=3 vs v2[2]=4 → 3 < 4 → return TRUE ← STOPS HERE!
// (v1[3], v1[4], v2[3] are NEVER compared)
// ```

// **ASCII ordering trap:**

// ```
// Uppercase: A=65  B=66  ... Z=90
// Lowercase: a=97  b=98  ... z=122

// "Apple"     vs  "apple"     → 'A'(65) < 'a'(97) → Apple FIRST
// "Microsoft" vs  "apple"     → 'M'(77) < 'a'(97) → Microsoft FIRST ← surprising!
// "Zebra"     vs  "ant"       → 'Z'(90) < 'a'(97) → Zebra FIRST    ← surprising!

// FIX: use tolower() in custom comparator for true alphabetical order
// ```

// **What the return value means:**

// ```cpp
// bool result = std::lexicographical_compare(r1, r2);

// result = true  → r1  < r2  (r1 comes FIRST in dictionary)
// result = false → r1 >= r2  (r1 comes AFTER, or they're EQUAL)

// // To get GREATER THAN:
// bool greaterThan = std::lexicographical_compare(r2.begin(), r2.end(),
//                                                 r1.begin(), r1.end());
// // Swap arguments to reverse the comparison!
// ```

// > **The practical rule**: `std::lexicographical_compare` is exactly what `std::string::operator<` uses internally, which is what `std::sort` uses when sorting strings. For strings with consistent casing it works perfectly. The main gotcha is **uppercase < lowercase** in ASCII ordering — if you mix cases, always provide a `tolower` comparator. For numeric strings or version numbers, don't use it at all — parse the numbers first.