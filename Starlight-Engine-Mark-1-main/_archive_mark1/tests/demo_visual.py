# Starlight Engine - Visual Test (Phase 5)
# Should show a white quad controlled by WASD

import starlight_rust

print("Launching Starlight Engine - Visual Test...")
print("Controls: WASD or Arrow Keys to move the White Quad")

starlight_rust.run_engine(
    title="Starlight Engine - Visual Test",
    width=1280,
    height=720,
    script_path="tests/test_phase5.py",  # Phase 5: Sprite Rendering
)
