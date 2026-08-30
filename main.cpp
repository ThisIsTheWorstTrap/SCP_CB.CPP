#include "engine/renderer/Renderer.hpp"
#include "engine/input/Input.hpp"

int main()
{
    auto renderer = CreateRenderer();
    auto input = CreateInput();

    renderer->init_window(1280, 720, "SCP Remake");

    Engine::ModelHandle mod = renderer->load_model("../models/zombiesurgeon.glb");
    Engine::Coordinates mod_pos(0.0f, 0.0f, 0.0f);

    Engine::Coordinates camera_position(0.0f, 2.0f, 0.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;

    const float move_speed = 5.0f;
    const float mouse_sensitivity = 0.003f;

    bool cursor_currently_hidden = true;
    input->hide_cursor();

    while (!renderer->window_should_close())
    {
        float delta_time = renderer->get_delta_time();

        if (input->is_key_pressed(Key::Escape))
        {
            input->hide_cursor();
            cursor_currently_hidden = !cursor_currently_hidden;
        }

        if (cursor_currently_hidden)
        {
            Engine::Coordinates2d mouse_delta = input->get_mouse_delta();
            yaw   -= mouse_delta.get_x() * mouse_sensitivity;
            pitch -= mouse_delta.get_y() * mouse_sensitivity;

            if (pitch > 1.5f) pitch = 1.5f;
            if (pitch < -1.5f) pitch = -1.5f;
        }

        Engine::Coordinates forward(
            cosf(pitch) * sinf(yaw),
            sinf(pitch),
            cosf(pitch) * cosf(yaw)
        );

        Engine::Coordinates up(0.0f, 1.0f, 0.0f);
        Engine::Coordinates right = forward.cross(up).normalized();

        if (input->is_key_down(Key::W)) camera_position += forward * (move_speed * delta_time);
        if (input->is_key_down(Key::S)) camera_position += forward * (-move_speed * delta_time);
        if (input->is_key_down(Key::D)) camera_position += right * (move_speed * delta_time);
        if (input->is_key_down(Key::A)) camera_position += right * (-move_speed * delta_time);

        renderer->set_camera_position(camera_position);
        renderer->set_camera_target(camera_position + forward);

        renderer->begin_frame();
        renderer->draw_model(mod, mod_pos, 0.1f);
        renderer->end_frame();
    }

    renderer->close_window();

    return 0;
}