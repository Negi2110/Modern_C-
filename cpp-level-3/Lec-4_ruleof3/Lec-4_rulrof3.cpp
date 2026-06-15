#include <iostream>
#include "Array.hpp"
//the rule of three
//shallow copy-if u pnly have trivial data type like int and float also like vector
//but whenn there is pointer..this law of three state that i need distructor copy consstrucot and vopy assigment operator???
int main()
{
    Array myArray;

    myArray.SetData(0, 10000000);
    myArray.SetData(1, 77);
    myArray.SetData(2, 99999);

    Array myArray2;
    myArray2 = myArray; // colpy assignment constructor
    myArray.PrintingData();
    myArray2.PrintingData();
    return 0;
}
