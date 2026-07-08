// ============================================================
// TEMPLATE CLASSES - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS A TEMPLATE CLASS?
// ============================================================

// Same concept as template FUNCTIONS but for entire CLASSES
// -> ONE class definition works for ANY type T and ANY size
// -> compiler GENERATES separate class for each unique combination
// -> std::vector<T>, std::array<T,N>, std::unique_ptr<T> are ALL template classes
//
// WITHOUT template class:
// class ContainerInt    { int*    m_data; ContainerInt(int n) { m_data = new int[n]; } };
// class ContainerDouble { double* m_data; ... };
// class ContainerFloat  { float*  m_data; ... };
// -> same pattern repeated 3 times -> DRY violation
//
// WITH template class:
// template<typename T, int Size>
// class Containers { T* m_data; };
// -> ONE definition handles ALL types and sizes!

// ============================================================
// SECTION 2: ANATOMY OF TEMPLATE CLASS
// ============================================================

template <typename T, int Size>
//         ^^^^^^^^^   ^^^
//         TYPE param  NON-TYPE param (compile-time integer value)
//         T = what type to store
//         Size = how many elements (baked in at compile time)

class Containers
{
public:
    Containers()
    {
        m_data = new T[Size];
        // T    = the actual type (int, double, float etc.)
        // Size = compile-time constant (10, 20, 13 etc.)
        // allocates Size elements of type T on heap
    }

    ~Containers()
    {
        delete[] m_data;
        // RAII: cleanup happens automatically
        // must use delete[] (not delete) because new T[Size] was used
    }

private:
    T* m_data;
    // pointer to heap-allocated array of type T
    // type changes based on template parameter T
};

// ============================================================
// SECTION 3: WHAT COMPILER GENERATES
// ============================================================

// Containers<int, 10> c;
// compiler generates:
// class Containers_int_10 {
// public:
//     Containers_int_10() { m_data = new int[10]; }
//     ~Containers_int_10() { delete[] m_data; }
// private:
//     int* m_data;
// };
//
// Containers<double, 20> c2;
// compiler generates:
// class Containers_double_20 {
// public:
//     Containers_double_20() { m_data = new double[20]; }
//     ~Containers_double_20() { delete[] m_data; }
// private:
//     double* m_data;
// };
//
// Containers<float, 13> c3;
// compiler generates:
// class Containers_float_13 {
// public:
//     Containers_float_13() { m_data = new float[13]; }
//     ~Containers_float_13() { delete[] m_data; }
// private:
//     float* m_data;
// };
//
// THREE separate classes generated from ONE template definition!
// Each is a COMPLETELY DIFFERENT TYPE:
// Containers<int,10> != Containers<int,20>   (different Size!)
// Containers<int,10> != Containers<double,10> (different T!)

// ============================================================
// SECTION 4: MEMORY VISUAL
// ============================================================

// Containers<int, 10> c;
//
// STACK:                     HEAP:
// c.m_data = 0x55a3c0 -----> [int][int][int]...[int]  (10 ints = 40 bytes)
//
// Containers<double, 20> c2;
//
// STACK:                     HEAP:
// c2.m_data = 0x55a400 ----> [double][double]...[double]  (20 doubles = 160 bytes)
//
// When scope ends (return 0;) - LIFO destruction:
// ~c3 called -> delete[] c3.m_data (13 floats freed)
// ~c2 called -> delete[] c2.m_data (20 doubles freed)
// ~c  called -> delete[] c.m_data  (10 ints freed)

// ============================================================
// SECTION 5: EXTENDING THE CLASS - WHAT YOU SHOULD ADD
// ============================================================

