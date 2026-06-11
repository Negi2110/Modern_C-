#include <iostream>
// heap=new and delete

// int *allocateMemroy()
// {

//     return new int;
// }
// alllows us to allocate resoirces at runtime (dynamic memrory allocation)
// that resource is returned to us in a pointer
int main()
{
    int numberofStudent;
    std::cout << "how many student are there in class?" << std::endl;
    std::cin >> numberofStudent;
    // int studentids[numberofStudent];//but this can overflow the stack as stack memory is very kess
    int *studentids = new int[numberofStudent];
    // int* studentids = new int(numberofStudent);..this will create that no of array and will be memeory leak

    for (int i = 0; i < numberofStudent; i++)
    {
        studentids[i] = i;
    }

     delete[] studentids;//delete all elements from the arrayv and most imp to prevent memeory leaks
    int* y = new int;//what does int(5) wiill do??
    delete y;
     return 0;
}