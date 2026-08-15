#include "math.hpp"

float Coordinates::get_x() const 
{
    return x;
}

float Coordinates::get_y() const
{
    return y;
}

float Coordinates::get_z() const
{
    return z;
}

Coordinates Coordinates::operator+(const Coordinates& other) const
{
    return Coordinates(x+other.x, y+other.y, z+other.z);
}