#ifndef MATH_CPP
#define MATH_CPP

class Coordinates
{
    public:
        Coordinates(float x=0.0f, float y=0.0f, float z=0.0f): x(x), y(y), z(z) {};

        float get_x() const;
        float get_y() const;
        float get_z() const;

        Coordinates operator+(const Coordinates& other) const;

    private:
        float x, y, z;
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

#endif