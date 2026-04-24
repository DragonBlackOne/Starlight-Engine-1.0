#include "Engine.hpp"
#include "InputSystem.hpp"
#include "Log.hpp"
#include "Renderer.hpp"
#include "Components.hpp"
#include <SDL2/SDL.h>
#include "imgui.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>

using namespace titan;

// =========================================================================
//  RICOCHETE - Pong +18 (2D ImGui Edition)
//  Baseado no game_pong_deluxe.c do Starlight Engine
//  Rodando dentro do editor TitanEngine via ImGui overlay
// =========================================================================
class RicocheteModule : public EngineModule {
public:
    std::string GetName() const override { return "Ricochete"; }

    // Arena
    static constexpr float ARENA_X = 40.0f;
    static constexpr float ARENA_Y = 40.0f;
    static constexpr float ARENA_W = 800.0f;
    static constexpr float ARENA_H = 500.0f;

    // Paddle
    float plY = 200.0f, prY = 200.0f;
    float plH = 80.0f, plH_target = 80.0f;
    static constexpr float PAD_W = 12.0f;
    static constexpr float PAD_SPD = 400.0f;

    // Ball
    float bx, by, bvx, bvy;
    float bsize = 10.0f;
    bool serving = true;
    float serveTimer = 1.5f;

    // Score
    int scoreL = 0, scoreR = 0;
    static constexpr int WIN_SCORE = 7;

    // FX
    float totalTime = 0.0f;
    float shakeTimer = 0.0f;
    float shakeX = 0.0f, shakeY = 0.0f;
    float chromatic = 0.0f;

    // Powerup
    float pwX = -100, pwY = -100, pwTimer = 5.0f;
    bool pwActive = false;

    // Particles
    struct Particle { float x, y, vx, vy, life, maxLife, size; ImU32 color; };
    Particle particles[200];
    int particleCount = 0;

    // State: 0=MENU, 1=PLAYING, 2=GAMEOVER
    int gameState = 0;
    int menuSel = 0;
    int difficulty = 1; // 0=easy,1=normal,2=hard

    void Initialize() override {
        Log::Info(">>> RICOCHETE PONG +18 - TitanEngine Edition <<<");
        ResetBall(1);
    }

    void ResetBall(int dir) {
        bx = ARENA_X + ARENA_W / 2.0f;
        by = ARENA_Y + ARENA_H / 2.0f;
        bvx = dir * (300.0f + difficulty * 80.0f);
        bvy = (float)(rand() % 200 - 100);
        serving = true;
        serveTimer = 1.0f;
    }

    void Emit(float x, float y, ImU32 col, int count) {
        for (int i = 0; i < count && particleCount < 200; i++) {
            Particle& p = particles[particleCount++];
            p.x = x; p.y = y;
            float a = ((rand() % 360) * 3.14159f) / 180.0f;
            float s = 60.0f + rand() % 120;
            p.vx = cosf(a) * s; p.vy = sinf(a) * s;
            p.life = 0.4f + (rand() % 40) / 100.0f;
            p.maxLife = p.life;
            p.size = 2.0f + rand() % 4;
            p.color = col;
        }
    }

