// inline variables (cpp17 and beyond)
// inline specifier when used in function,
// declare function to be an inline function.A function defined entirely inside a class/struct/union defination,whether its a member funcion or a non-member friend function,is implicitely an inline function unless it is attached to a named function
//after inline static u dont have to complete extra header
#include <iostream>
#include "struct.hpp"
int main()
{
   
    Struct s;
    //Struct::SomeValue=89;
    std::cout << Struct::SomeValue << '\n';
    return 0;
}