import sys
import os
import random
sys.path.insert(0, os.path.abspath("pysrc"))
import starlight.backend as backend
from starlight.procgen import VegetationSpawner, FaunaSystem, FloraLayer
from dataclasses import dataclass

# Mock App for ProcGen systems
class MockApp:
    pass

class ForestHeadless:
    def run(self):
        print("Initializing Headless Forest...")
        backend.init_headless(1920, 1080)
        
        # Setup
        app = MockApp()
        
        # Floor
        backend.spawn_floor(-0.5)
        
        # Vegetation
        print("Generating vegetation...")
        # Use small area for speed
        CONFIG = {
            "area_size": 30.0,
            "clearing_radius": 5.0,
            "density_multiplier": 0.2
        }
        
        vegetation = VegetationSpawner(app, seed=42)
        vegetation.spawn_forest(
            area_size=CONFIG["area_size"],
            clearing_radius=CONFIG["clearing_radius"],
            density_multiplier=CONFIG["density_multiplier"],
            layers=[FloraLayer.EMERGENT, FloraLayer.CANOPY, FloraLayer.UNDERSTORY]
        )
        
        # Fauna
        print("Initializing fauna...")
        fauna = FaunaSystem(app, area_size=CONFIG["area_size"])
        fauna.initialize()
        
        # Loop mainly to let physics/fauna settle?
        # Just render one frame
        print("Rendering and capturing...")
        
        # Update loop simulation (10 frames)
        for i in range(10):
            fauna.update(0.1)
            backend.render_frame() # Renders + Flushes
            
        # Capture
        backend.capture_screenshot("forest_headless_verify.png")
        # Need one more render to process the screenshot request
        backend.render_frame() 
        
        # Audio Test
        print("Testing Audio...")
        try:
            snd_id = backend.play_sound("assets/bgm.wav", True, 0.5, 0.5)
            print(f"Playing BGM ID: {snd_id}")
            backend.set_sound_panning(snd_id, 0.8)
            backend.set_sound_volume(snd_id, 0.7)
        except Exception as e:
            print(f"Audio failed: {e}")

        print("Screenshot command issued. Waiting for save...")
        # Maybe wait? synchronous render should handle it in `render_frame`.
        # backend.render_frame() calls rs.render() which saves.
        
        print("Done.")

if __name__ == "__main__":
    demo = ForestHeadless()
    demo.run()
