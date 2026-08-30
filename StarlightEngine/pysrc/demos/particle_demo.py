import starlight.backend as backend
import time
import math

def main():
    # Initialize engine
    backend.init_headless(1280, 720)
    
    # Setup scene
    backend.set_sun_direction(0.5, -1.0, 0.3)
    backend.set_sun_color(1.0, 0.9, 0.8, 2.0)
    backend.set_ambient_intensity(0.5)
    
    # Spawn a central emitter entity
    particle_ent = backend.spawn_entity(0, 0, 0)
    
    # Setup camera
    cam_id = backend.get_main_camera_id()
    backend.set_transform(cam_id, 0, 5, 15)
    backend.set_rotation(cam_id, -0.3, 0, 0)

    # Let's manually emit particles if the auto-system isn't fully wired yet
    # to guarantee a good screenshot.
    ps = backend.ParticleSystem(1000)
    
    # Set post process for glow
    backend.set_post_process_params(
        exposure=1.2,
        gamma=2.2,
        bloom_intensity=1.5,
        bloom_threshold=0.5,
        chromatic_aberration=0.002,
        time=0.0
    )
    
    print("[VFX DEMO] Running particle simulation...")
    
    # Simulate some frames
    for i in range(120):
        # Manually emit 10 particles per frame
        ps.emit(0.0, 0.0, 0.0, 10, 1.0, 0.5, 0.2, 1.0, 2.0, 0.5, 2.0, 0.1)
        ps.update(1.0/60.0, 0.0, -9.8)
        
        backend.update_game()
        backend.step_fixed()
        
        # Move camera in a slow circle
        angle = i * 0.02
        backend.set_transform(cam_id, math.sin(angle) * 15, 5, math.cos(angle) * 15)
        # Point camera at center roughly
        backend.set_rotation(cam_id, -0.3, angle, 0)
    
    # Capture final result
    print("[VFX DEMO] Capturing result...")
    backend.capture_screenshot("vfx_particles_validation.png")
    
    # Give time for screenshot capture
    time.sleep(2)
    print("[VFX DEMO] Done.")

if __name__ == "__main__":
    main()
