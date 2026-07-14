// static and dynamic cast
#include <iostream>
// static_cast-converts beetween types using a combination of implicit and user defined conversions.

// Base class(vtable ptr)---->derived class
// note:c style cast in a cpp compiler essentially try all these cast
// dynamic cast -safely converts pointers and references to classes up ,down, and sideways along the inheritance
// if(dynamic_cast<Derived*>(Base)) {}
// static=less expensive,more dangerous at compiler level vs dynmic cast=more xpensive works with pointer and references and also return std::bad_cast

int main()
{

    return 0;
}