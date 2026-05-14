# Arcade Tetris - Neon Edition
> **Este projeto é feito por IA e só o prompt é feito por um humano.**

O **Tetris Project** é a nossa consagração de maturidade do Starlight Engine em lidar com matrizes multidimensionais matemáticas, inputs ágeis e overlays de interface visual pixel-perfect, rodando sob a temática Retro-Arcade Neon.

## 🧩 Recursos de Qualidade (Torneio)

Para garantir que o Tetris fosse perfeitamente fluído, implementamos as regras oficiais clássicas do Tetris mundial diretamente na nossa lógica em LUA:

1. **Super Rotation System (SRS) e Wall Kicks:** As peças não travam quando rotacionadas contra as paredes; o algoritmo calcula vetores de deslocamento automático (`offsetR`, `offsetC`) para encaixe das peças.
2. **Ghost Piece:** Uma projeção algorítmica exibe a silhueta onde a sua peça irá colidir instantaneamente no assoalho, usando um lookahead de eixo Y para eliminar o chute ou falta de precisão da grade.
3. **Delayed Auto Shift (DAS) & Lock Delay:** Inputs precisos para deslize contínuo para os lados (`A/D` ou setas), aliado ao timer fixo (`0.5s`) de delay assim que a peça entra em contato físico com o chão (impedindo colamentos não intencionais instantâneos).
4. **HOLD e NEXT Queue:** Pressione "C" ou "Shift" para reservar peças para estratégias complexas. A máquina armazena o gerador de aleatoriedade futuro criando a fila visual de "próximas" peças no canto direito.

## 🖥️ UI Dinâmica e Escalonamento
O projeto introduziu uma adaptação inteligente na resolução de vídeo via Engine. A proporção nativa requer **640x800** para emular gabinetes de fliperama em monitores ultra-wide ou displays com DPI alto em Laptops (150% Scale), assegurando que a interface e as máscaras negras da UI não sofram "Screen Cutting" sobrepostas no papel de parede neural Synthwave.

## 🚀 Como Jogar

```bash
# Na raiz da engine:
cd Tetris_Project
cmake --build build --config Release
.\build\Release\Tetris_Project.exe
```
O jogo responde às setas direcionais tradicionais **e** às configurações nativas WSAD combinadas em C++, abraçando todos os estilos de jogador.
