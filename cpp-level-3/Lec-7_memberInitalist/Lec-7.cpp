#include<iostream>
#include "vector3f.hpp"

std::ostream &operator<<(std::ostream &os, const Vector3f &obj)
{
    os << obj.x << "," << obj.y << ","
       << obj.z << std::endl;
    return os;
}

int main()
{
    Vector3f myVector1;
    std::cout<<myVector1<<std::endl;

    return 0;
}
