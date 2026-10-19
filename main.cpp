//#include "model_viewer/model_viewer.hpp"
#include "map_system/rooms.hpp"
#include <string>

int main(int argc, char** argv)
{
    //auto model_viewer = new ModelViewer(std::string(argv[1]));
    std::string s = argv[2];
    make_room_list(s);

    return 0;
}