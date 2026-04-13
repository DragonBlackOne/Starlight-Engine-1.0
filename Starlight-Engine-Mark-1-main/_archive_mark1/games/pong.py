
import sys
import os
import math
import random
import time

# Ensure pysrc is in path
# Ensure pysrc is in path FIRST
pysrc_path = os.path.abspath("pysrc")
if pysrc_path not in sys.path:
    sys.path.insert(0, pysrc_path)

from starlight import App, Input, Keys, backend
from starlight import framework
from starlight.framework import Scene, Entity, Camera

# --- Constants ---
FIELD_WIDTH = 30.0
FIELD_DEPTH = 20.0
PADDLE_Z_SIZE = 4.0
PADDLE_X_SIZE = 0.5
BALL_SIZE = 0.5
PADDLE_SPEED = 15.0
BALL_SPEED_INITIAL = 12.0
BALL_SPEED_MAX = 25.0
WIN_SCORE = 11

class DigitEntity:
    """
    Represents a single 7-segment digit using 7 Cube Entities.
    Segments are indexed 0-6 (Top, TopLeft, TopRight, Middle, BottomLeft, BottomRight, Bottom)
    """
    # 0: Top, 1: TL, 2: TR, 3: Mid, 4: BL, 5: BR, 6: Bot
    SEGMENTS = {
        0: [0, 1, 2, 4, 5, 6],
        1: [2, 5],
        2: [0, 2, 3, 4, 6],
        3: [0, 2, 3, 5, 6],
        4: [1, 2, 3, 5],
        5: [0, 1, 3, 5, 6],
        6: [0, 1, 3, 4, 5, 6],
        7: [0, 2, 5],
        8: [0, 1, 2, 3, 4, 5, 6],
        9: [0, 1, 2, 3, 5, 6]
    }

    def __init__(self, scene, x, y, z, scale=1.0):
        self.segments = []
        self.x, self.y, self.z = x, y, z
        self.segments = []
        
        # Geometry constants
        t = 0.2 * scale # Thickness
        l = 1.0 * scale # Length (Half-length effectively for positioning)
        
        # 7-Segment Layout (Center Origin)
        # 0: Top, 1: TopLeft, 2: TopRight, 3: Mid, 4: BotLeft, 5: BotRight, 6: Bot
        
        # Seg 0 (Top)
        self._add_seg(scene, 0, -l, l*2, t)
        # Seg 1 (Top Left)
        self._add_seg(scene, -l, -l/2, t, l)
        # Seg 2 (Top Right)
        self._add_seg(scene, l, -l/2, t, l)
        # Seg 3 (Mid)
        self._add_seg(scene, 0, 0, l*2, t)
        # Seg 4 (Bot Left)
        self._add_seg(scene, -l, l/2, t, l)
        # Seg 5 (Bot Right)
        self._add_seg(scene, l, l/2, t, l)
        # Seg 6 (Bot)
        self._add_seg(scene, 0, l, l*2, t)
        
        self.SEGMENTS = {
            0: [0, 1, 2, 4, 5, 6],
            1: [2, 5],
            2: [0, 2, 3, 4, 6],
            3: [0, 2, 3, 5, 6],
            4: [1, 2, 3, 5],
            5: [0, 1, 3, 5, 6],
            6: [0, 1, 3, 4, 5, 6],
            7: [0, 2, 5],
            8: [0, 1, 2, 3, 4, 5, 6],
            9: [0, 1, 2, 3, 5, 6]
        }
        
        self.set_value(0)

    def _add_seg(self, scene, dx, dz, sx, sz):
        s = Entity(f"Seg_{random.randint(0,99999)}", self.x + dx, self.y, self.z + dz)
        s.set_mesh("cube").set_scale(sx, 0.1, sz).set_color(0.1, 0.1, 0.1, 1.0)
        # Emissive off
        s.set_material_params(0.0, 1.0)
        scene.add(s)
        self.segments.append(s)

    def set_value(self, val):
        active_indices = self.SEGMENTS.get(val, [])
        for i, s in enumerate(self.segments):
            if i in active_indices:
                s.set_color(0.0, 1.0, 1.0, 1.0) # Cyan ON
            else:
                s.set_color(0.1, 0.1, 0.1, 0.2) # Grey OFF

