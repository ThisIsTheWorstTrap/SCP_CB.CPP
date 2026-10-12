#include "model_viewer.hpp"
#include "engine/assets/AssetResolver.hpp"
#include "engine/assets/RoomLoader.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include "lib/raygui.h"

namespace fs = std::filesystem;

ModelViewer::ModelViewer(std::string path)
{
    this->renderer = CreateRenderer();
    this->input = CreateInput();

    scan_folder(path);
    if (entries.empty())
    {
        std::cerr << "No .rmesh or .m3d file in " << path << "\n";
        return;
    }

    renderer->init_window(1280, 720, "SCP Remake");
    run(path);
}

void ModelViewer::scan_folder(const std::string& folder)
{
    std::error_code ec;
    if (!fs::is_directory(folder, ec)) return;

    fs::path root(folder);
    for (const auto& entry : fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied, ec))
    {
        if (!entry.is_regular_file()) continue;
        std::string ext = Engine::to_lower(entry.path().extension().string());
        if (ext != ".m3d" && ext != ".rmesh") continue;

        Entry e;
        e.path = entry.path().string();
        e.label = fs::relative(entry.path(), root, ec).generic_string();
        e.is_room = (ext == ".rmesh");

        std::replace(e.label.begin(), e.label.end(), ';', '_');
        entries.push_back(std::move(e));
    }
    std::sort(entries.begin(), entries.end(),
              [](const Entry& a, const Entry& b) { return a.label < b.label; });

    size_t n = entries.size();
    loaded.assign(n, false);
    load_errors.assign(n, "");
    info.assign(n, "");
    room_min.assign(n, Engine::Coordinates());
    room_max.assign(n, Engine::Coordinates());
    room_start.assign(n, Engine::Coordinates());
    room_has_start.assign(n, false);
}

bool ModelViewer::load_entry(int index)
{
    if (index < 0 || index >= (int)entries.size()) return false;
    const Entry& e = entries[index];

    if (!e.is_room)
    {
        if (!loaded[index])
        {
            int anim_num = 0;
            renderer->load_model_anims(e.path.c_str(), index, &anim_num);
            loaded[index] = true;
        }
        return true;
    }

    if (loaded_room != index && loaded_room >= 0)
    {
        renderer->unload_static_model(loaded_room);
        loaded[loaded_room] = false;
        loaded_room = -1;
    }
    if (loaded[index]) return true;
    if (!load_errors[index].empty()) return false;

    Engine::StaticModel model;
    std::string error;
    if (!Engine::assets::load_room_model(e.path, model, error))
    {
        load_errors[index] = error;
        info[index] = "ERROR: " + error;
        std::cerr << e.label << ": " << error << "\n";
        return false;
    }
    for (const std::string& w : model.warnings)
        std::cerr << e.label << ": " << w << "\n";

    size_t verts = 0, tris = 0;
    for (const auto& p : model.parts) { verts += p.vertices.size(); tris += p.indices.size() / 3; }

    if (!renderer->load_static_model(index, model))
    {
        load_errors[index] = "nothing to draw";
        info[index] = "ERROR: nothing to draw";
        return false;
    }

    room_min[index] = model.bounds_min;
    room_max[index] = model.bounds_max;
    room_has_start[index] = model.has_player_start;
    room_start[index] = model.player_start;
    info[index] = e.label + " : " + std::to_string(model.parts.size()) + " parts, " +
                  std::to_string(verts) + " vertices, " + std::to_string(tris) + " triangles" +
                  (model.warnings.empty() ? "" : ", " + std::to_string(model.warnings.size()) + " missing files");
    loaded[index] = true;
    loaded_room = index;
    return true;
}

void ModelViewer::add_model(int index)
{
    if (index < 0 || index >= (int)entries.size() || !loaded[index]) return;

    if (entries[index].is_room)
        renderer->draw_static_model(index, Engine::Coordinates(0, 0, 0));
    else
        renderer->add_model_scene(index, Engine::Coordinates(0, 0, 0));
}

