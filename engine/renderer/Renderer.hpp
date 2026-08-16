#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "engine/core/math/math.hpp"
#include "engine/core/handle/handle.hpp"
#include "color.hpp"

class Renderer
{
    public:
        virtual ~Renderer()=default;

        virtual void init_window(int width, int height, const char* title)=0;
        virtual void close_window()=0;

        virtual bool window_should_close()=0;
        virtual float get_delta_time()=0;

        virtual void begin_frame()=0;
        virtual void end_frame()=0;

        virtual void draw_cube(Engine::Coordinates position, float size, Engine::Color color)=0;

        virtual Engine::TextureHandle load_texture(const char* path)=0;
        virtual void draw_cube_texture(Engine::TextureHandle handle, Engine::Coordinates position, float size, Engine::Color tint)=0;

        virtual Engine::ModelHandle load_model(const char* path)=0;
        virtual void draw_model(Engine::ModelHandle handle, Engine::Coordinates position, float scale) = 0;
        virtual void set_model_texture(Engine::ModelHandle model_handle, Engine::TextureHandle texture_handle) = 0;
};

Renderer* CreateRenderer();

#endif