class PongGame(App):
    def on_start(self):
        print("Starlight Pong - SYSTEM BOOT")
        self.scene = Scene()
        self.setup_rendering()
        
        # --- Entities ---
        # Floor
        self.floor = Entity("Floor").set_mesh("cube").set_scale(FIELD_WIDTH*1.2, 0.1, FIELD_DEPTH*1.2)
        self.floor.set_position(0, -1, 0).set_color(0.05, 0.05, 0.1) # Dark Blue
        self.scene.add(self.floor)
        
        # Walls (Top/Bottom in Z)
        self.wall_top = Entity("WallTop", 0, 0, -FIELD_DEPTH/2 - 0.5)
        self.wall_top.set_mesh("cube").set_scale(FIELD_WIDTH, 1.0, 1.0).set_color(0.5, 0.5, 0.5)
        self.scene.add(self.wall_top)
        
        self.wall_bot = Entity("WallBot", 0, 0, FIELD_DEPTH/2 + 0.5)
        self.wall_bot.set_mesh("cube").set_scale(FIELD_WIDTH, 1.0, 1.0).set_color(0.5, 0.5, 0.5)
        self.scene.add(self.wall_bot)
        
        # Paddles
        self.p1 = Entity("P1", -FIELD_WIDTH/2 + 2, 0, 0).set_mesh("cube")
        self.p1.set_scale(PADDLE_X_SIZE, 1.0, PADDLE_Z_SIZE).set_color(0.0, 0.5, 1.0) # Blue
        self.scene.add(self.p1)
        
        self.p2 = Entity("P2", FIELD_WIDTH/2 - 2, 0, 0).set_mesh("cube")
        self.p2.set_scale(PADDLE_X_SIZE, 1.0, PADDLE_Z_SIZE).set_color(1.0, 0.5, 0.0) # Orange
        self.scene.add(self.p2)
        
        # Ball
        self.ball = Entity("Ball", 0, 0, 0).set_mesh("cube")
        self.ball.set_scale(BALL_SIZE, BALL_SIZE, BALL_SIZE).set_color(1.0, 1.0, 1.0)
        self.scene.add(self.ball)
        
        # Ball Light
        backend.set_point_light(0, 0, 2, 0, 1.0, 1.0, 1.0, 20.0) # Larger radius
        
        # === SAFETY LIGHTING ===
        # Ensure scene is visible even if point light fails
        self.scene.set_sun((0.5, -1.0, 0.5), (1.0, 1.0, 1.0), 1.0) # Directional Sun
        self.scene.set_ambient(0.2) # Base visibility
        # =======================
        
        # Camera (TV Angle)
        self.camera = Camera()
        self.camera.set_position(0, 40, 25) 
        self.camera.look_at((0, 0, 0))
        

        # Floor
        self.floor_visual = Entity("FloorVisual", 0, -0.5, 0).set_mesh("cube")
        self.floor_visual.set_scale(FIELD_WIDTH + 2, 0.5, FIELD_DEPTH + 2).set_color(0.1, 0.1, 0.15) # Dark Blue-Grey
        self.scene.add(self.floor_visual)
        
        # Scoreboard
        self.score1_disp = DigitEntity(self.scene, -5, 0, -FIELD_DEPTH/2 - 3, scale=2.0)
        self.score2_disp = DigitEntity(self.scene, 5, 0, -FIELD_DEPTH/2 - 3, scale=2.0)
        
        # --- State ---
        self.score1 = 0
        self.score2 = 0
        self.ball_vel = [0.0, 0.0] # X, Z
        self.reset_ball(direction=1)
        
        self.p1_vel_z = 0.0
        self.p2_vel_z = 0.0 # AI
        
        self.hits = 0

    def setup_rendering(self):
        # Neon Style
        self.scene.set_sun((0, -1, 0), (0.1, 0.1, 0.1), 0.5) # Dim sun
        self.scene.set_ambient(0.0)
        backend.set_post_process_params(1.0, 2.2, 2.0, 0.8) # High Bloom

    def reset_ball(self, direction):
        self.ball.set_position(0, 0, 0)
        speed = BALL_SPEED_INITIAL
        # serve to direction
        vx = speed if direction > 0 else -speed
        vz = random.uniform(-speed*0.5, speed*0.5)
        self.ball_vel = [vx, vz]
        self.ball.set_color(1.0, 1.0, 1.0)
        self.hits = 0
        # Wait a bit? (Not implemented here for simplicity)

    def on_update(self, dt: float):
        # 1. Player Input
        self.p1_vel_z = 0
        if self.input.is_key_down(Keys.W): self.p1_vel_z = -PADDLE_SPEED
        if self.input.is_key_down(Keys.S): self.p1_vel_z = PADDLE_SPEED
        
        # 2. AI Input (Simple Tracking)
        target_z = self.ball.transform.position[2]
        # Adding some error/delay could be nice, but let's make it perfect first
        # AI Speed limit
        diff = target_z - self.p2.transform.position[2]
        if abs(diff) > 0.5:
            self.p2_vel_z = PADDLE_SPEED if diff > 0 else -PADDLE_SPEED
        else:
            self.p2_vel_z = 0
            
        # 3. Move Paddles (Constraint to walls)
        self._move_paddle(self.p1, self.p1_vel_z, dt)
        self._move_paddle(self.p2, self.p2_vel_z, dt)
        
        # 4. Move Ball
        bx, by, bz = self.ball.transform.position
        bx += self.ball_vel[0] * dt
        bz += self.ball_vel[1] * dt
        
        # 5. Physics: Wall Collision (Top/Bot)
        # Arena is centered at 0. Top is -H/2, Bot is H/2
        half_h = FIELD_DEPTH / 2
        if bz < -half_h + BALL_SIZE/2:
            bz = -half_h + BALL_SIZE/2
            self.ball_vel[1] *= -1
            # Wall Sound?
            # self.audio.play(...)
            
        if bz > half_h - BALL_SIZE/2:
            bz = half_h - BALL_SIZE/2
            self.ball_vel[1] *= -1

        # 6. Physics: Paddle Collision (AABB)
        # Check P1
        if self._check_aabb(bx, bz, self.p1):
            # Reflect
            self.ball_vel[0] = abs(self.ball_vel[0]) # Force positive X
            self._apply_spin(self.p1, bz)
            self._increase_speed()
            bx = self.p1.transform.position[0] + PADDLE_X_SIZE/2 + BALL_SIZE/2 + 0.1

        # Check P2
        if self._check_aabb(bx, bz, self.p2):
            # Reflect
            self.ball_vel[0] = -abs(self.ball_vel[0]) # Force negative X
            self._apply_spin(self.p2, bz)
            self._increase_speed()
            bx = self.p2.transform.position[0] - PADDLE_X_SIZE/2 - BALL_SIZE/2 - 0.1
            
        self.ball.set_position(bx, 0, bz)
        backend.set_point_light(0, bx, 2, bz, 1.0, 1.0, 1.0, 10.0) # Move light with ball
        
        # 7. Scoring
        if bx < -FIELD_WIDTH/2 - 2:
            self.score2 += 1
            self.score2_disp.set_value(self.score2)
            print(f"P2 Scored! {self.score1}-{self.score2}", flush=True)
            self.reset_ball(direction=1) # Serve to P2 (winner? or loser?) usually loser serves. P1 lost, P1 serves?
            # Let's serve to the one who scored
            
        if bx > FIELD_WIDTH/2 + 2:
            self.score1 += 1
            self.score1_disp.set_value(self.score1)
            print(f"P1 Scored! {self.score1}-{self.score2}", flush=True)
            self.reset_ball(direction=-1)

        # Win
        if self.score1 >= WIN_SCORE or self.score2 >= WIN_SCORE:
            print("GAME OVER")
            # Reset
            self.score1 = 0
            self.score2 = 0
            self.score1_disp.set_value(0)
            self.score2_disp.set_value(0)
            
        # 8. Verification (Auto-Screenshot & Quit)
        if not hasattr(self, 'verify_frame'): self.verify_frame = 0
        self.verify_frame += 1
        
        if self.verify_frame == 30:
            print("Capturing pong_visual_fix.png", flush=True)
            backend.capture_screenshot("pong_visual_fix.png")
            
        if self.verify_frame == 60:
            print("Auto-quit for verification", flush=True)
            self.quit()

    def _move_paddle(self, paddle, vz, dt):
        x, y, z = paddle.transform.position
        z += vz * dt
        # Clamp
        half_h = FIELD_DEPTH / 2
        limit = half_h - PADDLE_Z_SIZE / 2
        z = max(-limit, min(limit, z))
        paddle.set_position(x, y, z)

    def _check_aabb(self, bx, bz, paddle):
        px, py, pz = paddle.transform.position
        
        # Bounds
        b_min_x = bx - BALL_SIZE/2
        b_max_x = bx + BALL_SIZE/2
        b_min_z = bz - BALL_SIZE/2
        b_max_z = bz + BALL_SIZE/2
        
        p_min_x = px - PADDLE_X_SIZE/2
        p_max_x = px + PADDLE_X_SIZE/2
        p_min_z = pz - PADDLE_Z_SIZE/2
        p_max_z = pz + PADDLE_Z_SIZE/2
        
        return (b_min_x < p_max_x and b_max_x > p_min_x and
                b_min_z < p_max_z and b_max_z > p_min_z)

    def _apply_spin(self, paddle, bz):
        # English: Relative hit position determines Z velocity
        pz = paddle.transform.position[2]
        relative_z = (bz - pz) / (PADDLE_Z_SIZE / 2) # -1 to 1
        
        # Max angle ~45 deg?
        # keep speed magnitude, change direction
        speed = math.sqrt(self.ball_vel[0]**2 + self.ball_vel[1]**2)
        
        # Influence angle
        angle_factor = 0.8 # radians max adjustment
        angle = relative_z * angle_factor
        
        # Basic trigonometry to rotate the velocity vector
        # X is primary direction. 
        # If going Right (VelX > 0), and hit P2 (on right), we reflect Left.
        
        sign_x = -1 if self.ball_vel[0] > 0 else 1 
        
        # New velocity based on angle
        # Simple approximation: Vz = speed * sin(angle)
        # Vx = speed * cos(angle)
        
        self.ball_vel[1] = speed * math.sin(angle) * 1.5 # Boost Z effect
        self.ball_vel[0] = sign_x * abs(speed * math.cos(angle))
        
        # Flash paddle color
        paddle.set_color(1.0, 1.0, 1.0) # White flash
        # Restore color next frame? (Too lazy for timer, let's just leave it white for a sec or drift back)
        
    def _increase_speed(self):
        self.hits += 1
        if self.hits % 5 == 0:
            # +10% speed
            self.ball_vel[0] *= 1.1
            self.ball_vel[1] *= 1.1
            # Clamp
            mag = math.sqrt(self.ball_vel[0]**2 + self.ball_vel[1]**2)
            if mag > BALL_SPEED_MAX:
                ratio = BALL_SPEED_MAX / mag
                self.ball_vel[0] *= ratio
                self.ball_vel[1] *= ratio
            # print("SPEED UP!")

if __name__ == "__main__":
    game = PongGame(title="Starlight Pong", width=1280, height=720)
    game.run()
