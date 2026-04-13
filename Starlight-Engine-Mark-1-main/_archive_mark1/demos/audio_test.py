
import sys
import os
import math
import time

try:
    import starlight.backend as backend
    print("Backend imported successfully.")
except ImportError as e:
    print(f"Failed to import backend: {e}")
    sys.exit(1)

try:
    from starlight.audio import audio, SpatialAudio
    print("Audio module imported successfully.")
except ImportError as e:
    print(f"Failed to import audio module: {e}")
    # Print sys.path for debugging
    print(sys.path)
    sys.exit(1)

class MockApp:
    pass

def run_audio_test():
    print("Initializing Audio Test...")
    backend.init_headless(800, 600)
    
    # Setup Listener (Camera at 0,0,0 facing +Z)
    listener_pos = [0.0, 0.0, 0.0]
    listener_rot_y = 0.0 # Facing +Z (South)
    
    # Setup Sound Source
    print("Loading sound...")
    # Using bgm.wav as a test sound
    try:
        sound_source = SpatialAudio(audio, "assets/bgm.wav", looped=True, max_distance=20.0)
    except Exception as e:
        print(f"Error creating SpatialAudio: {e}")
        return

    # Start playing
    sound_source.play()
    sound_source.position = [0.0, 0.0, 10.0] # 10m in front
    
    print(f"Playing handle: {sound_source.handle}")
    
    print("Simulating movement...")
    
    # Move source around listener
    steps = 100
    radius = 10.0
    
    for i in range(steps):
        angle = (i / steps) * 2 * math.pi
        
        # Orbit around
        x = math.sin(angle) * radius
        z = math.cos(angle) * radius
        
        sound_source.position = [x, 0.0, z]
        sound_source.update(listener_pos, listener_rot_y)
        
        # Log status occasionally
        if i % 10 == 0:
            print(f"Step {i}: Pos=({x:.1f}, {z:.1f})") 

        backend.render_frame()
        time.sleep(0.01) # fast simulation
        
    print("Stopping sound...")
    sound_source.stop()
    
    # Test Distance Attenuation
    print("Testing Attenuation...")
    sound_source.play()
    for z in range(0, 30, 2):
        sound_source.position = [0.0, 0.0, float(z)]
        sound_source.update(listener_pos, listener_rot_y)
        print(f"Distance {z}: Moving away...")
        backend.render_frame()
        time.sleep(0.01)
        
    sound_source.stop()
    print("Audio Test Complete.")

if __name__ == "__main__":
    run_audio_test()
