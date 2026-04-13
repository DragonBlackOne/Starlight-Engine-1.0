# Starlight Engine - Rust Maestro Phase 3 Launcher
# Demonstrates Python Scripting (Input, Entity Control)

import starlight_rust

print("Launching Starlight Engine - Phase 3 (Python Scripting)...")
print("Controls: WASD or Arrow Keys to move the player (invisible entity but prints pos)")

starlight_rust.run_engine(
    title="Starlight Engine - Phase 3", width=1280, height=720, script_path="tests/test_phase3.py"
)
