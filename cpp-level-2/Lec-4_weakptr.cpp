// ============================================================
// weak_ptr - NON-OWNING REFERENCE, CYCLES, expired() - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <memory>

// ============================================================
// SECTION 1: WHAT IS weak_ptr - FROM FIRST PRINCIPLES
// ============================================================

// shared_ptr = OWNING reference  -> increments ref count
// weak_ptr   = NON-OWNING reference -> does NOT increment ref count
//
// weak_ptr answers this question:
// "I want to KNOW about this object but I don't want to be
//  responsible for keeping it alive"
//
// Real world analogy:
// shared_ptr = person who OWNS a house (pays mortgage, keeps it)
// weak_ptr   = neighbor who can SEE the house but doesn't own it
//              if all owners sell/leave -> house demolished
//              neighbor cannot prevent demolition
//              neighbor must CHECK if house still exists before visiting

// ============================================================
// SECTION 2: YOUR COMMENT - WHY weak_ptr IS A "SAFER DANGLING POINTER"
// ============================================================

// raw dangling pointer:
// int* px = new int(42);
// delete px;            // memory freed
// *px = 99;             // SILENT undefined behavior, possible crash
//                       // pointer looks valid but isn't
//                       // NO WAY to know it's dangling!
//
// weak_ptr "dangling":
// weak_ptr<UDT> wp = ptr1;
// ptr1 reset/destroyed  // resource freed
// wp.expired()          // CHECK: returns true (safely tells you it's gone)
// wp.lock()             // returns nullptr (safely tells you it's gone)
//
// weak_ptr gives you a SAFE way to have a potentially dangling reference
// you can always CHECK before accessing
// raw pointer gives you NO such check

class UDT
{
public:
    UDT()  { std::cout << "UDT created"   << std::endl; }
    ~UDT() { std::cout << "UDT Destroyed" << std::endl; }
};

int main()
{
    // ============================================================
    // SECTION 3: weak_ptr DECLARED OUTSIDE SCOPE
    // ============================================================

    std::weak_ptr<UDT> ptr2;
    // ptr2 is empty weak_ptr (points to nothing)
    // declared OUTSIDE inner scope deliberately
    // so we can test it AFTER shared_ptr is destroyed

    {   // outer scope - shared_ptr lives here
        std::shared_ptr<UDT> ptr1 = std::make_shared<UDT>();
        // "UDT created" printed
        // ref count = 1
        // control block: [ref=1, weak=0]
        //
        // HEAP:
        // [UDT object | ref_count=1 | weak_count=0]
        //      ^
        //      ptr1 owns this

        {   // inner scope
            ptr2 = ptr1;
            // ptr2 is weak_ptr assigned from shared_ptr
            // ref count = STILL 1 (weak_ptr does NOT increment!)
            // weak count = 1 (weak_ptr DOES increment weak count)
            //
            // control block: [ref=1, weak=1]
            //
            // HEAP:
            // [UDT object | ref_count=1 | weak_count=1]
            //      ^              ^
            //      ptr1 owns      ptr2 observes (no ownership)

            std::cout << "use count (inside scope) = "
                      << ptr2.use_count() << std::endl;
            // prints: 1
            // use_count() on weak_ptr shows shared ref count
            // still 1 because weak_ptr didn't change it

        }   // inner scope ends
            // ptr2 (weak_ptr) is still ALIVE (declared in outer scope)
            // nothing changes for shared_ptr ownership
            // ptr2 just observes, its scope hasn't ended

        std::cout << "use count = " << ptr1.use_count() << std::endl;
        // prints: 1
        // ptr1 is still sole owner
        // weak_ptr never changed the count

    }   // outer scope ends -> ptr1 DESTROYED
        // ref count decremented: 1 -> 0
        // ref count = 0 -> UDT DELETED
        // "UDT Destroyed" printed HERE
        // control block: [ref=0, weak=1]
        // control block itself NOT freed yet (weak_count=1 still)
        // ptr2 still exists and needs control block to check expired()

    // ============================================================
    // SECTION 4: expired() - CHECKING IF RESOURCE STILL ALIVE
    // ============================================================

    std::cout << "expired: " << ptr2.expired() << std::endl;
    // prints: 1 (true = expired)
    // expired() checks if ref count == 0
    // ref count IS 0 (ptr1 was destroyed in outer scope)
    // so resource is gone -> expired() = true (1)
    //
    // ptr2 still EXISTS as a variable but points to deleted memory
    // BUT unlike raw dangling pointer, we can SAFELY detect this!

    std::cout << "destructor called before this line" << std::endl;

    return 0;
    // ptr2 goes out of scope here
    // weak_count decremented: 1 -> 0
    // control block NOW freed (both ref=0 AND weak=0)
}

// ============================================================
// SECTION 5: COMPLETE OUTPUT TRACE
// ============================================================

// UDT created                          <- make_shared in outer scope
// use count (inside scope) = 1         <- weak_ptr doesn't increment
// use count = 1                        <- still only ptr1 owns it
// UDT Destroyed                        <- ptr1 scope ends, ref=0
// expired: 1                           <- resource gone, expired=true
// destructor called before this line

// ============================================================
// SECTION 6: HOW TO SAFELY USE weak_ptr - lock()
// ============================================================

