"""Editor Profiler UI — FPS tracking and performance stats."""
from __future__ import annotations
import time


class ProfilerUI:
    """Performance monitoring with FPS history."""

    def __init__(self) -> None:
        self.fps: float = 0.0
        self.frame_time_ms: float = 0.0
        self.fps_history: list[float] = []
        self.max_history: int = 120
        self._frame_count: int = 0
        self._last_time: float = time.time()

    def tick(self) -> None:
        """Call once per frame to update metrics."""
        self._frame_count += 1
        now = time.time()
        elapsed = now - self._last_time
        if elapsed >= 1.0:
            self.fps = self._frame_count / elapsed
            self.frame_time_ms = 1000.0 / max(self.fps, 0.001)
            self._frame_count = 0
            self._last_time = now
            self.fps_history.append(self.fps)
            if len(self.fps_history) > self.max_history:
                self.fps_history = self.fps_history[-self.max_history:]
