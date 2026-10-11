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

        void load_model_anims(const char* path, std::string id, int* anim_count) override;
        void add_model_scene(std::string model_id, Engine::Coordinates position, float angle) override;

        float get_model_height(std::string model_id) override;
        float get_model_width(std::string model_id) override;

        void play_selected_animation(std::string model_id, int anim_num, int frame) override;
        std::string get_animations_name(std::string model_id) override;
        int get_animations_num(std::string model_id) override;

        bool check_overlaping(std::string model_id1, std::string model_id2) override;

        void draw_grid() override;

    private:
        Camera3D camera;
        std::unordered_map<std::string, Model> models;
        std::unordered_map<std::string, ModelAnimation*> model_animations;
        std::unordered_map<std::string, int> anims_count_from_model;
};

#endif