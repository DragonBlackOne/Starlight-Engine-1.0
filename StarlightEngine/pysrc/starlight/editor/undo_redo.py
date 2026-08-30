"""Editor Undo/Redo — Command pattern for reversible operations."""
from __future__ import annotations
from typing import Any, Callable
from dataclasses import dataclass


@dataclass
class Command:
    """A reversible operation."""
    description: str
    do: Callable[[], None]
    undo: Callable[[], None]


class UndoRedoManager:
    """Command-based undo/redo stack."""

    def __init__(self, max_history: int = 100) -> None:
        self.undo_stack: list[Command] = []
        self.redo_stack: list[Command] = []
        self.max_history = max_history

    def execute(self, command: Command) -> None:
        """Execute a command and push it to the undo stack."""
        command.do()
        self.undo_stack.append(command)
        if len(self.undo_stack) > self.max_history:
            self.undo_stack.pop(0)
        self.redo_stack.clear()

    def undo(self) -> str | None:
        """Undo the last command. Returns description or None."""
        if not self.undo_stack:
            return None
        cmd = self.undo_stack.pop()
        cmd.undo()
        self.redo_stack.append(cmd)
        return cmd.description

    def redo(self) -> str | None:
        """Redo the last undone command. Returns description or None."""
        if not self.redo_stack:
            return None
        cmd = self.redo_stack.pop()
        cmd.do()
        self.undo_stack.append(cmd)
        return cmd.description

    @property
    def can_undo(self) -> bool:
        return len(self.undo_stack) > 0

    @property
    def can_redo(self) -> bool:
        return len(self.redo_stack) > 0
