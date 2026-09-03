#include "RaylibRenderer.hpp"

RaylibRenderer::RaylibRenderer() : camera{}
{
}

void RaylibRenderer::init_window(int width, int height, const char* title)
{
    InitWindow(width, height, title);

    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target   = { 0.0f, 0.0f, 0.0f };
    camera.up       = { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void RaylibRenderer::close_window()
{
    // TODO unload models and sounds...
    CloseWindow();
}

bool RaylibRenderer::window_should_close()
{
    return WindowShouldClose();
}

float RaylibRenderer::get_delta_time()
{
    return GetFrameTime();
}

void RaylibRenderer::begin_frame()
{
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera); 
}

void RaylibRenderer::end_frame()
{
    EndMode3D();
    EndDrawing();
}

void RaylibRenderer::set_camera_position(Engine::Coordinates position)
{
    camera.position = Vector3{position.get_x(), position.get_y(), position.get_z()};
}

void RaylibRenderer::set_camera_target(Engine::Coordinates target)
{
    camera.target = Vector3{target.get_x(), target.get_y(), target.get_z()};
}

void RaylibRenderer::load_model_anims(const char* path, int model_id, int* anim_count)
{
    Model model = ::LoadModel(path);
    models[model_id] = model;

    ModelAnimation* anim = ::LoadModelAnimations(path, anim_count);
    if (*anim_count > 0) model_animations[model_id] = anim;
}

void RaylibRenderer::add_model_scene(int model_id, Engine::Coordinates position)
{
    Model& model = models[model_id];
    Vector3 pos = { position.get_x(), position.get_y(), position.get_z() };
    ::DrawModel(model, pos, 1.0f, WHITE);
}

float RaylibRenderer::get_model_height(int model_id)
{
    BoundingBox bbox = ::GetModelBoundingBox(models[model_id]);
    return bbox.max.y - bbox.min.y;
}

void RaylibRenderer::play_selected_animation(int model_id, int anim_num, float frame)
{
    ::UpdateModelAnimation(models[model_id], *model_animations[anim_num], frame);
}