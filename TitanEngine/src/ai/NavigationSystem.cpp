#include "NavigationSystem.hpp"
#include <queue>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace titan {

    NavigationSystem::NavigationSystem(int gridSize, float worldSize) 
        : m_gridSize(gridSize), m_worldSize(worldSize) {
        m_grid.resize(gridSize * gridSize, false);
    }

    void NavigationSystem::SetObstacle(int x, int z, bool blocked) {
        if (x >= 0 && x < m_gridSize && z >= 0 && z < m_gridSize) {
            m_grid[z * m_gridSize + x] = blocked;
        }
    }

    void NavigationSystem::UpdateRect(int x1, int z1, int x2, int z2, bool blocked) {
        for (int i = x1; i <= x2; i++) {
            for (int j = z1; j <= z2; j++) {
                SetObstacle(i, j, blocked);
            }
        }
    }

    NavPos NavigationSystem::WorldToGrid(const glm::vec3& worldPos) const {
        int x = static_cast<int>((worldPos.x / m_worldSize) * m_gridSize + m_gridSize / 2);
        int z = static_cast<int>((worldPos.z / m_worldSize) * m_gridSize + m_gridSize / 2);
        return { std::clamp(x, 0, m_gridSize - 1), std::clamp(z, 0, m_gridSize - 1) };
    }

    glm::vec3 NavigationSystem::GridToWorld(NavPos gridPos) const {
        float x = (static_cast<float>(gridPos.x - m_gridSize / 2) / m_gridSize) * m_worldSize;
        float z = (static_cast<float>(gridPos.z - m_gridSize / 2) / m_gridSize) * m_worldSize;
        return glm::vec3(x, 0.0f, z);
    }

    bool NavigationSystem::FindPath(const glm::vec3& start, const glm::vec3& end, std::vector<glm::vec3>& outPath) {
        NavPos startPos = WorldToGrid(start);
        NavPos endPos = WorldToGrid(end);

        if (startPos == endPos) return false;
        if (m_grid[endPos.z * m_gridSize + endPos.x]) return false;

        std::vector<Node> allNodes;
        std::priority_queue<int, std::vector<int>, std::function<bool(int, int)>> openSet(
            [&allNodes](int a, int b) { return allNodes[a].f > allNodes[b].f; }
        );

        std::vector<int> nodeIdxGrid(m_gridSize * m_gridSize, -1);
        std::vector<bool> closedSet(m_gridSize * m_gridSize, false);

        allNodes.push_back({ startPos, 0, 0, -1 });
        openSet.push(0);
        nodeIdxGrid[startPos.z * m_gridSize + startPos.x] = 0;

        while (!openSet.empty()) {
            int currentIdx = openSet.top();
            openSet.pop();
            Node current = allNodes[currentIdx];

            if (current.pos == endPos) {
                // Reconstruct Path
                int p = currentIdx;
                while (p != -1) {
                    outPath.push_back(GridToWorld(allNodes[p].pos));
                    p = allNodes[p].parentIdx;
                }
                std::reverse(outPath.begin(), outPath.end());
                return true;
            }

            closedSet[current.pos.z * m_gridSize + current.pos.x] = true;

            // 8 Neighbors
            for (int dx = -1; dx <= 1; dx++) {
                for (int dz = -1; dz <= 1; dz++) {
                    if (dx == 0 && dz == 0) continue;
                    
                    int nx = current.pos.x + dx;
                    int nz = current.pos.z + dz;

                    if (nx < 0 || nx >= m_gridSize || nz < 0 || nz >= m_gridSize) continue;
                    if (closedSet[nz * m_gridSize + nx] || m_grid[nz * m_gridSize + nx]) continue;

                    float stepCost = (dx == 0 || dz == 0) ? 1.0f : 1.414f;
                    float ng = current.g + stepCost;
                    float nf = ng + static_cast<float>(abs(nx - endPos.x) + abs(nz - endPos.z));

                    int existingIdx = nodeIdxGrid[nz * m_gridSize + nx];
                    if (existingIdx == -1 || ng < allNodes[existingIdx].g) {
                        if (existingIdx == -1) {
                            nodeIdxGrid[nz * m_gridSize + nx] = (int)allNodes.size();
                            allNodes.push_back({ {nx, nz}, ng, nf, currentIdx });
                            openSet.push((int)allNodes.size() - 1);
                        } else {
                            allNodes[existingIdx].g = ng;
                            allNodes[existingIdx].f = nf;
                            allNodes[existingIdx].parentIdx = currentIdx;
                        }
                    }
                }
            }
        }

        return false;
    }

}
