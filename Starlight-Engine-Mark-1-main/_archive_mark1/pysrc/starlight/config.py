from dataclasses import dataclass, field
from typing import Dict, Any
import json
import os
import tomllib

@dataclass
class GameConfig:
    """
    Configuration for the Game Application.
    Skill: config-management
    """
    title: str = "Starlight Game"
    width: int = 1280
    height: int = 720
    vsync: bool = True
    fullscreen: bool = False
    
    # Advanced graphical settings
    shadow_resolution: int = 2048
    fog_density: float = 0.01
    sun_intensity: float = 1.0

    # Generic settings store
    settings: Dict[str, Any] = field(default_factory=dict)

    def load_from_file(self, path: str):
        """
        Loads configuration from a JSON or TOML file.
        Updates existing fields and stores unknown keys in 'settings'.
        """
        if not os.path.exists(path):
            raise FileNotFoundError(f"Configuration file not found: {path}")

        _, ext = os.path.splitext(path)
        ext = ext.lower()

        data = {}

        if ext == ".json":
            with open(path, "r", encoding="utf-8") as f:
                data = json.load(f)
        elif ext == ".toml":
            with open(path, "rb") as f:
                data = tomllib.load(f)
        else:
            raise ValueError(f"Unsupported configuration format: {ext}")

        # Update fields
        for key, value in data.items():
            if hasattr(self, key):
                setattr(self, key, value)
            else:
                self.settings[key] = value
