#include "NavigationSystem.hpp"
#include <queue>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include "Engine.hpp"
#include "Components.hpp"

namespace starlight {

    NavigationSystem::NavigationSystem(int gridSize, float worldSize) 
        : m_gridSize(gridSize <= 0 ? 64 : gridSize), 
          m_worldSize(worldSize <= 0.0001f ? 100.0f : worldSize) {
        m_grid.resize(m_gridSize * m_gridSize, false);
        m_gridCells.resize(m_gridSize * m_gridSize);
        m_allNodes.reserve(1024);
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

        // Incrementar o ID de execução
        m_currentRunID++;
        if (m_currentRunID == 0) {
            // Estouro seguro: limpa o epoch
            for (auto& cell : m_gridCells) {
                cell.nodeIdx = -1;
                cell.closedRunID = 0;
                cell.openRunID = 0;
            }
            m_currentRunID = 1;
        }

        m_allNodes.clear();

        auto comp = [this](int a, int b) { return m_allNodes[a].f > m_allNodes[b].f; };
        std::priority_queue<int, std::vector<int>, decltype(comp)> openSet(comp);

        m_allNodes.push_back({ startPos, 0, 0, -1 });
        openSet.push(0);
        
        int startCellIdx = startPos.z * m_gridSize + startPos.x;
        m_gridCells[startCellIdx].nodeIdx = 0;
        m_gridCells[startCellIdx].openRunID = m_currentRunID;

        while (!openSet.empty()) {
            int currentIdx = openSet.top();
            openSet.pop();
            Node current = m_allNodes[currentIdx];

            int currentCellIdx = current.pos.z * m_gridSize + current.pos.x;
            if (m_gridCells[currentCellIdx].closedRunID == m_currentRunID) continue;

            if (current.pos == endPos) {
                // Reconstruct Path
                int p = currentIdx;
                while (p != -1) {
                    outPath.push_back(GridToWorld(m_allNodes[p].pos));
                    p = m_allNodes[p].parentIdx;
                }
                std::reverse(outPath.begin(), outPath.end());
                return true;
            }

            m_gridCells[currentCellIdx].closedRunID = m_currentRunID;

            // 8 Neighbors
            for (int dx = -1; dx <= 1; dx++) {
                for (int dz = -1; dz <= 1; dz++) {
                    if (dx == 0 && dz == 0) continue;
                    
                    int nx = current.pos.x + dx;
                    int nz = current.pos.z + dz;

                    if (nx < 0 || nx >= m_gridSize || nz < 0 || nz >= m_gridSize) continue;
                    
                    int nCellIdx = nz * m_gridSize + nx;
                    if (m_gridCells[nCellIdx].closedRunID == m_currentRunID || m_grid[nCellIdx]) continue;

                    float stepCost = (dx == 0 || dz == 0) ? 1.0f : 1.414f;
                    float ng = current.g + stepCost;
                    float nf = ng + static_cast<float>(abs(nx - endPos.x) + abs(nz - endPos.z));

                    bool hasBeenVisited = (m_gridCells[nCellIdx].openRunID == m_currentRunID);
                    int existingIdx = hasBeenVisited ? m_gridCells[nCellIdx].nodeIdx : -1;

                    if (existingIdx == -1 || ng < m_allNodes[existingIdx].g) {
                        if (existingIdx == -1) {
                            int newNodeIdx = (int)m_allNodes.size();
                            m_gridCells[nCellIdx].nodeIdx = newNodeIdx;
                            m_gridCells[nCellIdx].openRunID = m_currentRunID;
                            m_allNodes.push_back({ {nx, nz}, ng, nf, currentIdx });
                            openSet.push(newNodeIdx);
                        } else {
                            m_allNodes[existingIdx].g = ng;
                            m_allNodes[existingIdx].f = nf;
                            m_allNodes[existingIdx].parentIdx = currentIdx;
                            openSet.push(existingIdx);
                        }
                    }
                }
            }
        }

        return false;
    }

    void NavigationSystem::OnUpdate(float dt) {
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene) return;

        auto& reg = activeScene->GetRegistry();
        auto view = reg.view<NavAgentComponent, TransformComponent>();

        struct AgentData {
            entt::entity entity;
            glm::vec3 pos;
            glm::vec3 vel;
            float radius;
            float maxSpeed;
            glm::vec3 target;
        };

        std::vector<AgentData> agents;
        view.each([&agents](entt::entity entity, const auto& agent, const auto& trans) {
            if (agent.active) {
                agents.push_back({ entity, trans.position, agent.velocity, agent.radius, agent.maxSpeed, agent.target });
            }
        });

        // Reciprocal velocity avoidance update (ORCA/RVO-like simulation)
        for (size_t i = 0; i < agents.size(); ++i) {
            auto& a = agents[i];
            
            // Preferred velocity towards target
            glm::vec3 toTarget = a.target - a.pos;
            float distToTarget = glm::length(toTarget);
            glm::vec3 prefVel(0.0f);
            if (distToTarget > 0.05f) {
                prefVel = (toTarget / distToTarget) * a.maxSpeed;
            }

            // Avoidance force calculation (Reciprocal Velocity Obstacles)
            glm::vec3 avoidanceForce(0.0f);
            for (size_t j = 0; j < agents.size(); ++j) {
                if (i == j) continue;
                const auto& b = agents[j];
                
                glm::vec3 relPos = a.pos - b.pos;
                relPos.y = 0.0f; // 2D floor avoidance
                float dist = glm::length(relPos);
                float minDist = a.radius + b.radius;
                
                if (dist < minDist + 1.0f && dist > 0.001f) {
                    float overlap = (minDist + 1.0f) - dist;
                    glm::vec3 repelDir = relPos / dist;
                    // Reciprocal force: push back based on relative proximity
                    avoidanceForce += repelDir * overlap * 3.0f;
                }
            }

            // Integrate preferred path velocity with ORCA avoidance vectors
            glm::vec3 targetVel = prefVel + avoidanceForce;
            float targetSpeed = glm::length(targetVel);
            if (targetSpeed > a.maxSpeed) {
                targetVel = (targetVel / targetSpeed) * a.maxSpeed;
            }

            // Smooth linear interpolation for velocity transitions
            glm::vec3 newVel = glm::mix(a.vel, targetVel, std::clamp(dt * 8.0f, 0.0f, 1.0f));
            
            auto& comp = reg.get<NavAgentComponent>(a.entity);
            auto& trans = reg.get<TransformComponent>(a.entity);
            
            comp.velocity = newVel;
            trans.position += newVel * dt;
            trans.UpdateLocalMatrix();
        }
    }

}
