#include <iostream>
#include <string>
#include "Lec-1_student.hpp"

int main()
{ // an instnace of student or object
  // std::cout<<"program starts"<<std::endl;
  // {
    //    Student aman;
    // }
    // Student* aman = new Student();
    // delete aman;
    // aman=nullptr;
    // std::cout << "program ends" <<std::endl;
    Student aman;
    aman.m_name = "Aman negi";
    aman.printName();

    Student sue;
    sue.m_name = "sagar negi";
    sue.printName();
    return 0;

    // if aman created firs then it will be last to be distroyed ad=s in same stack??this can differ when we have nested parantesis
}