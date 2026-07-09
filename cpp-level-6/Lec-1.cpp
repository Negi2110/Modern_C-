// ============================================================
// std::unique_ptr WITH CUSTOM DELETER - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <memory>

// ============================================================
// SECTION 1: YOUR QUESTION - "WHY ISN'T delete ENOUGH?"
// ============================================================

// NORMAL unique_ptr uses delete automatically:
// std::unique_ptr<int> p(new int(5));
// -> when p goes out of scope: delete p.get() called
// -> simple, works for heap memory
//
// BUT delete is NOT always the right cleanup action!
//
// CASES WHERE delete IS WRONG:
//
// CASE 1: C library resources (FILE*, socket, etc.)
// FILE* f = fopen("test.txt", "r");
// delete f;  // WRONG! file is not heap memory allocated with new
//            // must use: fclose(f)
//
// CASE 2: Arrays (need delete[] not delete)
// int* arr = new int[10];
// delete arr;   // WRONG! must be delete[]
// -> NOTE: std::unique_ptr<int[]> handles this automatically
//
// CASE 3: Custom memory pools
// int* p = MyPool::allocate();
// delete p;  // WRONG! must use MyPool::deallocate(p)
//
// CASE 4: SDL/OpenGL/OS resources
// SDL_Surface* surf = SDL_CreateSurface(...);
// delete surf;  // WRONG! must use SDL_FreeSurface(surf)
//
// CASE 5: Logging/debugging before cleanup
// -> want to print something BEFORE the deletion
// -> standard delete gives no hook for this
//
// SOLUTION: custom deleter = tell unique_ptr EXACTLY how to clean up

// ============================================================
// SECTION 2: CUSTOM DELETER - FUNCTOR STYLE (your code)
// ============================================================

struct IntDeleter
{
    void operator()(int* int_ptr)
    //   ^^^^^^^^^^
    //   operator() makes this a FUNCTOR (callable object)
    //   unique_ptr calls this instead of delete
    {
        std::cout << "IntDeleter called" << std::endl;
        std::cout << "the program has been called" << std::endl;
        delete int_ptr;   // still call delete here (for heap int)
                          // but now you have FULL CONTROL:
                          // can log, validate, null-check, etc.
    }
};
// IntDeleter is a FUNCTOR (class with operator())
// unique_ptr stores an instance of IntDeleter as part of its state
// when unique_ptr destructs -> calls IntDeleter::operator()(ptr)

// ============================================================
// SECTION 3: HOW unique_ptr WITH CUSTOM DELETER WORKS
// ============================================================

// std::unique_ptr<int, IntDeleter> my_ptr(new int);
//                 ^^^  ^^^^^^^^^^^
//                 |    DELETER TYPE (second template parameter)
//                 |    tells unique_ptr: "use IntDeleter, not default delete"
//                 TYPE of managed resource
//
// INTERNALLY unique_ptr stores:
// struct unique_ptr<int, IntDeleter> {
//     int* m_ptr;           // the raw pointer
//     IntDeleter m_deleter; // the deleter object
//                           // (empty struct = zero overhead via EBO!)
// };
//
// When unique_ptr goes out of scope:
// ~unique_ptr() { m_deleter(m_ptr); }
// -> calls IntDeleter::operator()(m_ptr)
// -> your custom code runs

// ============================================================
// SECTION 4: ALL CUSTOM DELETER STYLES
// ============================================================

// STYLE 1: FUNCTOR (your code - class with operator())
// struct IntDeleter {
//     void operator()(int* p) { delete p; }
// };
// std::unique_ptr<int, IntDeleter> p(new int);
// PRO: can have state (member variables), zero overhead (EBO)
// CON: more verbose to write

// STYLE 2: FUNCTION POINTER
// void myDeleter(int* p) {
//     std::cout << "custom delete\n";
//     delete p;
// }
// std::unique_ptr<int, void(*)(int*)> p(new int, myDeleter);
// PRO: simple for existing functions
// CON: function pointer has overhead (not inlinable), more verbose type

