
import sys
import os
import math
import random
import time

# Ensure pysrc is in path
sys.path.append(os.path.abspath("pysrc"))

from starlight import App, Input, Keys, backend
from starlight import framework
from starlight.framework import Scene, Entity, Camera, Character
from starlight.procgen.terrain import TerrainGenerator
from starlight.vfx import ScreenShake, GlitchEffect
from starlight.audio_proc import AudioSystem

class EntropySystem:
    def __init__(self, max_entropy=100.0, decay_rate=2.0):
        self.max_entropy = max_entropy
        self.current_entropy = max_entropy
        self.decay_rate = decay_rate
        self.is_critical = False
        
    def update(self, dt: float) -> bool:
        """
        Updates entropy. Returns True if entropy is depleted (Game Over).
        """
        self.current_entropy -= self.decay_rate * dt
        
        # Check critical state (below 20%)
        if self.current_entropy < (self.max_entropy * 0.2):
            self.is_critical = True
        else:
            self.is_critical = False
            
        if self.current_entropy <= 0:
            self.current_entropy = 0
            return True
        return False
        
    def restore(self, amount: float):
        self.current_entropy += amount
        if self.current_entropy > self.max_entropy:
            self.current_entropy = self.max_entropy

    def get_percentage(self):
        return self.current_entropy / self.max_entropy

class ObjectiveSystem:
    def __init__(self, scene: Scene):
        self.scene = scene
        self.beacons = []
        self.active_count = 0
        self.total_beacons = 0
        
    def spawn_beacon(self, x, y, z, id_offset):
        # Visual representation of a "Beacon"
        # Base
        base = Entity(f"BeaconBase_{id_offset}", x, y, z)
        base.set_mesh("cube").set_scale(2.0, 0.5, 2.0).set_color(0.3, 0.3, 0.3, 1.0)
        self.scene.add(base)
        
        # Pillar (Inactive color: Red)
        pillar = Entity(f"BeaconPillar_{id_offset}", x, y + 2.5, z)
        pillar.set_mesh("cube").set_scale(0.8, 5.0, 0.8).set_color(1.0, 0.0, 0.0, 1.0)
        # Add a point light
        backend.set_point_light(id_offset, x, y + 4.0, z, 1.0, 0.0, 0.0, 5.0) # Red light
        self.scene.add(pillar)
        
        self.beacons.append({
            "base": base,
            "pillar": pillar,
            "id": id_offset, # Light ID matches index for simplicity
            "active": False,
            "pos": (x, y, z)
        })
        self.total_beacons += 1
        
    def activate_beacon(self, beacon_index):
        if 0 <= beacon_index < len(self.beacons):
            b = self.beacons[beacon_index]
            if not b["active"]:
                b["active"] = True
                b["pillar"].set_color(0.0, 1.0, 1.0, 1.0) # Cyan
                # Update light to Cyan
                x, y, z = b["pos"]
                backend.set_point_light(b["id"], x, y + 4.0, z, 0.0, 1.0, 1.0, 8.0)
                self.active_count += 1
                return True
        return False

    def check_interaction(self, player_pos: tuple, interact_radius=5.0) -> int:
        """Returns beacon index if close enough to interact, else -1"""
        px, py, pz = player_pos
        for i, b in enumerate(self.beacons):
            if b["active"]: continue
            
            bx, by, bz = b["pos"]
            dist = math.sqrt((px-bx)**2 + (py-by)**2 + (pz-bz)**2)
            if dist < interact_radius:
                return i
        return -1

