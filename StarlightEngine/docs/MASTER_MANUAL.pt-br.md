# 🔱 Starlight ENGINE: O COMPÊNDIO OMEGA (2026)

Bem-vindo à **Engine Unificada Ultimate**. Este manual é o guia definitivo para a engine de grau industrial e o framework de desenvolvimento **SBA v2.0**.

---

## 🏛️ ARQUITETURA CORE
- **Engine Core**: Arquitetura modular com `EngineModule`.
- **ECS**: Integração `EnTT` de alta performance.
- **Job System**: Execução multi-thread via `wiJobSystem` (fiber-based).
- **Octree**: Particionamento espacial para culling.

---

## 🎨 PIPELINE DE RENDERIZAÇÃO AVANÇADA

| Feature | Detalhes |
|---------|----------|
| **PBR** | Workflow Metallic/Roughness com IBL |
| **CSM** | 4 cascatas de sombras 2048x2048 |
| **SSAO** | Oclusão ambiental temporal |
| **HDR Bloom** | Multi-pass com blur Gaussiano |
| **SSR** | Reflexões em tempo real via raymarching |
| **ACES** | Tone mapping cinema-quality |
| **Renderer2D** | Batch rendering para UI e jogos 2D |
| **GPU Culling** | Culling em compute shaders |

---

## 🎮 SBA v2.0 — STARLIGHT BRIDGE API

### SDK de Jogos (`sba_bridge.lua`)
| Sistema | API |
|---------|-----|
| **Entity** | `Entity("Tag", x, y, z)` → `:setColor()`, `:setScale()`, `:move()`, `:destroy()` |
| **Light** | `Light(x,y,z, r,g,b, intensidade)` → `:setColor()`, `:setIntensity()` |
| **Tween** | `Tween.to(entidade, {y=5}, 1.5, "easeOutElastic")` — 8 easings |
| **Scene** | `Scene.register("Jogo", {onEnter, onUpdate, onRenderUI, onExit})` |
| **Events** | `Events.on("hit", fn)` / `Events.emit("hit", dados)` |
| **Coroutine** | `Coroutine.start(fn)` / `Coroutine.wait(segundos)` |

### Biblioteca Padrão (`core.lua`)
| Sistema | API |
|---------|-----|
| **MathX** | `clamp`, `lerp`, `sign`, `distance`, `distance3D`, `smoothstep`, `remap`, `random_range`, `normalize2D`, `angle`, `wrap` |
| **Physics2D** | `CheckAABB`, `CheckCircle`, `PointInRect`, `RayCircle` |
| **Timer** | `Timer.after()`, `Timer.every()`, `Timer.cancel()` |
| **Color** | `Color.hsv()`, `Color.pulse()`, `Color.lerpRGB()` |
| **ScreenShake** | `trigger()`, `update()`, `getOffset()` |
| **ValueTween** | `ValueTween.to()`, `ValueTween.update()` |

---

## 📂 ESTRUTURA DE ASSETS
```
assets/
├── audio/       # Sons e músicas
├── textures/    # Imagens e sprites
├── models/      # Meshes 3D (OBJ/GLB)
├── shaders/     # Shaders GLSL OpenGL
└── scripts/     # Scripts Lua
    ├── core.lua           # Biblioteca Padrão
    ├── sba_bridge.lua     # SDK de Jogos
    └── jogo_main.lua      # Lógica do seu jogo
```

---

*Starlight Engine v2.1.0 — SBA v2.0 — 2026 Odyssey Edition*
