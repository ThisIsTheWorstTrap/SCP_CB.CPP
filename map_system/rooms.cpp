#include "rooms.hpp"

Engine::Coordinates Room::get_pos() const
{
    return position;
}

float Room::get_angle() const
{
    return angle;
}

Engine::Coordinates Room::get_size() const
{
    return size;
}

bool Room::check_overlaping(Room& other) const
{
    // TODO later
}