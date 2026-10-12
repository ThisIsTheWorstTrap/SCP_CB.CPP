#ifndef XMESH_HPP
#define XMESH_HPP

#include <string>
#include <vector>

namespace Engine::assets
{
    struct XVertex { float x, y, z, u, v; };

    struct XPart
    {
        std::string texture;
        std::vector<XVertex> vertices;
        std::vector<unsigned int> indices;
    };

    struct XMesh
    {
        std::vector<XPart> parts;
    };

    bool load_xmesh(const std::string& path, XMesh& out, std::string& error);
}

#endif
