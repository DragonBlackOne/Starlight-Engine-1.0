# Arcade Tetris — Neon Edition

> **Este projeto é feito por IA e só o prompt é feito por um humano.**

O **Tetris Project** demonstra a maturidade do Starlight Engine em lidar com matrizes multidimensionais, inputs precisos e overlays de interface neon pixel-perfect.

## 🧩 Recursos de Gameplay

### Mecânicas Oficiais
| Feature | Descrição |
|---------|-----------|
| **Super Rotation System (SRS)** | Wall kicks com vetores de deslocamento automático |
| **Ghost Piece** | Projeção da posição final da peça ativa |
| **Delayed Auto Shift (DAS)** | Delay de 150ms + repeat de 30ms para movimento lateral preciso |
| **Lock Delay** | 0.5s timer antes de fixar a peça no chão |
| **Hold Piece** | Pressione C/Shift para reservar peças |
| **Next Queue** | Fila de 3 próximas peças visível |
| **Hard Drop** | Espaço para queda instantânea |

### Sistemas Novos (SBA v2.0)
| Feature | Descrição |
|---------|-----------|
| **Combo System** | Limpar linhas consecutivamente gera bônus de 50 × combo × level |
| **Back-to-Back** | Triple/Tetris consecutivos dão 1.5× pontos com tag "B2B" |
| **Line Clear Flash** | Flash branco + mensagem com nome do clear (SINGLE/DOUBLE/TRIPLE/TETRIS!) |
| **Color.hsv Messages** | Mensagens de clear em cores rainbow via framework Color |

## 🎮 Controles

| Tecla | Ação |
|-------|------|
| ← → ou A/D | Mover peça |
| ↑ ou W | Rotacionar |
| ↓ ou S | Soft drop |
| Espaço | Hard drop |
| C / Shift | Hold piece |
| ESC | Pausar |

## 🚀 Como Jogar

```powershell
cd Tetris_Project
cmake -B build -S .
cmake --build build --config Release
.\build\Release\Tetris_Project.exe
```

## 🔧 Framework
Utiliza `core.lua` v2.1 com Class, MathX, Color (HSV para mensagens rainbow).
