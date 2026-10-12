#include "engine/assets/RoomLoader.hpp"
#include "engine/assets/AssetResolver.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <set>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    fs::path target = "GFX/map";
    bool print_files = false;
    for (int i = 1; i < argc; i++)
    {
        std::string a = argv[i];
        if (a == "--files") print_files = true;
        else target = a;
    }
    std::set<std::string> used;

    std::vector<fs::path> files;
    std::error_code ec;
    if (fs::is_regular_file(target, ec))
        files.push_back(target);
    else if (fs::is_directory(target, ec))
    {
        for (const auto& e : fs::recursive_directory_iterator(target))
            if (e.is_regular_file() && Engine::to_lower(e.path().extension().string()) == ".rmesh")
                files.push_back(e.path());
    }
    else
    {
        std::cerr << "Not found: " << target << "\n";
        std::cerr << "Usage: rmesh_check <folder or file.rmesh>\n";
        return 2;
    }
    std::sort(files.begin(), files.end());

    int failed = 0;
    std::map<std::string, int> all_warnings;
    size_t total_v = 0, total_t = 0;

    for (const auto& f : files)
    {
        Engine::StaticModel m;
        std::string err;
        if (!Engine::assets::load_room_model(f.string(), m, err))
        {
            std::cout << "FAIL  " << f.filename().string() << " : " << err << "\n";
            failed++;
            continue;
        }
        size_t v = 0, t = 0, alpha = 0, lm = 0;
        std::set<std::string> tex;
        for (const auto& p : m.parts)
        {
            v += p.vertices.size();
            t += p.indices.size() / 3;
            if (p.alpha) alpha++;
            if (!p.lightmap_path.empty()) lm++;
            if (!p.diffuse_path.empty()) tex.insert(p.diffuse_path);
        }
        total_v += v; total_t += t;
        std::cout << "ok    " << f.filename().string() << " : " << m.parts.size() << " parts ("
                  << lm << " lightmapped, " << alpha << " alpha), " << v << " vertices, "
                  << t << " triangles, " << tex.size() << " textures, "
                  << m.warnings.size() << " warnings, size "
                  << (m.bounds_max.get_x() - m.bounds_min.get_x()) << " x "
                  << (m.bounds_max.get_y() - m.bounds_min.get_y()) << " x "
                  << (m.bounds_max.get_z() - m.bounds_min.get_z()) << "\n";
        for (const auto& w : m.warnings) all_warnings[w]++;
        used.insert(m.used_files.begin(), m.used_files.end());
    }

    std::cout << "\n" << (files.size() - failed) << "/" << files.size() << " rooms loaded, "
              << total_v << " vertices, " << total_t << " triangles\n";
    if (!all_warnings.empty())
    {
        std::cout << "\nMissing files (" << all_warnings.size() << "):\n";
        for (const auto& [w, n] : all_warnings)
            std::cout << "  " << w << "   [" << n << " room(s)]\n";
    }
    if (print_files)
    {
        std::error_code sec;
        uintmax_t bytes = 0;
        std::cout << "\nFiles used (" << used.size() << "):\n";
        for (const auto& u : used)
        {
            std::cout << "  " << u << "\n";
            bytes += fs::file_size(u, sec);
        }
        std::cout << "total: " << bytes / (1024 * 1024) << " MB\n";
    }
    return failed ? 1 : 0;
}
