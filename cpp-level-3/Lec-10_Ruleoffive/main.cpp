#include <iostream>
#include <vector>
#include "IntArray.hpp"

// void foo(IntArray& temp) -it do not create copy
//  void foo(IntArray temp) {////it create new copy dusting funci0on calling use copy construto

// }
// rule of 5
IntArray foo()
{
    IntArray result("foo created array");
    return result;
}
int main()
{
    // /IntArray array1("array1");//first to be created last to be distroyed
    // IntArray array2=array1;//last to be created first to be distroyed
    // foo(array1);
    std::vector<IntArray> myArrays;

    myArrays.reserve(10);// without it 35 alloc in the valgrind-it is being tric
    for (int i = 0; i < 10; i++)
    {
        IntArray temp(std::to_string(i));
        // myArrays.push_back(temp);//here copy construtor being called in push_back func i think??//24 alloocation in valgrind
         myArrays.push_back(std::move(temp));//13 alloaction in the valgrinds with reserver tric otherwise 35
        //myArrays.push_back(temp);//49 alloc in the valgrind-withou using reserve
    }
    // IntArray array1("array1");

    return 0;
}