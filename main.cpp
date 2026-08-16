#include "engine/renderer/renderer.hpp"

int main()
{
    Renderer* renderer = CreateRenderer();
    renderer->init_window(1280, 720, "SCP Remake");

    Engine::ModelHandle mod = renderer->load_model("../models/surgeonzombie.glb");
    Engine::Coordinates mod_pos(0.0f, 0.0f, 0.0f);

    while (!renderer->window_should_close())
    {
        renderer->begin_frame();
        renderer->draw_model(mod, mod_pos, 0.1f);
        renderer->end_frame();
    }

    renderer->close_window();
    delete renderer;

    return 0;
}