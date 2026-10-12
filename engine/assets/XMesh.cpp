#include "XMesh.hpp"
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace Engine::assets
{
    namespace
    {
        enum class Tok { Word, String, Open, Close };
        struct Token { Tok type; std::string text; };

        std::vector<Token> tokenize(const std::string& s, size_t start)
        {
            std::vector<Token> t;
            size_t i = start, n = s.size();
            while (i < n)
            {
                char c = s[i];
                if (std::isspace((unsigned char)c) || c == ',' || c == ';') { i++; continue; }
                if (c == '#' || (c == '/' && i + 1 < n && s[i + 1] == '/'))
                {
                    while (i < n && s[i] != '\n') i++;
                    continue;
                }
                if (c == '{') { t.push_back({ Tok::Open, "{" }); i++; continue; }
                if (c == '}') { t.push_back({ Tok::Close, "}" }); i++; continue; }
                if (c == '"')
                {
                    size_t e = s.find('"', i + 1);
                    if (e == std::string::npos) e = n;
                    t.push_back({ Tok::String, s.substr(i + 1, e - i - 1) });
                    i = e + 1;
                    continue;
                }
                if (c == '<')
                {
                    size_t e = s.find('>', i);
                    i = (e == std::string::npos) ? n : e + 1;
                    continue;
                }
                size_t b = i;
                while (i < n && !std::isspace((unsigned char)s[i]) &&
                       s[i] != ',' && s[i] != ';' && s[i] != '{' && s[i] != '}' && s[i] != '"')
                    i++;
                t.push_back({ Tok::Word, s.substr(b, i - b) });
            }
            return t;
        }

        bool is_number(const std::string& w)
        {
            if (w.empty()) return false;
            char c = w[0];
            return std::isdigit((unsigned char)c) || c == '-' || c == '+' || c == '.';
        }

        struct Node
        {
            std::string type, name;
            std::vector<std::string> words;
            std::vector<std::string> strings;
            std::vector<std::unique_ptr<Node>> children;
        };

        std::unique_ptr<Node> parse_node(const std::vector<Token>& t, size_t& i, std::string type, std::string name)
        {

            auto node = std::make_unique<Node>();
            node->type = std::move(type);
            node->name = std::move(name);
            while (i < t.size())
            {
                const Token& k = t[i];
                if (k.type == Tok::Close) { i++; return node; }
                if (k.type == Tok::Open)
                {
                    i++;
                    node->children.push_back(parse_node(t, i, "ref", ""));
                    continue;
                }
                if (k.type == Tok::String) { node->strings.push_back(k.text); i++; continue; }

                if (!is_number(k.text))
                {
                    if (i + 1 < t.size() && t[i + 1].type == Tok::Open)
                    {
                        std::string ty = k.text;
                        i += 2;
                        node->children.push_back(parse_node(t, i, ty, ""));
                        continue;
                    }
                    if (i + 2 < t.size() && t[i + 1].type == Tok::Word && t[i + 2].type == Tok::Open)
                    {
                        std::string ty = k.text, nm = t[i + 1].text;
                        i += 3;
                        node->children.push_back(parse_node(t, i, ty, nm));
                        continue;
                    }
                }
                node->words.push_back(k.text);
                i++;
            }
            return node;
        }

        struct Mat { float m[16]; };
        Mat identity() { Mat r{}; r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f; return r; }
        Mat mul(const Mat& a, const Mat& b)
        {
            Mat r{};
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                {
                    float s = 0;
                    for (int k = 0; k < 4; k++) s += a.m[i * 4 + k] * b.m[k * 4 + j];
                    r.m[i * 4 + j] = s;
                }
            return r;
        }

        struct Material { std::string texture; };

        struct Ctx
        {
            std::map<std::string, Material> named;
            XMesh* out;
        };

        std::string texture_of(const Node& material)
        {
            for (auto& c : material.children)
                if (c->type == "TextureFilename" && !c->strings.empty())
                    return c->strings[0];
            return "";
        }

        void collect_materials(const Node& n, Ctx& ctx)
        {
            if (n.type == "Material" && !n.name.empty())
                ctx.named[n.name] = Material{ texture_of(n) };
            for (auto& c : n.children) collect_materials(*c, ctx);
        }

        float num(const std::string& s) { return (float)std::strtod(s.c_str(), nullptr); }
        long inum(const std::string& s) { return std::strtol(s.c_str(), nullptr, 10); }

        void build_mesh(const Node& mesh, const Mat& world, Ctx& ctx)
        {
            const auto& w = mesh.words;
            size_t p = 0;
            if (w.empty()) return;
            size_t nv = (size_t)inum(w[p++]);
            if (w.size() < p + nv * 3 + 1) throw std::runtime_error("truncated Mesh vertex list");

            std::vector<XVertex> verts(nv);
            for (size_t i = 0; i < nv; i++)
            {
                float x = num(w[p++]), y = num(w[p++]), z = num(w[p++]);
                const float* m = world.m;
                verts[i].x = x * m[0] + y * m[4] + z * m[8]  + m[12];
                verts[i].y = x * m[1] + y * m[5] + z * m[9]  + m[13];
                verts[i].z = x * m[2] + y * m[6] + z * m[10] + m[14];
                verts[i].u = verts[i].v = 0;
            }

            size_t nf = (size_t)inum(w[p++]);
            std::vector<std::vector<unsigned int>> faces;
            faces.reserve(nf);
            for (size_t f = 0; f < nf; f++)
            {
                if (p >= w.size()) throw std::runtime_error("truncated Mesh face list");
                size_t cnt = (size_t)inum(w[p++]);
                if (w.size() < p + cnt) throw std::runtime_error("truncated Mesh face list");
                std::vector<unsigned int> idx(cnt);
                for (size_t k = 0; k < cnt; k++)
                {
                    long v = inum(w[p++]);
                    if (v < 0 || (size_t)v >= nv) throw std::runtime_error("face index out of range");
                    idx[k] = (unsigned int)v;
                }
                faces.push_back(std::move(idx));
            }

            std::vector<unsigned int> face_material(nf, 0);
            std::vector<std::string> material_textures(1, "");

            for (auto& c : mesh.children)
            {
                const auto& cw = c->words;
                if (c->type == "MeshTextureCoords" && !cw.empty())
                {
                    size_t n = (size_t)inum(cw[0]);
                    for (size_t i = 0; i < n && i < nv && cw.size() >= 1 + (i + 1) * 2; i++)
                    {
                        verts[i].u = num(cw[1 + i * 2]);
                        verts[i].v = num(cw[2 + i * 2]);
                    }
                }
                else if (c->type == "MeshMaterialList" && cw.size() >= 2)
                {
                    size_t nmat = (size_t)inum(cw[0]);
                    size_t nidx = (size_t)inum(cw[1]);
                    for (size_t i = 0; i < nidx && i < nf && 2 + i < cw.size(); i++)
                        face_material[i] = (unsigned int)inum(cw[2 + i]);

                    material_textures.assign(nmat ? nmat : 1, "");
                    size_t m = 0;
                    for (auto& mc : c->children)
                    {
                        if (m >= material_textures.size()) break;
                        if (mc->type == "Material")
                            material_textures[m++] = texture_of(*mc);
                        else if (mc->type == "ref")
                        {
                            std::string name = mc->words.empty() ? "" : mc->words[0];
                            auto it = ctx.named.find(name);
                            material_textures[m++] = (it != ctx.named.end()) ? it->second.texture : "";
                        }
                    }
                }
            }

            std::map<unsigned int, size_t> part_of_material;
            for (size_t f = 0; f < nf; f++)
            {
                unsigned int mat = face_material[f];
                if (part_of_material.count(mat)) continue;
                part_of_material[mat] = ctx.out->parts.size();
                XPart part;
                part.texture = mat < material_textures.size() ? material_textures[mat] : "";
                ctx.out->parts.push_back(std::move(part));
            }
            std::map<unsigned int, std::map<unsigned int, unsigned int>> vmap;
            for (size_t f = 0; f < nf; f++)
            {
                unsigned int mat = face_material[f];
                XPart& part = ctx.out->parts[part_of_material[mat]];
                auto& m = vmap[mat];
                const auto& idx = faces[f];
                if (idx.size() < 3) continue;

                auto get = [&](unsigned int src) -> unsigned int
                {
                    auto it = m.find(src);
                    if (it != m.end()) return it->second;
                    unsigned int ni = (unsigned int)part.vertices.size();
                    part.vertices.push_back(verts[src]);
                    m[src] = ni;
                    return ni;
                };
                for (size_t k = 1; k + 1 < idx.size(); k++)
                {
                    part.indices.push_back(get(idx[0]));
                    part.indices.push_back(get(idx[k]));
                    part.indices.push_back(get(idx[k + 1]));
                }
            }
        }

        void walk(const Node& n, const Mat& parent, Ctx& ctx)
        {
            Mat local = identity();
            for (auto& c : n.children)
            {
                if (c->type == "FrameTransformMatrix" && c->words.size() >= 16)
                    for (int i = 0; i < 16; i++) local.m[i] = num(c->words[(size_t)i]);
            }
            Mat world = (n.type == "Frame") ? mul(local, parent) : parent;

            for (auto& c : n.children)
            {
                if (c->type == "Mesh") build_mesh(*c, world, ctx);
                else if (c->type == "Frame") walk(*c, world, ctx);
            }
        }
    }

    bool load_xmesh(const std::string& path, XMesh& out, std::string& error)
    {
        std::ifstream f(path, std::ios::binary);
        if (!f) { error = "cannot open file"; return false; }
        std::stringstream ss;
        ss << f.rdbuf();
        std::string s = ss.str();

        if (s.compare(0, 4, "xof ") != 0) { error = "not a DirectX .x file"; return false; }
        if (s.size() < 16 || s.compare(8, 3, "txt") != 0)
        {
            error = "binary .x not supported";
            return false;
        }

        try
        {
            out = XMesh{};
            std::vector<Token> t = tokenize(s, 16);

            Node root;
            root.type = "root";
            size_t i = 0;
            while (i < t.size())
            {
                if (t[i].type == Tok::Word && t[i].text == "template")
                {

                    while (i < t.size() && t[i].type != Tok::Open) i++;
                    int depth = 0;
                    for (; i < t.size(); i++)
                    {
                        if (t[i].type == Tok::Open) depth++;
                        else if (t[i].type == Tok::Close && --depth == 0) { i++; break; }
                    }
                    continue;
                }
                if (t[i].type == Tok::Word && !is_number(t[i].text))
                {
                    std::string ty = t[i].text, nm;
                    size_t j = i + 1;
                    if (j < t.size() && t[j].type == Tok::Word) { nm = t[j].text; j++; }
                    if (j < t.size() && t[j].type == Tok::Open)
                    {
                        i = j + 1;
                        root.children.push_back(parse_node(t, i, ty, nm));
                        continue;
                    }
                }
                i++;
            }

            Ctx ctx;
            ctx.out = &out;
            collect_materials(root, ctx);
            walk(root, identity(), ctx);
        }
        catch (const std::exception& e)
        {
            error = e.what();
            return false;
        }
        return true;
    }
}
