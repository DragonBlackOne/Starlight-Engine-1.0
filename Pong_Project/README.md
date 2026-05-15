# Starlight Pong — Neon Edition

> **Este projeto é feito por IA e só o prompt é feito por um humano.**

O **Pong Project** é nosso showcase de física 2D, AI e sistema de partículas rodando sob estética Neon Synthwave.

## 🏓 Recursos de Gameplay

| Feature | Descrição |
|---------|-----------|
| **AI Opponent** | CPU com tracking suavizado (85% da velocidade do jogador) |
| **Particle System** | Explosões de partículas em colisões + trails da bola |
| **Screen Shake** | Shake escalável: quanto maior o rally, mais intenso |
| **Energy Bars** | Score visual com barras de energia neon |
| **Power-Ups** | Cubos pulsantes que spawnam no campo |
| **Rally Counter** | Contador central com cor HSV rainbow + barra de velocidade |
| **Serve System** | Auto-serve com delay de 1s após pontuação |

### Power-Up System
| Tipo | Cor | Efeito |
|------|-----|--------|
| SlowBall | 🔵 Azul | Reduz velocidade da bola em 40% |
| SpeedBurst | 🩷 Rosa | Aumenta velocidade da bola em 40% |
| Visual | 🟢 Verde | Feedback visual com partículas |

## 🎮 Controles

| Tecla | Ação |
|-------|------|
| W / S | Mover paddle (Player 1) |
| ESC | Pausar |

## 🚀 Como Jogar

```powershell
cd Pong_Project
cmake -B build -S .
cmake --build build --config Release
.\build\Release\Pong_Project.exe
```

## 🔧 Framework
Utiliza `core.lua` v2.1 com Class, MathX, Physics2D.CheckAABB, Color.hsv para rally counter.
