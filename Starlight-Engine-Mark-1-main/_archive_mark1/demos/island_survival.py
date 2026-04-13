import sys
import os
sys.path.append(os.path.abspath("pysrc"))

from starlight import App, Input, Keys, backend
from starlight import framework
from starlight.framework import Scene, Entity, Camera, Character
from starlight.procgen.terrain import TerrainGenerator
from starlight.game.stat_system import StatSystem, Modifier, ModType
import math
import sys

class HUD:
    """Head-Up Display using 3D entities attached to camera."""
    def __init__(self, scene: Scene, camera_id: int):
        self.camera_id = camera_id
        
        # Health Bar (Background + Foreground)
        # Position relative to camera: Bottom Left (-0.5, -0.4, 1.0)
        self.health_bg = Entity("HealthBG", 0, 0, 0).set_mesh("cube").set_parent(camera_id)
        self.health_bg.set_position(-0.5, -0.4, 1.0).set_scale(0.3, 0.03, 0.01).set_color(0.1, 0.1, 0.1, 1.0)
        scene.add(self.health_bg)
        
        self.health_fg = Entity("HealthFG", 0, 0, 0).set_mesh("cube").set_parent(camera_id)
        # Slightly in front of BG (Z=0.99 relative to camera look? No, Local Z is depth. 1.0 is forward. So 0.99 is closer? No, further. Wait.
        # Front is -Z usually in OpenGL, but here we used 1.0 in update loop for crosshair?
        # Let's assume +Z is forward for parenting for now, or check generic camera.
        # Actually camera looks down -Z usually. So Z=-1.0 is in front.
        # Let's try Z=-1.0. If not visible, I'll flip.
        # Wait, previous update said "1.0". Let's stick to standard GL: -Z is forward.
        self.health_fg.set_position(-0.5, -0.4, -1.01).set_scale(0.29, 0.02, 0.01).set_color(0.8, 0.1, 0.1, 1.0)
        scene.add(self.health_fg)
        
        # Stamina Bar
        self.stamina_bg = Entity("StaminaBG", 0, 0, 0).set_mesh("cube").set_parent(camera_id)
        self.stamina_bg.set_position(-0.5, -0.45, -1.0).set_scale(0.25, 0.02, 0.01).set_color(0.1, 0.1, 0.1, 1.0)
        scene.add(self.stamina_bg)
        
        self.stamina_fg = Entity("StaminaFG", 0, 0, 0).set_mesh("cube").set_parent(camera_id)
        self.stamina_fg.set_position(-0.5, -0.45, -1.01).set_scale(0.24, 0.015, 0.01).set_color(0.8, 0.8, 0.1, 1.0)
        scene.add(self.stamina_fg)
        
        # Crosshair
        self.crosshair = Entity("Crosshair", 0, 0, 0).set_mesh("cube").set_parent(camera_id)
        self.crosshair.set_position(0.0, 0.0, -1.0).set_scale(0.005, 0.005, 0.005).set_color(1.0, 1.0, 1.0, 0.8)
        scene.add(self.crosshair)

    def update(self, health_pct: float, stamina_pct: float):
        # Scale X based on percentage
        # Initial scales: Health=0.29, Stamina=0.24
        
        # Health
        h_scale = max(0.0, 0.29 * health_pct)
        # To align left, we need to shift position as scale shrinks (since generic scale is from center)
        # Center = -0.5. Width = 0.29. Left edge = -0.5 - 0.145 = -0.645.
        # New Center = Left Edge + NewWidth/2 = -0.645 + h_scale/2
        self.health_fg.set_scale(h_scale, 0.02, 0.01)
        self.health_fg.set_position(-0.645 + h_scale*0.5, -0.4, -1.01)
        
        # Stamina
        s_scale = max(0.0, 0.24 * stamina_pct)
        # Left edge = -0.5 - 0.125 = -0.625
        self.stamina_fg.set_scale(s_scale, 0.015, 0.01)
        self.stamina_fg.set_position(-0.625 + s_scale*0.5, -0.45, -1.01)

