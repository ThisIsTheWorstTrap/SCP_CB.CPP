#include "RoomLoader.hpp"
#include "AssetResolver.hpp"
#include "RMesh.hpp"
#include "XMesh.hpp"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <limits>
#include <map>
#include <memory>
#include <tuple>

namespace fs = std::filesystem;

namespace Engine::assets
{
    namespace
    {

        constexpr size_t MAX_PART_VERTICES = 65000;

        using PartKey = std::tuple<std::string, std::string, bool>;

        class PartSink
        {
            public:
                explicit PartSink(std::vector<StaticPart>& parts) : parts(parts) {}

                void add(const PartKey& key, const std::vector<StaticVertex>& vertices,
                         const std::vector<unsigned int>& indices)
                {
                    std::vector<long> remap(vertices.size(), -1);
                    size_t current = open_part(key);

                    for (size_t t = 0; t + 2 < indices.size(); t += 3)
                    {
                        StaticPart* part = &parts[current];
                        if (part->vertices.size() + 3 > MAX_PART_VERTICES)
                        {
                            current = new_part(key);
                            part = &parts[current];
                            std::fill(remap.begin(), remap.end(), -1);
                        }
                        for (int k = 0; k < 3; k++)
                        {
                            unsigned int src = indices[t + (size_t)k];
                            if (remap[src] < 0)
                            {
                                remap[src] = (long)part->vertices.size();
                                part->vertices.push_back(vertices[src]);
                            }
                            part->indices.push_back((unsigned short)remap[src]);
                        }
                    }
                }

            private:
                size_t new_part(const PartKey& key)
                {
                    StaticPart p;
                    p.diffuse_path = std::get<0>(key);
                    p.lightmap_path = std::get<1>(key);
                    p.alpha = std::get<2>(key);
                    parts.push_back(std::move(p));
                    open[key] = parts.size() - 1;
                    return parts.size() - 1;
                }

                size_t open_part(const PartKey& key)
                {
                    auto it = open.find(key);
                    if (it != open.end() && parts[it->second].vertices.size() < MAX_PART_VERTICES)
                        return it->second;
                    return new_part(key);
                }

                std::vector<StaticPart>& parts;
                std::map<PartKey, size_t> open;
        };

        inline void to_engine(float& x, float& y, float& z)
        {
            x *= ROOM_SCALE;
            y *= ROOM_SCALE;
            z *= -ROOM_SCALE;
        }

        inline void rotate_blitz(float& x, float& y, float& z, float pitch, float yaw, float roll)
        {
            float cr = std::cos(roll),  sr = std::sin(roll);
            float cp = std::cos(pitch), sp = std::sin(pitch);
            float cy = std::cos(yaw),   sy = std::sin(yaw);

            float x1 = x * cr - y * sr;
            float y1 = x * sr + y * cr;
            float z1 = z;

            float y2 = y1 * cp - z1 * sp;
            float z2 = y1 * sp + z1 * cp;
            float x2 = x1;

            x = x2 * cy - z2 * sy;
            z = x2 * sy + z2 * cy;
            y = y2;
        }

        void flip_winding(std::vector<unsigned int>& idx)
        {
            for (size_t i = 0; i + 2 < idx.size(); i += 3) std::swap(idx[i + 1], idx[i + 2]);
        }

        void grow(StaticModel& m, bool& first, float x, float y, float z)
        {
            if (first)
            {
                m.bounds_min = Coordinates(x, y, z);
                m.bounds_max = Coordinates(x, y, z);
                first = false;
                return;
            }
            m.bounds_min = Coordinates(std::min(m.bounds_min.get_x(), x), std::min(m.bounds_min.get_y(), y), std::min(m.bounds_min.get_z(), z));
            m.bounds_max = Coordinates(std::max(m.bounds_max.get_x(), x), std::max(m.bounds_max.get_y(), y), std::max(m.bounds_max.get_z(), z));
        }
    }

