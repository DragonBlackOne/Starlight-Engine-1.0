# Starlight Engine: Architecture Blueprint v1.0

Este documento serve como a especificação técnica mestre para a reconstrução do **Starlight Engine**.

## 1. Visão Geral (Core Pillars)
O Starlight é um motor de jogo híbrido focado em performance (Rust) e flexibilidade (Python), projetado com suporte nativo a fluxos de trabalho de agentes de IA.

- **Core**: Rust (Segurança e Concorrência).
- **Backend de Renderização**: WGPU (Moderno, Multi-Backend).
- **ECS**: Bevy ECS (Sparse Sets para alta performance).
- **Física**: Rapier 3D (Determinística, Kinematic + Dynamic).
- **Scripting**: Python 3.10+ via PyO3 (Zero-copy binding).
- **Áudio**: Rodio / Spatial Audio 3D.

## 2. A Camada de Integração (PyO3)
O backend Rust (`engine_core`) exporta um módulo `backend` para o Python.
- **Mapeamento de Tipos**: `glam::Vec3` <-> `tuple(f32, f32, f32)`.
- **Sincronização**: Loop principal gerenciado pelo Rust com retroalimentação em Python via `on_update` e `on_fixed_update`.

## 3. ECS: Sistema de Entidades e Componentes
A reconstrução deve priorizar a separação clara entre componentes de dados e sistemas de lógica.
- **Componentes Rust**: `Transform`, `RigidBody`, `Material`.
- **Proxy Python**: Classe `Entity` que encapsula o `entity_id` e manipula componentes via chamadas de backend.

## 4. Pipeline de Renderização (Estabilidade WGPU)
Lições aprendidas na v1.0:
- **Blit Pass Obrigatório**: Para conversão de cores entre o formato interno (Rgba8UnormSrgb) e o da superfície (Bgra8Unorm).
- **Resolve Target Lifecycle**: Views de resolve targets devem ser persistentes durante o ciclo de vida do encoder para evitar erros de validação em drivers intermitentes.
- **Projection Matrices**: Correção de matriz OpenGL -> WGPU (ajuste de profundidade de [-1, 1] para [0, 1]).

## 5. Agent-Ready Design
A engine deve ser totalmente controlável via arquivos de texto (Markdown Blueprints) e oferecer APIs de reflexão para que agentes (como Antigravity) possam auditar o estado da cena em tempo real.
