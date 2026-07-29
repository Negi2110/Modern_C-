// STL std::span
// afetr cpp 20
// simlar to string_view
// describe an object that can refer to a contiguous sequence of objects with the first element of the
// seqenc at position zero
// array = []
// span pointer to first memeory location and got size of particular element
// pass around data structures in a non-owning way, the std::span added in C++ 20.
// std::span is a 'view' into a container and does not own the  elements. Just like string_view, it is an efficient way to pass data around and think about the 'range of computation'.
#include <iostream>
#include <span>
#include <array>
#include <vector>

// void PrintArray(std::span<int> param)
// {
//     if (param.extent == std::dynamic_extent)
//     {
//         std::cout << "dynamic: "  << std::endl;
//     }else{
//         std::cout<< "static "<<std::endl;
//     }
//     for (auto &elem : param)
//     {
//         std::cout << elem << std::endl;
//     }
// }
void PrintIntData(const std::span<int>& param)
{
    if (param.extent == std::dynamic_extent)
    {
        std::cout << "dynamic: " << std::endl;
    }
    else
    {
        std::cout << "static " << std::endl;
    }
    for (auto &elem : param)
    {
        std::cout << elem << std::endl;
    }
}

int main()
{
    std::array<int, 4> arr = {1, 2, 3, 4};
    std::span mySpan{arr};
    std::cout << mySpan.extent << std::endl; // return the number of elements in the sequentce
    PrintIntData(arr);
    std::vector<int> myVector = {1,2,3,4,5,6,7};
    PrintIntData(std::span(myVector.begin(),3));
    return 0;
}