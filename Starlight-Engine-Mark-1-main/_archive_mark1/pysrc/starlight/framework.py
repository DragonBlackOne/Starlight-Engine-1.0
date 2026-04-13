from typing import List, Optional, Tuple, Dict, Any, TYPE_CHECKING
from . import backend
import math

if TYPE_CHECKING:
    from .behaviour import StarlightBehaviour

_active_scene = None

def get_active_scene():
    return _active_scene

class Transform:
    def __init__(self, x: float = 0, y: float = 0, z: float = 0):
        self.position = [x, y, z]
        self.rotation = [0.0, 0.0, 0.0]
        self.scale = [1.0, 1.0, 1.0]

class Entity:
    def __init__(self, name: str = "Entity", x: float = 0, y: float = 0, z: float = 0):
        self.id = backend.spawn_entity(x, y, z)
        self.name = name
        self.transform = Transform(x, y, z)
        self._mesh: Optional[str] = None
        self._diffuse: Optional[str] = None
        self._normal: Optional[str] = "flat_normal"
        self.behaviours: List['StarlightBehaviour'] = []

    def add_behaviour(self, behaviour: 'StarlightBehaviour') -> 'StarlightBehaviour':
        """Attaches a Python logic script to this entity."""
        behaviour.entity = self
        self.behaviours.append(behaviour)
        behaviour.on_start()
        return behaviour

    def set_position(self, x: float, y: float, z: float):
        self.transform.position = [x, y, z]
        backend.set_transform(self.id, x, y, z)
        return self

    def set_rotation(self, x: float, y: float, z: float):
        self.transform.rotation = [x, y, z]
        backend.set_rotation(self.id, x, y, z)
        return self

    def set_scale(self, x: float, y: float, z: float):
        self.transform.scale = [x, y, z]
        backend.set_scale(self.id, x, y, z)
        return self

    def set_mesh(self, mesh_id: str):
        self._mesh = mesh_id
        backend.set_mesh(self.id, mesh_id)
        return self

    def set_material(self, diffuse_id: str, normal_id: str = "flat_normal"):
        self._diffuse = diffuse_id
        self._normal = normal_id
        backend.set_material_textures(self.id, diffuse_id, normal_id)
        return self

    def set_material_params(self, metallic: float, roughness: float):
        """Sets the PBR material parameters (metallic, roughness)."""
        self._metallic = metallic
        self._roughness = roughness
        backend.set_material_params(self.id, metallic, roughness)
        return self

    def set_color(self, r: float, g: float, b: float, a: float = 1.0):
        backend.set_color(self.id, r, g, b, a)
        return self

    def set_parent(self, parent_id: int):
        """Sets the parent entity (or camera) for this entity."""
        backend.set_parent(self.id, parent_id)
        return self

    # === PHYSICS ===
    def apply_impulse(self, x: float, y: float, z: float):
        backend.apply_impulse(self.id, x, y, z)
        return self

    def apply_force(self, x: float, y: float, z: float):
        backend.apply_force(self.id, x, y, z)
        return self

    def set_velocity(self, x: float, y: float, z: float):
        backend.set_velocity(self.id, x, y, z)
        return self

    def get_velocity(self) -> Tuple[float, float, float]:
        return backend.get_velocity(self.id)

