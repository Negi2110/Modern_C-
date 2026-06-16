#pragma once
//the order that u layout things in memeoiry x,y,z..u shoudl also do that order same in the memeber innitialiser list
class Vector3f
{
public:
    //Vector3f():y(z) , z(2.0f) , x(3.0f)//this will not work due to memory order is x y z
    Vector3f();
     
    
    float x, y, z;
};