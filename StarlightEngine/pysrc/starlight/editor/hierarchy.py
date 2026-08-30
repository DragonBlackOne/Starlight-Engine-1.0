"""Editor Scene Hierarchy — Entity tree view."""
from __future__ import annotations
from typing import Any, Callable


class SceneHierarchy:
    """Tree view of scene entities with selection tracking."""

    def __init__(self) -> None:
        self.selected_id: int | None = None
        self.on_select: Callable | None = None

    def select(self, entity_id: int) -> None:
        self.selected_id = entity_id
        if self.on_select:
            self.on_select(entity_id)

    def deselect(self) -> None:
        self.selected_id = None