class Scene:
    def __init__(self):
        global _active_scene
        _active_scene = self
        self.entities: List[Entity] = []
        self._sun_dir = (-0.5, -1.0, -0.5)
        self._sun_color = (1.0, 1.0, 1.0)
        self._sun_intensity = 1.0

    def add(self, entity: Entity):
        self.entities.append(entity)
        return entity

    def setup_skybox(self, folder_path: str):
        # Assumes standard names: right, left, top, bottom, front, back
        # This is a bit high level, but good for framework
        import os
        backend.load_skybox(
            os.path.join(folder_path, "right.jpg"),
            os.path.join(folder_path, "left.jpg"),
            os.path.join(folder_path, "top.jpg"),
            os.path.join(folder_path, "bottom.jpg"),
            os.path.join(folder_path, "front.jpg"),
            os.path.join(folder_path, "back.jpg")
        )

    def set_sun(self, direction: Tuple[float, float, float], color: Tuple[float, float, float], intensity: float = 1.0):
        self._sun_dir = direction
        self._sun_color = color
        self._sun_intensity = intensity
        backend.set_sun_direction(*direction)
        backend.set_sun_color(*color, intensity)

    def update(self, dt: float):
        """Ticks the scene and all attached behaviours."""
        for _ in range(3): # Dummy iter logic or physics step (optional)
            pass
        for ent in self.entities:
            for behaviour in ent.behaviours:
                behaviour.update(dt)

    def set_fog(self, density: float, color: Tuple[float, float, float]):
        backend.set_fog(density, *color)

    def set_ambient(self, intensity: float):
        backend.set_ambient_intensity(intensity)

    def set_post_process(self, exposure: float = 1.0, gamma: float = 2.2, bloom_intensity: float = 0.0, bloom_threshold: float = 1.0, chromatic_aberration: float = 0.0, time: float = 0.0):
        """
        Configure post-processing effects.
        :param exposure: Exposure multiplier (default 1.0)
        :param gamma: Gamma correction (default 2.2)
        :param bloom_intensity: Intensity of bloom effect (0.0 to disable)
        :param bloom_threshold: Luminance threshold for bloom
        :param chromatic_aberration: Intensity of chromatic aberration
        :param time: Current time for animated effects
        """
        backend.set_post_process_params(exposure, gamma, bloom_intensity, bloom_threshold, chromatic_aberration, time)

    def raycast(self, origin: Tuple[float, float, float], direction: Tuple[float, float, float], max_distance: float = 1000.0, solid: bool = True) -> Optional[Tuple[int, float, Tuple[float, float, float], Tuple[float, float, float]]]:
        """
        Cast a ray in the physics world.
        :return: (entity_id, distance, point, normal) or None
        """
        return backend.raycast(origin[0], origin[1], origin[2], direction[0], direction[1], direction[2], max_distance, solid)

class Camera:
    def __init__(self, id: Optional[int] = None):
        if id is None:
            self.id = backend.get_main_camera_id()
        else:
            self.id = id
        self.transform = Transform()

    def set_position(self, x, y, z):
        self.transform.position = [x, y, z]
        backend.set_transform(self.id, x, y, z)
    
    def set_transform(self, x, y, z):
        self.set_position(x, y, z)
    
    def set_rotation(self, pitch, yaw, roll=0):
        self.transform.rotation = [pitch, yaw, roll]
        backend.set_rotation(self.id, pitch, yaw, roll)

    def look_at(self, target_pos: Tuple[float, float, float]):
        # Calculate pitch/yaw
        dx = target_pos[0] - self.transform.position[0]
        dy = target_pos[1] - self.transform.position[1]
        dz = target_pos[2] - self.transform.position[2]
        dist = math.sqrt(dx*dx + dy*dy + dz*dz)
        if dist > 0:
            pitch = math.asin(dy / dist)
            yaw = math.atan2(dx, -dz)
            self.set_rotation(-pitch, yaw)

class Character(Entity):
    def __init__(self, x: float, y: float, z: float, speed: float = 5.0, max_slope: float = 0.78, offset: float = 0.1):
        # We don't call super().__init__ because we use a specific spawn function
        self.id = backend.spawn_character(x, y, z, speed, max_slope, offset)
        self.name = "Character"
        self.transform = Transform(x, y, z)
        self._mesh = None
        self._diffuse = None
        self._normal = "flat_normal"
        
    def move(self, dx: float, dy: float, dz: float) -> Tuple[float, float, float]:
        """
        Move the character by the given delta vector.
        Returns the actual movement vector applied (after collisions).
        """
        return backend.move_character(self.id, dx, dy, dz)

class Engine:
    """The God Tier entry point for Starlight games."""
    def __init__(self, app_class):
        self.app = app_class()
    
    def run(self):
        self.app.run()
