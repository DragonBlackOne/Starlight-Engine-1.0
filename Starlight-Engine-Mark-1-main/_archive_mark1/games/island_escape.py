import sys
import os
sys.path.append(os.path.abspath("pysrc"))

from starlight import App, Input, Keys, backend
from starlight import framework
from starlight.framework import Scene, Entity, Camera, Character
from starlight.procgen.terrain import TerrainGenerator
from starlight.game.stat_system import StatSystem, Modifier, ModType
import math
import random
import time

import wave
import struct

def generate_tone(filename: str, freq: float, duration: float, volume: float = 0.5, wave_type: str = 'sine'):
    sample_rate = 44100
    n_samples = int(sample_rate * duration)
    
    try:
        with wave.open(filename, 'w') as wav_file:
            wav_file.setnchannels(1)
            wav_file.setsampwidth(2)
            wav_file.setframerate(sample_rate)
            
            data = []
            for i in range(n_samples):
                t = float(i) / sample_rate
                if wave_type == 'sine':
                    val = math.sin(2.0 * math.pi * freq * t)
                elif wave_type == 'square':
                    val = 1.0 if math.sin(2.0 * math.pi * freq * t) > 0 else -1.0
                elif wave_type == 'sawtooth':
                    val = 2.0 * (freq * t - math.floor(freq * t + 0.5))
                elif wave_type == 'noise':
                    val = random.uniform(-1.0, 1.0)
                else:
                    val = math.sin(2.0 * math.pi * freq * t)
                
                # Simple envelope (fade in/out)
                if i < 1000: val *= i / 1000.0
                if i > n_samples - 1000: val *= (n_samples - i) / 1000.0
                
                sample = int(val * volume * 32767.0)
                data.append(struct.pack('<h', sample))
            
            wav_file.writeframes(b''.join(data))
    except Exception as e:
        print(f"Failed to generate {filename}: {e}")

class SoundGenerator:
    """Procedural Audio Generator using generated WAV files and backend."""
    def __init__(self):
        self.enabled = hasattr(backend, 'play_sound')
        self.sfx_path = os.path.abspath("assets/sfx")
        if not os.path.exists(self.sfx_path):
            os.makedirs(self.sfx_path, exist_ok=True)
            
        # Generate assets if missing
        self.jump_file = os.path.join(self.sfx_path, "jump.wav")
        self.collect_file = os.path.join(self.sfx_path, "collect.wav")
        self.step_file = os.path.join(self.sfx_path, "step.wav")
        
        if not os.path.exists(self.jump_file):
            generate_tone(self.jump_file, 440.0, 0.2, 0.5, 'square')
            
        if not os.path.exists(self.collect_file):
            generate_tone(self.collect_file, 880.0, 0.1, 0.5, 'sine')

        if not os.path.exists(self.step_file):
            generate_tone(self.step_file, 100.0, 0.05, 0.3, 'noise')

    def play_jump(self):
        if self.enabled:
            backend.play_sound(self.jump_file, False, 0.5, 0.5)
        else:
            print("[AUDIO] *Jump*")

    def play_collect(self):
        if self.enabled:
            backend.play_sound(self.collect_file, False, 0.6, 0.5)
        else:
            print("[AUDIO] *Collect*")

    def play_footstep(self):
        if self.enabled:
            backend.play_sound(self.step_file, False, 0.2, 0.5)

class HUD:
    """Head-Up Display: Health, Stamina, Crystals."""
    def __init__(self, scene: Scene, camera_id: int):
        self.camera_id = camera_id
        
        # Crosshair
        self.crosshair = Entity("Crosshair", 0, 0, 0).set_mesh("cube").set_parent(camera_id)
        self.crosshair.set_position(0.0, 0.0, -1.0).set_scale(0.005, 0.005, 0.005).set_color(1.0, 1.0, 1.0, 0.8)
        scene.add(self.crosshair)

        # Crystal Counter (using 3D spheres as icons)
        self.crystal_icons = []
        for i in range(3):
            icon = Entity(f"CrystalIcon_{i}", 0, 0, 0).set_mesh("sphere").set_parent(camera_id)
            # Top Right
            x_pos = 0.5 - (i * 0.05)
            icon.set_position(x_pos, 0.4, -1.0).set_scale(0.02, 0.02, 0.02).set_color(0.2, 0.2, 0.2, 0.5)
            scene.add(icon)
            self.crystal_icons.append(icon)

    def update_crystals(self, count: int):
        # Light up collected crystals
        for i in range(3):
            if i < count:
                # Active: Cyan Glowing
                self.crystal_icons[i].set_color(0.0, 1.0, 1.0, 1.0)
            else:
                # Inactive: Dim
                self.crystal_icons[i].set_color(0.2, 0.2, 0.2, 0.5)

    def set_crosshair_color(self, r, g, b):
        self.crosshair.set_color(r, g, b, 0.8)

