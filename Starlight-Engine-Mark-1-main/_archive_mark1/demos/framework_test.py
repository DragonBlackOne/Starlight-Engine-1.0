import os
import sys
import math

# Ensure pysrc is in path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, Scene, FrameworkEntity, Camera, backend

class FrameworkTest(App):
    def on_start(self):
        print("[TEST] Initializing Framework Scene...")
        self.scene = Scene()
        self.camera = Camera()
        
        # Setup Environment
        self.scene.set_sun(
            direction=(0.5, -1.0, 0.5), 
            color=(1.0, 0.9, 0.7), 
            intensity=2.0
        )
        self.scene.set_ambient(0.15)
        self.scene.set_fog(0.01, (0.5, 0.6, 0.7))
        
        # Add a ground
        self.ground = FrameworkEntity("Ground", 0, -1, 0)
        self.ground.set_mesh("cube")
        self.ground.set_scale(50, 1, 50)
        self.ground.set_color(0.2, 0.3, 0.2)
        self.scene.add(self.ground)
        
        # Add a central monolith
        self.monolith = FrameworkEntity("Monolith", 0, 2, 0)
        self.monolith.set_mesh("cube")
        self.monolith.set_scale(1, 4, 1)
        self.monolith.set_color(0.8, 0.1, 0.1)
        self.scene.add(self.monolith)
        
        # Load custom mesh if exists
        try:
            trunk_id = backend.load_mesh("assets/models/generated/trunk.obj")
            bark_id = backend.load_texture("assets/textures/oak_bark.png")
            
            self.tree = FrameworkEntity("Tree", 5, 0, 5)
            self.tree.set_mesh(trunk_id)
            self.tree.set_material(bark_id)
            self.scene.add(self.tree)
        except:
            print("[TEST] Custom assets not found, skipping tree.")

        self.t = 0
        self.captured = False

    def on_update(self, dt):
        self.t += dt
        
        # Orbit camera
        cam_x = math.sin(self.t * 0.5) * 15.0
        cam_z = math.cos(self.t * 0.5) * 15.0
        self.camera.set_transform(cam_x, 5.0, cam_z)
        self.camera.look_at((0, 2, 0))
        
        # Rotate monolith
        self.monolith.set_rotation(0, self.t, 0)
        
        if self.t > 3.0 and not self.captured:
            backend.capture_screenshot("screenshots/framework_verification.png")
            print("[TEST] Framework Verification Screenshot saved!")
            self.captured = True

if __name__ == "__main__":
    os.makedirs("screenshots", exist_ok=True)
    FrameworkTest(title="Starlight Framework Verification").run()
