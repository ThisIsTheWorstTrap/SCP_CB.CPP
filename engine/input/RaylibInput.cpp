#include "RaylibInput.hpp"
#include <raylib.h>

RaylibInput::RaylibInput()
{
    cursor_hidden = false;
}

void RaylibInput::update()
{
    // nothing now. Possibly needed after
}

bool RaylibInput::is_key_down(Key key)
{
    return IsKeyDown(to_raylib_key(key));
}

bool RaylibInput::is_key_pressed(Key key)
{
    return IsKeyPressed(to_raylib_key(key));
}

bool RaylibInput::is_mouse_button_down(Engine::MouseButton button)
{
    return IsMouseButtonDown(to_raylib_mouse_button(button));
}

Engine::Coordinates2d RaylibInput::get_mouse_delta()
{
    Vector2 delta = GetMouseDelta();
    return Engine::Coordinates2d{delta.x, delta.y};
}

void RaylibInput::hide_cursor()
{
    cursor_hidden = !cursor_hidden;

    if (cursor_hidden)
    {
        DisableCursor();
    }
    else
    {
        EnableCursor();
    }
}

int RaylibInput::to_raylib_key(Key key)
{
    switch (key)
    {
        case Key::W:         return KEY_W;
        case Key::A:         return KEY_A;
        case Key::S:         return KEY_S;
        case Key::D:         return KEY_D;
        case Key::Space:     return KEY_SPACE;
        case Key::LeftShift: return KEY_LEFT_SHIFT;
        case Key::Escape:    return KEY_ESCAPE;
    }

    return KEY_NULL;
}

int RaylibInput::to_raylib_mouse_button(Engine::MouseButton button)
{
    switch (button)
    {
        case Engine::MouseButton::Left:   return MOUSE_BUTTON_LEFT;
        case Engine::MouseButton::Right:  return MOUSE_BUTTON_RIGHT;
        case Engine::MouseButton::Middle: return MOUSE_BUTTON_MIDDLE;
    }

    return MOUSE_BUTTON_LEFT;
}