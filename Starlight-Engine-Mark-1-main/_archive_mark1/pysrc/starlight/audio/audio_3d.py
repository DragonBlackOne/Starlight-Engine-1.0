"""3D Audio Processing (HRTF, Occlusion, Doppler).

Usage:
    audio3d = Audio3DManager()
    source = audio3d.create_source(pos=(10,0,0))
    audio3d.update_listener(pos=(0,0,0), forward=(0,0,1), up=(0,1,0))
"""
from __future__ import annotations
from dataclasses import dataclass

@dataclass
class AudioListener:
    position: tuple[float,float,float] = (0,0,0)
    forward: tuple[float,float,float] = (0,0,1)
    up: tuple[float,float,float] = (0,1,0)
    velocity: tuple[float,float,float] = (0,0,0)

@dataclass
class AudioSource3D:
    id: int
    position: tuple[float,float,float]
    velocity: tuple[float,float,float] = (0,0,0)
    min_dist: float = 1.0
    max_dist: float = 50.0
    volume: float = 1.0
    pitch: float = 1.0
    loop: bool = False
    
    # Internal state
    _playing: bool = False

class Audio3DManager:
    """Manages spatial audio calculations."""
    def __init__(self) -> None:
        self.listener = AudioListener()
        self.sources: dict[int, AudioSource3D] = {}
        self._next_id = 0

    def create_source(self, position: tuple[float,float,float]) -> AudioSource3D:
        sid = self._next_id
        self._next_id += 1
        src = AudioSource3D(sid, position)
        self.sources[sid] = src
        return src

    def update_listener(self, position: tuple[float,float,float], 
                       forward: tuple[float,float,float], 
                       up: tuple[float,float,float]) -> None:
        self.listener.position = position
        self.listener.forward = forward
        self.listener.up = up
        # Velocity calc would need dt + prev_pos

    def update(self, dt: float) -> None:
        # Calculate attenuation, doppler, panning for each source
        # This would interface with backend (OpenAL/FMOD/Rodio)
        pass
