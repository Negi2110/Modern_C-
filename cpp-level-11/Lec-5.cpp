#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cctype>

// ============================================================
// std::all_of, any_of, none_of - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT ARE THESE ALGORITHMS?
// ============================================================

// YOUR COMMENT: "useful for querying about sequences"
//
// THREE BOOLEAN QUERY ALGORITHMS:
// all_of  -> does EVERY element satisfy the condition?
// any_of  -> does AT LEAST ONE element satisfy the condition?
// none_of -> does NO element satisfy the condition?
//
// ALL THREE:
// -> take a range [first, last)
// -> take a PREDICATE (lambda/function returning bool)
// -> return BOOL
// -> SHORT-CIRCUIT (stop as soon as answer is known)
// -> O(n) worst case, O(1) best case
// -> use cbegin/cend (YOUR CODE) for read-only (correct!)
//
// INTRODUCED: C++11
// HEADER: <algorithm>

// ============================================================
// SECTION 2: SIGNATURES
// ============================================================

// template<class InputIt, class UnaryPred>
// bool all_of(InputIt first, InputIt last, UnaryPred pred);
//
// template<class InputIt, class UnaryPred>
// bool any_of(InputIt first, InputIt last, UnaryPred pred);
//
// template<class InputIt, class UnaryPred>
// bool none_of(InputIt first, InputIt last, UnaryPred pred);
//
// pred = any callable: lambda, function pointer, functor
// pred(element) must return bool (or convertible to bool)

// ============================================================
// SECTION 3: test0 - std::all_of
// ============================================================

void test0()
{
    std::vector v1{1, 3, 5, 7};

    bool result = std::all_of(v1.cbegin(), v1.cend(),
        [](int i) { return i > 0; });

    std::cout << "all_of > 0: " << result << '\n';

    // HOW IT WORKS:
    // checks: 1>0? YES, 3>0? YES, 5>0? YES, 7>0? YES
    // ALL passed → TRUE
    //
    // SHORT-CIRCUIT: if ANY element FAILS → return false immediately
    // doesn't check remaining elements
    //
    // INTERNAL EQUIVALENT:
    // for (auto& e : v1) {
    //     if (!pred(e)) return false;  // short-circuit on FAILURE
    // }
    // return true;  // all passed
    //
    // SPECIAL CASE - EMPTY RANGE:
    // all_of on empty range → TRUE (vacuously true)
    // "all 0 elements satisfy the condition" = true by definition!
    // std::vector<int> empty;
    // std::all_of(empty.begin(), empty.end(), pred) → true ALWAYS
}

// ============================================================
// SECTION 4: test1 - std::any_of
// ============================================================

void test1()
{
    std::vector v1{-1, -3, 5, 7};

    bool result = std::any_of(v1.cbegin(), v1.cend(),
        [](int i) { return i > 0; });

    std::cout << "any_of > 0: " << result << '\n';

    // HOW IT WORKS:
    // checks: -1>0? NO, -3>0? NO, 5>0? YES → return true immediately!
    // 7 is NEVER checked (short-circuit on FIRST SUCCESS)
    //
    // SHORT-CIRCUIT: stops at FIRST element that PASSES
    //
    // INTERNAL EQUIVALENT:
    // for (auto& e : v1) {
    //     if (pred(e)) return true;  // short-circuit on SUCCESS
    // }
    // return false;  // none passed
    //
    // SPECIAL CASE - EMPTY RANGE:
    // any_of on empty range → FALSE
    // "at least one of 0 elements satisfies condition" = false
    // std::vector<int> empty;
    // std::any_of(empty.begin(), empty.end(), pred) → false ALWAYS
}

// ============================================================
// SECTION 5: test2 - std::none_of
// ============================================================

void test2()
{
    std::vector v1{-1, -3, -5, 7};  // 7 is > 0!

    bool result = std::none_of(v1.cbegin(), v1.cend(),
        [](int i) { return i > 0; });

    std::cout << "none_of > 0: " << result << '\n';

    // HOW IT WORKS:
    // checks: -1>0? NO, -3>0? NO, -5>0? NO, 7>0? YES → return false!
    // "7 satisfies the condition" → none_of = FALSE
    // result = false (7 IS > 0, so it's NOT true that none are > 0)
    //
    // WAIT: your test2 has 7 in the vector!
    // v1{-1, -3, -5, 7} → 7 > 0 is TRUE
    // none_of returns FALSE (7 passes the predicate → NOT none_of!)
    //
    // IF INTENT was all negative: use v1{-1, -3, -5, -7}
    // then none_of > 0 → TRUE
    //
    // SHORT-CIRCUIT: stops at FIRST element that PASSES (same as any_of!)
    //
    // INTERNAL EQUIVALENT:
    // for (auto& e : v1) {
    //     if (pred(e)) return false;  // found one → NOT none_of
    // }
    // return true;  // none passed
    //
    // SPECIAL CASE - EMPTY RANGE:
    // none_of on empty range → TRUE
    // "none of 0 elements satisfy condition" = trivially true

    // FIX: what test2 PROBABLY intended:
    std::vector v2{-1, -3, -5, -7};  // all negative
    bool fixedResult = std::none_of(v2.cbegin(), v2.cend(),
        [](int i) { return i > 0; });
    std::cout << "none_of > 0 (all negative): " << fixedResult << '\n'; // 1
}

