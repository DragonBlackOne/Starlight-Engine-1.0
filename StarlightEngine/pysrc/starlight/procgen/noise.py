from .. import backend

class NoiseGenerator:
    """
    Skill: noise-algorithms
    """
    @staticmethod
    def get_noise_2d(x: float, y: float, seed: int = 0) -> float:
        # Wrapper around backend noise if available, else simplex fallback
        if hasattr(backend, 'noise_get'):
            return backend.noise_get(x, y, seed)
        # Fallback (simple sin wave for demo)
        import math
        return math.sin(x) * math.cos(y) 
