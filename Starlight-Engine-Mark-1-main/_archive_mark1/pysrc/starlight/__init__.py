# Backend must be imported first - other modules depend on it
import numpy as _np # Initializes numpy C-API for PyO3 Rust backend

try:
    from . import backend
    # Expose Components directly
    from .backend import (
        Transform, Rotation, Scale, Color, Parent, Camera,
        MeshName, Material, CharacterController, Health, Mana
    )
except ImportError:
    backend = None
    Transform = None
    Rotation = None
    Scale = None
    Color = None
    Parent = None
    Camera = None
    MeshName = None
    Material = None
    CharacterController = None
    Health = None
    Mana = None

# Now import modules that depend on backend
from .input import Input
from .keys import Keys
from . import math
from . import audio
from . import procgen
from .save import save_manager as save
from .app import App
# from .entity import Entity # Deprecated/Legacy
from .framework import Scene, Camera, Engine, Entity
from .network.serializer import NetworkSerializer
from .network.manager import NetworkManager

__all__ = [
    'App', 'Entity', 'Keys', 'Input', 'math', 'backend', 'audio', 'procgen', 'save', 'NetworkSerializer', 'NetworkManager',
    'Transform', 'Rotation', 'Scale', 'Color', 'Parent', 'Camera',
    'MeshName', 'Material', 'CharacterController', 'Health', 'Mana',
    'Scene', 'Engine'
]
