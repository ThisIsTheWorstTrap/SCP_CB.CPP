#include "RMesh.hpp"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>

namespace Engine::assets
{
    namespace
    {
        class Reader
        {
            public:
                explicit Reader(std::vector<unsigned char> d) : data(std::move(d)) {}

                bool eof() const { return pos >= data.size(); }

                unsigned char byte()
                {
                    need(1);
                    return data[pos++];
                }

                int32_t i32()
                {
                    need(4);
                    int32_t v;
                    std::memcpy(&v, &data[pos], 4);
                    pos += 4;
                    return v;
                }

                float f32()
                {
                    need(4);
                    float v;
                    std::memcpy(&v, &data[pos], 4);
                    pos += 4;
                    return v;
                }

                std::string str()
                {
                    int32_t n = i32();
                    if (n < 0) throw std::runtime_error("negative string length");
                    need((size_t)n);
                    std::string s((const char*)&data[pos], (size_t)n);
                    pos += (size_t)n;
                    return s;
                }

                int32_t count(size_t min_bytes_per_item)
                {
                    int32_t n = i32();
                    if (n < 0 || (size_t)n * min_bytes_per_item > data.size() - pos)
                        throw std::runtime_error("bad count");
                    return n;
                }

            private:
                void need(size_t n) const
                {
                    if (pos + n > data.size())
                        throw std::runtime_error("unexpected end of file");
                }

                std::vector<unsigned char> data;
                size_t pos = 0;
        };

        void read_triangles(Reader& r, std::vector<unsigned int>& indices, size_t vertex_count)
        {
            int32_t tri = r.count(12);
            indices.reserve((size_t)tri * 3);
            for (int32_t i = 0; i < tri; i++)
            {
                for (int k = 0; k < 3; k++)
                {
                    int32_t idx = r.i32();
                    if (idx < 0 || (size_t)idx >= vertex_count)
                        throw std::runtime_error("triangle index out of range");
                    indices.push_back((unsigned int)idx);
                }
            }
        }

        RMeshCollisionSurface read_collision_surface(Reader& r)
        {
            RMeshCollisionSurface s;
            int32_t nv = r.count(12);
            s.vertices.resize((size_t)nv);
            for (auto& v : s.vertices) { v[0] = r.f32(); v[1] = r.f32(); v[2] = r.f32(); }
            read_triangles(r, s.indices, s.vertices.size());
            return s;
        }

        std::array<float, 3> read_vec3(Reader& r)
        {
            std::array<float, 3> v;
            v[0] = r.f32(); v[1] = r.f32(); v[2] = r.f32();
            return v;
        }

        void parse_color(const std::string& s, int out[3])
        {
            out[0] = out[1] = out[2] = 255;
            std::istringstream is(s);
            for (int i = 0; i < 3; i++) { int v; if (is >> v) out[i] = v; }
        }

        void parse_angles(const std::string& s, float out[3])
        {
            out[0] = out[1] = out[2] = 0;
            std::istringstream is(s);
            for (int i = 0; i < 3; i++) { float v; if (is >> v) out[i] = v; }
        }

        void parse(Reader& r, RMesh& m)
        {
            std::string header = r.str();
            bool has_trigger = false;
            if (header == "RoomMesh.HasTriggerBox") has_trigger = true;
            else if (header != "RoomMesh")
                throw std::runtime_error("not an RMESH file (header \"" + header + "\")");

            int32_t surface_count = r.count(1);
            m.surfaces.reserve((size_t)surface_count);
            for (int32_t i = 0; i < surface_count; i++)
            {
                RMeshSurface s;
                for (int slot = 0; slot < 2; slot++)
                {
                    unsigned char flag = r.byte();
                    std::string name;
                    if (flag != 0) name = r.str();
                    if (slot == 0) { s.lightmap_flag = flag; s.lightmap = name; }
                    else           { s.diffuse_flag  = flag; s.diffuse  = name; }
                }

                s.alpha = (s.diffuse_flag == 3);

                int32_t nv = r.count(31);
                s.vertices.resize((size_t)nv);
                for (auto& v : s.vertices)
                {
                    v.x = r.f32(); v.y = r.f32(); v.z = r.f32();
                    v.u = r.f32(); v.v = r.f32();
                    v.lu = r.f32(); v.lv = r.f32();
                    v.r = r.byte(); v.g = r.byte(); v.b = r.byte();
                }
                read_triangles(r, s.indices, s.vertices.size());
                m.surfaces.push_back(std::move(s));
            }

            int32_t collision_count = r.count(8);
            for (int32_t i = 0; i < collision_count; i++)
                m.collision.push_back(read_collision_surface(r));

            if (has_trigger)
            {
                int32_t boxes = r.count(8);
                for (int32_t b = 0; b < boxes; b++)
                {
                    RMeshTriggerBox box;
                    int32_t n = r.count(8);
                    for (int32_t i = 0; i < n; i++)
                        box.surfaces.push_back(read_collision_surface(r));
                    box.name = r.str();
                    m.trigger_boxes.push_back(std::move(box));
                }
            }

            int32_t entity_count = r.count(4);
            for (int32_t i = 0; i < entity_count; i++)
            {
                std::string type = r.str();
                if (type == "screen")
                {
                    RMeshScreen s;
                    s.pos = read_vec3(r);
                    s.image = r.str();
                    m.screens.push_back(std::move(s));
                }
                else if (type == "waypoint")
                {
                    m.waypoints.push_back(read_vec3(r));
                }
                else if (type == "light" || type == "spotlight")
                {
                    RMeshLight l;
                    l.pos = read_vec3(r);
                    l.range = r.f32();
                    parse_color(r.str(), l.color);
                    l.intensity = r.f32();
                    if (type == "spotlight")
                    {
                        l.spot = true;
                        float a[3];
                        parse_angles(r.str(), a);
                        l.pitch = a[0];
                        l.yaw = a[1];
                        l.inner_cone = r.i32();
                        l.outer_cone = r.i32();
                    }
                    m.lights.push_back(l);
                }
                else if (type == "soundemitter")
                {
                    RMeshSound s;
                    s.pos = read_vec3(r);
                    s.sound_id = r.i32();
                    s.range = r.f32();
                    m.sound_emitters.push_back(s);
                }
                else if (type == "playerstart")
                {
                    m.has_player_start = true;
                    m.player_start = read_vec3(r);
                    float a[3];
                    parse_angles(r.str(), a);
                    m.player_angles = { a[0], a[1], a[2] };
                }
                else if (type == "model")
                {
                    RMeshProp p;
                    p.file = r.str();
                    p.pos = read_vec3(r);
                    p.rot = read_vec3(r);
                    p.scale = read_vec3(r);

                    if (!p.file.empty()) m.props.push_back(std::move(p));
                }
                else
                {

                    throw std::runtime_error("unknown entity type \"" + type + "\"");
                }
            }

        }
    }

    bool load_rmesh(const std::string& path, RMesh& out, std::string& error)
    {
        std::ifstream f(path, std::ios::binary);
        if (!f)
        {
            error = "cannot open file";
            return false;
        }
        std::vector<unsigned char> data((std::istreambuf_iterator<char>(f)),
                                         std::istreambuf_iterator<char>());
        try
        {
            out = RMesh{};
            Reader r(std::move(data));
            parse(r, out);
        }
        catch (const std::exception& e)
        {
            error = e.what();
            return false;
        }
        return true;
    }
}
