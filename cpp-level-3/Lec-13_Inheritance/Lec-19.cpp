// ============================================================
// PURE VIRTUAL FUNCTIONS, ABSTRACT CLASSES, INTERFACES - NOTES
// ============================================================

#include <iostream>

// ============================================================
// SECTION 1: WHAT IS A PURE VIRTUAL FUNCTION?
// ============================================================

// virtual void Draw() = 0;
//                    ^^^
//                "= 0" makes this PURE VIRTUAL
//
// MEANING: "this class declares that Draw() MUST EXIST,
//           but provides NO IMPLEMENTATION itself"
//          "every derived class is FORCED to implement it"
//
// REGULAR virtual function:
// virtual void Draw() { /* default behavior */ }
// -> has a body, derived classes CAN override (optional)
//
// PURE virtual function:
// virtual void Draw() = 0;
// -> NO body, derived classes MUST override (mandatory)
// -> if a derived class doesn't override it -> derived class
//    ALSO becomes abstract (cannot be instantiated either!)

// ============================================================
// SECTION 2: ABSTRACT CLASS - YOUR COMMENT IS CORRECT
// ============================================================

// YOUR COMMENT: "pure virtual function class can't be instantiated"
// 100% correct!
//
// class IRenderer {
//     virtual void Draw() = 0;     // pure virtual
//     virtual void Update() = 0;   // pure virtual
// };
// -> IRenderer has AT LEAST ONE pure virtual function
// -> IRenderer is now an ABSTRACT CLASS
//
// IRenderer renderer;          // COMPILER ERROR!
//                              // "cannot instantiate abstract class"
// IRenderer* p = new IRenderer; // COMPILER ERROR! same reason
//
// WHY does this make sense?
// -> IRenderer doesn't KNOW how to Draw() (no implementation)
// -> creating an IRenderer object would mean: "call Draw()" -> but WHAT runs??
// -> there's nothing to call -> compiler REFUSES to let you create one
// -> you can ONLY create objects of CONCRETE (fully implemented) derived classes
//
// IRenderer* p = new OpenGL;   // VALID! OpenGL implements both pure virtuals
//                              // OpenGL is a CONCRETE class (not abstract)

// ============================================================
// SECTION 3: INTERFACE PATTERN (I prefix)
// ============================================================

// class IRenderer {
// ^
// 'I' prefix = common NAMING CONVENTION for INTERFACES
// (not a C++ language feature, just a convention everyone follows)
//
// AN "INTERFACE" in C++ = a class with:
// -> ONLY pure virtual functions (no data, no implementation)
// -> defines a CONTRACT: "anyone implementing me must provide these"
// -> often has a virtual destructor too (good practice)
//
// class IRenderer {
// public:
//     virtual ~IRenderer() = default;  // SHOULD have this! (missing in your code)
//     virtual void Draw() = 0;
//     virtual void Update() = 0;
// };
//
// THIS PATTERN models "CAN-DO" relationships:
// OpenGL  "CAN-DO" rendering (implements IRenderer)
// Vulkan  "CAN-DO" rendering (implements IRenderer)
// -> different from IS-A (inheritance for shared implementation)
// -> interface inheritance = shared CONTRACT, not shared CODE

class IRenderer
{
public:
    virtual void Draw() = 0;   // PURE virtual - MUST be overridden
    virtual void Update() = 0; // PURE virtual - MUST be overridden
    // virtual void drawOptimised() = 0;  // commented out - see Section 5
};

// ============================================================
// SECTION 4: CONCRETE (DERIVED) CLASSES - MUST IMPLEMENT ALL
// ============================================================

class OpenGL : public IRenderer
{
public:
    void Draw() override // REQUIRED - Draw() was pure virtual
    {
        std::cout << "OpenGL: draw" << std::endl;
    }
    void Update() override // REQUIRED - Update() was pure virtual
    {
        std::cout << "OpenGL: Update" << std::endl;
    }
    void drawOptimised() {} // EXTRA function - NOT part of IRenderer's contract
                            // OpenGL-SPECIFIC, not inherited from interface
};
// OpenGL implements BOTH pure virtuals -> OpenGL is CONCRETE (instantiable!)

