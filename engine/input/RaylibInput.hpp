#ifndef RAYLIB_INPUT_HPP
#define RAYLIB_INPUT_HPP

#include "Input.hpp"
#include "key.hpp"
#include "engine/core/math/math.hpp"

class RaylibInput : public Input
{
    public:
        void update() override;

        bool is_key_down(Key key) override;
        bool is_key_pressed(Key key) override;

        bool is_mouse_button_down(MouseButton button) override;
        Coordinates2d get_mouse_delta() override;
        
        void hide_cursor() override;

    private:
        static int to_raylib_key(Key key);
        static int to_raylib_mouse_button(MouseButton button);

        bool cursor_hidden;
}

#endif