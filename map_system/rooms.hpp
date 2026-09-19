#ifndef ROOMS_HPP
#define ROOMS_HPP

#include "engine/engine.hpp"

typedef enum
{
    NORTH,
    WEST,
    SOUTH,
    EAST,
} RoomsCardinal;

class Room
{
    public:
        Room(Engine::Coordinates position, float angle): position(position), angle(angle) {};
        Engine::Coordinates get_pos() const;
        float get_angle() const;
        Engine::Coordinates get_size() const;

        bool check_overlaping(Room& other) const;

    private:
        Engine::Coordinates position;
        float angle;

        Engine::Coordinates size;

};

#endif