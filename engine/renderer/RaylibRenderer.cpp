#include "RaylibRenderer.hpp"
#include <rlgl.h>
#include <filesystem>

RaylibRenderer::RaylibRenderer() : camera{}
{
}

void RaylibRenderer::init_window(int width, int height, const char* title)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, title);
    SetTargetFPS(60);

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
    std::vector<int> static_ids;
    for (const auto& entry : static_models) static_ids.push_back(entry.first);
    for (int id : static_ids) unload_static_model(id);
    for (const auto& [path, texture] : texture_cache) ::UnloadTexture(texture);
    texture_cache.clear();
    if (white_texture.id != 0) ::UnloadTexture(white_texture);
    if (room_shader_loaded) ::UnloadShader(room_shader);

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
    ::ClearBackground(clear_color);
    ::BeginMode3D(camera);
    in_3d = true;
}

void RaylibRenderer::end_scene_3d()
{
    if (in_3d)
    {
        ::EndMode3D();
        in_3d = false;
    }
}

void RaylibRenderer::end_frame()
{
    end_scene_3d();
    ::EndDrawing();
}

void RaylibRenderer::set_clear_color(Engine::Color color)
{
    clear_color = ::Color{ color.r, color.g, color.b, color.a };
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

static const char* ROOM_VS = R"(#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec2 vertexTexCoord2;
in vec4 vertexColor;
uniform mat4 mvp;
out vec2 fragTexCoord;
out vec2 fragTexCoord2;
out vec4 fragColor;
void main()
{
    fragTexCoord = vertexTexCoord;
    fragTexCoord2 = vertexTexCoord2;
    fragColor = vertexColor;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
)";

static const char* ROOM_FS = R"(#version 330
in vec2 fragTexCoord;
in vec2 fragTexCoord2;
in vec4 fragColor;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform vec4 colDiffuse;
uniform int useLightmap;
uniform int lightmapsOn;
uniform float exposure;
out vec4 finalColor;

const float AMBIENT = 0.12;
const float UNLIT_PROP_LIGHT = 0.7;

void main()
{
    vec4 diffuse = texture(texture0, fragTexCoord);
    if (diffuse.a < 0.01) discard;

    vec3 light = vec3(1.0);
    if (lightmapsOn == 1)
    {
        if (useLightmap == 1)
            light = (fragColor.rgb * AMBIENT * 2.0 + texture(texture1, fragTexCoord2).rgb) * 2.0 * exposure;
        else
            light = vec3(UNLIT_PROP_LIGHT) * min(exposure, 1.5);
    }
    finalColor = vec4(diffuse.rgb * light * colDiffuse.rgb, diffuse.a * colDiffuse.a);
}
)";

void RaylibRenderer::ensure_room_shader()
{
    if (room_shader_loaded) return;

    room_shader = ::LoadShaderFromMemory(ROOM_VS, ROOM_FS);
    room_shader_loaded = true;
    loc_use_lightmap = ::GetShaderLocation(room_shader, "useLightmap");
    loc_lightmaps_on = ::GetShaderLocation(room_shader, "lightmapsOn");
    loc_exposure = ::GetShaderLocation(room_shader, "exposure");

    Image img = ::GenImageColor(1, 1, WHITE);
    white_texture = ::LoadTextureFromImage(img);
    ::UnloadImage(img);
}

Texture2D RaylibRenderer::get_texture(const std::string& path, bool is_lightmap)
{
    if (path.empty()) return white_texture;

    auto it = texture_cache.find(path);
    if (it != texture_cache.end()) return it->second;

    Texture2D tex = ::LoadTexture(path.c_str());
    if (tex.id == 0)
    {
        ::TraceLog(LOG_WARNING, "Cannot load texture %s", path.c_str());
        texture_cache[path] = white_texture;
        return white_texture;
    }
    if (is_lightmap)
    {
        ::SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
        ::SetTextureWrap(tex, TEXTURE_WRAP_CLAMP);
    }
    else
    {
        ::GenTextureMipmaps(&tex);
        ::SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
        ::SetTextureWrap(tex, TEXTURE_WRAP_REPEAT);
    }
    texture_cache[path] = tex;
    return tex;
}

