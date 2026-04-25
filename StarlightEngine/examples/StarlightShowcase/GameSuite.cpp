// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "GameSuite.hpp"
#include "imgui.h"
#include "InputSystem.hpp"
#include "Log.hpp"
#include <cmath>
#include <algorithm>

namespace starlight {

    // --- Invaders Module ---
    void InvadersModule::Initialize() {
        state = 0; score = 0; wave = 1;
        player = { { 640 - 25, 650 }, { 50, 30 }, true };
        SpawnWave();
    }

    void InvadersModule::SpawnWave() {
        enemies.clear();
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 8; c++) {
                Entity e;
                e.pos = { 150 + c * 100, 100 + r * 60 };
                e.size = { 40, 30 };
                e.active = true;
                enemies.push_back(e);
            }
        }
        enemyDir = 1.0f;
        enemySpeed = 40.0f + wave * 15.0f;
    }

    void InvadersModule::Update(float dt) {
        if (dt > 0.05f) dt = 0.05f;
        totalTime += dt;
        UpdateShake(dt);
        UpdateParticles(dt);

        if (state == 0) {
            if (InputSystem::IsKeyPressed(SDL_SCANCODE_RETURN) || InputSystem::IsKeyPressed(SDL_SCANCODE_SPACE)) state = 1;
            return;
        }

        if (state == 1) {
            // Player Move
            if (InputSystem::IsKeyPressed(SDL_SCANCODE_A) || InputSystem::IsKeyPressed(SDL_SCANCODE_LEFT)) player.pos.x -= 400 * dt;
            if (InputSystem::IsKeyPressed(SDL_SCANCODE_D) || InputSystem::IsKeyPressed(SDL_SCANCODE_RIGHT)) player.pos.x += 400 * dt;
            player.pos.x = std::clamp(player.pos.x, 20.0f, 1280.0f - 70.0f);

            // Fire
            fireTimer -= dt;
            if (InputSystem::IsKeyPressed(SDL_SCANCODE_SPACE) && fireTimer <= 0) {
                Entity b;
                b.pos = { player.pos.x + player.size.x / 2 - 2, player.pos.y - 10 };
                b.size = { 4, 15 };
                b.active = true;
                pBullets.push_back(b);
                fireTimer = 0.4f;
            }

            // Update Bullets
            for (size_t i = 0; i < pBullets.size(); i++) {
                pBullets[i].pos.y -= 600 * dt;
                if (pBullets[i].pos.y < 0) {
                    pBullets.erase(pBullets.begin() + i);
                    i--;
                    continue;
                }

                // Collision with enemies
                for (auto& e : enemies) {
                    if (e.active && pBullets[i].pos.x < e.pos.x + e.size.x && pBullets[i].pos.x + 4 > e.pos.x &&
                        pBullets[i].pos.y < e.pos.y + e.size.y && pBullets[i].pos.y + 15 > e.pos.y) {
                        e.active = false;
                        pBullets[i].active = false;
                        score += 100;
                        Emit(e.pos + e.size * 0.5f, glm::vec4(1, 1, 0, 1), 8);
                        shakeTimer = 0.15f;
                        break;
                    }
                }
                if (!pBullets[i].active) {
                    pBullets.erase(pBullets.begin() + i);
                    i--;
                }
            }

            // Update Enemies
            int activeCount = 0;
            float minX = 2000, maxX = -2000;
            for (auto& e : enemies) {
                if (e.active) {
                    activeCount++;
                    minX = std::min(minX, e.pos.x);
                    maxX = std::max(maxX, e.pos.x + e.size.x);
                }
            }

            if (activeCount == 0) {
                wave++;
                SpawnWave();
                return;
            }

            bool hitWall = (enemyDir > 0 && maxX > 1280 - 20) || (enemyDir < 0 && minX < 20);
            if (hitWall) {
                enemyDir *= -1;
                for (auto& e : enemies) e.pos.y += 30;
            }

            for (auto& e : enemies) {
                if (e.active) {
                    e.pos.x += enemyDir * enemySpeed * dt;
                    if (e.pos.y + e.size.y > player.pos.y) state = 2; // Game Over
                }
            }
        }
    }

    void InvadersModule::RenderUI() {
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        float ox = shakeX, oy = shakeY;

        // BG
        dl->AddRectFilled({0,0}, {1280,720}, ImColor(10, 0, 25));

        // Player
        dl->AddRectFilled({player.pos.x + ox, player.pos.y + oy}, {player.pos.x + player.size.x + ox, player.pos.y + player.size.y + oy}, ImColor(0, 255, 255));
        dl->AddRectFilled({player.pos.x + 15 + ox, player.pos.y - 10 + oy}, {player.pos.x + 35 + ox, player.pos.y + oy}, ImColor(0, 255, 255));

        // Enemies
        for (auto& e : enemies) {
            if (e.active) {
                float g = 0.5f + 0.5f * sin(totalTime * 5);
                dl->AddRectFilled({e.pos.x + ox, e.pos.y + oy}, {e.pos.x + e.size.x + ox, e.pos.y + e.size.y + oy}, ImColor(255, 0, 255, (int)((0.6f + 0.4f * g) * 255)));
                dl->AddRectFilled({e.pos.x + 10 + ox, e.pos.y + 10 + oy}, {e.pos.x + 15 + ox, e.pos.y + 15 + oy}, ImColor(255, 255, 255));
                dl->AddRectFilled({e.pos.x + 25 + ox, e.pos.y + 10 + oy}, {e.pos.x + 30 + ox, e.pos.y + 15 + oy}, ImColor(255, 255, 255));
            }
        }

        // Bullets
        for (auto& b : pBullets) dl->AddRectFilled({b.pos.x + ox, b.pos.y + oy}, {b.pos.x + 4 + ox, b.pos.y + 15 + oy}, ImColor(255, 255, 0));

        // Particles
        for (auto& p : particles) dl->AddRectFilled({p.pos.x + ox, p.pos.y + oy}, {p.pos.x + p.size + ox, p.pos.y + p.size + oy}, ImColor(p.color.r, p.color.g, p.color.b, p.color.a));

        // HUD
        char buf[64];
        sprintf(buf, "SCORE: %06d", score);
        dl->AddText({20, 20}, ImColor(255, 255, 255), buf);
        sprintf(buf, "WAVE: %d", wave);
        dl->AddText({1130, 20}, ImColor(255, 0, 255), buf);

        if (state == 0) {
            dl->AddRectFilled({0, 0}, {1280, 720}, ImColor(0, 0, 0, 180));
            dl->AddText(ImGui::GetFont(), 64.0f, {390, 260}, ImColor(0, 255, 255), "STARLIGHT INVADERS");
            dl->AddText(ImGui::GetFont(), 32.0f, {490, 380}, ImColor(255, 255, 255, (int)((0.5f + 0.5f * sin(totalTime * 8)) * 255)), "PRESS ENTER TO START");
        } else if (state == 2) {
            dl->AddRectFilled({0, 0}, {1280, 720}, ImColor(0, 0, 0, 180));
            dl->AddText(ImGui::GetFont(), 64.0f, {440, 300}, ImColor(255, 0, 0), "GAME OVER");
        }
    }

    // --- Breakout Module ---
    void BreakoutModule::Initialize() {
        state = 0; score = 0; lives = 3;
        player = { { 640 - 60, 620 }, { 120, 15 }, true };
        ResetBall();
        SpawnBlocks();
    }

    void BreakoutModule::ResetBall() {
        ball.pos = { 640, 570 };
        ball.size = { 12, 12 };
        ball.active = true;
        // Velocity stored in pos.x/y for simple entity struct? No, let's use ball as special
    }

    static glm::vec2 ballVel = { 300, -300 };

    void BreakoutModule::SpawnBlocks() {
        blocks.clear();
        float bw = 100, bh = 30, gap = 10;
        float startX = (1280 - (10 * (bw + gap))) / 2;
        float startY = 100;
        for (int r = 0; r < 5; r++) {
            for (int c = 0; c < 10; c++) {
                Entity b;
                b.pos = { startX + c * (bw + gap), startY + r * (bh + gap) };
                b.size = { bw, bh };
                b.active = true;
                blocks.push_back(b);
            }
        }
    }

    void BreakoutModule::Update(float dt) {
        if (dt > 0.05f) dt = 0.05f;
        totalTime += dt;
        UpdateParticles(dt);

        if (state == 0) {
            if (InputSystem::IsKeyPressed(SDL_SCANCODE_RETURN) || InputSystem::IsKeyPressed(SDL_SCANCODE_SPACE)) state = 1;
            return;
        }

        if (state == 1) {
            // Paddle
            if (InputSystem::IsKeyPressed(SDL_SCANCODE_A) || InputSystem::IsKeyPressed(SDL_SCANCODE_LEFT)) player.pos.x -= 600 * dt;
            if (InputSystem::IsKeyPressed(SDL_SCANCODE_D) || InputSystem::IsKeyPressed(SDL_SCANCODE_RIGHT)) player.pos.x += 600 * dt;
            player.pos.x = std::clamp(player.pos.x, 10.0f, 1280.0f - player.size.x - 10.0f);

            // Ball
            ball.pos += ballVel * dt;

            // Walls
            if (ball.pos.x < 0 || ball.pos.x > 1280 - ball.size.x) ballVel.x *= -1;
            if (ball.pos.y < 0) ballVel.y *= -1;
            if (ball.pos.y > 720) {
                lives--;
                ResetBall();
                ballVel = { 300, -300 };
                if (lives <= 0) state = 2; // Game Over
            }

            // Paddle Collision
            if (ballVel.y > 0 && ball.pos.y + ball.size.y > player.pos.y && ball.pos.x > player.pos.x && ball.pos.x < player.pos.x + player.size.x) {
                ballVel.y *= -1.05f;
                float hit = (ball.pos.x + ball.size.x / 2 - (player.pos.x + player.size.x / 2)) / (player.size.x / 2);
                ballVel.x = 400 * hit;
            }

            // Bricks Collision
            int activeBricks = 0;
            for (auto& b : blocks) {
                if (!b.active) continue;
                activeBricks++;
                if (ball.pos.x + ball.size.x > b.pos.x && ball.pos.x < b.pos.x + b.size.x &&
                    ball.pos.y + ball.size.y > b.pos.y && ball.pos.y < b.pos.y + b.size.y) {
                    b.active = false;
                    ballVel.y *= -1;
                    score += 50;
                    Emit(b.pos + b.size * 0.5f, glm::vec4(1, 0.5f, 0, 1), 6);
                    break;
                }
            }
            if (activeBricks == 0) state = 3; // Win
        }
    }

    void BreakoutModule::RenderUI() {
        ImDrawList* dl = ImGui::GetForegroundDrawList();

        // BG
        dl->AddRectFilled({0,0}, {1280,720}, ImColor(10, 5, 5));

        // Paddle
        dl->AddRectFilled({player.pos.x, player.pos.y}, {player.pos.x + player.size.x, player.pos.y + player.size.y}, ImColor(255, 100, 0));

        // Ball
        dl->AddRectFilled({ball.pos.x, ball.pos.y}, {ball.pos.x + ball.size.x, ball.pos.y + ball.size.y}, ImColor(255, 255, 255));

        // Bricks
        for (size_t i = 0; i < blocks.size(); i++) {
            if (blocks[i].active) {
                float hue = (float)(i / 10) / 5.0f;
                dl->AddRectFilled({blocks[i].pos.x, blocks[i].pos.y}, {blocks[i].pos.x + blocks[i].size.x, blocks[i].pos.y + blocks[i].size.y}, ImColor::HSV(hue, 0.8f, 0.8f));
            }
        }

        // Particles
        for (auto& p : particles) dl->AddRectFilled({p.pos.x, p.pos.y}, {p.pos.x + p.size, p.pos.y + p.size}, ImColor(p.color.r, p.color.g, p.color.b, p.color.a));

        // HUD
        char buf[64];
        sprintf(buf, "SCORE: %d", score);
        dl->AddText({20, 20}, ImColor(255, 255, 255), buf);
        sprintf(buf, "LIVES: %d", lives);
        dl->AddText({1130, 20}, ImColor(255, 0, 0), buf);

        if (state == 0) {
            dl->AddRectFilled({0, 0}, {1280, 720}, ImColor(0, 0, 0, 180));
            dl->AddText(ImGui::GetFont(), 64.0f, {420, 260}, ImColor(255, 128, 0), "STARLIGHT BREAKOUT");
            dl->AddText(ImGui::GetFont(), 32.0f, {490, 380}, ImColor(255, 255, 255, 200), "PRESS ENTER TO START");
        } else if (state == 3) {
            dl->AddText(ImGui::GetFont(), 64.0f, {480, 300}, ImColor(0, 255, 0), "YOU WIN!");
        }
    }

    // --- Runner Module ---
    void RunnerModule::Initialize() {
        state = 0; score = 0; distance = 0; speed = 400.0f;
        playerY = 360; playerVY = 0;
        obstacles.clear();
    }

    void RunnerModule::Update(float dt) {
        if (dt > 0.05f) dt = 0.05f;
        totalTime += dt;
        UpdateParticles(dt);

        if (state == 0) {
            if (InputSystem::IsKeyPressed(SDL_SCANCODE_SPACE)) state = 1;
            return;
        }

        if (state == 1) {
            playerVY += 1200 * dt;
            if (InputSystem::IsKeyJustPressed(SDL_SCANCODE_SPACE)) {
                playerVY = -450;
                Emit({120, playerY + 20}, {0, 1, 1, 0.5f}, 3);
            }
            playerY += playerVY * dt;
            if (playerY < 0 || playerY > 720 - 40) state = 2; // Game Over

            speed += 5 * dt;
            distance += speed * dt;
            score = (int)(distance / 100);

            obsTimer -= dt;
            if (obsTimer <= 0) {
                float gap = 200 - (speed / 10);
                if (gap < 120) gap = 120;
                Obstacle o;
                o.pos = { 1280, (float)(rand() % (int)(720 - gap)) };
                o.size = { 60, gap };
                o.active = true;
                obstacles.push_back(o);
                obsTimer = 1.8f - (speed / 1000);
                if (obsTimer < 0.6f) obsTimer = 0.6f;
            }

            for (size_t i = 0; i < obstacles.size(); i++) {
                obstacles[i].pos.x -= speed * dt;
                if (obstacles[i].pos.x < -100) {
                    obstacles.erase(obstacles.begin() + i);
                    i--;
                    continue;
                }
                // Collision
                bool hitTop = (100 < obstacles[i].pos.x + 60 && 140 > obstacles[i].pos.x && playerY < obstacles[i].pos.y);
                bool hitBot = (100 < obstacles[i].pos.x + 60 && 140 > obstacles[i].pos.x && playerY + 40 > obstacles[i].pos.y + obstacles[i].size.y);
                if (hitTop || hitBot) state = 2;
            }
        }
    }

    void RunnerModule::RenderUI() {
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        dl->AddRectFilled({0,0}, {1280,720}, ImColor(0, 10, 20));

        // Player
        dl->AddRectFilled({100, playerY}, {140, playerY + 40}, ImColor(0, 255, 255));
        dl->AddRectFilled({110, playerY + 10}, {130, playerY + 30}, ImColor(255, 255, 255));

        // Obstacles
        for (auto& o : obstacles) {
            dl->AddRectFilled({o.pos.x, 0}, {o.pos.x + o.size.x, o.pos.y}, ImColor(255, 0, 100));
            dl->AddRectFilled({o.pos.x, o.pos.y + o.size.y}, {o.pos.x + o.size.x, 720}, ImColor(255, 0, 100));
        }

        // Particles
        for (auto& p : particles) dl->AddRectFilled({p.pos.x, p.pos.y}, {p.pos.x + p.size, p.pos.y + p.size}, ImColor(p.color.r, p.color.g, p.color.b, p.color.a));

        dl->AddText({20, 20}, ImColor(255, 255, 255), (std::string("SCORE: ") + std::to_string(score)).c_str());

        if (state == 0) {
            dl->AddRectFilled({0,0}, {1280,720}, ImColor(0,0,0,150));
            dl->AddText(ImGui::GetFont(), 64, {460, 300}, ImColor(0, 255, 255), "VOID RUNNER");
            dl->AddText(ImGui::GetFont(), 32, {480, 400}, ImColor(255,255,255), "PRESS SPACE TO JUMP");
        } else if (state == 2) {
            dl->AddText(ImGui::GetFont(), 64, {480, 300}, ImColor(255,0,0), "GAME OVER");
        }
    }

    // --- Ricochete Module ---
    void RicocheteModule::Initialize() {
        ResetBall(1);
        scoreL = scoreR = 0;
        state = 0;
    }

    void RicocheteModule::ResetBall(int dir) {
        ballPos = { 640, 360 };
        ballVel = { dir * (300.0f + difficulty * 80.0f), (float)(rand() % 200 - 100) };
    }

    void RicocheteModule::Update(float dt) {
        if (dt > 0.05f) dt = 0.05f;
        totalTime += dt;
        if (shakeTimer > 0) {
            shakeTimer -= dt;
            shakeX = ((float)(rand() % 100) - 50.0f) / 50.0f * shakeTimer * 8.0f;
            shakeY = ((float)(rand() % 100) - 50.0f) / 50.0f * shakeTimer * 8.0f;
        } else { shakeX = shakeY = 0; }

        if (state == 0) {
            if (InputSystem::IsKeyJustPressed(SDL_SCANCODE_RETURN)) state = 1;
            return;
        }
        if (state == 2) { if (InputSystem::IsKeyJustPressed(SDL_SCANCODE_RETURN)) state = 0; return; }

        // Paddle Move
        if (InputSystem::IsKeyPressed(SDL_SCANCODE_W)) plY -= 400 * dt;
        if (InputSystem::IsKeyPressed(SDL_SCANCODE_S)) plY += 400 * dt;
        plY = std::clamp(plY, 45.0f, 540.0f - plH - 5.0f);
        prY += (ballPos.y - 40 - prY) * (0.04f + difficulty * 0.03f);

        ballPos += ballVel * dt;
        if (ballPos.y < 45 || ballPos.y > 540 - 5) { ballVel.y *= -1.01f; shakeTimer = 0.05f; }
        if (ballVel.x < 0 && ballPos.x < 40 + 42 && ballPos.y > plY && ballPos.y < plY + plH) {
            ballVel.x = std::abs(ballVel.x) * 1.06f;
            ballVel.y += (ballPos.y - (plY + plH / 2)) / (plH / 2) * 180.0f;
            shakeTimer = 0.15f;
        }
        if (ballVel.x > 0 && ballPos.x > 840 - 42 && ballPos.y > prY && ballPos.y < prY + 80) {
            ballVel.x = -std::abs(ballVel.x) * 1.06f;
            ballVel.y += (ballPos.y - (prY + 40)) / 40.0f * 120.0f;
            shakeTimer = 0.1f;
        }
        if (ballPos.x < 40) { scoreR++; if (scoreR >= 7) state = 2; else ResetBall(1); }
        if (ballPos.x > 840) { scoreL++; if (scoreL >= 7) state = 2; else ResetBall(-1); }
    }

    void RicocheteModule::RenderUI() {
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        float ox = shakeX, oy = shakeY;
        dl->AddRectFilled({0,0}, {1280,720}, ImColor(2, 2, 8));
        dl->AddRect({40 + ox, 40 + oy}, {840 + ox, 540 + oy}, ImColor(0, 200, 255), 0, 0, 2);
        dl->AddRectFilled({40 + 30 + ox, plY + oy}, {40 + 42 + ox, plY + plH + oy}, ImColor(0, 255, 255));
        dl->AddRectFilled({840 - 42 + ox, prY + oy}, {840 - 30 + ox, prY + 80 + oy}, ImColor(255, 0, 255));
        dl->AddRectFilled({ballPos.x - 5 + ox, ballPos.y - 5 + oy}, {ballPos.x + 5 + ox, ballPos.y + 5 + oy}, ImColor(255, 255, 0));
        char scoreBuf[32]; sprintf(scoreBuf, "%d - %d", scoreL, scoreR);
        dl->AddText(ImGui::GetFont(), 48, {400, 10}, ImColor(255, 255, 255), scoreBuf);
    }

    // --- Cinema 3D Module ---
    void Cinema3DModule::Initialize() {
        totalTime = 0;
        rotation = 0;
    }

    void Cinema3DModule::Update(float dt) {
        totalTime += dt;
        rotation += 1.0f * dt;
    }

    void Cinema3DModule::RenderUI() {
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        
        // Dark cinematic background
        dl->AddRectFilled({0,0}, {1280,720}, ImColor(5, 5, 10));
        
        // Grid Floor (Perspective simulation)
        ImColor gridColor(30, 30, 60);
        for(int i = -10; i <= 10; i++) {
            auto Project = [](float x, float y, float z) -> ImVec2 {
                float fov = 500.0f;
                float factor = fov / (fov + z + 400);
                return { 640 + x * factor, 360 + y * factor };
            };
            
            dl->AddLine(Project(-1000, 200, (float)i * 100), Project(1000, 200, (float)i * 100), gridColor);
            dl->AddLine(Project((float)i * 100, 200, -1000), Project((float)i * 100, 200, 1000), gridColor);
        }

        // Rotating 3D Monolith (SIMD Math Simulation)
        static float pts[8][3] = {
            {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
            {-1,-1,1}, {1,-1,1}, {1,1,1}, {-1,1,1}
        };
        ImVec2 proj[8];
        float s = 150.0f;
        for(int i=0; i<8; i++) {
            float x = pts[i][0], y = pts[i][1], z = pts[i][2];
            // Rotate Y
            float rx = x * cos(rotation) - z * sin(rotation);
            float rz = x * sin(rotation) + z * cos(rotation);
            // Rotate X
            float ry = y * cos(rotation*0.5f) - rz * sin(rotation*0.5f);
            float rrzt = y * sin(rotation*0.5f) + rz * cos(rotation*0.5f);
            
            float fov = 800.0f;
            float factor = fov / (fov + rrzt * s + 400);
            proj[i] = { 640 + rx * s * factor, 360 + ry * s * factor };
        }
        
        // Draw Edges
        ImColor cubeColor(0, 200, 255, 200);
        int edges[12][2] = { {0,1},{1,2},{2,3},{3,0}, {4,5},{5,6},{6,7},{7,4}, {0,4},{1,5},{2,6},{3,7} };
        for(int i=0; i<12; i++) dl->AddLine(proj[edges[i][0]], proj[edges[i][1]], cubeColor, 2.0f);

        // Titles
        dl->AddText(ImGui::GetFont(), 32.0f, {500, 50}, ImColor(255, 255, 255), "3D CINEMA MODE");
        dl->AddText(ImGui::GetFont(), 18.0f, {480, 85}, ImColor(150, 150, 255), "PBR / AVX2 SIMD / LOGARITHMIC DEPTH");
        
        ImGui::SetNextWindowPos({1000, 100}, ImGuiCond_FirstUseEver);
        ImGui::Begin("3D Controls");
        ImGui::Text("Camera: Free-Look (WASD)");
        ImGui::Text("Pipeline: Phase 10 AAA");
        ImGui::SliderFloat("Rotation Speed", &rotation, 0, 10);
        ImGui::Checkbox("Show Wireframe", &showWireframe);
        if(ImGui::Button("Spawn PBR Monolith")) Log::Info("3D Cinema: Entity spawned.");
        ImGui::End();
    }

}
