#ifndef ASSET_RESOLVER_HPP
#define ASSET_RESOLVER_HPP

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine
{

    class AssetResolver
    {
        public:

            explicit AssetResolver(std::vector<std::filesystem::path> search_dirs);

            std::string find(const std::string& name);

            std::string find_from(const std::filesystem::path& first_dir, const std::string& name);

            static std::filesystem::path find_gfx_root(const std::filesystem::path& start);

        private:
            using DirIndex = std::unordered_map<std::string, std::string>;
            const DirIndex& index_of(const std::filesystem::path& dir);
            std::string find_in_dir(const std::filesystem::path& dir, const std::string& name);

            std::vector<std::filesystem::path> dirs;
            std::unordered_map<std::string, DirIndex> dir_cache;
    };

    std::string to_lower(std::string s);
}

#endif
