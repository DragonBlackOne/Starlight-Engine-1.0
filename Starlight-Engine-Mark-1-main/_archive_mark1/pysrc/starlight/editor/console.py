"""Editor Console — Log display and Python REPL."""
from __future__ import annotations
from typing import Any
import time


class Console:
    """Log buffer with severity filtering."""

    def __init__(self) -> None:
        self.logs: list[dict[str, str]] = []
        self.max_logs: int = 500

    def log(self, text: str, level: str = "INFO") -> None:
        """Add a log entry."""
        ts = time.strftime("%H:%M:%S")
        entry = {"text": f"[{ts}] [{level}] {text}", "level": level, "raw": text}
        self.logs.append(entry)
        if len(self.logs) > self.max_logs:
            self.logs = self.logs[-self.max_logs:]

    def clear(self) -> None:
        self.logs.clear()

    def get_filtered(self, level: str = "ALL") -> list[dict[str, str]]:
        if level == "ALL":
            return self.logs
        return [l for l in self.logs if l["level"] == level]
