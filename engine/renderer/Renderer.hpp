#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "engine/core/math/math.hpp"
#include "color.hpp"
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

        virtual void set_camera_position(Engine::Coordinates position) = 0;
        virtual void set_camera_target(Engine::Coordinates target) = 0;
        
        virtual std::vector<std::string> get_models_from_folder(std::string folder) = 0;

        virtual void load_model_anims(const char* path, std::string id, int* anim_count) = 0;
        virtual void add_model_scene(std::string model_id, Engine::Coordinates position, float angle) = 0;

        virtual float get_model_height(std::string model_id) = 0;
        virtual float get_model_width(std::string model_id) = 0;

        virtual void play_selected_animation(std::string model_id, int anim_num, int frame) = 0;
        virtual std::string get_animations_name(std::string model_id) = 0;
        virtual int get_animations_num(std::string model_id) = 0;

        virtual bool check_overlaping(std::string model_id1, std::string model_id2) = 0;

        virtual void draw_grid() = 0;
};

std::unique_ptr<Renderer> CreateRenderer();

#endif