#ifndef STARLIGHT_NAVMESH_H
#define STARLIGHT_NAVMESH_H

#include <stdbool.h>

#define NAV_GRID_SIZE 32

typedef struct {
    int x, y;
} NavPos;

// Finds a path in a grid using A*
bool nav_find_path(int start_x, int start_y, int end_x, int end_y, NavPos* out_path, int* out_count);
void nav_set_obstacle(int x, int y, bool blocked);
void nav_update_rect(int x1, int y1, int x2, int y2, bool blocked);

#endif
