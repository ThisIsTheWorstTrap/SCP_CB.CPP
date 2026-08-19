#ifndef RAYLIB_INPUT_HPP
#define RAYLIB_INPUT_HPP

#include "Input.hpp"
#include "Key.hpp"
#include "engine/core/math/math.hpp"

class RaylibInput : public Input
{
    public:
        RaylibInput();

        void update() override;

        bool is_key_down(Key key) override;
        bool is_key_pressed(Key key) override;

        bool is_mouse_button_down(Engine::MouseButton button) override;
        Engine::Coordinates2d get_mouse_delta() override;

        void hide_cursor() override;

    private:
        static int to_raylib_key(Key key);
        static int to_raylib_mouse_button(Engine::MouseButton button);

        bool cursor_hidden;
};

#endif