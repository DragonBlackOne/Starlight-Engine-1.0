// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "Arcade25D.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "InputSystem.hpp"
#include "Components.hpp"
#include "TitanAudio.hpp"
#include "Log.hpp"
#include "imgui.h"
#include <algorithm>

namespace starlight {

    void Arcade25DModule::Initialize() {
        Log::Info("Arcade25D: Initializing 10-Game Suite...");
        m_currentGame = 0;
        m_totalTime = 0;
    }

    void Arcade25DModule::Shutdown() {
        m_arcadeRegistry.clear();
    }

    void Arcade25DModule::SpawnPrimitive(const std::string& tag, glm::vec3 pos, glm::vec3 scale, glm::vec3 color, float metallic, float roughness) {
        auto entity = m_arcadeRegistry.create();
        m_arcadeRegistry.emplace<TagComponent>(entity, tag);
        auto& t = m_arcadeRegistry.emplace<TransformComponent>(entity);
        t.position = pos;
        t.scale = scale;
        
        auto& mc = m_arcadeRegistry.emplace<MeshComponent>(entity);
        mc.mesh = Engine::Get().GetRenderer().GetCubeMesh();
        mc.material.isPBR = true;
        mc.material.albedo = color;
        mc.material.metallic = metallic;
        mc.material.roughness = roughness;
    }

    void Arcade25DModule::InitGame(int index) {
        m_arcadeRegistry.clear();
        m_currentGame = index;
        m_totalTime = 0;

        // Common Camera Setup for 2.5D
        auto cam = m_arcadeRegistry.create();
        auto& ct = m_arcadeRegistry.emplace<TransformComponent>(cam);
        auto& cc = m_arcadeRegistry.emplace<CameraComponent>(cam);
        cc.primary = true;

        switch(index) {
            case 1: // Voxel Rush
                ct.position = {0, 5, 15};
                ct.rotation = glm::quat(glm::vec3(glm::radians(-15.0f), 0, 0));
                SpawnPrimitive("Player", {0, 0, 0}, {1, 1, 1}, {0, 1, 1});
                break;
            case 2: // Solar Defender
                ct.position = {0, 0, 20};
                SpawnPrimitive("Base", {0, 0, 0}, {2, 2, 2}, {1, 1, 0}, 0.9f, 0.1f);
                break;
            case 3: // Neon Tower
                ct.position = {0, 5, 20};
                SpawnPrimitive("Player", {0, 0, 0}, {1, 1, 1}, {1, 0, 1});
                for(int i=0; i<10; i++) SpawnPrimitive("Platform", {(float)(rand()%10-5), (float)i*4, 0}, {3, 0.5f, 2}, {0, 1, 0});
                break;
            case 4: // Trench Fighter
                ct.position = {0, 0, 5};
                SpawnPrimitive("Ship", {0, -2, 0}, {1, 0.5f, 1.5f}, {1, 1, 1});
                break;
            case 5: // Vortex Pong
                ct.position = {0, 15, 20};
                ct.rotation = glm::quat(glm::vec3(glm::radians(-45.0f), 0, 0));
                SpawnPrimitive("PaddleL", {-10, 0, 0}, {1, 1, 4}, {0, 1, 1});
                SpawnPrimitive("PaddleR", {10, 0, 0}, {1, 1, 4}, {1, 0, 1});
                SpawnPrimitive("Ball", {0, 0, 0}, {0.5f, 0.5f, 0.5f}, {1, 1, 1});
                break;
            case 6: // Bullet Storm
                ct.position = {0, 0, 25};
                SpawnPrimitive("Player", {0, 0, 0}, {1, 1, 1}, {1, 1, 1});
                break;
            case 7: // Crystal Cracker
                ct.position = {0, 10, 20};
                ct.rotation = glm::quat(glm::vec3(glm::radians(-30.0f), 0, 0));
                for(int i=0; i<20; i++) SpawnPrimitive("Crystal", {(float)(i%5*3-6), 0, (float)(i/5*2-10)}, {1, 1, 1}, {0.5f, 0, 1}, 0.8f);
                break;
            case 8: // Cyber Maze
                ct.position = {0, 30, 0};
                ct.rotation = glm::quat(glm::vec3(glm::radians(-90.0f), 0, 0));
                SpawnPrimitive("Player", {0, 0, 0}, {0.8f, 0.8f, 0.8f}, {1, 1, 0});
                break;
            case 9: // Lava Escape
                ct.position = {0, 5, 25};
                SpawnPrimitive("Player", {0, 0, 0}, {1, 1, 1}, {0, 1, 0});
                SpawnPrimitive("Lava", {0, -5, 0}, {50, 1, 50}, {1, 0.2f, 0}, 0.0f, 0.1f);
                break;
            case 10: // Nexus Arena
                ct.position = {0, 20, 20};
                ct.rotation = glm::quat(glm::vec3(glm::radians(-45.0f), 0, 0));
                SpawnPrimitive("Player", {0, 0, 0}, {1, 1, 1}, {1, 1, 1});
                break;
        }
    }

