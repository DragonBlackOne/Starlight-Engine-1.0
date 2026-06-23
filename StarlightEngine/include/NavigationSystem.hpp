#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "EngineSystem.hpp"

namespace starlight {

    struct NavPos {
        int x, z;
        bool operator==(const NavPos& other) const { return x == other.x && z == other.z; }
    };

    class NavigationSystem : public ISystem {
    public:
        NavigationSystem(int gridSize = 64, float worldSize = 100.0f);
        
        // ISystem
        bool OnInitialize(const EngineContext& context) override { (void)context; return true; }
        void OnShutdown() override {}
        void OnUpdate(float dt) override;
        const char* GetName() const override { return "NavigationSystem"; }
        
        void SetObstacle(int x, int z, bool blocked);
        void UpdateRect(int x1, int z1, int x2, int z2, bool blocked);
        
        bool FindPath(const glm::vec3& start, const glm::vec3& end, std::vector<glm::vec3>& outPath);

        // Helpers
        NavPos WorldToGrid(const glm::vec3& worldPos) const;
        glm::vec3 GridToWorld(NavPos gridPos) const;

    private:
        int m_gridSize = 64;
        float m_worldSize = 100.0f;
        std::vector<bool> m_grid;

        struct Node {
            NavPos pos;
            float g, f;
            int parentIdx;
        };

        struct GridCell {
            int nodeIdx = -1;
            uint32_t closedRunID = 0;
            uint32_t openRunID = 0;
        };

        std::vector<GridCell> m_gridCells;
        uint32_t m_currentRunID = 0;
        std::vector<Node> m_allNodes;
    };

}
