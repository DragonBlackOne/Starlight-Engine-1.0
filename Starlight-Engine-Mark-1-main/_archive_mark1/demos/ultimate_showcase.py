#!/usr/bin/env python3
"""
Starlight Engine - Ultimate Showcase Demo (Fixed)
==================================================
Demonstração de TODOS os recursos gráficos disponíveis na engine.

FIX: Aguarda as texturas carregarem antes de criar entidades que as usam.
"""

import sys
import os
import random
import math

# Prioritizar módulo local sobre site-packages
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, backend

class UltimateShowcase(App):
    def on_start(self):
        print("[ULTIMATE] Initializing Ultimate Showcase...")
        
        # === Asset Paths ===
        self.models_dir = "assets/models/generated"
        self.tex_dir = "assets/textures"
        
        # Get camera
        try:
            self.cam_id = backend.get_main_camera_id()
        except:
            self.cam_id = None
        
        # === 1. PRELOAD ALL ASSETS (async) ===
        print("[ULTIMATE] Loading assets (textures & meshes)...")
        
        # Textures - just trigger loading (textures are in assets/ root, not assets/textures/)
        try:
            self.grass_id = backend.load_texture("assets/ground_grass_diffuse.png")
            self.bark_id = backend.load_texture("assets/palm_bark_diffuse.png")
            self.leaves_id = backend.load_texture("assets/leaves_diffuse.png")
            print(f"[ULTIMATE] Texture UUIDs: grass={self.grass_id[:8]}, bark={self.bark_id[:8]}, leaves={self.leaves_id[:8]}")
        except Exception as e:
            print(f"[ULTIMATE] Texture load error: {e}")
            self.grass_id = "default"
            self.bark_id = "default"
            self.leaves_id = "default"
        
        # Meshes - just trigger loading
        try:
            self.trunk_mesh = backend.load_mesh(os.path.join(self.models_dir, "trunk.obj"))
            self.leaves_mesh = backend.load_mesh(os.path.join(self.models_dir, "leaves.obj"))
        except Exception as e:
            print(f"[ULTIMATE] Mesh load error: {e}")
            self.trunk_mesh = "cube"
            self.leaves_mesh = "cube"
        
        # === State ===
        self.t = 0.0
        self.frame = 0
        self.scene_created = False
        self.captured = False
        self.ship_id = None
        
        print("[ULTIMATE] Asset loading triggered, waiting for async load...")
    
    def create_scene(self):
        """Create scene after assets have loaded."""
        print("[ULTIMATE] Creating scene...")
        
        # === LIGHTING ===
        backend.set_sun_direction(0.3, -0.8, 0.2)
        backend.set_sun_color(1.6, 1.5, 1.3)
        backend.set_ambient_intensity(0.4)
        
        # 4 Point Lights
        backend.set_point_light(0, -8.0, 4.0, 0.0, 0.3, 0.5, 1.0, 20.0)
        backend.set_point_light(1, 8.0, 4.0, 0.0, 0.3, 1.0, 0.5, 20.0)
        backend.set_point_light(2, 0.0, 3.0, 10.0, 1.0, 0.6, 0.2, 15.0)
        backend.set_point_light(3, 0.0, 5.0, -10.0, 0.9, 0.4, 1.0, 15.0)
        
        # Fog & Sky
        backend.set_fog(0.001, 0.7, 0.75, 0.85)
        backend.set_skybox_color(0.3, 0.45, 0.65)
        
        # === GROUND ===
        print("[ULTIMATE] Creating ground with texture...")
        floor_id = backend.spawn_static_box(0.0, -0.5, 0.0, 50.0, 0.5, 50.0, 0.35, 0.45, 0.3)
        backend.set_material_textures(floor_id, self.grass_id, "flat_normal")
        
        # === PROCEDURAL SPACESHIP ===
        print("[ULTIMATE] Generating procedural spaceship...")
        self.ship_id = backend.spawn_procedural_ship(0.0, 4.0, 0.0, seed=42)
        backend.set_scale(self.ship_id, 3.0, 3.0, 3.0)
        backend.set_color(self.ship_id, 0.9, 0.9, 0.95, 1.0)
        
        # === FOREST RING ===
        print("[ULTIMATE] Spawning trees with textures...")
        TREE_COUNT = 16
        RING_RADIUS = 18.0
        
        for i in range(TREE_COUNT):
            angle = (2 * math.pi * i) / TREE_COUNT
            x = math.cos(angle) * RING_RADIUS
            z = math.sin(angle) * RING_RADIUS
            scale = 0.8 + random.random() * 0.3
            rot_y = random.random() * 6.28
            
            # Trunk
            tid = backend.spawn_entity(x, 0.0, z)
            backend.set_mesh(tid, self.trunk_mesh)
            backend.set_material_textures(tid, self.bark_id, "flat_normal")
            backend.set_scale(tid, scale, scale * 1.2, scale)
            backend.set_rotation(tid, 0.0, rot_y, 0.0)
            
            # Leaves
            lid = backend.spawn_entity(x, 0.0, z)
            backend.set_mesh(lid, self.leaves_mesh)
            backend.set_material_textures(lid, self.leaves_id, "flat_normal")
            backend.set_scale(lid, scale * 1.0, scale * 1.0, scale * 1.0)
            backend.set_rotation(lid, 0.0, rot_y, 0.0)
        
        # === PBR CUBES ===
        print("[ULTIMATE] Creating PBR cubes...")
        colors = [
            (1.0, 0.85, 0.25),  # Gold
            (0.95, 0.95, 1.0),  # Silver
            (0.85, 0.5, 0.35),  # Copper
            (0.2, 0.9, 0.95),   # Turquoise
            (0.95, 0.25, 0.35), # Ruby
        ]
        for i, color in enumerate(colors):
            x = (i - 2) * 4.0
            cube = backend.spawn_dynamic_cube(x, 8.0, -8.0, color[0], color[1], color[2])
            backend.set_scale(cube, 1.5, 1.5, 1.5)
        
        # === PHYSICS CASCADE ===
        print("[ULTIMATE] Spawning physics cubes...")
        random.seed(789)
        for i in range(10):
            x = random.uniform(-6, 6)
            y = 12.0 + i * 2.0
            z = random.uniform(-3, 3)
            r, g, b = random.random(), random.random(), random.random()
            cube = backend.spawn_dynamic_cube(x, y, z, r, g, b)
            backend.set_scale(cube, 0.6, 0.6, 0.6)
        
        self.scene_created = True
        print("[ULTIMATE] Scene created!")
    
    def on_update(self, dt):
        self.t += dt
        self.frame += 1
        
        # Wait 60 frames (~2 seconds) for assets to load, then create scene
        if self.frame == 60 and not self.scene_created:
            self.create_scene()
            return
        
        if not self.scene_created:
            return
        
        # === ANIMATE POINT LIGHTS ===
        backend.set_point_light(0, 
            math.cos(self.t * 0.8) * 10.0, 
            3.5 + math.sin(self.t * 1.5) * 1.0, 
            math.sin(self.t * 0.8) * 10.0, 
            0.2, 0.4, 1.0, 18.0)
        
        backend.set_point_light(1,
            math.cos(-self.t * 0.8 + 3.14) * 10.0,
            3.5 + math.sin(self.t * 1.5 + 1.0) * 1.0,
            math.sin(-self.t * 0.8 + 3.14) * 10.0,
            0.2, 1.0, 0.4, 18.0)
        
        # === ROTATE SPACESHIP ===
        if self.ship_id:
            backend.set_rotation(self.ship_id, 0.0, self.t * 0.3, math.sin(self.t * 0.5) * 0.1)
        
        # === CINEMATIC CAMERA ===
        if self.cam_id:
            cam_x = math.sin(self.t * 0.15) * 15.0
            cam_z = math.cos(self.t * 0.12) * 18.0
            cam_y = 6.0 + math.sin(self.t * 0.2) * 1.5
            
            dx = -cam_x
            dy = 3.0 - cam_y
            dz = -cam_z
            dist = math.sqrt(dx*dx + dy*dy + dz*dz)
            if dist > 0:
                dx /= dist
                dy /= dist
                dz /= dist
            
            pitch = math.asin(dy)
            yaw = math.atan2(dx, -dz)
            
            backend.set_transform(self.cam_id, cam_x, cam_y, cam_z)
            backend.set_rotation(self.cam_id, -pitch, yaw, 0.0)
        
        # === SCREENSHOT after scene exists + 4 seconds ===
        if self.frame > 180 and not self.captured:  # 60 load + 120 render = ~6s
            backend.capture_screenshot("screenshots/ultimate_showcase.png")
            print("[ULTIMATE] Screenshot saved!")
            self.captured = True

if __name__ == "__main__":
    os.makedirs("screenshots", exist_ok=True)
    UltimateShowcase(title="Starlight Engine - Ultimate Showcase", width=1920, height=1080).run()
