#include "math.hpp"

namespace Engine
{

    // Coordinates methods
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

    Coordinates Coordinates::operator-(const Coordinates& other) const
    {
        return Coordinates(x - other.x, y - other.y, z - other.z);
    }

    Coordinates Coordinates::operator*(float scalar) const
    {
        return Coordinates(x * scalar, y * scalar, z * scalar);
    }

    Coordinates& Coordinates::operator+=(const Coordinates& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }


    // Coordinates2d methods
    float Coordinates2d::get_x() const
    {
        return x;
    }

    float Coordinates2d::get_y() const
    {
        return y;
    }


    // Transform methods
    Coordinates Transform::get_position() const
    {
        return position;
    }

    Coordinates Transform::get_rotation() const
    {
        return rotation;
    }

    float Transform::get_scale() const
    {
        return scale;
    }

    void Transform::set_position(Coordinates new_pos)
    {
        position = new_pos;
    }

    void Transform::set_rotation(Coordinates new_rot)
    {
        rotation = new_rot;
    }

    void Transform::set_scale(float new_scale) 
    {
        scale = new_scale;
    }

}