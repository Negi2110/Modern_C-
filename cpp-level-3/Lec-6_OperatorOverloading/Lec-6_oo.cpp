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
    float x, y, z;
};

int main()
{
    Vector3f myVector;

    myVector.x = 1.0f;
    myVector.y = 2.0f;
    myVector.z = 3.0f;

    Vector3f myVector2;

    myVector2.x = 3.0f;
    myVector2.y = 2.0f;
    myVector2.z = 1.0f;

    // Vector3f result = myVector+myVector2;
    myVector = myVector + myVector2;
    // std::cout<<result.x<<","<<result.y<<","<<result.z<<std::endl;
    std::cout << myVector.x << "," << myVector.y << "," << myVector.z << std::endl;

    return 0;
}