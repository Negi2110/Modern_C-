#pragma once
#include <vector>
//the rule of 3
class Array
{
public:
    // special memeber functions
    // 1.Constructor
    Array();
    // 2/Destructor
    ~Array();
    // 3.Copy Constructor
    Array(const Array &rhs)=delete;//why do this deletye
    // 4or 4 copy assignment operator
    Array &operator=(const Array &rhs);
    void PrintingData();
    void SetData(int index, int value);

private:
    std::vector<int> data;
};