// STYLE 3: LAMBDA (most common in modern C++)
// auto myDeleter = [](int* p) {
//     std::cout << "lambda deleter\n";
//     delete p;
// };
// std::unique_ptr<int, decltype(myDeleter)> p(new int, myDeleter);
// PRO: inline, concise, can capture variables (state)
// CON: type is complex (decltype needed)
// C++20: can use auto for lambda type

// STYLE 4: std::function (most flexible, most overhead)
// std::unique_ptr<int, std::function<void(int*)>> p(
//     new int,
//     [](int* ptr) { delete ptr; }
// );
// PRO: most flexible, can store any callable
// CON: heap allocation overhead, not zero-cost

// ============================================================
// SECTION 5: REAL WORLD EXAMPLES - WHERE CUSTOM DELETER SHINES
// ============================================================

// EXAMPLE 1: C FILE handling
// struct FileDeleter {
//     void operator()(FILE* f) {
//         if (f) fclose(f);
//         std::cout << "File closed!\n";
//     }
// };
// std::unique_ptr<FILE, FileDeleter> filePtr(fopen("data.txt", "r"));
// -> file automatically closed when unique_ptr goes out of scope
// -> even if exception thrown!
// -> RAII for C resources!

// EXAMPLE 2: SDL Surface (game dev / graphics)
// struct SDLSurfaceDeleter {
//     void operator()(SDL_Surface* s) { SDL_FreeSurface(s); }
// };
// std::unique_ptr<SDL_Surface, SDLSurfaceDeleter> surface(
//     SDL_LoadBMP("image.bmp")
// );
// -> surface automatically freed even on exceptions

// EXAMPLE 3: Arrays with logging
// struct ArrayDeleter {
//     void operator()(int* arr) {
//         std::cout << "Freeing array\n";
//         delete[] arr;  // NOTE: delete[] for arrays!
//     }
// };
// std::unique_ptr<int, ArrayDeleter> arr(new int[100]);
// -> (alternatively: std::unique_ptr<int[]> arr(new int[100])
//    which auto-uses delete[] via built-in specialization)

// EXAMPLE 4: Qt context (relevant to your ISSST work)
// struct QObjectDeleter {
//     void operator()(QObject* obj) {
//         if (!obj->parent())  // only delete if no parent owner
//             delete obj;
//     }
// };

// ============================================================
// SECTION 6: EMPTY BASE OPTIMIZATION (EBO) - WHY FUNCTOR IS BEST
// ============================================================

// sizeof(std::unique_ptr<int>)                    = 8 bytes (just pointer)
// sizeof(std::unique_ptr<int, IntDeleter>)        = 8 bytes! (EBO applies)
// sizeof(std::unique_ptr<int, void(*)(int*)>)     = 16 bytes (pointer + fn ptr)
// sizeof(std::unique_ptr<int, std::function<...>>) = 32+ bytes
//
// EBO (Empty Base Optimization):
// -> if deleter has NO member variables (stateless)
// -> compiler can optimize it to ZERO extra bytes
// -> functor with no data members = zero overhead
// -> this is why functor style is most efficient!

// ============================================================
// SECTION 7: main() TRACE
// ============================================================

int main()
{
    std::unique_ptr<int, IntDeleter> my_ptr(new int);
    // allocates one int on heap
    // IntDeleter instance stored inside unique_ptr (EBO: zero extra bytes)
    // my_ptr owns the int

    return 0;
    // my_ptr goes out of scope
    // unique_ptr destructor: calls IntDeleter::operator()(ptr)
    // PRINTS: "IntDeleter called"
    // PRINTS: "the program has been called"
    // then: delete int_ptr (heap memory freed)
}

// OUTPUT:
// IntDeleter called
// the program has been called

// ============================================================
// SECTION 8: COMPARISON - DEFAULT vs CUSTOM DELETER
// ============================================================

