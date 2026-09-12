#ifndef RAYLIB_RENDERER_HPP
#define RAYLIB_RENDERER_HPP

#include "Renderer.hpp"
#include <raylib.h>
#include <unordered_map>
#include <vector>
#include <string>

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

        std::vector<std::string> get_models_from_folder(std::string folder) override;

        void load_model_anims(const char* path, int id, int* anim_count) override;
        void add_model_scene(int model_id, Engine::Coordinates position) override;

        float get_model_height(int model_id) override;

        void play_selected_animation(int model_id, int anim_num, int frame) override;
        std::string get_animations_name(int model_id) override;
        int get_animations_num(int model_id) override;

    private:
        Camera3D camera;
        std::unordered_map<int, Model> models;
        std::unordered_map<int, ModelAnimation*> model_animations;
        std::unordered_map<int, int> anims_count_from_model;
};

#endif