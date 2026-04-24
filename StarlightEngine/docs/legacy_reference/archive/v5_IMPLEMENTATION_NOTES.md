# Starlight Engine v5.0 - Implementation Notes

## Feature: glTF 2.0 Loader

**Status:** Alpha Implementation
**Dependencies:** `pygltflib`

### Overview
The v5.0 "Kinetic Update" introduces a native loader for the glTF 2.0 format, replacing the legacy OBJ loader. This allows for:
- Hierarchical scenes (Scene Graph).
- Binary data storage (GLB) for faster loading.
- PBR materials (future work).
- Skeletal Animation (future work).

### Architecture

#### `GLTFLoader` (`src/starlight/engine/gltf_loader.py`)
Responsible for parsing the JSON structure and extracting binary data from buffers.
- **Geometry Extraction:** Interleaves `POSITION`, `NORMAL`, and `TEXCOORD_0` into a single `float32` buffer with stride 8 (`3+3+2`).
- **Index Extraction:** Supports `uint16` and `uint32` indices.
- **Node Hierarchy:** Reconstructs the transform tree (Translation, Rotation, Scale) into `GLTFNode` objects.

### Scene Graph (Hierarquia)
Implementado em `src/starlight/engine/node.py`.
- **Classe `Node`:** Suporta hierarquia Pai-Filho.
- **Transformações:** Calcula automaticamente `world_matrix` multiplicando `parent.world_matrix * local_matrix`.
- **Anexos:** Suporta `mesh` e `material` opcionais.

#### `Renderer3D` Integration
The `Mesh` class in `renderer_3d.py` was upgraded to support Element Buffer Objects (IBO/EBO) for indexed drawing.
- **`draw_mesh`**: Renderiza uma malha individual.
- **`render_scene`**: Percorre recursivamente um grafo de `Node`, desenhando objetos visíveis com suas transformações globais corretas.

### Material System
The `Material` class (`src/starlight/engine/material.py`) encapsulates PBR parameters.
- **Loader:** Extracts `baseColorFactor`, `metallicFactor`, `roughnessFactor`.
- **Textures:** Automatically resolves and loads `baseColorTexture` via `AssetLoader`.

### Usage Example

```python
from starlight.engine.gltf_loader import gltf_loader

# Load model data
model_data = gltf_loader.load("assets/models/character.glb")

if model_data:
    # Iterate over meshes (simplified)
    for mesh_group in model_data.meshes:
        for primitive in mesh_group:
            # Create GPU resources
            mesh = renderer.create_mesh(primitive['vertices'], primitive['indices'])
            mesh.create_vao(renderer.program)
            material = primitive['material']
            
            # Render
            renderer.draw_mesh(mesh, material, glm.mat4(1.0))
```

### Next Steps
1.  **Scene Graph:** Implement recursive rendering of the Node hierarchy.
2.  **Skinning:** Parse `JOINTS` and `WEIGHTS` attributes for skeletal animation.
