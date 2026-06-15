#include <iostream>
#include "Array.hpp"

Array::Array()
{
    std::cout << "constructor" << std::endl;
    for (int i = 0; i < 1; i++)
    {
        data.push_back(i);
    }
}

Array::~Array()
{
}

// copy constructor
// for case1. Array mynewArray= someexistingAraay;
// case 2. when first we do Array newArray;
// then ..newArray = someexistingArray
Array::Array(const Array &rhs)
{
    std::cout << "copy constructor" << std::endl;
    for (int i = 0; i < rhs.data.size(); i++)
    {
        data.push_back(rhs.data[i]);
    }
}

// copy assignment operator
// object is already constructed, we are just making a copy later (e.g. myArray2 = myArray)
Array &Array::operator=(const Array &rhs)
{
    std::cout << "copy assignment operator" << std::endl;
    if (&rhs == this) // handles myArray2 = myArray2 (self-assignment)
        return *this;

    data.clear();
    for (int i = 0; i < rhs.data.size(); i++)
    {
        data.push_back(rhs.data[i]);
    }
    return *this;
}

void Array::PrintingData()
{
    for (int i = 0; i < data.size(); i++)
    {
        std::cout << data[i] << std::endl;
    }
}

void Array::SetData(int index, int value)
{
    data[index] = value;
}