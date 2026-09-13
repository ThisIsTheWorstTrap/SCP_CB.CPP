#include "RaylibRenderer.hpp"
#include <filesystem>

RaylibRenderer::RaylibRenderer() : camera{}
{
}

void RaylibRenderer::init_window(int width, int height, const char* title)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, title);
    SetTargetFPS(60); // Temporary

    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target   = { 0.0f, 0.0f, 0.0f };
    camera.up       = { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void RaylibRenderer::close_window()
{
    for (const auto& [num, animations]: model_animations)
    {
        ::UnloadModelAnimations(animations, anims_count_from_model[num]);
    }
    for (const auto& [model_id, model]: models)
    {
        ::UnloadModel(model);
    }
    // TODO unload sounds
    ::CloseWindow();
}

bool RaylibRenderer::window_should_close()
{
    return ::WindowShouldClose();
}

float RaylibRenderer::get_delta_time()
{
    return ::GetFrameTime();
}

void RaylibRenderer::begin_frame()
{
    ::BeginDrawing();
    ::ClearBackground(WHITE);
    ::BeginMode3D(camera); 
}

void RaylibRenderer::end_frame()
{
    ::EndMode3D();
    ::EndDrawing();
}

void RaylibRenderer::set_camera_position(Engine::Coordinates position)
{
    camera.position = Vector3{position.get_x(), position.get_y(), position.get_z()};
}

void RaylibRenderer::set_camera_target(Engine::Coordinates target)
{
    camera.target = Vector3{target.get_x(), target.get_y(), target.get_z()};
}

namespace fs = std::filesystem;

std::vector<std::string> RaylibRenderer::get_models_from_folder(std::string folder)
{
    std::vector<std::string> files;
    for (const auto& entry : fs::recursive_directory_iterator(folder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".m3d") {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

void RaylibRenderer::load_model_anims(const char* path, int model_id, int* anim_count)
{
    Model model = ::LoadModel(path);
    models[model_id] = model;
    for (int i = 0; i < model.materialCount; i++)
        model.materials[i].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

    ModelAnimation* anim = ::LoadModelAnimations(path, anim_count);

    if (*anim_count > 0) 
    {
        model_animations[model_id] = anim;
    }
    anims_count_from_model[model_id] = *anim_count;
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

void RaylibRenderer::play_selected_animation(int model_id, int anim_num, int frame)
{
    int model_anim_count = anims_count_from_model[model_id];
    if (model_anim_count > 0 && model_anim_count > anim_num)
    {
        ::UpdateModelAnimation(models[model_id], model_animations[model_id][anim_num], frame);
    }
    else
        ::TraceLog(LOG_ERROR, "You're trying to play a model (%d) that has no animation", model_id);
}

std::string RaylibRenderer::get_animations_name(int model_id)
{
    std::string result;
    for (int i = 0; i < anims_count_from_model[model_id]; i++)
    {
        result += model_animations[model_id][i].name;
        result += ";";
    }
    return result;
}

int RaylibRenderer::get_animations_num(int model_id)
{
    return anims_count_from_model[model_id];
}