"""Editor Play Mode — Edit/Play/Pause/Stop state machine."""
from __future__ import annotations
from typing import Any


class PlayMode:
    """State machine for editor play controls."""

    EDIT = "Edit"
    PLAY = "Play"
    PAUSE = "Pause"

    def __init__(self) -> None:
        self.state: str = self.EDIT
        self._snapshot: Any = None

    def play(self) -> None:
        if self.state == self.EDIT:
            # Save snapshot before playing
            self._snapshot = None  # Would serialize scene here
        self.state = self.PLAY

    def pause(self) -> None:
        if self.state == self.PLAY:
            self.state = self.PAUSE

    def stop(self) -> None:
        self.state = self.EDIT
        # Restore snapshot
        if self._snapshot:
            pass  # Would deserialize scene here
        self._snapshot = None

    @property
    def is_playing(self) -> bool:
        return self.state == self.PLAY

    @property
    def is_editing(self) -> bool:
        return self.state == self.EDIT
