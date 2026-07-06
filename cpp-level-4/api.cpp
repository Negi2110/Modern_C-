#include <iostream>
#include "api.hpp"

API::API() {}
API::~API() {}
 int API::GetMajorVersion()//dont write static here because it is already declared as static in the class
{ // std::cout<<this<<'\n';//'this' is not available in static function because this is a pointer to the instance of the class
    // things specific to the instnace of class are not allowed inside static function
    return MAJOR_VERSION;
    // return m_local; // this will not work because m_local is not static and belongs to an instance
} // static function can only use and return static member variables and functions
int API::MAJOR_VERSION = 7;