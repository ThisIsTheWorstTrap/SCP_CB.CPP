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
    for (Texture2D& texture : textures)
        UnloadTexture(texture);

    for (Model& model : models)
        UnloadModel(model);

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

void RaylibRenderer::draw_cube(Engine::Coordinates position, float size, Engine::Color color)
{
    Vector3 pos = { position.get_x(), position.get_y(), position.get_z() };
    ::Color rl_color = { color.r, color.g, color.b, color.a };

    ::DrawCube(pos, size, size, size, rl_color);
}

Engine::TextureHandle RaylibRenderer::load_texture(const char* path)
{
    Texture2D texture = ::LoadTexture(path);
    textures.push_back(texture);

    std::int32_t id = static_cast<std::int32_t>(textures.size() - 1);
    return Engine::TextureHandle(id);
}

Engine::ModelHandle RaylibRenderer::load_model(const char* path)
{
    Model model = ::LoadModel(path);
    models.push_back(model);

    std::int32_t id = static_cast<std::int32_t>(models.size() - 1);
    return Engine::ModelHandle(id);
}

void RaylibRenderer::draw_model(Engine::ModelHandle handle, Engine::Coordinates position, float scale)
{
    if (!handle.is_valid())
        return;

    Model& model = models[handle.get_id()];
    Vector3 pos = { position.get_x(), position.get_y(), position.get_z() };

    ::DrawModel(model, pos, scale, WHITE);
}

void RaylibRenderer::set_model_texture(Engine::ModelHandle model_handle, Engine::TextureHandle texture_handle)
{
    if (!model_handle.is_valid() || !texture_handle.is_valid())
        return;

    Model& model = models[model_handle.get_id()];
    Texture2D& texture = textures[texture_handle.get_id()];

    SetMaterialTexture(&model.materials[0], MATERIAL_MAP_DIFFUSE, texture);
}