    void Arcade25DModule::Update(float dt) {
        if (m_currentGame == 0) return;
        m_totalTime += dt;

        UpdateGame(m_currentGame, dt);
    }

    void Arcade25DModule::UpdateGame(int index, float dt) {
        // Simplified Logic for 10 games
        auto view = m_arcadeRegistry.view<TransformComponent, TagComponent>();
        
        switch(index) {
            case 1: // Voxel Rush
                for(auto e : view) {
                    auto& t = view.get<TransformComponent>(e);
                    auto& tag = view.get<TagComponent>(e);
                    if (tag.tag == "Player") {
                        if (InputSystem::IsKeyPressed(SDL_SCANCODE_A)) t.position.x -= 10 * dt;
                        if (InputSystem::IsKeyPressed(SDL_SCANCODE_D)) t.position.x += 10 * dt;
                    } else if (tag.tag == "Obstacle") {
                        t.position.z += 15 * dt;
                        if (t.position.z > 20) m_arcadeRegistry.destroy(e);
                    }
                }
                if (rand()%10 == 0) SpawnPrimitive("Obstacle", {(float)(rand()%20-10), 0, -50}, {2, 2, 2}, {1, 0, 0});
                break;
            
            case 2: // Solar Defender
                for(auto e : view) {
                    auto& t = view.get<TransformComponent>(e);
                    if (view.get<TagComponent>(e).tag == "Enemy") {
                        t.position -= glm::normalize(t.position) * 5.0f * dt;
                        if (glm::length(t.position) < 2.0f) { m_arcadeRegistry.destroy(e); Engine::Get().GetAudio().PlayNote(100, 0.1f, WaveType::Noise); }
                    }
                }
                if (rand()%30 == 0) SpawnPrimitive("Enemy", glm::normalize(glm::vec3(rand()%10-5, rand()%10-5, 0)) * 30.0f, {1, 1, 1}, {1, 0.5f, 0});
                break;

            case 3: // Neon Tower
                for(auto e : view) {
                    auto& t = view.get<TransformComponent>(e);
                    if (view.get<TagComponent>(e).tag == "Player") {
                        static float vy = 0;
                        vy -= 20 * dt;
                        t.position.y += vy * dt;
                        if (InputSystem::IsKeyPressed(SDL_SCANCODE_A)) t.position.x -= 8 * dt;
                        if (InputSystem::IsKeyPressed(SDL_SCANCODE_D)) t.position.x += 8 * dt;
                        if (t.position.y < -10) { t.position.y = 0; vy = 15; }
                    }
                }
                break;

            case 4: // Trench Fighter
                for(auto e : view) {
                    auto& t = view.get<TransformComponent>(e);
                    auto& tag = view.get<TagComponent>(e);
                    if (tag.tag == "Ship") {
                        if (InputSystem::IsKeyPressed(SDL_SCANCODE_A)) t.position.x -= 15 * dt;
                        if (InputSystem::IsKeyPressed(SDL_SCANCODE_D)) t.position.x += 15 * dt;
                    }
                    if (tag.tag == "Wall") {
                        t.position.z += 20 * dt;
                        if (t.position.z > 10) t.position.z = -100;
                    }
                }
                if (m_totalTime < 0.1f) {
                    for(int i=0; i<20; i++) {
                        SpawnPrimitive("Wall", {-5, 0, (float)-i*10}, {1, 20, 10}, {0.2f, 0.2f, 0.3f});
                        SpawnPrimitive("Wall", {5, 0, (float)-i*10}, {1, 20, 10}, {0.2f, 0.2f, 0.3f});
                    }
                }
                break;

            case 5: // Vortex Pong
            {
                static glm::vec3 ballVel = {10, 0, 10};
                for(auto e : view) {
                    auto& t = view.get<TransformComponent>(e);
                    auto& tag = view.get<TagComponent>(e);
                    if (tag.tag == "PaddleL") { if (InputSystem::IsKeyPressed(SDL_SCANCODE_W)) t.position.z -= 15 * dt; if (InputSystem::IsKeyPressed(SDL_SCANCODE_S)) t.position.z += 15 * dt; }
                    if (tag.tag == "Ball") {
                        t.position += ballVel * dt;
                        if (std::abs(t.position.x) > 10) ballVel.x *= -1;
                        if (std::abs(t.position.z) > 15) ballVel.z *= -1;
                    }
                }
                break;
            }

            case 6: // Bullet Storm
                for(auto e : view) {
                    auto& t = view.get<TransformComponent>(e);
                    if (view.get<TagComponent>(e).tag == "Bullet") {
                        t.position.y -= 10 * dt;
                        if (t.position.y < -15) m_arcadeRegistry.destroy(e);
                    }
                }
                if (rand()%5 == 0) SpawnPrimitive("Bullet", {(float)(sin(m_totalTime*5)*10), 15, 0}, {0.3f, 0.3f, 0.3f}, {1, 0, 0}, 1.0f, 0.0f);
                break;

            case 7: // Crystal Cracker
                // Static scene for now, rotation logic
                for(auto e : view) {
                    auto& t = view.get<TransformComponent>(e);
                    if (view.get<TagComponent>(e).tag == "Crystal") t.rotation *= glm::quat(glm::vec3(0, dt, 0));
                }
                break;

            case 8: // Cyber Maze
                for(auto e : view) {
                    auto& t = view.get<TransformComponent>(e);
                    if (view.get<TagComponent>(e).tag == "Player") {
                        if (InputSystem::IsKeyPressed(SDL_SCANCODE_W)) t.position.z -= 10 * dt;
                        if (InputSystem::IsKeyPressed(SDL_SCANCODE_S)) t.position.z += 10 * dt;
                        if (InputSystem::IsKeyPressed(SDL_SCANCODE_A)) t.position.x -= 10 * dt;
                        if (InputSystem::IsKeyPressed(SDL_SCANCODE_D)) t.position.x += 10 * dt;
                    }
                }
                break;

            case 9: // Lava Escape
                for(auto e : view) {
                    auto& t = view.get<TransformComponent>(e);
                    if (view.get<TagComponent>(e).tag == "Lava") t.position.y += 0.5f * dt;
                }
                break;

            case 10: // Nexus Arena
                for(auto e : view) {
                    auto& t = view.get<TransformComponent>(e);
                    if (view.get<TagComponent>(e).tag == "Player") {
                        t.rotation *= glm::quat(glm::vec3(0, dt * 2, 0));
                    }
                }
                break;
        }
    }

