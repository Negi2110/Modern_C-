// ============================================================
// STATIC MEMBERS IN TEMPLATE CLASSES - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: THE KEY INSIGHT - YOUR COMMENT IS EXACTLY RIGHT
// ============================================================

// YOUR COMMENT: "each instantiated class that has been generated
//                gets its OWN COPY of static variable"
//
// THIS IS THE CRITICAL DIFFERENCE from non-template classes:
//
// NON-TEMPLATE class with static member:
// class Foo { static int count; };
// int Foo::count = 0;
// -> ONE count shared by ALL Foo objects (as you learned before)
//
// TEMPLATE class with static member:
// template<typename T, int Size>
// class Containers { static T m_variables; };
//
// Containers<int,5>  has its OWN static T m_variables  <- copy 1
// Containers<int,7>  has its OWN static T m_variables  <- copy 2
// Containers<int,10> has its OWN static T m_variables  <- copy 3
//
// WHY? Because each template instantiation = COMPLETELY SEPARATE CLASS
// Containers<int,5> and Containers<int,7> are DIFFERENT TYPES
// Each different type gets its OWN set of static members!
// Just like Foo and Bar would each have their own statics

// ============================================================
// SECTION 2: THE CLASS WITH STATIC MEMBER
// ============================================================

template <typename T, int Size>
class Containers
{
public:
    Containers()
    {
        m_data = new T[Size];
    }
    ~Containers()
    {
        delete[] m_data;
    }

    static T m_variables;
    // static = ONE per instantiated class (not per object)
    // T = type varies with template param
    //
    // Containers<int,5>  -> static int   m_variables  (one for all <int,5>)
    // Containers<int,7>  -> static int   m_variables  (one for all <int,7>)
    // Containers<float,5>-> static float m_variables  (one for all <float,5>)
    // ALL SEPARATE!

private:
    T* m_data;
};

// ============================================================
// SECTION 3: STATIC MEMBER DEFINITION OUTSIDE - TEMPLATE SYNTAX
// ============================================================

template <typename T, int Size>
T Containers<T, Size>::m_variables;
// ^^^^^^^^^  ^^^^^^^^^^^^^^^^^^
// |          full class name with template params
// repeat template params before the type
//
// YOUR COMMENT: "telling the compiler hey we exist and we are of type T and size Size"
// EXACTLY - this is the DEFINITION (creates storage in data segment)
// The class declaration was just a DECLARATION (said it will exist)
//
// THIS IS NOT ONE DEFINITION - it's a TEMPLATE for definitions!
// When compiler sees Containers<int,5> used, it generates:
//   int Containers<int,5>::m_variables;   // actual storage for <int,5>
// When compiler sees Containers<int,7> used, it generates:
//   int Containers<int,7>::m_variables;   // separate storage for <int,7>
//
// COMPARE to non-template static definition:
// int Foo::count = 0;    // single definition, single storage
//
// Template version:
// template<typename T, int Size>
// T Containers<T,Size>::m_variables;   // template for definitions
//                                        // generates one per instantiation

// ============================================================
// SECTION 4: main() - PROVING SEPARATE COPIES
// ============================================================

int main()
{
    Containers<int, 10> c;
    // instantiates Containers<int,10>
    // generates: int Containers<int,10>::m_variables (= 0, zero-initialized)
    // c.m_data = new int[10]

    Containers<int, 7> c2;
    // instantiates Containers<int,7>
    // generates: int Containers<int,7>::m_variables (= 0, zero-initialized)
    // c2.m_data = new int[7]

    Containers<int, 5>::m_variables = 7;
    // accesses Containers<int,5>'s static member
    // NOTE: Containers<int,5> class is instantiated just from this line!
    // NO Containers<int,5> OBJECT is created (no stack/heap allocation)
    // just the STATIC member (which belongs to the class, not objects)
    // Containers<int,5>::m_variables = 7

    Containers<int, 7>::m_variables = 157;
    // accesses Containers<int,7>'s static member (DIFFERENT from <int,5>!)
    // c2 is an object of Containers<int,7>
    // this static is SHARED with c2 (same class)
    // Containers<int,7>::m_variables = 157
    // c2.m_variables would ALSO be 157 (same static storage)

    std::cout << Containers<int, 7>::m_variables << std::endl;
    // prints: 157
    // Containers<int,5>::m_variables = 7    (UNTOUCHED)
    // Containers<int,7>::m_variables = 157  (what we set above)
    // Containers<int,10>::m_variables = 0   (never set, zero-initialized)

    return 0;
}

// ============================================================
// SECTION 5: MEMORY LAYOUT - PROVING SEPARATION
// ============================================================

// DATA SEGMENT (static storage):
//
// Containers<int,5>::m_variables   = 7    [address: 0x601040]
// Containers<int,7>::m_variables   = 157  [address: 0x601044]
// Containers<int,10>::m_variables  = 0    [address: 0x601048]
//
// THREE SEPARATE memory locations in data segment!
// Each template instantiation = its own static variable
//
// STACK:
// c  (Containers<int,10>) -> m_data pointer -> heap[10 ints]
// c2 (Containers<int,7>)  -> m_data pointer -> heap[7 ints]
//
// NOTE: Containers<int,5> has NO object on stack
//       but its STATIC member DOES exist (accessed via :: directly)

