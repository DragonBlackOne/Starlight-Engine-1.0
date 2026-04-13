from typing import Dict, Optional
from . import backend

class ResourceManager:
    """
    Manages loading and caching of game assets.
    """
    _instance: Optional["ResourceManager"] = None

    def __init__(self) -> None:
        self.textures: Dict[str, str] = {}
        self.meshes: Dict[str, str] = {}

    @classmethod
    def get(cls) -> "ResourceManager":
        if cls._instance is None:
            cls._instance = ResourceManager()
        return cls._instance

    def load_texture(self, name: str, path: str, is_normal: bool = False) -> str:
        """
        Loads a texture and maps it to a friendly name.
        Returns the backend UUID for the texture.
        """
        if name in self.textures:
            return self.textures[name]

        try:
            # Backend now returns UUID string and expects (path, is_normal)
            # We treat 'name' as a client-side alias for the path.
            uuid_str = backend.load_texture(path, is_normal)
            self.textures[name] = uuid_str
            print(f"[Resource] Loaded texture '{name}' from '{path}' -> UUID: {uuid_str}")
            return uuid_str
        except Exception as e:
            print(f"[Resource] Failed to load texture {name}: {e}")
            return ""

    def load_mesh(self, name: str, path: str) -> str:
        """
        Loads a mesh and maps it to a friendly name.
        Returns the backend UUID for the mesh.
        """
        if name in self.meshes:
            return self.meshes[name]

        try:
            # Backend now returns UUID string and expects (path)
            uuid_str = backend.load_mesh(path)
            self.meshes[name] = uuid_str
            print(f"[Resource] Loaded mesh '{name}' from '{path}' -> UUID: {uuid_str}")
            return uuid_str
        except Exception as e:
            print(f"[Resource] Failed to load mesh {name}: {e}")
            return ""

    def load_skybox(self, path: str) -> str:
        try:
            uuid_str = backend.load_skybox(path)
            print(f"[Resource] Loaded skybox from: {path} -> UUID: {uuid_str}")
            return uuid_str
        except Exception as e:
            print(f"[Resource] Failed to load skybox: {e}")
            return ""

    def get_texture_id(self, name: str) -> str:
        return self.textures.get(name, name) # Fallback to name if not found (for reserved IDs like 'default')

    def get_mesh_id(self, name: str) -> str:
        return self.meshes.get(name, name)

class Scene:
    """
    Base class for game scenes.
    """
    def on_start(self) -> None:
        pass

    def on_update(self, dt: float) -> None:
        pass

    def on_exit(self) -> None:
        pass
