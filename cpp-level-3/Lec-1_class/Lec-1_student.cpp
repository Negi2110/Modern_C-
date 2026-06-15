#include "Lec-1_student.hpp"
#include  <iostream>
// special actions
// Actions-constructor
// called everytime studne t instnace being creqted
Student::Student()
{
    std::cout << "Constructor" << std::endl;
}
// special actionss
// Action-destructor
////called everytime studne t instnace being removed
Student::~Student()
{
    std::cout << "Distructor: " << m_name << std::endl;
}
// hidden 'fields' or 'attributes' our class
void Student::printName()
{
    std::cout << "name is: " << m_name << std::endl;
}