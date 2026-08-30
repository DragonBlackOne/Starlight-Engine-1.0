"""Auto-generated Settings Menu.

Usage:
    menu = SettingsMenu()
    menu.add(Setting("Master Volume", "audio.master", kind="slider", min_val=0, max_val=100, default=80))
    menu.add(Setting("Fullscreen", "display.fullscreen", kind="toggle", default=False))
    menu.add(Setting("Resolution", "display.resolution", kind="dropdown",
                     options=["1920x1080", "1280x720", "3840x2160"], default="1920x1080"))
    menu.add(Setting("Mouse Sensitivity", "input.sensitivity", kind="slider",
                     min_val=0.1, max_val=3.0, default=1.0, step=0.1))
    menu.apply_all()  # Apply current values
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any, Callable
import json
import os


@dataclass
class Setting:
    """A single settings entry."""
    label: str
    key: str
    kind: str = "slider"  # slider, toggle, dropdown, keybind
    default: Any = 0
    value: Any = None
    min_val: float = 0.0
    max_val: float = 100.0
    step: float = 1.0
    options: list[str] = field(default_factory=list)
    category: str = "General"
    on_change: Callable[[Any], None] | None = None
    tooltip: str = ""

    def __post_init__(self):
        if self.value is None:
            self.value = self.default

    def set_value(self, val: Any) -> None:
        old = self.value
        if self.kind == "slider":
            val = max(self.min_val, min(self.max_val, val))
        elif self.kind == "dropdown" and val not in self.options:
            return
        self.value = val
        if self.on_change and old != val:
            self.on_change(val)

    def reset(self) -> None:
        self.set_value(self.default)


class SettingsMenu:
    """Settings menu with categories, persistence, and auto-apply.

    Example:
        menu = SettingsMenu(config_path="saves/settings.json")
        menu.add(Setting("Shadow Quality", "gfx.shadows", kind="dropdown",
                         options=["Off", "Low", "Medium", "High"], default="Medium"))
        menu.load()
        menu.apply_all()
    """

    def __init__(self, config_path: str = "saves/settings.json") -> None:
        self._settings: dict[str, Setting] = {}
        self._categories: dict[str, list[str]] = {}
        self.config_path = config_path

    def add(self, setting: Setting) -> None:
        self._settings[setting.key] = setting
        cat = setting.category
        if cat not in self._categories:
            self._categories[cat] = []
        if setting.key not in self._categories[cat]:
            self._categories[cat].append(setting.key)

    def get(self, key: str) -> Any:
        s = self._settings.get(key)
        return s.value if s else None

    def set(self, key: str, value: Any) -> None:
        if key in self._settings:
            self._settings[key].set_value(value)

    def get_setting(self, key: str) -> Setting | None:
        return self._settings.get(key)

    @property
    def categories(self) -> list[str]:
        return list(self._categories.keys())

    def get_by_category(self, category: str) -> list[Setting]:
        keys = self._categories.get(category, [])
        return [self._settings[k] for k in keys if k in self._settings]

    def reset_all(self) -> None:
        for s in self._settings.values():
            s.reset()

    def apply_all(self) -> None:
        for s in self._settings.values():
            if s.on_change:
                s.on_change(s.value)

    def save(self) -> None:
        os.makedirs(os.path.dirname(self.config_path) or ".", exist_ok=True)
        data = {key: s.value for key, s in self._settings.items()}
        with open(self.config_path, "w") as f:
            json.dump(data, f, indent=2)
        print(f"[Settings] Saved to {self.config_path}")

    def load(self) -> bool:
        if not os.path.exists(self.config_path):
            return False
        try:
            with open(self.config_path) as f:
                data = json.load(f)
            for key, val in data.items():
                if key in self._settings:
                    self._settings[key].value = val
            print(f"[Settings] Loaded from {self.config_path}")
            return True
        except Exception as e:
            print(f"[Settings] Error loading: {e}")
            return False

    def to_dict(self) -> dict[str, Any]:
        return {key: s.value for key, s in self._settings.items()}
