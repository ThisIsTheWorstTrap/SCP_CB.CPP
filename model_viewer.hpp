#ifndef MODEL_VIEWER_HPP
#define MODEL_VIEWER_HPP

#include "engine/renderer/Renderer.hpp"
#include "engine/input/Input.hpp"
#include <unordered_map>

typedef enum
{
    MODEL_035,
    MODEL_173_2,
    MODEL_SCP_049,
    __MODEL_LAST
} ModelsEnum;

class ModelViewer
{
    public:
        ModelViewer(std::string path);

        void load_models(std::string path);
        void add_model(ModelsEnum model);
        
        void run();

    private:
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Input> input;
        std::unordered_map<int, int> anims_count_from_model;
};

#endif