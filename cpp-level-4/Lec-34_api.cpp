// static member variable and functionss in class

// static has to do with the lifetime of variable

// API
// major -static...they are scoope op..API::major
// minor-static...API::minor
// no instnace of api..stattic varible in a class exist outside of the class
// the scope is within the class however??what does this mean in here
#include <iostream>
#include "api.hpp"
// struct API
// {
// public:
//     API() {}
//     ~API() {}

//     // const static int MAJOR_VERSION;..betwer to put const on static
//     // const static int MINOR_VERSION;
//     // member varibale that belongs to each instance
//     int m_local;
//     static int MAJOR_VERSION;
//     static int MINOR_VERSION;
//     static int GetMajorVersion()
//     { // std::cout<<this<<'\n';//'this' is not available in static function because this is a pointer to the instance of the class
//         // things specific to the instnace of class are not allowed inside static function
//         return MAJOR_VERSION;
//         // return m_local; // this will not work because m_local is not static and belongs to an instance
//     } // static function can only use and return static member variables and functions
// private:
// };
// const int API::MAJOR_VERSION = 7;
//int API::MAJOR_VERSION = 7;
int main()
{
    // API instance;
    // instance.API::MAJOR_VERSION = 7;
    std::cout << "Major : " << API::MAJOR_VERSION << '\n';
    std::cout << "Major : " << API::GetMajorVersion() << '\n';

    return 0;
}