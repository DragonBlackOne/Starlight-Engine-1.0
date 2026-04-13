#!/usr/bin/env python3
"""
Starlight Engine - Ultra Realistic Forest Demo
===============================================
Demonstração de floresta fotorrealista usando:
- Poisson Disk Sampling para posicionamento natural
- Múltiplas camadas de vegetação
- Texturas geradas por IA
- Iluminação golden hour
- Fog volumétrico
"""

import sys
import os
import random
import math
import time

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, backend


def poisson_disk_sample(width, height, min_dist, k=30):
    """Generate points using Poisson Disk Sampling for natural distribution."""
    cell_size = min_dist / math.sqrt(2)
    cols = int(width / cell_size) + 1
    rows = int(height / cell_size) + 1
    grid = [[None for _ in range(cols)] for _ in range(rows)]
    points = []
    active = []
    
    # Start with random point
    x0, y0 = random.random() * width, random.random() * height
    points.append((x0, y0))
    active.append((x0, y0))
    grid[int(y0 / cell_size)][int(x0 / cell_size)] = (x0, y0)
    
    while active:
        idx = random.randint(0, len(active) - 1)
        px, py = active[idx]
        found = False
        
        for _ in range(k):
            angle = random.random() * 2 * math.pi
            r = min_dist + random.random() * min_dist
            nx = px + r * math.cos(angle)
            ny = py + r * math.sin(angle)
            
            if 0 <= nx < width and 0 <= ny < height:
                col, row = int(nx / cell_size), int(ny / cell_size)
                if 0 <= col < cols and 0 <= row < rows:
                    valid = True
                    
                    for dr in range(-2, 3):
                        for dc in range(-2, 3):
                            r2, c2 = row + dr, col + dc
                            if 0 <= r2 < rows and 0 <= c2 < cols:
                                if grid[r2][c2]:
                                    ox, oy = grid[r2][c2]
                                    if (nx - ox)**2 + (ny - oy)**2 < min_dist**2:
                                        valid = False
                                        break
                        if not valid:
                            break
                    
                    if valid:
                        points.append((nx, ny))
                        active.append((nx, ny))
                        grid[row][col] = (nx, ny)
                        found = True
                        break
        
        if not found:
            active.pop(idx)
    
    return points


