"""Finite State Machine (FSM).

Usage:
    fsm = StateMachine()
    fsm.add_state(State("Idle", on_update=lambda dt: print("Idling...")))
    fsm.set_state("Idle")
    fsm.update(dt)
"""
from __future__ import annotations
from dataclasses import dataclass
from typing import Callable, Any

@dataclass
class State:
    name: str
    on_enter: Callable[[], None] | None = None
    on_exit: Callable[[], None] | None = None
    on_update: Callable[[float], None] | None = None

class StateMachine:
    def __init__(self) -> None:
        self.states: dict[str, State] = {}
        self.current_state: State | None = None

    def add_state(self, state: State) -> None:
        self.states[state.name] = state

    def set_state(self, name: str) -> None:
        if self.current_state and self.current_state.on_exit:
            self.current_state.on_exit()
        
        self.current_state = self.states.get(name)
        
        if self.current_state and self.current_state.on_enter:
            self.current_state.on_enter()

    def update(self, dt: float) -> None:
        if self.current_state and self.current_state.on_update:
            self.current_state.on_update(dt)
