
// ============================================================
// FROM FIRST PRINCIPLES - WHY WE WRITE CUSTOM VERSIONS
// ============================================================

// ============================================================
// STEP 1: UNDERSTAND WHAT C++ DOES FOR FREE
// ============================================================

// When you write a class, C++ gives you 4 things FREE:
//
// class Array {
//     int* data;   // raw pointer to heap
// };
//
// C++ auto-generates:
//
// 1. Array()              -> default constructor
//    does: nothing        -> data = garbage pointer (dangerous!)
//
// 2. ~Array()             -> destructor
//    does: nothing        -> heap memory NEVER freed = memory leak
//
// 3. Array(const Array&)  -> copy constructor
//    does: data = rhs.data -> copies POINTER (shallow!)
//
// 4. operator=            -> copy assignment
//    does: data = rhs.data -> copies POINTER (shallow!)
//
// FOR SIMPLE CLASSES (no heap):
// class Vector3 { float x,y,z; };  // no raw pointer
// -> free versions work PERFECTLY
// -> compiler copies x, y, z values directly
// -> no heap involved, no danger
//
// FOR CLASSES WITH HEAP (raw pointer):
// class Array { int* data; };  // raw pointer!
// -> free versions are WRONG and DANGEROUS
// -> THIS is why you write custom versions

// ============================================================
// STEP 2: THE REAL WORLD ANALOGY - UNDERSTAND IT FOREVER
// ============================================================

// Think of your class like a HOUSE that has a GARAGE KEY:
//
// class Array {
//     int* data;   // = the GARAGE KEY (pointer)
//                  // the garage itself = heap memory
// };
//
// SHALLOW COPY (what compiler gives):
// Array a2 = a1;
// -> compiler copies the KEY
// -> now a1 and a2 have COPIES OF THE SAME KEY
// -> BOTH open the SAME GARAGE
//
// PROBLEM:
// a1 destroyed -> a1 locks and DEMOLISHES the garage (~Array)
// a2 destroyed -> a2 tries to DEMOLISH the same garage again
// -> garage already demolished -> CRASH (double free)
//
// DEEP COPY (what you write):
// Array a2 = a1;
// -> you BUILD A NEW GARAGE for a2
// -> copy all CONTENTS from a1's garage into a2's garage
// -> a2 gets its OWN KEY to its OWN GARAGE
//
// a1 destroyed -> demolishes a1's garage (safe)
// a2 destroyed -> demolishes a2's garage (safe)
// -> two separate garages, no conflict

// ============================================================
// STEP 3: WHY EACH CUSTOM FUNCTION IS NEEDED - ONE BY ONE
// ============================================================

// ---- WHY CUSTOM DESTRUCTOR? ----
//
// FREE version does:   nothing
//
// class Array {
//     int* data;
//     Array() { data = new int[10]; }  // allocates garage
//     // no destructor
// };
//
// Array a;     // garage built    (heap allocated)
// }            // a destroyed     (scope ends)
//              // FREE destructor: does NOTHING
//              // garage NEVER demolished
//              // 40 bytes leaked EVERY TIME object created and destroyed
//              // in long running program -> RAM fills up -> crash
//
// YOUR custom destructor:
// ~Array() { delete[] data; }
// -> properly demolishes the garage when object dies
// -> no leak

// ---- WHY CUSTOM COPY CONSTRUCTOR? ----
//
// FREE version does:   data = rhs.data  (copies the KEY not the garage)
//
// Array a1;           // a1 builds its garage, gets key1
// Array a2 = a1;      // FREE copy ctor: a2.data = a1.data
//                     // a2 gets COPY OF key1 (same garage!)
//
// a2.setData(0, 999); // modifies a1's garage too! (same memory!)
//                     // because a2.data == a1.data (same address)
//
// }  // scope ends
//    // ~Array() called for a2: delete[] a2.data -> garage demolished
//    // ~Array() called for a1: delete[] a1.data -> SAME garage!
//    //                         DOUBLE FREE -> crash
//
// YOUR custom copy constructor:
// Array(const Array& rhs) {
//     data = new int[10];           // BUILD NEW GARAGE for a2
//     for(...) data[i]=rhs.data[i]; // COPY CONTENTS from a1's garage
// }
// -> a2 has its own separate garage
// -> modifying a2 doesn't affect a1
// -> destructors free different garages -> no crash

