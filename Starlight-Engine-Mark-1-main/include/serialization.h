#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "starlight.h"
#include <stdio.h>

/**
 * @brief Salva o estado completo do mundo ECS em um arquivo binario.
 */
bool serialization_save_world(ecs_world_t* world, const char* filepath);

/**
 * @brief Carrega o estado do mundo ECS a partir de um arquivo binario.
 */
bool serialization_load_world(ecs_world_t* world, const char* filepath);

/**
 * @brief Carrega uma cena (conjunto de entidades) de um arquivo JSON.
 */
bool serialization_load_scene_json(ecs_world_t* world, const char* filepath);

/**
 * @brief Salva uma "snapshot" de uma entidade especifica.
 */
void serialization_save_entity(ecs_world_t* world, ecs_entity_t entity, FILE* file);

#endif
