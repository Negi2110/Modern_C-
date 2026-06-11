// ============================================================
// LVALUE, RVALUE, LVALUE REF, RVALUE REF - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <string>

// ============================================================
// SECTION 1: LVALUE vs RVALUE - FROM FIRST PRINCIPLES
// ============================================================

// Original meaning (C era):
// lvalue = what appears on LEFT  side of =
// rvalue = what appears on RIGHT side of =
//
// Modern C++ meaning (more accurate):
// lvalue = has a PERSISTENT MEMORY LOCATION (address you can take)
// rvalue = TEMPORARY value, no persistent address, exists momentarily
//
// SIMPLE TEST:
// Can you take its address with &?
// YES -> lvalue
// NO  -> rvalue
//
// int i = 10;
// &i          -> VALID   -> i is lvalue (has address on stack)
// &10         -> INVALID -> 10 is rvalue (no address, just a literal)
// &(a+b)      -> INVALID -> (a+b) is rvalue (temporary result)
// &get42()    -> INVALID -> return value is rvalue (temporary)

// ============================================================
// SECTION 2: EXAMPLES - LVALUE vs RVALUE
// ============================================================

// int a = 1;       a  = lvalue (has address)
//                  1  = rvalue (literal, no address)
//
// int b = 2;       b  = lvalue
//                  2  = rvalue
//
// int c = (a+b);   c  = lvalue
//                 (a+b) = rvalue (temporary, computed, discarded)
//
// int array[100];
// array[10+a]      -> array[10+a] is lvalue! (has address in array)
//                     10+a is rvalue (temporary index calculation)
//
// get42() = 100;   -> COMPILER ERROR
//                     get42() returns rvalue (temporary int)
//                     cannot assign to rvalue (no memory location)
//
// std::string s1 = "mike";
// s1              -> lvalue (named variable, has address)
// s1 + s2         -> rvalue (temporary string result, discarded after use)

// ============================================================
// SECTION 3: LVALUE REFERENCE (int& ref)
// ============================================================

// int& ref = lvalue   -> WORKS
// int& ref = rvalue   -> ERROR

// int i = 10;
// int& ref = i;    // WORKS - i is lvalue, ref is alias for i
// int& ref = 10;   // ERROR - 10 is rvalue, no memory to reference

// WHY error?
// ref is a reference = alias to a memory location
// 10 has NO memory location (it's a temporary literal)
// there is nothing to alias!
// where would ref point to?

// ============================================================
// SECTION 4: YOUR QUESTION - WHY const int& ref = 10 WORKS??
// ============================================================

// const int& ref = 10;   // WORKS! but why??
//
// This is a SPECIAL RULE in C++:
// "A const lvalue reference can bind to an rvalue"
//
// WHAT THE COMPILER DOES INTERNALLY:
// const int& ref = 10;
// is secretly transformed into:
//
// int __temp = 10;        // compiler creates hidden temporary variable
// const int& ref = __temp; // ref binds to that temporary
//
// The temporary's LIFETIME IS EXTENDED to match ref's lifetime!
// Without const: compiler won't do this (you could modify a temporary = nonsense)
// With const:    safe, you're only reading it, so compiler allows it
//
// REAL WORLD USE:
// void print(const int& x) { }
// print(10);    // WORKS because of this rule
// print(a+b);   // WORKS - temporary (a+b) bound to const ref
//               // lifetime extended to function duration
//
// INTERVIEW: Why does const int& bind to rvalue but int& doesn't?
//   -> Modifying a temporary makes no sense (it disappears after expression)
//      const prevents modification -> binding is safe -> compiler allows it
//      int& would allow modification of a temporary -> meaningless -> ERROR

// ============================================================
// SECTION 5: setvalue() - WHY setvalue(10) FAILS
// ============================================================

void setvalue(int& changedMe)
{
    changedMe = 69;
    // changedMe is lvalue reference
    // it MUST refer to a real memory location
    // because we are MODIFYING it (= 69)
}

// int i = 10;
// setvalue(i);    // WORKS - i is lvalue, has address, can be modified
// setvalue(10);   // ERROR - 10 is rvalue, no address, cannot be modified
//                 // what would changedMe = 69 even mean?
//                 // where would 69 be stored? 10 has no memory!
//
// void setvalue(const int& changedMe) // this WOULD accept 10
// {                                   // but then changedMe=69 = ERROR
// }                                   // const prevents modification anyway

// ============================================================
// SECTION 6: RVALUE REFERENCE (&&) - THE NEW THING
// ============================================================

// C++11 introduced RVALUE REFERENCE: &&
// int&& rvref = rvalue   -> WORKS  (binds to temporary)
// int&& rvref = lvalue   -> ERROR  (cannot bind to named variable)
//
// & = lvalue reference  (binds to lvalues)
// && = rvalue reference (binds to rvalues/temporaries)
//
// int&& rvref = 10;
// -> 10 is rvalue
// -> && explicitly says "I want to own this temporary"
// -> temporary's lifetime EXTENDED to match rvref
// -> rvref can now READ and MODIFY the temporary
// -> (unlike const& which only reads)

