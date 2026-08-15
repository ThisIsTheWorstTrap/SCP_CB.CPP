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

#endif