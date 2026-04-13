# ⭐ Stardust Engine v3.0 - Metropolis

> **High-Fidelity Procedural City & Terrain Engine**
> 
> *A ModernGL + Python powered 3D engine featuring Numba-accelerated generation, PBR textures, and atmospheric effects.*

![Stardust Engine](https://raw.githubusercontent.com/antigravity-engine/assets/main/banner_v3.png)

## 🚀 Getting Started

### Prerequisites
- Python 3.8+
- ModernGL
- NumPy
- Numba
- Pillow (PIL)
- GLFW

### Installation
```bash
pip install -r requirements.txt
python setup.py build_ext --inplace  # Compile High-Performance Modules
python tools/asset_manager.py      # Download resources
```

### Run
```bash
python main.py
```

## ✨ Key Features

### 🏙️ Procedural City
- **Infinite Generation**: Uses Numba (`engine/procedural.py`) to generate heightmaps and city layouts in milliseconds.
- **Instanced Rendering**: Renders thousands of buildings with a single draw call.
- **Dynamic Architecture**: varies building height, width, and type (Concrete/Glass) based on city density.

### 🎨 High-Fidelity Visuals
- **PBR Texturing**: Supports Diffuse and Specular maps for realistic materials.
- **Tri-planar Terrain**: Blends Grass and Rock textures based on slope without UV distortion.
- **Atmosphere**: 
    - High-Res Starry Skybox.
    - Volumetric-style Fog.
    - Day/Night cycle lighting.
    - Procedural Window lights (Night mode).

### 🌧️ Weather System
- **Volumetric Rain**: Wraps around the camera for infinite effect.
- **Particles**: Fireflies and atmospheric wisps.

## 🎮 Controls
| Key | Action |
| --- | --- |
| `W, A, S, D` | Move Camera |
| `Mouse` | Look Around |
| `F` | Toggle **Fly Mode** |
| `SPACE` | Fly Up |
| `CTRL` | Fly Down |
| `ESC` | Exit |

## 📁 Architecture
- `main.py`: Entry point and Event Loop.
- `game/world.py`: World Manager (Terrain, City, Weather instancing).
- `engine/shaders.py`: GLSL Shader definitions (PBR, Tri-planar, Skybox).
- `engine/procedural.py`: CPU-side generation logic (Numba optimized).
- `tools/asset_manager.py`: Asset downloader/generator.

---
*Created by Antigravity Agent*
