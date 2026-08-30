"""Tutorial Step Manager.

Usage:
    tutorial = TutorialSystem()
    tutorial.add_step(TutorialStep("move", "Use WASD to move around",
                                   condition=lambda: player.has_moved))
    tutorial.add_step(TutorialStep("jump", "Press SPACE to jump",
                                   condition=lambda: player.has_jumped))
    tutorial.start()
    # In game loop:
    tutorial.update()
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Callable


@dataclass
class TutorialStep:
    """A single tutorial step."""
    step_id: str
    message: str
    condition: Callable[[], bool] | None = None  # Auto-complete when True
    on_start: Callable[[], None] | None = None
    on_complete: Callable[[], None] | None = None
    highlight_element: str = ""  # UI element to highlight
    timeout: float = -1.0  # Auto-skip after N seconds (-1 = never)
    _elapsed: float = 0.0
    _completed: bool = False


class TutorialSystem:
    """Sequential tutorial manager.

    Example:
        tut = TutorialSystem()
        tut.add_step(TutorialStep("look", "Move your mouse to look around"))
        tut.start()
        tut.update(dt)
        if tut.active:
            show_tooltip(tut.current_message)
    """

    def __init__(self) -> None:
        self._steps: list[TutorialStep] = []
        self._current_index: int = -1
        self.active: bool = False
        self._on_complete: list[Callable[[], None]] = []

    def add_step(self, step: TutorialStep) -> None:
        self._steps.append(step)

    def start(self) -> None:
        if not self._steps:
            return
        self._current_index = 0
        self.active = True
        step = self._steps[0]
        if step.on_start:
            step.on_start()
        print(f"[Tutorial] Step 1/{len(self._steps)}: {step.message}")

    def skip_step(self) -> None:
        """Skip current step."""
        self._advance()

    def skip_all(self) -> None:
        """Skip entire tutorial."""
        self._current_index = len(self._steps)
        self.active = False
        print("[Tutorial] Skipped")

    def update(self, dt: float = 0.016) -> None:
        """Check conditions and advance. Call every frame."""
        if not self.active or self._current_index >= len(self._steps):
            return

        step = self._steps[self._current_index]

        # Check timeout
        if step.timeout > 0:
            step._elapsed += dt
            if step._elapsed >= step.timeout:
                self._advance()
                return

        # Check completion condition
        if step.condition and step.condition():
            self._advance()

    def _advance(self) -> None:
        if self._current_index < len(self._steps):
            step = self._steps[self._current_index]
            step._completed = True
            if step.on_complete:
                step.on_complete()

        self._current_index += 1
        if self._current_index >= len(self._steps):
            self.active = False
            print("[Tutorial] Completed!")
            for cb in self._on_complete:
                cb()
        else:
            step = self._steps[self._current_index]
            if step.on_start:
                step.on_start()
            print(f"[Tutorial] Step {self._current_index + 1}/{len(self._steps)}: {step.message}")

    def on_complete(self, callback: Callable[[], None]) -> None:
        self._on_complete.append(callback)

    @property
    def current_message(self) -> str:
        if 0 <= self._current_index < len(self._steps):
            return self._steps[self._current_index].message
        return ""

    @property
    def current_step_id(self) -> str:
        if 0 <= self._current_index < len(self._steps):
            return self._steps[self._current_index].step_id
        return ""

    @property
    def progress_percent(self) -> float:
        if not self._steps:
            return 100.0
        return (self._current_index / len(self._steps)) * 100.0

    @property
    def step_number(self) -> tuple[int, int]:
        return (self._current_index + 1, len(self._steps))
