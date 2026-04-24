// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
// ============================================================
// Vamos Engine - Navigation Module (Recast/Detour)
// Licença: zlib | Origem: Usado por Unity/Unreal
// ============================================================

#include <Recast.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <vector>
#include <memory>
#include "Log.hpp"

namespace Vamos {
    class NavigationSystem {
    public:
        NavigationSystem() {
            navQuery = dtAllocNavMeshQuery();
            VAMOS_CORE_INFO("Navigation: Recast/Detour inicializado");
        }

        ~NavigationSystem() {
            if (navQuery) dtFreeNavMeshQuery(navQuery);
            if (navMesh) dtFreeNavMesh(navMesh);
        }

        // Configuração do Recast
        struct NavMeshConfig {
            float cellSize = 0.3f;
            float cellHeight = 0.2f;
            float agentHeight = 2.0f;
            float agentRadius = 0.6f;
            float agentMaxClimb = 0.9f;
            float agentMaxSlope = 45.0f;
            int regionMinSize = 8;
            int regionMergeSize = 20;
            float edgeMaxLen = 12.0f;
            float edgeMaxError = 1.3f;
            int vertsPerPoly = 6;
            float detailSampleDist = 6.0f;
            float detailSampleMaxError = 1.0f;
        };

        bool FindPath(const float* start, const float* end, std::vector<float>& path) {
            if (!navMesh || !navQuery) return false;

            dtPolyRef startRef, endRef;
            float polyPickExt[3] = {2.0f, 4.0f, 2.0f};
            dtQueryFilter filter;

            navQuery->findNearestPoly(start, polyPickExt, &filter, &startRef, nullptr);
            navQuery->findNearestPoly(end, polyPickExt, &filter, &endRef, nullptr);

            if (!startRef || !endRef) return false;

            dtPolyRef pathPolys[256];
            int pathCount = 0;
            navQuery->findPath(startRef, endRef, start, end, &filter, pathPolys, &pathCount, 256);

            if (pathCount == 0) return false;

            float straightPath[256 * 3];
            int straightPathCount = 0;
            unsigned char straightPathFlags[256];
            dtPolyRef straightPathPolys[256];

            navQuery->findStraightPath(start, end, pathPolys, pathCount,
                straightPath, straightPathFlags, straightPathPolys, &straightPathCount, 256);

            path.clear();
            for (int i = 0; i < straightPathCount * 3; i++) {
                path.push_back(straightPath[i]);
            }
            return true;
        }

        bool IsInitialized() const { return navMesh != nullptr; }

    private:
        dtNavMesh* navMesh = nullptr;
        dtNavMeshQuery* navQuery = nullptr;
        NavMeshConfig config;
    };

    // Componente ECS
    struct NavAgentComponent {
        float speed = 3.5f;
        float radius = 0.6f;
        std::vector<float> currentPath;
        int currentPathIndex = 0;
        bool hasPath = false;
    };
}
