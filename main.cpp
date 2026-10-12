#include "model_viewer/model_viewer.hpp"
#include <filesystem>
#include <iostream>
#include <string>

#ifndef SCP_DEFAULT_GFX
#define SCP_DEFAULT_GFX "GFX/map"
#endif

int main(int argc, char** argv)
{
    std::string path;
    if (argc > 1)
        path = argv[1];
    else if (std::filesystem::is_directory("GFX/map"))
        path = "GFX/map";
    else
        path = SCP_DEFAULT_GFX;

    ModelViewer model_viewer(path);

    return 0;
}
