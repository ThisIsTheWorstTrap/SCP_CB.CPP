#include <fstream>
#include <iostream>
#include <stdexcept>
#include "lib/json.hpp"
#include "rooms.hpp"

using json = nlohmann::json;

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

std::unordered_map<std::string, Room> make_room_list(const std::string& path)
{
    std::ifstream f(path);
    if (!f)
        throw std::runtime_error("File does not exists :" + path);

    json data = json::parse(f);
    if (!data.is_object())
        throw std::runtime_error(path + " must be a json object");

    std::unordered_map<std::string, Room> room_list;

    for (const auto& [key, val] : data.items())
    {
        if (key.compare("room ambience") == 0)
            continue;

        room_list.insert_or_assign(key, Room(
            val.value("descr", "any"),
            val.value("mesh path", "any"),
            val.value("shape", 1),
            val.value("commonness", 0),
            val.value("large", false),
            val.value("disabledecals", false),
            val.value("zone1", 0),
            val.value("zone2", 0),
            val.value("zone3", 0),
            val.value("usevolumelighting", 0),
            val.value("walksound", 0)
        ));

        std::cout << val << std::endl;
    }

    return room_list;
}