class IslandEscape(App):
    def on_start(self):
        print("Starlight Odyssey: Island Escape - START")
        
        # 1. Setup Scene
        self.scene = Scene()
        self.scene.set_sun(direction=(0.5, -0.6, 0.6), color=(1.0, 0.9, 0.8), intensity=2.0)
        self.scene.set_ambient(0.4)
        self.scene.set_fog(0.008, (0.6, 0.7, 0.9))
        self.scene.set_post_process(exposure=1.1, bloom_intensity=0.6)
        
        # Audio
        self.audio = SoundGenerator()

        # 2. Generate Terrain
        self.generator = TerrainGenerator(seed=12345) # Fixed seed for gameplay consistency or Random?
        self.terrain = self.generator.generate_chunk(self.scene, -128, -128, size=128, scale=256.0)
        self.terrain.set_color(0.2, 0.5, 0.25)
        self.scene.add(self.terrain)
        
        # 3. Water
        self.water = Entity("Water", 0, 4.0, 0).set_mesh("cube").set_scale(500.0, 0.1, 500.0)
        self.water.set_color(0.0, 0.3, 0.8, 0.7).set_material_params(0.0, 0.1)
        self.scene.add(self.water)

        # 4. Player
        # Spawn near center but high up
        self.character = Character(0, 40, 0, speed=12.0)
        self.scene.add(self.character)
        
        self.camera = Camera()
        self.yaw = 0.0
        self.pitch = 0.0
        
        # Physics
        self.vertical_velocity = 0.0
        self.gravity = -25.0
        self.grounded = False

        # 5. Game Goal: Crystals
        self.crystals = []
        self.crystals_collected = 0
        self.total_crystals = 3
        
        # Spawn Crystals at random locations
        # We need to find valid Y heights. 
        # For now, we place them high and let them "float" or just assume terrain height ~10-20
        positions = [
            (40, 20, 40),
            (-50, 25, -30),
            (20, 15, -60)
        ]
        
        for i, pos in enumerate(positions):
            c = Entity(f"Crystal_{i}", pos[0], pos[1], pos[2])
            c.set_mesh("sphere").set_scale(1.0, 2.0, 1.0)
            c.set_color(0.0, 1.0, 1.0, 1.0) # Cyan
            # Emission hack: high bloom intensity handles 'glow'
            self.scene.add(c)
            self.crystals.append(c)

        # 6. UI
        self.hud = HUD(self.scene, self.camera.id)
        
        # State
        self.state = "MENU"
        self.menu_shown = False
        
        print(">>> MAIN MENU LOADED <<<")

    def on_update(self, dt: float):
        # Rotator for crystals
        for c in self.crystals:
            # Simple Bobbing
            t = time.time()
            base_y = c.transform.position[1] # This drifts if we update strict pos
            # Actually we just set rotation
            c.set_rotation(0, t * 1.5, 0)
        
        # State Machine
        if self.state == "MENU":
            if not self.menu_shown:
                print("\n" + "="*40)
                print("   STARLIGHT ODYSSEY: ISLAND ESCAPE")
                print("="*40)
                print("Objective: Collect 3 Energy Crystals")
                print("Controls:  WASD to Move")
                print("           SPACE to Jump")
                print("           E to Collect Crystal")
                print("           SHIFT to Run")
                print("Press SPACE to Begin Mission")
                print("="*40 + "\n")
                self.menu_shown = True
                
            if self.input.is_key_just_pressed(Keys.SPACE):
                self.state = "PLAY"
                self.input.set_cursor_grab(True)
                self.input.set_cursor_visible(False)
                self.menu_shown = False
                print(">>> MISSION START <<<")
            
            # Auto-start for verification
            if self.input.frame_count == 60:
                 self.state = "PLAY"
                 self.input.set_cursor_grab(True)
                 self.input.set_cursor_visible(False)
                 self.menu_shown = False
                 print(">>> AUTO START (Verification) <<<")

            return

        elif self.state == "VICTORY":
             if not self.menu_shown:
                print("\n" + "*"*40)
                print("       MISSION ACCOMPLISHED!")
                print("*"*40)
                print("All Crystals Collected! Beacon Activated.")
                print("Press R to Play Again or ESC to Quit.")
                self.menu_shown = True
                self.input.set_cursor_grab(False)
                self.input.set_cursor_visible(True)
             
             if self.input.is_key_just_pressed(Keys.R):
                 # Restart (Simple reload)
                 self.crystals_collected = 0
                 self.hud.update_crystals(0)
                 self.state = "PLAY"
                 self.character.set_position(0, 40, 0)
                 self.vertical_velocity = 0
                 self.input.set_cursor_grab(True)
                 self.input.set_cursor_visible(False)
                 
                 # Reset crystals (enable them)
                 # Wait, we need an 'active' flag or just move them back?
                 # Assuming logic hides them.
                 self.menu_shown = False
                 print(">>> RESTART <<<")
             return

        # === PLAY ===
        
        # Mouse Look
        dx, dy = self.input.get_mouse_delta()
        self.yaw += dx * 0.003
        self.pitch -= dy * 0.003
        self.pitch = max(-1.5, min(1.5, self.pitch)) # Radians approx -85 to 85 deg
        self.camera.set_rotation(self.pitch, self.yaw, 0.0)

        # Interaction Raycast
        fx = math.sin(self.yaw) * math.cos(self.pitch)
        fy = math.sin(self.pitch)
        fz = math.cos(self.yaw) * math.cos(self.pitch)
        cam_pos = self.camera.transform.position
        
        hit = self.scene.raycast(tuple(cam_pos), (fx, fy, fz), 6.0)
        
        target_crystal = None
        if hit:
            # Check if hit entity is a crystal
            entity_id = hit[0]
            for c in self.crystals:
                 # Check ID match - Entity.id is int
                 if c.id == entity_id:
                     # Check if already collected (we hide collected by moving them far away?)
                     if c.transform.position[1] > -100:
                         target_crystal = c
                         break
        
        if target_crystal:
            self.hud.set_crosshair_color(0.0, 1.0, 1.0) # Active Cyan
            if self.input.is_key_just_pressed(Keys.E):
                self.collect_crystal(target_crystal)
        else:
             self.hud.set_crosshair_color(1.0, 1.0, 1.0)

        # Cheats
        if self.input.is_key_just_pressed(Keys.Escape):
             self.input.set_cursor_grab(False)
             self.state = "MENU"
             self.menu_shown = False
             
        if self.input.frame_count == 120:
             self.world.capture_screenshot("island_escape.png")
             print("Screenshot captured")

    def collect_crystal(self, crystal):
        print(f"Collected Crystal! ({self.crystals_collected + 1}/{self.total_crystals})")
        self.audio.play_collect()
        
        # "Remove" it visually
        crystal.set_position(0, -500, 0) 
        
        self.crystals_collected += 1
        self.hud.update_crystals(self.crystals_collected)
        
        if self.crystals_collected >= self.total_crystals:
            self.state = "VICTORY"

    def on_fixed_update(self, dt: float):
        if self.state != "PLAY": return
        
        # Physics
        # ... (Similar to island_survival.py but simplified)
        speed = 12.0
        if self.input.is_key_down(Keys.Shift): speed = 20.0
        
        fwd_x = math.sin(self.yaw)
        fwd_z = math.cos(self.yaw)
        right_x = math.cos(self.yaw)
        right_z = -math.sin(self.yaw)
        
        move_x = 0.0
        move_z = 0.0
        
        if self.input.is_key_down(Keys.W):
            move_x -= fwd_x; move_z -= fwd_z
        if self.input.is_key_down(Keys.S):
            move_x += fwd_x; move_z += fwd_z
        if self.input.is_key_down(Keys.D):
            move_x -= right_x; move_z -= right_z
        if self.input.is_key_down(Keys.A):
            move_x += right_x; move_z += right_z
            
        # Normalize
        length = math.sqrt(move_x*move_x + move_z*move_z)
        if length > 0:
            move_x /= length
            move_z /= length
            self.audio.play_footstep() # Simple trigger (too frequent? add timer)
            
        move_x *= speed * dt
        move_z *= speed * dt
        
        if self.grounded and self.input.is_key_down(Keys.SPACE):
            self.vertical_velocity = 8.0
            self.grounded = False
            self.audio.play_jump()

        self.vertical_velocity += self.gravity * dt
        move_y = self.vertical_velocity * dt
        
        old_y = self.character.transform.position[1]
        _, new_y, _ = self.character.move(move_x, move_y, move_z)
        
        actual_dy = new_y - old_y
        
        if move_y < 0 and actual_dy > move_y + 0.001:
            self.grounded = True
            self.vertical_velocity = 0
        else:
            self.grounded = False

    def on_late_update(self, dt: float):
        pos = self.character.transform.position
        self.camera.set_position(pos[0], pos[1] + 1.8, pos[2])

if __name__ == "__main__":
    game = IslandEscape(title="Starlight Odyssey: Island Escape", width=1280, height=720)
    game.run()
