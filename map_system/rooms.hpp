#ifndef ROOMS_HPP
#define ROOMS_HPP

#include "engine/engine.hpp"

typedef enum
{
    NORTH=0,
    WEST=90,
    SOUTH=180,
    EAST=270,
} RoomsCardinal;

class Room
{
    public:
        Room(Engine::Coordinates position, RoomsCardinal angle): position(position), angle(angle) {};
        Engine::Coordinates get_pos() const;
        RoomsCardinal get_angle() const;
        Engine::Coordinates get_size() const;

        bool check_overlaping(Room& other) const;

    private:
        Engine::Coordinates position;
        RoomsCardinal angle;

        Engine::Coordinates size;

};

#endif