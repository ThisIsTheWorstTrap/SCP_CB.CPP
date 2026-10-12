#ifndef ROOM_LOADER_HPP
#define ROOM_LOADER_HPP

#include "StaticModel.hpp"
#include <string>

namespace Engine::assets
{

    constexpr float ROOM_SCALE = 8.0f / 2048.0f;

    bool load_room_model(const std::string& rmesh_path, StaticModel& out, std::string& error);
}

#endif
