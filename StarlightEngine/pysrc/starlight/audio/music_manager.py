"""Music Manager (Transitions, Layers).

Usage:
    music = MusicManager()
    music.play_track("exploration", fade_time=2.0)
    music.set_layer("combat", 1.0)
"""
from __future__ import annotations
from dataclasses import dataclass

@dataclass
class MusicTrack:
    name: str
    file_path: str
    volume: float = 1.0
    loop: bool = True

class MusicManager:
    """Handles background music with crossfading."""
    def __init__(self) -> None:
        self.tracks: dict[str, MusicTrack] = {}
        self.current_track: str | None = None
        self.target_volume: float = 1.0
        self.fade_timer: float = 0.0
    
    def register_track(self, name: str, path: str) -> None:
        self.tracks[name] = MusicTrack(name, path)

    def play_track(self, name: str, fade_time: float = 2.0) -> None:
        if name not in self.tracks:
            print(f"[Music] Track not found: {name}")
            return
            
        if self.current_track == name:
            return

        print(f"[Music] Playing {name} (fade: {fade_time}s)")
        self.current_track = name
        # Implementation would handle backend streaming + volume lerp
    
    def set_layer_volume(self, layer_name: str, volume: float) -> None:
        # For dynamic music (stems)
        pass
    
    def update(self, dt: float) -> None:
        pass
