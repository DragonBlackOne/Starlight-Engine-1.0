# Starlight Engine: Site Map & Inventory

## 1. Módulos de Backend (Rust)
- `engine_core`: Gerenciamento de janelas (Winit), ECS (Bevy), Física (Rapier), VFS.
- `engine_render`: Pipeline WGPU, Shaders, Buffers de Materiais, Blit Pass.
- `engine_audio`: Spatial Audio (Rodio), Panning 3D.

## 2. API de Backend (Python Exposed)
Principais funções disponíveis em `starlight.backend`:
| Função | Descrição |
|---|---|
| `run_engine()` | Inicializa o motor e entra no loop principal. |
| `spawn_entity()` | Cria uma entidade vazia no ECS. |
| `spawn_character()` | Cria um personagem kinematic com Capsule Collider. |
| `move_character()` | Aplica deslocamento via Rapier Controller. |
| `apply_impulse()` | Física: Mudança instantânea de velocidade. |
| `play_sound()` | Áudio: Executa som 2D/3D. |
| `capture_screenshot()`| Gera validação visual do frame. |

## 3. Catálogo de Shaders (High-End)
O motor atualmente possui 107 shaders. Os principais recursos incluem:
- **PBR (Physically Based Rendering)**: `pbr.frag`, `brdf.frag`.
- **Post-FX**: `bloom_extract.frag`, `fxaa.frag`, `godrays.frag`.
- **Compute**: `fluid_force.comp`, `cloth_update.comp`, `particle_update.comp`.
- **WGPU Optimized**: `shader.wgsl` (Código unificado).

## 4. Componentes ECS (Blueprints)
- **Essential**: `Transform`, `Rotation`, `Scale`, `Color`, `GlobalTransform`.
- **Gameplay**: `Health`, `Mana`, `Inventory` (Draft).
- **Physics**: `RigidBodyComponent`, `ColliderComponent`, `CharacterController`.
- **Visual**: `MeshName`, `Material`.

## 5. Assets Recomendados para Reuso
- `assets/models/cube`: Primitive básico para física.
- `assets/textures/default`: Textura quadriculada de debug.
- `assets/skyboxes/forest`: Texturas HDR para iluminação global.
