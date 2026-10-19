#ifndef ROOMS_HPP
#define ROOMS_HPP

#include "engine/engine.hpp"
#include <unordered_map>
#include <string>

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
        Room(std::string description, std::string mesh_path, int shape, int commonness, bool large, bool disable_decals,
            int zone1, int zone2, int zone3, int use_volumetric_lighting, int walksound): description(description), mesh_path(mesh_path),
            shape(shape), commonness(commonness), large(large), disable_decals(disable_decals), zone1(zone1), zone2(zone2), zone3(zone3),
            use_volumetric_lighting(use_volumetric_lighting), walksound(walksound) {};
        
        void set_pos(Engine::Coordinates pos);
        void set_angle(RoomsCardinal direction);
        
        Engine::Coordinates get_pos() const;
        RoomsCardinal get_angle() const;

        bool check_overlaping(Room& other) const;

    private:
        Engine::Coordinates position;
        RoomsCardinal angle;

        std::string description;
        std::string mesh_path;
        int shape;
        int commonness;
        bool large;
        bool disable_decals;
        int zone1;
        int zone2;
        int zone3;
        int use_volumetric_lighting;
        int walksound;
};

std::unordered_map<std::string, Room> make_room_list(const std::string& path);

#endif