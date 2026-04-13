import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "pysrc")))

from starlight import App, Entity, Keys

class BaseDemo(App):
    """
    A base template for visual demos.
    Automatically sets up:
    - Skybox
    - Ground Plane (Tiled)
    - Lighting (IBL + Sun)
    - Grid Disabled
    """
    def __init__(self, title="Starlight Engine Demo", width=1600, height=900):
        super().__init__(title, width, height)

    def on_start(self):
        print("[BaseDemo] Initialization started...")
        print(f"World attributes: {dir(self.world)}")
        try:
            # 1. Disable Debug Grid (We want a real scene)
            self.world.show_grid = False
        except Exception as e:
            print(f"Error setting show_grid: {e}")
        
        # 2. Setup Lighting & Atmosphere
        self.world.set_sun_direction(0.5, -0.8, 0.5)
        self.world.set_visual_params(1.0, 1.0) # Full IBL & Skybox
        self.world.set_fog(0.005, 0.4, 0.45, 0.5) # Slight atmospheric fog
        
        # 3. Load Skybox
        # Use simple error handling in case files are missing
        try:
            self.world.load_skybox(
                "assets/skybox/right.jpg",
                "assets/skybox/left.jpg",
                "assets/skybox/top.jpg",
                "assets/skybox/bottom.jpg",
                "assets/skybox/front.jpg",
                "assets/skybox/back.jpg"
            )
        except Exception as e:
            print(f"[BaseDemo] Warning: Could not load skybox: {e}")

        # 4. Create nice Ground Plane
        # Using index 2 (which maps to ground material logic in recent demos)
        # or use standard texture loading if available.
        # Assuming asset loader defaults 'ground_grass_diffuse.png' to id 2 or similar.
        # Let's spawn a grid of tiled grounds.
        for x in range(-2, 3):
            for z in range(-2, 3):
                 # index=2 is typically ground/grass in our engine setup
                 Entity.spawn(self, x * 20.0, 0.0, z * 20.0, scale=10.0, model="index=2")

        print("[BaseDemo] Initialization complete.")
        # Capture screenshot for verification
        self.world.capture_screenshot("template_verify.png")

    def on_update(self, dt):
        pass

if __name__ == "__main__":
    app = BaseDemo()
    app.run()
