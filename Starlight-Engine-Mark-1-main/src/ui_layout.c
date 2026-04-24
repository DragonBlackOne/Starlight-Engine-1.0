// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "ui_layout.h"
#include "starlight.h"

void ui_layout_calculate(UI_Element* node, float x, float y, float parent_w, float parent_h) {
    // Definir dimensoes se forem relativas (ex: width=0 significa 100%)
    if (node->width <= 0) node->width = parent_w - node->margin[1] - node->margin[3];
    if (node->height <= 0) node->height = parent_h - node->margin[0] - node->margin[2];

    float current_x = x + node->margin[3] + node->padding[3];
    float current_y = y + node->margin[0] + node->padding[0];

    for (int i = 0; i < node->child_count; i++) {
        UI_Element* child = &node->children[i];
        ui_layout_calculate(child, current_x, current_y, node->width, node->height);

        if (node->direction == UI_DIR_VERTICAL) {
            current_y += child->height + child->margin[0] + child->margin[2];
        } else {
            current_x += child->width + child->margin[1] + child->margin[3];
        }
    }
}

void ui_layout_render(SLF_App* app, UI_Element* node) {
    // Este seria o loop real de desenho usando a posição calculada
    // Por brevidade, simulamos o desenho do retangulo do nodo
    starlight_ui_draw_rect(app, node->margin[3], node->margin[0], node->width, node->height, node->color);
    
    for (int i = 0; i < node->child_count; i++) {
        ui_layout_render(app, &node->children[i]);
    }
}

// --- Immediate Mode UI (Pure C) Implementation ---

bool slf_ui_button(SLF_App* app, const char* label, float x, float y, float w, float h) {
    int mx, my;
    uint32_t m_state = SDL_GetMouseState(&mx, &my);
    bool hover = (mx >= x && mx <= x + w && my >= y && my <= y + h);
    bool click = hover && (m_state & SDL_BUTTON(SDL_BUTTON_LEFT));

    vec4 color = {0.2f, 0.2f, 0.25f, 1.0f};
    if (hover) { color[0] = 0.3f; color[1] = 0.3f; color[2] = 0.4f; }
    if (click) { color[0] = 0.1f; color[1] = 0.5f; color[2] = 0.8f; }

    starlight_ui_draw_rect(app, x, y, w, h, color);

    // Centering text (approximated)
    if (app->default_font) {
        starlight_ui_draw_text(app, app->default_font, label, x + 10, y + h/2 - 10, (vec4){1,1,1,1});
    }

    return click;
}

void slf_ui_label(SLF_App* app, const char* text, float x, float y, vec4 color) {
    if (app->default_font) {
        starlight_ui_draw_text(app, app->default_font, text, x, y, color);
    }
}

void slf_ui_panel(SLF_App* app, float x, float y, float w, float h, vec4 color) {
    starlight_ui_draw_rect(app, x, y, w, h, color);
}
