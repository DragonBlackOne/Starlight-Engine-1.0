
import sys
import os
import random
import math

# Add pysrc to path
current_dir = os.path.dirname(os.path.abspath(__file__))
pysrc_dir = os.path.join(os.path.dirname(current_dir), 'pysrc')
sys.path.insert(0, pysrc_dir)

from starlight.ai.steering import SteeringAgent, Vector3

def run_flocking_simulation():
    print("Initializing Flocking Simulation...")
    
    # Create a flock
    flock_size = 20
    flock = []
    for _ in range(flock_size):
        # Spawn in a random box [-10, 10]
        x = random.uniform(-10, 10)
        y = random.uniform(0, 5) # Birds usually fly higher
        z = random.uniform(-10, 10)
        agent = SteeringAgent(x, y, z)
        flock.append(agent)
        
    print(f"Created flock of {flock_size} agents.")
    
    # Simulation loop
    steps = 100
    dt = 0.1
    
    print("Running simulation...")
    for step in range(steps):
        # Calculate forces for each agent
        for agent in flock:
            # Flocking behavior
            # Weights: Separation 2.0, Alignment 1.0, Cohesion 1.0
            agent.flock(flock, sep_w=2.0, alg_w=1.0, coh_w=1.0)
            
            # Bound them to a center point (Mock "Stay in Forest")
            to_center = agent.seek(Vector3(0, 10, 0))
            agent.apply_force(to_center * 0.5)
            
        # Update agents
        for agent in flock:
            agent.update(dt)
            
        # Metrics
        if step % 20 == 0:
            avg_pos = Vector3(0,0,0)
            for a in flock:
                avg_pos += a.position
            avg_pos = avg_pos / flock_size
            
            # Calculate spread
            spread = 0
            for a in flock:
                spread += a.position.distance_to(avg_pos)
            avg_spread = spread / flock_size
            
            print(f"Step {step}: Centroid=({avg_pos.x:.2f}, {avg_pos.y:.2f}, {avg_pos.z:.2f}), Avg Spread={avg_spread:.2f}")

    print("Simulation complete.")
    
    # Validation: Check if they are somewhat cohesive
    final_spread = 0
    avg_pos = Vector3(0,0,0)
    for a in flock:
        avg_pos += a.position
    avg_pos = avg_pos / flock_size
    for a in flock:
        final_spread += a.position.distance_to(avg_pos)
    final_spread /= flock_size
    
    if final_spread < 20.0:
        print("SUCCESS: Flock stayed cohesive.")
    else:
        print("WARNING: Flock dispersed too much.")

if __name__ == "__main__":
    run_flocking_simulation()
