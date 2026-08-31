#ifndef MODEL_VIEWER_HPP
#define MODEL_VIEWER_HPP

#include "engine/renderer/Renderer.hpp"
#include "engine/input/Input.hpp"

typedef enum
{
    MODEL_035,
    MODEL_035_TENT,
    MODEL_1499,
    MODEL_173_2,
    MODEL_205_DEMON1,
    MODEL_205_DEMON2,
    MODEL_205_DEMON3,
    MODEL_205_WOMAN,
    __MODEL_LAST
} ModelsEnum;

class ModelViewer
{
    public:
        ModelViewer(std::string path);

        void load_models(std::string path);
        void add_model(ModelsEnum model);
        void fit_camera_to_model(int model_id, Engine::Coordinates& out_position, float& out_yaw, float& out_pitch);
        
        void run();

    private:
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Input> input;
};

#endif