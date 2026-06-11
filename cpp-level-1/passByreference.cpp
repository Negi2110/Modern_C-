// ============================================================
// PASS BY VALUE vs PASS BY REFERENCE - VECTOR - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <vector>
#include <algorithm>

// ============================================================
// SECTION 1: PASS BY VALUE - EXPENSIVE COPY
// ============================================================

void passByval(std::vector<int> arg)
{
    // When this function is called with x (100000 ints):
    // -> ENTIRE vector is COPIED into arg
    // -> 100000 ints x 4 bytes = 400,000 bytes COPIED onto heap
    // -> arg is a completely separate vector in memory
    // -> slow, wasteful for large data

    arg[0] = 1;
    // modifies arg[0] = copy's first element
    // x[0] in main() is UNTOUCHED
    // when function returns, arg is DESTROYED (400KB freed)
}

// ============================================================
// SECTION 2: PASS BY REFERENCE - NO COPY, MODIFIES ORIGINAL
// ============================================================

void passByref(std::vector<int>& alias)
{
    // When this function is called with x:
    // -> NO copy made
    // -> alias IS x - same memory, same data
    // -> only 8 bytes passed (reference = pointer internally)
    // -> fast, efficient regardless of vector size

    alias[0] = 1;
    // modifies x[0] in main() DIRECTLY
    // original vector is changed
}

