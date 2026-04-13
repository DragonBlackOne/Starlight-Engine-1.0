import sys
import os
sys.path.insert(0, os.path.abspath("pysrc"))

from starlight import App
from starlight import backend
from starlight.framework import Scene, Entity, Camera
import time

class SanityTest(App):
    def on_start(self):
        print("SANITY CHECK STARTED")
        self.scene = Scene()
        
        # 1. Camera at 0,0,5 looking at 0,0,0
        self.camera = Camera()
        self.camera.set_position(0, 0, 10)
        self.camera.look_at((0, 0, 0))
        
        # 2. Simple Cube at 0,0,0
        self.cube = Entity("DebugCube", 0, 0, 0).set_mesh("cube")
        self.cube.set_scale(1.0, 1.0, 1.0).set_color(1.0, 0.0, 0.0) # RED
        # Force emissive to ignore lighting
        self.cube.set_material_params(1.0, 0.0) 
        self.scene.add(self.cube)
        
        # 3. Sun just in case
        self.scene.set_sun((0.0, -1.0, 0.0), (1.0, 1.0, 1.0), 1.0)
        
        self.frame = 0

    def on_update(self, dt):
        self.cube.transform.rotation[1] += dt # Rotate
        self.frame += 1
        if self.frame == 10:
            print("Capturing sanity_check.png")
            backend.capture_screenshot("sanity_check.png")
        if self.frame >= 20:
            print("Sanity Check Complete")
            self.quit()

if __name__ == "__main__":
    SanityTest(title="Sanity Check", width=800, height=600).run()
