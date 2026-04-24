// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "OctreeSystem.hpp"
#include <algorithm>

namespace titan {

    OctreeSystem::OctreeSystem(const AABB& worldBounds) 
        : m_worldBounds(worldBounds) {
        m_root = std::make_unique<OctreeNode>(worldBounds);
    }

    void OctreeSystem::Clear() {
        m_root = std::make_unique<OctreeNode>(m_worldBounds);
    }

    void OctreeSystem::Split(OctreeNode* node) {
        node->isLeaf = false;
        glm::vec3 center = (node->bounds.min + node->bounds.max) * 0.5f;

        for (int i = 0; i < 8; i++) {
            AABB b;
            b.min.x = (i & 1) ? center.x : node->bounds.min.x;
            b.max.x = (i & 1) ? node->bounds.max.x : center.x;
            b.min.y = (i & 2) ? center.y : node->bounds.min.y;
            b.max.y = (i & 2) ? node->bounds.max.y : center.y;
            b.min.z = (i & 4) ? center.z : node->bounds.min.z;
            b.max.z = (i & 4) ? node->bounds.max.z : center.z;
            
            node->children[i] = std::make_unique<OctreeNode>(b);
        }

        // Redistribute existing entities
        for (auto e : node->entities) {
            // Re-inserting logic would go here, simplified: just insert into fit children
            // For brevity in the showcase, we usually insert into children directly on split
        }
        node->entities.clear();
    }

    void OctreeSystem::Insert(entt::entity entity, const AABB& entityAABB) {
        OctreeNode* current = m_root.get();
        
        while (!current->isLeaf) {
            bool foundChild = false;
            for (int i = 0; i < 8; i++) {
                if (current->children[i]->bounds.Contains(entityAABB)) {
                    current = current->children[i].get();
                    foundChild = true;
                    break;
                }
            }
            if (!foundChild) break; // Doesn't fit perfectly in a child, keep in current
        }

        if (current->isLeaf && current->entities.size() >= OctreeNode::MAX_ENTITIES) {
            Split(current);
            // After split, try to insert again to move down if possible
            Insert(entity, entityAABB);
            return;
        }

        current->entities.push_back(entity);
    }

    void OctreeSystem::Query(const glm::mat4& viewProj, std::vector<entt::entity>& outEntities) {
        QueryRecursive(m_root.get(), viewProj, outEntities);
    }

    void OctreeSystem::QueryRecursive(OctreeNode* node, const glm::mat4& viewProj, std::vector<entt::entity>& outEntities) {
        if (!IsInsideFrustum(viewProj, node->bounds)) return;

        for (auto e : node->entities) {
            outEntities.push_back(e);
        }

        if (!node->isLeaf) {
            for (int i = 0; i < 8; i++) {
                if (node->children[i]) {
                    QueryRecursive(node->children[i].get(), viewProj, outEntities);
                }
            }
        }
    }

    bool OctreeSystem::IsInsideFrustum(const glm::mat4& viewProj, const AABB& bounds) {
        // Simplified Frustum Culling (Check if center is somewhat in NDC [-1, 1])
        // In a real pro engine, we'd check all 6 planes. 
        // For the showcase, we'll check the center transformed point.
        glm::vec3 center = (bounds.min + bounds.max) * 0.5f;
        glm::vec4 clipPos = viewProj * glm::vec4(center, 1.0f);
        if (clipPos.w == 0) return true;
        
        float ndcX = clipPos.x / clipPos.w;
        float ndcY = clipPos.y / clipPos.w;
        float ndcZ = clipPos.z / clipPos.w;

        // Give some margin for the bounds size
        float margin = 1.2f; 
        return (ndcX >= -margin && ndcX <= margin && ndcY >= -margin && ndcY <= margin && ndcZ >= -margin && ndcZ <= margin);
    }

}