    void Update(float dt) override {
        if (dt > 0.05f) dt = 0.05f;
        totalTime += dt;

        // Update particles
        for (int i = 0; i < particleCount;) {
            Particle& p = particles[i];
            p.life -= dt;
            if (p.life <= 0) { particles[i] = particles[--particleCount]; continue; }
            p.x += p.vx * dt;
            p.y += p.vy * dt;
            i++;
        }

        // Shake decay
        if (shakeTimer > 0) {
            shakeTimer -= dt;
            shakeX = ((rand() % 100 - 50) / 50.0f) * shakeTimer * 8.0f;
            shakeY = ((rand() % 100 - 50) / 50.0f) * shakeTimer * 8.0f;
        } else { shakeX = shakeY = 0; }

        if (chromatic > 0) chromatic -= dt;

        if (gameState == 0) {
            // Menu navigation
            if (InputSystem::IsKeyJustPressed(SDL_SCANCODE_W) || InputSystem::IsKeyJustPressed(SDL_SCANCODE_UP))
                menuSel = (menuSel + 2) % 3;
            if (InputSystem::IsKeyJustPressed(SDL_SCANCODE_S) || InputSystem::IsKeyJustPressed(SDL_SCANCODE_DOWN))
                menuSel = (menuSel + 1) % 3;
            if (InputSystem::IsKeyJustPressed(SDL_SCANCODE_RETURN) || InputSystem::IsKeyJustPressed(SDL_SCANCODE_SPACE)) {
                if (menuSel == 0) { difficulty = 0; gameState = 1; scoreL = scoreR = 0; ResetBall(1); }
                if (menuSel == 1) { difficulty = 1; gameState = 1; scoreL = scoreR = 0; ResetBall(1); }
                if (menuSel == 2) { difficulty = 2; gameState = 1; scoreL = scoreR = 0; ResetBall(1); }
            }
            return;
        }

        if (gameState == 2) {
            if (InputSystem::IsKeyJustPressed(SDL_SCANCODE_RETURN) || InputSystem::IsKeyJustPressed(SDL_SCANCODE_SPACE)) {
                gameState = 0;
            }
            return;
        }

        // === PLAYING ===
        // Paddle smoothing
        plH += (plH_target - plH) * dt * 5.0f;

        // Serve
        if (serving) {
            serveTimer -= dt;
            if (serveTimer <= 0) serving = false;
            UpdatePaddles(dt);
            return;
        }

        UpdatePaddles(dt);

        // Ball sub-stepping
        int steps = 4;
        float sdt = dt / (float)steps;
        for (int s = 0; s < steps; s++) {
            bx += bvx * sdt;
            by += bvy * sdt;

            float topWall = ARENA_Y + 5;
            float botWall = ARENA_Y + ARENA_H - 5;

            // Wall bounce
            if (by < topWall) { by = topWall; bvy = fabs(bvy) * 1.01f; shakeTimer = 0.05f; }
            if (by > botWall) { by = botWall; bvy = -fabs(bvy) * 1.01f; shakeTimer = 0.05f; }

            // Left paddle hit
            float lx = ARENA_X + 30;
            if (bvx < 0 && bx < lx + PAD_W && bx > lx && by > plY && by < plY + plH) {
                float hit = (by - (plY + plH / 2.0f)) / (plH / 2.0f);
                bvx = fabs(bvx) * 1.06f;
                bvy += hit * 180.0f;
                Emit(bx, by, IM_COL32(0, 255, 255, 255), 8);
                shakeTimer = 0.15f;
                chromatic = 0.15f;
                break;
            }

            // Right paddle hit
            float rx = ARENA_X + ARENA_W - 30 - PAD_W;
            if (bvx > 0 && bx > rx && bx < rx + PAD_W && by > prY && by < prY + 80) {
                bvx = -fabs(bvx) * 1.06f;
                float hit = (by - (prY + 40.0f)) / 40.0f;
                bvy += hit * 120.0f;
                Emit(bx, by, IM_COL32(255, 0, 255, 255), 8);
                shakeTimer = 0.1f;
                chromatic = 0.1f;
                break;
            }
        }

        // Clamp ball speed
        float maxSpd = 800.0f + difficulty * 200.0f;
        if (fabs(bvx) > maxSpd) bvx = (bvx > 0 ? 1 : -1) * maxSpd;
        if (fabs(bvy) > maxSpd) bvy = (bvy > 0 ? 1 : -1) * maxSpd;

        // Powerup
        pwTimer -= dt;
        if (pwTimer <= 0 && !pwActive) {
            pwX = ARENA_X + 200 + rand() % 400;
            pwY = ARENA_Y + 50 + rand() % 400;
            pwActive = true;
            pwTimer = 8.0f;
        }
        if (pwActive) {
            float dx = bx - pwX, dy = by - pwY;
            if (sqrtf(dx * dx + dy * dy) < 25.0f) {
                plH_target = 140.0f;
                pwActive = false;
                Emit(pwX, pwY, IM_COL32(255, 255, 0, 255), 15);
            }
        }

        // Scoring
        if (bx < ARENA_X) {
            scoreR++;
            Emit(ARENA_X, by, IM_COL32(255, 50, 150, 255), 10);
            if (scoreR >= WIN_SCORE) { gameState = 2; return; }
            ResetBall(1); plH_target = 80.0f;
        }
        if (bx > ARENA_X + ARENA_W) {
            scoreL++;
            Emit(ARENA_X + ARENA_W, by, IM_COL32(50, 255, 150, 255), 10);
            if (scoreL >= WIN_SCORE) { gameState = 2; return; }
            ResetBall(-1); plH_target = 80.0f;
        }
    }

