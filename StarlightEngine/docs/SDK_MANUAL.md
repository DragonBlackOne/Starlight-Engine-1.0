# Starlight Engine SDK: Commercial Development Manual 📑

// This project is AI-driven with human creative vision.

Welcome to the official **Starlight Engine SDK** manual. This document teaches you how to use the engine to create, protect, and distribute professional games.

---

## 🏁 1. Starting a New Project
Starlight Engine provides an automated script to create the basic structure of a commercial game.

```powershell
powershell -ExecutionPolicy Bypass -File ./create_project.ps1 -ProjectName "MyHeroicGame"
```

This will create a folder next to the SDK with:
- `src/main.cpp`: Your game's entry point.
- `assets/`: Where you place textures, models, and sounds.
- `CMakeLists.txt`: Pre-optimized build configuration.

---

## 🏛️ 2. Scene Architecture (ECS)
The engine uses a model based on **Scenes** and **ECS (EnTT)**.

### Boilerplate Example:
```cpp
class MyGame : public starlight::Scene {
    void OnEnter() override {
        // Initialize entities here
        auto player = m_registry.create();
        m_registry.emplace<starlight::Transform>(player);
    }

    void OnUpdate(float dt) override {
        // Frame logic
    }
};
```

---

## ⚡ 3. Optimization with SIMD Math
For heavy calculations (e.g., transforming 10,000 particles), use the `starlight::simd` namespace.

```cpp
starlight::simd::TransformPoints(modelMatrix, inData, outData, count);
```
*Note: Requires AVX2 compatible CPUs for maximum performance.*

---

## 🔐 4. Asset Protection (PAK System)
For commercial distribution, you should convert your `assets/` folder into a `.pak` file.

### How to load a PAK:
```cpp
starlight::VFSSystem::Get().LoadPak("data.pak");
// Now all assets can be accessed via virtual paths
auto texture = AssetLoader::LoadTexture("@assets/player.png");
```

---

## 🛠️ 5. Compilation and Distribution
The SDK supports **Debug** builds (with editor and logs) and **Release** builds (optimized for the end player).

1. In terminal: `cmake -B build -D CMAKE_BUILD_TYPE=Release`
2. Compile: `cmake --build build --config Release`
3. Distribute the executable along with the generated `.pak` file.

---
**Support**: Consult the source code in `StarlightEngine/src/core` for deep implementation details.
