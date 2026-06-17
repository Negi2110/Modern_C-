#include <iostream>
#include <string>
#include "IntArray.hpp"

IntArray::IntArray(std::string name) : m_name(name), m_data(new int[10])
{
    std::cout << m_name << "was constructed" << std::endl;
}
IntArray::~IntArray()
{
    std::cout << m_name << "was distructed" << std::endl;
    delete[] m_data;
}

IntArray::IntArray(const IntArray &rhs)
{
    m_name = rhs.m_name + "(copy)";
    std::cout << "was copy constructed from: " << rhs.m_name << std::endl;
    m_data = new int[10];
    for (int i = 0; i < 10; i++)
    {
        m_data[i] = rhs.m_data[i];
    }
}
IntArray &IntArray::operator=(const IntArray &rhs)
{
    std::cout << "was copy assigned from" << rhs.m_name << std::endl;
    if (this != &rhs)
    {
        delete[] m_data;
        m_name = rhs.m_name + "(copy)";
        m_data = new int[10];
        for (int i = 0; i < 10; i++)
        {
            m_data[i] = rhs.m_data[i];
        }
    }
    return *this;
}
// move construtor
IntArray::IntArray(IntArray &&source)//why double &&..is it for move std::move?
{
    m_name = source.m_name;
    source.m_name = "";
    m_data = source.m_data;
    source.m_data = nullptr;
    std::cout << m_name << "was move constructed" << std::endl;
}

// move assigment operator
IntArray &IntArray::operator=(IntArray &&source)
{
    if (this != &source)
    {
        m_name = source.m_name;
        source.m_name = "";
        m_data = source.m_data;
        source.m_data = nullptr;
        std::cout << m_name << "used move assignment" << std::endl;
    }
    return *this;
}