// DEFAULT deleter (no custom):
// std::unique_ptr<int> p(new int);
// -> uses std::default_delete<int> internally
// -> std::default_delete<int>::operator()(ptr) calls: delete ptr
// -> simple, no overhead, no flexibility
//
// CUSTOM deleter:
// std::unique_ptr<int, IntDeleter> p(new int);
// -> uses YOUR IntDeleter
// -> IntDeleter::operator()(ptr) does whatever YOU decide
// -> log, validate, use fclose instead of delete, etc.
// -> FULL CONTROL over cleanup

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS
// ============================================================

// Q1: Why isn't delete always sufficient for unique_ptr cleanup?
//     -> delete only works for heap memory allocated with new.
//        C resources (FILE*, sockets) need fclose/closesocket.
//        Custom memory pools need their own deallocation.
//        Sometimes logging or validation before deletion is needed.

// Q2: What is a custom deleter in unique_ptr?
//     -> A callable (functor, function pointer, lambda) that
//        unique_ptr calls instead of delete when it goes out of scope.
//        Specified as the second template parameter: unique_ptr<T, Deleter>.

// Q3: What are the three main styles of custom deleters?
//     -> 1. Functor (struct with operator()): most efficient (EBO)
//        2. Function pointer: simple but has overhead, not inlinable
//        3. Lambda: modern, concise, can capture state

// Q4: Why is functor style most memory-efficient?
//     -> Empty Base Optimization (EBO). Stateless functors (no member vars)
//        contribute zero extra bytes to unique_ptr's size. Function pointers
//        always add 8 bytes (pointer size). Functors are also inlinable.

// Q5: What does unique_ptr internally do with the custom deleter?
//     -> Stores a deleter instance as a member (or uses EBO).
//        In destructor: calls deleter(m_ptr) instead of delete m_ptr.
//        For functor: calls operator()(m_ptr).
//        For function ptr: calls the function with m_ptr.

// Q6: How do you use unique_ptr to manage a C FILE*?
//     -> struct FileDeleter { void operator()(FILE* f) { fclose(f); } };
//        std::unique_ptr<FILE, FileDeleter> fp(fopen("test.txt","r"));
//        File automatically closed when fp goes out of scope.

// Q7: What is the size of unique_ptr with a stateless functor deleter?
//     -> Same as plain unique_ptr: 8 bytes (just the pointer).
//        EBO eliminates the empty functor's storage.
//        Function pointer deleter would be 16 bytes (ptr + fn ptr).

// Q8: Can the custom deleter have state?
//     -> YES. Functor with member variables can store state.
//        struct LoggingDeleter { string filename; void operator()(int* p){...} };
//        Lambda can capture variables.
//        With state: EBO doesn't apply, deleter occupies extra space.
// ```

// **The one-line answer to "why isn't delete enough?":**

// ```
// delete only cleans up one thing: heap memory from new.
// Custom deleter = FULL CONTROL over what "cleanup" means for ANY resource.
// ```

// **Three deleter styles - efficiency comparison:**

// ```cpp
// // FUNCTOR (best - zero overhead via EBO):
// struct Del { void operator()(int* p) { delete p; } };
// unique_ptr<int, Del> p1(new int);        // sizeof = 8 bytes

// // FUNCTION POINTER (has overhead):
// void del(int* p) { delete p; }
// unique_ptr<int, void(*)(int*)> p2(new int, del);  // sizeof = 16 bytes

// // LAMBDA (modern, concise):
// auto del = [](int* p){ delete p; };
// unique_ptr<int, decltype(del)> p3(new int, del);  // sizeof = 8 bytes (stateless)
// ```

// > **The ISSST connection**: in DIS/simulation code you'll encounter C APIs everywhere — SDL surfaces, POSIX file descriptors, network sockets, OpenGL handles. None of them use `new`/`delete`. Custom deleters let you wrap **any C resource** in a `unique_ptr` and get automatic RAII cleanup — the same guarantee you get for heap memory, extended to the entire world of OS and hardware resources.