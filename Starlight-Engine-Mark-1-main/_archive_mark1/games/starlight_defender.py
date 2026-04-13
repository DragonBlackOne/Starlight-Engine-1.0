import sys
import os
import math
import random
import traceback
import time

# Add pysrc to path so we can import the engine
project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(os.path.join(project_root, "pysrc"))

from starlight import App, Entity, Keys

class StarlightDefender(App):
    def __init__(self):
        super().__init__("Starlight Defender 🚀", 1600, 900)
        self.player = None
        self.wings = []
        self.player_speed = 20.0
        self.tilt = 0.0
        
        self.bullets = []
        self.enemies = []
        
        self.score = 0
        self.health = 100
        self.game_over = False
        
        # Game boundaries
        self.bounds_x = 22.0
        self.bounds_y = 12.0
        
        self.frame_count = 0
        self.laser_cooldown = 0.0
        self.enemy_spawn_rate = 0.05
        self.difficulty_timer = 0.0

    def on_start(self):
        print("[Game] Starting Starlight Defender v2...")
        
        # 1. Setup Environment
        self.world.set_sun_direction(0.5, -0.5, -1.0)
        self.world.set_visual_params(0.4, 0.8) # IBL, Skybox
        self.world.set_fog(0.002, 0.05, 0.05, 0.1) # Soft space fog
        
        try:
            self.world.show_grid = False
        except:
            pass
            
        # 2. Setup Composite Player Ship
        # Body
        self.player = Entity.spawn(self, 0.0, 0.0, 0.0, scale=1.0, model="cube")
        self.player.scale = [0.4, 0.4, 1.2]
        self.player.update_transform()

        # Wings (Visual only, no physics)
        left_wing = Entity.spawn(self, -1.0, 0.0, -0.5, scale=1.0, model="cube")
        left_wing.scale = [0.8, 0.1, 0.6]
        left_wing.update_transform()
        
        right_wing = Entity.spawn(self, 1.0, 0.0, -0.5, scale=1.0, model="cube")
        right_wing.scale = [0.8, 0.1, 0.6]
        right_wing.update_transform()
        
        self.wings = [left_wing, right_wing]
        
        # Camera behind player
        self.update_camera_pos()

    def update_camera_pos(self):
        if not self.player: return
        cam_z = float(self.player.transform.z + 12.0)
        cam_x = float(self.player.transform.x * 0.3)
        cam_y = float(self.player.transform.y * 0.3 + 3.0)
        
        # Look target
        target = [self.player.transform.x, self.player.transform.y, self.player.transform.z - 10.0]
        
        # Simple look_at logic (simplified for fixed camera)
        view = [[1.0,0.0,0.0,0.0],[0.0,1.0,0.0,0.0],[0.0,0.0,1.0,0.0],[0.0,0.0,0.0,1.0]]
        self.world.update_camera(view, [cam_x, cam_y, cam_z])

    def reset_game(self):
        self.score = 0
        self.health = 100
        self.game_over = False
        self.bullets = []
        self.enemies = []
        self.player.transform.x = 0.0
        self.player.transform.y = 0.0
        self.player.transform.z = 0.0
        self.difficulty_timer = 0.0
        print("Game Restarted!")

    def on_update(self, dt):
        try:
            # --- UI RENDERING ---
            # Score
            self.world.draw_text(
                f"SCORE: {self.score:05}", 
                30.0, 30.0, 
                1.0, # Scale
                1.0, 1.0, 0.0, 1.0 # Yellow
            )
            
            # Health
            hp_color = (0.0, 1.0, 0.0) if self.health > 50 else (1.0, 0.0, 0.0)
            self.world.draw_text(
                f"SHIELD: {int(self.health)}%", 
                30.0, 60.0, 
                1.0, 
                hp_color[0], hp_color[1], hp_color[2], 1.0
            )

            if self.game_over:
                self.world.draw_text("MISSION FAILED", 500.0, 400.0, 2.0, 1.0, 0.0, 0.0, 1.0)
                self.world.draw_text("PRESS [R] TO RESTART", 520.0, 450.0, 1.0, 1.0, 1.0, 1.0, 1.0)
                
                if self.input.is_key_down(Keys.R):
                    self.reset_game()
                return

            if not self.player:
                return

            self.frame_count += 1
            self.difficulty_timer += dt
            
            # Difficulty Ramp
            current_spawn_rate = min(0.2, 0.02 + (self.difficulty_timer * 0.001))

            # 1. Player Movement
            dx = 0.0
            dy = 0.0
            
            if self.input.is_key_down(Keys.W): dy += 1.0
            if self.input.is_key_down(Keys.S): dy -= 1.0
            if self.input.is_key_down(Keys.A): dx -= 1.0
            if self.input.is_key_down(Keys.D): dx += 1.0
            
            # Tilt effect
            target_tilt = -dx * 0.5 # Radians
            self.tilt += (target_tilt - self.tilt) * 5.0 * dt
            
            if dx != 0 and dy != 0:
                dx *= 0.707
                dy *= 0.707
                
            move_x = dx * self.player_speed * dt
            move_y = dy * self.player_speed * dt
            
            # Update Transform
            self.player.transform.x += move_x
            self.player.transform.y += move_y
            # Constant forward motion illusion (actually player stays z=0, enemies move)
            # But here we move player forward in Z
            self.player.transform.z -= 10.0 * dt 
            
            # Clamp bounds
            self.player.transform.x = max(-self.bounds_x, min(self.bounds_x, self.player.transform.x))
            self.player.transform.y = max(-self.bounds_y, min(self.bounds_y, self.player.transform.y))
            
            self.player.update_transform()
            
            # Sync Wings
            if len(self.wings) >= 2:
                # Left Wing
                self.wings[0].transform.x = self.player.transform.x - 0.8
                self.wings[0].transform.y = self.player.transform.y
                self.wings[0].transform.z = self.player.transform.z + 0.2
                self.wings[0].update_transform()
                
                # Right Wing
                self.wings[1].transform.x = self.player.transform.x + 0.8
                self.wings[1].transform.y = self.player.transform.y
                self.wings[1].transform.z = self.player.transform.z + 0.2
                self.wings[1].update_transform()
            
            # Update Camera
            self.update_camera_pos()
            
            # 2. Weapon System
            if self.laser_cooldown > 0:
                self.laser_cooldown -= dt
                
            if (self.input.is_key_down(Keys.SPACE) or self.frame_count % 10 == 0) and self.laser_cooldown <= 0:
                self.laser_cooldown = 0.1 # Fast fire
                # Spawn Laser (Green)
                px = float(self.player.transform.x)
                py = float(self.player.transform.y)
                pz = float(self.player.transform.z - 1.5)
                
                # Green Laser
                self.world.spawn_particles(px, py, pz, int(1), 0.2, 1.0, 0.2, 50.0, 1.5, 0.4)
                # Twin shots
                self.world.spawn_particles(px - 0.8, py, pz, int(1), 0.2, 1.0, 0.2, 50.0, 1.5, 0.3)
                self.world.spawn_particles(px + 0.8, py, pz, int(1), 0.2, 1.0, 0.2, 50.0, 1.5, 0.3)
                
                self.bullets.append({'x': px, 'y': py, 'z': pz, 'vx': 0.0, 'vy': 0.0, 'vz': -50.0})
                self.bullets.append({'x': px - 0.8, 'y': py, 'z': pz, 'vx': 0.0, 'vy': 0.0, 'vz': -50.0})
                self.bullets.append({'x': px + 0.8, 'y': py, 'z': pz, 'vx': 0.0, 'vy': 0.0, 'vz': -50.0})

            # 3. Enemy System
            if random.random() < current_spawn_rate: 
                ex = float((random.random() * 2 - 1) * self.bounds_x)
                ey = float((random.random() * 2 - 1) * self.bounds_y)
                ez = float(self.player.transform.z - 60.0)
                
                self.enemies.append({'x': ex, 'y': ey, 'z': ez})
                
            active_enemies = []
            for e in self.enemies:
                e['z'] += 15.0 * dt # Enemies move towards player (relative speed)
                
                # Simple AI
                if e['x'] < self.player.transform.x: e['x'] += 5.0 * dt
                if e['x'] > self.player.transform.x: e['x'] -= 5.0 * dt
                
                # Render Enemy (Red)
                self.world.spawn_particles(
                    float(e['x']), float(e['y']), float(e['z']), 
                    int(1), 
                    1.0, 0.1, 0.1, 
                    0.0, 0.1, 1.0
                )
                
                # Collision: Player vs Enemy
                dist_sq = (e['x'] - self.player.transform.x)**2 + (e['y'] - self.player.transform.y)**2 + (e['z'] - self.player.transform.z)**2
                if dist_sq < 3.0:
                    self.health -= 20
                    self.world.spawn_particles(
                        float(e['x']), float(e['y']), float(e['z']), 
                        int(30), 1.0, 0.5, 0.0, 10.0, 0.5, 0.5
                    ) # Explosion
                    continue # Destroy enemy on impact
                    
                if e['z'] < self.player.transform.z + 10.0:
                    active_enemies.append(e)
                    
            self.enemies = active_enemies

            # Update Bullets & Collisions
            active_bullets = []
            for b in self.bullets:
                b['z'] += b['vz'] * dt
                hit = False
                for e in self.enemies:
                        if e.get('dead'):
                            continue
                        dist_sq = (b['x'] - e['x'])**2 + (b['y'] - e['y'])**2 + (b['z'] - e['z'])**2
                        if dist_sq < 9.0: # Generous hit box
                            self.score += 50
                            # Explosion
                            self.world.spawn_particles(
                            float(e['x']), float(e['y']), float(e['z']), 
                            int(15), 1.0, 0.8, 0.2, 8.0, 0.4, 0.6
                            )
                            e['dead'] = True
                            hit = True
                            break
                
                if not hit and b['z'] > self.player.transform.z - 80.0:
                    active_bullets.append(b)
            self.bullets = active_bullets

            # Remove dead enemies
            self.enemies = [e for e in self.enemies if not e.get('dead')]

            # Game Over check
            if self.health <= 0:
                self.game_over = True
                print("GAME OVER")

            # Screenshot Logic for Validation
            if self.frame_count == 100:
                self.world.capture_screenshot("defender_ui.png")
            if self.frame_count > 200:
                 sys.exit(0)
                 
        except Exception:
            traceback.print_exc()
            sys.exit(1)

    def on_render(self):
        pass

if __name__ == "__main__":
    game = StarlightDefender()
    game.run()