// ---- WHY CUSTOM COPY ASSIGNMENT? ----
//
// FREE version does:   data = rhs.data  (same shallow copy problem)
//
// BUT copy assignment has an EXTRA problem copy constructor doesn't:
// the object ALREADY EXISTS with its own garage!
//
// Array a1;    // a1 builds garage1
// Array a2;    // a2 builds garage2
// a2 = a1;     // FREE assignment: a2.data = a1.data
//              // a2's KEY now points to garage1
//              // garage2 is ABANDONED (no key, can never be freed!)
//              // MEMORY LEAK + a2 and a1 now share garage1 -> double free
//
// YOUR custom copy assignment:
// Array& operator=(const Array& rhs) {
//     if (&rhs == this) return *this; // self-assign check
//     delete[] data;                  // DEMOLISH a2's OLD garage2 first
//     data = new int[10];             // BUILD NEW garage for a2
//     for(...) data[i]=rhs.data[i];   // COPY contents from a1's garage
//     return *this;
// }
// -> old garage2 properly demolished (no leak)
// -> a2 gets fresh garage with a1's contents
// -> a1 keeps its own garage
// -> destructors free separate garages -> no crash

// ============================================================
// STEP 4: VISUAL MEMORY PROOF
// ============================================================

// SHALLOW COPY (compiler free version):
//
// STACK:         HEAP:
// a1.data = 100 --> [0][1][4][9]...[81]   address 100
// a2.data = 100 --> [0][1][4][9]...[81]   address 100
//            ^same address!^
//
// a1 dies: delete[] (address 100) -> freed
// a2 dies: delete[] (address 100) -> DOUBLE FREE CRASH!
//
// DEEP COPY (your custom version):
//
// STACK:         HEAP:
// a1.data = 100 --> [0][1][4][9]...[81]   address 100
// a2.data = 200 --> [0][1][4][9]...[81]   address 200
//            ^different addresses!^
//
// a1 dies: delete[] (address 100) -> freed safely
// a2 dies: delete[] (address 200) -> freed safely
// NO crash!

// ============================================================
// STEP 5: THE RULE TO REMEMBER - RULE OF THREE
// ============================================================

// IF your class has a raw pointer member (manages heap memory):
//
// SYMPTOM: "I need a custom destructor because free version leaks"
// CONSEQUENCE: "then copy constructor and copy assignment are also wrong"
// RULE: write ALL THREE or bugs are guaranteed
//
// | Function          | Free version problem     | Your fix           |
// |-------------------|--------------------------|---------------------|
// | Destructor        | doesn't free heap        | delete[] data       |
// | Copy constructor  | copies pointer (shallow) | new + copy values   |
// | Copy assignment   | copies pointer + leak    | delete old + new    |
//
// THESE THREE ALWAYS COME TOGETHER for heap-managing classes
// Miss any one -> either memory leak, double free, or silent corruption

// ============================================================
// STEP 6: TECHNICAL INTERVIEW ANSWER
// ============================================================

// Q: "Why did you write a custom destructor, copy constructor,
//     and copy assignment operator?"
//
// A: "My class manages a raw heap resource (int* data).
//
//     The compiler-generated versions perform SHALLOW COPY -
//     they copy the POINTER VALUE, not the heap data itself.
//     This means multiple objects end up pointing to the same
//     heap memory.
//
//     This causes TWO specific bugs:
//     1. DOUBLE FREE: when both objects are destroyed, both
//        destructors call delete[] on the same address -> crash
//     2. UNINTENDED ALIASING: modifying one object's data
//        modifies the other's too (same memory block)
//
//     My custom versions implement DEEP COPY:
//     -> Destructor:       frees the heap memory
//     -> Copy constructor: allocates NEW heap, copies VALUES
//     -> Copy assignment:  frees OLD heap, allocates NEW heap,
//                          copies VALUES, handles self-assignment
//
//     This follows the RULE OF THREE: if you need any one of
//     these three, you almost certainly need all three."

