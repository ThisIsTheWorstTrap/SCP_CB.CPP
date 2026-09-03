#include "model_viewer.hpp"
#include <string>
#include <iostream>

#define RAYGUI_IMPLEMENTATION
#include "lib/raygui.h"

ModelViewer::ModelViewer(std::string path)
{
    this->renderer = CreateRenderer();
    this->input = CreateInput();
    renderer->init_window(1280, 720, "SCP Remake");
    load_models(path);
}

void ModelViewer::load_models(std::string models_folder_path)
{
    std::string model_names[] = {
        "035",
        "035tentacle",
        "173_2"
        "scp-049"
    };

    for (int i=0; i<__MODEL_LAST; i++)
    {
        std::string model_path = models_folder_path + model_names[i] + ".glb";
        int anim_num = 0;
        renderer->load_model_anims(model_path.c_str(), (ModelsEnum)i, &anim_num);
        anims_count_from_model[i] = anim_num;
    }
}

void ModelViewer::add_model(ModelsEnum model)
{
    renderer->add_model_scene((int)model, Engine::Coordinates(0, 0, 0));
}

void ModelViewer::run()
{
    Engine::Coordinates camera_position(0.0f, 2.0f, 5.0f);
    float yaw = 3.14159265f; // face -Z
    float pitch = 0.0f;

    const float move_speed = 10.0f;
    const float mouse_sensitivity = 0.003f;

    bool cursor_currently_hidden = true;
    input->switch_cursor();

    bool camera_fitted = false;

    bool drop_down_edit_mode = false;
    int drop_down_active = 0;

    while (!renderer->window_should_close())
    {
        float delta_time = renderer->get_delta_time();

        
        if (GuiDropdownBox((Rectangle){ 100, 80, 200, 30 },
                           "MODEL_035;MODEL_035_TENT;MODEL_173_2",
                           &drop_down_active, drop_down_edit_mode))
        {
            drop_down_edit_mode = !drop_down_edit_mode;
            camera_fitted = !camera_fitted;
        }
        

        if (input->is_mouse_button_down(Engine::MouseButton::Right))
        {
            cursor_currently_hidden = !cursor_currently_hidden;
            input->switch_cursor();
        }

        if (cursor_currently_hidden)
        {
            Engine::Coordinates2d mouse_delta =
                input->get_mouse_delta();

            yaw   -= mouse_delta.get_x() * mouse_sensitivity;
            pitch -= mouse_delta.get_y() * mouse_sensitivity;

            if (pitch > 1.5f)
                pitch = 1.5f;

            if (pitch < -1.5f)
                pitch = -1.5f;
        }

        if (!camera_fitted)
        {
            float model_height =
                renderer->get_model_height(drop_down_active);

            if (model_height > 0.0f)
            {
                const float fovy = 45.0f;
                const float fov_rad =
                    fovy * (3.14159265f / 180.0f);

                float distance =
                    (model_height * 0.5f) /
                    tanf(fov_rad * 0.5f);

                distance *= 1.3f;

                float center = model_height * 0.5f;

                camera_position =
                    Engine::Coordinates(
                        0.0f,
                        center,
                        distance
                    );

                yaw = 3.14159265f;
                pitch = 0.0f;

                camera_fitted = true;
            }
        }

        Engine::Coordinates forward(
            cosf(pitch) * sinf(yaw),
            sinf(pitch),
            cosf(pitch) * cosf(yaw)
        );

        Engine::Coordinates up(0.0f, 1.0f, 0.0f);

        Engine::Coordinates right =
            forward.cross(up).normalized();

        if (input->is_key_down(Key::W)) camera_position += forward * (move_speed * delta_time);
        if (input->is_key_down(Key::S)) camera_position += forward * (-move_speed * delta_time);
        if (input->is_key_down(Key::D)) camera_position += right * (move_speed * delta_time);
        if (input->is_key_down(Key::A)) camera_position += right * (-move_speed * delta_time);

        renderer->set_camera_position(camera_position);
        renderer->set_camera_target(
            camera_position + forward
        );

        renderer->begin_frame();
        add_model((ModelsEnum)drop_down_active);
        renderer->end_frame();
    }
}