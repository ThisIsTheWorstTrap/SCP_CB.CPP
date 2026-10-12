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
        void end_scene_3d() override;
        void set_clear_color(Engine::Color color) override;

        void set_camera_position(Engine::Coordinates position) override;
        void set_camera_target(Engine::Coordinates target) override;

        std::vector<std::string> get_models_from_folder(std::string folder) override;

        void load_model_anims(const char* path, int id, int* anim_count) override;
        void add_model_scene(int model_id, Engine::Coordinates position) override;

        float get_model_height(int model_id) override;

        void play_selected_animation(int model_id, int anim_num, int frame) override;
        std::string get_animations_name(int model_id) override;
        int get_animations_num(int model_id) override;

        bool load_static_model(int id, const Engine::StaticModel& model) override;
        void unload_static_model(int id) override;
        void draw_static_model(int id, Engine::Coordinates position) override;
        void set_lightmaps_enabled(bool enabled) override;
        void set_exposure(float new_exposure) override;

    private:
        struct StaticPartGpu
        {
            Mesh mesh;
            Material material;
            bool has_lightmap;
            bool alpha;
        };

        Texture2D get_texture(const std::string& path, bool is_lightmap);
        void ensure_room_shader();

        Camera3D camera;
        bool in_3d = false;
        ::Color clear_color = WHITE;
        std::unordered_map<int, std::vector<StaticPartGpu>> static_models;
        std::unordered_map<std::string, Texture2D> texture_cache;
        Texture2D white_texture{};
        Shader room_shader{};
        bool room_shader_loaded = false;
        int loc_use_lightmap = -1;
        int loc_lightmaps_on = -1;
        bool lightmaps_enabled = true;
        float exposure = 1.0f;
        int loc_exposure = -1;

        std::unordered_map<int, Model> models;
        std::unordered_map<int, ModelAnimation*> model_animations;
        std::unordered_map<int, int> anims_count_from_model;
};

#endif
