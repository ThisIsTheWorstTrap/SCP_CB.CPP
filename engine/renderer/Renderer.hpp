#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "engine/core/math/math.hpp"
#include "color.hpp"
#include <memory>
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

        virtual void load_model(const char* path, int id) = 0;
        virtual void add_model_scene(int model_id, Engine::Coordinates position) = 0;

        virtual float get_model_height(int model_id) = 0;
};

std::unique_ptr<Renderer> CreateRenderer();

#endif