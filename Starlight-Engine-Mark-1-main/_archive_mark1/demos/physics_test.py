"""
Physics Test Demo - Verifies apply_impulse, get_velocity, set_velocity.
"""
import sys
import os
import time

try:
    import starlight.backend as backend
    from starlight.components import RigidBody
    print("Imports OK.")
except ImportError as e:
    print(f"Import failed: {e}")
    sys.exit(1)

def run_physics_test():
    print("Initializing Physics Test...")
    backend.init_headless(800, 600)
    
    # Spawn a dynamic cube that will be affected by physics
    cube_id = backend.spawn_dynamic_cube(0.0, 10.0, 0.0, 1.0, 0.5, 0.5)
    print(f"Spawned dynamic cube ID: {cube_id}")
    
    # Wrap with Python RigidBody
    rb = RigidBody(cube_id)
    
    # Check initial velocity (should be 0 or near 0)
    vel = rb.get_velocity()
    print(f"Initial velocity: {vel}")
    
    # Apply an upward impulse
    print("Applying upward impulse (0, 50, 0)...")
    rb.apply_impulse(0.0, 50.0, 0.0)
    
    # Step physics a few times
    for i in range(20):
        backend.render_frame()
    
    # Check velocity after impulse
    vel = rb.get_velocity()
    print(f"Velocity after impulse: {vel}")
    
    # Set velocity directly
    print("Setting velocity to (10, 0, 5)...")
    rb.set_velocity(10.0, 0.0, 5.0)
    
    vel = rb.get_velocity()
    print(f"Velocity after set: {vel}")
    
    # Apply force (should accelerate)
    print("Applying force (100, 0, 0) for 1 second...")
    for i in range(50):
        rb.apply_force(100.0, 0.0, 0.0)
        backend.render_frame()
    
    vel = rb.get_velocity()
    print(f"Final velocity after force: {vel}")
    
    print("Physics Test Complete!")

if __name__ == "__main__":
    run_physics_test()
