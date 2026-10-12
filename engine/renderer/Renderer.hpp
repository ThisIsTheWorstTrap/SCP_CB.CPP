#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "engine/core/math/math.hpp"
#include "color.hpp"
#include "engine/assets/StaticModel.hpp"
#include <memory>
#include <vector>
#include <string>

class Renderer
{
    public:
        virtual ~Renderer() = default;

        virtual void init_window(int width, int height, const char* title) = 0;
        virtual void close_window() = 0;

        virtual bool window_should_close() = 0;
        virtual float get_delta_time() = 0;

        virtual void begin_frame() = 0;
        virtual void end_frame() = 0;

        virtual void end_scene_3d() = 0;
        virtual void set_clear_color(Engine::Color color) = 0;

        virtual void set_camera_position(Engine::Coordinates position) = 0;
        virtual void set_camera_target(Engine::Coordinates target) = 0;

        virtual std::vector<std::string> get_models_from_folder(std::string folder) = 0;

        virtual void load_model_anims(const char* path, int id, int* anim_count) = 0;
        virtual void add_model_scene(int model_id, Engine::Coordinates position) = 0;

        virtual float get_model_height(int model_id) = 0;

        virtual void play_selected_animation(int model_id, int anim_num, int frame) = 0;
        virtual std::string get_animations_name(int model_id) = 0;
        virtual int get_animations_num(int model_id) = 0;

        virtual bool load_static_model(int id, const Engine::StaticModel& model) = 0;
        virtual void unload_static_model(int id) = 0;
        virtual void draw_static_model(int id, Engine::Coordinates position) = 0;

        virtual void set_lightmaps_enabled(bool enabled) = 0;

        virtual void set_exposure(float exposure) = 0;
};

std::unique_ptr<Renderer> CreateRenderer();

#endif
