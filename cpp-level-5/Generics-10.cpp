//effective modern c++

#include <iostream>
#include <vector>

template <typename first, typename second>
struct Entry
{
    Entry(first key, second value):m_key(key) ,m_value(value)
    {
       
    }
    first m_key;
    second m_value;
};

int main()
{
    // class template argument deduction (CTAD)
    Entry entry(5, 100);
    std::cout << entry.m_key << std::endl;
    std::cout << entry.m_value << std::endl;

    std::vector v {1.0f,2.0f,3.0f,4.0f,5.0f};
    return 0;
}