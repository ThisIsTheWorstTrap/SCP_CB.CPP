#ifndef MATH_HPP
#define MATH_HPP

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

    class Transform
    {
        public:
            Transform(Coordinates position, Coordinates rotation, float scale): position(position), rotation(rotation), scale(scale) {};

            Coordinates get_position() const;
            Coordinates get_rotation() const;
            float get_scale() const;

            void set_position(Coordinates new_pos);
            void set_rotation(Coordinates new_rot);
            void set_scale(float new_scale);

        private:
            Coordinates position;
            Coordinates rotation;
            float scale;
    };

}

#endif