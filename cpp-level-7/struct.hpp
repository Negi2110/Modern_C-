#pragma once
//so how does inline works?
struct Struct
{

    // static  const int SomeValue=8;
    //    inline  static   int SomeValue=127;
    static constexpr int SomeValue = 127;
};
// if it is inclass static const then we can initilise it inside class bbut if is only stait cthen no