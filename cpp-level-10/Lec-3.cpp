// STL C++ Iterators - Range Access (non-member functions-begin,cbegin,etc)

#include <iostream>
#include <vector>
#include <list>
#include <forward_list>

int main()
{
    std::vector<int> container{1, 2, 3, 4, 5, 6};

    // auto it  = begin(container);
    // auto it = cbegin(container); // read only more beacuse of 'c-begin'copnst
    auto it = crbegin(container);
    // auto it = rbegin(container);//revere itrator point to the last one
    // *it = 7;

    std::cout << *it << std::endl;
    it++;
    std::cout << *it << std::endl;
    it++;
    std::cout << *it << std::endl;
    it++;
    std::cout << *it << std::endl;

    std::cout << "Size: " << size(container) << std::endl;

    auto mydata = data(container);

    for (size_t i = 0; i < size(container); i++)
    {
        std::cout << mydata[i] << std::endl;
    }
    return 0;
}