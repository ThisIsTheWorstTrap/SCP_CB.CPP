#ifndef MODEL_VIEWER_HPP
#define MODEL_VIEWER_HPP

#include "engine/engine.hpp"
#include <unordered_map>

class ModelViewer
{
    public:
        ModelViewer(std::string path);

        std::string transform_vector_of_models(std::string models_folder_path);

        void load_models(std::string path);
        void add_model(int model);
        void run_animation(int model_id, int anim_num, int frame);
        
        void run(std::string path);

    private:
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Input> input;
};

#endif