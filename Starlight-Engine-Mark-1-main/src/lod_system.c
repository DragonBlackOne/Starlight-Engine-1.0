// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "lod.h"
#include <math.h>

int lod_select_level(LODGroup* group, float distance) {
    if (group->count <= 1) return 0;

    int selected = 0;
    for (int i = 0; i < group->count; i++) {
        if (distance > group->levels[i].distance_threshold) {
            selected = i;
        } else {
            break;
        }
    }
    group->current_lod = selected;
    return selected;
}
