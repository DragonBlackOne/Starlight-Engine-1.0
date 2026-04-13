"""Cube simulation with periodic screenshots for visual debugging."""
import math
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, Entity, backend
from starlight import math as smath


class DebugCubeDemo(App):
    def on_start(self):
        print("[Debug] Initializing visual debug scene...")
        
        # Spawn cube
        self.cube = Entity.spawn(self, 0.0, 1.0, 0.0, 1.5, model="cube")
        print(f"[Debug] Spawned cube: {self.cube.id}")
        
        # Spawn ground
        Entity.spawn(self, 0.0, -0.5, 0.0, 10.0, model="floor")
        
        self.t = 0.0
        self.screenshot_count = 0
        print("[Debug] Scene initialized: 1 Cube, 1 Floor")

    def on_update(self, dt):
        self.t += dt
        
        # Camera orbit
        radius = 8.0
        cam_x = math.sin(self.t * 0.3) * radius
        cam_z = math.cos(self.t * 0.3) * radius
        cam_y = 4.0

        # Calculate view matrix
        view = smath.look_at([cam_x, cam_y, cam_z], [0.0, 0.5, 0.0], [0.0, 1.0, 0.0])
        proj = smath.perspective(math.radians(60.0), 1280.0 / 720.0, 0.1, 1000.0)
        view_proj = smath.mat4_multiply(proj, view)
        
        # Update camera
        backend.update_camera(view_proj, [cam_x, cam_y, cam_z])

        # Take screenshots at specific times
        if self.screenshot_count == 0 and self.t > 1.0:
            backend.capture_screenshot("debug_frame_1s.png")
            self.screenshot_count += 1
            print("[Debug] Screenshot 1 taken at t=1s")

        if self.screenshot_count == 1 and self.t > 3.0:
            backend.capture_screenshot("debug_frame_3s.png")
            self.screenshot_count += 1
            print("[Debug] Screenshot 2 taken at t=3s")

        if self.screenshot_count == 2 and self.t > 5.0:
            backend.capture_screenshot("debug_frame_5s.png")
            self.screenshot_count += 1
            print("[Debug] Screenshot 3 taken at t=5s - Analysis complete!")


if __name__ == "__main__":
    print("[Debug] Starting Visual Debug Session...")
    DebugCubeDemo(title="Visual Debug - Cube").run()
