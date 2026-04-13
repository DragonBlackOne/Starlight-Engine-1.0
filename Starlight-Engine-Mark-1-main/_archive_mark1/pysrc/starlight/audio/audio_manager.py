"""Central Audio System Facade.

Usage:
    audio = AudioManager()
    audio.update(dt)
"""
from __future__ import annotations
from .audio_3d import Audio3DManager
from .music_manager import MusicManager
from .sfx_manager import SFXManager

class AudioManager:
    """Main entry point for audio systems."""
    def __init__(self) -> None:
        self.sfx = SFXManager()
        self.music = MusicManager()
        self.spatial = Audio3DManager()
        
    def update(self, dt: float) -> None:
        self.music.update(dt)
        self.spatial.update(dt)
        # sfx usually updates itself or via spatial
