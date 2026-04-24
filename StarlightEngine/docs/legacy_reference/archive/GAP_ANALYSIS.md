# Análise de Lacunas (Gap Analysis): Starlight Engine vs Arquitetura Convergente

Este documento detalha as discrepâncias entre a implementação atual da `starlight` engine e a "Arquitetura Convergente" definida em `docs/ARQUITETURA.md`. O objetivo é guiar o desenvolvimento futuro para alinhar o motor com os padrões da indústria.

## 1. O Ciclo Vital (Game Loop)

### Estado Atual (AS-IS)
A implementação atual reside em `crates/engine_core/src/lib.rs` (struct `StarlightRunner`).
O ciclo de atualização implementa o padrão de **Passo de Tempo Fixo (Fixed Timestep)** com acumulador.
*   **Comportamento:** A física (`physics.step()`) e sistemas críticos rodam em um loop fixo (50Hz), desacoplados da renderização.
*   **Status:** **CONCLUÍDO**. A implementação segue as diretrizes da arquitetura convergente, incluindo interpolação de estado (alpha blending) para renderização suave.

### Estado Desejado (TO-BE)
Manter a implementação atual e garantir que novos sistemas (como AI ou Scripts) respeitem a distinção entre `Update` (Variável) e `FixedUpdate` (Fixo).

## 2. Gestão de Memória

### Estado Atual (AS-IS)
*   O motor utiliza majoritariamente o alocador padrão do Rust (jemalloc/system malloc).
*   `Bevy ECS` gerencia a memória das entidades e componentes de forma eficiente (Archetypal), o que se alinha parcialmente com o princípio de "Data-Oriented Design".
*   Não há alocadores de pilha (Stack) ou piscina (Pool) expostos explicitamente para lógica de jogo temporária.

### Estado Desejado (TO-BE)
*   Embora o Rust ofereça segurança de memória, a introdução de **Pool Allocators** para entidades de vida curta (projéteis, partículas) pode reduzir a pressão no gerenciador de memória do sistema.
*   **Prioridade:** MÉDIA. O ECS atual já mitiga os piores problemas de fragmentação.

## 3. Camada de Plataforma e Abstração (HAL)

### Estado Atual (AS-IS)
*   Utiliza `winit` para gerenciamento de janelas e eventos, que é uma excelente abstração multiplataforma (Windows, Linux, macOS, Web).
*   `wgpu` atua como RHI (Render Hardware Interface), abstraindo DX12, Vulkan, Metal.
*   **VFS:** Implementado em `crates/engine_core/src/vfs.rs` com suporte a montagem de diretórios físicos.

### Estado Desejado (TO-BE)
*   **Expansão do VFS:** Adicionar suporte a arquivos compactados (`.pak`, `.zip`) para distribuição. Atualmente suporta apenas diretórios físicos.
*   **Prioridade:** MÉDIA.

## 4. Gestão de Recursos (Assets)

### Estado Atual (AS-IS)
*   Identificação por UUID Strings: O sistema foi migrado para usar UUIDs no `AssetRegistry`.
*   Carregamento Assíncrono: Implementado `AsyncAssetLoader` com suporte a OBJ, Texturas e Skybox.
*   **GLTF:** Suporte pendente.

### Estado Desejado (TO-BE)
*   **Suporte a GLTF 2.0:** Implementar carregamento de cenas e malhas GLTF/GLB.
*   **Pipeline de Importação:** Converter PNGs para formatos de GPU (BC7/ASTC) offline (futuro).
*   **Prioridade:** ALTA.

## 5. Sistema de Input

### Estado Atual (AS-IS)
*   **Action Mapping:** Implementado (`bind_action`, `is_action_down`).
*   **Axis Mapping:** Implementado (`bind_axis`, `get_axis`).
*   Suporte a Gamepads via `gilrs`.

### Estado Desejado (TO-BE)
*   Expandir suporte a remapeamento em runtime (UI de configuração).
*   **Prioridade:** BAIXA. O core está funcional.

## 6. Física

### Estado Atual (AS-IS)
*   Usa `rapier3d`.
*   Integração alinhada com **Fixed Timestep**.
*   Callbacks de colisão expostos para Python.

### Estado Desejado (TO-BE)
*   Refinar a API de colisão para expor mais dados (pontos de contato, normais).

## 7. Navegação e AI

### Estado Atual (AS-IS)
*   Inexistente.

### Estado Desejado (TO-BE)
*   Integração com **Recast/Detour** ou similar para geração de NavMesh.
*   **Prioridade:** BAIXA (Futuro). Focar primeiro no Core Loop e Rendering.

---

## Plano de Ação Recomendado (Roadmap)

1.  **GLTF Loader:** Completar o suporte a GLTF 2.0 no backend.
2.  **Scene Graph:** Melhorar a hierarquia de entidades (Parent/Child) para suportar transformações relativas complexas.
3.  **VFS Archives:** Adicionar suporte a .zip/.pak.
