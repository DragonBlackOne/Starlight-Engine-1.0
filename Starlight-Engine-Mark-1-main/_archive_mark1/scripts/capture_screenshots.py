"""Capture with proper lighting and bright colors."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "pysrc"))

from starlight import App, Entity, backend


class CaptureDemo(App):
    def __init__(self, name):
        super().__init__(title=f"Capture: {name}")
        self.name = name
        self.t = 0.0
        self.captured = False
        
    def on_start(self):
        print(f"[CAP] {self.name} - Setting up lighting...")
        
        # Setup directional light
        try:
            backend.set_sun_direction(0.5, 0.8, 0.3)
            print("[CAP] Sun direction set")
        except:
            print("[CAP] No sun_direction available")
            
        print(f"[CAP] {self.name} - Spawning bright colored scene...")
        
        # Use BRIGHT colors that will be visible
        Entity.spawn(self, 0, 0.5, 0, 1.5, model="cube", color=(1.0, 0.2, 0.2))  # Red
        Entity.spawn(self, -3, 0.5, 0, 1.0, model="cube", color=(0.2, 1.0, 0.2))  # Green
        Entity.spawn(self, 3, 0.5, 0, 1.0, model="cube", color=(0.2, 0.2, 1.0))  # Blue
        Entity.spawn(self, 0, -0.5, 0, 20.0, model="floor", color=(0.5, 0.5, 0.5))  # Gray floor
        
        print(f"[CAP] {self.name} - Ready!")
    
    def on_update(self, dt):
        self.t += dt
        
        if self.t >= 1.5 and not self.captured:
            path = f"screenshots/{self.name}.png"
            print(f"[CAP] Saving: {path}")
            backend.capture_screenshot(path)
            self.captured = True
        
        if self.t >= 2.0:
            print(f"[CAP] Done!")
            os._exit(0)


if __name__ == "__main__":
    os.makedirs("screenshots", exist_ok=True)
    name = sys.argv[1] if len(sys.argv) > 1 else "demo"
    CaptureDemo(name).run()
