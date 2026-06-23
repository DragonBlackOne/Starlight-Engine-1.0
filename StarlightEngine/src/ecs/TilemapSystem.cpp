#include "TilemapSystem.hpp"
#include "Components.hpp"
#include "Renderer2D.hpp"
#include "Engine.hpp"
#include "Log.hpp"

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

        Renderer2D::BeginBatch();

        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& tilemap = view.get<TilemapComponent>(entity);

            if (tilemap.tiles.empty() || tilemap.tilesetTexture == 0) continue;

            float tileW = (float)tilemap.tileSize;
            float tileH = (float)tilemap.tileSize;

            float tilesetW = 1.0f / (float)tilemap.tilesetColumns;
            float tilesetH = 1.0f / (float)tilemap.tilesetRows;

            for (int y = 0; y < tilemap.mapHeight; y++) {
                for (int x = 0; x < tilemap.mapWidth; x++) {
                    int tileID = tilemap.tiles[y * tilemap.mapWidth + x];
                    if (tileID <= 0) continue;

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

        Renderer2D::EndBatch();
    }

}
