// ============================================================
// SMART POINTERS - unique_ptr, RAII, OWNERSHIP - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <memory>  // unique_ptr, shared_ptr, make_unique

// ============================================================
// SECTION 1: PROBLEMS WITH RAW POINTERS (your commented code)
// ============================================================

// int* array = new int[100];
// int* ptr   = array;    // two pointers to same memory
// int* ptr2  = ptr;      // three pointers to same memory
//
// PROBLEM 1: WHO OWNS IT? who should call delete?
// -> ptr?  ptr2?  array?  all three?
// -> no clear ownership = confusion = bugs
//
// PROBLEM 2: DOUBLE FREE risk:
// delete[] array;   // freed
// delete[] ptr;     // double free! CRASH
//
// PROBLEM 3: MEMORY LEAK risk:
// // forgot delete[] -> 400 bytes leaked forever
//
// PROBLEM 4: DANGLING POINTER:
// delete[] array;
// *ptr = 5;         // ptr still points to freed memory -> UB
//
// RAW POINTERS ALLOW SHARING but with ZERO ownership rules
// programmer must manually track everything = error prone

// ============================================================
// SECTION 2: RAII - THE SOLUTION PRINCIPLE
// ============================================================

// RAII = Resource Acquisition Is Initialization
//
// RULE: tie resource lifetime to OBJECT lifetime
// -> acquire resource in CONSTRUCTOR
// -> release resource in DESTRUCTOR
// -> destructor ALWAYS called when object goes out of scope
// -> even if exception is thrown!
//
// std::unique_ptr implements RAII:
// -> constructor: takes ownership of raw pointer
// -> destructor:  automatically calls delete
// -> you NEVER call delete manually
// -> memory freed when unique_ptr goes out of scope

// ============================================================
// SECTION 3: UDT CLASS - PROVING AUTOMATIC DESTRUCTION
// ============================================================

class UDT
{
public:
    UDT()
    {
        std::cout << "UDT created" << std::endl;
        // called when object is constructed (new UDT)
    }
    ~UDT()
    {
        std::cout << "UDT Destroyed" << std::endl;
        // called automatically when unique_ptr goes out of scope
        // YOU never call this manually
        // RAII in action!
    }
};

