// Exactly this case — **free function has no `this`, so `const` at end is meaningless:**

// ```cpp
// // YOUR CODE - ERROR:
// bool operator==(const Vector3f& lhs, const Vector3f& rhs) const
// //                                                         ^
// //                                                    ERROR!
// //                                         "non-member function cannot have const"
// //                                         there is no THIS to protect!

// // FIX - just remove the trailing const:
// bool operator==(const Vector3f& lhs, const Vector3f& rhs)
// {
//     return lhs.x == rhs.x &&
//            lhs.y == rhs.y &&
//            lhs.z == rhs.z;
// }
// ```

// **Why it works differently:**

// ```cpp
// // MEMBER function (inside class):
// bool operator==(const Vector3f& rhs) const
// //                                   ^
// //               protects THIS (the left side object)
// //               myVector.operator==(myVector2)
// //                  ^this is protected by const

// // FREE function (outside class):
// bool operator==(const Vector3f& lhs, const Vector3f& rhs)
// //                              ^                    ^
// //              BOTH sides already protected by const&
// //              no THIS exists -> trailing const = nonsense -> ERROR
// ```

// **One line:**

// > Trailing `const` = protect `this`. Free function has **no** `this`. So trailing `const` is **not allowed.**

#include <iostream>

// a vector3 data type ,storing floats
class Vector3f
{
public:
    Vector3f()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }
    Vector3f operator+(const Vector3f &rhs) const
    {
        Vector3f result;
        result.x = x + rhs.x;
        result.y = y + rhs.y;
        result.z = z + rhs.z;
        return result;
    }
    Vector3f operator++()
    {
        x = x + 1;
        y = y + 1;
        z = z + 1;
        return *this;
    }
   bool operator==(const Vector3f &rhs)
    {  
        std::cout<<"Part of class"<<std::endl;
        if (x == rhs.x &&
            y == rhs.y &&
            z == rhs.z){
                return true;
            }
        return false;
    }
    
    float x, y, z;
};
std::ostream &operator<<(std::ostream &os, const Vector3f &obj)
{
    os << obj.x << "," << obj.y << ","
       << obj.z << std::endl;
    return os;
}
bool operator==(const Vector3f &lhs, const Vector3f &rhs)//gerally free is safe to do
    {
        std::cout<<"Free Functions"<<std::endl;
        //for floatinf saf e u can do is if(abs(lhs.x-rhs.x)<0.01 && similar for y z)
        if (lhs.x == rhs.x &&
            lhs.y == rhs.y &&
            lhs.z == rhs.z){
                return true;
            }
        return false;
    }
int main()
{
    Vector3f myVector;

    myVector.x = 1.0f;
    myVector.y = 1.0f;
    myVector.z = 1.0f;

    Vector3f myVector2;

    myVector2.x = 1.0f;
    myVector2.y = 1.0f;
    myVector2.z = 1.0f;
//for floats this qual overidng may not be work correctly
   if(myVector == myVector2){
    std::cout<<"vector are equal"<<std::endl;
   }else{
    std::cout<<"vector are not equal"<<std::endl;
   }
    // Vector3f result = myVector+myVector2;
    myVector = myVector + myVector2;
    ++myVector;
    ++myVector;
    ++myVector;
    ++myVector;
    std::cout << myVector;
    return 0;
}



// ```cpp
// // ============================================================
// // operator<<, operator==, operator++, FREE vs MEMBER - INTERVIEW NOTES
// // ============================================================

// #include <iostream>

// class Vector3f
// {
// public:
//     Vector3f()
//     {
//         x = 0.0f;
//         y = 0.0f;
//         z = 0.0f;
//     }

//     // ============================================================
//     // SECTION 1: operator+ (already know this)
//     // ============================================================

//     Vector3f operator+(const Vector3f& rhs) const
//     {
//         Vector3f result;
//         result.x = x + rhs.x;
//         result.y = y + rhs.y;
//         result.z = z + rhs.z;
//         return result;
//     }

