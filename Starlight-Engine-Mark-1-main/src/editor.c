// Este projeto é feito por IA e só o prompt é feito por um humano.
#define SDL_MAIN_HANDLED
#include "starlight.h"
#include "ui_layout.h"
#include "components.h"
#include "serialization.h"
#include <stdio.h>

SLF_Font* f_small; // Re-using engine font

// --- Editor State ---
typedef struct {
    ecs_entity_t selected_entity;
    bool show_assets;
    float sidebar_width;
    float asset_height;
    char status_msg[128];
} EditorState;

static EditorState g_editor;

// --- Callbacks ---

void Editor_OnStart(SLF_App* app) {
    printf(">>> STARLIGHT STUDIO (C-NATIVE) EVOLVED <<<\n");
    g_editor.selected_entity = 0;
    g_editor.show_assets = true;
    g_editor.sidebar_width = 320.0f;
    g_editor.asset_height = 240.0f;
    snprintf(g_editor.status_msg, 128, "Ready.");

    f_small = starlight_load_font("assets/fonts/Inconsolata-Regular.ttf", 18.0f);

    // Register components for the world
    components_register(app->world);

    // Create some test entities for the hierarchy
    ecs_entity_t e1 = ecs_new_id(app->world);
    ecs_set_name(app->world, e1, "Main Camera");

    ecs_entity_t e2 = ecs_new_id(app->world);
    ecs_set_name(app->world, e2, "Sun Light");

    ecs_entity_t e3 = ecs_new_id(app->world);
    ecs_set_name(app->world, e3, "Banana Mesh");
    Transform t = {{0,0,0}, {0,0,0}, {1,1,1}};
    ecs_set_id(app->world, e3, ecs_id(Transform), sizeof(Transform), &t);

    gizmo_init(&app->gizmo);
}

void Editor_OnUpdate(SLF_App* app) {
    // Keyboard shortcuts
    if (app->keys[SDL_SCANCODE_S] && (app->keys[SDL_SCANCODE_LCTRL] || app->keys[SDL_SCANCODE_RCTRL])) {
        serialization_save_world(app->world, "assets/scenes/editor_autosave.starlight");
        snprintf(g_editor.status_msg, 128, "Scene autosaved!");
    }
}

void Editor_OnUI(SLF_App* app) {
    float sw = (float)app->width;
    float sh = (float)app->height;

    // 1. TOP TOOLBAR
    vec4 bar_color = {0.15f, 0.15f, 0.18f, 1.0f};
    slf_ui_panel(app, 0, 0, sw, 40, bar_color);
    slf_ui_label(app, "STARLIGHT STUDIO v0.9.8 | Pure C Engine", 20, 25, (vec4){0, 0.8, 1, 1});

    if (slf_ui_button(app, "SAVE SCENE", sw - 120, 5, 100, 30)) {
        serialization_save_world(app->world, "assets/scenes/scene_1.starlight");
        snprintf(g_editor.status_msg, 128, "Scene saved to assets/scenes/scene_1.starlight");
    }

    // 2. LEFT SIDEBAR (Hierarchy)
    slf_ui_panel(app, 0, 40, g_editor.sidebar_width, sh - 40 - g_editor.asset_height, (vec4){0.1, 0.1, 0.12, 1});
    slf_ui_label(app, "HIERARCHY", 10, 65, (vec4){0.5, 0.5, 0.5, 1});

    // List entities from Flecs
    ecs_query_t* q = ecs_query(app->world, { .filter.terms = {{0}} }); // Query all
    ecs_iter_t it = ecs_query_iter(app->world, q);
    int y_off = 80;
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const char* name = ecs_get_name(app->world, it.entities[i]);
            char label[64];
            snprintf(label, 64, "[%d] %s", (int)it.entities[i], name ? name : "Unnamed");

            if (slf_ui_button(app, label, 5, y_off, g_editor.sidebar_width - 10, 25)) {
                g_editor.selected_entity = it.entities[i];
            }
            y_off += 30;
        }
    }

    // 3. RIGHT SIDEBAR (Inspector)
    float insp_x = sw - g_editor.sidebar_width;
    slf_ui_panel(app, insp_x, 40, g_editor.sidebar_width, sh - 40, (vec4){0.1, 0.1, 0.12, 1});
    slf_ui_label(app, "INSPECTOR", insp_x + 10, 65, (vec4){0.5, 0.5, 0.5, 1});

    if (g_editor.selected_entity != 0) {
        const char* name = ecs_get_name(app->world, g_editor.selected_entity);
        slf_ui_label(app, name ? name : "Entity", insp_x + 20, 100, (vec4){1,1,0,1});
        
        Transform* t = ecs_get_mut_id(app->world, g_editor.selected_entity, ecs_id(Transform));
        if (t) {
            slf_ui_label(app, "Transform", insp_x + 20, 140, (vec4){0, 1, 1, 1});
            char pos_str[64];
            snprintf(pos_str, 64, "Pos: %.2f, %.2f, %.2f", t->position[0], t->position[1], t->position[2]);
            slf_ui_label(app, pos_str, insp_x + 30, 170, (vec4){1,1,1,0.8});

            if (slf_ui_button(app, "Move +1 X", insp_x + 30, 190, 100, 25)) t->position[0] += 1.0f;
        } else {
            if (slf_ui_button(app, "ADD TRANSFORM", insp_x + 20, 140, 150, 30)) {
                Transform nt = {{0,0,0}, {0,0,0}, {1,1,1}};
                ecs_set_id(app->world, g_editor.selected_entity, ecs_id(Transform), sizeof(Transform), &nt);
            }
        }
    }

    // 4. BOTTOM BAR (Assets & Status)
    slf_ui_panel(app, g_editor.sidebar_width, sh - g_editor.asset_height, sw - g_editor.sidebar_width * 2, g_editor.asset_height, (vec4){0.08, 0.08, 0.1, 1});
    slf_ui_label(app, "ASSET BROWSER", g_editor.sidebar_width + 10, sh - g_editor.asset_height + 25, (vec4){0.5, 0.5, 0.5, 1});

    // Status Bar
    slf_ui_panel(app, 0, sh - 25, sw, 25, (vec4){0.1, 0.5, 0.8, 1});
    slf_ui_label(app, g_editor.status_msg, 10, sh - 7, (vec4){1,1,1,1});
}

int main(int argc, char* argv[]) {
    printf("[EDITOR] main starting...\n"); fflush(stdout);
    SLF_Config config = {0};
    config.on_start = Editor_OnStart;
    config.on_update = Editor_OnUpdate;
    config.on_ui = Editor_OnUI;

    return starlight_framework_init("Starlight Studio (Native C)", 1280, 720, &config);
}
