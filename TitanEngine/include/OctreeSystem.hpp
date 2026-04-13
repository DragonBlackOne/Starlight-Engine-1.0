#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <entt/entt.hpp>

namespace titan {

    struct AABB {
        glm::vec3 min = glm::vec3(0.0f);
        glm::vec3 max = glm::vec3(0.0f);

        bool Contains(const AABB& other) const {
            return other.min.x >= min.x && other.max.x <= max.x &&
                   other.min.y >= min.y && other.max.y <= max.y &&
                   other.min.z >= min.z && other.max.z <= max.z;
        }
    };

    struct OctreeNode {
        AABB bounds;
        bool isLeaf = true;
        std::vector<entt::entity> entities;
        std::unique_ptr<OctreeNode> children[8];
        
        static constexpr int MAX_ENTITIES = 8;

        OctreeNode(const AABB& b) : bounds(b) {}
    };

    class OctreeSystem {
    public:
        OctreeSystem(const AABB& worldBounds);
        
        void Clear();
        void Insert(entt::entity entity, const AABB& entityAABB);
        
        // Query for frustum culling or broadphase
        void Query(const glm::mat4& viewProj, std::vector<entt::entity>& outEntities);

    private:
        std::unique_ptr<OctreeNode> m_root;
        AABB m_worldBounds;

        void Split(OctreeNode* node);
        void QueryRecursive(OctreeNode* node, const glm::mat4& viewProj, std::vector<entt::entity>& outEntities);
        bool IsInsideFrustum(const glm::mat4& viewProj, const AABB& bounds);
    };

}
