#include "TilemapSystem.hpp"
#include "Components.hpp"
#include "Renderer2D.hpp"
#include "Engine.hpp"
#include "Log.hpp"
#include "CVarSystem.hpp"
#include <cmath>
#include <algorithm>

namespace starlight {

    bool TilemapSystem::OnInitialize(const EngineContext& /*context*/) {
        Log::Info("TilemapSystem: Initialized.");
        return true;
    }

    void TilemapSystem::OnShutdown() {
        Log::Info("TilemapSystem: Shutdown.");
    }

    void TilemapSystem::OnRender() {
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene) return;

        auto& registry = activeScene->GetRegistry();
        auto view = registry.view<TransformComponent, TilemapComponent>();

        bool cullEnabled = false;
        float c_left = 0.0f, c_right = 0.0f, c_bottom = 0.0f, c_top = 0.0f;

        auto cvarSys = Engine::Get().GetSystem<CVarSystem>();
        bool enableCulling = cvarSys ? cvarSys->GetBool("r_cull2d") : true;

        if (enableCulling) {
            auto cameraView = registry.view<TransformComponent, Camera2DComponent>();
            for (auto camEnt : cameraView) {
                auto& cam = cameraView.get<Camera2DComponent>(camEnt);
                if (cam.primary) {
                    auto& camTransform = cameraView.get<TransformComponent>(camEnt);
                    float width = (float)Engine::Get().GetWindow().GetWidth();
                    float height = (float)Engine::Get().GetWindow().GetHeight();
                    float halfW = width * 0.5f / cam.zoom;
                    float halfH = height * 0.5f / cam.zoom;

                    if (cam.rotation != 0.0f) {
                        float cosA = std::abs(std::cos(cam.rotation));
                        float sinA = std::abs(std::sin(cam.rotation));
                        float rotatedHalfW = halfW * cosA + halfH * sinA;
                        float rotatedHalfH = halfW * sinA + halfH * cosA;
                        c_left = camTransform.position.x - rotatedHalfW;
                        c_right = camTransform.position.x + rotatedHalfW;
                        c_bottom = camTransform.position.y - rotatedHalfH;
                        c_top = camTransform.position.y + rotatedHalfH;
                    } else {
                        c_left = camTransform.position.x - halfW;
                        c_right = camTransform.position.x + halfW;
                        c_bottom = camTransform.position.y - halfH;
                        c_top = camTransform.position.y + halfH;
                    }
                    cullEnabled = true;
                    break;
                }
            }
        }

        Renderer2D::BeginBatch();

        uint32_t totalTiles = 0;
        uint32_t visibleTiles = 0;

        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& tilemap = view.get<TilemapComponent>(entity);

            if (tilemap.tiles.empty() || tilemap.tilesetTexture == 0 || tilemap.mapWidth <= 0 || tilemap.mapHeight <= 0 || tilemap.tileSize <= 0) continue;

            // Count total active tiles
            for (int tileID : tilemap.tiles) {
                if (tileID > 0) totalTiles++;
            }

            float tileW = (float)tilemap.tileSize;
            float tileH = (float)tilemap.tileSize;

            float tilesetW = 1.0f / (float)tilemap.tilesetColumns;
            float tilesetH = 1.0f / (float)tilemap.tilesetRows;

            int minX = 0;
            int maxX = tilemap.mapWidth - 1;
            int minY = 0;
            int maxY = tilemap.mapHeight - 1;

            if (cullEnabled) {
                minX = std::max(0, static_cast<int>(std::floor((c_left - transform.position.x) / tileW)));
                maxX = std::min(tilemap.mapWidth - 1, static_cast<int>(std::ceil((c_right - transform.position.x) / tileW)));
                minY = std::max(0, static_cast<int>(std::floor((c_bottom - transform.position.y) / tileH)));
                maxY = std::min(tilemap.mapHeight - 1, static_cast<int>(std::ceil((c_top - transform.position.y) / tileH)));
            }

            for (int y = minY; y <= maxY; y++) {
                for (int x = minX; x <= maxX; x++) {
                    size_t index = static_cast<size_t>(y * tilemap.mapWidth + x);
                    if (index >= tilemap.tiles.size()) continue;
                    int tileID = tilemap.tiles[index];
                    if (tileID <= 0) continue;

                    visibleTiles++;

                    int tsCol = (tileID - 1) % tilemap.tilesetColumns;
                    int tsRow = (tileID - 1) / tilemap.tilesetColumns;

                    glm::vec2 pos = {
                        transform.position.x + (float)x * tileW,
                        transform.position.y + (float)y * tileH
                    };

                    glm::vec2 uv0 = {(float)tsCol * tilesetW, (float)tsRow * tilesetH};
                    glm::vec2 uv1 = {(float)(tsCol + 1) * tilesetW, (float)tsRow * tilesetH};
                    glm::vec2 uv2 = {(float)(tsCol + 1) * tilesetW, (float)(tsRow + 1) * tilesetH};
                    glm::vec2 uv3 = {(float)tsCol * tilesetW, (float)(tsRow + 1) * tilesetH};

                    Renderer2D::DrawQuad(pos, {tileW, tileH}, tilemap.tilesetTexture,
                                         uv0, uv1, uv2, uv3, tilemap.color, tilemap.layer);
                }
            }
        }

        Renderer2D::Increment2DStats(totalTiles, visibleTiles);
        Renderer2D::EndBatch();
    }

}
