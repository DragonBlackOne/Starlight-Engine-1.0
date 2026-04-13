#include "serialization.h"
#include "components.h"
#include <stdio.h>
#include <stdlib.h>

bool serialization_save_world(ecs_world_t* world, const char* filepath) {
    FILE* file = fopen(filepath, "wb");
    if (!file) return false;

    uint32_t magic = 0x534C4621;
    uint32_t version = 1;
    fwrite(&magic, sizeof(uint32_t), 1, file);
    fwrite(&version, sizeof(uint32_t), 1, file);

    ecs_query_desc_t desc = {0};
    desc.filter.terms[0].id = ecs_id(Transform);
    ecs_query_t* q = ecs_query_init(world, &desc);

    ecs_iter_t it = ecs_query_iter(world, q);
    int count = 0;
    while (ecs_query_next(&it)) count += it.count;
    fwrite(&count, sizeof(int), 1, file);

    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Transform* t = ecs_field(&it, Transform, 1);
        for (int i = 0; i < it.count; i++) {
            fwrite(&it.entities[i], sizeof(ecs_entity_t), 1, file);
            fwrite(&t[i], sizeof(Transform), 1, file);
        }
    }

    ecs_query_fini(q);
    fclose(file);
    return true;
}

#include "../thirdparty/cJSON.h"

bool serialization_load_scene_json(ecs_world_t* world, const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) return false;

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = (char*)malloc(length + 1);
    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);

    cJSON* json_scene = cJSON_Parse(data);
    free(data);
    if (!json_scene) {
        printf("[SERIALIZATION] Erro ao parear JSON: %s\n", cJSON_GetErrorPtr());
        return false;
    }

    cJSON* entities = cJSON_GetObjectItem(json_scene, "entities");
    if (cJSON_IsArray(entities)) {
        cJSON* entity = NULL;
        cJSON_ArrayForEach(entity, entities) {
            ecs_entity_t e = ecs_new_id(world);
            
            cJSON* name = cJSON_GetObjectItem(entity, "name");
            if (cJSON_IsString(name) && name->valuestring) {
                ecs_set_name(world, e, name->valuestring);
            }

            cJSON* component = NULL;
            cJSON_ArrayForEach(component, cJSON_GetObjectItem(entity, "components")) {
                if (strcmp(component->string, "Transform") == 0) {
                    Transform t = {{0,0,0}, {0,0,0}, {1,1,1}};
                    cJSON* pos = cJSON_GetObjectItem(component, "position");
                    if (cJSON_IsArray(pos) && cJSON_GetArraySize(pos) == 3) {
                        t.position[0] = cJSON_GetArrayItem(pos, 0)->valuedouble;
                        t.position[1] = cJSON_GetArrayItem(pos, 1)->valuedouble;
                        t.position[2] = cJSON_GetArrayItem(pos, 2)->valuedouble;
                    }
                    cJSON* scale = cJSON_GetObjectItem(component, "scale");
                    if (cJSON_IsArray(scale) && cJSON_GetArraySize(scale) == 3) {
                        t.scale[0] = cJSON_GetArrayItem(scale, 0)->valuedouble;
                        t.scale[1] = cJSON_GetArrayItem(scale, 1)->valuedouble;
                        t.scale[2] = cJSON_GetArrayItem(scale, 2)->valuedouble;
                    }
                    ecs_set_id(world, e, ecs_id(Transform), sizeof(Transform), &t);
                }
                else if (strcmp(component->string, "Velocity") == 0) {
                    Velocity v = {{0,0,0}};
                    cJSON* vec = cJSON_GetObjectItem(component, "vec");
                    if (cJSON_IsArray(vec) && cJSON_GetArraySize(vec) == 3) {
                        v.velocity[0] = cJSON_GetArrayItem(vec, 0)->valuedouble;
                        v.velocity[1] = cJSON_GetArrayItem(vec, 1)->valuedouble;
                        v.velocity[2] = cJSON_GetArrayItem(vec, 2)->valuedouble;
                    }
                    ecs_set_id(world, e, ecs_id(Velocity), sizeof(Velocity), &v);
                }
            }
        }
    }

    cJSON_Delete(json_scene);
    printf("[SERIALIZATION] Data-Driven Scene (%s) Loaded with cJSON\n", filepath);
    return true;
}

bool serialization_load_world(ecs_world_t* world, const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) return false;

    uint32_t magic, version;
    fread(&magic, sizeof(uint32_t), 1, file);
    if (magic != 0x534C4621) { fclose(file); return false; }
    fread(&version, sizeof(uint32_t), 1, file);

    int count;
    fread(&count, sizeof(int), 1, file);

    for (int i = 0; i < count; i++) {
        ecs_entity_t e_id;
        Transform t;
        fread(&e_id, sizeof(ecs_entity_t), 1, file);
        fread(&t, sizeof(Transform), 1, file);
        ecs_set_id(world, 0, ecs_id(Transform), sizeof(Transform), &t);
    }

    fclose(file);
    return true;
}
