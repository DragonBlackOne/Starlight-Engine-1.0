import math
import time
import random

# Starlight Engine Imports
# Assuming starlight is correctly installed in pysrc
import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "pysrc")))

import starlight
from starlight import backend
from starlight.audio import AudioManager
from starlight.framework import Scene, Camera
from starlight.procgen.terrain import TerrainGenerator

# Game States
STATE_INTRO = 0
STATE_PLAY = 1
STATE_GAMEOVER = 2
STATE_VICTORY = 3

class HUD:
    def __init__(self):
        self.message = ""
        self.sub_message = ""
        self.message_time = 0.0
        self.sub_message_time = 0.0
        
    def show_message(self, msg, duration=3.0):
        self.message = msg
        self.message_time = time.time() + duration
        print(f"[HUD] Message: {msg}")

    def show_sub(self, msg, duration=3.0):
        self.sub_message = msg
        self.sub_message_time = time.time() + duration
        print(f"[HUD] Sub: {msg}")

    def render(self):
        if self.message:
            # Centered Text (Approximate)
            # Assuming backend.draw_text(x, y, text, size, color)
            # White text
            backend.draw_text(400, 300, self.message, 32.0, (1.0, 1.0, 1.0, 1.0))
            
        if self.sub_message:
            # Subtitle below
            backend.draw_text(400, 350, self.sub_message, 24.0, (0.8, 0.8, 0.8, 1.0))
            
        # Debug Stats
        backend.draw_text(10, 10, "FPS: 60", 16.0, (0.0, 1.0, 0.0, 1.0))

class GameSession:
    def __init__(self, hud):
        self.hud = hud
        self.state = STATE_INTRO
        self.entropy = 100.0
        self.max_entropy = 100.0
        self.beacons_activated = 0
        self.total_beacons = 3
        self.start_time = time.time()
        
        # Initial Message
        self.hud.show_message("Starlight Odyssey: The Signal", 10.0)
        self.hud.show_sub("Press SPACE to Begin", 10.0)
        
    def start_game(self):
        self.state = STATE_PLAY
        self.entropy = 100.0
        self.beacons_activated = 0
        self.start_time = time.time()
        self.hud.show_message("SIGNAL LOST - ENTROPY CRITICAL", 3.0)
        self.hud.show_sub("Find 3 Beacons to re-establish contact.", 5.0)
        print("Game Started: entropy stabilizing...")

    def update(self, dt):
        self.hud.update()
        
        if self.state == STATE_PLAY:
            # Decay Mechanics
            self.entropy -= 2.0 * dt # Lose 2 entropy per second
            
            # Clamp
            self.entropy = max(0.0, min(100.0, self.entropy))

            if self.entropy <= 0:
                self.state = STATE_GAMEOVER
                self.hud.show_message("SIGNAL LOST", 10.0)
                self.hud.show_sub("Entropy consumed you. Press R to Retry.", 10.0)
                print("Game Over: Signal Lost")
            elif self.beacons_activated >= self.total_beacons:
                self.state = STATE_VICTORY
                self.hud.show_message("SIGNAL ESTABLISHED", 10.0)
                self.hud.show_sub("Rescue is incoming. Press R to Play Again.", 10.0)
                print("Victory: Signal Established")

