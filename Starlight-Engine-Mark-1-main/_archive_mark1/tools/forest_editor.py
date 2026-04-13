import os
import json
import math
from starlight import App, backend

class ForestEditor(App):
    def __init__(self):
        super().__init__("Starlight Forest Editor", 1280, 720)
        self.assets = {}
        self.entities = []
        self.selected_asset = "oak_tree"
        self.asset_list = ["oak_tree", "fern", "rock", "mushroom"]
        self.camera_pos = [0, 5, 15]
        self.camera_rot = [0, -0.1, 0]
        self.is_running = True

    def on_start(self):
        print("[EDITOR] Loading assets...")
        # Textures
        self.assets["oak_tree"] = {
            "bark": backend.load_texture("assets/textures/oak_bark.png"),
            "leaves": backend.load_texture("assets/textures/oak_leaves_alpha.png"),
            "trunk_mesh": backend.load_mesh("assets/models/generated/trunk.obj"),
            "leaves_mesh": backend.load_mesh("assets/models/generated/leaves.obj")
        }
        self.assets["fern"] = {
            "tex": backend.load_texture("assets/textures/fern_plant_alpha.png"),
            "mesh": backend.load_mesh("assets/models/generated/vegetation_cross.obj")
        }
        self.assets["rock"] = {
            "tex": backend.load_texture("assets/textures/moss_rock.png"),
            "mesh": "cube" # For now
        }
        self.assets["mushroom"] = {
            "tex": backend.load_texture("assets/textures/mushroom_alpha.png"),
            "mesh": "cube"
        }
        
        # Ground
        backend.spawn_static_box(0, -0.25, 0, 100, 0.25, 100, 0.2, 0.3, 0.1)
        backend.set_sun_direction(-0.5, -0.5, -0.5)
        backend.set_sun_color(1.0, 0.9, 0.8, 1.0)
        backend.set_ambient_intensity(0.2)
        
        print(f"[EDITOR] Controls: ")
        print("  WASD: Move | Mouse: Rotate")
        print("  1-4: Select Asset | Click: Place Asset")
        print("  S: Save Map | L: Load Map")

    def spawn_asset(self, asset_type, x, y, z):
        if asset_type == "oak_tree":
            trunk = backend.spawn_entity(x, y, z)
            backend.set_mesh(trunk, self.assets["oak_tree"]["trunk_mesh"])
            backend.set_material_textures(trunk, self.assets["oak_tree"]["bark"], "flat_normal")
            
            leaves = backend.spawn_entity(x, y, z)
            backend.set_mesh(leaves, self.assets["oak_tree"]["leaves_mesh"])
            backend.set_material_textures(leaves, self.assets["oak_tree"]["leaves"], "flat_normal")
            self.entities.append({"type": "oak_tree", "entities": [trunk, leaves], "pos": [x, y, z]})
        
        elif asset_type == "fern":
            fern = backend.spawn_entity(x, y, z)
            backend.set_mesh(fern, self.assets["fern"]["mesh"])
            backend.set_material_textures(fern, self.assets["fern"]["tex"], "flat_normal")
            backend.set_scale(fern, 0.8, 0.8, 0.8)
            self.entities.append({"type": "fern", "entities": [fern], "pos": [x, y, z]})
            
        elif asset_type == "rock":
            rock = backend.spawn_entity(x, y, z)
            backend.set_mesh(rock, "cube")
            backend.set_material_textures(rock, self.assets["rock"]["tex"], "flat_normal")
            backend.set_scale(rock, 1.2, 0.8, 1.5)
            self.entities.append({"type": "rock", "entities": [rock], "pos": [x, y, z]})

    def on_update(self):
        # Extremely simplified raycast (intersect with Y=0 plane)
        # We assume cursor is center of screen for now
        # Project camera forward
        forward_x = math.sin(self.camera_rot[0]) * math.cos(self.camera_rot[1])
        forward_y = math.sin(self.camera_rot[1])
        forward_z = math.cos(self.camera_rot[0]) * math.cos(self.camera_rot[1])
        
        # Ray: P = O + t*D.  P.y = 0  => O.y + t*D.y = 0 => t = -O.y / D.y
        if abs(forward_y) > 0.001:
            t = -self.camera_pos[1] / forward_y
            if t > 0:
                hit_x = self.camera_pos[0] + t * forward_x
                hit_z = self.camera_pos[2] + t * forward_z
                
                # Visual guide (Place holder cursor)
                # backend.set_point_light(0, hit_x, 0.5, hit_z, 1, 1, 0, 5) 
                
                # Mock input - for now we use frame intervals as "click" for testing
                # In real app, we check mouse buttons
                pass

    def save_map(self):
        data = []
        for item in self.entities:
            data.append({"type": item["type"], "pos": item["pos"]})
        with open("assets/scenes/forest_map.json", "w") as f:
            json.dump(data, f)
        print("[EDITOR] Map saved to assets/scenes/forest_map.json")

if __name__ == "__main__":
    app = ForestEditor()
    # app.run() # User will run manually
