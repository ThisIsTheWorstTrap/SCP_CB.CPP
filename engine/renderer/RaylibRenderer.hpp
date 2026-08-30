#ifndef RAYLIB_RENDERER_HPP
#define RAYLIB_RENDERER_HPP

#include "Renderer.hpp"
#include <raylib.h>
#include <vector>

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

        void draw_cube(Engine::Coordinates position, float size, Engine::Color color) override;

        Engine::TextureHandle load_texture(const char* path) override;
        Engine::ModelHandle load_model(const char* path) override;
        void draw_model(Engine::ModelHandle handle, Engine::Coordinates position, float scale) override;
        void set_model_texture(Engine::ModelHandle model_handle, Engine::TextureHandle texture_handle) override;

    private:
        Camera3D camera;
        std::vector<Model> models;
        std::vector<Texture2D> textures;
};

#endif