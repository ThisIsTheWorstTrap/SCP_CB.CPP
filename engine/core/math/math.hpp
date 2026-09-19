#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>

namespace Engine
{

    class Coordinates
    {
        public:
            Coordinates(float x=0.0f, float y=0.0f, float z=0.0f): x(x), y(y), z(z) {};

            float get_x() const;
            float get_y() const;
            float get_z() const;

            Coordinates operator+(const Coordinates& other) const;
            Coordinates operator-(const Coordinates& other) const;
            Coordinates operator*(const float scalar) const;
            Coordinates& operator+=(const Coordinates& other);

            Coordinates cross(const Coordinates& other) const;
            float length() const;
            Coordinates normalized() const;

        private:
            float x, y, z;
    };

    class Coordinates2d
    {
        public:
            Coordinates2d(float x=0.0f, float y=0.0f): x(x), y(y) {};

            float get_x() const;
            float get_y() const;

        private:
            float x, y;
    };

}

#endif