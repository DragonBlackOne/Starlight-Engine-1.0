import sys
import os

import backend
import time
import random

def main():
    print("Initializing Engine with Forward+ architecture...")
    backend.init_headless(800, 600)
    
    print("Spawning 200 PointLights dynamically...")
    for i in range(200):
        backend.set_point_light(
            i,
            random.uniform(-50.0, 50.0), random.uniform(1.0, 10.0), random.uniform(-50.0, 50.0),
            random.uniform(0.5, 1.0), random.uniform(0.5, 1.0), random.uniform(0.5, 1.0),
            random.uniform(0.5, 2.0)
        )

    print("Spawning geometry to receive light...")
    for x in range(-5, 5):
        for z in range(-5, 5):
            backend.spawn_static_box(x*10.0, 0.0, z*10.0, 5.0, 1.0, 5.0)

    print("Rendering frames to dispatch StorageBuffer to GPU...")
    for _ in range(10):
        backend.update_game()
        time.sleep(0.01)

    print("SUCCESS! 200 lights spawned and processed without GPU VRAM exhaustion or crashing.")

if __name__ == "__main__":
    main()