// CURRENT CLASS is missing:
// 1. operator[] (to access elements)
// 2. size() method (to get Size)
// 3. Rule of Three (raw pointer member -> must add copy ctor, copy assign)
//    (currently: copy would do shallow copy -> double free!)
//
// EXTENDED VERSION:
// template<typename T, int Size>
// class Containers {
// public:
//     Containers() : m_data(new T[Size]{}) { }      // zero-init with {}
//     ~Containers() { delete[] m_data; }
//
//     // Rule of Three:
//     Containers(const Containers& rhs) {            // deep copy
//         m_data = new T[Size];
//         for (int i = 0; i < Size; i++)
//             m_data[i] = rhs.m_data[i];
//     }
//     Containers& operator=(const Containers& rhs) { // copy assign
//         if (this == &rhs) return *this;
//         for (int i = 0; i < Size; i++)
//             m_data[i] = rhs.m_data[i];
//         return *this;
//     }
//     // Rule of Five additions:
//     Containers(Containers&& rhs) noexcept {        // move ctor
//         m_data = rhs.m_data;
//         rhs.m_data = nullptr;
//     }
//
//     T& operator[](int index) { return m_data[index]; }
//     const T& operator[](int index) const { return m_data[index]; }
//     constexpr int size() const { return Size; }
//
// private:
//     T* m_data;
// };
//
// THIS IS ESSENTIALLY WHAT std::array DOES! (but std::array uses stack)

// ============================================================
// SECTION 6: TEMPLATE CLASS vs std::array vs std::vector
// ============================================================

// YOUR Containers<T, Size>:
// -> Size = compile-time constant (template param)
// -> data = HEAP (new T[Size])
// -> size fixed at compile time but stored on heap
// -> unusual combination (compile-time size + heap storage)
//
// std::array<T, N>:
// -> N = compile-time constant (template param)
// -> data = STACK (T data[N] inside the class)
// -> size fixed at compile time, stack stored
// -> fastest option, size known at compile time
//
// std::vector<T>:
// -> size = RUNTIME (can grow/shrink)
// -> data = HEAP (dynamic allocation)
// -> most flexible, slightly more overhead

// ============================================================
// SECTION 7: SEPARATING DECLARATION AND DEFINITION
// ============================================================

// PROBLEM with template classes and .hpp/.cpp split:
//
// Containers.hpp:
// template<typename T, int Size>
// class Containers {
//     Containers();  // declaration
// };
//
// Containers.cpp:
// template<typename T, int Size>
// Containers<T,Size>::Containers() { m_data = new T[Size]; }
// ^^^^^^^^^^^^^^^^^^^
// must use full class name with template params in definition!
//
// BUT: if Containers.cpp is compiled separately, main.cpp can't
// generate Containers<int,10> because it can't see the implementation!
//
// SOLUTIONS:
// 1. Put EVERYTHING in the .hpp (most common for templates)
// 2. Explicit instantiation at end of .cpp:
//    template class Containers<int, 10>;   // forces generation
//    template class Containers<double, 20>;
//    (rare, only works if you know all types in advance)
//
// THIS IS WHY STL headers are huge - all implementation is in .hpp!

// ============================================================
// SECTION 8: TEMPLATE CLASS MEMBER FUNCTION OUTSIDE CLASS
// ============================================================

// If you define members OUTSIDE the class body:
//
// template<typename T, int Size>
// class Containers {
// public:
//     Containers();     // declaration
//     ~Containers();    // declaration
//     T& operator[](int i);
// private:
//     T* m_data;
// };
//
// DEFINITION outside (in same .hpp or explicit .cpp with instantiation):
// template<typename T, int Size>
// Containers<T, Size>::Containers()         // note full name!
// {
//     m_data = new T[Size];
// }
//
// template<typename T, int Size>
// Containers<T, Size>::~Containers()
// {
//     delete[] m_data;
// }
//
// template<typename T, int Size>
// T& Containers<T, Size>::operator[](int i)
// {
//     return m_data[i];
// }
// -> every definition must repeat the template parameters

// ============================================================
// SECTION 9: main() TRACE
// ============================================================

