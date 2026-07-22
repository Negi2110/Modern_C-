#include <iostream>
#include <vector>
#include <algorithm>

// struct print_functor
// {
//     int lastResult;
//     inline void operator()(int n)
//     {
//         lastResult = n;
//         std::cout << n << ",";
//     }
// };
//[captures] (params) {body}

// void print_vFN(int n)
// {
//     std::cout<< n << ",";
// }
int global = 1;
static int global1 = -1;
int main()
{
    std::vector<int> v{1, 3, 2, 5, 9};

    int lastResult = -1;
    // int lastResult2 = -1;
    //  Is essentially a 'raw function pointer'
    //  void(*print_v)(int);
    //  auto print_v = [&lastResult,&lastResult2](int n)
    //  auto print_v = [lastResult,lastResult2](int n)-this will generate eroor-here it wil not let edit thiese
    // auto print_v = [&](int n)
    // auto print_v = [lastResult,lastResult2]  (int n) mutable
    //    auto print_v = [&]  (int n)
    auto print_v = [=](int n) mutable // why =?..and also & without mutable will also wok
    {
        // lastResult = n;
        //  lastResult2 = n;
        // std::cout<<lastResult<<std::endl;
        global = 10;
        global1 = 11;
        std::cout << n << ",";
    };
    std::for_each(begin(v), end(v), print_v);
    // std::cout << '\n'
    //           << lastResult << std::endl;
    std::cout << '\n'
              << global << std::endl;
    std::cout << '\n'
              << global1 << std::endl;
    return 0;
}