"""Editor Inspector — Property editor for selected entities."""
from __future__ import annotations
from typing import Any


class Inspector:
    """Reflection-based property editor."""

    def __init__(self) -> None:
        self.target: dict | None = None

    def inspect(self, target: dict | None) -> None:
        self.target = target

    def get_property(self, key: str, default: Any = None) -> Any:
        if self.target:
            return self.target.get(key, default)
        return default

    def set_property(self, key: str, value: Any) -> None:
        if self.target:
            self.target[key] = value
