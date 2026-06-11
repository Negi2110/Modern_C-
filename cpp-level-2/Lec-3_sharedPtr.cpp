// ============================================================
// shared_ptr, REFERENCE COUNTING, CONTROL BLOCK - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <memory>

// ============================================================
// SECTION 1: WHY shared_ptr EXISTS
// ============================================================

// unique_ptr = ONE owner, cannot share
// BUT sometimes multiple parts of code need SAME resource:
//
// Example:
// - Two objects both need access to a database connection
// - Multiple threads sharing same data
// - Observer pattern (many observers watching one subject)
//
// raw pointer "sharing":
// int* p1 = new int(42);
// int* p2 = p1;           // both point to same memory
// delete p1;              // freed
// delete p2;              // DOUBLE FREE - crash!
// -> no ownership tracking = disaster
//
// shared_ptr solution:
// both p1 and p2 know about each other via CONTROL BLOCK
// resource only deleted when LAST owner is gone

// ============================================================
// SECTION 2: CONTROL BLOCK - THE KEY MECHANISM
// ============================================================

// shared_ptr internally has TWO parts:
//
// 1. Pointer to the RESOURCE (your UDT object)
// 2. Pointer to CONTROL BLOCK (shared metadata)
//
// CONTROL BLOCK contains:
// -> reference count (how many shared_ptrs own this resource)
// -> weak count      (how many weak_ptrs observe this resource)
// -> deleter         (how to destroy the resource)
// -> allocator       (how memory was allocated)
//
// MEMORY LAYOUT:
//
// STACK:              HEAP:
// [ptr1]  ----+-----> [UDT object]     (the actual resource)
//             |
//             +-----> [Control Block]
//                      ref_count = 2
//                      weak_count = 0
// [ptr2]  ----+
// (ptr2 shares SAME control block as ptr1)
//
// make_shared OPTIMIZATION:
// std::make_shared<UDT>() allocates UDT AND control block
// in ONE heap allocation (more efficient!)
//
// new UDT way:
// shared_ptr<UDT>(new UDT) = TWO heap allocations
//   -> one for UDT
//   -> one for control block
// make_shared way:
// make_shared<UDT>() = ONE heap allocation
//   -> UDT and control block together in one block
// ALWAYS prefer make_shared for this reason!

class UDT
{
public:
    UDT()  { std::cout << "UDT created"   << std::endl; }
    ~UDT() { std::cout << "UDT Destroyed" << std::endl; }
};

// ============================================================
// SECTION 3: REFERENCE COUNT IS THREAD SAFE
// ============================================================

// Your comment: "reference count is thread safe"
// CORRECT - incrementing/decrementing ref count uses
// ATOMIC operations (hardware-level thread safety)
//
// BUT: the resource itself is NOT thread safe
// shared_ptr guarantees:
// -> ref count changes = thread safe (atomic)
// -> accessing the OBJECT through ptr = NOT thread safe
// -> use mutex if multiple threads read/write the UDT object

int main()
{
    {   // outer scope
        // ============================================================
        // SECTION 4: CREATING shared_ptr
        // ============================================================

        std::shared_ptr<UDT> ptr1 = std::make_shared<UDT>();
        // -> "UDT created" printed
        // -> ONE heap allocation (UDT + control block together)
        // -> ref count = 1 (only ptr1 owns it)
        //
        // STACK:         HEAP (one block):
        // [ptr1] ------> [UDT object | ref_count=1 | weak=0]

        {   // inner scope
            // ============================================================
            // SECTION 5: SHARING - COPY IS ALLOWED (unlike unique_ptr)
            // ============================================================

            std::shared_ptr<UDT> ptr2 = ptr1;
            // -> COPY of shared_ptr (NOT copy of UDT object!)
            // -> ptr2 points to SAME UDT, SAME control block
            // -> ref count INCREMENTED to 2
            //
            // STACK:         HEAP:
            // [ptr1] --+---> [UDT object | ref_count=2 | weak=0]
            // [ptr2] --+
            //
            // UDT object NOT duplicated - both see same object
            // control block atomically incremented

            std::cout << "use count = " << ptr2.use_count() << std::endl;
            // prints: use count = 2
            // ptr1.use_count() would also print 2 (same control block)

        }   // inner scope ends -> ptr2 DESTROYED
            // -> ptr2 destructor called
            // -> ref count DECREMENTED to 1
            // -> ref count != 0 so UDT NOT deleted yet
            // -> "UDT Destroyed" NOT printed here
            //
            // STACK:         HEAP:
            // [ptr1] ------> [UDT object | ref_count=1 | weak=0]
            // ptr2 gone

        std::cout << "use count = " << ptr1.use_count() << std::endl;
        // prints: use count = 1
        // only ptr1 remains

    }   // outer scope ends -> ptr1 DESTROYED
        // -> ptr1 destructor called
        // -> ref count DECREMENTED to 0
        // -> ref count == 0 -> UDT DELETED NOW
        // -> "UDT Destroyed" printed HERE
        // -> control block also freed

    std::cout << "destructor called before this line" << std::endl;
    // prints AFTER "UDT Destroyed"

    return 0;
}

