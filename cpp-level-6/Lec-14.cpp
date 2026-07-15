// ============================================================
// NESTED CLASSES - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <vector>

// ============================================================
// SECTION 1: WHAT IS A NESTED CLASS?
// ============================================================

// Nested class = a class/struct defined INSIDE another class
// The inner class is SCOPED to the outer class
//
// WHY nested classes?
// -> LOGICAL GROUPING: Particle only makes sense in context of ParticleSystem
// -> ENCAPSULATION: hide implementation detail inside the class
// -> SCOPE CONTROL: user writes ParticleSystem::Particle (clear ownership)
// -> AVOIDS NAMESPACE POLLUTION: Particle name doesn't leak to global scope
//
// WITHOUT nesting:
// struct Particle { float x, y, z; };      // global scope, pollutes namespace
// class ParticleSystem { };                 // what's the relationship?
//
// WITH nesting:
// class ParticleSystem {
//     struct Particle { float x, y, z; };  // clearly BELONGS to ParticleSystem
// };
// ParticleSystem::Particle p;              // scope makes relationship obvious

// ============================================================
// SECTION 2: THE CODE EXPLAINED
// ============================================================

class ParticleSystem
{
public:
    // NESTED STRUCT - defined INSIDE ParticleSystem
    struct Particle
    // -> struct = public by default (vs class = private by default)
    // -> Particle is PUBLIC nested type
    //    -> users CAN access it: ParticleSystem::Particle p;
    //    -> if PRIVATE: only ParticleSystem itself could use it
    {
        float x{0.0f};
        float y{0.0f};
        float z{0.0f};
        float speed{1.0f};
        float lifetime{50.0f};
        // in-class member initializers (C++11)
        // default values if not specified

        void Move()
        {
            // moves particle based on speed, direction etc.
            // x += speed * direction.x etc.
        }
    };
    // Particle is accessed as: ParticleSystem::Particle
    // NOT just "Particle" from outside

    void Simulation()
    {
        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            m_particles[i].Move();
            // access Particle methods directly (inside ParticleSystem)
            // inside the outer class: can just say Particle (no ::)
        }
    }

    // COULD ALSO write with range-based for (cleaner):
    // for (auto& p : m_particles) { p.Move(); }

private:
    std::vector<Particle> m_particles;
    // inside ParticleSystem: just "Particle" (no ParticleSystem:: needed)
    // outside: would need ParticleSystem::Particle
};

// ============================================================
// SECTION 3: ACCESS RULES FOR NESTED CLASSES
// ============================================================

// NESTED CLASS accessing OUTER CLASS:
// nested class does NOT automatically have access to outer class members!
// (unlike Java/C# inner classes which have implicit outer reference)
//
// class Outer {
//     int m_data = 5;
//
//     class Inner {
//         void foo() {
//             m_data;  // ERROR! Inner cannot see Outer's members
//                      // unless passed explicitly
//         }
//     };
// };
//
// TO give Inner access to Outer:
// -> pass reference to Outer in Inner's constructor
// -> make Inner a friend of Outer
// -> use static members (no instance needed)

// OUTER CLASS accessing NESTED CLASS:
// outer CAN access all nested class members (even private ones)
// if nested class is declared in outer's private section!
//
// class Outer {
// private:
//     class Inner {
//     private:
//         int secret = 42;  // Outer can access this!
//     };
// };
// -> Outer treats Inner's privates as accessible
// -> but Inner still can't access Outer's privates without being friend

// ============================================================
// SECTION 4: PUBLIC vs PRIVATE NESTED CLASS
// ============================================================

// PUBLIC nested class (your code):
// class ParticleSystem {
// public:
//     struct Particle { ... };  // public nested type
// };
//
// ParticleSystem::Particle p;  // OK from OUTSIDE (user can use it)
// p.x = 5.0f;                  // OK (struct members are public)
//
// PRIVATE nested class:
// class ParticleSystem {
// private:
//     struct Particle { ... };  // private nested type
// };
//
// ParticleSystem::Particle p;  // ERROR from outside!
//                               // only ParticleSystem itself can use Particle
// -> FULL encapsulation: implementation detail hidden from users
// -> user doesn't need to know HOW particles are represented

// ============================================================
// SECTION 5: main() TRACE
// ============================================================

int main()
{
    ParticleSystem::Particle individualParticle;
    // ACCESS nested struct from OUTSIDE via ::
    // ParticleSystem:: = scope resolution = "look inside ParticleSystem"
    // individualParticle.x = 0.0f (default)
    // individualParticle.y = 0.0f
    // individualParticle.z = 0.0f
    // individualParticle.speed = 1.0f
    // individualParticle.lifetime = 50.0f
    // (all defaults from in-class initializers)

    ParticleSystem p;
    // creates ParticleSystem
    // m_particles is empty vector initially

    p.Simulation();
    // loops over m_particles (empty) -> does nothing
    // (would need to add particles first)

    return 0;
}

// ============================================================
// SECTION 6: BETTER EXTENDED VERSION - COMPLETE PARTICLE SYSTEM
// ============================================================

