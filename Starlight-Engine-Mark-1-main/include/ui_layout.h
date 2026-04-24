// Este projeto é feito por IA e só o prompt é feito por um humano.
#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include <cglm/cglm.h>
#include <stdbool.h>

typedef enum { UI_ALIGN_START, UI_ALIGN_CENTER, UI_ALIGN_END } UI_Alignment;
typedef enum { UI_DIR_HORIZONTAL, UI_DIR_VERTICAL } UI_Direction;

typedef struct UI_Element {
    float width, height;
    float margin[4];
    float padding[4];
    UI_Alignment align;
    UI_Direction direction;
    vec4 color;
    struct UI_Element* children;
    int child_count;
} UI_Element;

struct SLF_App;
void ui_layout_calculate(UI_Element* root, float x, float y, float parent_w, float parent_h);
void ui_layout_render(struct SLF_App* app, UI_Element* root);

// --- Immediate Mode UI (Pure C) ---
bool slf_ui_button(struct SLF_App* app, const char* label, float x, float y, float w, float h);
void slf_ui_label(struct SLF_App* app, const char* text, float x, float y, vec4 color);
void slf_ui_panel(struct SLF_App* app, float x, float y, float w, float h, vec4 color);

#endif
