#ifndef MODEL_VIEWER_HPP
#define MODEL_VIEWER_HPP

#include "engine/engine.hpp"
#include <unordered_map>

typedef enum
{
    MODEL_SCP_049,
    __MODEL_LAST
} ModelsEnum;

class ModelViewer
{
    public:
        ModelViewer(std::string path);

        void load_models(std::string path);
        void add_model(ModelsEnum model);
        void run_animation(int model_id, int anim_num, int frame);
        
        void run();

    private:
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Input> input;
};

#endif