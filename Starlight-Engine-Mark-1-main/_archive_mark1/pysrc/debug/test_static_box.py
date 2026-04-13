"""Test spawn_static_box directly for proper colors."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "pysrc"))

from starlight import App, backend


class StaticBoxTest(App):
    def __init__(self):
        super().__init__(title="Static Box Color Test")
        self.t = 0.0
        self.captured = False
        
    def on_start(self):
        print("[TEST] Spawning colored static boxes...")
        
        # Try spawn_static_box with colors
        # signature: (x, y, z, sx, sy, sz, r, g, b)
        backend.spawn_static_box(0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0)  # Red
        backend.spawn_static_box(-3.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0)  # Green
        backend.spawn_static_box(3.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0)  # Blue
        
        # Floor
        backend.spawn_static_box(0.0, -0.5, 0.0, 20.0, 0.5, 20.0, 0.5, 0.5, 0.5)  # Gray
        
        print("[TEST] Scene ready!")
    
    def on_update(self, dt):
        self.t += dt
        
        if self.t >= 1.5 and not self.captured:
            path = "screenshots/static_box_test.png"
            print(f"[TEST] Saving: {path}")
            backend.capture_screenshot(path)
            self.captured = True
        
        if self.t >= 2.0:
            print("[TEST] Done!")
            os._exit(0)


if __name__ == "__main__":
    os.makedirs("screenshots", exist_ok=True)
    StaticBoxTest().run()