class Watcher:
    def __init__(self, scene, terrain, x, z, patrol_radius=30.0):
        self.scene = scene
        self.terrain = terrain
        self.start_x = x
        self.start_z = z
        self.patrol_radius = patrol_radius
        
        # Entity Setup
        y = self.terrain.get_height(x, z) + 3.0
        self.entity = starlight.framework.Entity(f"Watcher_{id(self)}", x, y, z)
        # Visual: Floating Eye
        self.entity.set_mesh("sphere").set_material("metal", "flat_normal")
        self.entity.set_scale(1.0, 1.0, 1.0)
        self.scene.add(self.entity)
        
        # Light (Evil Eye)
        # We need a unique ID for the light. Let's assume we reserved 10+ for enemies
        self.light_id = 10 + int(random.random() * 1000) 
        backend.set_point_light(self.light_id, x, y, z, 2.0, 0.0, 0.0, 10.0)
        
        # AI State
        self.state = "PATROL" # PATROL, CHASE
        self.target_pos = [x, z]
        self.pick_new_patrol_point()
        self.speed = 5.0
        self.chase_speed = 9.0
        self.detection_radius = 25.0
        self.contact_radius = 2.0
        
    def pick_new_patrol_point(self):
        angle = random.random() * 6.28
        dist = random.random() * self.patrol_radius
        self.target_pos[0] = self.start_x + math.cos(angle) * dist
        self.target_pos[1] = self.start_z + math.sin(angle) * dist
        
    def update(self, dt, player_pos, session):
        # Current Pos
        curr_x = self.entity.transform.position[0]
        curr_z = self.entity.transform.position[2]
        
        dist_to_player = math.sqrt((curr_x - player_pos[0])**2 + (curr_z - player_pos[2])**2)
        
        # State Machine
        if self.state == "PATROL":
            if dist_to_player < self.detection_radius:
                self.state = "CHASE"
                print("Watcher: Target Acquired!")
            
            # Move to patrol point
            dx = self.target_pos[0] - curr_x
            dz = self.target_pos[1] - curr_z
            dist = math.sqrt(dx*dx + dz*dz)
            
            if dist < 1.0:
                self.pick_new_patrol_point()
            else:
                dir_x = dx / dist
                dir_z = dz / dist
                curr_x += dir_x * self.speed * dt
                curr_z += dir_z * self.speed * dt
                
        elif self.state == "CHASE":
            if dist_to_player > self.detection_radius * 1.5:
                self.state = "PATROL"
                print("Watcher: Target Lost.")
            else:
                # Move to player
                dx = player_pos[0] - curr_x
                dz = player_pos[2] - curr_z
                dist = math.sqrt(dx*dx + dz*dz)
                
                if dist > 0.5:
                    dir_x = dx / dist
                    dir_z = dz / dist
                    curr_x += dir_x * self.chase_speed * dt
                    curr_z += dir_z * self.chase_speed * dt
                    
                # Damage
                if dist_to_player < self.contact_radius:
                    session.entropy -= 15.0 * dt # Massive damage
                    session.hud.show_sub("WARNING: PROXIMITY ALERT", 1.0)

        # Update Height (Hover)
        ground_y = self.terrain.get_height(curr_x, curr_z)
        hover_height = 3.0 + math.sin(time.time() * 2.0) * 0.5
        curr_y = ground_y + hover_height
        
        self.entity.set_position(curr_x, curr_y, curr_z)
        
        # Update Light
        # Pulse intensity if chasing
        intensity = 2.0
        if self.state == "CHASE":
            intensity = 5.0 + math.sin(time.time() * 15.0) * 5.0
            
        backend.set_point_light(self.light_id, curr_x, curr_y, curr_z, intensity, 0.0, 0.0, 15.0)