int main()
{
    Containers<int, 10> c;
    // compiler instantiates Containers with T=int, Size=10
    // constructor: m_data = new int[10] -> 40 bytes on heap

    Containers<double, 20> c2;
    // compiler instantiates Containers with T=double, Size=20
    // constructor: m_data = new double[20] -> 160 bytes on heap

    Containers<float, 13> c3;
    // compiler instantiates Containers with T=float, Size=13
    // constructor: m_data = new float[13] -> 52 bytes on heap

    return 0;
    // LIFO destruction (c3, c2, c destroyed in reverse order):
    // ~c3: delete[] m_data (52 bytes freed)
    // ~c2: delete[] m_data (160 bytes freed)
    // ~c:  delete[] m_data (40 bytes freed)
    // no memory leak!
}

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a template class?
//     -> A class parameterized by one or more types or values.
//        Compiler generates separate concrete classes for each unique
//        combination of template arguments used.

// Q2: What is the difference between Containers<int,10> and Containers<int,20>?
//     -> They are COMPLETELY DIFFERENT TYPES (different Size param).
//        Cannot assign one to the other without conversion.
//        Same T but different non-type param = different instantiation.

// Q3: Why does the template class need to be in the header file?
//     -> Compiler needs to see the full template definition to generate
//        instantiations for each type used. If only in .cpp, other
//        translation units can't generate their own instantiations.

// Q4: What bug exists in the current class and how to fix it?
//     -> Raw pointer m_data without Rule of Three/Five.
//        Copy constructor does shallow copy -> double free!
//        Fix: add copy constructor, copy assignment, move constructor,
//        move assignment (Rule of Five) OR use std::vector<T> instead.

// Q5: How do you define template class member functions outside the class?
//     -> Must repeat the template parameters:
//        template<typename T, int Size>
//        Containers<T,Size>::Containers() { m_data = new T[Size]; }
//        Must use full class name Containers<T,Size>:: prefix.

// Q6: What is the difference between your Containers and std::array?
//     -> std::array: data on STACK (T data[N] inside), no heap.
//        Containers: data on HEAP (new T[Size]), more like std::vector
//        but with compile-time fixed size.

// Q7: What standard library classes are template classes?
//     -> std::vector<T>, std::array<T,N>, std::unique_ptr<T>,
//        std::shared_ptr<T>, std::map<K,V>, std::pair<T1,T2>,
//        std::tuple<Types...>, std::function<Signature>. All of STL!

// Q8: What happens if Size=0 is used: Containers<int, 0> c?
//     -> new int[0] is technically valid in C++ (returns non-null pointer).
//        delete[] on it is also valid.
//        But accessing m_data[0] would be undefined behavior.
//        Could add static_assert(Size > 0, "Size must be positive") to prevent.
// ```

// **Three different types from one template:**

// ```cpp
// Containers<int, 10>    // T=int,    Size=10  ← TYPE A
// Containers<double, 20> // T=double, Size=20  ← TYPE B (completely different!)
// Containers<float, 13>  // T=float,  Size=13  ← TYPE C (completely different!)

// // You cannot do:
// Containers<int,10> a;
// Containers<int,20> b = a;  // ERROR! Different types, not assignable
// ```

// **The bug hiding in plain sight:**

// ```cpp
// Containers<int,10> a;
// Containers<int,10> b = a;  // COMPILES but WRONG!
// // compiler-generated copy ctor does: b.m_data = a.m_data (shallow!)
// // b and a now point to SAME heap memory
// // ~b: delete[] m_data ← first delete
// // ~a: delete[] m_data ← DOUBLE FREE → crash!
// // Fix: implement Rule of Five or use std::vector<T> as m_data
// ```

// > **The connection to everything you've learned**: this template class combines templates (generics) + non-type parameters (`Size`) + RAII (constructor acquires, destructor releases) + Rule of Three (raw pointer needs copy/move handling) + `new`/`delete[]` — all in one small class. It's essentially a simplified `std::array` variant, and understanding it means you understand how the STL containers work internally.