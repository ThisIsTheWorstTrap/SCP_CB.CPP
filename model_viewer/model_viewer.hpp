#ifndef MODEL_VIEWER_HPP
#define MODEL_VIEWER_HPP

#include "engine/engine.hpp"
#include <memory>
#include <string>
#include <vector>

class ModelViewer
{
    public:
        ModelViewer(std::string path);

        void run(std::string path);

    private:
        struct Entry
        {
            std::string path;
            std::string label;
            bool is_room;
        };

        void scan_folder(const std::string& folder);
        bool load_entry(int index);
        void add_model(int index);

        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Input> input;

        std::vector<Entry> entries;
        std::vector<bool> loaded;
        std::vector<std::string> load_errors;
        std::vector<std::string> info;
        std::vector<Engine::Coordinates> room_min, room_max;
        std::vector<Engine::Coordinates> room_start;
        std::vector<bool> room_has_start;
        int loaded_room = -1;
};

#endif
