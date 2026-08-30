
import json
import os
from ..framework import Scene, Entity, get_active_scene


def serialize_entity(ent: Entity) -> dict:
    """Converts an Entity object into a dictionary."""
    ent_data = {
        "name": ent.name,
        "transform": {
            "position": ent.transform.position,
            "rotation": ent.transform.rotation,
            "scale": ent.transform.scale
        },
        "components": {}
    }

    # Mesh / Material
    if ent._mesh:
        ent_data["components"]["mesh"] = ent._mesh
    
    if ent._diffuse:
        ent_data["components"]["material"] = {
            "diffuse": ent._diffuse,
            "normal": ent._normal
        }
    return ent_data

def deserialize_entity(ent_data: dict) -> Entity:
    """Creates an Entity object from a dictionary."""
    t = ent_data.get("transform", {})
    pos = t.get("position", [0, 0, 0])
    
    # Create Entity
    entity = Entity(ent_data.get("name", "Entity"), pos[0], pos[1], pos[2])
    entity.set_rotation(*t.get("rotation", [0, 0, 0]))
    entity.set_scale(*t.get("scale", [1, 1, 1]))
    
    # Restore Components
    comps = ent_data.get("components", {})
    if "mesh" in comps:
        entity.set_mesh(comps["mesh"])
    
    if "material" in comps:
        mat = comps["material"]
        entity.set_material(mat.get("diffuse"), mat.get("normal", "flat_normal"))
        
    return entity

def serialize_scene(scene: Scene) -> dict:
    """Converts a Scene object into a dictionary."""
    data = {
        "version": 1,
        "sun": {
            "direction": scene._sun_dir,
            "color": scene._sun_color,
            "intensity": scene._sun_intensity
        },
        "entities": []
    }

    for ent in scene.entities:
        data["entities"].append(serialize_entity(ent))
        
    return data

def save_scene(scene: Scene, filepath: str):
    """Saves the scene to a JSON file."""
    data = serialize_scene(scene)
    with open(filepath, 'w') as f:
        json.dump(data, f, indent=4)
    print(f"[Serialization] Scene saved to {filepath}")

def load_scene(filepath: str):
    """Loads a scene from a JSON file. Returns a new Scene object."""
    if not os.path.exists(filepath):
        print(f"[Serialization] File not found: {filepath}")
        return None

    try:
        with open(filepath, 'r') as f:
            data = json.load(f)
            
        # Clear existing entities from backend
        try:
            from .. import backend
            all_ids = backend.get_all_entities()
            print(f"[Serialization] Clearing {len(all_ids)} entities...")
            for id in all_ids:
                backend.despawn_entity(id)
        except Exception as e:
            print(f"[Serialization] Warning: Failed to clear backend entities: {e}")

        # Create new scene (this sets it as active global)
        scene = Scene()
        
        # Restore Sun
        sun = data.get("sun", {})
        if sun:
            scene.set_sun(
                tuple(sun.get("direction", (-0.5, -1.0, -0.5))),
                tuple(sun.get("color", (1.0, 1.0, 1.0))),
                sun.get("intensity", 1.0)
            )

        # Restore Entities
        for ent_data in data.get("entities", []):
            entity = deserialize_entity(ent_data)
            scene.add(entity)
            
        print(f"[Serialization] Scene loaded from {filepath}")
        return scene

    except Exception as e:
        print(f"[Serialization] Failed to load scene: {e}")
        import traceback
        traceback.print_exc()
        return None

def save_prefab(entity: Entity, filepath: str):
    """Saves a single entity as a prefab JSON file."""
    data = serialize_entity(entity)
    # Wrap in a prefab container for future proofing? Or just the entity dict?
    # Let's wrap it to identify it easily.
    container = {
        "type": "prefab",
        "version": 1,
        "data": data
    }
    with open(filepath, 'w') as f:
        json.dump(container, f, indent=4)
    print(f"[Serialization] Prefab saved to {filepath}")

def load_prefab(filepath: str) -> Entity:
    """Loads a prefab from a JSON file and returns a new Entity (not added to scene)."""
    if not os.path.exists(filepath):
        print(f"[Serialization] Prefab file not found: {filepath}")
        return None
    
    try:
        with open(filepath, 'r') as f:
            container = json.load(f)
        
        # Check format
        if container.get("type") == "prefab":
            data = container.get("data")
        else:
            # Fallback for raw entity dicts if we ever have them
            data = container
            
        return deserialize_entity(data)
    except Exception as e:
        print(f"[Serialization] Failed to load prefab: {e}")
        return None
