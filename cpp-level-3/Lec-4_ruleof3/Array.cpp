#include <iostream>
#include "Array.hpp"

Array::Array()
{
    std::cout << "constructor" << std::endl;
    data = new int[10];
    for (int i = 0; i < 10; i++)
    {
        data[i] = i * i;
    }
}

Array::~Array()
{
    delete[] data;
}

// copy constructor
//for case1. Array mynewArray= someexistingAraay;
//case 2. when first we do Array newArray;
//then ..newArray = someexistingArray
Array::Array(const Array &rhs)
{
    std::cout << "copy constructor" << std::endl;
    data = new int[10];
    for (int i = 0; i < 10; i++)
    {
        data[i] = rhs.data[i];
    }
}

// copy assignment operator
// object is already constructed, we are just making a copy later (e.g. myArray2 = myArray)
Array& Array::operator=(const Array &rhs)
{
    std::cout << "copy assignment operator" << std::endl;
    if (&rhs == this)  // handles myArray2 = myArray2 (self-assignment)
        return *this;

    delete[] data;     // safe: data is always valid after constructor
    data = new int[10];
    for (int i = 0; i < 10; i++)
    {
        data[i] = rhs.data[i];
    }
    return *this;
}

void Array::PrintingData()
{
    for (int i = 0; i < 10; i++)
    {
        std::cout << data[i] << std::endl;
    }
}

void Array::SetData(int index, int value)
{
    data[index] = value;
}