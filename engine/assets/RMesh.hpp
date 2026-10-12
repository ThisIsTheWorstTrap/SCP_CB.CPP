#ifndef RMESH_HPP
#define RMESH_HPP

#include <array>
#include <string>
#include <vector>

namespace Engine::assets
{
    struct RMeshVertex
    {
        float x, y, z;
        float u, v;
        float lu, lv;
        unsigned char r, g, b;
    };

    struct RMeshSurface
    {
        std::string diffuse;
        std::string lightmap;
        unsigned char diffuse_flag = 0;
        unsigned char lightmap_flag = 0;
        bool alpha = false;
        std::vector<RMeshVertex> vertices;
        std::vector<unsigned int> indices;
    };

    struct RMeshCollisionSurface
    {
        std::vector<std::array<float, 3>> vertices;
        std::vector<unsigned int> indices;
    };

    struct RMeshTriggerBox
    {
        std::vector<RMeshCollisionSurface> surfaces;
        std::string name;
    };

    struct RMeshLight
    {
        std::array<float, 3> pos;
        float range;
        int color[3];
        float intensity;
        bool spot = false;
        float pitch = 0, yaw = 0;
        int inner_cone = 0, outer_cone = 0;
    };

    struct RMeshScreen    { std::array<float, 3> pos; std::string image; };
    struct RMeshSound     { std::array<float, 3> pos; int sound_id; float range; };
    struct RMeshProp
    {
        std::string file;
        std::array<float, 3> pos;
        std::array<float, 3> rot;
        std::array<float, 3> scale;
    };

    struct RMesh
    {
        std::vector<RMeshSurface> surfaces;
        std::vector<RMeshCollisionSurface> collision;
        std::vector<RMeshTriggerBox> trigger_boxes;

        std::vector<RMeshLight> lights;
        std::vector<RMeshScreen> screens;
        std::vector<RMeshSound> sound_emitters;
        std::vector<std::array<float, 3>> waypoints;
        std::vector<RMeshProp> props;

        bool has_player_start = false;
        std::array<float, 3> player_start{};
        std::array<float, 3> player_angles{};
    };

    bool load_rmesh(const std::string& path, RMesh& out, std::string& error);
}

#endif