// ============================================================
// SECTION 6: SHORT-CIRCUIT SUMMARY
// ============================================================

// all_of:
// -> returns FALSE immediately when first element FAILS predicate
// -> returns TRUE only after ALL elements checked and PASSED
// -> worst case: all pass (checks all n elements)
//
// any_of:
// -> returns TRUE immediately when first element PASSES predicate
// -> returns FALSE only after ALL elements checked and FAILED
// -> worst case: all fail (checks all n elements)
//
// none_of:
// -> returns FALSE immediately when first element PASSES predicate
// -> returns TRUE only after ALL elements checked and FAILED
// -> worst case: all fail (checks all n elements)
//
// PERFORMANCE TIP:
// Put most likely TRUE/FALSE at the FRONT of the range:
// all_of:  put most likely FAILING element FIRST → exits sooner
// any_of:  put most likely PASSING element FIRST → exits sooner
// none_of: put most likely PASSING element FIRST → exits sooner

// ============================================================
// SECTION 7: RELATIONSHIP BETWEEN THE THREE
// ============================================================

// MATHEMATICAL RELATIONSHIPS:
// all_of(r, pred)  == !any_of(r, [pred](x){ return !pred(x); })
// none_of(r, pred) == !any_of(r, pred)
// none_of(r, pred) ==  all_of(r, [pred](x){ return !pred(x); })
//
// TRUTH TABLE for {-1, -3, 5, 7} with pred: x > 0:
// all_of  → false (not ALL are > 0: -1 fails)
// any_of  → true  (SOME are > 0: 5 and 7)
// none_of → false (NOT none are > 0: 5 and 7 satisfy it)
//
// ONLY POSSIBLE CONSISTENT COMBINATIONS:
// all true:  all_of=T, any_of=T, none_of=F  (all elements pass)
// mixed:     all_of=F, any_of=T, none_of=F  (some pass, some fail)
// all false: all_of=F, any_of=F, none_of=T  (no elements pass)
// empty:     all_of=T, any_of=F, none_of=T  (edge case!)

// ============================================================
// SECTION 8: REAL USE CASES
// ============================================================

void realUseCases()
{
    // USE CASE 1: Validate all inputs
    std::vector<int> ages{25, 30, 18, 45, 22};
    bool allAdults = std::all_of(ages.begin(), ages.end(),
        [](int age) { return age >= 18; });
    std::cout << "All adults (>=18): " << allAdults << '\n';  // 1

    // USE CASE 2: Check if any element needs attention
    std::vector<int> temperatures{36, 37, 39, 36, 38};
    bool fever = std::any_of(temperatures.begin(), temperatures.end(),
        [](int t) { return t >= 39; });
    std::cout << "Any fever (>=39): " << fever << '\n';  // 1

    // USE CASE 3: Ensure no invalid values
    std::vector<int> scores{85, 92, 78, 88, 95};
    bool noInvalid = std::none_of(scores.begin(), scores.end(),
        [](int s) { return s < 0 || s > 100; });
    std::cout << "No invalid scores: " << noInvalid << '\n';  // 1

    // USE CASE 4: String validation
    std::string password = "SecurePass123!";
    bool hasUpper = std::any_of(password.begin(), password.end(), ::isupper);
    bool hasDigit = std::any_of(password.begin(), password.end(), ::isdigit);
    bool hasSpecial = std::any_of(password.begin(), password.end(),
        [](char c) { return !::isalnum(c); });
    bool allPrintable = std::all_of(password.begin(), password.end(), ::isprint);

    std::cout << "Has uppercase: " << hasUpper   << '\n';    // 1
    std::cout << "Has digit: "     << hasDigit   << '\n';    // 1
    std::cout << "Has special: "   << hasSpecial << '\n';    // 1 (!)
    std::cout << "All printable: " << allPrintable << '\n';  // 1
}

// ============================================================
// SECTION 9: ISSST/DIS USE CASES
// ============================================================

// In your DIS simulation:

