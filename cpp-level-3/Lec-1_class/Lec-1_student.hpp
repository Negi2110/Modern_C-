#pragma once
#include <string>
// class -user defined type(UDT) that has data members and member functions
// int,char,float=primitve
// Student aman;
// student = data type
// aman = object(instance) of student class

class Student // student -new user defied type
{             // by default everything is private
              // information hiding(public protected and private)
              // ability to crreate instnaces of objects and think in objects in our software

public:
    // special actions
    // Actions-constructor
    // called everytime studne t instnace being creqted
    Student();
    // special actionss
    // Action-destructor
    ////called everytime studne t instnace being removed
    ~Student();
    // hidden 'fields' or 'attributes' our class
    void printName();

public:
    std::string m_name;
};
