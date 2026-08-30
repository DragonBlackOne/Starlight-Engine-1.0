"""
Tropical Flora - Sistema de Geração Procedural de Vegetação Tropical

Este módulo implementa a geração procedural de vegetação para
simulação de floresta tropical com 5 camadas:
1. Emergente (45-65m) - Árvores gigantes
2. Dossel (25-40m) - Canopy denso
3. Sub-bosque (10-20m) - Árvores médias
4. Arbustos (1-5m) - Samambaias, arbustos
5. Solo (0-1m) - Plantas rasteiras, litter
"""

import math
import random
from dataclasses import dataclass, field
from typing import List, Tuple, Optional, Union, Dict
from enum import Enum


class FloraLayer(Enum):
    """Camadas da floresta tropical."""
    EMERGENT = "emergent"      # 45-65m
    CANOPY = "canopy"          # 25-40m
    UNDERSTORY = "understory"  # 10-20m
    SHRUB = "shrub"            # 1-5m
    FLOOR = "floor"            # 0-1m


@dataclass
class FloraType:
    """Definição de um tipo de planta."""
    name: str
    layer: FloraLayer
    height_min: float
    height_max: float
    trunk_radius: float
    crown_radius: float
    density: float  # Plantas por 100m²
    color_base: Tuple[float, float, float] = (0.2, 0.5, 0.2)
    color_variation: float = 0.1
    has_leaves: bool = True
    has_trunk: bool = True


# Biblioteca de Flora Tropical
TROPICAL_FLORA_LIBRARY: List[FloraType] = [
    # Camada Emergente
    FloraType(
        name="kapok_tree",
        layer=FloraLayer.EMERGENT,
        height_min=45.0, height_max=65.0,
        trunk_radius=2.0, crown_radius=15.0,
        density=0.5,
        color_base=(0.15, 0.35, 0.15)
    ),
    FloraType(
        name="brazil_nut",
        layer=FloraLayer.EMERGENT,
        height_min=40.0, height_max=55.0,
        trunk_radius=1.5, crown_radius=12.0,
        density=0.3,
        color_base=(0.18, 0.4, 0.18)
    ),
    
    # Camada Dossel
    FloraType(
        name="canopy_tree",
        layer=FloraLayer.CANOPY,
        height_min=25.0, height_max=40.0,
        trunk_radius=0.8, crown_radius=8.0,
        density=3.0,
        color_base=(0.2, 0.45, 0.2)
    ),
    FloraType(
        name="palm_tree",
        layer=FloraLayer.CANOPY,
        height_min=20.0, height_max=30.0,
        trunk_radius=0.4, crown_radius=5.0,
        density=2.0,
        color_base=(0.25, 0.5, 0.2)
    ),
    
    # Camada Sub-bosque
    FloraType(
        name="understory_tree",
        layer=FloraLayer.UNDERSTORY,
        height_min=10.0, height_max=20.0,
        trunk_radius=0.3, crown_radius=4.0,
        density=5.0,
        color_base=(0.22, 0.48, 0.22)
    ),
    FloraType(
        name="banana_plant",
        layer=FloraLayer.UNDERSTORY,
        height_min=3.0, height_max=8.0,
        trunk_radius=0.2, crown_radius=3.0,
        density=2.0,
        color_base=(0.3, 0.55, 0.25),
        has_trunk=False
    ),
    
    # Camada Arbustos
    FloraType(
        name="fern_large",
        layer=FloraLayer.SHRUB,
        height_min=1.5, height_max=4.0,
        trunk_radius=0.1, crown_radius=2.0,
        density=15.0,
        color_base=(0.25, 0.55, 0.25),
        has_trunk=False
    ),
    FloraType(
        name="heliconia",
        layer=FloraLayer.SHRUB,
        height_min=1.0, height_max=3.0,
        trunk_radius=0.05, crown_radius=1.0,
        density=8.0,
        color_base=(0.5, 0.2, 0.2),  # Flores vermelhas
        has_trunk=False
    ),
    
    # Camada Solo
    FloraType(
        name="ground_fern",
        layer=FloraLayer.FLOOR,
        height_min=0.2, height_max=0.8,
        trunk_radius=0.02, crown_radius=0.5,
        density=50.0,
        color_base=(0.2, 0.5, 0.2),
        has_trunk=False
    ),
    FloraType(
        name="moss_patch",
        layer=FloraLayer.FLOOR,
        height_min=0.01, height_max=0.1,
        trunk_radius=0.0, crown_radius=0.3,
        density=100.0,
        color_base=(0.15, 0.4, 0.15),
        has_trunk=False,
        has_leaves=False
    ),
]


@dataclass
class PlantInstance:
    """Instância de uma planta gerada."""
    flora_type: FloraType
    x: float
    y: float  # Altura do solo (normalmente 0)
    z: float
    height: float
    rotation_y: float
    scale: float
    color: Tuple[float, float, float]


