"""
Vegetation Spawner - Converte plantas geradas em entidades do Engine.

Faz a ponte entre TropicalFloraGenerator e o backend Starlight,
spawando entidades com modelos e materiais apropriados.
"""

from typing import Dict, List, Optional, TYPE_CHECKING
from loguru import logger
from dataclasses import dataclass

if TYPE_CHECKING:
    from starlight import App

from .tropical_flora import (
    TropicalFloraGenerator, 
    PlantInstance, 
    FloraLayer,
    FloraType
)


@dataclass
class SpawnedPlant:
    """Referência a uma planta spawnada."""
    entity_id: int
    instance: PlantInstance


class VegetationSpawner:
    """
    Spawna vegetação gerada proceduralmente no mundo.
    
    Usa modelos primitivos otimizados para performance:
    - Emergente/Dossel: Cylinder (tronco) + Sphere/Cone (copa)
    - Sub-bosque: Cylinder mais fino + Cone
    - Arbustos: Sphere achatada
    - Solo: Cube pequeno (billboard futuro)
    """
    
    def __init__(self, app: "App", seed: int = 42):
        self.app = app
        self.generator = TropicalFloraGenerator(seed=seed)
        self.spawned_plants: Dict[FloraLayer, List[SpawnedPlant]] = {}
        
        try:
            from starlight import backend
            self._backend = backend
        except ImportError:
            logger.error("Backend not available")
            self._backend = None
    
    def _spawn_tree(self, plant: PlantInstance) -> Optional[int]:
        """Spawna uma árvore (camadas altas)."""
        backend = self._backend
        if not backend:
            return None
            
        flora = plant.flora_type
        
        # Cor baseada na planta
        r, g, b = plant.color
        
        # Tronco
        trunk_height = plant.height * 0.6
        trunk_y = trunk_height / 2.0
        
        try:
            # Usar cube escalado como tronco
            trunk_id = backend.spawn_entity()
            backend.set_transform(
                trunk_id, 
                plant.x, trunk_y, plant.z
            )
            backend.set_scale(
                trunk_id,
                flora.trunk_radius * plant.scale,
                trunk_height,
                flora.trunk_radius * plant.scale
            )
            backend.set_rotation(trunk_id, 0.0, plant.rotation_y, 0.0)
            backend.set_color(trunk_id, 0.35, 0.25, 0.15, 1.0)  # Brown
            
            # Copa
            crown_y = trunk_height + (plant.height - trunk_height) / 2.0
            crown_id = backend.spawn_entity()
            backend.set_transform(crown_id, plant.x, crown_y, plant.z)
            backend.set_scale(
                crown_id,
                flora.crown_radius * plant.scale,
                (plant.height - trunk_height),
                flora.crown_radius * plant.scale
            )
            backend.set_color(crown_id, r, g, b, 1.0)
            
            return trunk_id
            
        except Exception as e:
            logger.debug(f"Failed to spawn tree: {e}")
            return None
    
    def _spawn_shrub(self, plant: PlantInstance) -> Optional[int]:
        """Spawna um arbusto (camadas baixas)."""
        backend = self._backend
        if not backend:
            return None
            
        flora = plant.flora_type
        r, g, b = plant.color
        
        try:
            entity_id = backend.spawn_entity()
            backend.set_transform(
                entity_id,
                plant.x, 
                plant.height / 2.0,
                plant.z
            )
            backend.set_scale(
                entity_id,
                flora.crown_radius * plant.scale,
                plant.height,
                flora.crown_radius * plant.scale
            )
            backend.set_rotation(entity_id, 0.0, plant.rotation_y, 0.0)
            backend.set_color(entity_id, r, g, b, 1.0)
            
            return entity_id
            
        except Exception as e:
            logger.debug(f"Failed to spawn shrub: {e}")
            return None
    
    def _spawn_ground_cover(self, plant: PlantInstance) -> Optional[int]:
        """Spawna cobertura de solo."""
        backend = self._backend
        if not backend:
            return None
            
        r, g, b = plant.color
        
        try:
            entity_id = backend.spawn_entity()
            backend.set_transform(
                entity_id,
                plant.x,
                plant.height / 2.0,
                plant.z
            )
            # Achatado
            backend.set_scale(
                entity_id,
                plant.flora_type.crown_radius * 2.0,
                plant.height,
                plant.flora_type.crown_radius * 2.0
            )
            backend.set_color(entity_id, r, g, b, 1.0)
            
            return entity_id
            
        except Exception as e:
            logger.debug(f"Failed to spawn ground cover: {e}")
            return None
    
    def spawn_plant(self, plant: PlantInstance) -> Optional[int]:
        """Spawna uma planta individual baseada na sua camada."""
        layer = plant.flora_type.layer
        
        if layer in (FloraLayer.EMERGENT, FloraLayer.CANOPY, FloraLayer.UNDERSTORY):
            return self._spawn_tree(plant)
        elif layer == FloraLayer.SHRUB:
            return self._spawn_shrub(plant)
        else:  # FLOOR
            return self._spawn_ground_cover(plant)
    
    def spawn_forest(self,
                     area_size: float = 200.0,
                     clearing_radius: float = 15.0,
                     layers: Optional[List[FloraLayer]] = None,
                     density_multiplier: float = 1.0) -> Dict[FloraLayer, List[SpawnedPlant]]:
        """
        Spawna floresta completa.
        
        Args:
            area_size: Tamanho da área em metros
            clearing_radius: Raio da clareira central
            layers: Camadas a spawnar (None = todas)
        
        Returns:
            Dict com plantas spawnadas por camada.
        """
        logger.info(f"Generating tropical forest: {area_size}m area, {clearing_radius}m clearing")
        
        # Gera vegetação
        forest_data = self.generator.generate_forest(
            area_size=area_size,
            clearing_radius=clearing_radius,
            density_multiplier=density_multiplier
        )
        
        stats = self.generator.get_statistics(forest_data)
        logger.info(f"Generated {stats['total_plants']} plants: {stats['by_layer']}")
        
        # Filtra camadas se especificado
        if layers:
            forest_data = {k: v for k, v in forest_data.items() if k in layers}
        
        # Spawna plantas
        self.spawned_plants = {}
        spawned_count = 0
        
        for layer, plants in forest_data.items():
            self.spawned_plants[layer] = []
            
            for plant in plants:
                entity_id = self.spawn_plant(plant)
                if entity_id is not None:
                    self.spawned_plants[layer].append(
                        SpawnedPlant(entity_id=entity_id, instance=plant)
                    )
                    spawned_count += 1
        
        logger.info(f"Spawned {spawned_count} plants successfully")
        return self.spawned_plants
    
    def get_plant_at(self, x: float, z: float, radius: float = 2.0) -> Optional[SpawnedPlant]:
        """Encontra planta na posição especificada."""
        for layer_plants in self.spawned_plants.values():
            for plant in layer_plants:
                dx = x - plant.instance.x
                dz = z - plant.instance.z
                if dx*dx + dz*dz < radius*radius:
                    return plant
        return None
