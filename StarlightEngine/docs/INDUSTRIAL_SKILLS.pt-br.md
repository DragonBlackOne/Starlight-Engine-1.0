# 🧠 STARLIGHT ENGINE: COMPÊNDIO DE HABILIDADES INDUSTRIAIS

Este documento consolida o conhecimento técnico avançado necessário para dominar os sistemas Ômega da Starlight Engine.

---

## 🛰️ 1. MENSAGERIA GLOBAL (SISTEMA DE EVENTOS)
**Conceito**: Comunicação desacoplada via Barramento de Eventos (Event Bus).
- **Emit Imediato**: `EventSystem::Get().Emit(ev)` — Despacha imediatamente.
- **Emit Diferido**: `EventSystem::Get().EmitDeferred(ev)` — Enfileira para o próximo frame.
- **Inscrição**: Registre callbacks para `EventType` ou strings personalizadas.
- **Zero-Allocation**: Usa uma arena de memória interna para armazenamento de carga útil durante eventos diferidos.

---

## 🌲 2. OTIMIZAÇÃO ESPACIAL (OCTREE)
**Conceito**: Divisão recursiva do espaço 3D.
- **Frustum Culling**: Renderize apenas malhas dentro do frustum de visão da câmera.
- **Ray-Picking**: Acelere a seleção de entidades testando nós em vez de cada objeto individual.
- **Colisão**: Verificação de fase estreita apenas para entidades em nós compartilhados.

---

## 🎭 3. ANIMAÇÃO PROFISSIONAL (OZZ)
**Conceito**: Sistema de animação esquelética de nível industrial.
- **Ozz-Animation**: Integração com o framework profissional Ozz para amostragem e blending de poses.
- **Hardware Skinning**: Vértices deformados por uma hierarquia de articulações (joints) na GPU.
- **Joints**: Cada vértice suporta 4 índices de articulação (`ivec4`).
- **Weights**: Influência normalizada de cada articulação (`vec4`).
- **Buffer de Vértices**: Estrutura `starlight::Vertex` atualizada para incluir dados de animação por padrão.

---

## 🌐 4. REDE & NAVEGAÇÃO
**Conceito**: Estado distribuído e busca de caminhos.
- **Networking**: Sincronização de estado Cliente-Servidor (via NetworkSystem).
- **Navegação**: Implementação de A* dinâmico em uma grade baseada em voxels.
- **IA**: Árvores de Comportamento (Behavior Trees) integradas e Máquinas de Estado Finito (FSM).

---

## 🏗️ 5. RENDERGRAPH & MODULARIDADE
**Conceito**: Renderização baseada em passagens com gerenciamento automático de recursos.
- **Desacoplamento**: Cada passagem (CSM, SSAO, Bloom) é um nó independente.
- **Eficiência**: Reutilização automática de texturas transientes e buffers.
- **Escalabilidade**: Facilidade para adicionar novos efeitos de pós-processamento ou iluminação.

---

> [!TIP]
> **BOA PRÁTICA**: Use o **Sistema de Eventos** para disparar Conquistas ou atualizações de Missão, mantendo sua lógica de jogo separada dos loops de atualização principais do motor.

*Starlight Engine v2.1.0 - 2026 Odyssey Edition*