int main()
{
    // ============================================================
    // SECTION 4: CREATING unique_ptr
    // ============================================================

    // WAY 1: explicit construction (your code)
    std::unique_ptr<UDT> aman = std::unique_ptr<UDT>(new UDT);
    // -> "UDT created" printed (constructor called)
    // -> aman owns the UDT object on heap
    // -> aman is on STACK, UDT object is on HEAP
    //
    // STACK:              HEAP:
    // [aman (ptr)] -----> [UDT object]
    //
    // WAY 2: make_unique (PREFERRED - modern C++)
    // std::unique_ptr<UDT> aman = std::make_unique<UDT>();
    // -> safer (exception safe), cleaner syntax
    // -> no raw new at all
    // -> factory pattern (creates and wraps in one step)
    //
    // WHY make_unique over new?
    // -> exception safety: new UDT can throw, make_unique handles it
    // -> no raw pointer ever exposed
    // -> cleaner, less typing

    // ============================================================
    // SECTION 5: UNIQUE OWNERSHIP - NO COPIES ALLOWED
    // ============================================================

    // std::unique_ptr<UDT> joe = aman;  // COMPILER ERROR!
    // -> copy constructor is DELETED in unique_ptr
    // -> "unique" means ONE owner at a time
    // -> if copies allowed, two ptrs would try to delete same memory
    //    = double free!
    //
    // THIS IS THE POINT:
    // compiler ENFORCES ownership rules
    // you CANNOT accidentally share ownership
    // raw pointer has no such protection

    // ============================================================
    // SECTION 6: std::move - TRANSFERRING OWNERSHIP
    // ============================================================

    std::unique_ptr<UDT> joe = std::move(aman);
    // WHAT HAPPENS:
    // -> aman's internal raw pointer STOLEN by joe
    // -> aman set to nullptr (empty, owns nothing)
    // -> joe now owns the UDT object
    // -> only ONE owner at any time (unique guarantee maintained)
    //
    // BEFORE move:
    // STACK:              HEAP:
    // [aman (ptr)] -----> [UDT object]
    // [joe (null)]
    //
    // AFTER move:
    // STACK:              HEAP:
    // [aman (null)]
    // [joe  (ptr)] -----> [UDT object]
    //
    // aman is now EMPTY - accessing *aman = crash (nullptr deref)
    // joe is the sole owner
    //
    // YOUR COMMENT: "mike is nulled out and joe has ownership"
    // 100% correct!

    // ============================================================
    // SECTION 7: AUTOMATIC DESTRUCTION AT SCOPE END
    // ============================================================

    // when main() returns:
    // -> joe goes out of scope
    // -> joe's destructor called
    // -> joe's destructor calls delete on the UDT pointer
    // -> UDT's destructor called -> "UDT Destroyed" printed
    // -> heap memory freed
    // -> YOU did nothing! RAII handled everything

    // FULL OUTPUT of this program:
    // UDT created       <- new UDT in aman's construction
    // UDT Destroyed     <- joe's unique_ptr destructor at scope end

    // ============================================================
    // SECTION 8: unique_ptr FOR ARRAYS
    // ============================================================

    // WAY 1: explicit
    // std::unique_ptr<UDT[]> arr = std::unique_ptr<UDT[]>(new UDT[10]);
    // -> prints "UDT created" 10 times

    // WAY 2: make_unique (preferred)
    // std::unique_ptr<UDT[]> arr = std::make_unique<UDT[]>(10);
    // -> same result, cleaner
    //
    // NOTE: unique_ptr<T[]> uses delete[] automatically
    //       unique_ptr<T>   uses delete  automatically
    //       correct version chosen based on template type!
    //       raw pointer: YOU must remember delete vs delete[]

    // ============================================================
    // SECTION 9: unique_ptr USEFUL METHODS
    // ============================================================

    // .get()      -> returns raw pointer (doesn't give up ownership)
    //               UDT* raw = joe.get();
    //               use for C APIs that need raw pointer

    // .release()  -> gives up ownership, returns raw pointer
    //               UDT* raw = joe.release();
    //               joe is now nullptr, YOU must delete raw!

    // .reset()    -> deletes current object, optionally takes new one
    //               joe.reset();           // delete UDT, joe = nullptr
    //               joe.reset(new UDT());  // delete old, own new one

    // operator*   -> dereference: *joe -> UDT object
    // operator->  -> member access: joe->someMethod()
    // operator bool -> if(joe) checks if not nullptr

    // ============================================================
    // SECTION 10: THREE SMART POINTERS - WHEN TO USE WHICH
    // ============================================================

    // unique_ptr<T>:
    // -> ONE owner, cannot copy, can move
    // -> zero overhead vs raw pointer
    // -> USE: default choice for heap objects
    //         factory functions returning heap objects
    //         class members owning heap resources

    // shared_ptr<T>:
    // -> MULTIPLE owners (reference counted)
    // -> copies allowed (increments count)
    // -> deleted when count reaches 0
    // -> small overhead (reference count on heap)
    // -> USE: when multiple objects need to share ownership
    //         callbacks that might outlive the creator

    // weak_ptr<T>:
    // -> observes shared_ptr WITHOUT owning
    // -> does not affect reference count
    // -> must lock() to access (might be null if object deleted)
    // -> USE: breaking circular references with shared_ptr
    //         caches, observers

    // ============================================================
    // SECTION 11: INTERVIEW QUESTIONS
    // ============================================================

    // Q1: What is RAII?
    //     -> Resource Acquisition Is Initialization.
    //        Resource tied to object lifetime.
    //        Constructor acquires, destructor releases automatically.
    //        Guarantees cleanup even on exceptions.

    // Q2: Why can't you copy a unique_ptr?
    //     -> Copy would create two owners of same memory.
    //        Both would try to delete it = double free.
    //        Copy constructor is explicitly deleted.
    //        Use std::move to transfer ownership instead.

    // Q3: What happens to unique_ptr when it goes out of scope?
    //     -> Destructor called automatically.
    //        Internally calls delete (or delete[] for arrays).
    //        No manual delete needed. Cannot leak.

    // Q4: What is the difference between .get() and .release()?
    //     -> .get()     : returns raw pointer, KEEPS ownership
    //        .release() : returns raw pointer, GIVES UP ownership
    //                     caller must manually delete after release()

    // Q5: Why prefer make_unique over new?
    //     -> Exception safe (no raw pointer exposed mid-expression)
    //        Cleaner syntax, less typing
    //        No accidental raw pointer management

    // Q6: What is the overhead of unique_ptr vs raw pointer?
    //     -> ZERO overhead. Same size as raw pointer (8 bytes).
    //        Destructor call is inlined by compiler.
    //        No runtime cost for the safety guarantee.

    // Q7: What is the difference between unique_ptr and shared_ptr?
    //     -> unique_ptr: one owner, no copy, move only, zero overhead
    //        shared_ptr: many owners, reference counted, small overhead
    //        Use unique_ptr by default, shared_ptr when sharing needed.

    // Q8: How does unique_ptr work in Qt context?
    //     -> Qt has its own QScopedPointer (similar to unique_ptr)
    //        Qt parent-child ownership (QObject parent deletes children)
    //        Modern Qt code uses unique_ptr alongside Qt ownership model
    //        Qt 5.12+ embraces standard smart pointers more fully

    return 0;
}
// PROGRAM OUTPUT:
// UDT created
// UDT Destroyed   <- automatic! you wrote zero delete calls
// ```

// **Ownership rules - the core interview answer:**

// ```
// raw pointer    -> NO ownership rules  -> YOU manage everything -> error prone
// unique_ptr     -> ONE owner           -> auto delete on scope exit -> safe
// shared_ptr     -> MANY owners         -> auto delete when count=0 -> safe
// weak_ptr       -> NO ownership        -> just observes shared_ptr -> safe
// ```

// **The zero-cost guarantee:**
// > `unique_ptr` has **identical performance** to raw pointer. You get 100% safety for 0% extra cost. There is **no reason** to use raw `new`/`delete` in modern C++ when `unique_ptr` exists.