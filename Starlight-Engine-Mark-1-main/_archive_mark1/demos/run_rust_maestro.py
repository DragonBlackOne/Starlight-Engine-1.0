# Starlight Engine - Rust Maestro Launcher
# This is how you start the engine with Rust as the main loop

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, backend


class RustMaestroDemo(App):
    """Simple demo showing engine running with Rust main loop."""
    
    def on_start(self):
        print("[Maestro] Engine started with Rust as Maestro!")
        
        # Spawn some cubes
        for i in range(3):
            x = (i - 1) * 2.0
            backend.spawn_dynamic_cube(x, 1.0, 0.0, 1.0, 0.3, 0.6)
        
        self.t = 0.0

    def on_update(self, dt):
        self.t += dt
        
        # Simple camera update
        if hasattr(backend, 'update_camera'):
            import math
            from starlight import math as smath
            
            cam_x = math.sin(self.t * 0.5) * 10.0
            cam_z = math.cos(self.t * 0.5) * 10.0
            cam_y = 5.0
            
            view = smath.look_at([cam_x, cam_y, cam_z], [0.0, 0.0, 0.0], [0.0, 1.0, 0.0])
            proj = smath.perspective(math.radians(60.0), 1280.0 / 720.0, 0.1, 1000.0)
            view_proj = smath.mat4_multiply(proj, view)
            
            backend.update_camera(view_proj, [cam_x, cam_y, cam_z])


if __name__ == "__main__":
    print("Starting Starlight Engine (Rust Maestro)...")
    RustMaestroDemo(title="Starlight Engine (Rust Maestro)").run()
