#pragma once
#include <iostream>
struct API
{
    API();
    ~API();

    // const static int MAJOR_VERSION;..betwer to put const on static
    // const static int MINOR_VERSION;
    // member varibale that belongs to each instance
    static int MAJOR_VERSION;
    static int MINOR_VERSION;
    static int GetMajorVersion();
    // static function can only use and return static member variables and functions
};