class UltraRealisticForest(App):
    def on_start(self):
        print("[FOREST] Initializing Ultra Realistic Forest...")
        
        # === Camera ===
        try:
            self.cam_id = backend.get_main_camera_id()
        except:
            self.cam_id = None
        
        # === ASSET REGISTRY ===
        self.assets = {}
        
        # Ground textures
        self.assets["grass"] = backend.load_texture("assets/textures/forest_grass.png")
        
        # Tree textures
        self.assets["bark"] = backend.load_texture("assets/textures/oak_bark.png")
        self.assets["leaves"] = backend.load_texture("assets/textures/oak_leaves_alpha.png")
        
        # Detail textures
        self.assets["rock"] = backend.load_texture("assets/textures/moss_rock.png")
        self.assets["fern"] = backend.load_texture("assets/textures/fern_plant_alpha.png")
        self.assets["mushroom"] = backend.load_texture("assets/textures/mushroom_alpha.png")
        
        # Meshes
        self.assets["trunk_mesh"] = backend.load_mesh("assets/models/generated/trunk.obj")
        self.assets["leaves_mesh"] = backend.load_mesh("assets/models/generated/leaves.obj")
        self.assets["veg_mesh"] = backend.load_mesh("assets/models/generated/vegetation_cross.obj")
        
        # State
        self.t = 0.0
        self.frame = 0
        self.scene_created = False
        self.captured = False
        self.trees = []
        self.undergrowth = []
        
        print(f"[FOREST] {len(self.assets)} assets queued for loading...")
    
    def create_scene(self):
        """Create the realistic forest after assets load."""
        print("[FOREST] Creating ultra-realistic forest scene...")
        
        random.seed(2024)  # Reproducible
        
        # === LIGHTING ===
        print("[FOREST] Setting up golden hour lighting...")
        backend.set_sun_direction(-0.7, -0.4, -0.6)  # Low sun angle
        backend.set_sun_color(1.0, 0.85, 0.65, 3.5)  # Warm golden light, high intensity
        backend.set_ambient_intensity(0.08)          # Deep shadows for realism
        
        # Warm point lights simulating sun rays through trees
        backend.set_point_light(0, -15.0, 8.0, 10.0, 1.0, 0.85, 0.5, 25.0)
        backend.set_point_light(1, 20.0, 6.0, -5.0, 1.0, 0.9, 0.6, 20.0)
        backend.set_point_light(2, 5.0, 12.0, -20.0, 0.95, 0.8, 0.55, 30.0)
        backend.set_point_light(3, -10.0, 10.0, -15.0, 1.0, 0.7, 0.4, 18.0)
        
        # === VOLUMETRIC-LOOKING FOG ===
        backend.set_fog(0.015, 0.55, 0.5, 0.42)      # Slightly denser fog
        backend.set_skybox_color(0.5, 0.45, 0.35)    # Muted dark sky for contrast
        
        # === WIND ANIMATION ===
        backend.set_wind_strength(0.15)              # Subtle natural sway

        # === POST PROCESSING ===
        print("[FOREST] Enabling cinematic post-processing (Bloom + Tone Mapping)...")
        # Exposure 1.2 for brightness, Gamma 2.2 standard, Bloom 0.4 soft glow, Threshold 1.5 (only bright spots)
        backend.set_post_process_params(1.2, 2.2, 0.4, 1.5)
        
        # === GROUND ===
        print("[FOREST] Creating forest floor...")
        floor = backend.spawn_static_box(0.0, -0.25, 0.0, 80.0, 0.25, 80.0, 0.3, 0.4, 0.2)
        backend.set_material_textures(floor, self.assets["grass"], "flat_normal")
        
        # === GENERATE TREES WITH POISSON DISK SAMPLING ===
        print("[FOREST] Generating tree positions with Poisson Disk Sampling...")
        
        FOREST_SIZE = 60.0
        MIN_TREE_DISTANCE = 5.0  # Minimum distance between trees
        
        tree_positions = poisson_disk_sample(
            FOREST_SIZE, FOREST_SIZE, 
            MIN_TREE_DISTANCE
        )
        print(f"[FOREST] Generated {len(tree_positions)} tree positions")
        
        # Spawn trees
        for i, (px, pz) in enumerate(tree_positions):
            x = px - FOREST_SIZE / 2
            z = pz - FOREST_SIZE / 2
            
            # Skip center area (clearing for camera)
            dist_from_center = math.sqrt(x*x + z*z)
            if dist_from_center < 8.0:
                continue
            
            # Tree variation
            scale = 0.6 + random.random() * 0.5
            rot_y = random.random() * 6.28
            lean = random.uniform(-0.05, 0.05)  # Slight lean
            
            # Height variation based on distance from center
            height_mult = 0.9 + 0.2 * (dist_from_center / FOREST_SIZE)
            
            # === TRUNK ===
            trunk = backend.spawn_entity(x, 0.0, z)
            backend.set_mesh(trunk, self.assets["trunk_mesh"])
            backend.set_material_textures(trunk, self.assets["bark"], "flat_normal")
            backend.set_scale(trunk, scale, scale * height_mult * 1.2, scale)
            backend.set_rotation(trunk, lean, rot_y, 0.0)
            
            # === CANOPY ===
            leaves = backend.spawn_entity(x, 0.0, z)
            backend.set_mesh(leaves, self.assets["leaves_mesh"])
            backend.set_material_textures(leaves, self.assets["leaves"], "flat_normal")
            canopy_scale = scale * (0.9 + random.random() * 0.3)
            backend.set_scale(leaves, canopy_scale, canopy_scale * 0.9, canopy_scale)
            backend.set_rotation(leaves, lean, rot_y + random.uniform(-0.2, 0.2), 0.0)
            
            self.trees.append((trunk, leaves))
            
            # === UNDERGROWTH (Near trees) ===
            if random.random() < 0.7:
                for _ in range(random.randint(1, 4)):
                    ux = x + random.uniform(-2.5, 2.5)
                    uz = z + random.uniform(-2.5, 2.5)
                    
                    if random.random() < 0.8:
                        # Fern (Cross Plane)
                        fern = backend.spawn_entity(ux, 0.0, uz)
                        backend.set_mesh(fern, self.assets["veg_mesh"])
                        backend.set_material_textures(fern, self.assets["fern"], "flat_normal")
                        fs = 0.5 + random.random() * 0.8
                        backend.set_scale(fern, fs, fs * 0.8, fs)
                        backend.set_rotation(fern, 0.0, random.uniform(0, 6.28), 0.0)
                        self.undergrowth.append(fern)
                    else:
                        # Mushroom (Small Cubes for now, but better scaled)
                        mush = backend.spawn_entity(ux, 0.0, uz)
                        backend.set_mesh(mush, "cube")
                        backend.set_material_textures(mush, self.assets["mushroom"], "flat_normal")
                        ms = 0.15 + random.random() * 0.1
                        backend.set_scale(mush, ms, ms * 1.5, ms)
                        self.undergrowth.append(mush)
        
        print(f"[FOREST] Spawned {len(self.trees)} trees")
        
        # === DECORATIVE ROCKS ===
        print("[FOREST] Adding decorative elements...")
        for _ in range(15):
            rx = random.uniform(-30, 30)
            rz = random.uniform(-30, 30)
            if math.sqrt(rx*rx + rz*rz) < 6.0:
                continue
            rock = backend.spawn_static_box(rx, 0.2, rz, 
                                           0.5 + random.random() * 1.0,
                                           0.2 + random.random() * 0.4,
                                           0.5 + random.random() * 1.0,
                                           0.4, 0.4, 0.35)
            backend.set_material_textures(rock, self.assets["rock"], "flat_normal")
        
        self.scene_created = True
        print("[FOREST] Scene creation complete!")
    
    def on_update(self, dt):
        self.t += dt
        self.frame += 1
        
        # Wait for all assets to load before creating scene
        if not self.scene_created:
            loaded_count = sum(1 for id in self.assets.values() if backend.is_asset_loaded(id))
            if self.frame % 30 == 0:
                print(f"[FOREST] Loading assets: {loaded_count}/{len(self.assets)}")
            
            if loaded_count == len(self.assets):
                self.create_scene()
            return
        
        if not self.scene_created:
            return
        
        # === ANIMATE LIGHTS (simulating light through moving leaves) ===
        t = self.t
        backend.set_point_light(0, 
            -15.0 + math.sin(t * 0.3) * 3.0, 
            8.0 + math.sin(t * 0.5) * 1.5, 
            10.0 + math.cos(t * 0.4) * 2.0, 
            1.0, 0.85, 0.5, 25.0)
        
        # === CINEMATIC CAMERA (closer view) ===
        if self.cam_id:
            # Smooth orbit through forest - closer to trees
            cam_angle = t * 0.1
            cam_radius = 6.0 + math.sin(t * 0.1) * 2.0  # Much closer
            
            cam_x = math.sin(cam_angle) * cam_radius
            cam_z = math.cos(cam_angle) * cam_radius
            cam_y = 2.5 + math.sin(t * 0.15) * 0.5  # Lower, more immersive
            
            # Look toward center with slight offset
            look_x = math.sin(t * 0.05) * 3.0
            look_z = math.cos(t * 0.07) * 3.0
            
            dx = look_x - cam_x
            dy = 2.0 - cam_y
            dz = look_z - cam_z
            dist = math.sqrt(dx*dx + dy*dy + dz*dz)
            if dist > 0:
                dx /= dist
                dy /= dist
                dz /= dist
            
            pitch = math.asin(dy)
            yaw = math.atan2(dx, -dz)
            
            backend.set_transform(self.cam_id, cam_x, cam_y, cam_z)
            backend.set_rotation(self.cam_id, -pitch, yaw, 0.0)
        
        # === SCREENSHOT (wait longer for textures to fully apply) ===
        if self.frame > 300 and not self.captured:  # ~10 seconds
            backend.capture_screenshot("screenshots/ultra_realistic_forest.png")
            print("[FOREST] Screenshot saved!")
            self.captured = True


if __name__ == "__main__":
    os.makedirs("screenshots", exist_ok=True)
    UltraRealisticForest(
        title="Starlight Engine - Ultra Realistic Forest",
        width=1920, height=1080
    ).run()