//     // ============================================================
//     // SECTION 2: operator++ (PREFIX version)
//     // ============================================================

//     Vector3f operator++()
//     // -> prefix ++  (++myVector)
//     // -> NO parameters = prefix version
//     // -> modifies THIS object directly
//     // -> returns modified object
//     {
//         x = x + 1;
//         y = y + 1;
//         z = z + 1;
//         return *this;  // return THIS object (already modified)
//     }
//     // ++myVector translates to:
//     // myVector.operator++()
//     //
//     // PREFIX vs POSTFIX difference:
//     // operator++()      -> PREFIX  (++x) -> modify first, return modified
//     // operator++(int)   -> POSTFIX (x++) -> return original, then modify
//     //                      'int' is a DUMMY parameter (just to differentiate)
//     //
//     // POSTFIX would look like:
//     // Vector3f operator++(int)  // int = dummy, means postfix
//     // {
//     //     Vector3f original = *this;  // save original
//     //     x++; y++; z++;              // modify
//     //     return original;            // return ORIGINAL (before modify)
//     // }

//     // ============================================================
//     // SECTION 3: operator== MEMBER version
//     // ============================================================

//     bool operator==(const Vector3f& rhs)
//     // -> member function
//     // -> this = left side (myVector)
//     // -> rhs  = right side (myVector2)
//     // -> missing trailing const (should have it - see below)
//     {
//         std::cout << "Part of class" << std::endl;
//         if (x == rhs.x &&
//             y == rhs.y &&
//             z == rhs.z){
//             return true;
//         }
//         return false;
//     }
//     // SHOULD BE:
//     // bool operator==(const Vector3f& rhs) const
//     //                                      ^
//     //                   comparison should NEVER modify the object
//     //                   without const -> non-const objects only
//     //                   with const    -> works on const objects too

//     float x, y, z;
// };

// // ============================================================
// // SECTION 4: operator<< FREE FUNCTION - MUST be free!
// // ============================================================

// std::ostream& operator<<(std::ostream& os, const Vector3f& obj)
// // ^              ^           ^                    ^
// // return         return    left side            right side
// // ostream&       type      (cout/cerr/file)     (your object)
// //
// // WHY must be FREE function (cannot be member)?
// // -> left side of << is std::ostream (cout)
// // -> member function: obj.operator<<(cout)
// //    means << would be called ON Vector3f
// //    but cout is on the LEFT: cout << myVector
// //    so Vector3f would need to be on LEFT side
// //    which means: myVector << cout -> WRONG way around!
// // -> free function: operator<<(cout, myVector)
// //    both sides explicit -> correct order!
// //
// // WHY return std::ostream&?
// // -> enables CHAINING:
// //    std::cout << myVector << myVector2 << std::endl;
// //    evaluates left to right:
// //    operator<<(cout, myVector)  -> returns cout
// //    operator<<(cout, myVector2) -> returns cout
// //    operator<<(cout, endl)      -> returns cout
// //    WITHOUT returning ostream&: chaining BREAKS
// {
//     os << obj.x << "," << obj.y << "," << obj.z << std::endl;
//     return os;  // return the stream for chaining
// }

// // ============================================================
// // SECTION 5: operator== FREE FUNCTION
// // ============================================================

// bool operator==(const Vector3f& lhs, const Vector3f& rhs)
// // -> free function
// // -> lhs = left side (myVector)
// // -> rhs = right side (myVector2)
// // -> NO trailing const (free function - has no this!)
// // -> both params const& (read only, no copy)
// {
//     std::cout << "Free Function" << std::endl;
//     if (lhs.x == rhs.x &&
//         lhs.y == rhs.y &&
//         lhs.z == rhs.z){
//         return true;
//     }
//     return false;
// }

// // ============================================================
// // SECTION 6: MEMBER == vs FREE == - WHICH ONE GETS CALLED?
// // ============================================================

