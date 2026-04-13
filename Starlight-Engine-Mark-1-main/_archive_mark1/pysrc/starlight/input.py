from typing import Optional, Tuple
from . import backend

class Input:
    """
    Handles user input (Keyboard, Mouse, Gamepad).
    """
    def __init__(self):
        self.dt = 0.0
        self.frame_count = 0


    def is_key_down(self, key: str) -> bool:
        """Deprecated: Use actions instead."""
        return backend.is_key_down(key)

    def is_key_just_pressed(self, key: str) -> bool:
        """Deprecated: Use actions instead."""
        return backend.is_key_just_pressed(key)

    def bind_action(self, name: str, key: str) -> None:
        """Binds a key (e.g., 'w', 'space', 'enter') to an action name."""
        backend.bind_action(name, key)

    def is_action_down(self, name: str) -> bool:
        """Checks if an action is currently active."""
        return backend.is_action_down(name)

    def bind_axis(
        self,
        name: str,
        key: Optional[str] = None,
        gamepad_button: Optional[str] = None,
        gamepad_axis: Optional[str] = None,
        scale: float = 1.0,
        deadzone: float = 0.1
    ) -> None:
        if key:
            backend.bind_axis(name, "Key", key, scale, deadzone)
        elif gamepad_button:
            backend.bind_axis(name, "GamepadButton", gamepad_button, scale, deadzone)
        elif gamepad_axis:
            backend.bind_axis(name, "GamepadAxis", gamepad_axis, scale, deadzone)
        else:
            print("[Input] Warning: bind_axis called without source")

    def get_axis(self, name: str) -> float:
        """Returns the current value of an axis."""
        return backend.get_axis(name)

    def is_gamepad_connected(self, id: int) -> bool:
        """Checks if a gamepad with the given ID is connected."""
        return backend.is_gamepad_connected(id)

    def get_mouse_delta(self) -> Tuple[float, float]:
        """Returns (dx, dy) of mouse movement."""
        return backend.get_mouse_delta()

    def set_cursor_grab(self, grab: bool) -> None:
        """Grabs or releases the mouse cursor."""
        backend.set_cursor_grab(grab)

    def set_cursor_visible(self, visible: bool) -> None:
        """Shows or hides the mouse cursor."""
        backend.set_cursor_visible(visible)