// struct Entity { uint32_t id; bool active; float x,y,z; float health; };
// std::vector<Entity> entities;
//
// Check if ALL entities are active:
// bool allActive = std::all_of(entities.begin(), entities.end(),
//     [](const Entity& e) { return e.active; });
//
// Check if ANY entity is in danger zone:
// bool dangerZone = std::any_of(entities.begin(), entities.end(),
//     [](const Entity& e) {
//         return e.x > 500.0f && e.y > 500.0f;  // in zone?
//     });
//
// Verify NO entity has invalid position (NaN check):
// bool noNaN = std::none_of(entities.begin(), entities.end(),
//     [](const Entity& e) {
//         return std::isnan(e.x) || std::isnan(e.y) || std::isnan(e.z);
//     });
//
// Check if exercise can START (all simulators connected):
// std::vector<bool> connected = {true, true, true, false};
// bool readyToStart = std::all_of(connected.begin(), connected.end(),
//     [](bool c) { return c; });
//
// Check if any PDU exceeds size limit:
// bool oversized = std::any_of(pdus.begin(), pdus.end(),
//     [](const PDU& p) { return p.size() > 1400; }); // MTU safe limit

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What do all_of, any_of, none_of return for an empty range?
//     -> all_of:  TRUE  (vacuously: ALL 0 elements satisfy = true)
//        any_of:  FALSE (none of 0 elements satisfy = false)
//        none_of: TRUE  (none of 0 elements satisfy = true)
//        This is mathematically consistent and important to know!

// Q2: What is short-circuit evaluation in these algorithms?
//     -> all_of:  stops at first FAILING element → no need to check rest
//        any_of:  stops at first PASSING element → no need to check rest
//        none_of: stops at first PASSING element → no need to check rest
//        Important for performance: early exit saves time on large ranges.

// Q3: What is the bug in test2?
//     -> v1{-1, -3, -5, 7} contains 7 which IS > 0.
//        none_of(pred: x>0) returns FALSE (7 satisfies the predicate).
//        To test "none are positive": vector should be all negative.
//        To print result correctly: check what none_of actually means.

// Q4: How do all_of and none_of relate mathematically?
//     -> none_of(r, pred) == all_of(r, !pred)
//        none_of(v, x>0)  == all_of(v, x<=0)
//        Also: none_of(r, pred) == !any_of(r, pred)

// Q5: Can you use function pointers instead of lambdas?
//     -> YES! any standard callable works:
//        std::any_of(s.begin(), s.end(), ::isupper)  // function pointer
//        std::all_of(v.begin(), v.end(), isPositive) // named function
//        std::none_of(v.begin(), v.end(), MyPred{})  // functor

// Q6: What is the time complexity?
//     -> O(n) worst case (must check all elements).
//        O(1) best case (first element determines result via short-circuit).
//        Use cbegin/cend (YOUR CODE) for const access - correct practice!

// Q7: How do these differ from std::count_if?
//     -> all_of/any_of/none_of: return BOOL (yes/no query).
//        count_if: returns HOW MANY elements satisfy predicate.
//        Use all/any/none when you don't need the count, just the bool.
//        Bonus: all/any/none short-circuit; count_if ALWAYS scans all.

// Q8: How would you implement any_of manually?
//     -> template<class It, class Pred>
//        bool any_of(It first, It last, Pred p) {
//            for (; first != last; ++first)
//                if (p(*first)) return true;  // short-circuit!
//            return false;
//        }

int main()
{
    std::cout << "=== test0: all_of ===\n";
    test0();  // 1 (all elements > 0)

    std::cout << "\n=== test1: any_of ===\n";
    test1();  // 1 (5 and 7 are > 0)

    std::cout << "\n=== test2: none_of ===\n";
    test2();  // 0 (7 > 0 → NOT none_of!)

    std::cout << "\n=== real use cases ===\n";
    realUseCases();

    return 0;
}
// ```

// **The three algorithms - visual summary:**

// ```
// Vector: {-1, -3, 5, 7}   Predicate: x > 0

// all_of:  -1>0? NO → FALSE ← stops here! (short-circuit on failure)
// any_of:  -1>0? NO, -3>0? NO, 5>0? YES → TRUE ← stops here!
// none_of: -1>0? NO, -3>0? NO, 5>0? YES → FALSE ← stops here!
// ```

// **Empty range edge cases (interview favorite!):**

// ```cpp
// std::vector<int> empty{};

// std::all_of(empty.begin(), empty.end(), pred)  → true  ← "vacuously true"
// std::any_of(empty.begin(), empty.end(), pred)  → false ← "no element satisfies"
// std::none_of(empty.begin(), empty.end(), pred) → true  ← "no element fails"
// ```

// **The bug in test2:**

// ```
// YOUR CODE:  v1{-1, -3, -5, 7}   ← 7 IS positive!
// none_of(x>0) → false             ← 7 satisfies x>0 → NOT none_of!

// INTENDED?:  v1{-1, -3, -5, -7}  ← all negative
// none_of(x>0) → true              ← none are positive ✅
// ```

// > **The one-liner**: `all_of`, `any_of`, `none_of` are the STL equivalent of asking "ALL?", "EXISTS?", "NOTHING?" over a collection. They short-circuit for performance, work with any predicate, and their edge case behavior on empty ranges (`all=true`, `any=false`, `none=true`) is mathematically consistent and important to remember for interviews.