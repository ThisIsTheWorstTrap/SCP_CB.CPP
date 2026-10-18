#include "rooms.hpp"

void Room::set_pos(Engine::Coordinates pos)
{
    this->position = pos;
}

void Room::set_angle(RoomsCardinal direction)
{
    this->angle = direction;
}

Engine::Coordinates Room::get_pos() const
{
    return position;
}

RoomsCardinal Room::get_angle() const
{
    return angle;
}

bool Room::check_overlaping(Room& other) const
{
    // TODO later
}

std::unordered_map<std::string, Room> make_room_list(std::string path)
{

}