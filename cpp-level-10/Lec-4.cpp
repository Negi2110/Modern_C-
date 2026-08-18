#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

// ============================================================
// CUSTOM ITERATOR - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHY WRITE A CUSTOM ITERATOR?
// ============================================================

// When you build a CUSTOM CONTAINER, you get nothing for free.
// No STL algorithms, no range-based for, no std::for_each.
//
// TO MAKE YOUR CONTAINER WORK WITH STL, you need:
// 1. begin() and end() member functions
// 2. An iterator type with the right operator overloads
// 3. Iterator TRAITS (type aliases inside the iterator struct)
//
// WHAT YOU GAIN once done:
// for (const auto& e : c)         // range-based for WORKS
// std::for_each(c.begin(),...)    // STL algorithms WORK
// std::find(c.begin(), c.end(),v) // all STL WORKS
// auto it = c.begin(); ++it;      // iterator arithmetic WORKS

// ============================================================
// SECTION 2: ITERATOR TRAITS - THE TYPE ALIASES
// ============================================================

// Inside your iterator struct, you MUST declare these type aliases
// so STL algorithms can inspect your iterator's capabilities:
//
// using iterator_category = std::forward_iterator_tag;
//       ^^^^^^^^^^^^^^^^^^^
//       TELLS STL: "I am a Forward Iterator"
//       Other options:
//       std::input_iterator_tag
//       std::output_iterator_tag
//       std::forward_iterator_tag        ← YOUR CODE
//       std::bidirectional_iterator_tag
//       std::random_access_iterator_tag
//       std::contiguous_iterator_tag     (C++20)
//
// using difference_type = std::ptrdiff_t;
//       ^^^^^^^^^^^^^^^^
//       type for distance between iterators (result of it1 - it2)
//       ptrdiff_t = signed integer type for pointer differences
//
// using value_type = T;
//       ^^^^^^^^^^
//       type of element the iterator points to
//       algorithms use this to know what type to work with
//
// using pointer = T*;
//       ^^^^^^^
//       type returned by operator->
//
// using reference = T&;
//       ^^^^^^^^^
//       type returned by operator*
//
// WITHOUT THESE: STL algorithms can't use your iterator!
// WITH THESE: std::for_each, std::find, etc. all work

// ============================================================
// SECTION 3: THE COMPLETE CUSTOM ITERATOR
// ============================================================

template <typename T>
class FixedSizeArray
{
private:
    struct iterator
    {
        // ---- ITERATOR TRAITS (required by STL) ----
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        explicit iterator(pointer ptr) : mPtr(ptr) {}
        // explicit: prevents accidental implicit conversion
        // takes raw pointer, wraps it in iterator

        // ---- DEREFERENCE OPERATORS ----
        reference operator*() const { return *mPtr; }
        // *it -> returns T& (reference to element)
        // const because dereferencing doesn't change the iterator

        pointer operator->() const { return mPtr; }
        // it->field -> returns T* (for struct/class element access)
        // e.g., if T is Edge: it->v1, it->v2

        // ---- INCREMENT OPERATORS ----
        // PRE-INCREMENT: ++it (preferred in for loops)
        iterator& operator++() {
            ++mPtr;     // advance raw pointer
            return *this; // return reference to THIS (no copy!)
        }

        // POST-INCREMENT: it++ (creates temporary copy)
        iterator operator++(int) {
            // int parameter = dummy to distinguish from pre-increment
            iterator tmp = *this;  // copy current state
            ++(*this);              // call pre-increment (advance)
            return tmp;             // return OLD position (before advance)
        }
        // WHY PREFER ++it OVER it++:
        // it++: creates a copy (tmp), advances, returns copy
        // ++it: advances in place, returns reference (no copy!)
        // For simple types: compiler optimizes away the difference
        // For complex iterators: ++it is genuinely faster

        // ---- COMPARISON OPERATORS ----
        friend bool operator==(const iterator& a, const iterator& b) {
            return a.mPtr == b.mPtr;
            // two iterators equal if they point to SAME address
        }
        friend bool operator!=(const iterator& a, const iterator& b) {
            return a.mPtr != b.mPtr;
            // NOT EQUAL if different addresses
        }
        // friend = these functions can access mPtr (private member)
        // defined inside struct = inline (no separate definition needed)

    private:
        pointer mPtr;  // the raw pointer this iterator wraps
    };

private:
    T*     mData;      // heap-allocated array
    size_t mCapacity;  // fixed size (cannot grow)

public:
    explicit FixedSizeArray(size_t capacity)
        : mData(new T[capacity]), mCapacity(capacity) {}

    ~FixedSizeArray() {
        delete[] mData;  // RAII: clean up heap allocation
    }

