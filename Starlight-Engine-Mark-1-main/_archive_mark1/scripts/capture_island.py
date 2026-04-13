
import sys
import os
import time

# Add pysrc to path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "pysrc")))

# Import the demo class
from demos.island_survival import IslandSurvival
from starlight import backend

class CaptureIsland(IslandSurvival):
    def __init__(self):
        super().__init__(title="Capture: Island Survival", width=1280, height=720)
        self.capture_timer = 0.0
        self.captured = False
        
    def on_start(self):
        super().on_start()
        print("[CAP] Island started. Waiting for terrain generation and stabilization...")
        # Force state to PLAY to skip menu
        self.state = "PLAY" 
        self.input.set_cursor_grab(True)
        self.input.set_cursor_visible(False)
        
        # Position camera to look at shadows
        # Character starts at (0, 50, 0).
        # Let's move character to a good spot if needed, or just look down.
        # Shadow direction is (0.5, -0.8, 0.4). Sun comes from Top-East-South?
        # Shadows will fall towards West-North.
        
        # Camera is attached to character.
        # Let's look at the ground.
        self.pitch = -0.5 # Look down slightly
        
    def on_update(self, dt):
        super().on_update(dt)
        self.capture_timer += dt
        
        # Wait for 5 seconds to let everything load and physics settle
        if self.capture_timer > 5.0 and not self.captured:
            print("[CAP] Requesting screenshot...")
            if not os.path.exists("screenshots"):
                os.makedirs("screenshots")
            
            backend.capture_screenshot("screenshots/island_shadows.png")
            self.captured = True
            
        if self.capture_timer > 6.0:
            print("[CAP] Finished.")
            # Verify file exists
            if os.path.exists("screenshots/island_shadows.png"):
                print("[CAP] Screenshot verified.")
            else:
                print("[CAP] Screenshot FAILED to save.")
            os._exit(0)

if __name__ == "__main__":
    app = CaptureIsland()
    app.run()
