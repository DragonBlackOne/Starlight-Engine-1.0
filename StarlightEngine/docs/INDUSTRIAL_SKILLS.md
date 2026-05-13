# 🧠 starlight ENGINE: INDUSTRIAL SKILLS COMPENDIUM

This document consolidates the advanced technical knowledge required to master the starlight Omega Fusion systems.

---

## 🛰️ 1. GLOBAL MESSAGING (EVENT SYSTEM)
**Concept**: Decoupled communication via an Event Bus.
- **Immediate Emit**: `EventSystem::Get().Emit(ev)` — Dispatches right away.
- **Deferred Emit**: `EventSystem::Get().EmitDeferred(ev)` — Queues for next frame.
- **Subscribing**: Register callbacks to `EventType` or custom strings.
- **Zero-Allocation**: Uses an internal memory arena for payload storage during deferred events.

---

## 🌲 2. SPATIAL OPTIMIZATION (OCTREE)
**Concept**: Recursive division of 3D space.
- **Frustum Culling**: Only render meshes inside the camera's view frustum.
- **Ray-Picking**: Accelerate entity selection by testing nodes instead of every object.
- **Collision**: Narrow-phase check only for entities in shared nodes.

---

## 🎭 3. PROFESSIONAL ANIMATION (OZZ)
**Concept**: Industrial-grade skeletal animation system.
- **Ozz-Animation**: Integration with the professional Ozz framework for sampling and blending.
- **Hardware Skinning**: Vertices deformed by a joint hierarchy on the GPU.
- **Joints**: Each vertex supports 4 joint indices (`ivec4`).
- **Weights**: Normalized influence of each joint (`vec4`).
- **Vertex Buffer**: Upgraded `starlight::Vertex` structure to include animation data by default.

---

## 🌐 4. NETWORK & NAVIGATION
**Concept**: Distributed state and pathfinding.
- **Networking**: Client-Server state synchronization (via NetworkSystem).
- **Navigation**: Dynamic A* implementation on a voxel-based grid.
- **AI**: Integrated Behavior Trees and Finite State Machines (FSM).

---

## 🏗️ 5. RENDERGRAPH & MODULARITY
**Concept**: Pass-based rendering with automatic resource management.
- **Decoupling**: Each rendering pass (CSM, SSAO, Bloom) is an independent node.
- **Efficiency**: Automatic reuse of transient textures and buffers.
- **Scalability**: Easily add new post-processing or lighting effects.

---

> [!TIP]
> **BEST PRACTICE**: Use the **Event System** to trigger Achievements or Quest updates, keeping your game logic separate from the Engine's core update loops.

*Starlight Engine v2.1.0 - 2026 Odyssey Edition*
