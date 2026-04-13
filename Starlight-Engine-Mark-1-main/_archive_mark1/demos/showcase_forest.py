"""Showcase Forest - High fidelity graphics demo.

Features:
- Procedural Forest Generation with Low Poly Models
- Golden Hour Lighting
- Atmospheric Fog
- Skybox
- Cinematic Camera
"""
import math
import os
import random
import sys

# Ensure pysrc is in path
# Ensure pysrc is in path (prepend to override installed package)
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, backend, math as smath

class ForestDemo(App):
    def on_start(self):
        print("[FOREST] Initializing Showcase Environment...")
        
        # Get Camera ID
        try:
            self.cam_id = backend.get_main_camera_id()
            print(f"[FOREST] Main Camera ID: {self.cam_id}")
        except:
            print("[FOREST] Failed to get main camera ID")
            self.cam_id = None

        # --- 1. Load Assets ---
        bg_sky = "assets/skybox"
        models_dir = "assets/models/generated"
        tex_dir = "assets"
        
        # Skybox
        try:
            sky_path = os.path.join(os.path.dirname(__file__), f"../{bg_sky}")
            backend.load_skybox(
                os.path.join(sky_path, "right.jpg"),
                os.path.join(sky_path, "left.jpg"),
                os.path.join(sky_path, "top.jpg"),
                os.path.join(sky_path, "bottom.jpg"),
                os.path.join(sky_path, "front.jpg"),
                os.path.join(sky_path, "back.jpg")
            )
        except Exception as e:
            print(f"[FOREST] Failed to load skybox: {e}")

        # Models
        trunk_path = os.path.join(models_dir, "trunk.obj")
        leaves_path = os.path.join(models_dir, "leaves.obj")
        rock_path = os.path.join(models_dir, "rock.obj")
        
        # Textures
        tex_bark_path = os.path.join(tex_dir, "palm_bark_diffuse.png")
        tex_leaf_path = os.path.join(tex_dir, "leaves_diffuse.png")
        tex_grass_path = os.path.join(tex_dir, "ground_grass_diffuse.png")
        
        print(f"[FOREST] Loading assets from: {os.getcwd()}")
        print(f"[FOREST] Bark Path: {tex_bark_path}")
        
        # Load and get IDs
        trunk_id = backend.load_mesh(trunk_path)
        leaves_id = backend.load_mesh(leaves_path)
        rock_id = backend.load_mesh(rock_path)
        
        bark_id = backend.load_texture(tex_bark_path)
        leaf_id = backend.load_texture(tex_leaf_path)
        grass_id = backend.load_texture(tex_grass_path)
        
        print(f"[FOREST] Asset IDs -> Trunk: {trunk_id}, Bark: {bark_id}, Leaf: {leaf_id}, Grass: {grass_id}") 
        
        # --- 2. Lighting & Fog ---
        try:
            # Adjusted for less saturation
            backend.set_sun_direction(0.6, 0.3, 0.4)
            backend.set_sun_color(1.2, 1.1, 0.9, 1.5) # Warm golden light with intensity
            backend.set_ambient_intensity(0.3)   # Lower ambient
            backend.set_fog(0.012, 0.7, 0.75, 0.8) # density, r, g, b
        except AttributeError:
            print("[FOREST] Lighting API update pending backend compile.")
        except Exception as e:
            print(f"[FOREST] Lighting error: {e}")
            
        # --- 3. Terrain ---
        # Floor (using spawn_floor or scaled cube with texture)
        floor_id = backend.spawn_static_box(0.0, -1.0, 0.0, 100.0, 1.0, 100.0, 1.0, 1.0, 1.0)
        # Apply grass texture to floor
        backend.set_material_textures(floor_id, grass_id, "flat_normal")
        
        # --- 4. Forest Generation ---
        TREE_COUNT = 150
        AREA_SIZE = 80.0
        
        print(f"[FOREST] Generating {TREE_COUNT} trees...")
        
        for _ in range(TREE_COUNT):
            x = (random.random() * AREA_SIZE) - (AREA_SIZE / 2.0)
            z = (random.random() * AREA_SIZE) - (AREA_SIZE / 2.0)
            
            if -5.0 < x < 5.0 and -5.0 < z < 5.0:
                continue
                
            scale = 0.8 + random.random() * 0.6
            rot_y = random.random() * 6.28
            
            # Spawn Trunk
            tid = backend.spawn_entity(x, 0.0, z)
            backend.set_mesh(tid, trunk_id)
            backend.set_material_textures(tid, bark_id, "flat_normal")
            backend.set_scale(tid, scale, scale, scale)
            backend.set_rotation(tid, 0.0, rot_y, 0.0)
            
            # Spawn Leaves (child or separate)
            lid = backend.spawn_entity(x, 0.0, z)
            backend.set_mesh(lid, leaves_id)
            backend.set_material_textures(lid, leaf_id, "flat_normal")
            backend.set_scale(lid, scale, scale, scale)
            backend.set_rotation(lid, 0.0, rot_y, 0.0) 

        # --- Rocks ---
        ROCK_COUNT = 60
        for _ in range(ROCK_COUNT):
             x = (random.random() * AREA_SIZE) - (AREA_SIZE / 2.0)
             z = (random.random() * AREA_SIZE) - (AREA_SIZE / 2.0)
             s = 0.5 + random.random() * 1.5
             rot_y = random.random() * 6.28
             
             rid = backend.spawn_entity(x, -0.2, z) # Slightly buried
             backend.set_mesh(rid, rock_id)
             backend.set_material_textures(rid, bark_id, "flat_normal") # Reuse bark for rock texture
             backend.set_scale(rid, s, s, s)
             backend.set_rotation(rid, random.random(), rot_y, random.random())
             
             # Color tint for gray rocks
             backend.set_color(rid, 0.6, 0.6, 0.6, 1.0)

        self.t = 0.0
        self.captured = False
        print("[FOREST] Generation Complete.")

    def on_update(self, dt):
        self.t += dt
        
        if self.cam_id is None:
            return

        # Cinematic Camera Path
        cam_x = math.sin(self.t * 0.15) * 25.0
        cam_z = math.cos(self.t * 0.1) * 35.0 + (self.t * 2.0)
        cam_z = (cam_z % 80.0) - 40.0
        cam_y = 6.0 + math.sin(self.t * 0.2) * 2.0
        
        # Look ahead
        target_x = math.sin((self.t + 1.0) * 0.15) * 25.0
        target_z = cam_z + 15.0
        target_y = 4.0
        
        # Calculate LookAt Rotation (Euler)
        dx = target_x - cam_x
        dy = target_y - cam_y
        dz = target_z - cam_z
        dist = math.sqrt(dx*dx + dy*dy + dz*dz)
        if dist > 0:
            dx /= dist
            dy /= dist
            dz /= dist
            
        pitch = math.asin(dy)
        yaw = math.atan2(dx, -dz) 
        
        backend.set_transform(self.cam_id, cam_x, cam_y, cam_z)
        backend.set_rotation(self.cam_id, -pitch, yaw, 0.0)
        
        if self.t > 4.0 and not self.captured:
            backend.capture_screenshot("screenshots/showcase_forest_v5.png")
            print("[FOREST] Screenshot saved.")
            self.captured = True

if __name__ == "__main__":
    os.makedirs("screenshots", exist_ok=True)
    ForestDemo(title="Starlight Engine - Showcase Forest V5").run()
