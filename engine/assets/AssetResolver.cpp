#include "AssetResolver.hpp"
#include <algorithm>
#include <cctype>
#include <system_error>

namespace fs = std::filesystem;

namespace Engine
{
    std::string to_lower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return (char)std::tolower(c); });
        return s;
    }

    static std::vector<std::string> split_path(const std::string& name)
    {
        std::vector<std::string> parts;
        std::string cur;
        for (char c : name)
        {
            if (c == '\\' || c == '/')
            {
                if (!cur.empty() && cur != ".") parts.push_back(cur);
                cur.clear();
            }
            else cur += c;
        }
        if (!cur.empty() && cur != ".") parts.push_back(cur);
        return parts;
    }

    AssetResolver::AssetResolver(std::vector<fs::path> search_dirs)
        : dirs(std::move(search_dirs))
    {
    }

    const AssetResolver::DirIndex& AssetResolver::index_of(const fs::path& dir)
    {
        std::string key = dir.string();
        auto it = dir_cache.find(key);
        if (it != dir_cache.end()) return it->second;

        DirIndex index;
        std::error_code ec;
        for (fs::directory_iterator d(dir, ec), end; !ec && d != end; d.increment(ec))
        {
            std::string real = d->path().filename().string();
            index.emplace(to_lower(real), real);
        }
        return dir_cache.emplace(key, std::move(index)).first->second;
    }

    std::string AssetResolver::find_in_dir(const fs::path& dir, const std::string& name)
    {
        std::vector<std::string> parts = split_path(name);
        if (parts.empty()) return "";

        fs::path current = dir;
        for (size_t i = 0; i < parts.size(); i++)
        {
            if (parts[i] == "..")
            {
                current = current.parent_path();
                continue;
            }
            const DirIndex& index = index_of(current);
            bool last = (i + 1 == parts.size());

            auto it = index.find(to_lower(parts[i]));
            if (it != index.end())
            {
                current /= it->second;
                continue;
            }
            if (!last) return "";

            std::string stem = to_lower(fs::path(parts[i]).stem().string());
            static const char* exts[] = { ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".dds" };
            for (const char* e : exts)
            {
                auto f = index.find(stem + e);
                if (f != index.end())
                {
                    current /= f->second;
                    return current.string();
                }
            }
            return "";
        }

        std::error_code ec;
        if (fs::is_regular_file(current, ec)) return current.string();
        return "";
    }

    std::string AssetResolver::find_from(const fs::path& first_dir, const std::string& name)
    {
        if (name.empty()) return "";

        std::string r = find_in_dir(first_dir, name);
        if (!r.empty()) return r;

        for (const fs::path& d : dirs)
        {
            r = find_in_dir(d, name);
            if (!r.empty()) return r;
        }

        std::vector<std::string> parts = split_path(name);
        if (parts.size() > 1)
        {
            const std::string& base = parts.back();
            r = find_in_dir(first_dir, base);
            if (!r.empty()) return r;
            for (const fs::path& d : dirs)
            {
                r = find_in_dir(d, base);
                if (!r.empty()) return r;
            }
        }
        return "";
    }

    std::string AssetResolver::find(const std::string& name)
    {
        if (dirs.empty()) return "";
        return find_from(dirs.front(), name);
    }

    fs::path AssetResolver::find_gfx_root(const fs::path& start)
    {
        std::error_code ec;
        fs::path p = fs::absolute(start, ec);
        if (ec) p = start;
        for (fs::path cur = p; !cur.empty(); cur = cur.parent_path())
        {
            if (to_lower(cur.filename().string()) == "gfx") return cur;
            if (cur == cur.parent_path()) break;
        }
        return start;
    }
}
