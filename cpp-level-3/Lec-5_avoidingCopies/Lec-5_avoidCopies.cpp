#include <iostream>
#include "Array.hpp"
//the rule of three
//shallow copy-if u pnly have trivial data type like int and float also like vector
//but whenn there is pointer..this law of three state that i need distructor copy consstrucot and vopy assigment operator???
void printanArray( Array a) {//try to avoid copies,delete copy ctor and pass by ref
   // a.PrintingData();
}
//as sson as i move copy consturto to private ..that way we can handle this situation
int main()
{
    Array myArray;

    myArray.SetData(0, 12345640);
   

    Array myArray2= myArray;
    //myArray2 = myArray; // colpy assignment constructor
    // myArray.PrintingData();
    // myArray2.PrintingData();
    printanArray(myArray);
    return 0;
}
