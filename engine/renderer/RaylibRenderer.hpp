#ifndef RAYLIB_RENDERER_HPP
#define RAYLIB_RENDERER_HPP

#include "Renderer.hpp"
#include <raylib.h>
#include <unordered_map>

class RaylibRenderer : public Renderer
{
    public:
        RaylibRenderer();

        void init_window(int width, int height, const char* title) override;
        void close_window() override;

        bool window_should_close() override;
        float get_delta_time() override;

        void begin_frame() override;
        void end_frame() override;

        void set_camera_position(Engine::Coordinates position) override;
        void set_camera_target(Engine::Coordinates target) override;

        void load_model(const char* path, int id) override;
        void add_model_scene(int model_id, Engine::Coordinates position) override;

        float get_model_height(int model_id) override;

    private:
        Camera3D camera;
        std::unordered_map<int, Model> models;
        std::unordered_map<int, Texture2D> textures;
};

#endif