    bool load_room_model(const std::string& rmesh_path, StaticModel& out, std::string& error)
    {
        RMesh rmesh;
        if (!load_rmesh(rmesh_path, rmesh, error)) return false;

        out = StaticModel{};
        out.name = fs::path(rmesh_path).stem().string();

        fs::path dir = fs::path(rmesh_path).parent_path();
        if (dir.empty()) dir = ".";
        fs::path gfx = AssetResolver::find_gfx_root(dir);

        AssetResolver resolver({ dir, gfx / "map", gfx / "Map" / "Props", gfx / "map" / "props", gfx });
        PartSink sink(out.parts);
        bool first_bound = true;

        auto use_file = [&](const std::string& path)
        {
            if (!path.empty() && std::find(out.used_files.begin(), out.used_files.end(), path) == out.used_files.end())
                out.used_files.push_back(path);
        };
        use_file(rmesh_path);

        auto warn_once = [&](const std::string& what)
        {
            if (std::find(out.warnings.begin(), out.warnings.end(), what) == out.warnings.end())
                out.warnings.push_back(what);
        };

        for (const RMeshSurface& s : rmesh.surfaces)
        {
            std::string diffuse, lightmap;
            if (!s.diffuse.empty())
            {
                diffuse = resolver.find_from(dir, s.diffuse);
                if (diffuse.empty()) warn_once("missing texture: " + s.diffuse);
                use_file(diffuse);
            }
            if (!s.lightmap.empty())
            {
                lightmap = resolver.find_from(dir, s.lightmap);
                if (lightmap.empty()) warn_once("missing lightmap: " + s.lightmap);
                use_file(lightmap);
            }

            std::vector<StaticVertex> verts(s.vertices.size());
            for (size_t i = 0; i < s.vertices.size(); i++)
            {
                const RMeshVertex& v = s.vertices[i];
                float x = v.x, y = v.y, z = v.z;
                to_engine(x, y, z);
                verts[i] = { x, y, z, v.u, v.v, v.lu, v.lv, v.r, v.g, v.b, 255 };
                grow(out, first_bound, x, y, z);
            }
            std::vector<unsigned int> idx = s.indices;
            flip_winding(idx);
            sink.add(PartKey{ diffuse, lightmap, s.alpha }, verts, idx);
        }

        std::map<std::string, std::unique_ptr<XMesh>> prop_cache;
        for (const RMeshProp& p : rmesh.props)
        {
            std::string key = to_lower(p.file);
            auto it = prop_cache.find(key);
            if (it == prop_cache.end())
            {
                std::unique_ptr<XMesh> mesh;
                std::string path = resolver.find_from(dir / "Props", "Props/" + p.file);
                if (path.empty()) path = resolver.find_from(dir / "Props", p.file);
                if (path.empty())
                {
                    warn_once("missing prop model: " + p.file);
                }
                else
                {
                    use_file(path);
                    mesh = std::make_unique<XMesh>();
                    std::string err;
                    if (!load_xmesh(path, *mesh, err))
                    {
                        warn_once("cannot load prop " + p.file + ": " + err);
                        mesh.reset();
                    }
                    else
                    {

                        fs::path xdir = fs::path(path).parent_path();
                        for (XPart& part : mesh->parts)
                        {
                            if (part.texture.empty()) continue;
                            std::string real = resolver.find_from(xdir, part.texture);
                            if (real.empty()) warn_once("missing prop texture: " + part.texture + " (" + p.file + ")");
                            part.texture = real;
                            use_file(real);
                        }
                    }
                }
                it = prop_cache.emplace(key, std::move(mesh)).first;
            }
            const XMesh* mesh = it->second.get();
            if (!mesh) continue;

            const float deg = 3.14159265358979f / 180.0f;
            for (const XPart& part : mesh->parts)
            {
                std::vector<StaticVertex> verts(part.vertices.size());
                for (size_t i = 0; i < part.vertices.size(); i++)
                {
                    const XVertex& v = part.vertices[i];
                    float x = v.x * p.scale[0], y = v.y * p.scale[1], z = v.z * p.scale[2];
                    rotate_blitz(x, y, z, p.rot[0] * deg, p.rot[1] * deg, p.rot[2] * deg);
                    x += p.pos[0]; y += p.pos[1]; z += p.pos[2];
                    to_engine(x, y, z);
                    verts[i] = { x, y, z, v.u, v.v, 0.f, 0.f, 255, 255, 255, 255 };
                    grow(out, first_bound, x, y, z);
                }
                std::vector<unsigned int> idx = part.indices;

                float sign = p.scale[0] * p.scale[1] * p.scale[2];
                if (sign >= 0.0f) flip_winding(idx);
                sink.add(PartKey{ part.texture, "", false }, verts, idx);
            }
        }

        if (rmesh.has_player_start)
        {
            float x = rmesh.player_start[0], y = rmesh.player_start[1], z = rmesh.player_start[2];
            to_engine(x, y, z);
            out.has_player_start = true;
            out.player_start = Coordinates(x, y, z);
        }

        if (out.parts.empty())
        {
            error = "no geometry";
            return false;
        }
        return true;
    }
}
