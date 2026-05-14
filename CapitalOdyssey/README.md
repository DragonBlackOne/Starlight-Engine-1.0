# Capital Odyssey Showcase
> **Este projeto é feito por IA e só o prompt é feito por um humano.**

**Capital Odyssey** é o maior e mais ambicioso módulo de demonstração do ecossistema Fusion Engine. Ele representa o teto tecnológico da engine operando em modo procedural e Isométrico, construindo uma cidade cibernética com simulação financeira em tempo real.

## 🌆 Sobre o Projeto

O projeto adota uma câmera isométrica bloqueada (`OnFixedUpdate` LookAt em 45 graus) para construir uma verdadeira utopia (ou distopia) focada na expansão do mercado. O layout visual é fortemente inspirado no gênero Synthwave/Cyberpunk de finanças, exibindo "painéis" de UI complexos que controlam o ritmo do mundo virtual.

## ✨ Destaques Tecnológicos

1. **Geração Procedural Segura de Assets**
   - Para resolver o antigo erro de dependência de arquivos em disco corrompidos (texturas quebradas resultando no clássico "quadriculado rosa"), implementamos um gerador de arte procedural C++ em tempo de compilação gráfica (`AssetLoader::CreateProceduralIsometricTile` e `CreateProceduralBuilding`).
   - A Engine agora desenha diamantes isométricos, arranha-céus neons e tiles tech diretamente gerando Pixels Array matemáticos enviados via OpenGL.

2. **Shader Isométrico com Transparência (Fragment Mask)**
   - Customizamos shaders na base da Starlight para ignorar o tradicional "Alpha" fixo, implementando máscaras poligonais lógicas no fragment shader (`batch.frag`). 
   - Elementos gerados recebem antialiasing de borda de forma autônoma sem requerer imagens grandes.

3. **Arquitetura Lua Dirigida a Dados**
   - O projeto `capital_odyssey.lua` não processa controles de personagem, mas lida com dados lógicos da bolsa de valores, taxas de juros, IA de cidadãos e atualizações rítmicas de mercado, espelhando os valores nas chamadas de `DashboardSystem` e ImGui.

## 🛠️ Como Jogar / Executar
Compile o projeto através da raiz:
```bash
cmake --build build --config Release
.\build\Release\Capital_Odyssey.exe
```
Explore o Dashboard integrado que reflete a taxa de inflação e orçamento central em tempo real.