    void Arcade25DModule::Render() {
        if (m_currentGame == 0) return;
        Engine::Get().GetRenderer().RenderRegistry(m_arcadeRegistry);
    }

    void Arcade25DModule::RenderUI() {
        ImGui::Begin("2.5D Arcade Suite");
        if (m_currentGame == 0) {
            RenderMenu();
        } else {
            if (ImGui::Button("Back to Menu")) m_currentGame = 0;
            ImGui::Separator();
            ImGui::Text("Playing: Game %d", m_currentGame);
            ImGui::Text("Time: %.2f", m_totalTime);
        }
        ImGui::End();
    }

    void Arcade25DModule::RenderMenu() {
        ImGui::Text("Select a 2.5D Experience:");
        ImGui::Separator();
        const char* games[] = {
            "1. Voxel Rush (Runner)",
            "2. Solar Defender (Orbit)",
            "3. Neon Tower (Platformer)",
            "4. Trench Fighter (Shooter)",
            "5. Vortex Pong (3D Pong)",
            "6. Bullet Storm (Bullet Hell)",
            "7. Crystal Cracker (Breakout 3D)",
            "8. Cyber Maze (Top-Down)",
            "9. Lava Escape (Survival)",
            "10. Nexus Arena (Arena)"
        };

        for(int i=1; i<=10; i++) {
            if (ImGui::Button(games[i-1], ImVec2(-1, 35))) {
                InitGame(i);
            }
        }
    }
}
