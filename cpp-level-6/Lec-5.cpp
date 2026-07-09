// constexpr is a keyword in C++ that was introduced in C++11 and has been enhanced in subsequent versions, including C++17. It is used to declare that a function or variable can be evaluated at compile time. This allows for optimizations and can lead to more efficient code.

#include <iostream>

constexpr float f = 3.14f;
constexpr int add(int a, int b)
{

    return a + b;
}
int main()
{
    constexpr int i = add(5, 18);
    return 0;
}