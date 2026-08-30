#ifndef INPUT_HPP
#define INPUT_HPP

#include "key.hpp"
#include "engine/core/math/math.hpp"
#include <memory>

class Input
{
    public:
        virtual ~Input() = default;

        virtual void update() = 0;

        virtual bool is_key_down(Key key) = 0;
        virtual bool is_key_pressed(Key key) = 0;

        virtual bool is_mouse_button_down(Engine::MouseButton button) = 0;
        virtual Engine::Coordinates2d get_mouse_delta() = 0;

        virtual void hide_cursor() = 0;
};

std::unique_ptr<Input> CreateInput();

#endif