int main()
{
    // ============================================================
    // SECTION 3: std::vector - QUICK RECAP
    // ============================================================

    std::vector<int> x(100000);
    // creates vector with 100000 ints
    // all initialized to 0 by default
    // data lives on HEAP (unlike std::array which is on stack)
    // x itself (the object) lives on STACK but it has 3 parts:
    //
    // STACK:                    HEAP:
    // x object:                 actual data:
    // [pointer] ------------>   [0][0][0]...[0]  (100000 ints)
    // [size=100000]
    // [capacity=100000]
    //
    // sizeof(x) = 24 bytes (just the 3 pointers/values on stack)
    // actual data = 400,000 bytes on heap

    std::fill(std::begin(x), std::end(x), 1);
    // fills all 100000 elements with 1
    // x = {1, 1, 1, 1, .... 1}  (100000 ones)

    // ============================================================
    // SECTION 4: WHAT ACTUALLY GETS COPIED IN PASS BY VALUE
    // ============================================================

    passByval(x);
    // WHAT HAPPENS:
    // -> new vector arg created
    // -> arg gets its own HEAP allocation (400,000 bytes)
    // -> all 100000 elements copied from x to arg
    // -> arg[0] = 1 (already 1, no visible change)
    // -> function returns, arg DESTROYED, 400KB freed
    //
    // COPY VISUAL:
    // STACK:                    HEAP:
    // x:   [ptr1] ----------->  [1][1][1]...[1]  original
    // arg: [ptr2] ----------->  [1][1][1]...[1]  full copy!
    //                            ^
    //                            arg[0]=1 modifies THIS copy only

    std::cout << "the first:" << x[0] << std::endl;
    // prints: 1  (unchanged, pass by value protected it)

    // ============================================================
    // SECTION 5: WHAT HAPPENS IN PASS BY REFERENCE
    // ============================================================

    passByref(x);
    // WHAT HAPPENS:
    // -> NO new vector created
    // -> alias refers to x directly
    // -> alias[0] = 1 modifies x[0] directly
    //
    // REFERENCE VISUAL:
    // STACK:                    HEAP:
    // x:     [ptr1] -------->   [1][1][1]...[1]
    //                            ^
    // alias: refers to x --------| same memory!
    //                            alias[0]=1 changes THIS directly

    std::cout << "the first:" << x[0] << std::endl;
    // prints: 1 (was already 1 from fill, but NOW modified by reference)
    // if fill had set to 5, passByref would change x[0] to 1
    // passByval would NOT change it

    // ============================================================
    // SECTION 6: PERFORMANCE COMPARISON
    // ============================================================

    // For std::vector<int> with 100000 elements:
    //
    // passByval:
    // -> 400,000 bytes copied      (slow)
    // -> heap allocation for copy  (slow)
    // -> heap deallocation after   (slow)
    // -> total: significant overhead
    //
    // passByref:
    // -> 8 bytes passed (reference/pointer size on 64-bit)
    // -> no allocation, no deallocation
    // -> total: nearly zero overhead
    //
    // | Size       | By Value    | By Reference |
    // |------------|-------------|--------------|
    // | 100 ints   | 400 bytes   | 8 bytes      |
    // | 10K ints   | 40 KB       | 8 bytes      |
    // | 100K ints  | 400 KB      | 8 bytes      |
    // | 1M ints    | 4 MB        | 8 bytes      |
    //
    // Reference cost is ALWAYS 8 bytes regardless of vector size!

    // ============================================================
    // SECTION 7: FOUR WAYS TO PASS - COMPLETE PICTURE
    // ============================================================

    // 1. Pass by value
    //    void f(std::vector<int> v)
    //    -> full copy, original safe, expensive for large data
    //    -> USE WHEN: you need to modify locally without affecting original
    //                 AND data is small (int, float, char)

    // 2. Pass by reference
    //    void f(std::vector<int>& v)
    //    -> no copy, modifies original, fast
    //    -> USE WHEN: you need to MUTATE the original data

    // 3. Pass by const reference  (MOST COMMON for large objects)
    //    void f(const std::vector<int>& v)
    //    -> no copy, cannot modify, fast, read-only
    //    -> USE WHEN: you only need to READ large data
    //    -> best of both worlds: speed of reference + safety of value

    // 4. Pass by pointer
    //    void f(std::vector<int>* v)
    //    -> no copy, can modify original, can be null
    //    -> USE WHEN: parameter is optional (can be nullptr)

    // ============================================================
    // SECTION 8: YOUR COMMENT IS EXACTLY RIGHT
    // ============================================================

    // "pass by ref - mutate data and efficient"
    //
    // TWO benefits of pass by reference:
    // BENEFIT 1: EFFICIENT   -> no copy, always 8 bytes passed
    // BENEFIT 2: MUTATE      -> can modify original directly
    //
    // If you want efficient but NO mutation -> use const reference:
    // void passByConstRef(const std::vector<int>& alias)
    // {
    //     alias[0] = 1;  // COMPILER ERROR - const prevents mutation
    // }

    // ============================================================
    // SECTION 9: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: Why is pass by value expensive for std::vector?
    //     -> Entire heap data is copied. 100K ints = 400KB copy.
    //        Heap allocation + element-by-element copy + deallocation.

    // Q2: How much data is actually passed in pass by reference?
    //     -> 8 bytes (size of a pointer/reference on 64-bit system)
    //        Regardless of vector size. Always O(1).

    // Q3: What is const reference and why use it?
    //     -> const std::vector<int>& v
    //        No copy (efficient) + cannot modify (safe)
    //        Best choice when you only need to read large objects.

    // Q4: Where does std::vector store its data?
    //     -> HEAP. The vector object (3 pointers) is on stack,
    //        but actual elements are heap-allocated.
    //        This is why copying a vector is expensive.

    // Q5: What is the difference between std::array and std::vector?
    //     -> std::array: fixed size, stack allocated, compile time size
    //        std::vector: dynamic size, heap allocated, runtime size

    // Q6: After passByval returns, what happens to the copy?
    //     -> Copy (arg) is destroyed. Destructor called.
    //        Heap memory for 100K ints is freed.
    //        Stack frame for passByval is popped.

    // Q7: Can you pass a const vector by non-const reference?
    //     -> NO. const std::vector<int> x;
    //        passByref(x);  // ERROR - cannot bind const to non-const ref
    //        Must use: void f(const std::vector<int>& v)

    // Q8: What does std::vector<int> x(100000) do?
    //     -> Creates vector of 100000 ints, all zero-initialized.
    //        Different from x(100000, 5) which initializes all to 5.

    return 0;
}
// ```

// **The golden rule from this topic:**

// ```
// primitive types  (int, float, char)  ->  pass by value      (copy is cheap)
// large objects    (vector, string)    ->  const reference     (read only)
// need to mutate   (any type)          ->  reference           (modify original)
// optional param   (any type)          ->  pointer             (can be null)
// ```

// > Reference cost is **always 8 bytes** no matter how big the vector is. That single fact is what makes `const&` the default choice for large objects in every real C++ codebase — including Qt.