// class ParticleSystem {
// public:
//     struct Particle {
//         float x{0.0f}, y{0.0f}, z{0.0f};
//         float speedX{0.0f}, speedY{1.0f}, speedZ{0.0f};
//         float lifetime{50.0f};
//
//         void Move() {
//             x += speedX;
//             y += speedY;
//             z += speedZ;
//             lifetime -= 1.0f;
//         }
//
//         bool IsAlive() const { return lifetime > 0.0f; }
//     };
//
//     void AddParticle(float x, float y, float z) {
//         m_particles.push_back({x, y, z});
//         // aggregate initialization: x, y, z set, rest use defaults
//     }
//
//     void Simulation() {
//         // modern range-based for:
//         for (auto& particle : m_particles) {
//             particle.Move();
//         }
//         // remove dead particles (erase-remove idiom):
//         m_particles.erase(
//             std::remove_if(m_particles.begin(), m_particles.end(),
//                 [](const Particle& p) { return !p.IsAlive(); }),
//             m_particles.end()
//         );
//     }
//
//     size_t GetCount() const { return m_particles.size(); }
//
// private:
//     std::vector<Particle> m_particles;
// };
//
// int main() {
//     ParticleSystem ps;
//     ps.AddParticle(0.0f, 0.0f, 0.0f);
//     ps.AddParticle(1.0f, 2.0f, 3.0f);
//
//     // create individual particle (public nested type)
//     ParticleSystem::Particle p;
//     p.x = 5.0f;
//     p.Move();
//
//     for (int i = 0; i < 60; i++) {  // 60 frames
//         ps.Simulation();
//     }
//     std::cout << ps.GetCount() << " particles remaining\n";
// }

// ============================================================
// SECTION 7: REAL WORLD NESTED CLASS EXAMPLES
// ============================================================

// std::vector::iterator -> nested inside std::vector
// std::map::value_type  -> nested inside std::map
// std::list::node       -> (private) nested inside std::list
//
// Qt examples:
// QAbstractItemModel::index() returns QModelIndex
// QTreeWidget has QTreeWidgetItem (separate but conceptually nested)
//
// YOUR ISSST context:
// class DISProtocol {
//     struct PDUHeader { uint8_t version; uint8_t pduType; ... };
//     struct EntityStatePDU { PDUHeader header; EntityID id; ... };
// };
// -> PDUHeader and EntityStatePDU are clearly PART OF DISProtocol
// -> DISProtocol::PDUHeader makes ownership crystal clear

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a nested class?
//     -> A class or struct defined inside another class.
//        Scoped to the outer class (accessed via OuterClass::InnerClass).
//        Used for logical grouping, encapsulation, and avoiding
//        namespace pollution.

// Q2: How do you access a nested class from outside?
//     -> Use scope resolution operator: OuterClass::InnerClass.
//        ParticleSystem::Particle p; (your code does this).
//        Only works if the nested class is in a public section.

// Q3: Can a nested class access the outer class's private members?
//     -> NO (unlike Java). In C++, the nested class has no implicit
//        access to the outer class's members. Must be passed explicitly
//        or the nested class declared as friend of outer.

// Q4: Can the outer class access private members of the nested class?
//     -> YES. The outer class is considered a "privileged" context
//        and can access all members of its nested classes.

// Q5: What is the difference between public and private nested class?
//     -> Public: external users can create instances (ParticleSystem::Particle p)
//        Private: only the outer class can use it (implementation detail hidden)

// Q6: Why use nested class instead of a separate class?
//     -> Logical grouping (Particle belongs to ParticleSystem).
//        Avoids polluting the global/namespace scope.
//        Makes ownership and relationship obvious in code.
//        Can be made private for full encapsulation.

// Q7: Inside the outer class, do you need :: to access nested class?
//     -> NO. Inside ParticleSystem, you can just write Particle
//        (not ParticleSystem::Particle). The :: is only needed
//        from OUTSIDE the outer class scope.

// Q8: What is a real-world example of nested classes in STL?
//     -> std::vector<int>::iterator (nested type inside vector).
//        std::map<K,V>::value_type, std::map::iterator.
//        All STL container iterators are nested types of their containers.
// ```

// **Nested class access summary:**

// ```cpp
// class ParticleSystem {
// public:
//     struct Particle { };          // public: anyone can use

// private:
//     struct InternalNode { };      // private: only ParticleSystem can use
//     std::vector<Particle> m_particles; // inside: just "Particle" (no ::)
// };

// // Outside:
// ParticleSystem::Particle p;      // OK (public nested type)
// ParticleSystem::InternalNode n;  // ERROR (private nested type)
// ```

// **The scope resolution rule:**

// ```
// INSIDE outer class:     just write  Particle        (no :: needed)
// OUTSIDE outer class:    must write  ParticleSystem::Particle
// ```

// > **The design principle**: use nested classes when the inner type is so tightly coupled to the outer class that it wouldn't make sense without it. `Particle` without `ParticleSystem` is a bit meaningless — nesting makes the relationship explicit in the code itself. In your ISSST/DIS work, `PDUHeader` inside `DISProtocol` is the classic example of this pattern.