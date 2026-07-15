
// ============================================================
// INTERVIEW QUESTION: Casting Base* to Derived* - FULL ANSWER
// ============================================================

// QUESTION 1: Which C++ feature to cast Base* to Derived*?
// ANSWER: dynamic_cast (preferred) OR static_cast (if you KNOW it's safe)

// ============================================================
// THE TWO SCENARIOS - THIS IS THE KEY
// ============================================================

// SCENARIO A: Base* B = new Base;
// B actually points to a BASE object
//
// (Derived*)B->Do();              // C-style: COMPILES, UNDEFINED BEHAVIOR
//                                  // B is not Derived, reads garbage memory
//
// dynamic_cast<Derived*>(B)       // returns nullptr (safe!)
//                                  // "B is not actually a Derived" -> null
//
// if (dynamic_cast<Derived*>(B)) {
//     std::cout << "this works";  // NEVER PRINTS - correctly blocked!
// }
// -> dynamic_cast CAUGHT THE INVALID CAST
// -> nullptr = false = if block skipped = no crash

// ============================================================

// SCENARIO B: Base* B = new Derived;
// B is declared as Base* BUT actually points to a DERIVED object
// THIS is the common polymorphism pattern!
//
// (Derived*)B->Do();              // C-style: works accidentally
//                                  // but dangerous (no check!)
//
// dynamic_cast<Derived*>(B)       // returns valid Derived* (NOT null)
//                                  // "B actually IS a Derived" -> valid ptr
//
// if (dynamic_cast<Derived*>(B)) {
//     std::cout << "this works";  // PRINTS - cast is valid!
// }
// -> dynamic_cast VERIFIED the cast is safe at runtime

// ============================================================
// SIDE BY SIDE CODE PROOF
// ============================================================

// #include <iostream>

// struct Base {
//     virtual void Do() { std::cout << "Base::Do()\n"; }
//     virtual ~Base() = default;
// };

// struct Derived : Base {
//     void Do() override { std::cout << "Derived::Do()\n"; }
// };

// int main()
// {
//     // ---- SCENARIO A: actual Base object ----
//     Base* B1 = new Base;

//     if (dynamic_cast<Derived*>(B1)) {
//         std::cout << "A: cast valid\n";   // NEVER prints
//     } else {
//         std::cout << "A: cast INVALID - B1 is actually Base\n";  // prints
//     }
//     // Output: "A: cast INVALID - B1 is actually Base"

//     // ---- SCENARIO B: actual Derived object ----
//     Base* B2 = new Derived;

//     if (dynamic_cast<Derived*>(B2)) {
//         std::cout << "B: cast valid - B2 is actually Derived\n"; // prints!
//         dynamic_cast<Derived*>(B2)->Do(); // safe to use here
//     }
//     // Output: "B: cast valid - B2 is actually Derived"
//     //         "Derived::Do()"

//     // ---- Derived* D = new Derived (no cast needed) ----
//     Derived* D = new Derived;
//     D->Do();  // direct call, no cast needed at all
//     // Output: "Derived::Do()"

//     delete B1;
//     delete B2;
//     delete D;
//     return 0;
// }
// ```

// **The one-picture answer:**

// ```
// Base* B = new Base;          Base* B = new Derived;
//       ↓                             ↓
// [Base object]                 [Derived object]
//       ↓                             ↓
// dynamic_cast<Derived*>(B)    dynamic_cast<Derived*>(B)
//       ↓                             ↓
//    nullptr ❌                  valid ptr ✅
//    if block SKIPPED            if block ENTERED
//    (correctly safe!)           (correctly works!)
// ```

// **Interview answer in one sentence:**

// > Use `dynamic_cast<Derived*>(B)` — it checks the **actual runtime type** of the object being pointed to, returns `nullptr` if the cast is invalid, and a valid pointer if it succeeds. The declared type of `B` (`Base*`) doesn't matter — what matters is what `B` **actually points to**.