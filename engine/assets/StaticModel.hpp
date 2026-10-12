#ifndef STATIC_MODEL_HPP
#define STATIC_MODEL_HPP

#include "engine/core/math/math.hpp"
#include <string>
#include <vector>

namespace Engine
{
    struct StaticVertex
    {
        float x, y, z;
        float u, v;
        float lu, lv;
        unsigned char r, g, b, a;
    };

    struct StaticPart
    {
        std::string diffuse_path;
        std::string lightmap_path;
        bool alpha = false;
        std::vector<StaticVertex> vertices;
        std::vector<unsigned short> indices;
    };

    struct StaticModel
    {
        std::string name;
        std::vector<StaticPart> parts;

        Coordinates bounds_min;
        Coordinates bounds_max;

        bool has_player_start = false;
        Coordinates player_start;

        std::vector<std::string> warnings;
        std::vector<std::string> used_files;
    };
}

#endif
