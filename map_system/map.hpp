#ifndef MAP_HPP
#define MAP_HPP

#include <array>
#include <string>

#define MAP_SIZE 19

class Map
{
    public:
        Map();

        void create_random_grid();
        void build_map();
        void save_map(std::string file);

    private:
        std::array<std::array<int, MAP_SIZE>, MAP_SIZE> map_grid;
};

#endif