---
description: Auditar e validar o pipeline de renderização WGPU
---

# Render Pipeline Check Skill

Esta skill ajuda a verificar a sanidade do pipeline gráfico.

## Checklist

### 1. Shaders
- [ ] Verificar se shaders estão compilando sem logs de erro.
- [ ] Verificar alinhamento de Uniforms (Rust struct vs WGSL `struct` alignment). Lembre-se que WGPU requer padding de 16 bytes.
    - Ex: `vec3` no Rust precisa de padding para alinhar como `vec4` no buffer se seguido de outro campo.

### 2. Resources
- [ ] BindGroups: Verificar se texturas e samplers estão sendo bindados corretamente.
- [ ] Depth Buffer: Verificar formato (`Depth32Float`).

### 3. PBR
- [ ] Metallic/Roughness channel mapping.
- [ ] sRGB awareness (Texturas de cor devem ser sRGB, Normais/Metallic Linear).

## Script de Validação (Pseudo-código)

Você pode criar um script Python para checar capacidades do backend:
```c
#include "starlight.h".backend as backend
# Info não exposta ainda, mas seria útil:
# print(backend.get_adapter_info())
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