class IslandSurvival(App):
    def on_start(self):
        print("Island Survival Demo Starting (FPS Mode)...")
        
        # 1. Setup Scene
        self.scene = Scene()
        self.scene.set_sun(direction=(0.5, -0.8, 0.4), color=(1.0, 0.95, 0.8), intensity=2.5)
        self.scene.set_ambient(0.3)
        self.scene.set_fog(0.005, (0.6, 0.7, 0.9))
        self.scene.set_post_process(exposure=1.0, bloom_intensity=0.4)
        
        # 2. Generate Terrain
        self.generator = TerrainGenerator(seed=67890)
        print("Generating Island Terrain...")
        self.terrain = self.generator.generate_chunk(self.scene, -128, -128, size=128, scale=256.0)
        self.terrain.set_color(0.2, 0.6, 0.3)
        self.scene.add(self.terrain)
        
        # 3. Water Plane
        self.water_visual = Entity("WaterVisual", 0, 4.0, 0)
        self.water_visual.set_mesh("cube")
        self.water_visual.set_scale(500.0, 0.1, 500.0)
        self.water_visual.set_color(0.0, 0.4, 0.9, 0.6)
        # Shiny water
        self.water_visual.set_material_params(0.0, 0.05)
        self.scene.add(self.water_visual)

        # 4. Player / Character
        # Start high to fall onto terrain (ensure Physics is working)
        # Using Character class wrapping backend.spawn_character
        self.character = Character(0, 50, 0, speed=10.0, max_slope=0.9, offset=0.5)
        self.scene.add(self.character)
        
        self.camera = Camera()
        self.yaw = 0.0
        self.pitch = 0.0
        
        # Physics State
        self.vertical_velocity = 0.0
        self.gravity = -30.0
        self.jump_speed = 10.0
        self.grounded = False
        
        # Game State
        self.state = "MENU" # MENU, PLAY, GAMEOVER
        
        # Stats
        self.stats = StatSystem()
        self.stats.add_stat("health", base=100.0, max_val=100.0)
        self.stats.add_stat("stamina", base=100.0, max_val=100.0)
        
        # UI
        self.hud = HUD(self.scene, self.camera.id)
        
        print(">>> PRESS SPACE TO START <<<")

    def on_update(self, dt: float):
        if self.state == "MENU":
            if self.input.is_key_just_pressed(Keys.SPACE):
                self.state = "PLAY"
                self.input.set_cursor_grab(True)
                self.input.set_cursor_visible(False)
                print("Game Started!")
            return

        if self.state == "GAMEOVER":
            if self.input.is_key_just_pressed(Keys.R):
                # Reset
                self.stats.set_base("health", 100.0)
                self.stats.set_base("stamina", 100.0)
                self.character.set_position(0, 50, 0)
                self.vertical_velocity = 0
                self.state = "PLAY"
                print("Game Restarted!")
            return

        # === PLAY STATE ===
        
        # Stats Logic
        # Regen Stamina
        stamina = self.stats.get("stamina")
        if stamina < 100.0:
            self.stats.set_base("stamina", stamina + 5.0 * dt)
            
        # Update HUD
        hp_pct = self.stats.get("health") / 100.0
        st_pct = self.stats.get("stamina") / 100.0
        self.hud.update(hp_pct, st_pct)
        
        # Check Death
        if self.stats.get("health") <= 0:
            self.state = "GAMEOVER"
            self.input.set_cursor_grab(False)
            self.input.set_cursor_visible(True)
            print(">>> GAME OVER - Press R to Restart <<<")

        # Mouse Look (Visual)
        dx, dy = self.input.get_mouse_delta()
        self.yaw += dx * 0.005
        self.pitch -= dy * 0.005
        self.pitch = max(-1.5, min(1.5, self.pitch))
        
        # Interaction Raycast
        # Calculate 3D forward vector from yaw/pitch
        # Pitch is negative looking up? In `on_update` pitch -= dy.
        # If pitch is 0, looking horizon. 
        # Standard: Y is Up.
        # fwd_x = sin(yaw) * cos(pitch)
        # fwd_y = sin(pitch) 
        # fwd_z = cos(yaw) * cos(pitch)
        # Note: check coordinate system. `on_fixed_update` uses sin(yaw) for X, cos(yaw) for Z.
        
        fx = math.sin(self.yaw) * math.cos(self.pitch)
        fy = math.sin(self.pitch)
        fz = math.cos(self.yaw) * math.cos(self.pitch)
        
        # Origin is camera pos
        cam_pos = self.camera.transform.position
        
        hit = self.scene.raycast((cam_pos[0], cam_pos[1], cam_pos[2]), (fx, fy, fz), 4.0)
        
        if hit:
            # hit is (entity_id, distance, point, normal)
            # Change crosshair to Red to indicate interactable
            self.hud.crosshair.set_color(1.0, 0.0, 0.0, 1.0)
            if self.input.is_key_just_pressed(Keys.E):
                print(f"Interacted with Entity ID {hit[0]}")
                # Simple feedback mechanisms
                self.stats.add_modifier("stamina", Modifier("adrenaline", 20.0, ModType.FLAT, duration=2.0))
        else:
            self.hud.crosshair.set_color(1.0, 1.0, 1.0, 0.8)

        self.camera.set_rotation(self.pitch, self.yaw, 0.0)
        
        # Debug Keys
        if self.input.is_key_just_pressed(Keys.T):
            # Take damage test
            curr = self.stats.get("health")
            self.stats.set_base("health", curr - 10.0)
            print(f"Health: {curr - 10.0}")
            
        if self.input.is_key_just_pressed(Keys.P):
             print(f"Pos: {self.character.transform.position}")
             
        # Escape to Exit to Menu implies unlocking cursor
        if self.input.is_key_just_pressed(Keys.Escape):
             self.input.set_cursor_grab(False)
             self.input.set_cursor_visible(True)
             self.state = "MENU"

    def on_fixed_update(self, dt: float):
        if self.state != "PLAY":
            return
            
        # Physics Movement
        speed = 10.0
        if self.input.is_key_down(Keys.Shift): speed *= 2.0
        
        fwd_x = math.sin(self.yaw)
        fwd_z = math.cos(self.yaw)
        right_x = math.cos(self.yaw)
        right_z = -math.sin(self.yaw)
        
        move_x = 0.0
        move_z = 0.0
        
        if self.input.is_key_down(Keys.W):
            move_x -= fwd_x
            move_z -= fwd_z
        if self.input.is_key_down(Keys.S):
            move_x += fwd_x
            move_z += fwd_z
        if self.input.is_key_down(Keys.D):
            move_x -= right_x
            move_z -= right_z
        if self.input.is_key_down(Keys.A):
            move_x += right_x
            move_z += right_z
            
        # Normalize
        length = math.sqrt(move_x*move_x + move_z*move_z)
        if length > 0:
            move_x /= length
            move_z /= length
            
        move_x *= speed * dt
        move_z *= speed * dt
        
        # Grounded Check & Jump
        # (Simple state machine based on previous frame's result)
        if self.grounded and self.input.is_key_down(Keys.SPACE):
            self.vertical_velocity = self.jump_speed
            self.grounded = False
            
        self.vertical_velocity += self.gravity * dt
        move_y = self.vertical_velocity * dt
        
        # Apply Movement
        old_pos_y = self.character.transform.position[1]
        
        # Call backend move
        # Returns actual new position
        new_x, new_y, new_z = self.character.move(move_x, move_y, move_z)
        
        # Heuristic Ground Check
        actual_dy = new_y - old_pos_y
        
        # We wanted to move down (gravity), but stopped -> Grounded
        if move_y < 0 and actual_dy > move_y + 0.001:
            self.grounded = True
            self.vertical_velocity = 0.0
            # Snap to ground slightly? Not needed if stuck
        elif move_y > 0 and actual_dy < move_y - 0.001:
            # Hit head
            self.vertical_velocity = 0.0
        else:
            self.grounded = False

    def on_late_update(self, dt: float):
        # Sync Camera to Character
        pos = self.character.transform.position
        # Eye height offset
        self.camera.set_position(pos[0], pos[1] + 1.8, pos[2])

if __name__ == "__main__":
    app = IslandSurvival(title="Starlight: The Island (FPS)", width=1280, height=720)
    app.run()
