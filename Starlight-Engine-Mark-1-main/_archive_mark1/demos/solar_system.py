import math
from template_scene import BaseDemo
from starlight import Entity

class SolarSystemDemo(BaseDemo):
    def __init__(self):
        super().__init__("Solar System Demo (Orbit Physics)", 1600, 900)
        self.sun = None
        self.planets = []
        self.t = 0.0

    def on_start(self):
        super().on_start() # Setup Skybox/Ground
        
        print("[Solar] Spawning Planets...")
        # 1. Sun (Yellow/Emissive?) - Index 0 is often a Cube, let's assume Sphere is Index 1 or similar
        # For now, using 'cube' model for everything if sphere not distinct, or 'sphere' if asset exists.
        # Based on assets, we have models but indices are magic. 
        # Let's use 'cube' scaled up.
        
        self.sun = Entity.spawn(self, 0.0, 5.0, 0.0, scale=3.0, model="cube")
        # Visual params: make it bright? Only if we have material overrides.
        
        # 2. Planets
        # (Distance, Speed, Scale, Color/Model)
        planet_data = [
            (8.0, 1.0, 0.5),   # Mercury
            (12.0, 0.7, 0.8),  # Venus
            (16.0, 0.5, 0.9),  # Earth
            (22.0, 0.4, 0.7),  # Mars
            (35.0, 0.2, 2.0),  # Jupiter
        ]
        
        for p in planet_data:
            dist, speed, scale = p
            planet = Entity.spawn(self, dist, 5.0, 0.0, scale=scale, model="cube")
            self.planets.append({"entity": planet, "dist": dist, "speed": speed, "angle": 0.0})

        # Set camera slightly higher
        # self.camera_pos defaults to 0,5,10 usually.

    def on_update(self, dt):
        self.t += dt
        
        # Animate Orbits
        for p in self.planets:
            p["angle"] += p["speed"] * dt
            x = math.cos(p["angle"]) * p["dist"]
            z = math.sin(p["angle"]) * p["dist"]
            
            # Update entity transform
            # Assuming set_position exists on entity wrapper or world
            # Entity wrapper in new pysrc usually has .set_position?
            # Let's check entity.py or use world method.
            # self.world.set_position(p["entity"].id, x, 5.0, z)
            
            # Using Entity wrapper method if available
            p["entity"].x = x
            p["entity"].z = z
            # Or p["entity"].position = (x, 5.0, z)

if __name__ == "__main__":
    demo = SolarSystemDemo()
    demo.run()
