# 🔱 STARLIGHT ENGINE: O COMPÊNDIO ÔMEGA (2026)

Bem-vindo ao **Motor Unificado Definitivo**. Este manual serve como o guia definitivo para o motor de alto desempenho, nível industrial e elite visual nascido da fusão da *Starlight* e *Quimera*.

---

## 🏛️ ARQUITETURA CENTRAL
- **Engine Core**: Arquitetura modular com suporte a `EngineModule`.
- **ECS (Entity Component System)**: Integração de alto desempenho com `EnTT`.
- **Job System**: Execução de tarefas multi-thread via `wiJobSystem`.
- **Octree**: Particionamento espacial para frustum culling e otimização de física.

---

## 🎨 PIPELINE DE RENDERIZAÇÃO AVANÇADO
O renderizador da Starlight é um pipeline PBR de última geração com:
- **PBR (Physically Based Rendering)**: Fluxo de trabalho Metallic/Roughness.
- **HDR Bloom**: Extração de brilho e blur gaussiano de 10 passagens.
- **SSR (Screen Space Reflections)**: Reflexos em tempo real via raymarching.
- **ACES Filmic Tone Mapping**: Graduação de cores com qualidade de cinema.
- **Névoa Volumétrica**: Névoa baseada em profundidade e distância.
- **Renderização Instanciada**: Loteamento acelerado por hardware para mais de 100.000 objetos.

---

## 🛠️ STUDIO & FLUXO DE TRABALHO
- **Starlight Studio (F2)**: Editor interativo com Hierarquia, Inspetor e Navegador de Assets.
- **Picking 3D**: Seleção por ray-cast diretamente na viewport.
- **Hot Reload**: Atualização automática de recursos via `FileWatcher`.
- **Console de Desenvolvedor**: Terminal de comando in-game com suporte a `CVar`.

---

## 🤖 SISTEMAS ESPECIALIZADOS
- **Level of Detail (LOD)**: Otimização automatizada de malha baseada na distância.
- **Hardware Skinning**: Suporte a animação esquelética para até 64 articulações.
- **Barramento de Eventos Global**: Mensageria desacoplada para comunicação modular.
- **Física (Jolt)**: Dinâmica de corpos rígidos de nível industrial.
- **Navegação**: Implementação de A* dinâmico em grade baseada em voxels.

---

## 📂 ESTRUTURA DE ASSETS
Todos os recursos do motor estão localizados em `assets/`:
- `/audio`: Temas e efeitos.
- `/fonts`: Ativos tipográficos.
- `/images`: Texturas e sprites.
- `/models`: Malhas 3D (OBJ/GLB).
- `/shaders`: Arquivos fonte OpenGL.
- `/scripts`: Scripts de lógica/Lua.

---

> [!IMPORTANT]
> **PROCEDA COM CAUTELA**: O poder da Fusão Ômega é vasto. Use o Console de Desenvolvedor (`help`) para explorar CVars e ajustar a performance em tempo real.

*Starlight Engine v2.0.0 - 2026 Fusion Edition*