class Vulkan : public IRenderer
{
public:
    void Draw() override
    {
        std::cout << "Vulkan: draw" << std::endl;
    }
    void Update() override
    {
        std::cout << "Vulkan: Update" << std::endl;
    }
    void drawOptimised() {} // ALSO Vulkan-SPECIFIC, separate from OpenGL's
};
// Vulkan ALSO implements both -> Vulkan is also CONCRETE

// WHAT IF A DERIVED CLASS FORGETS TO IMPLEMENT ONE?
// class BrokenRenderer : public IRenderer {
//     void Draw() override { }
//     // Update() NOT implemented!
// };
// BrokenRenderer b;   // COMPILER ERROR!
//                     // "cannot instantiate abstract class BrokenRenderer"
//                     // because it STILL has Update() as pure virtual
//                     // (inherited, unimplemented)
// THIS IS A SAFETY FEATURE: compiler FORCES you to implement
// the entire contract, or your class stays abstract too

// ============================================================
// SECTION 5: YOUR MAIN QUESTION - drawOptimised() PROBLEM
// ============================================================

// drawOptimised() exists in OpenGL AND Vulkan
// but is NOT declared in IRenderer (the interface)
//
// IRenderer* myRenderer = new OpenGL;
// myRenderer->drawOptimised();   // COMPILER ERROR!
//
// WHY?
// -> myRenderer's DECLARED TYPE is IRenderer*
// -> compiler checks: "does IRenderer have a drawOptimised() method?"
// -> NO! IRenderer's interface only has Draw() and Update()
// -> compiler REFUSES the call, even though the ACTUAL object
//    (OpenGL) DOES have drawOptimised()
//
// THIS IS A FUNDAMENTAL C++ RULE:
// "The compiler only allows you to call functions that exist
//  in the DECLARED TYPE of the pointer/reference,
//  NOT the actual runtime type"
//
// virtual functions get RUNTIME dispatch (which version runs)
// but the AVAILABLE function SET is still determined by
// the COMPILE TIME (declared) type

// ============================================================
// SECTION 6: TWO WAYS TO FIX - YOUR COMMENT COVERS BOTH
// ============================================================

// YOUR COMMENT: "you have to create ptr of same type rather
//                than base class OR we can create virtual fun
//                in base"
//
// FIX OPTION 1: use the DERIVED type pointer directly
// OpenGL* myGLRenderer = new OpenGL;
// myGLRenderer->drawOptimised();   // WORKS! OpenGL* knows about drawOptimised()
// -> downside: lose the polymorphism benefit (can't swap with Vulkan easily)
//
// FIX OPTION 2: add it to the interface (your commented-out line)
// class IRenderer {
//     virtual void Draw() = 0;
//     virtual void Update() = 0;
//     virtual void drawOptimised() = 0;   // ADD to the contract
// };
// -> NOW: IRenderer* myRenderer = new OpenGL;
//         myRenderer->drawOptimised();   // WORKS! part of interface now
// -> downside: EVERY derived class (Vulkan too) MUST implement it
//              even if it doesn't make sense for some renderers
//
// FIX OPTION 3 (not in your code, but worth knowing): dynamic_cast
// if (OpenGL* gl = dynamic_cast<OpenGL*>(myRenderer)) {
//     gl->drawOptimised();   // only runs if myRenderer IS actually OpenGL
// }
// -> safely "downcast" to check the actual type at runtime
// -> returns nullptr if myRenderer is NOT actually an OpenGL object

// ============================================================
// SECTION 7: PURE VIRTUAL WITH A BODY (UNCOMMON BUT VALID)
// ============================================================

