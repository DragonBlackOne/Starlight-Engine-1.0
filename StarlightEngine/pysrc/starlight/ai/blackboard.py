"""Shared Blackboard for Squad AI.

Usage:
    bb = SharedBlackboard()
    bb.set("target_pos", (10, 0, 10))
    # In agent update:
    target = bb.get("target_pos")
"""
from __future__ import annotations
from typing import Any, Callable


class Blackboard:
    """A data store for an individual AI agent."""
    def __init__(self) -> None:
        self._data: dict[str, Any] = {}

    def set(self, key: str, value: Any) -> None:
        self._data[key] = value

    def get(self, key: str, default: Any = None) -> Any:
        return self._data.get(key, default)

    def has(self, key: str) -> bool:
        return key in self._data

    def clear(self, key: str) -> None:
        self._data.pop(key, None)


class SharedBlackboard(Blackboard):
    """A blackboard shared among members of a squad.

    Supports 'expiry' for data points (e.g. last known enemy position).
    """
    def __init__(self) -> None:
        super().__init__()
        self._metadata: dict[str, float] = {}  # key -> timestamp

    def set_timed(self, key: str, value: Any, timestamp: float) -> None:
        self.set(key, value)
        self._metadata[key] = timestamp

    def get_timestamp(self, key: str) -> float:
        return self._metadata.get(key, 0.0)

    def is_fresh(self, key: str, current_time: float, max_age: float) -> bool:
        """Check if data is recent enough."""
        ts = self._metadata.get(key, 0.0)
        return (current_time - ts) <= max_age
