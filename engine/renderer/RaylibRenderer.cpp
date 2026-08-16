#include "RaylibRenderer.hpp"
#include <rlgl.h>

namespace
{
    void draw_textured_cube(Texture2D texture, Vector3 position, float size, ::Color tint)
    {
        float half = size / 2.0f;
        float x = position.x;
        float y = position.y;
        float z = position.z;

        rlSetTexture(texture.id);

        rlBegin(RL_QUADS);
            rlColor4ub(tint.r, tint.g, tint.b, tint.a);

            // Frong
            rlNormal3f(0.0f, 0.0f, 1.0f);
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - half, y - half, z + half);
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + half, y - half, z + half);
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + half, y + half, z + half);
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - half, y + half, z + half);

            // Back
            rlNormal3f(0.0f, 0.0f, -1.0f);
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - half, y - half, z - half);
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - half, y + half, z - half);
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + half, y + half, z - half);
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + half, y - half, z - half);

            // Up
            rlNormal3f(0.0f, 1.0f, 0.0f);
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - half, y + half, z - half);
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - half, y + half, z + half);
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + half, y + half, z + half);
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + half, y + half, z - half);

            // Down
            rlNormal3f(0.0f, -1.0f, 0.0f);
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - half, y - half, z - half);
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + half, y - half, z - half);
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + half, y - half, z + half);
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - half, y - half, z + half);

            // Right
            rlNormal3f(1.0f, 0.0f, 0.0f);
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x + half, y - half, z - half);
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x + half, y + half, z - half);
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x + half, y + half, z + half);
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + half, y - half, z + half);

            // Left
            rlNormal3f(-1.0f, 0.0f, 0.0f);
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - half, y - half, z - half);
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(x - half, y - half, z + half);
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(x - half, y + half, z + half);
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(x - half, y + half, z - half);
        rlEnd();

        rlSetTexture(0);
    }
}

RaylibRenderer::RaylibRenderer() : camera{}
{
}

void RaylibRenderer::init_window(int width, int height, const char* title)
{
    InitWindow(width, height, title);

    camera.position = { 0.0f, 5.0f, 10.0f };
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

void RaylibRenderer::draw_cube_texture(Engine::TextureHandle handle, Engine::Coordinates position, float size, Engine::Color tint)
{
    if (!handle.is_valid())
        return;

    Texture2D& texture = textures[handle.get_id()];
    Vector3 pos = { position.get_x(), position.get_y(), position.get_z() };
    ::Color rl_tint = { tint.r, tint.g, tint.b, tint.a };

    draw_textured_cube(texture, pos, size, rl_tint);
}

Engine::ModelHandle RaylibRenderer::load_model(const char* path)
{
    Model model = ::LoadModel(path);
    models.push_back(model);

    std::int32_t id = static_cast<std::int32_t>(models.size() - 1);
    return Engine::ModelHandle(id);
}

void RaylibRenderer::draw_model(Engine::ModelHandle handle, Engine::Coordinates position)
{
    if (!handle.is_valid())
        return;

    Model& model = models[handle.get_id()];
    Vector3 pos = { position.get_x(), position.get_y(), position.get_z() };

    ::DrawModel(model, pos, 1.0f, WHITE);
}