// YOUR COMMENT: "when this was not equal to zero we can
//                implement it here as well as override it...
//                but after 0 it must be overridden in derived class"
//
// You CAN actually give a pure virtual function a BODY:
// class IRenderer {
//     virtual void Update() = 0;   // still PURE (still abstract)
// };
// void IRenderer::Update() {       // body defined OUTSIDE the class
//     std::cout << "default update logic";
// }
//
// class OpenGL : public IRenderer {
//     void Update() override {
//         IRenderer::Update();      // can call the base's "default" version!
//         std::cout << "OpenGL extra update logic";
//     }
// };
// -> still MUST override (it's still pure virtual, = 0)
// -> still CANNOT instantiate IRenderer directly
// -> but derived classes have the OPTION to call the base
//    implementation as a "fallback" or "shared logic" helper
// -> rare pattern, but valid and sometimes used for shared boilerplate

// ============================================================
// SECTION 8: EXECUTION TRACE
// ============================================================

int main()
{
    IRenderer *myRenderer = new OpenGL;
    // OpenGL constructor runs (default, nothing printed)
    // vptr set to OpenGL::vtable

    myRenderer->Draw();
    // virtual dispatch -> OpenGL::Draw() called
    // prints: "OpenGL: draw"

    myRenderer->Update();
    // virtual dispatch -> OpenGL::Update() called
    // prints: "OpenGL: Update"

    // myRenderer->drawOptimised();
    // COMPILER ERROR - not part of IRenderer's interface
    delete myRenderer;
    return 0;
    // NOTE: 'delete myRenderer;' is MISSING here - memory leak!
    // ALSO: IRenderer has NO virtual destructor - another bug!
    // (see Section 9)
}

// ============================================================
// SECTION 9: BUGS IN YOUR CODE TO FLAG
// ============================================================

// BUG 1: missing virtual destructor in IRenderer
// class IRenderer {
//     virtual ~IRenderer() = default;  // SHOULD be here!
//     virtual void Draw() = 0;
//     virtual void Update() = 0;
// };
// without it: delete myRenderer would only call ~IRenderer()
//             (if it existed) and skip ~OpenGL() - same leak bug
//             as covered in the previous session

// BUG 2: missing delete in main()
// IRenderer* myRenderer = new OpenGL;
// ... use it ...
// delete myRenderer;   // MISSING! -> memory leak (OpenGL object never freed)

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a pure virtual function?
//     -> A virtual function declared with "= 0" instead of a body.
//        Forces derived classes to provide an implementation.
//        Makes the containing class abstract (cannot be instantiated).

// Q2: What is an abstract class?
//     -> A class containing at least one pure virtual function.
//        Cannot be instantiated directly. Can still have pointers/
//        references to it (pointing to concrete derived objects).

// Q3: Can a pure virtual function have a body?
//     -> YES (uncommon but valid). Still must be overridden by
//        derived classes (still abstract), but provides a default
//        implementation derived classes can optionally call.

// Q4: Why can't you call myRenderer->drawOptimised() through
//     an IRenderer* pointer even if the object is OpenGL?
//     -> Compiler checks function availability based on the
//        DECLARED (static) type of the pointer (IRenderer*),
//        not the actual runtime object type. drawOptimised()
//        isn't part of IRenderer's interface.

// Q5: What's the difference between IS-A and interface (CAN-DO)
//     inheritance?
//     -> IS-A: derived class shares implementation with base
//              (e.g., Golden IS-A Dog, inherits Bark/Walk code)
//        Interface: derived class shares only a CONTRACT,
//              provides its OWN implementation for everything
//              (e.g., OpenGL CAN-DO rendering per IRenderer's contract)

// Q6: What happens if a derived class doesn't implement all
//     pure virtual functions from its base?
//     -> The derived class REMAINS abstract too. Cannot be
//        instantiated until ALL pure virtuals are implemented
//        somewhere in the chain.

// Q7: Why is the 'I' prefix used (IRenderer)?
//     -> Common naming CONVENTION (not a language feature) to
//        signal "this is an interface" - a contract with no
//        implementation, typically all pure virtual functions.