class _SpatialGrid:
    """Helper class for O(1) spatial queries."""
    def __init__(self, cell_size: float):
        self.cell_size = cell_size
        self.grid: Dict[Tuple[int, int], List[PlantInstance]] = {}

    def add(self, plant: PlantInstance):
        cx = int(plant.x // self.cell_size)
        cz = int(plant.z // self.cell_size)
        if (cx, cz) not in self.grid:
            self.grid[(cx, cz)] = []
        self.grid[(cx, cz)].append(plant)

    def is_too_close(self, x: float, z: float, min_dist: float) -> bool:
        """Checks if (x, z) is within min_dist of any plant in the grid."""
        cx = int(x // self.cell_size)
        cz = int(z // self.cell_size)

        min_dist_sq = min_dist * min_dist

        # Check adjacent cells (3x3 area)
        # Since cell_size is chosen to be >= max(min_dist) for the layer,
        # checking neighbors is sufficient.
        for i in range(cx - 1, cx + 2):
            for j in range(cz - 1, cz + 2):
                if (i, j) in self.grid:
                    for plant in self.grid[(i, j)]:
                        dx = x - plant.x
                        dz = z - plant.z
                        if dx*dx + dz*dz < min_dist_sq:
                            return True
        return False


class TropicalFloraGenerator:
    """Gerador de vegetação tropical procedural."""
    
    def __init__(self, seed: int = 42):
        self.seed = seed
        self.rng = random.Random(seed)
        self.flora_library = TROPICAL_FLORA_LIBRARY
        
    def _random_in_range(self, min_val: float, max_val: float) -> float:
        return min_val + self.rng.random() * (max_val - min_val)
    
    def _vary_color(self, base: Tuple[float, float, float], 
                    variation: float) -> Tuple[float, float, float]:
        """Aplica variação de cor."""
        return (
            max(0.0, min(1.0, base[0] + self.rng.uniform(-variation, variation))),
            max(0.0, min(1.0, base[1] + self.rng.uniform(-variation, variation))),
            max(0.0, min(1.0, base[2] + self.rng.uniform(-variation, variation)))
        )
    
    def _is_too_close(self, x: float, z: float, 
                      existing: Union[List[PlantInstance], _SpatialGrid],
                      min_dist: float) -> bool:
        """Verifica se posição está muito próxima de plantas existentes."""
        if isinstance(existing, _SpatialGrid):
            return existing.is_too_close(x, z, min_dist)

        # Fallback for list (O(N) check)
        for plant in existing:
            dx = x - plant.x
            dz = z - plant.z
            dist_sq = dx*dx + dz*dz
            if dist_sq < min_dist * min_dist:
                return True
        return False
    
    def generate_layer(self, 
                      layer: FloraLayer,
                      area_size: float,
                      center_x: float = 0.0,
                      center_z: float = 0.0,
                      clearing_radius: float = 10.0) -> List[PlantInstance]:
        """Gera plantas para uma camada específica."""
        plants: List[PlantInstance] = []
        
        # Filtra tipos desta camada
        layer_types = [f for f in self.flora_library if f.layer == layer]
        
        if not layer_types:
            return []

        # Determine grid cell size based on the largest possible check radius for this layer
        # min_distance = flora.crown_radius * 1.5
        max_check_dist = max((f.crown_radius * 1.5 for f in layer_types), default=1.0)
        # Ensure a reasonable minimum size to avoid too many small cells
        grid_cell_size = max(max_check_dist, 1.0)

        grid = _SpatialGrid(grid_cell_size)

        for flora in layer_types:
            # Calcula número de plantas baseado em densidade
            area = area_size * area_size
            expected_count = int((flora.density / 100.0) * area)
            
            # Distância mínima baseada no tamanho da copa
            min_distance = flora.crown_radius * 1.5
            
            attempts = 0
            max_attempts = expected_count * 10

            # Count how many of this type we have already (O(N) but only done once per type)
            current_count = len([p for p in plants if p.flora_type == flora])
            
            while current_count < expected_count and attempts < max_attempts:
                attempts += 1
                
                # Posição aleatória
                x = center_x + self.rng.uniform(-area_size/2, area_size/2)
                z = center_z + self.rng.uniform(-area_size/2, area_size/2)
                
                # Pula se na clareira central
                dist_from_center = math.sqrt(x*x + z*z)
                if dist_from_center < clearing_radius:
                    continue
                
                # Pula se muito próximo de outra planta
                if self._is_too_close(x, z, grid, min_distance):
                    continue
                
                # Gera instância
                height = self._random_in_range(flora.height_min, flora.height_max)
                scale = height / ((flora.height_min + flora.height_max) / 2)
                
                plant = PlantInstance(
                    flora_type=flora,
                    x=x,
                    y=0.0,
                    z=z,
                    height=height,
                    rotation_y=self.rng.uniform(0, math.pi * 2),
                    scale=scale,
                    color=self._vary_color(flora.color_base, flora.color_variation)
                )
                plants.append(plant)
                grid.add(plant)
                current_count += 1
        
        return plants
    
    def generate_forest(self,
                       area_size: float = 200.0,
                       center_x: float = 0.0,
                       center_z: float = 0.0,
                       clearing_radius: float = 15.0,
                       density_multiplier: float = 1.0) -> dict:
        """
        Gera floresta completa com todas as 5 camadas.
        
        Args:
            area_size: Tamanho da área
            center_x, center_z: Centro
            clearing_radius: Raio da clareira
            density_multiplier: Multiplicador de densidade (0.0 a 1.0+)
        
        Returns:
            Dict com plants por camada.
        """
        forest = {}
        
        for layer in FloraLayer:
            plants = self.generate_layer(
                layer=layer,
                area_size=area_size,
                center_x=center_x,
                center_z=center_z,
                clearing_radius=clearing_radius
            )
            
            # Apply density multiplier randomly removing plants
            if density_multiplier < 1.0:
                target_count = int(len(plants) * density_multiplier)
                if target_count < len(plants):
                    plants = self.rng.sample(plants, target_count)
            
            forest[layer] = plants
        
        return forest
    
    def get_statistics(self, forest: dict) -> dict:
        """Retorna estatísticas da floresta gerada."""
        stats = {
            "total_plants": 0,
            "by_layer": {}
        }
        
        for layer, plants in forest.items():
            count = len(plants)
            stats["by_layer"][layer.value] = count
            stats["total_plants"] += count
        
        return stats