    // RULE OF FIVE: raw pointer -> must define/delete copy and move
    FixedSizeArray(const FixedSizeArray&) = delete;             // no copy
    FixedSizeArray& operator=(const FixedSizeArray&) = delete;  // no copy assign
    // WHY DELETE COPY?
    // default copy would shallow-copy mData pointer
    // both objects would delete[] same memory -> double free crash!
    // OR: implement deep copy (copy all mData elements)
    // YOUR CODE chooses to delete (simpler, prevents misuse)

    // MOVE CONSTRUCTOR (transfer ownership):
    FixedSizeArray(FixedSizeArray&& other) noexcept
        : mData(other.mData), mCapacity(other.mCapacity)
    {
        other.mData     = nullptr;  // old object no longer owns data
        other.mCapacity = 0;
        // noexcept: move operations SHOULD be noexcept
        // allows vector to use move instead of copy during reallocation
    }

    // MOVE ASSIGNMENT:
    FixedSizeArray& operator=(FixedSizeArray&& other) noexcept
    {
        if (this != &other) {          // self-assignment check
            delete[] mData;             // free current data
            mData           = other.mData;
            mCapacity       = other.mCapacity;
            other.mData     = nullptr;  // old object no longer owns
            other.mCapacity = 0;
        }
        return *this;
    }

    // ELEMENT ACCESS:
    T& operator[](size_t index)             { return mData[index]; }
    const T& operator[](size_t index) const { return mData[index]; }
    size_t capacity() const                 { return mCapacity; }

    // BEGIN AND END - THE BRIDGE TO STL:
    iterator begin() { return iterator(&mData[0]); }
    // iterator to first element
    // &mData[0] = address of element 0

    iterator end()   { return iterator(&mData[mCapacity]); }
    // iterator to ONE PAST LAST element (sentinel)
    // &mData[mCapacity] = address past last valid element
    // NEVER dereferenced - just used for comparison in loops
};

// ============================================================
// SECTION 4: main() TRACE
// ============================================================

int main()
{
    using T = FixedSizeArray<int>;
    T c(5);
    // allocates int[5] on heap (uninitialized!)
    // c[0..4] contain garbage values

    // STYLE 1: Iterator loop
    for (auto it = c.begin(); it != c.end(); ++it)
        std::cout << *it << '\n';
    // c.begin() -> iterator(&mData[0])
    // c.end()   -> iterator(&mData[5])
    // ++it -> calls iterator::operator++() -> ++mPtr
    // *it  -> calls iterator::operator*()  -> *mPtr
    // it != end -> calls operator!=() -> mPtr != &mData[5]
    // PRINTS: 5 garbage int values

    // STYLE 2: std::for_each with lambda
    std::for_each(c.begin(), c.end(), [](auto& e) {
        std::cout << e << '\n';
    });
    // for_each sees forward_iterator_tag -> knows it can traverse once forward
    // calls lambda for each element: lambda(&*it) = lambda(mData[i])

    // STYLE 3: Range-based for (cleanest)
    for (const auto& e : c)
        std::cout << e << '\n';
    // compiler generates:
    // auto&& range = c;
    // auto it = c.begin();        // calls YOUR begin()
    // auto end = c.end();         // calls YOUR end()
    // for (; it != end; ++it) {   // calls YOUR operator++
    //     const auto& e = *it;    // calls YOUR operator*
    //     ...
    // }

    return 0;
}

// ============================================================
// SECTION 5: MINIMUM REQUIREMENTS FOR range-based for
// ============================================================

// A type works with range-based for if it has:
// 1. begin() returning an iterator
// 2. end()   returning an iterator (or sentinel)
// 3. The iterator has:
//    -> prefix operator++ (advance)
//    -> operator* (dereference)
//    -> operator!= (comparison)
//
// THAT'S IT - just these three on the iterator
// You DON'T need all five traits for basic range-based for
// Traits are needed for STL algorithms to inspect your iterator

// ============================================================
// SECTION 6: MINIMUM REQUIREMENTS FOR STL ALGORITHMS
// ============================================================

// FORWARD ITERATOR requires (your iterator provides all):
// 1. Default constructible (or copy constructible)
// 2. Copy constructible + copy assignable
// 3. operator++ (pre AND post)
// 4. operator* (dereference, returns reference)
// 5. operator-> (member access)
// 6. operator== and operator!=
// 7. The five type aliases (iterator_category, etc.)
//
// YOUR ITERATOR: passes all of these!
// -> std::for_each works (needs Forward Iterator)
// -> std::find works    (needs Input Iterator = subset of Forward)
// -> std::count works   (needs Input Iterator)
// -> std::sort FAILS    (needs Random Access Iterator - not provided)

// ============================================================
// SECTION 7: UPGRADING TO BIDIRECTIONAL OR RANDOM ACCESS
// ============================================================

