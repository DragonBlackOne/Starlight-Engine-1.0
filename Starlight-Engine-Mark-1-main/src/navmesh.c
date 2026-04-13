#include "navmesh.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

static bool g_nav_grid[NAV_GRID_SIZE][NAV_GRID_SIZE];

void nav_set_obstacle(int x, int y, bool blocked) {
    if (x >= 0 && x < NAV_GRID_SIZE && y >= 0 && y < NAV_GRID_SIZE) {
        g_nav_grid[x][y] = blocked;
    }
}

// O(1) Block placement via Rect
void nav_update_rect(int x1, int y1, int x2, int y2, bool blocked) {
    for (int i = x1; i <= x2; i++) {
        for (int j = y1; j <= y2; j++) {
            nav_set_obstacle(i, j, blocked);
        }
    }
}

typedef struct {
    int x, y;
    float g, f;
    int parent_idx;
} Node;

bool nav_find_path(int sx, int sy, int ex, int ey, NavPos* out_path, int* out_count) {
    if (sx == ex && sy == ey) return false;
    if (ex < 0 || ex >= NAV_GRID_SIZE || ey < 0 || ey >= NAV_GRID_SIZE || g_nav_grid[ex][ey]) return false;
    
    // Stable Array Pool to prevent pointer corruption during stack mutation
    static Node nodes[NAV_GRID_SIZE * NAV_GRID_SIZE];
    int node_count = 0;
    
    int open[1024]; int open_cnt = 0;
    bool closed[NAV_GRID_SIZE][NAV_GRID_SIZE] = {0};

    nodes[node_count] = (Node){sx, sy, 0, 0, -1};
    open[open_cnt++] = node_count++;
    
    while(open_cnt > 0) {
        // Find best in open
        int best_q = 0;
        for(int i = 1; i < open_cnt; i++) {
            if(nodes[open[i]].f < nodes[open[best_q]].f) best_q = i;
        }
        
        int current_idx = open[best_q];
        Node current = nodes[current_idx];
        
        if(current.x == ex && current.y == ey) {
            // Reconstruct path
            int count = 0;
            int p = current_idx;
            
            // Temporary buffer to reverse path
            NavPos temp[256];
            while(p != -1 && count < 256) {
                temp[count].x = nodes[p].x; 
                temp[count].y = nodes[p].y;
                p = nodes[p].parent_idx; 
                count++;
            }
            
            // Store path from start to end
            for (int i = 0; i < count; i++) {
                out_path[i] = temp[(count - 1) - i];
            }
            *out_count = count;
            return true;
        }

        // Move to closed
        closed[current.x][current.y] = true;
        open[best_q] = open[--open_cnt];

        // Neighbors
        int dx[] = {0,0,1,-1, 1, -1, 1, -1}, dy[] = {1,-1,0,0, 1, -1, -1, 1}; // +Diagonals
        for(int i = 0; i < 8; i++) {
            int nx = current.x + dx[i], ny = current.y + dy[i];
            if(nx < 0 || nx >= NAV_GRID_SIZE || ny < 0 || ny >= NAV_GRID_SIZE) continue;
            if(closed[nx][ny] || g_nav_grid[nx][ny]) continue;
            
            float step_cost = (i < 4) ? 1.0f : 1.414f;
            float ng = current.g + step_cost;
            float nf = ng + fabsf((float)(nx-ex)) + fabsf((float)(ny-ey)); // Manhattan H
            
            if(node_count < NAV_GRID_SIZE * NAV_GRID_SIZE) {
                nodes[node_count] = (Node){nx, ny, ng, nf, current_idx};
                open[open_cnt++] = node_count++;
            }
        }
    }
    return false;
}