int main()
{
    // ============================================================
    // SECTION 7: YOUR CODE EXPLAINED
    // ============================================================

    std::string s1 = "mike ";                    // lvalue
    std::string s2 = "sujan a really long string"; // lvalue

    std::string&& s3 = s1 + s2;
    // s1 + s2 = rvalue (temporary string result)
    //           normally destroyed immediately after expression
    // && binds to this temporary
    // temporary's lifetime EXTENDED to match s3
    // s3 can now READ and MODIFY this temporary string
    // NO COPY MADE - s3 directly owns the temporary!
    //
    // WHY useful?
    // std::string s3 = s1+s2;  // COPY of temporary into s3
    // std::string&& s3 = s1+s2; // NO copy, s3 IS the temporary
    // for large strings -> significant performance difference

    int&& rvref = 10;
    // 10 is rvalue (literal, no address)
    // && binds to it, extends its lifetime
    // rvref can now be used like a normal int variable
    // int& rvref = 10;   // ERROR (lvalue ref cannot bind rvalue)
    // int&& rvref = 10;  // WORKS (rvalue ref CAN bind rvalue)

    std::cout << rvref << std::endl;  // prints 10
    std::cout << s3 << std::endl;     // prints "mike sujan a really long string"

    return 0;
}

// ============================================================
// SECTION 8: THE BIG PICTURE - WHY && EXISTS (MOVE SEMANTICS)
// ============================================================

// REAL PURPOSE of && is MOVE SEMANTICS (C++11)
//
// PROBLEM with copies:
// std::string result = s1 + s2;
// s1+s2 creates a TEMPORARY string (heap allocated, maybe 1MB)
// then it gets COPIED into result (another 1MB heap allocation)
// then temporary is DESTROYED (1MB freed)
// TOTAL: 2MB allocated, 1MB wasted on a copy that gets destroyed!
//
// WITH move semantics:
// std::string result = std::move(tempString);
// instead of copying the data -> STEAL the pointer from temporary
// temporary had:  [ptr -> "huge data on heap"]
// after move:     result has [ptr -> "huge data on heap"]
//                 temporary has [ptr -> nullptr]  (gutted!)
// TOTAL: 0 extra allocation, pointer just transferred
//
// && tells compiler: "this is a temporary, it's safe to steal from"
//
// THIS is why && was invented:
// identify temporaries -> enable stealing their resources -> no copies
// HUGE performance win for large objects (strings, vectors)

// ============================================================
// SECTION 9: COMPLETE REFERENCE TABLE
// ============================================================

// | Type          | Syntax      | Binds to          | Can modify? |
// |---------------|-------------|-------------------|-------------|
// | lvalue ref    | int&        | lvalues only      | YES         |
// | const lvalue  | const int&  | lvalues + rvalues | NO          |
// | rvalue ref    | int&&       | rvalues only      | YES         |
// | const rvalue  | const int&& | rvalues only      | NO (rare)   |

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is the difference between lvalue and rvalue?
//     -> lvalue: persistent memory location, address can be taken (&x)
//        rvalue: temporary value, no persistent address, short-lived

// Q2: Why does int& ref = 10 fail but const int& ref = 10 work?
//     -> int& would allow modifying a temporary (meaningless).
//        const int& is read-only, so compiler creates hidden temporary,
//        binds ref to it, and extends its lifetime. Safe because
//        const prevents modification of the temporary.

// Q3: What is an rvalue reference (&&)?
//     -> Reference that binds ONLY to rvalues (temporaries).
//        Introduced in C++11 to enable move semantics.
//        Allows stealing resources from temporaries without copying.

// Q4: What is the difference between & and &&?
//     -> &  (lvalue ref)  binds to named variables (lvalues)
//        && (rvalue ref)  binds to temporaries (rvalues)
//        const& binds to both (most flexible, read-only)

// Q5: Why was && introduced in C++11?
//     -> To enable move semantics.
//        Identify temporaries whose resources can be STOLEN (moved)
//        instead of copied. Major performance optimization for
//        large objects like strings, vectors.

// Q6: What is lifetime extension?
//     -> When const& or && binds to a temporary,
//        the temporary's lifetime is extended to match the reference.
//        Normally temporaries die at end of expression.

// Q7: What is std::move?
//     -> Casts an lvalue to rvalue reference.
//        Tells compiler "treat this as a temporary, steal from it"
//        std::move(s1) -> s1's resources moved, s1 left in valid but empty state
//        Does NOT actually move anything - just enables move semantics

// Q8: What is the most important use of const& in function parameters?
//     -> void f(const std::string& s)
//        Accepts BOTH lvalues and rvalues (temporaries)
//        No copy made (efficient)
//        Read-only (safe)
//        Universal parameter type for large read-only objects
// ```

// **The four cases that interviewers always test:**

// ```cpp
// int i = 10;

// int& ref = i;          // ✅ lvalue ref   <- binds to lvalue
// int& ref = 10;         // ❌ ERROR        <- lvalue ref rejects rvalue
// const int& ref = 10;   // ✅ const ref    <- binds to BOTH (special rule!)
// int&& ref = 10;        // ✅ rvalue ref   <- binds to rvalue
// int&& ref = i;         // ❌ ERROR        <- rvalue ref rejects lvalue
// ```

// > `&&` was not created for the code you wrote today. It was created for **move semantics** — the ability to **steal** a temporary's heap data instead of copying it. That's the real interview answer.