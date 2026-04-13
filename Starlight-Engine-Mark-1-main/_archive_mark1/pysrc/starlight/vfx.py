
import random
import math
from starlight import backend

class ScreenShake:
    def __init__(self):
        self.trauma = 0.0
        self.max_offset = 0.5
        self.max_roll = 5.0 # degrees
        self.decay = 2.0 # Trauma reduction per second
        self.time = 0.0

    def add_trauma(self, amount: float):
        """Add trauma (0 to 1). Trauma causes shake."""
        self.trauma = min(self.trauma + amount, 1.0)

    def update(self, dt: float) -> tuple[float, float, float]:
        """
        Returns (offset_x, offset_y, roll_degrees) to apply to camera.
        """
        if self.trauma > 0:
            self.trauma = max(self.trauma - self.decay * dt, 0.0)
            
        shake = self.trauma * self.trauma # Quadratic falloff feels better
        
        # Simple perlin-like noise using sine waves with different frequencies
        self.time += dt * 20.0
        
        offset_x = self.max_offset * shake * (math.sin(self.time) + math.sin(self.time * 0.5) * 0.5)
        offset_y = self.max_offset * shake * (math.cos(self.time * 1.1) + math.cos(self.time * 0.6) * 0.5)
        roll = self.max_roll * shake * (math.sin(self.time * 0.8))
        
        return offset_x, offset_y, roll

class GlitchEffect:
    def __init__(self):
        self.active = False
        self.timer = 0.0
        self.duration = 0.0
        
        # Base params
        self.base_exposure = 1.0
        self.base_gamma = 2.2
        self.base_bloom = 0.5
        self.base_threshold = 0.8
        
    def trigger(self, duration: float = 0.2):
        self.active = True
        self.timer = 0.0
        self.duration = duration
        
    def update(self, dt: float):
        if not self.active:
            return

        self.timer += dt
        if self.timer >= self.duration:
            self.active = False
            # Reset
            backend.set_post_process_params(
                self.base_exposure,
                self.base_gamma,
                self.base_bloom,
                self.base_threshold
            )
            return

        # Randomize for glitch feel
        exposure = self.base_exposure + random.uniform(-0.5, 1.0)
        bloom = self.base_bloom + random.uniform(0.0, 2.0)
        gamma = self.base_gamma + random.uniform(-0.2, 0.2)
        
        backend.set_post_process_params(exposure, gamma, bloom, self.base_threshold)
