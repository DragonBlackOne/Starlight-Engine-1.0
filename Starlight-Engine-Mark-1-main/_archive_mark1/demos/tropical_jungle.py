"""Tropical Jungle Demo - Procedural Forest Environment."""
import math
import os
import random
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, backend
from starlight import math as smath


class TropicalJungleDemo(App):
    def on_start(self):
        print("[DEMO] Initializing Tropical Jungle...")
        
        # Set fog for atmosphere
        backend.set_fog(0.015, 0.8, 0.9, 0.95)
        print("[DEMO] Deep Jungle Fog Enabled.")
        
        # Set sun direction
        backend.set_sun_direction(0.0, 0.3, 1.0)
        
        # Spawn ground
        backend.spawn_floor(-0.5, 0.3, 0.4, 0.25)
        
        # Generate forest with cubes as placeholder trees
        FOREST_SIZE = 50.0
        TREE_COUNT = 100
        
        print(f"[DEMO] Spawning {TREE_COUNT} trees...")
        for i in range(TREE_COUNT):
            x = (random.random() * FOREST_SIZE) - (FOREST_SIZE / 2.0)
            z = (random.random() * FOREST_SIZE) - (FOREST_SIZE / 2.0)
            height = 3.0 + random.random() * 4.0
            
            # Green tree trunk (cube)
            backend.spawn_static_box(x, height/2, z, 0.5, height, 0.5, 0.3, 0.5, 0.2)
        
        self.t = 0.0
        self.screenshot_taken = False
        print("[DEMO] Jungle Generation Complete.")

    def on_update(self, dt):
        self.t += dt
        
        # Cinematic camera flythrough
        cam_x = math.sin(self.t * 0.1) * 30.0
        cam_z = (self.t * 2.0) % 100.0 - 50.0
        cam_y = 5.0 + math.sin(self.t * 0.2) * 2.0
        
        target_x = cam_x + math.sin(self.t * 0.1) * 10.0
        target_y = cam_y
        target_z = cam_z + 10.0
        
        view = smath.look_at([cam_x, cam_y, cam_z], [target_x, target_y, target_z], [0.0, 1.0, 0.0])
        proj = smath.perspective(math.radians(60.0), 1280.0 / 720.0, 0.1, 1000.0)
        view_proj = smath.mat4_multiply(proj, view)
        
        backend.update_camera(view_proj, [cam_x, cam_y, cam_z])
        
        # Capture screenshot
        if self.t > 5.0 and not self.screenshot_taken:
            backend.capture_screenshot("tropical_verify.png")
            self.screenshot_taken = True
            print("[DEMO] Screenshot captured!")


if __name__ == "__main__":
    print("[DEMO] Starting Tropical Jungle Demo...")
    TropicalJungleDemo(title="Tropical Jungle").run()
