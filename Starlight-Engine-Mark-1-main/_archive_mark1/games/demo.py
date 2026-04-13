"""Demo Game - Shows player movement and physics."""
import math
import os
import random
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, Entity, backend
from starlight import math as smath


class DemoGame(App):
    def on_start(self):
        print("Starting Demo Game...")
        
        # Lighting
        backend.set_sun_direction(0.5, 0.8, 0.2)
        backend.set_fog(0.02, 0.5, 0.6, 0.7)
        
        # Ground
        Entity.spawn(self, 0, -2, 0, 10.0, model="floor")

        # Player (simple cube for now)
        self.player = Entity.spawn(self, 0, 2, 0, 1.0, model="cube")
        
        # Camera
        self.camera_angle = 0.0
        self.camera_dist = 10.0
        self.t = 0.0

        # Spawn some boxes
        for i in range(5):
            Entity.spawn(self, random.uniform(-5, 5), 5 + i * 2, random.uniform(-5, 5), 1.0, model="cube")
        
        print("Demo Game initialized!")

    def on_update(self, dt):
        self.t += dt
        
        # Camera Orbit
        self.camera_angle += dt * 0.5
        
        # Camera Position
        if self.player.transform:
            px, py, pz = self.player.transform.x, self.player.transform.y, self.player.transform.z
        else:
            px, py, pz = 0, 2, 0
            
        cx = px - math.sin(self.camera_angle) * self.camera_dist
        cz = pz - math.cos(self.camera_angle) * self.camera_dist
        cy = py + 5.0
        
        # Update camera
        view = smath.look_at([cx, cy, cz], [px, py, pz], [0.0, 1.0, 0.0])
        proj = smath.perspective(math.radians(60.0), 1280.0 / 720.0, 0.1, 1000.0)
        view_proj = smath.mat4_multiply(proj, view)
        backend.update_camera(view_proj, [cx, cy, cz])


if __name__ == "__main__":
    DemoGame(title="Starlight MVP").run()