int main()
{
    // everything below now works CORRECTLY with deep copy:

    Array a1;              // constructor: builds garage1
    Array a2 = a1;         // copy constructor: builds garage2
                           //                  copies a1's contents

    a1.setData(0, 999);    // modifies ONLY a1's garage
    a2.printData();        // a2 still has original values (separate garage!)

    Array a3;              // constructor: builds garage3
    a3 = a1;               // copy ASSIGNMENT: frees garage3,
                           //                  builds new garage,
                           //                  copies a1's contents

    return 0;
    // a3 destroyed: delete[] a3's heap (safe)
    // a2 destroyed: delete[] a2's heap (safe)
    // a1 destroyed: delete[] a1's heap (safe)
    // ALL separate -> no double free!
}
// ```

// **The garage analogy in one picture:**

// ```
// SHALLOW (compiler free - WRONG for heap):
// a1 ──key──> [garage with stuff]
// a2 ──key──>      same garage!
// → a1 demolished it → a2 tries again → CRASH

// DEEP (your custom - CORRECT):
// a1 ──key──> [garage1 with stuff]
// a2 ──key──> [garage2 with stuff]  ← NEW garage, COPIED contents
// → a1 demolishes garage1 → safe
// → a2 demolishes garage2 → safe
// ```

// > **Bottom line**: C++ gives you free versions that work perfectly for simple data (`int`, `float`). The moment your class has a **raw pointer to heap**, those free versions become dangerous. You write custom versions to say: *"don't copy my key, build a new garage."*


// Great question. Because **copy constructor and copy assignment are called in different situations:**

// ```cpp
// Array a1;           // default constructor

// // COPY CONSTRUCTOR called here:
// Array a2 = a1;      // a2 does NOT exist yet -> being BORN from a1
// Array a2(a1);       // same thing, explicit version

// // COPY ASSIGNMENT called here:
// Array a2;           // a2 BORN (default constructor)
// a2 = a1;            // a2 ALREADY EXISTS -> being ASSIGNED new value
// ```

// **They handle two completely different moments in an object's life:**

// ```
// Copy Constructor  = object is BORN and initialized from another object
//                     (one step: birth + copy together)

// Copy Assignment   = object ALREADY ALIVE, gets new value from another
//                     (object existed before this line)
// ```

// **Why you cannot use copy constructor to handle the second case:**

// ```cpp
// Array a2;      // a2 already born here (default constructor ran)
//                // a2 already has its OWN heap memory allocated!

// a2 = a1;       // NOW what?
//                // a2 already EXISTS with its own heap
//                // copy constructor cannot help here
//                // copy constructor only runs at BIRTH
//                // it already ran (at Array a2; line)
//                // it will NEVER run again for a2
// ```

// **The extra problem copy assignment must handle that copy constructor never faces:**

// ```cpp
// // Copy constructor:
// Array(const Array& rhs) {
//     // a2 is FRESH, data pointer is uninitialized
//     // nothing to free, just allocate and copy
//     data = new int[10];
//     for(...) data[i] = rhs.data[i];
// }

// // Copy assignment:
// Array& operator=(const Array& rhs) {
//     // a2 ALREADY HAS heap memory from when it was born!
//     // if you just do:
//     // data = new int[10];  <- YOU JUST LOST the old heap address!
//     //                         that old heap block is now LEAKED forever
//     //
//     // MUST free old heap FIRST:
//     delete[] data;         // free a2's existing garage
//     data = new int[10];    // build new garage
//     for(...) data[i] = rhs.data[i];
//     return *this;
// }
// ```

// **One line summary:**

// ```
// Copy constructor  -> object has NO previous life  -> just allocate + copy
// Copy assignment   -> object HAS previous life     -> free old + allocate + copy
// ```

// > Think of it like moving houses.
// > **Copy constructor** = you are moving into your **first ever house** (nothing to pack up, just move in)
// > **Copy assignment** = you **already live somewhere**, now moving to new house (must sell/leave old house first, THEN move in)
// > 
// > You cannot use "moving into first house" logic when you already own a house — you'd abandon your old house without selling it (memory leak).