from template_scene import BaseDemo
from starlight import Keys

class ParticlesShowcase(BaseDemo):
    def __init__(self):
        super().__init__("Particle Effects Showcase", 1600, 900)
        self.mode = 0
        self.modes = ["Fire (Default)", "Rain", "Magic/Snow", "Explosion"]
        self.time_since_switch = 0.0

    def on_start(self):
        super().on_start()
        print("[Particles] Press SPACE to cycle effects.")
        
        # DEBUG: Check if spawn_particles exists
        print(f"[Particles] World attributes: {dir(self.world)}")
        
        # Start with Fire
        self.set_effect(0)
        
        # Set night time for better particle visibility
        self.world.set_sun_direction(0.1, -0.2, 0.1) # Sunset/Night
        self.world.set_visual_params(0.3, 0.5) # Darker ambient

    def set_effect(self, mode_idx):
        self.mode = mode_idx % len(self.modes)
        print(f"[Particles] Switched to: {self.modes[self.mode]}")
        
        # Clear existing? We don't have clear_particles yet exposed maybe?
        # Actually particle system is persistent unless lifetime ends.
        # But we can change gravity or spawn different emitters.
        
        m = self.mode
        if m == 0: # Fire
            # Fire logic is usually high emission, low gravity, short life, red/orange
            # self.world.spawn_particle_emitter(...) if we had an emitter object.
            # Currently we spawn individual particles or have a hardcoded test?
            # ECS has `spawn_particle(x,y,z, vx,vy,vz, life, color_idx)`
            # Let's spawn a continuous stream in on_update.
            self.world.particle_gravity = (0.0, 1.0) # Rising smoke/fire
            
        elif m == 1: # Rain
            self.world.particle_gravity = (0.0, -9.8) # Falling
            
        elif m == 2: # Magic
            self.world.particle_gravity = (0.0, 0.0) # Floating
            
        elif m == 3: # Explosion
            self.world.particle_gravity = (0.0, -2.0)

    def on_update(self, dt):
        self.time_since_switch += dt
        
        # Input handling for switching
        # Accessing input via self.input? Or direct Keys?
        # wrapper App usually exposes self.input
        # But let's check starlight.input wrapper in pysrc. 
        # Actually App.run handles input, but exposure might be via world or global input.
        # Let's assume automatic cycling for demo purposes if input not easy.
        
        if self.time_since_switch > 5.0:
            self.time_since_switch = 0.0
            self.set_effect(self.mode + 1)
            
        # Emit particles continuously based on mode
        # x, y, z, count, r, g, b, speed, life, size
        m = self.mode
        if m == 0: # Fire
            # Orange/Red, rising, short life
            self.world.spawn_particles(0.0, 1.0, 0.0, 5, 1.0, 0.5, 0.0, 2.0, 1.5, 0.3)
            self.world.spawn_particles(0.0, 1.0, 0.0, 2, 1.0, 0.2, 0.0, 3.0, 1.0, 0.2)
            
        elif m == 1: # Rain
            # Blue/White, falling (gravity set in set_effect), widespread
            import random
            for _ in range(10): # Heavy rain
                px = (random.random() - 0.5) * 40.0
                pz = (random.random() - 0.5) * 40.0
                self.world.spawn_particles(px, 15.0, pz, 1, 0.7, 0.8, 1.0, 0.0, 2.0, 0.1)
            
        elif m == 2: # Magic
            # Purple/Cyan, floating
            import math
            t = self.time_since_switch
            radius = 5.0
            px = math.cos(t * 2.0) * radius
            pz = math.sin(t * 2.0) * radius
            self.world.spawn_particles(px, 2.0, pz, 5, 0.8, 0.0, 1.0, 1.0, 3.0, 0.2)
            self.world.spawn_particles(-px, 2.0, -pz, 5, 0.0, 0.8, 1.0, 1.0, 3.0, 0.2)
            
        elif m == 3: # Explosion
            # Burst every second
            if int(self.time_since_switch * 2) > int((self.time_since_switch - dt) * 2):
                self.world.spawn_particles(0.0, 5.0, 0.0, 100, 1.0, 1.0, 0.5, 10.0, 2.0, 0.5)

if __name__ == "__main__":
    demo = ParticlesShowcase()
    demo.run()
