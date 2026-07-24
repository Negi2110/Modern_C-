// ============================================================
// NAMESPACES, using, typedef, TYPE ALIASES - INTERVIEW NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: YOUR QUESTION - WHY NOT "using namespace std" AT TOP?
// ============================================================

// using namespace std;  // at global scope - WHY AVOID?
//
// PROBLEM 1: NAME COLLISION
// std has: std::distance, std::count, std::find, std::max etc.
// YOUR code might also have: distance(), count(), find()
//
// using namespace std;
// void distance() { }  // YOUR function
// distance();          // which one? AMBIGUOUS! compiler error
//                       // or silently picks wrong one
//
// PROBLEM 2: HIDDEN BUGS
// std::vector has push_back
// using namespace std;
// struct list { void push_back(int) {} };
// list l;
// l.push_back(1);  // is this list::push_back or std::list's? confusing!
//
// PROBLEM 3: IN HEADERS = DISASTER
// If you put "using namespace std" in a .hpp file:
// -> EVERY file that includes your .hpp gets the entire std namespace
// -> users cannot opt out
// -> namespace pollution spreads to all code
//
// PREFERRED APPROACHES (what your code shows):
// 1. using std::cout;           // import SPECIFIC name only
// 2. using namespace std;       // INSIDE a function (limited scope)
// 3. std::cout << "..."         // fully qualified (safest)

// ============================================================
// SECTION 2: typedef vs using - YOUR QUESTION "WHY NEED BOTH?"
// ============================================================

// ORIGINAL C way (typedef):
// typedef void void_t;
// typedef Datastructure<int,10> DatastructureInt;
//
// MODERN C++11 way (using alias):
// using void_t = void;
// using DatastructureInt = Datastructure<int,10>;
//
// FOR SIMPLE TYPES: identical, use whichever
// void_t foo() { }  // works with both
//
// THE KEY DIFFERENCE: TEMPLATES
//
// typedef CANNOT create template type aliases:
// template<class T>
// typedef Datastructure<T, 10> DatastructureT;  // COMPILER ERROR!
//                                                // typedef can't be templated!
//
// using CAN create template type aliases (alias templates):
// template<class T>
// using DatastructureT = Datastructure<T, 10>;  // WORKS! C++11
//
// DatastructureT<int>   t1;  // Datastructure<int, 10>
// DatastructureT<float> t2;  // Datastructure<float, 10>
//
// THIS IS WHY using is preferred over typedef in modern C++!
// using is a STRICT SUPERSET of typedef functionality

// ============================================================
// SECTION 3: YOUR CODE - PUTTING IT ALL TOGETHER
// ============================================================

using void_t = void;
// type alias: void_t is now another name for void
// used as return type of foo() below
// identical to: typedef void void_t;
// useful for: documenting intent ("this returns nothing explicitly")
// OR in template metaprogramming (std::void_t is a C++17 standard feature!)

template <class T, size_t N>
struct Datastructure
{
    // generic data structure parameterized by type T and size N
    // T = element type, N = capacity
};

// TEMPLATE ALIAS (cannot be done with typedef!):
template <class T>
using DatastructureInt = Datastructure<T, 10>;
//    ^^^^^^^^^^^^^^^^^                    ^^
//    alias name                           10 is FIXED
//    T is still generic                   only T changes
//
// DatastructureInt<int>   = Datastructure<int,   10>
// DatastructureInt<float> = Datastructure<float, 10>
// DatastructureInt<double>= Datastructure<double,10>
//
// WITHOUT alias: must write Datastructure<int,10> every time
// WITH alias:    just write DatastructureInt<int>
// -> reduces repetition, bakes in the size=10 assumption

// ============================================================
// SECTION 4: SCOPED using INSIDE FUNCTIONS
// ============================================================

void_t foo()
{
    using std::cout;
    // imports ONLY cout into foo()'s scope
    // -> cout works inside foo() without std::
    // -> does NOT pollute global scope or other functions
    // -> disappears when foo() returns

    cout << "will this work? \n";  // YES! works because of using std::cout
    // cout << still requires endl from std:: though:
    // cout << std::endl;  // need full qualification for endl
    // (or add: using std::endl;)
}

int main()
{
    using namespace std;
    // imports ALL of std namespace BUT:
    // -> ONLY inside main() (scoped to this function)
    // -> does NOT affect foo() or global scope
    // -> safer than global using namespace std
    // -> still potentially ambiguous within main() but contained

    cout << "mike's Modern C++ Series\n" << endl;
    // cout and endl work because of: using namespace std; above

    foo();
    // "will this work?" printed

    DatastructureInt<int>   t;   // = Datastructure<int,   10>
    DatastructureInt<float> t2;  // = Datastructure<float, 10>
    // template alias makes usage clean

    return 0;
}

// OUTPUT:
// mike's Modern C++ Series
//
// will this work?

// ============================================================
// SECTION 5: NAMESPACE ALIAS
// ============================================================

