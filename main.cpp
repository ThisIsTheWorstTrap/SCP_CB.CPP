#include "model_viewer/model_viewer.hpp"
#include <string>

int main(int argc, char** argv)
{
    auto model_viewer = new ModelViewer(std::string(argv[1]));

    return 0;
}