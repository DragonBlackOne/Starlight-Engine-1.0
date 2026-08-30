"""Input Remapping System.

Usage:
    mapper = InputMapper()
    mapper.bind_action("Jump", InputBinding.KEY_SPACE)
    if mapper.is_pressed("Jump"): ...
"""
from __future__ import annotations
from enum import IntEnum
from dataclasses import dataclass

class InputBinding(IntEnum):
    KEY_UNKNOWN = 0
    KEY_SPACE = 32
    KEY_W = 87
    KEY_A = 65
    KEY_S = 83
    KEY_D = 68
    MOUSE_LEFT = 1001
    MOUSE_RIGHT = 1002
    GAMEPAD_A = 2001

@dataclass
class ActionMapping:
    name: str
    primary: InputBinding
    secondary: InputBinding | None = None

class InputMapper:
    """Translation layer between raw input and game actions."""
    def __init__(self) -> None:
        self.actions: dict[str, ActionMapping] = {}
        # Backend input state reference would go here
        
    def bind_action(self, name: str, primary: InputBinding, secondary: InputBinding | None = None) -> None:
        self.actions[name] = ActionMapping(name, primary, secondary)

    def get_binding(self, name: str) -> ActionMapping | None:
        return self.actions.get(name)

    def is_pressed(self, action_name: str, backend_key_func) -> bool:
        """Check if action is triggered given a backend query function."""
        mapping = self.actions.get(action_name)
        if not mapping: return False
        
        if backend_key_func(mapping.primary): return True
        if mapping.secondary and backend_key_func(mapping.secondary): return True
        return False
