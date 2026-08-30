"""
Starlight ProcGen - Módulo de Geração Procedural

Contém sistemas para geração procedural de:
- Vegetação tropical (tropical_flora, vegetation_spawner)
- Fauna animada (fauna_system)
- Ruído (noise)
"""

from .noise import NoiseGenerator
from .tropical_flora import (
    TropicalFloraGenerator,
    FloraLayer,
    FloraType,
    PlantInstance,
    TROPICAL_FLORA_LIBRARY
)
from .vegetation_spawner import VegetationSpawner, SpawnedPlant
from .fauna_system import FaunaSystem, FaunaType, FaunaConfig

__all__ = [
    # Noise
    "NoiseGenerator",
    # Flora
    "TropicalFloraGenerator",
    "FloraLayer",
    "FloraType", 
    "PlantInstance",
    "TROPICAL_FLORA_LIBRARY",
    # Spawner
    "VegetationSpawner",
    "SpawnedPlant",
    # Fauna
    "FaunaSystem",
    "FaunaType",
    "FaunaConfig",
]
