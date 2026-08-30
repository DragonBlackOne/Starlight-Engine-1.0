from typing import Self, TYPE_CHECKING
from . import backend

if TYPE_CHECKING:
    from .app import App

class Entity:
    """
    Represents a game object in the Starlight Engine.
    """
    def __init__(self, id: int) -> None:
        self.id = id
        self.scale: list[float] = [1.0, 1.0, 1.0]
        self._transform: backend.Transform | None = None

    @property
    def transform(self) -> backend.Transform | None:
        if self._transform is None:
            try:
                self._transform = backend.get_transform(self.id)
            except Exception:
                pass
        return self._transform

    def update_transform(self) -> None:
        if self._transform is not None:
            backend.set_transform(self.id, self._transform.x, self._transform.y, self._transform.z)

    def set_rotation(self, x: float, y: float, z: float) -> None:
        """Sets the rotation of the entity using Euler angles (radians)."""
        if hasattr(backend, 'set_rotation'):
            backend.set_rotation(self.id, x, y, z)

    def set_mesh(self, mesh_id: str) -> None:
        """Sets the mesh for this entity using a UUID."""
        if hasattr(backend, 'set_mesh'):
            backend.set_mesh(self.id, mesh_id)

    def set_material(self, diffuse_id: str, normal_id: str = "flat_normal") -> None:
        """Sets the material textures for this entity using UUIDs."""
        if hasattr(backend, 'set_material_textures'):
            backend.set_material_textures(self.id, diffuse_id, normal_id)

    @staticmethod
    def spawn(app: "App", x: float, y: float, z: float, scale: float | list[float] = 1.0, model: str = "cube", color: tuple[float, float, float] = (0.7, 0.7, 0.7)) -> "Entity":
        # Helper to map model types to backend calls
        id: int
        r, g, b = color
        
        # Calculate half-size for box
        if isinstance(scale, list):
            sx, sy, sz = scale[0] * 0.5, scale[1] * 0.5, scale[2] * 0.5
        else:
            sx = sy = sz = scale * 0.5
            
        match model:
            case "cube":
                # Use spawn_static_box for visible colors
                id = backend.spawn_static_box(x, y, z, sx, sy, sz, r, g, b)
            case "floor":
                # Floor is a flat box
                id = backend.spawn_static_box(x, -0.25, z, scale * 0.5, 0.25, scale * 0.5, r, g, b)
            case "ship":
                id = backend.spawn_procedural_ship(x, y, z, 0)
            case _:
                id = backend.spawn_entity(x, y, z)

        ent = Entity(id)
        if isinstance(scale, list):
            ent.scale = scale
        else:
            ent.scale = [scale, scale, scale]

        # If model name is not special, assume it's a loaded mesh name
        if model not in ["cube", "floor", "ship"]:
            ent.set_mesh(model)

        return ent
