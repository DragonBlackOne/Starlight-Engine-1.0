#include "editor.h"
#include "components.h"
#include "debug_console.h"
#include "ui_layout.h"
#include <stdio.h>

void editor_init(Editor* editor) {
    editor->selected_entity = 0;
    editor->active = false;
    editor->show_ui = true;
    debug_log("[EDITOR] Starlight Studio Lite initialized.");
}

void editor_update(SLF_App* app, Editor* editor) {
    if (app->keys[SDL_SCANCODE_F2] && !app->prev_keys[SDL_SCANCODE_F2]) {
        editor->active = !editor->active;
        SDL_SetRelativeMouseMode(!editor->active);
    }

    if (!editor->active) return;
    
    int mx, my;
    uint32_t m_state = SDL_GetMouseState(&mx, &my);
    
    // Pick entity only if clicking inside viewport area (not over panels)
    if ((m_state & SDL_BUTTON(SDL_BUTTON_LEFT)) && 
        mx > 200 && mx < (app->width - 250) && my < (app->height - 200)) {
        editor_pick_entity(app, editor);
    }
}

void editor_pick_entity(SLF_App* app, Editor* editor) {
    ecs_query_desc_t desc = {0};
    desc.filter.terms[0].id = ecs_id(Transform);
    ecs_query_t* q = ecs_query_init(app->world, &desc);
    
    ecs_iter_t it = ecs_query_iter(app->world, q);
    if (ecs_query_next(&it)) {
        // Just grabs the first one for now
        editor->selected_entity = it.entities[0];
        debug_log("[EDITOR] Selected Entity: %u", (uint32_t)editor->selected_entity);
    }
    ecs_query_fini(q);
}

void editor_render_ui(SLF_App* app, Editor* editor) {
    if (!editor->active || !editor->show_ui) return;
    
    // Draw 3D Gizmos for selected entity
    if (editor->selected_entity && ecs_is_valid(app->world, editor->selected_entity)) {
        Transform* t = ecs_get_mut(app->world, editor->selected_entity, Transform);
        if (t) {
            mat4 view, proj;
            float aspect = (float)app->width / (float)app->height;
            camera_get_view_matrix(&app->camera, view);
            glm_perspective(glm_rad(60.0f), aspect, 0.1f, 10000.0f, proj);
            
            // Disable depth test so gizmo shows on top
            glDisable(GL_DEPTH_TEST);
            
            vec3 right = {2.0f, 0.0f, 0.0f}; vec3 p_right;
            vec3 up    = {0.0f, 2.0f, 0.0f}; vec3 p_up;
            vec3 fwd   = {0.0f, 0.0f, 2.0f}; vec3 p_fwd;
            
            glm_vec3_add(t->position, right, p_right);
            glm_vec3_add(t->position, up, p_up);
            glm_vec3_add(t->position, fwd, p_fwd);
            
            gizmo_draw_line(&app->gizmo, t->position, p_right, (vec3){1.0f, 0.0f, 0.0f}, view, proj); // X Red
            gizmo_draw_line(&app->gizmo, t->position, p_up,    (vec3){0.0f, 1.0f, 0.0f}, view, proj); // Y Green
            gizmo_draw_line(&app->gizmo, t->position, p_fwd,   (vec3){0.0f, 0.0f, 1.0f}, view, proj); // Z Blue
            
            glEnable(GL_DEPTH_TEST);
        }
    }
    
    vec4 panel_color  = {0.15f, 0.15f, 0.15f, 0.95f};
    vec4 header_color = {0.25f, 0.25f, 0.25f, 1.0f};
    vec4 text_color   = {0.9f, 0.9f, 0.9f, 1.0f};
    
    // 1. Hierarchy Panel (Left)
    float hierarchy_w = 200.0f;
    slf_ui_panel(app, 0, 0, hierarchy_w, app->height - 200, panel_color);
    slf_ui_panel(app, 0, 0, hierarchy_w, 30, header_color);
    slf_ui_label(app, "Scene Hierarchy", 10, 5, text_color);
    
    ecs_query_t* q = ecs_query_init(app->world, &(ecs_query_desc_t){
        .filter.terms = {{ ecs_id(Transform) }}
    });
    ecs_iter_t it = ecs_query_iter(app->world, q);
    float item_y = 40.0f;
    while(ecs_query_next(&it)) {
        for(int i = 0; i < it.count; ++i) {
            char label[64];
            snprintf(label, sizeof(label), "Entity %u", (uint32_t)it.entities[i]);
            if (slf_ui_button(app, label, 5, item_y, hierarchy_w - 10, 25)) {
                editor->selected_entity = it.entities[i];
            }
            item_y += 30.0f;
        }
    }
    ecs_query_fini(q);
    
    // 2. Inspector Panel (Right)
    float inspector_w = 250.0f;
    float inspector_x = app->width - inspector_w;
    slf_ui_panel(app, inspector_x, 0, inspector_w, app->height - 200, panel_color);
    slf_ui_panel(app, inspector_x, 0, inspector_w, 30, header_color);
    slf_ui_label(app, "Inspector", inspector_x + 10, 5, text_color);
    
    if (editor->selected_entity && ecs_is_valid(app->world, editor->selected_entity)) {
        Transform* t = ecs_get_mut(app->world, editor->selected_entity, Transform);
        if (t) {
            char buf[64];
            slf_ui_label(app, "Transform", inspector_x + 10, 40, text_color);
            snprintf(buf, sizeof(buf), "Pos X: %.2f", t->position[0]); slf_ui_label(app, buf, inspector_x + 10, 60, text_color);
            snprintf(buf, sizeof(buf), "Pos Y: %.2f", t->position[1]); slf_ui_label(app, buf, inspector_x + 10, 80, text_color);
            snprintf(buf, sizeof(buf), "Pos Z: %.2f", t->position[2]); slf_ui_label(app, buf, inspector_x + 10, 100, text_color);
            
            snprintf(buf, sizeof(buf), "Scale: X:%.1f Y:%.1f Z:%.1f", t->scale[0], t->scale[1], t->scale[2]); 
            slf_ui_label(app, buf, inspector_x + 10, 120, text_color);
        }
    } else {
        slf_ui_label(app, "No Entity Selected", inspector_x + 10, 40, text_color);
    }
    
    // 3. Asset Browser Panel (Bottom)
    float browser_h = 200.0f;
    float browser_y = app->height - browser_h;
    slf_ui_panel(app, 0, browser_y, app->width, browser_h, panel_color);
    slf_ui_panel(app, 0, browser_y, app->width, 30, header_color);
    slf_ui_label(app, "Asset Browser", 10, browser_y + 5, text_color);
    
    // Mock representation of assets
    slf_ui_button(app, "player.glb", 10, browser_y + 40, 100, 40);
    slf_ui_button(app, "world.glb", 120, browser_y + 40, 100, 40);
    slf_ui_button(app, "theme.mp3", 230, browser_y + 40, 100, 40);
}
