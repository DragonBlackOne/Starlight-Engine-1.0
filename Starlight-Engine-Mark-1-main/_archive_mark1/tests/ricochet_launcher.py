import os

import starlight_rust

# Configuration
SCREEN_WIDTH = 1280
SCREEN_HEIGHT = 720
SCRIPT_NAME = "ricochet_native.py"


def main():
    print("[Launcher] Starting Ricochet Native...")

    # Calculate absolute path to the logic script
    current_dir = os.path.dirname(os.path.abspath(__file__))
    script_path = os.path.join(current_dir, SCRIPT_NAME)

    if not os.path.exists(script_path):
        print(f"[Launcher] Error: Script not found at {script_path}")
        return

    print(f"[Launcher] Loading logic from: {script_path}")

    # Launch Engine
    # The Engine will load 'ricochet_native.py' as a module "game_script"
    try:
        starlight_rust.run_engine("Ricochet Native", SCREEN_WIDTH, SCREEN_HEIGHT, script_path)
    except Exception as e:
        print(f"[Launcher] Engine crashed: {e}")


if __name__ == "__main__":
    main()
