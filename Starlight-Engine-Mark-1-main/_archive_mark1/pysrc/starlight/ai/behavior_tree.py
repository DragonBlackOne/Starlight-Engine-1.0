"""Behavior Tree System.

Usage:
    root = Sequence([
        Condition(lambda: player_visible),
        Action(lambda: attack_player())
    ])
    root.tick()
"""
from __future__ import annotations
from abc import ABC, abstractmethod
from enum import Enum, auto
from typing import Callable

class Status(Enum):
    SUCCESS = auto()
    FAILURE = auto()
    RUNNING = auto()

class Node(ABC):
    @abstractmethod
    def tick(self) -> Status: ...

class Leaf(Node):
    pass

class Condition(Leaf):
    def __init__(self, func: Callable[[], bool]) -> None:
        self.func = func
    
    def tick(self) -> Status:
        return Status.SUCCESS if self.func() else Status.FAILURE

class Action(Leaf):
    def __init__(self, func: Callable[[], Status]) -> None:
        self.func = func
    
    def tick(self) -> Status:
        return self.func()

class Composite(Node):
    def __init__(self, children: list[Node]) -> None:
        self.children = children

class Sequence(Composite):
    def tick(self) -> Status:
        for child in self.children:
            status = child.tick()
            if status != Status.SUCCESS:
                return status
        return Status.SUCCESS

class Selector(Composite):
    def tick(self) -> Status:
        for child in self.children:
            status = child.tick()
            if status != Status.FAILURE:
                return status
        return Status.FAILURE

class Decorator(Node):
    def __init__(self, child: Node) -> None:
        self.child = child

class Inverter(Decorator):
    def tick(self) -> Status:
        status = self.child.tick()
        if status == Status.SUCCESS: return Status.FAILURE
        if status == Status.FAILURE: return Status.SUCCESS
        return Status.RUNNING

class BehaviorTree:
    def __init__(self, root: Node) -> None:
        self.root = root
    
    def tick(self) -> Status:
        return self.root.tick()
