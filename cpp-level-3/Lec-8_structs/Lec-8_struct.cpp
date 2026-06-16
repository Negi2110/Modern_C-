#include<iostream>
#include "Vector3f.hpp"

std::ostream &operator<<(std::ostream &os, const Vector3f &obj)
{
    os << obj.x << "," << obj.y << ","
       << obj.z << std::endl;
    return os;
}
//strcut is c contain by default all public
int main()
{
    Vector3f myVector1;
    myVector1.x=32;
    std::cout<<myVector1<<std::endl;

    return 0;
}
