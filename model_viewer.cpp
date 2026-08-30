#include "model_viewer.hpp"
#include <string>

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
        "1499",
        "173_2",
        "205_demon1",
        "205_demon2",
        "205_demon3",
        "205_woman"
    };

    for (int i=0; i<__MODEL_LAST; i++)
    {
        std::string model_path = models_folder_path + model_names[i] + ".glb";
        renderer->load_model(model_path.c_str(), (ModelsEnum)i);
    }
}

void ModelViewer::add_model(ModelsEnum model)
{
    renderer->add_model_scene((int)model, Engine::Coordinates(0, 0, 0));
}

void ModelViewer::run()
{
    Engine::Coordinates camera_position(0.0f, 2.0f, 0.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;

    const float move_speed = 10.0f;
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
        add_model(MODEL_173_2);
        renderer->end_frame();
    }

    renderer->close_window();
}