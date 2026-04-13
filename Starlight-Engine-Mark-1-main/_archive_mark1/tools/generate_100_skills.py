import os

SKILLS = {
    # 1. Rendering
    "render-pbr-ibl": "Image Based Lighting with HDR Cubemaps",
    "render-shadows-csm": "Cascaded Shadow Maps for large environments",
    "render-shadows-pcf": "Soft Shadows using Percentage Closer Filtering",
    "render-post-bloom": "Physically Based Bloom (Karis Average)",
    "render-post-tonemap": "ACES Filmic Tone Mapping",
    "render-ssao": "Screen Space Ambient Occlusion",
    "render-ssr": "Screen Space Reflections",
    "render-dof": "Depth of Field with Bokeh",
    "render-motion-blur": "Per-object Motion Blur",
    "render-volumetric-fog": "Ray-marched Volumetric Fog",

    # 2. Physics
    "physics-character-controller": "Kinematic Character Controller",
    "physics-vehicle-controller": "Raycast Vehicle Controller",
    "physics-soft-body": "Basic Soft Body Simulation hooks",
    "physics-fluid-particles": "SPH Fluid Simulation basics",
    "physics-fracture": "Mesh Slicing and Fracturing",
    "physics-ragdoll": "Radgoll Joint Hierarchies",
    "physics-collision-events": "Detailed Contact Point Events",
    "physics-layers-matrix": "Collision Interaction Matrix",
    "physics-continuous-cd": "Continuous Collision Detection (CCD)",
    "physics-query-system": "Advanced Ray and Shape Casting",

    # 3. AI
    "ai-navmesh-generation": "Recast/Detour Navigation Mesh Generation",
    "ai-pathfinding-astar": "Multi-threaded A* Pathfinding",
    "ai-steering-behaviors": "Seek, Flee, Arrive, Flocking Behaviors",
    "ai-perception-vision": "Visual Perception System (Cones)",
    "ai-perception-hearing": "Auditory Perception System",
    "ai-utility-system": "Utility-based Decision Making",
    "ai-blackboard-shared": "Shared Blackboard for Squad AI",
    "ai-cover-system": "Tactical Cover Finding System",
    "ai-goap": "Goal Oriented Action Planning Stub",
    "ai-dialogue-llm": "Local LLM Integration Hooks",

    # 4. Audio
    "audio-reverb-zones": "Convolution Reverb Zones",
    "audio-occlusion": "Raycast-based Sound Occlusion",
    "audio-doppler": "Velocity-based Doppler Effect",
    "audio-music-transitions": "Layered Music Transitions",
    "audio-footsteps": "Material-based Footstep Sounds",
    "audio-voice-chat": "VoIP Basic Implementation",
    "audio-synthesis": "Real-time Wave Synthesis",
    "audio-analysis-fft": "Real-time Spectrum Analysis (FFT)",
    "audio-hrtf": "Binaural 3D Audio Positioning",
    "audio-mixer-groups": "Audio Mixer Groups and Ducking",

    # 5. UI
    "ui-layout-flexbox": "Flexbox-like Layout Engines",
    "ui-rich-text": "Rich Text with Markdown/Colors",
    "ui-world-space": "3D World Space Widgets",
    "ui-inventory-grid": "Grid-based Inventory UI",
    "ui-skill-tree": "Skill Tree Visualization",
    "ui-dialogue-visual": "Visual Node Editor for Dialogue",
    "ui-minimap": "Minimap and Radar System",
    "ui-damage-numbers": "Floating Damage Numbers",
    "ui-settings-menu": "Auto-generated Settings Menu",
    "ui-accessibility": "TTS and High Contrast Modes",

    # 6. Network
    "net-lobby-system": "Matchmaking and Lobby System",
    "net-replication": "Entity State Replication",
    "net-interpolation": "Snapshot Interpolation",
    "net-prediction": "Client-side Prediction",
    "net-lag-compensation": "Lag Compensation (Rewind)",
    "net-chat-system": "Chat Channels and Commands",
    "net-voice-transport": "Voice Packet Transport",
    "net-security": "Token Authentication and Security",
    "net-serialization": "Optimized Bit-packing Serialization",
    "net-dedicated-server": "Headless Dedicated Server Mode",

    # 7. Editor
    "editor-gizmos": "Transform Gizmos (Translate/Rotate/Scale)",
    "editor-scene-hierarchy": "Scene Graph Hierarchy View",
    "editor-inspector": "Reflection-based Property Inspector",
    "editor-asset-browser": "Asset Browser with Thumbnails",
    "editor-console": "Runtime In-game Console",
    "editor-profiler-ui": "Visual Profiler UI",
    "editor-prefab-editor": "Prefab Editing System",
    "editor-play-mode": "Play/Simulate/Stop State Machine",
    "editor-timeline": "Cinematic Timeline Editor",
    "editor-undo-redo": "Command Pattern Undo/Redo",

    # 8. Optimization
    "opt-lod-system": "Mesh Level of Detail System",
    "opt-culling-frustum": "Advanced Frustum Culling",
    "opt-culling-occlusion": "Software Occlusion Culling",
    "opt-texture-streaming": "Texture Mip-map Streaming",
    "opt-memory-pool": "Custom Memory Pools/Arenas",
    "opt-ecs-archetypes": "ECS Archetype Optimization",
    "opt-job-system": "Thread Pool Job System",
    "opt-instancing": "Automatic GPU Instancing",
    "opt-simd-math": "SIMD Vector Math Optimizations",
    "opt-build-size": "Build Size Optimization",

    # 9. Gameplay
    "game-inventory-system": "Backend Inventory Data Structures",
    "game-quest-system": "Quest and Objective Tracking",
    "game-dialogue-manager": "Branching Dialogue Backend",
    "game-stat-system": "RPG Stats and Modifiers",
    "game-ability-system": "Ability Cooldowns and Costs",
    "game-interaction-system": "Interaction (Press F) System",
    "game-camera-system": "Advanced Camera Controllers",
    "game-save-cloud": "Cloud Save Integration Hooks",
    "game-achievements": "Achievement Tracking System",
    "game-tutorial-system": "Tutorial Step Manager",

    # 10. Platform
    "platform-steam-api": "Steamworks API Integration",
    "platform-discord-rpc": "Discord Rich Presence",
    "platform-input-remapping": "Runtime Input Remapping",
    "platform-display-settings": "Resolution and Display Settings",
    "platform-crash-reporting": "Sentry Crash Reporting",
    "platform-mod-support": "Mod Loading Support",
    "platform-dlc-manager": "DLC Content Management",
    "platform-localization-tool": "Localization CSV Tools",
    "platform-video-player": "Video Cutscene Player",
    "platform-analytics": "Analytics Event Tracking"
}

BASE_DIR = ".agent/skills"

TEMPLATE = """# Skill: {title}

## Context
This skill is part of the "God Tier" expansion for Starlight Engine, aiming to provide {description}.

## Goals
1. Implement the core logic for {name}.
2. Integrate with the existing systems.
3. Verify functionality with a test or demo.

## Implementation Steps
- [ ] Research best practices for {name}.
- [ ] Create necessary data structures in `crates/engine_core` or `pysrc/starlight`.
- [ ] Implement the main logic.
- [ ] Expose API to Python if needed.
- [ ] Create a usage example.
"""

def main():
    if not os.path.exists(BASE_DIR):
        os.makedirs(BASE_DIR)
    
    count = 0
    for key, desc in SKILLS.items():
        folder = os.path.join(BASE_DIR, key)
        if not os.path.exists(folder):
            os.makedirs(folder)
            
            md_path = os.path.join(folder, "SKILL.md")
            with open(md_path, "w", encoding="utf-8") as f:
                f.write(TEMPLATE.format(title=key, description=desc, name=key))
            print(f"Created: {key}")
            count += 1
        else:
            print(f"Skipped (Exists): {key}")

    print(f"Finished. Created {count} new skills.")

if __name__ == "__main__":
    main()
