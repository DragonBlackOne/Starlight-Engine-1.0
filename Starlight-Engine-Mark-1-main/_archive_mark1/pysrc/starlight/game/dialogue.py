"""Branching Dialogue System.

Usage:
    dm = DialogueManager()
    greeting = DialogueNode("greeting", "Guard", "Halt! Who goes there?")
    greeting.add_choice("I'm a friend", "friendly")
    greeting.add_choice("None of your business", "hostile")
    dm.add_node(greeting)
    dm.add_node(DialogueNode("friendly", "Guard", "Welcome, friend. Pass through."))
    dm.add_node(DialogueNode("hostile", "Guard", "Then you shall not pass!",
                             on_enter=lambda node: print("[Combat starts]")))
    dm.start("greeting")
    dm.choose(0)  # Pick first choice
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Callable, Any


@dataclass
class DialogueChoice:
    """A player choice in dialogue."""
    text: str
    next_node: str
    condition: Callable[[], bool] | None = None  # Only show if True
    on_select: Callable[[], None] | None = None


@dataclass
class DialogueNode:
    """A single dialogue node (speaker line + choices)."""
    node_id: str
    speaker: str
    text: str
    choices: list[DialogueChoice] = field(default_factory=list)
    on_enter: Callable[["DialogueNode"], None] | None = None
    on_exit: Callable[["DialogueNode"], None] | None = None
    auto_next: str | None = None  # Auto-advance if no choices
    portrait: str = ""
    mood: str = "neutral"

    def add_choice(self, text: str, next_node: str,
                   condition: Callable[[], bool] | None = None,
                   on_select: Callable[[], None] | None = None) -> None:
        self.choices.append(DialogueChoice(text, next_node, condition, on_select))

    @property
    def available_choices(self) -> list[DialogueChoice]:
        return [c for c in self.choices if c.condition is None or c.condition()]


class DialogueManager:
    """Manages dialogue trees.

    Example:
        dm = DialogueManager()
        dm.add_node(DialogueNode("start", "NPC", "Hello!"))
        dm.start("start")
        print(dm.current_text)  # "Hello!"
    """

    def __init__(self) -> None:
        self._nodes: dict[str, DialogueNode] = {}
        self._current: DialogueNode | None = None
        self._history: list[str] = []
        self._on_dialogue_end: list[Callable[[], None]] = []
        self.active: bool = False

    def add_node(self, node: DialogueNode) -> None:
        self._nodes[node.node_id] = node

    def start(self, node_id: str) -> bool:
        node = self._nodes.get(node_id)
        if not node:
            return False
        self._current = node
        self._history = [node_id]
        self.active = True
        if node.on_enter:
            node.on_enter(node)
        print(f"[Dialogue] {node.speaker}: {node.text}")
        return True

    def choose(self, choice_index: int) -> bool:
        """Select a dialogue choice by index."""
        if not self._current:
            return False
        choices = self._current.available_choices
        if not (0 <= choice_index < len(choices)):
            return False

        choice = choices[choice_index]
        if choice.on_select:
            choice.on_select()

        if self._current.on_exit:
            self._current.on_exit(self._current)

        return self._goto(choice.next_node)

    def advance(self) -> bool:
        """Advance to auto_next node (for nodes without choices)."""
        if not self._current or not self._current.auto_next:
            self.end()
            return False
        if self._current.on_exit:
            self._current.on_exit(self._current)
        return self._goto(self._current.auto_next)

    def end(self) -> None:
        if self._current and self._current.on_exit:
            self._current.on_exit(self._current)
        self._current = None
        self.active = False
        for cb in self._on_dialogue_end:
            cb()
        print("[Dialogue] Ended")

    def on_end(self, callback: Callable[[], None]) -> None:
        self._on_dialogue_end.append(callback)

    def _goto(self, node_id: str) -> bool:
        node = self._nodes.get(node_id)
        if not node:
            self.end()
            return False
        self._current = node
        self._history.append(node_id)
        if node.on_enter:
            node.on_enter(node)
        print(f"[Dialogue] {node.speaker}: {node.text}")
        return True

    @property
    def current_text(self) -> str:
        return self._current.text if self._current else ""

    @property
    def current_speaker(self) -> str:
        return self._current.speaker if self._current else ""

    @property
    def current_choices(self) -> list[str]:
        if not self._current:
            return []
        return [c.text for c in self._current.available_choices]

    @property
    def has_choices(self) -> bool:
        return bool(self._current and self._current.available_choices)