// TO MAKE BIDIRECTIONAL (add -- operator):
// iterator& operator--() {
//     --mPtr;
//     return *this;
// }
// iterator operator--(int) {
//     iterator tmp = *this;
//     --(*this);
//     return tmp;
// }
// Change tag: using iterator_category = std::bidirectional_iterator_tag;
// UNLOCKS: std::reverse, std::prev, rbegin/rend

// TO MAKE RANDOM ACCESS (add +n, -n, [], comparisons):
// iterator operator+(difference_type n) const { return iterator(mPtr + n); }
// iterator operator-(difference_type n) const { return iterator(mPtr - n); }
// difference_type operator-(const iterator& other) const { return mPtr - other.mPtr; }
// reference operator[](difference_type n) const { return *(mPtr + n); }
// bool operator<(const iterator& other) const { return mPtr < other.mPtr; }
// bool operator>(const iterator& other) const { return mPtr > other.mPtr; }
// bool operator<=(const iterator& other) const { return mPtr <= other.mPtr; }
// bool operator>=(const iterator& other) const { return mPtr >= other.mPtr; }
// Change tag: using iterator_category = std::random_access_iterator_tag;
// UNLOCKS: std::sort, it+n, it[n], std::distance O(1)

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What five type aliases must a custom iterator provide?
//     -> iterator_category (forward/bidirectional/random_access etc.)
//        difference_type (ptrdiff_t for pointer differences)
//        value_type (T - type of element)
//        pointer (T* - type of operator-> return)
//        reference (T& - type of operator* return)

// Q2: What is the minimum for range-based for loop to work?
//     -> begin() and end() on container.
//        Iterator with: operator++ (pre), operator*, operator!=.
//        The five type aliases are NOT required for range-based for.

// Q3: Why is pre-increment (++it) preferred over post-increment (it++)?
//     -> Post-increment creates a COPY of the iterator, advances, returns copy.
//        Pre-increment advances in place, returns reference (no copy).
//        For simple pointer wrappers: compiler optimizes away difference.
//        For complex iterators: ++it is genuinely more efficient.

// Q4: Why is copy deleted in FixedSizeArray?
//     -> Raw pointer member. Default copy = shallow copy (both objects
//        point to same heap data). Both destructors would delete[] same
//        memory = double free = crash. Options: delete copy (your code)
//        or implement deep copy that copies all mData elements.

// Q5: What does end() return and can you dereference it?
//     -> Returns iterator past the last valid element (&mData[mCapacity]).
//        NEVER dereference end() - undefined behavior.
//        Only used for comparison: it != end().

// Q6: Why does post-increment have (int) parameter?
//     -> Compiler uses int parameter as a dummy to DISTINGUISH
//        post-increment from pre-increment. The int is never used.
//        it++ is operator++(int), ++it is operator++().

// Q7: What STL algorithms work with forward iterator vs random access?
//     -> Forward: for_each, find, count, copy, fill, transform.
//        Bidirectional: reverse, prev, next.
//        Random access: sort, binary_search, nth_element, it+n.

// Q8: What is the friend keyword doing in the comparison operators?
//     -> Allows the free operator functions (== and !=) to access
//        the private mPtr member of iterator.
//        Defined inside the class = automatically inline.
//        Alternative: make mPtr public or add getter.
// ```

// **The minimum viable iterator:**

// ```cpp
// struct iterator {
//     // REQUIRED FOR STL:
//     using iterator_category = std::forward_iterator_tag;
//     using difference_type   = std::ptrdiff_t;
//     using value_type        = T;
//     using pointer           = T*;
//     using reference         = T&;

//     // REQUIRED FOR range-based for:
//     reference operator*() const { return *mPtr; }     // dereference
//     iterator& operator++() { ++mPtr; return *this; }  // advance
//     bool operator!=(const iterator& b) { return mPtr != b.mPtr; } // compare

//     // REQUIRED FOR STL algorithms:
//     pointer operator->() const { return mPtr; }
//     iterator operator++(int) { auto t=*this; ++(*this); return t; }
//     bool operator==(const iterator& b) { return mPtr == b.mPtr; }

//     T* mPtr;
// };
// ```

// **What the range-based for compiles to:**

// ```cpp
// for (const auto& e : c) { cout << e; }

// // Compiler generates:
// auto it  = c.begin();    // YOUR begin()
// auto end = c.end();      // YOUR end()
// for (; it != end; ++it)  // YOUR operator!=, YOUR operator++
//     const auto& e = *it; // YOUR operator*
//     cout << e;
// }
// ```

// > **The key insight**: writing a custom iterator is about implementing a **pointer-like interface** (`*`, `++`, `!=`) plus declaring **type traits** so the STL can introspect your iterator. Once those are in place, your custom container gets all of STL — `for_each`, `find`, `count`, range-based for — for free. The iterator category tag (`forward_iterator_tag`) is what tells the compiler which algorithms your container is compatible with.