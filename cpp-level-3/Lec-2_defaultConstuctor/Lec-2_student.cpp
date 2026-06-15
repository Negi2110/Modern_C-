#include "Lec-2_student.hpp"
#include <iostream>
// special actions
// Actions-constructor
// called everytime studne t instnace being creqted
Student::Student()
{
    m_name = "no name";
    std::cout << "constructor" << std::endl;
    std::cout << "my name: " << m_name << std::endl;
}
Student::Student(std::string name)
{
    m_name = name;
    std::cout << "constructor" << std::endl;
    std::cout << "m_name: " << m_name << std::endl;
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