// ============================================================
// SECTION 6: COMPLETE OUTPUT TRACE
// ============================================================

// UDT created           <- make_shared<UDT>() in outer scope
// use count = 2         <- ptr2 created, ref=2
// use count = 1         <- ptr2 destroyed, ref=1
// UDT Destroyed         <- ptr1 destroyed, ref=0, delete called
// destructor called before this line <- after all cleanup

// ============================================================
// SECTION 7: shared_ptr vs unique_ptr COMPARISON
// ============================================================

// | Feature              | unique_ptr        | shared_ptr            |
// |----------------------|-------------------|-----------------------|
// | Owners               | exactly ONE       | MANY                  |
// | Copy allowed         | NO (deleted)      | YES (increments count)|
// | Move allowed         | YES               | YES                   |
// | Overhead             | ZERO              | ref count + ctrl block|
// | Memory               | 8 bytes           | 16 bytes (2 pointers) |
// | Thread safe          | N/A (one owner)   | ref count only        |
// | Circular ref risk    | NO                | YES (use weak_ptr)    |
// | delete called when   | scope ends        | last owner destroyed  |
// | Use case             | sole ownership    | shared ownership      |

// ============================================================
// SECTION 8: CIRCULAR REFERENCE - shared_ptr WEAKNESS
// ============================================================

// DANGEROUS:
// struct Node {
//     std::shared_ptr<Node> next;  // points to next node
//     std::shared_ptr<Node> prev;  // points to prev node
// };
//
// shared_ptr<Node> a = make_shared<Node>();  // ref=1
// shared_ptr<Node> b = make_shared<Node>();  // ref=1
// a->next = b;  // b ref=2
// b->prev = a;  // a ref=2
//
// a goes out of scope: a ref = 1 (b->prev still holds it)
// b goes out of scope: b ref = 1 (a->next still holds it)
// NEITHER ever reaches 0 -> MEMORY LEAK!
//
// FIX: use weak_ptr for back-references
// struct Node {
//     std::shared_ptr<Node> next;   // strong ownership forward
//     std::weak_ptr<Node>   prev;   // weak observation backward
// };
// weak_ptr does NOT increment ref count
// cycle broken -> proper cleanup

// ============================================================
// SECTION 9: weak_ptr - OBSERVING WITHOUT OWNING
// ============================================================

// std::weak_ptr<UDT> weak = ptr1;
// -> does NOT increment ref count
// -> just observes the resource
// -> resource can be deleted even while weak_ptr exists
//
// TO USE weak_ptr:
// must LOCK it first (converts to shared_ptr temporarily)
// if (auto locked = weak.lock()) {
//     // resource still alive, locked is shared_ptr
//     locked->someMethod();
// } else {
//     // resource was deleted, locked is nullptr
// }

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is shared_ptr and how does it work?
//     -> Smart pointer with shared ownership via reference counting.
//        Control block tracks how many shared_ptrs own the resource.
//        Resource deleted when count reaches zero.

// Q2: What is a control block?
//     -> Heap-allocated metadata shared by all shared_ptrs to same object.
//        Contains: ref count, weak count, deleter, allocator.
//        Created once, shared by all copies of shared_ptr.

// Q3: Why prefer make_shared over shared_ptr(new T)?
//     -> make_shared: ONE heap allocation (object + control block together)
//        new T way:   TWO heap allocations (object separate from control block)
//        make_shared is faster and more cache-friendly.

// Q4: Is shared_ptr thread safe?
//     -> ref count operations ARE thread safe (atomic).
//        The RESOURCE itself is NOT thread safe.
//        Multiple threads reading/writing object = need mutex.

// Q5: What is a circular reference and how to fix it?
//     -> Two shared_ptrs pointing to each other.
//        Ref count never reaches 0 -> memory leak.
//        Fix: use weak_ptr for one direction of the cycle.

// Q6: What is weak_ptr?
//     -> Observes a shared_ptr without owning (no ref count increment).
//        Resource can be deleted while weak_ptr exists.
//        Must call .lock() to access (returns nullptr if deleted).
//        Used to break circular references.

// Q7: What is the size of shared_ptr?
//     -> 16 bytes (two pointers: one to resource, one to control block)
//        unique_ptr = 8 bytes (one pointer, zero overhead)

// Q8: When should you use shared_ptr vs unique_ptr?
//     -> unique_ptr: default choice, sole ownership, zero overhead
//        shared_ptr: only when multiple owners genuinely needed
//        Don't use shared_ptr "just in case" - unique_ptr first,
//        upgrade to shared_ptr only when sharing is actually needed.
// ```

// **Reference count lifecycle - the interview trace:**

// ```
// make_shared<UDT>()     ref = 1   "UDT created"
// ptr2 = ptr1            ref = 2
// ptr2 scope ends        ref = 1   (NO destruction yet)
// ptr1 scope ends        ref = 0   "UDT Destroyed" <- only NOW
// ```

// > The golden rule: **object deleted only when the LAST shared_ptr is destroyed**. Not when the first one leaves scope — when the **last** one does. The control block counts silently in the background.