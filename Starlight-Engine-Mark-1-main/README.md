# Starlight Engine Mark-C 🛡️✨

**The Hybrid Powerhouse: Ultra-Modern PBR Performance meets 16-Bit Retro Magic.**

Starlight Mark-C is a professional-grade, high-performance game engine written in **Pure C11**. It is designed for developers who demand total control, zero overhead, and the ability to switch between modern 4K rendering and authentic retro aesthetics.

---

## 🚀 Two Eras, One Engine

### 💎 The Modern Era
- **High-End PBR Rendering**: Physically Based Rendering with IBL support.
- **Advanced Post-FX**: ACES Tonemapping, Bloom, SSAO, and Volumetric Fog.
- **ECS-Powered Scale**: Massive entity management using **Flecs**.
- **Physics & Navigation**: Built-in 3D physics and A* Pathfinding.

### 🕹️ The Retro Genesis
- **Mode 7 Engine**: Authentic SNES-style inverse perspective mapping.
- **Chiptune Synth**: Mathematical wave generation with full ADSR envelopes.
- **Parallax & Tilemaps**: High-performance scrolling and AABB grid physics.
- **60Hz Lock**: Deterministic fixed-timestep loop for frame-perfect gameplay.

---

## 🛠️ Performance Architecture
- **Framework (SLF)**: Lightweight, ergonomic callback system with Scene Stacks.
- **Asset Pool**: Ref-counted caching for Textures, Fonts, and Sounds.
- **Shader Hot-Reload**: Real-time iteration on `.vert` and `.frag` files.
- **Tween Library**: Built-in mathematical easing for silky-smooth animations.

---

## 🏁 Quick Setup

```powershell
mkdir build -Force; cd build
cmake ..
cmake --build . --config Release
```

---

## 📚 Master Documentation Suite

Explore our comprehensive guides to master the Starlight Engine:

| Document | Description |
|---|---|
| 🚀 **[Quick Start](docs/QUICKSTART.md)** | Your first application in 5 minutes. |
| 🏛️ **[Framework Guide](docs/framework_guide.md)** | Mastery of Scenes, Tweens, and Asset Pools. |
| 🕹️ **[Retro Genesis Guide](docs/retro_development.md)** | How to build authentic 16-bit experiences. |
| ⚙️ **[Settings Guide](docs/settings_guide.md)** | How to add and manage hub configurations. |
| 🏗️ **[Architecture](docs/ARCHITECTURE.md)** | Deep dive into the 60Hz loop and memory model. |
| 📖 **[API Reference](docs/api_reference.md)** | The complete blueprint of all `starlight_*` functions. |

---
**🏆 Starlight: The future of C-native game development.**

