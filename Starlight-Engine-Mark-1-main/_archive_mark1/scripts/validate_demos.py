"""Automated Demo Validator - Runs demos and captures screenshots."""
import os
import sys
import time
import subprocess

sys.path.append(os.path.join(os.path.dirname(__file__), "pysrc"))

from starlight import App, Entity, backend
from starlight import math as smath
import math


class ValidatorDemo(App):
    """Generic validator that captures screenshot after brief run."""
    
    def __init__(self, title, demo_name, setup_fn=None):
        super().__init__(title=title)
        self.demo_name = demo_name
        self.setup_fn = setup_fn
        self.t = 0.0
        self.screenshot_taken = False
        
    def on_start(self):
        print(f"[Validator] Starting {self.demo_name}...")
        
        # Default setup: spawn some objects
        if self.setup_fn:
            self.setup_fn(self)
        else:
            # Default scene
            Entity.spawn(self, 0, 1, 0, 1.0, model="cube")
            Entity.spawn(self, 0, -0.5, 0, 10.0, model="floor")
        
        print(f"[Validator] {self.demo_name} initialized!")
    
    def on_update(self, dt):
        self.t += dt
        
        # Simple camera orbit
        cam_x = math.sin(self.t * 0.5) * 8.0
        cam_z = math.cos(self.t * 0.5) * 8.0
        cam_y = 5.0
        
        view = smath.look_at([cam_x, cam_y, cam_z], [0.0, 0.5, 0.0], [0.0, 1.0, 0.0])
        proj = smath.perspective(math.radians(60.0), 1280.0 / 720.0, 0.1, 1000.0)
        view_proj = smath.mat4_multiply(proj, view)
        backend.update_camera(view_proj, [cam_x, cam_y, cam_z])
        
        # Capture screenshot after 2 seconds
        if self.t > 2.0 and not self.screenshot_taken:
            screenshot_path = f"screenshots/{self.demo_name}.png"
            print(f"[Validator] Capturing screenshot: {screenshot_path}")
            backend.capture_screenshot(screenshot_path)
            self.screenshot_taken = True
        
        # Exit after 3 seconds
        if self.t > 3.0:
            print(f"[Validator] {self.demo_name} validation complete!")
            # Force exit
            os._exit(0)


def setup_simple_cube(app):
    Entity.spawn(app, 0, -2, 0, 10.0, model="floor")
    app.cube = Entity.spawn(app, 0, 0, 0, 2.0, model="cube")

def setup_minimal_render(app):
    Entity.spawn(app, 0.0, 1.0, 0.0, 1.0, model="cube")
    Entity.spawn(app, -3.0, 1.0, 0.0, 1.0, model="cube")
    Entity.spawn(app, 3.0, 1.0, 0.0, 1.0, model="cube")
    Entity.spawn(app, 0.0, -1.0, 0.0, 10.0, model="floor")

def setup_multi_objects(app):
    import random
    for i in range(8):
        x = (i % 4 - 1.5) * 2.0
        z = (i // 4 - 0.5) * 2.0
        Entity.spawn(app, x, 0.5, z, 1.0, model="cube")
    Entity.spawn(app, 0, -0.5, 0, 15.0, model="floor")


if __name__ == "__main__":
    demo_name = sys.argv[1] if len(sys.argv) > 1 else "test"
    
    setups = {
        "simple_cube": setup_simple_cube,
        "minimal_render": setup_minimal_render,
        "multi_objects": setup_multi_objects,
        "default": None,
    }
    
    setup_fn = setups.get(demo_name, None)
    ValidatorDemo(f"Validation: {demo_name}", demo_name, setup_fn).run()
