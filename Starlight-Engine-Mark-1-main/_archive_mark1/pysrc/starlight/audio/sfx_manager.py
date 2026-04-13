"""SFX Manager (Pooling, One-shots).

Usage:
    sfx.play("gunshot", pos=(10,0,0), variance=0.1)
"""
from __future__ import annotations
from dataclasses import dataclass
import random

@dataclass
class SFXClip:
    name: str
    file_paths: list[str] # Variations
    base_volume: float = 1.0
    base_pitch: float = 1.0

class SFXManager:
    """Handles sound effects logic."""
    def __init__(self) -> None:
        self.clips: dict[str, SFXClip] = {}
    
    def register(self, name: str, paths: list[str]) -> None:
        self.clips[name] = SFXClip(name, paths)

    def play(self, name: str, position: tuple[float,float,float] | None = None, 
             volume: float = 1.0, pitch: float = 1.0, 
             variance: float = 0.0) -> int:
        
        if name not in self.clips:
            return -1
            
        clip = self.clips[name]
        
        # Select variation
        path = random.choice(clip.file_paths)
        
        # Apply variance
        final_pitch = pitch * clip.base_pitch
        if variance > 0:
            final_pitch += random.uniform(-variance, variance)
            
        # Example backend call
        # id = backend.play_sound(path, pos=position, vol=volume, pitch=final_pitch)
        print(f"[SFX] Playing {path} at {position} (pitch={final_pitch:.2f})")
        return 0 # ID