void ModelViewer::run(std::string )
{
    Engine::Coordinates camera_position(0.0f, 2.0f, 5.0f);
    float yaw = 3.14159265f;
    float pitch = 0.0f;

    float move_speed = 10.0f;
    const float mouse_sensitivity = 0.003f;

    bool cursor_currently_hidden = true;
    input->switch_cursor();

    bool camera_fitted = false;

    std::string entry_names;
    for (size_t i = 0; i < entries.size(); i++)
        entry_names += entries[i].label + (i + 1 < entries.size() ? ";" : "");
    int list_scroll = 0;
    int list_active = 0;
    int selected = -1;

    bool anim_drop_down_edit_mode = false;
    int anim_drop_down_active = 0;

    bool is_anim_active = true;
    int frame = 0;

    bool lightmaps = true;
    float exposure = 1.0f;

    while (!renderer->window_should_close())
    {
        float delta_time = renderer->get_delta_time();

        if (list_active != selected)
        {
            selected = list_active;
            anim_drop_down_active = 0;
            frame = 0;
            camera_fitted = false;
            load_entry(selected);
            renderer->set_clear_color(entries[selected].is_room ? Engine::Color{ 25, 25, 28, 255 }
                                                                  : Engine::Color{ 255, 255, 255, 255 });
        }
        const bool is_room = entries[selected].is_room;

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

        if (!camera_fitted && loaded[selected])
        {
            if (is_room)
            {

                const Engine::Coordinates& mn = room_min[selected];
                const Engine::Coordinates& mx = room_max[selected];
                float cx = (mn.get_x() + mx.get_x()) * 0.5f;
                float cz = (mn.get_z() + mx.get_z()) * 0.5f;
                float height = mx.get_y() - mn.get_y();
                float extent = std::max(mx.get_x() - mn.get_x(), mx.get_z() - mn.get_z());

                camera_position = Engine::Coordinates(cx, mn.get_y() + height * 0.35f, cz);
                move_speed = std::max(1.0f, extent * 0.25f);
                yaw = 3.14159265f;
                pitch = 0.0f;
                camera_fitted = true;
            }
            else
            {
                float model_height =
                    renderer->get_model_height(selected);

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

                    move_speed = 10.0f;
                    yaw = 3.14159265f;
                    pitch = 0.0f;

                    camera_fitted = true;
                }
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

        float speed = move_speed * (input->is_key_down(Key::LeftShift) ? 3.0f : 1.0f);
        if (input->is_key_down(Key::W)) camera_position += forward * (speed * delta_time);
        if (input->is_key_down(Key::S)) camera_position += forward * (-speed * delta_time);
        if (input->is_key_down(Key::D)) camera_position += right * (speed * delta_time);
        if (input->is_key_down(Key::A)) camera_position += right * (-speed * delta_time);

        renderer->set_camera_position(camera_position);
        renderer->set_camera_target(
            camera_position + forward
        );
        renderer->set_lightmaps_enabled(lightmaps);
        renderer->set_exposure(exposure);

        renderer->begin_frame();
        add_model(selected);
        if (!is_room && loaded[selected] && is_anim_active && renderer->get_animations_num(selected) > 0)
        {
            frame += 1;
            renderer->play_selected_animation(selected, anim_drop_down_active, frame);
        }
        renderer->end_scene_3d();

        GuiListView(Rectangle{ 10, 10, 300, 660 }, entry_names.c_str(), &list_scroll, &list_active);
        GuiStatusBar(Rectangle{ 10, 680, 1260, 30 },
                     info[selected].empty() ? "Right click: mouse / cursor, WASD: move, Shift: fast"
                                            : info[selected].c_str());

        if (is_room)
        {
            GuiCheckBox(Rectangle{ 1000, 20, 20, 20 }, "Lightmaps", &lightmaps);
            GuiSliderBar(Rectangle{ 1060, 50, 150, 20 }, "Exposure ", TextFormat("%.1f", exposure), &exposure, 0.25f, 6.0f);
        }
        else
        {
            std::string anims_name = renderer->get_animations_name(selected);
            if (GuiDropdownBox(Rectangle{ 1000, 80, 200, 30 },
                               anims_name.c_str(),
                               &anim_drop_down_active, anim_drop_down_edit_mode))
            {
                anim_drop_down_edit_mode = !anim_drop_down_edit_mode;
            }
        }

        renderer->end_frame();
    }

    renderer->close_window();
}