// Q8: What's missing from this IRenderer that should be there?
//     -> A virtual destructor. Since IRenderer is used
//        polymorphically (IRenderer* pointing to OpenGL/Vulkan),
//        deleting through the base pointer without a virtual
//        destructor would skip the derived class's cleanup.
// ```

// **The core rule that explains your `drawOptimised()` question:**

// ```
// Compiler decides WHICH FUNCTIONS YOU CAN CALL
//     → based on the DECLARED type of the pointer (IRenderer*)

// vtable decides WHICH VERSION ACTUALLY RUNS
//     → based on the ACTUAL object type (OpenGL)

// drawOptimised() fails because step 1 already rejects it -
// it never even gets to the vtable lookup stage.
// ```

// **Two fixes side by side:**

// ```cpp
// // FIX A - lose polymorphism, gain access:
// OpenGL* gl = new OpenGL;
// gl->drawOptimised();          // works, but can't swap to Vulkan easily

// // FIX B - keep polymorphism, expand the contract:
// class IRenderer {
//     virtual void drawOptimised() = 0;   // now ALL renderers must have it
// };
// ```

// > **Don't forget**: your `IRenderer` is missing a `virtual ~IRenderer()`, and `main()` never calls `delete myRenderer;` — both are the exact memory-leak bugs from your previous session, just reappearing in interface form.

// **Yes — abstract classes CAN and SHOULD have both.**

// ```cpp
// class IRenderer
// {
// public:
//     IRenderer()                       // constructor - VALID
//     {
//         std::cout << "IRenderer constructed\n";
//     }

//     virtual ~IRenderer()               // destructor - SHOULD have this!
//     {
//         std::cout << "IRenderer destructed\n";
//     }

//     virtual void Draw() = 0;           // pure virtual - makes class abstract
// };
// ```

// **Why this seems confusing at first - resolve the contradiction:**

// ```
// "Abstract class CANNOT be instantiated"
//         vs
// "Abstract class HAS a constructor that runs"

// These are NOT contradictory:
// You cannot create a STANDALONE IRenderer object.
// But you CAN create a DERIVED (concrete) object,
// which ALWAYS calls IRenderer's constructor first.
// ```

// **Proof it runs:**

// ```cpp
// class OpenGL : public IRenderer
// {
// public:
//     void Draw() override { std::cout << "OpenGL draw\n"; }
// };

// int main()
// {
//     IRenderer* p = new OpenGL();
//     // IRenderer() constructor runs FIRST (even though abstract!)
//     // OpenGL() constructor runs SECOND

//     delete p;
//     // ~OpenGL() runs FIRST
//     // ~IRenderer() runs SECOND (because virtual)
// }
// ```

// **Output:**
// ```
// IRenderer constructed
// OpenGL constructed
// OpenGL destructed
// IRenderer destructed
// ```

// **Why constructor makes sense even though you can't instantiate directly:**

// ```cpp
// class IRenderer
// {
// public:
//     IRenderer(std::string name) : m_name(name) { }
//     // abstract classes CAN have member data + setup logic!
//     virtual void Draw() = 0;

// protected:
//     std::string m_name;   // shared by ALL renderers
// };

// class OpenGL : public IRenderer
// {
// public:
//     OpenGL() : IRenderer("OpenGL") { }   // must call base ctor, like any inheritance
//     void Draw() override { }
// };
// ```

// > Abstract just means: *"you cannot make an object of EXACTLY this type."* It does NOT mean: *"this class has no setup/cleanup logic to contribute."* Every concrete deri`ved class still relies on the abstract base's constructor and destructor running as part of its own construction/destruction chain — **exactly like any other inheritance you've already learned.**

// **Why the destructor MUST be virtual here — the rule you already know:**

// > Since `IRenderer*` will point to derived objects (`OpenGL`, `Vulkan`) and get `delete`d through the base pointer, `~IRenderer()` **must** be `virtual` — otherwise you get the exact same leak bug from two sessions ago.