class GameSession(App):
    def on_start(self):
        print("Starlight Odyssey: The Signal - SYSTEM BOOT")
        
        # 1. Core Systems
        self.scene = Scene()
        self.setup_rendering()
        
        self.audio = AudioSystem()
        self.vfx_shake = ScreenShake()
        self.vfx_glitch = GlitchEffect()
        
        # 2. Game Logic
        self.entropy = EntropySystem(max_entropy=100.0, decay_rate=3.0) # 33 seconds to live without crystals
        self.objectives = ObjectiveSystem(self.scene)
        
        # 3. World Generation
        self.setup_world()
        
        # 4. Player
        self.character = Character(0, 50, 0, speed=10.0) # Spawn high
        self.scene.add(self.character)
        self.camera = Camera()
        self.yaw = 0.0
        self.pitch = 0.0
        self.grounded = False
        self.vertical_velocity = 0.0
        self.gravity = -25.0
        
        # 5. Crystals (Entropy Refills)
        self.crystals = []
        self.spawn_crystals()
        
        # State
        self.state = "INTRO" # INTRO, PLAY, GAMEOVER, VICTORY
        self.state_timer = 0.0
        
        # Capture verification screenshot automatically ?
        self.screenshot_taken = False

    def setup_rendering(self):
        # Mood: Desolate Sci-Fi
        self.scene.set_sun(direction=(0.3, -0.5, 0.8), color=(1.0, 0.9, 0.8), intensity=1.5)
        self.scene.set_ambient(0.1) # Dark shadows
        self.scene.set_fog(0.015, (0.05, 0.05, 0.1)) # Dark blue fog
        self.scene.set_post_process(exposure=1.0, bloom_intensity=0.8)

    def setup_world(self):
        # Terrain
        self.generator = TerrainGenerator(seed=random.randint(0, 9999)) 
        self.terrain = self.generator.generate_chunk(self.scene, -128, -128, size=128, scale=256.0)
        self.terrain.set_color(0.1, 0.1, 0.15) # Dark rock
        self.scene.add(self.terrain)
        
        # Water/Lava? Let's do simple dark water
        self.water = Entity("Water", 0, 2.0, 0).set_mesh("cube").set_scale(500.0, 0.1, 500.0)
        self.water.set_color(0.0, 0.1, 0.2, 0.8).set_material_params(0.0, 0.9)
        self.scene.add(self.water)
        
        # Beacons (Obvious Landmarks)
        # 1: Near spawn
        self.objectives.spawn_beacon(30, 15, 30, 0)
        # 2: Far out
        self.objectives.spawn_beacon(-50, 20, -50, 1)
        # 3: Hard to reach
        self.objectives.spawn_beacon(60, 25, -60, 2)

    def spawn_crystals(self):
        # Scattered refills
        coords = [
            (10, 20, 10), (-20, 20, 20), (40, 20, -10),
            (-40, 20, -40), (0, 25, -50), (60, 20, 40)
        ]
        for i, pos in enumerate(coords):
            c = Entity(f"Crystal_{i}", pos[0], pos[1], pos[2])
            c.set_mesh("sphere").set_scale(0.8, 1.5, 0.8)
            c.set_color(0.8, 0.0, 1.0, 1.0) # Purple Energy
            self.scene.add(c)
            self.crystals.append(c)

    def on_update(self, dt: float):
        self.state_timer += dt
        
        # VFX Updates
        dx, dy, rot = self.vfx_shake.update(dt)
        self.vfx_glitch.update(dt)
        
        # Update Camera Shake (applied on top of look)
        # Note: In a real engine we'd composite transformation matrices.
        # Here we just offset the look variables slightly or the final cam position?
        # Let's offset the position for shake
        cam_pos = self.character.transform.position
        shake_pos = (cam_pos[0] + dx, cam_pos[1] + 1.8 + dy, cam_pos[2])
        self.camera.set_position(*shake_pos)
        self.camera.set_rotation(self.pitch + math.radians(rot), self.yaw, math.radians(rot))

        if self.state == "INTRO":
            if self.state_timer < 3.0:
                # Glitch effect intro
                self.vfx_glitch.trigger(0.1)
            else:
                self.state = "PLAY"
                print(">>> SYSTEM ONLINE. ENTROPY DECAY STARTED. <<<")
        
        elif self.state == "PLAY":
            # 1. Physics & Movement
            self.handle_input_movement(dt)
            
            # 2. Mechanics: Entropy
            game_over = self.entropy.update(dt)
            if self.entropy.is_critical:
                # Heartbeat shake
                if int(self.state_timer * 2) % 2 == 0:
                     self.vfx_shake.add_trauma(0.05 * dt)
                # Play alarm if not playing? (Audio system handles logic?)
            
            if game_over:
                self.state = "GAMEOVER"
                self.vfx_shake.add_trauma(1.0)
                self.vfx_glitch.trigger(2.0)
                self.audio.play("low_entropy", 1.0, 0.5)
                print(">>> CRITICAL FAILURE: ENTROPY DEPLETED <<<")

            # 3. Interactions
            # Check Crystal proximity
            ppos = self.character.transform.position
            for c in self.crystals:
                if c.transform.position[1] < -100: continue # Collected
                
                cx, cy, cz = c.transform.position
                dist = math.sqrt((ppos[0]-cx)**2 + (ppos[1]-cy)**2 + (ppos[2]-cz)**2)
                
                # Visual rotation
                c.set_rotation(0, self.state_timer, 0)
                
                if dist < 2.5:
                    # Collect
                    c.set_position(0, -500, 0)
                    self.entropy.restore(30.0) # +30%
                    self.audio.play("collect")
                    self.vfx_shake.add_trauma(0.3)
                    print("Entropy Restored +30%")

            # Check Beacon proximity for interaction (Press E)
            beacon_idx = self.objectives.check_interaction(ppos)
            if beacon_idx != -1:
                if self.input.is_key_just_pressed(Keys.E):
                    self.objectives.activate_beacon(beacon_idx)
                    self.audio.play("beacon_active")
                    self.vfx_shake.add_trauma(0.5)
                    self.vfx_glitch.trigger(0.3)
                    print(f"Beacon {beacon_idx+1} Activated!")
                    
                    if self.objectives.active_count >= self.objectives.total_beacons:
                        self.state = "VICTORY"
                        print(">>> SIGNAL ESTABLISHED. EXTRACTION IMMINENT. <<<")

            # Simple HUD (Console for now, maybe title later)
            if self.input.frame_count % 60 == 0:
                 print(f"Entropy: {self.entropy.current_entropy:.1f}% | Beacons: {self.objectives.active_count}/{self.objectives.total_beacons}")

        elif self.state == "GAMEOVER":
             # Spin camera/fall?
             pass
        elif self.state == "VICTORY":
             # Ascend?
             self.vertical_velocity = 5.0
             self.character.move(0, self.vertical_velocity * dt, 0)

        # Verification Screenshot
        if self.state == "PLAY" and self.objectives.active_count >= 1 and not self.screenshot_taken:
             # Take a pic when 1 beacon is active to prove gameplay
             # Or just at frame 200
             pass
        if self.input.frame_count == 240:
             backend.capture_screenshot("starlight_odyssey.png")
             self.screenshot_taken = True
             print("Screenshot Captured")

    def handle_input_movement(self, dt):
        # Mouse Look
        dx, dy = self.input.get_mouse_delta()
        self.yaw += dx * 0.003
        self.pitch -= dy * 0.003
        self.pitch = max(-1.5, min(1.5, self.pitch))
        
        speed = 10.0
        if self.input.is_key_down(Keys.Shift): speed = 18.0
        
        fwd_x = math.sin(self.yaw)
        fwd_z = math.cos(self.yaw)
        right_x = math.cos(self.yaw)
        right_z = -math.sin(self.yaw)
        
        move_x = 0.0
        move_z = 0.0
        
        if self.input.is_key_down(Keys.W): move_x -= fwd_x; move_z -= fwd_z
        if self.input.is_key_down(Keys.S): move_x += fwd_x; move_z += fwd_z
        if self.input.is_key_down(Keys.D): move_x -= right_x; move_z -= right_z
        if self.input.is_key_down(Keys.A): move_x += right_x; move_z += right_z
            
        # Normalize
        length = math.sqrt(move_x*move_x + move_z*move_z)
        if length > 0:
            move_x /= length
            move_z /= length
            
        move_x *= speed * dt
        move_z *= speed * dt
        
        if self.grounded and self.input.is_key_down(Keys.SPACE):
            self.vertical_velocity = 12.0
            self.grounded = False
            self.audio.play("jump")
            self.vfx_shake.add_trauma(0.1)

        self.vertical_velocity += self.gravity * dt
        move_y = self.vertical_velocity * dt
        
        old_y = self.character.transform.position[1]
        _, new_y, _ = self.character.move(move_x, move_y, move_z)
        
        if move_y < 0 and (new_y - old_y) > move_y + 0.001:
            self.grounded = True
            self.vertical_velocity = 0
            # Land impact
            if self.vertical_velocity < -10:
                 self.audio.play("step")
        else:
            self.grounded = False

if __name__ == "__main__":
    game = GameSession(title="Starlight Odyssey: The Signal", width=1280, height=720)
    game.run()