class OdysseyGame(starlight.App):
    def __init__(self):
        is_headless = "--headless" in sys.argv
        super().__init__("Starlight Odyssey: The Signal", 1280, 720, headless=is_headless)
        self.hud = HUD()
        self.session = GameSession(self.hud)
        self.camera_pitch = 0.0
        self.camera_yaw = -1.57 # Look towards +X
        self.player_pos = [0.0, 50.0, 0.0] # High start to not fall through immediately
        self.rot_speed = 0.002
        self.move_speed = 25.0 # Increased for better feel
        
        # Audio
        try:
            self.audio = AudioManager()
            self.audio.start_music("assets/audio/music/ambient_loop.mp3", 0.5)
        except Exception as e:
            print(f"Audio Warning: {e}")
            self.audio = None
        self.background_music = None

    def on_start(self):
        print("Initializing Starlight Odyssey...")
        
        # Setup Scene
        self.scene = Scene()
        
        # Sun: Warm (Golden Hour), Intensity 3.0
        self.scene.set_sun((0.5, -0.8, 0.2), (1.0, 0.9, 0.7), 3.0)
        # Ambient: Dark Blue (Scary), Intensity 0.05
        self.scene.set_ambient(0.05)
        # Fog: Dense, alien atmosphere
        self.scene.set_fog(0.02, (0.02, 0.02, 0.05))

        # Camera
        self.main_camera = Camera() # Default to main camera ID
        self.main_camera.set_position(*self.player_pos)
        self.main_camera.set_rotation(self.camera_pitch, self.camera_yaw, 0.0)

        # Terrain Generation
        self.generator = TerrainGenerator(seed=int(time.time()))
        print("Generating Terrain...")
        # Generate a 256x256 chunk centered roughly at 0,0 (offset -128, -128)
        self.terrain = self.generator.generate_chunk(self.scene, -128, -128, size=128, scale=256.0)
        self.terrain.entity.set_color(0.2, 0.2, 0.2) # Dark alien ground

        # Entities (Beacons)
        # Using "crystal" material if available (emission), else "metal"
        # We will assume "beacon_on" and "beacon_off" materials can be swapped or just color tint loop
        
        # Beacon 1
        # Beacon 1
        b1_x, b1_z = 50.0, 50.0
        b1_y = self.terrain.get_height(b1_x, b1_z)
        self.beacon1 = starlight.framework.Entity("beacon_1", b1_x, b1_y, b1_z)
        self.beacon1.set_scale(2.0, 10.0, 2.0).set_mesh("cube").set_material("metal", "flat_normal")
        self.scene.add(self.beacon1)
        backend.set_point_light(1, b1_x, b1_y + 5.0, b1_z, 1.0, 0.0, 0.0, 20.0)

        # Beacon 2
        b2_x, b2_z = -50.0, 50.0
        b2_y = self.terrain.get_height(b2_x, b2_z)
        self.beacon2 = starlight.framework.Entity("beacon_2", b2_x, b1_y, b2_z)
        self.beacon2.set_scale(2.0, 10.0, 2.0).set_mesh("cube").set_material("metal", "flat_normal")
        self.scene.add(self.beacon2)
        backend.set_point_light(2, b2_x, b2_y + 5.0, b2_z, 0.0, 1.0, 0.0, 20.0)

        # Beacon 3
        b3_x, b3_z = 0.0, -50.0
        b3_y = self.terrain.get_height(b3_x, b3_z)
        self.beacon3 = starlight.framework.Entity("beacon_3", b3_x, b3_y, b3_z)
        self.beacon3.set_scale(2.0, 10.0, 2.0).set_mesh("cube").set_material("metal", "flat_normal")
        self.scene.add(self.beacon3)
        backend.set_point_light(3, b3_x, b3_y + 5.0, b3_z, 0.0, 0.0, 1.0, 20.0)

        # Spawn Watchers
        self.watchers = []
        # One near each beacon?
        self.watchers.append(Watcher(self.scene, self.terrain, 30.0, 30.0))
        self.watchers.append(Watcher(self.scene, self.terrain, -30.0, 30.0))
        self.watchers.append(Watcher(self.scene, self.terrain, 0.0, -30.0))
        # And a roamer near start
        self.watchers.append(Watcher(self.scene, self.terrain, 10.0, 10.0))

        # Player Light (Flashlight)
        # Point Light 0 attached to player
        # self.scene.set_point_light(0, self.player_pos, (1.0, 1.0, 1.0), 10.0, 20.0) 
        
        current_time = time.time()
        
        # Calculate Visual Effects based on Entropy/Stress
        aberration = 0.0
        if self.session.entropy > 50:
            aberration = (self.session.entropy - 50) / 50.0 * 5.0 # Up to 5.0 strength
        
        # Apply Post Process (Exposure, Gamma, BloomInt, BloomThr, Aberration, Time)
        backend.set_post_process_params(1.0, 2.2, 0.8, 1.0, aberration, current_time)

        # Beacon Lights logic
        # HUD Setup
        self.ui_camera = Camera(id=None)

    def on_update(self, dt):

        input = self.input
        
        # State Machine Input
        if self.session.state == STATE_INTRO:
            if input.is_key_down("Space"):
                self.session.start_game()
        
        elif self.session.state == STATE_GAMEOVER or self.session.state == STATE_VICTORY:
            if input.is_key_down("R"):
                self.session.start_game()

        elif self.session.state == STATE_PLAY:
            # Player Movement (FPS)
            # Yaw is rotation around Y axis. 0 = looking down -Z?
            # cos(yaw) -> Z, sin(yaw) -> X
            
            forward_x = math.sin(self.camera_yaw)
            forward_z = math.cos(self.camera_yaw)
            
            right_x = math.cos(self.camera_yaw)
            right_z = -math.sin(self.camera_yaw)
            
            move_x = 0.0
            move_z = 0.0
            
            if input.is_key_down("W"):
                move_x += forward_x
                move_z += forward_z
            if input.is_key_down("S"):
                move_x -= forward_x
                move_z -= forward_z
            if input.is_key_down("A"):
                move_x -= right_x
                move_z -= right_z
            if input.is_key_down("D"):
                move_x += right_x
                move_z += right_z

            # Normalize diagonal
            length = math.sqrt(move_x**2 + move_z**2)
            if length > 0.001:
                move_x /= length
                move_z /= length
                
            self.player_pos[0] += move_x * self.move_speed * dt
            self.player_pos[2] += move_z * self.move_speed * dt
            
            # Simple Gravity/Floor Clamp
            ground_y = self.terrain.get_height(self.player_pos[0], self.player_pos[2])
            self.player_pos[1] = max(self.player_pos[1], ground_y + 5.0) # Walking height (5.0 eye level?)
            # Actually let's make it follow terrain
            self.player_pos[1] = ground_y + 3.0
            
            # Mouse Look
            # We need to get delta. Assuming input.mouse_delta returns (dx, dy)
            # This API might need verification from 'core-input-manager'
            dx, dy = input.get_mouse_delta()
            self.camera_yaw += dx * self.rot_speed
            self.camera_pitch -= dy * self.rot_speed
            self.camera_pitch = max(-1.5, min(1.5, self.camera_pitch))

            # Update Camera
            # Debug print every second
            # if int(time.time()) % 2 == 0:
            #     print(f"Pos: {self.player_pos}, Rot: {self.camera_yaw}")

            self.main_camera.set_position(*self.player_pos)
            # Camera Up vector extraction or quaternion logic in backend? 
            # Usually front vector. backend.look_at? 
            # Or set_camera_rot(pitch, yaw, roll)?
            self.main_camera.set_rotation(self.camera_pitch, self.camera_yaw, 0.0)
            
            # Update Game Session (Entropy)
            self.session.update(dt)
            
            # Check Interactions (Distance to Beacons)
            # Beacon 1
            if self.session.beacons_activated < 1:
                dist = math.sqrt((self.player_pos[0] - 50)**2 + (self.player_pos[2] - 50)**2)
                if dist < 8.0:
                    self.session.beacons_activated += 1
                    print("Beacon 1 Activated!")
                    # Turn on Light 1 (Red)
                    backend.set_point_light(1, 50.0, 5.0, 50.0, 5.0, 0.1, 0.1, 50.0)

            # Update Watchers
            for w in self.watchers:
                w.update(dt, self.player_pos, self.session)

            # Beacon 2
            if self.session.beacons_activated < 2: # Must be sequential? Or any order? Let's say sequential for now or check flags
                 # Actually let's use a bitmask or set for non-sequential if we want
                 pass 
            
            # Simple Distance Check for all (assuming sequential for simplicity in Phase 1)
            # Actually, let's allow any order but logic is easier if we check specific coords
            # TODO: Improve Beacon State tracking (Set of IDs)
            
            # Cheat Keys
            if input.is_key_down("1"): 
                self.session.beacons_activated = 1
                print("Cheat: Beacon 1 Active")
            if input.is_key_down("2"): 
                self.session.beacons_activated = 2
            if input.is_key_down("3"): 
                self.session.beacons_activated = 3

    def on_render(self):
        # UI Rendering
        self.hud.render()

if __name__ == "__main__":
    game = OdysseyGame()
    try:
        game.run()
    except Exception as e:
        print(f"Fatal Error: {e}")
        import traceback
        traceback.print_exc()
