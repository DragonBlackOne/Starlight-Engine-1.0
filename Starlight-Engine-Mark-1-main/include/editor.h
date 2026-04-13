#ifndef EDITOR_H
#define EDITOR_H

#include "starlight.h"

typedef struct {
    ecs_entity_t selected_entity;
    bool active;
    bool show_ui;
} Editor;

/**
 * @brief Inicializa o editor in-game.
 */
void editor_init(Editor* editor);

/**
 * @brief Tenta selecionar uma entidade baseada na posicao do mouse (Raycasting).
 */
void editor_pick_entity(SLF_App* app, Editor* editor);

/**
 * @brief Atualiza a logica do editor (gizmos, input).
 */
void editor_update(SLF_App* app, Editor* editor);

/**
 * @brief Renderiza os overlays do editor (HUD, menus).
 */
void editor_render_ui(SLF_App* app, Editor* editor);

#endif
