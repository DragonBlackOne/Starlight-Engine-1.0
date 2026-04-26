// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include "CoreMinimal.hpp"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "TitanAudio.hpp"

namespace starlight {

    struct ArcadeParticle {
        glm::vec2 pos, vel;
        float life, maxLife;
        float size;
        glm::vec4 color;
    };

    class ArcadeModule : public EngineModule {
    protected:
        float shakeX = 0, shakeY = 0, shakeTimer = 0;
        std::vector<ArcadeParticle> particles;
        float totalTime = 0;

        void UpdateParticles(float dt) {
            for (size_t i = 0; i < particles.size(); i++) {
                particles[i].life -= dt;
                if (particles[i].life <= 0) {
                    particles.erase(particles.begin() + i);
                    i--;
                    continue;
                }
                particles[i].pos += particles[i].vel * dt;
                particles[i].color.a = particles[i].life / particles[i].maxLife;
            }
        }

        void Emit(glm::vec2 pos, glm::vec4 col, int count) {
            for (int i = 0; i < count; i++) {
                ArcadeParticle p;
                p.pos = pos;
                float angle = (float)(rand() % 360) * 0.01745f;
                float speed = 80.0f + (rand() % 120);
                p.vel = glm::vec2(cos(angle) * speed, sin(angle) * speed);
                p.life = 0.4f + (float)(rand() % 40) / 100.0f;
                p.maxLife = p.life;
                p.size = 2.0f + (rand() % 4);
                p.color = col;
                particles.push_back(p);
            }
        }

        void UpdateShake(float dt) {
            if (shakeTimer > 0) {
                shakeTimer -= dt;
                shakeX = ((float)(rand() % 100) - 50.0f) / 50.0f * shakeTimer * 10.0f;
                shakeY = ((float)(rand() % 100) - 50.0f) / 50.0f * shakeTimer * 10.0f;
            } else {
                shakeX = shakeY = 0;
            }
        }

        void PlayBeep(float freq, float dur);
        void PlayExplosion();

    public:
        virtual ~ArcadeModule() = default;
    };

    // --- Invaders ---
    class InvadersModule : public ArcadeModule {
    public:
        void Initialize() override;
        void Update(float dt) override;
        void RenderUI() override;
        void Shutdown() override {}
        std::string GetName() const override { return "Invaders"; }

    private:
        struct Entity { glm::vec2 pos, size; bool active; };
        Entity player;
        std::vector<Entity> pBullets, eBullets, enemies;
        int state = 0; // 0: Menu, 1: Play, 2: GameOver
        int score = 0, wave = 1;
        float fireTimer = 0;
        float enemyDir = 1.0f, enemySpeed = 100.0f;
        void SpawnWave();
    };

    // --- Breakout ---
    class BreakoutModule : public ArcadeModule {
    public:
        void Initialize() override;
        void Update(float dt) override;
        void RenderUI() override;
        void Shutdown() override {}
        std::string GetName() const override { return "Breakout"; }

    private:
        struct Entity { glm::vec2 pos, size; bool active; };
        Entity player, ball;
        std::vector<Entity> blocks;
        int state = 0;
        int score = 0, lives = 3;
        void ResetBall();
        void SpawnBlocks();
    };

    // --- Runner ---
    class RunnerModule : public ArcadeModule {
    public:
        void Initialize() override;
        void Update(float dt) override;
        void RenderUI() override;
        void Shutdown() override {}
        std::string GetName() const override { return "Runner"; }

    private:
        struct Obstacle { glm::vec2 pos, size; bool active; };
        float playerY = 360, playerVY = 0;
        std::vector<Obstacle> obstacles;
        float obsTimer = 0;
        float speed = 400.0f;
        float distance = 0;
        int state = 0;
        int score = 0;
    };

    // --- CyberSnake ---
    class SnakeModule : public ArcadeModule {
    public:
        void Initialize() override;
        void Update(float dt) override;
        void RenderUI() override;
        void Shutdown() override {}
        std::string GetName() const override { return "CyberSnake"; }

    private:
        struct Node { int x, y; };
        std::vector<Node> snake;
        Node food;
        glm::vec2 dir = { 1, 0 };
        float moveTimer = 0;
        int state = 0;
        int score = 0;
        void SpawnFood();
    };

    // --- Ricochete ---
    class RicocheteModule : public ArcadeModule {
    public:
        void Initialize() override;
        void Update(float dt) override;
        void RenderUI() override;
        void Shutdown() override {}
        std::string GetName() const override { return "Ricochete"; }

    private:
        float plY = 200, prY = 200, plH = 80, plHTarget = 80;
        glm::vec2 ballPos, ballVel;
        int scoreL = 0, scoreR = 0;
        int state = 0, menuSel = 0, difficulty = 1;
        float shakeX = 0, shakeY = 0, shakeTimer = 0, chromatic = 0;
        void ResetBall(int dir);
    };

    // --- Cinema 3D ---
    class Cinema3DModule : public EngineModule {
    public:
        void Initialize() override;
        void Update(float dt) override;
        void RenderUI() override;
        void Shutdown() override {}
        std::string GetName() const override { return "3D Cinema"; }
    private:
        float totalTime = 0;
        float rotation = 0;
        bool showWireframe = false;
    };

}
