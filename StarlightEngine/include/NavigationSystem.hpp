// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace starlight {

    struct NavPos {
        int x, z;
        bool operator==(const NavPos& other) const { return x == other.x && z == other.z; }
    };

    class NavigationSystem {
    public:
        NavigationSystem(int gridSize = 64, float worldSize = 100.0f);
        
        void SetObstacle(int x, int z, bool blocked);
        void UpdateRect(int x1, int z1, int x2, int z2, bool blocked);
        
        bool FindPath(const glm::vec3& start, const glm::vec3& end, std::vector<glm::vec3>& outPath);

        // Helpers
        NavPos WorldToGrid(const glm::vec3& worldPos) const;
        glm::vec3 GridToWorld(NavPos gridPos) const;

    private:
        int m_gridSize;
        float m_worldSize;
        std::vector<bool> m_grid;

        struct Node {
            NavPos pos;
            float g, f;
            int parentIdx;
        };

        std::vector<int> m_nodeIdxGrid;
        std::vector<bool> m_closedSet;
        std::vector<Node> m_allNodes;
    };

}