// // YOU HAVE BOTH:
// // member: bool operator==(const Vector3f& rhs)
// // free:   bool operator==(const Vector3f& lhs, const Vector3f& rhs)
// //
// // myVector == myVector2
// //
// // COMPILER PREFERS MEMBER FUNCTION when available!
// // -> prints "Part of class" (member called first)
// // -> free function ignored when member exists
// //
// // WHEN does free function get called?
// // -> when LEFT side is NOT a Vector3f:
// //    someOtherType == myVector -> member cannot be called
// //    -> free function called instead
// //
// // YOUR COMMENT: "generally free is safe to do"
// // WHY free is preferred by many developers:
// // -> symmetric: both sides treated equally (both const&)
// // -> works even when left side is not your type
// // -> cleaner design for comparison operators

// // ============================================================
// // SECTION 7: FLOAT COMPARISON WARNING - YOUR COMMENT
// // ============================================================

// // YOUR COMMENT: "for floats this equal overriding may not work correctly"
// // 100% CORRECT!
// //
// // float a = 0.1f + 0.2f;   // = 0.30000001192... (not exactly 0.3!)
// // float b = 0.3f;           // = 0.30000001192... (also not exactly 0.3!)
// // a == b  -> may be false even though mathematically equal!
// //
// // WHY? floating point numbers are stored in binary
// // 0.1 cannot be represented EXACTLY in binary (like 1/3 in decimal)
// // tiny rounding errors accumulate
// //
// // FIX - use epsilon comparison:
// // bool operator==(const Vector3f& rhs) const
// // {
// //     float epsilon = 0.0001f;   // "close enough" threshold
// //     return (std::abs(x - rhs.x) < epsilon &&
// //             std::abs(y - rhs.y) < epsilon &&
// //             std::abs(z - rhs.z) < epsilon);
// // }
// // -> if difference is smaller than epsilon -> "equal enough"
// // -> common in game dev, physics, graphics code

// // ============================================================
// // SECTION 8: main() TRACE
// // ============================================================

// int main()
// {
//     Vector3f myVector;
//     myVector.x = 1.0f;
//     myVector.y = 1.0f;
//     myVector.z = 1.0f;
//     // myVector = {1, 1, 1}

//     Vector3f myVector2;
//     myVector2.x = 1.0f;
//     myVector2.y = 1.0f;
//     myVector2.z = 1.0f;
//     // myVector2 = {1, 1, 1}

//     if (myVector == myVector2){
//         // MEMBER operator== called (preferred over free)
//         // prints: "Part of class"
//         // x==x, y==y, z==z -> true
//         std::cout << "vector are equal" << std::endl;
//         // prints: "vector are equal"
//     }

//     myVector = myVector + myVector2;
//     // operator+ called: {1,1,1} + {1,1,1} = {2,2,2}
//     // operator= called: myVector = {2,2,2}
//     // myVector = {2, 2, 2}

//     ++myVector;  // operator++() -> {3,3,3}
//     ++myVector;  // operator++() -> {4,4,4}
//     ++myVector;  // operator++() -> {5,5,5}
//     ++myVector;  // operator++() -> {6,6,6}

//     std::cout << myVector;
//     // operator<< called: os << 6.0 << "," << 6.0 << "," << 6.0
//     // prints: 6,6,6

//     return 0;
// }

// // FULL OUTPUT:
// // Part of class
// // vector are equal
// // 6,6,6
// ```

// **All operators at a glance:**

// ```
// OPERATOR    MEMBER or FREE?    WHY
// ────────────────────────────────────────────────────────
// operator+   member             left side is always your type
// operator++  member             modifies THIS directly
// operator==  both work          free preferred (symmetric)
// operator<<  MUST be free       left side is ostream (not your type)
// ```

// **The `<<` rule - always trips people up in interviews:**

// > `cout << myVector` means left side is `cout` (ostream).
// > Member function would need **your object on the left**.
// > So `<<` **must** be a free function — always.