// Not in your code but related - NAMESPACE aliases:
// namespace LongNamespaceName { void foo() {} }
//
// namespace LNN = LongNamespaceName;  // namespace alias
// LNN::foo();  // instead of LongNamespaceName::foo()
//
// COMMON IN REAL CODE:
// namespace fs = std::filesystem;  // C++17
// fs::path p = "/home/aman";       // instead of std::filesystem::path
//
// namespace chrono = std::chrono;
// chrono::seconds s(5);            // instead of std::chrono::seconds

// ============================================================
// SECTION 6: std::void_t - WHERE void_t IS ACTUALLY USED
// ============================================================

// YOUR void_t is a simple demo but real use is in SFINAE:
//
// template<class T, class = void>
// struct has_size_member : std::false_type {};
//
// template<class T>
// struct has_size_member<T, std::void_t<decltype(std::declval<T>().size())>>
//     : std::true_type {};
//
// has_size_member<std::vector<int>>::value  // true (has .size())
// has_size_member<int>::value               // false (no .size())
//
// std::void_t<...> = "if this expression is valid, give me void"
// -> used to detect if a type has certain members/methods at compile time
// -> C++17 standard feature, your using void_t = void is the pre-C++17 version

// ============================================================
// SECTION 7: REAL WORLD using ALIASES YOU SEE EVERYWHERE
// ============================================================

// STL uses them internally:
// using size_type = std::size_t;
// using value_type = T;
// using pointer = T*;
// using reference = T&;
//
// YOUR own code (cleaner types):
// using EntityID = uint16_t;          // instead of raw uint16_t everywhere
// using PDUBuffer = std::vector<char>; // meaningful name
// using Clock = std::chrono::steady_clock;
//
// In Qt:
// using QStringList = QList<QString>;  // Qt uses this extensively

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: Why avoid "using namespace std" at global scope?
//     -> Risk of name collisions with std's common names (count, find,
//        distance, list). In headers, it pollutes all including files.
//        Prefer specific using declarations or fully qualified names.

// Q2: What is the difference between typedef and using?
//     -> Functionally identical for simple type aliases.
//        using is preferred because it can create TEMPLATE TYPE ALIASES
//        (alias templates), which typedef cannot.
//        template<class T> using Vec = std::vector<T>; // only with using

// Q3: What is an alias template?
//     -> A template that creates a type alias with some parameters fixed.
//        template<class T> using DatastructureInt = Datastructure<T, 10>;
//        DatastructureInt<int> = Datastructure<int, 10>.
//        Cannot be done with typedef.

// Q4: What does "using std::cout" inside a function do?
//     -> Imports ONLY cout into that function's scope.
//        Safer than "using namespace std" - only one name imported.
//        Scoped to the function - disappears when function exits.

// Q5: What is the best practice for using in headers?
//     -> NEVER put "using namespace X" in a header file.
//        NEVER put global "using namespace std" in a header.
//        Can put specific "using SomeType = ..." (type aliases) in headers.
//        Function-scoped using is safe anywhere.

// Q6: What is a namespace alias?
//     -> namespace fs = std::filesystem; creates fs as a short alias.
//        Then fs::path works instead of std::filesystem::path.
//        Common for long namespace names or to abstract library namespaces.

// Q7: When would you use "using namespace std" inside main()?
//     -> Small programs, examples, interview/competitive code where
//        brevity matters. Still not ideal but safer than global scope
//        since it's contained to one function.

// Q8: What is std::void_t used for in modern C++?
//     -> Used in SFINAE (Substitution Failure Is Not An Error) to detect
//        if a type has certain members or operations at compile time.
//        template<class T, class = std::void_t<decltype(T().size())>>
//        "if T has .size(), enable this template specialization."
// ```

// **typedef vs using - the template difference:**

// ```cpp
// // SIMPLE ALIAS - both work identically:
// typedef void void_t;        // C style
// using void_t = void;        // C++11 style (prefer)

// // TEMPLATE ALIAS - ONLY using works:
// template<class T>
// typedef vector<T> Vec;      // COMPILER ERROR! typedef can't be templated

// template<class T>
// using Vec = vector<T>;      // WORKS! alias template (C++11)
// Vec<int> v;                 // = vector<int>
// Vec<float> v2;              // = vector<float>
// ```

// **The three levels of "using" - scoped safety:**

// ```cpp
// // GLOBAL (avoid - pollutes everything):
// using namespace std;         // BAD in production code

// // FUNCTION-SCOPED (acceptable):
// void foo() {
//     using namespace std;     // OK: contained, disappears after foo()
//     using std::cout;         // BETTER: only imports what you need
// }

// // SPECIFIC NAME (best):
// using std::cout;             // BEST: one name, clear, findable
// using std::vector;
// ```

// > **The one answer that wins interviews**: *"I prefer `using` over `typedef` because it's more readable — the alias is on the left, the type on the right — but more importantly, `using` supports alias templates while `typedef` doesn't. That's the practical reason modern C++ style guides recommend `using` exclusively."*