    void UpdatePaddles(float dt) {
        // Player
        if (InputSystem::IsKeyPressed(SDL_SCANCODE_W) || InputSystem::IsKeyPressed(SDL_SCANCODE_UP))
            plY -= PAD_SPD * dt;
        if (InputSystem::IsKeyPressed(SDL_SCANCODE_S) || InputSystem::IsKeyPressed(SDL_SCANCODE_DOWN))
            plY += PAD_SPD * dt;
        if (plY < ARENA_Y + 5) plY = ARENA_Y + 5;
        if (plY + plH > ARENA_Y + ARENA_H - 5) plY = ARENA_Y + ARENA_H - 5 - plH;

        // AI
        float aiSpd = 0.04f + difficulty * 0.03f;
        float aiTarget = by - 40.0f;
        prY += (aiTarget - prY) * aiSpd;
        if (prY < ARENA_Y + 5) prY = ARENA_Y + 5;
        if (prY + 80 > ARENA_Y + ARENA_H - 5) prY = ARENA_Y + ARENA_H - 85;
    }

    void RenderUI() override {
        // Render everything via ImGui overlay
        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        float ox = shakeX, oy = shakeY;

        if (gameState == 0) {
            // === MENU ===
            dl->AddRectFilled(ImVec2(0, 0), ImVec2(io.DisplaySize.x, io.DisplaySize.y), IM_COL32(2, 2, 8, 255));
            
            // Title
            const char* title = "RICOCHETE";
            dl->AddText(ImGui::GetFont(), 64, ImVec2(io.DisplaySize.x/2 - 180, 80), IM_COL32(0, 255, 255, 255), title);
            
            const char* sub = "Pong +18 - TitanEngine Edition";
            dl->AddText(ImVec2(io.DisplaySize.x/2 - 140, 155), IM_COL32(255, 0, 255, 180), sub);

            // Menu items
            const char* items[] = { "FACIL", "NORMAL", "DIFICIL" };
            ImU32 colors[] = { IM_COL32(0, 255, 100, 255), IM_COL32(255, 255, 0, 255), IM_COL32(255, 50, 50, 255) };
            for (int i = 0; i < 3; i++) {
                float yy = 280 + i * 60;
                ImU32 col = (menuSel == i) ? colors[i] : IM_COL32(100, 100, 100, 180);
                if (menuSel == i) {
                    dl->AddRectFilled(ImVec2(io.DisplaySize.x/2 - 100, yy - 5), ImVec2(io.DisplaySize.x/2 + 100, yy + 35), IM_COL32(255, 255, 255, 15));
                    dl->AddText(ImGui::GetFont(), 32, ImVec2(io.DisplaySize.x/2 - 50 - 20, yy), col, ">");
                }
                dl->AddText(ImGui::GetFont(), 32, ImVec2(io.DisplaySize.x/2 - 40, yy), col, items[i]);
            }

            dl->AddText(ImVec2(io.DisplaySize.x/2 - 120, 520), IM_COL32(255, 255, 255, 80), "W/S para navegar | ENTER para jogar");
            
            // Decorative particles
            float t = totalTime;
            for (int i = 0; i < 20; i++) {
                float px = io.DisplaySize.x/2 + sinf(t * 0.5f + i * 0.8f) * 300;
                float py = 350 + cosf(t * 0.7f + i * 1.2f) * 200;
                float alpha = 0.15f + 0.1f * sinf(t * 2 + i);
                dl->AddCircleFilled(ImVec2(px, py), 2 + sinf(t + i) * 1, IM_COL32(0, 200, 255, (int)(alpha * 255)));
            }
            return;
        }

        if (gameState == 2) {
            // === GAME OVER ===
            dl->AddRectFilled(ImVec2(0, 0), ImVec2(io.DisplaySize.x, io.DisplaySize.y), IM_COL32(2, 2, 8, 240));
            
            bool playerWon = scoreL >= WIN_SCORE;
            const char* msg = playerWon ? "VOCE VENCEU!" : "IA VENCEU!";
            ImU32 msgCol = playerWon ? IM_COL32(0, 255, 200, 255) : IM_COL32(255, 50, 100, 255);
            dl->AddText(ImGui::GetFont(), 64, ImVec2(io.DisplaySize.x/2 - 200, 200), msgCol, msg);
            
            char score[32]; snprintf(score, sizeof(score), "%d - %d", scoreL, scoreR);
            dl->AddText(ImGui::GetFont(), 48, ImVec2(io.DisplaySize.x/2 - 60, 300), IM_COL32(255, 255, 255, 255), score);
            
            dl->AddText(ImVec2(io.DisplaySize.x/2 - 100, 420), IM_COL32(255, 255, 255, 120), "ENTER para voltar ao menu");
            return;
        }

        // === PLAYING ===
        // Background
        dl->AddRectFilled(ImVec2(0, 0), ImVec2(io.DisplaySize.x, io.DisplaySize.y), IM_COL32(2, 2, 8, 255));

        // Pulsing grid
        float pulse = 0.03f + 0.05f * sinf(totalTime * 4.0f);
        ImU32 gridCol = IM_COL32(0, 80, 120, (int)(pulse * 255));
        for (float x = ARENA_X; x < ARENA_X + ARENA_W; x += 80)
            dl->AddLine(ImVec2(x + ox, ARENA_Y + oy), ImVec2(x + ox, ARENA_Y + ARENA_H + oy), gridCol);
        for (float y = ARENA_Y; y < ARENA_Y + ARENA_H; y += 80)
            dl->AddLine(ImVec2(ARENA_X + ox, y + oy), ImVec2(ARENA_X + ARENA_W + ox, y + oy), gridCol);

        // Arena border (neon glow)
        dl->AddRect(ImVec2(ARENA_X + ox - 2, ARENA_Y + oy - 2), 
                     ImVec2(ARENA_X + ARENA_W + ox + 2, ARENA_Y + ARENA_H + oy + 2), 
                     IM_COL32(0, 200, 255, 120), 0, 0, 2);
        dl->AddRect(ImVec2(ARENA_X + ox, ARENA_Y + oy), 
                     ImVec2(ARENA_X + ARENA_W + ox, ARENA_Y + ARENA_H + oy), 
                     IM_COL32(0, 200, 255, 60), 0, 0, 1);

        // Center line
        for (float y = ARENA_Y; y < ARENA_Y + ARENA_H; y += 20) {
            dl->AddRectFilled(ImVec2(ARENA_X + ARENA_W/2 - 1 + ox, y + oy), 
                              ImVec2(ARENA_X + ARENA_W/2 + 1 + ox, y + 10 + oy), 
                              IM_COL32(255, 255, 255, 40));
        }

        // Chromatic aberration on ball
        if (chromatic > 0) {
            float co = (rand() % 6) - 3;
            dl->AddRectFilled(ImVec2(bx - bsize/2 + co + ox, by - bsize/2 + oy), 
                              ImVec2(bx + bsize/2 + co + ox, by + bsize/2 + oy), IM_COL32(255, 0, 0, 120));
            dl->AddRectFilled(ImVec2(bx - bsize/2 - co + ox, by - bsize/2 + oy), 
                              ImVec2(bx + bsize/2 - co + ox, by + bsize/2 + oy), IM_COL32(0, 0, 255, 120));
        }

        // Shadows
        ImU32 shadowCol = IM_COL32(0, 0, 0, 80);
        dl->AddRectFilled(ImVec2(ARENA_X + 30 + 3 + ox, plY + 3 + oy), ImVec2(ARENA_X + 30 + PAD_W + 3 + ox, plY + plH + 3 + oy), shadowCol);
        dl->AddRectFilled(ImVec2(ARENA_X + ARENA_W - 30 - PAD_W + 3 + ox, prY + 3 + oy), ImVec2(ARENA_X + ARENA_W - 30 + 3 + ox, prY + 80 + 3 + oy), shadowCol);
        dl->AddRectFilled(ImVec2(bx - bsize/2 + 2 + ox, by - bsize/2 + 2 + oy), ImVec2(bx + bsize/2 + 2 + ox, by + bsize/2 + 2 + oy), shadowCol);

        // Left paddle (Cyan)
        dl->AddRectFilled(ImVec2(ARENA_X + 30 + ox, plY + oy), 
                          ImVec2(ARENA_X + 30 + PAD_W + ox, plY + plH + oy), 
                          IM_COL32(0, 255, 255, 255));
        // Glow
        dl->AddRectFilled(ImVec2(ARENA_X + 30 - 3 + ox, plY - 3 + oy), 
                          ImVec2(ARENA_X + 30 + PAD_W + 3 + ox, plY + plH + 3 + oy), 
                          IM_COL32(0, 255, 255, 30));

        // Right paddle (Magenta)
        dl->AddRectFilled(ImVec2(ARENA_X + ARENA_W - 30 - PAD_W + ox, prY + oy), 
                          ImVec2(ARENA_X + ARENA_W - 30 + ox, prY + 80 + oy), 
                          IM_COL32(255, 0, 255, 255));
        dl->AddRectFilled(ImVec2(ARENA_X + ARENA_W - 30 - PAD_W - 3 + ox, prY - 3 + oy), 
                          ImVec2(ARENA_X + ARENA_W - 30 + 3 + ox, prY + 83 + oy), 
                          IM_COL32(255, 0, 255, 30));

        // Ball (Yellow)
        dl->AddRectFilled(ImVec2(bx - bsize/2 + ox, by - bsize/2 + oy), 
                          ImVec2(bx + bsize/2 + ox, by + bsize/2 + oy), 
                          IM_COL32(255, 255, 0, 255));
        // Ball glow
        dl->AddCircleFilled(ImVec2(bx + ox, by + oy), bsize, IM_COL32(255, 255, 100, 40));

        // Powerup
        if (pwActive) {
            float s = 15.0f + 5.0f * sinf(totalTime * 8.0f);
            dl->AddRectFilled(ImVec2(pwX - s/2 + ox, pwY - s/2 + oy), ImVec2(pwX + s/2 + ox, pwY + s/2 + oy), IM_COL32(255, 255, 0, 200));
            dl->AddRect(ImVec2(pwX - s/2 - 2 + ox, pwY - s/2 - 2 + oy), ImVec2(pwX + s/2 + 2 + ox, pwY + s/2 + 2 + oy), IM_COL32(255, 255, 0, 80), 0, 0, 1);
            dl->AddText(ImVec2(pwX - 3 + ox, pwY - 7 + oy), IM_COL32(0, 0, 0, 255), "!");
        }

        // Score
        char score[32]; snprintf(score, sizeof(score), "%d", scoreL);
        dl->AddText(ImGui::GetFont(), 56, ImVec2(ARENA_X + ARENA_W/2 - 80, ARENA_Y - 5), IM_COL32(0, 255, 255, 200), score);
        snprintf(score, sizeof(score), "%d", scoreR);
        dl->AddText(ImGui::GetFont(), 56, ImVec2(ARENA_X + ARENA_W/2 + 45, ARENA_Y - 5), IM_COL32(255, 0, 255, 200), score);

        // Serve indicator
        if (serving) {
            float alpha = 0.5f + 0.5f * sinf(totalTime * 6.0f);
            dl->AddText(ImVec2(ARENA_X + ARENA_W/2 - 20, ARENA_Y + ARENA_H/2 + 30), 
                        IM_COL32(255, 255, 255, (int)(alpha * 255)), "READY");
        }

        // Particles
        for (int i = 0; i < particleCount; i++) {
            Particle& p = particles[i];
            float alpha = p.life / p.maxLife;
            ImU32 col = (p.color & 0x00FFFFFF) | ((int)(alpha * 255) << 24);
            dl->AddRectFilled(ImVec2(p.x - p.size/2, p.y - p.size/2), ImVec2(p.x + p.size/2, p.y + p.size/2), col);
        }

        // CRT scanlines
        for (float y = 0; y < io.DisplaySize.y; y += 3) {
            dl->AddLine(ImVec2(0, y), ImVec2(io.DisplaySize.x, y), IM_COL32(0, 0, 0, 25));
        }

        // Difficulty indicator
        const char* diffNames[] = { "FACIL", "NORMAL", "DIFICIL" };
        dl->AddText(ImVec2(ARENA_X, ARENA_Y + ARENA_H + 10), IM_COL32(255, 255, 255, 60), diffNames[difficulty]);

        // Controls hint
        dl->AddText(ImVec2(ARENA_X + ARENA_W - 170, ARENA_Y + ARENA_H + 10), IM_COL32(255, 255, 255, 60), "W/S ou Setas para mover");
    }

    void Shutdown() override {}
};

// Cena vazia (o jogo é todo via ImGui)
class SandboxScene : public Scene {
public:
    void OnEnter() override { Log::Info("SandboxScene: Scene entered."); }
    void OnUpdate(float dt) override { (void)dt; }
    void OnFixedUpdate(float dt) override { (void)dt; }
    void OnRender() override {}
    void OnExit() override {}
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    WindowConfig config = { "Ricochete Pong +18 - TitanEngine", 1280, 720 };
    Engine engine;
    engine.Initialize(config);
    engine.GetSceneStack().Push(std::make_shared<SandboxScene>());
    engine.AddModule(std::make_shared<RicocheteModule>());
    engine.Run();
    engine.Shutdown();
    return 0;
}