// ============================================================
// SECTION 6: SHARED WITHIN SAME INSTANTIATION - IMPORTANT
// ============================================================

// Static member IS still shared among objects of the SAME instantiation:
//
// Containers<int,7> obj1;
// Containers<int,7> obj2;
// Containers<int,7>::m_variables = 99;
//
// obj1.m_variables == 99  // shared!
// obj2.m_variables == 99  // same storage!
//
// BUT:
// Containers<int,5>::m_variables == 7   // completely separate!
// Containers<int,7>::m_variables == 99  // its own copy
//
// SUMMARY:
// Static shared WITHIN  same <T,Size> instantiation (all <int,7> share one)
// Static SEPARATE ACROSS different <T,Size> instantiations (<int,5> vs <int,7>)

// ============================================================
// SECTION 7: REAL WORLD USE CASE - INSTANCE COUNTER PER TYPE
// ============================================================

// template<typename T>
// class TypedContainer {
// public:
//     TypedContainer()  { ++s_count; }
//     ~TypedContainer() { --s_count; }
//     static int getCount() { return s_count; }
// private:
//     static int s_count;
// };
// template<typename T>
// int TypedContainer<T>::s_count = 0;
//
// TypedContainer<int>    a, b, c;  // int  count = 3
// TypedContainer<float>  x, y;     // float count = 2
//
// TypedContainer<int>::getCount()   -> 3  (only int instances)
// TypedContainer<float>::getCount() -> 2  (only float instances)
//
// USEFUL: track how many of each type exist independently!
// If single class with single static: can't distinguish int vs float count

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: In a template class, how many copies of a static member exist?
//     -> ONE per TEMPLATE INSTANTIATION (per unique combination of
//        template arguments). Containers<int,5> and Containers<int,7>
//        each have their own separate static member variable.

// Q2: Why does Containers<int,5> and Containers<int,7> have separate statics?
//     -> Because they are DIFFERENT CLASSES (different template instantiations).
//        Containers<int,5> != Containers<int,7> as types.
//        Each class has its own set of static members.

// Q3: What is the syntax for defining a static member of a template class?
//     -> Must repeat template parameters:
//        template<typename T, int Size>
//        T Containers<T,Size>::m_variables;
//        NOT a single definition - a template for definitions.
//        Compiler generates one per instantiation used.

// Q4: Can you access a template class static member without creating an object?
//     -> YES. Containers<int,5>::m_variables = 7;
//        This only instantiates the CLASS (static member storage created),
//        not any OBJECT of that class. Static members belong to class, not objects.

// Q5: Are static members shared between objects of the SAME template instantiation?
//     -> YES. All Containers<int,7> objects share ONE m_variables.
//        Same behavior as non-template static members - shared within that class.
//        The difference is ACROSS instantiations (separate per <T,Size> combo).

// Q6: What is the zero-initialization of static members in templates?
//     -> Like all statics, template static members are zero-initialized
//        by default (ints to 0, pointers to nullptr, floats to 0.0).
//        Unless explicitly assigned (your code sets <int,5>=7, <int,7>=157).

// Q7: What real-world problem does per-instantiation static solve?
//     -> Counting instances per type independently.
//        TypedContainer<int>::count and TypedContainer<float>::count
//        are separate - can track int and float containers independently.
//        Single class + single static: can't distinguish types.

// Q8: Does defining template static member create storage immediately?
//     -> NO. The template definition creates a RECIPE.
//        Storage is created only when that specific instantiation is USED.
//        Containers<int,5>::m_variables storage created when compiler
//        first sees Containers<int,5> used (either object creation or :: access).
// ```

// **The visual proof of separate copies:**

// ```
// DATA SEGMENT:
// Containers<int, 5>::m_variables = 7    ← address 0x601040
// Containers<int, 7>::m_variables = 157  ← address 0x601044  (DIFFERENT address!)
// Containers<int,10>::m_variables = 0    ← address 0x601048  (DIFFERENT address!)

// Three separate variables in memory - one per template instantiation!
// ```

// **Non-template vs template static - the key difference:**

// ```cpp
// // NON-TEMPLATE: ONE static shared by ALL objects
// class Foo { static int count; };
// Foo a, b, c;  // all share ONE count

// // TEMPLATE: ONE static PER INSTANTIATION
// template<typename T>
// class Bar { static int count; };
// Bar<int>   a, b;  // share Bar<int>::count
// Bar<float> x, y;  // share Bar<float>::count (DIFFERENT variable!)
// // Bar<int>::count and Bar<float>::count are completely separate
// ```

// > **The mental model**: think of each template instantiation as a **brand new class** that happens to have been generated from the same blueprint. Brand new class = brand new set of static members. `Containers<int,5>` and `Containers<int,7>` are no more related (from a static member perspective) than two completely unrelated classes `ClassA` and `ClassB`.