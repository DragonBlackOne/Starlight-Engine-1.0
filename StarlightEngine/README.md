<!-- Este projeto é feito por IA e só o prompt é feito por um humano. -->
# 🏗️ TitanEngine Core

O **TitanEngine** é o motor de simulação principal da suíte **Fusion ENGINE**. Focado em renderização PBR industrial e física de alta fidelidade.

## 📁 Estrutura de Pastas

- `src/`: Implementação dos sistemas (Renderer, Physics, ECS, etc).
- `include/`: Cabeçalhos da API da engine.
- `assets/`: Recursos (Shaders, Texturas, Modelos).
- `thirdparty/`: Dependências integradas (ImGui, Sol2, EnTT, etc).

## 🧬 Tecnologias Integradas

| Biblioteca | Versão | Função |
| :--- | :--- | :--- |
| **Jolt Physics** | Master | Simulação de corpos rígidos e dinâmica |
| **EnTT** | 3.13.2 | Sistema de Componentes de Entidade (ECS) |
| **SDL2** | 2.30.0 | Gerenciamento de Janelas e Input |
| **Lua / Sol2** | 5.4.6 | Scripting e lógica de alto nível |
| **meshoptimizer** | 0.21 | Otimização de geometria para GPU |
| **cgltf** | 1.14 | Carregamento de modelos PBR (glTF) |

## 🕹️ Funcionalidades de Play Mode

Pressione **F2** durante a execução para alternar entre o **Edit Mode** e o **Game Mode**.
- No **Edit Mode**: Câmera livre, inspeção de entidades e ajuste de materiais em tempo real.
- No **Game Mode**: Lógica de scripts ativa, física simulada e câmera cinematográfica orbital.

---
*Este submódulo faz parte do ecossistema Fusion ENGINE.*