// WRONG - dangerous:
// weak_ptr<UDT> wp = ptr1;
// UDT* raw = wp.???   // can't dereference weak_ptr directly!
// wp->method()        // COMPILER ERROR - no operator-> on weak_ptr
//
// CORRECT - use lock():
// weak_ptr<UDT> wp = ptr1;
//
// if (auto locked = wp.lock()) {
//     // lock() returns shared_ptr
//     // if resource alive  -> returns valid shared_ptr (ref++ temporarily)
//     // if resource dead   -> returns nullptr shared_ptr
//     locked->someMethod();  // SAFE - resource guaranteed alive here
//     // locked goes out of scope -> ref-- again
// } else {
//     std::cout << "resource is gone!" << std::endl;
// }
//
// DURING lock():
// temporarily creates shared_ptr -> ref count bumped to 1 (at least)
// resource CANNOT be deleted while locked shared_ptr exists
// ATOMIC operation - thread safe check + access

// ============================================================
// SECTION 7: BREAKING CYCLES - THE MAIN USE CASE
// ============================================================

// CIRCULAR REFERENCE PROBLEM (shared_ptr only):
// struct Node {
//     shared_ptr<Node> next;  // A points to B
//     shared_ptr<Node> prev;  // B points back to A
// };
// shared_ptr<Node> A = make_shared<Node>(); // ref=1
// shared_ptr<Node> B = make_shared<Node>(); // ref=1
// A->next = B;  // B ref=2
// B->prev = A;  // A ref=2
//
// A goes out of scope: A ref = 1 (B->prev holds it) NOT deleted
// B goes out of scope: B ref = 1 (A->next holds it) NOT deleted
// MEMORY LEAK - neither ever freed!
//
// YOUR COMMENT: sp1->sp2->sp3->sp1 = cycle = leak
//
// FIX WITH weak_ptr:
// struct Node {
//     shared_ptr<Node> next;   // forward = strong (owns next)
//     weak_ptr<Node>   prev;   // backward = weak (doesn't own prev)
// };
// A->next = B;   // B ref=2
// B->prev = A;   // A ref=1 still! (weak_ptr!)
//
// A goes out of scope: A ref = 0 -> A DELETED
// A's deletion removes A->next: B ref = 1 -> B DELETED
// NO LEAK!

// ============================================================
// SECTION 8: CONTROL BLOCK LIFECYCLE - COMPLETE PICTURE
// ============================================================

// Control block freed only when BOTH:
// ref_count == 0  (all shared_ptrs gone)
// weak_count == 0 (all weak_ptrs gone)
//
// TIMELINE for your code:
//
// make_shared:    [ref=1, weak=0]  UDT alive
// ptr2 = ptr1:    [ref=1, weak=1]  UDT alive
// ptr1 destroyed: [ref=0, weak=1]  UDT DELETED, ctrl block alive
// ptr2.expired(): reads ctrl block safely (weak=1, ctrl alive)
// ptr2 destroyed: [ref=0, weak=0]  CTRL BLOCK DELETED

// ============================================================
// SECTION 9: THREE SMART POINTERS - COMPLETE DECISION TREE
// ============================================================

// Do you need shared ownership?
//   NO  -> unique_ptr (default, zero overhead)
//   YES -> Do you want to PARTICIPATE in ownership?
//            YES -> shared_ptr (ref counted)
//            NO  -> weak_ptr  (observe only, no ownership)

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is weak_ptr?
//     -> Non-owning smart pointer that observes a shared_ptr's resource.
//        Does not increment ref count.
//        Resource can be deleted while weak_ptr still exists.
//        Must use lock() to access safely.

// Q2: What is the difference between shared_ptr and weak_ptr?
//     -> shared_ptr: owns resource, increments ref count, keeps alive
//        weak_ptr:   observes only, no ref count, resource can die

// Q3: What does expired() return?
//     -> true (1) if ref count == 0 (resource deleted)
//        false (0) if resource still alive
//        Safe way to check if observed resource is still valid.

// Q4: How do you access the resource through weak_ptr?
//     -> Use .lock() which returns a shared_ptr.
//        If alive: valid shared_ptr (temporarily bumps ref count)
//        If dead:  nullptr shared_ptr
//        Always check before using: if(auto sp = wp.lock()) { }

// Q5: When is the control block freed?
//     -> When BOTH ref_count AND weak_count reach zero.
//        Resource freed when ref_count = 0.
//        Control block freed when weak_count also = 0.

// Q6: What is a circular reference and how does weak_ptr fix it?
//     -> Two shared_ptrs pointing to each other -> ref never reaches 0
//        -> memory leak.
//        Fix: make one direction weak_ptr (no ownership of back-reference)
//        -> cycle broken -> proper cleanup.

// Q7: Why can't you dereference weak_ptr directly?
//     -> Resource might be deleted at any moment.
//        Dereferencing without checking = potential dangling access.
//        lock() forces you to CHECK and temporarily own it safely.

// Q8: What is the difference between weak_ptr and raw dangling pointer?
//     -> Raw dangling: no way to detect it's invalid, UB when accessed
//        weak_ptr:     expired() tells you safely, lock() returns nullptr
//        weak_ptr is intentionally "possibly dangling" but SAFELY so.
// ```

// **The complete smart pointer decision tree:**

// ```
// Need heap object?
// │
// ├── ONE owner only?
// │     └── unique_ptr  ← default choice, zero overhead
// │
// ├── MULTIPLE owners?
// │     └── shared_ptr  ← ref counted, small overhead
// │
// └── Want to OBSERVE shared_ptr without owning?
//       └── weak_ptr    ← no ref count, must lock() to access
// ```

// **Control block lifecycle - always asked:**
// ```
// make_shared:     ref=1  weak=0   → UDT alive
// weak = shared:   ref=1  weak=1   → UDT alive
// shared dies:     ref=0  weak=1   → UDT DELETED, block alive
// weak dies:       ref=0  weak=0   → BLOCK DELETED
// ```

// > `weak_ptr` exists for exactly **two reasons**: breaking circular references and safely observing an object you don't own. If neither applies, you don't need it.