"""Final validation with bright colors using Entity.spawn."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "pysrc"))

from starlight import App, Entity, backend


class FinalValidation(App):
    def __init__(self):
        super().__init__(title="Final Color Validation")
        self.t = 0.0
        self.captured = False
        
    def on_start(self):
        print("[FINAL] Creating colored scene...")
        
        # Use Entity.spawn with explicit bright colors
        Entity.spawn(self, -3.0, 1.0, 0.0, 1.5, model="cube", color=(1.0, 0.2, 0.2))  # Red
        Entity.spawn(self, 0.0, 1.0, 0.0, 1.5, model="cube", color=(0.2, 1.0, 0.2))   # Green
        Entity.spawn(self, 3.0, 1.0, 0.0, 1.5, model="cube", color=(0.2, 0.2, 1.0))   # Blue
        
        # Floor
        Entity.spawn(self, 0, 0, 0, 30.0, model="floor", color=(0.6, 0.6, 0.6))
        
        print("[FINAL] Scene ready!")
    
    def on_update(self, dt):
        self.t += dt
        
        if self.t >= 1.5 and not self.captured:
            path = "screenshots/final_validation.png"
            print(f"[FINAL] Saving: {path}")
            backend.capture_screenshot(path)
            self.captured = True
        
        if self.t >= 2.0:
            print("[FINAL] Done!")
            os._exit(0)


if __name__ == "__main__":
    os.makedirs("screenshots", exist_ok=True)
    FinalValidation().run()
