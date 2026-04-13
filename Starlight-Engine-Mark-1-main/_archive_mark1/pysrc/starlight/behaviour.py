"""Base class for Python scripts attached to Entities."""
from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from .framework import Entity


class StarlightBehaviour:
    """Base script for custom game logic on Entities.
    
    Like Unity's MonoBehaviour, this class can be attached to an Entity.
    The engine will automatically call its lifecycle methods.
    """

    def __init__(self) -> None:
        self.entity: 'Entity' = None  # Injected when attached

    def on_start(self) -> None:
        """Called once when the entity is instantiated or the scene starts."""
        pass

    def update(self, dt: float) -> None:
        """Called every frame.
        
        Args:
            dt: Delta time in seconds.
        """
        pass

    def on_destroy(self) -> None:
        """Called when the entity is destroyed."""
        pass
