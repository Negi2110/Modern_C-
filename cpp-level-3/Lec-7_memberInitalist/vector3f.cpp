
#include "vector3f.hpp"
// Vector3f():y(z) , z(2.0f) , x(3.0f)//this will not work due to memory order is x y z

Vector3f::Vector3f() : x(1.0f), y(2.0f), z(3.0f) // how initialiser list affect the performanace
{
    // x = 0.0f;
    // y = 0.0f;
    // z = 0.0f;
}