bool RaylibRenderer::load_static_model(int id, const Engine::StaticModel& model)
{
    ensure_room_shader();
    unload_static_model(id);

    std::vector<StaticPartGpu> gpu_parts;
    gpu_parts.reserve(model.parts.size());

    for (const Engine::StaticPart& part : model.parts)
    {
        size_t vcount = part.vertices.size();
        size_t icount = part.indices.size();
        if (vcount == 0 || icount < 3 || vcount > 65535) continue;

        Mesh mesh{};
        mesh.vertexCount = (int)vcount;
        mesh.triangleCount = (int)(icount / 3);
        mesh.vertices   = (float*)::MemAlloc((unsigned int)(vcount * 3 * sizeof(float)));
        mesh.texcoords  = (float*)::MemAlloc((unsigned int)(vcount * 2 * sizeof(float)));
        mesh.texcoords2 = (float*)::MemAlloc((unsigned int)(vcount * 2 * sizeof(float)));
        mesh.colors     = (unsigned char*)::MemAlloc((unsigned int)(vcount * 4));
        mesh.indices    = (unsigned short*)::MemAlloc((unsigned int)(icount * sizeof(unsigned short)));

        for (size_t i = 0; i < vcount; i++)
        {
            const Engine::StaticVertex& v = part.vertices[i];
            mesh.vertices[i * 3 + 0] = v.x;
            mesh.vertices[i * 3 + 1] = v.y;
            mesh.vertices[i * 3 + 2] = v.z;
            mesh.texcoords[i * 2 + 0] = v.u;
            mesh.texcoords[i * 2 + 1] = v.v;
            mesh.texcoords2[i * 2 + 0] = v.lu;
            mesh.texcoords2[i * 2 + 1] = v.lv;
            mesh.colors[i * 4 + 0] = v.r;
            mesh.colors[i * 4 + 1] = v.g;
            mesh.colors[i * 4 + 2] = v.b;
            mesh.colors[i * 4 + 3] = v.a;
        }
        for (size_t i = 0; i < icount; i++) mesh.indices[i] = part.indices[i];

        ::UploadMesh(&mesh, false);

        StaticPartGpu gpu{};
        gpu.mesh = mesh;
        gpu.has_lightmap = !part.lightmap_path.empty();
        gpu.alpha = part.alpha;

        gpu.material = ::LoadMaterialDefault();
        gpu.material.shader = room_shader;
        gpu.material.maps[MATERIAL_MAP_DIFFUSE].texture = get_texture(part.diffuse_path, false);
        gpu.material.maps[MATERIAL_MAP_SPECULAR].texture =
            gpu.has_lightmap ? get_texture(part.lightmap_path, true) : white_texture;

        gpu_parts.push_back(gpu);
    }

    static_models[id] = std::move(gpu_parts);
    return !static_models[id].empty();
}

void RaylibRenderer::unload_static_model(int id)
{
    auto it = static_models.find(id);
    if (it == static_models.end()) return;

    for (StaticPartGpu& p : it->second)
    {
        ::UnloadMesh(p.mesh);

        ::MemFree(p.material.maps);
        p.material.maps = nullptr;
    }
    static_models.erase(it);
}

void RaylibRenderer::set_lightmaps_enabled(bool enabled)
{
    lightmaps_enabled = enabled;
}

void RaylibRenderer::set_exposure(float new_exposure)
{
    exposure = new_exposure;
}

void RaylibRenderer::draw_static_model(int id, Engine::Coordinates position)
{
    auto it = static_models.find(id);
    if (it == static_models.end()) return;

    Matrix transform = { 1.0f, 0.0f, 0.0f, position.get_x(),
                         0.0f, 1.0f, 0.0f, position.get_y(),
                         0.0f, 0.0f, 1.0f, position.get_z(),
                         0.0f, 0.0f, 0.0f, 1.0f };

    int on = lightmaps_enabled ? 1 : 0;
    ::SetShaderValue(room_shader, loc_lightmaps_on, &on, SHADER_UNIFORM_INT);
    ::SetShaderValue(room_shader, loc_exposure, &exposure, SHADER_UNIFORM_FLOAT);

    for (int pass = 0; pass < 2; pass++)
    {
        if (pass == 1)
        {
            ::rlDisableBackfaceCulling();
            ::rlDisableDepthMask();
        }
        for (StaticPartGpu& p : it->second)
        {
            if (p.alpha != (pass == 1)) continue;
            int use = p.has_lightmap ? 1 : 0;
            ::SetShaderValue(room_shader, loc_use_lightmap, &use, SHADER_UNIFORM_INT);
            ::DrawMesh(p.mesh, p.material, transform);
        }
        if (pass == 1)
        {
            ::rlEnableDepthMask();
            ::rlEnableBackfaceCulling();
        }
    }
}
