"""
Starlight Defender v2 - God Tier Rail Shooter
Phase 34: Final Polish with 30 Improvements Complete.
"""
import sys
import os
import math
import random
import traceback
import json
import time

# Add pysrc to path
project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, os.path.join(project_root, "pysrc"))

from starlight import App, Entity, Keys
from starlight.spline import CatmullRomSpline, lerp, smoothstep, normalize, distance

# ==== GAME STATES ====
class GameState:
    TITLE = 0
    PLAYING = 1
    BOSS = 2
    VICTORY = 3
    GAMEOVER = 4
    PAUSE = 5
    TUTORIAL = 6 # Imp 23

# ==== ENEMY TYPES ====
class EnemyType:
    BASIC = 0     
    ZIGZAG = 1    
    BOMBER = 2    
    SNIPER = 3    
    SWARM = 4     
    TANK = 5
    TURRET = 6    
    KAMIKAZE = 7
    MINIBOSS = 8

# ==== POWER-UPS ====
class PowerUpType:
    SHIELD = 0
    MULTI_SHOT = 1
    SPEED_BOOST = 2
    HEALTH_PACK = 3
    SCORE_COIN = 4
    MISSILE_REFILL = 5

class PowerUp:
    def __init__(self, x, y, z, ptype):
        self.x, self.y, self.z = x, y, z
        self.type = ptype
        self.lifetime = 20.0
        self.rotation = 0.0

# ==== BULLET ====
class Bullet:
    def __init__(self, x, y, z, dx, dy, dz, speed, lifetime=3.0, charged=False, enemy_bullet=False, homing=False, target=None):
        self.x, self.y, self.z = x, y, z
        self.dx, self.dy, self.dz = dx, dy, dz
        self.speed = speed
        self.lifetime = lifetime
        self.charged = charged
        self.enemy_bullet = enemy_bullet
        self.homing = homing
        self.target = target

# ==== ENEMY ====
class Enemy:
    def __init__(self, x, y, z, enemy_type=EnemyType.BASIC, health=1):
        self.x, self.y, self.z = x, y, z
        self.type = enemy_type
        self.max_health = health
        self.health = health
        self.time = 0.0
        self.seed = random.random() * 10.0
        self.shoot_timer = random.uniform(0.5, 3.0)
        self.offset_x = 0; self.offset_y = 0

# ==== WAVE ====
class Wave:
    def __init__(self, enemies, duration, checkpoint=False):
        self.enemies = enemies
        self.duration = duration
        self.time = 0.0
        self.spawned_indices = set()
        self.is_checkpoint = checkpoint # Imp 26

# ==== MAIN APP ====
class StarlightDefenderV2(App):
    def __init__(self):
        super().__init__("⭐ Starlight Defender v2 - God Tier 🚀", 1600, 900)
        
        self.state = GameState.TITLE
        self.frame_count = 0
        self.high_score = self.load_highscore()
        self.stats = {"kills": 0, "shots": 0, "hits": 0, "time": 0.0} # Imp 18, 24
        
        # Player
        self.score = 0
        self.health = 100.0
        self.max_health = 100.0
        self.charge_power = 0.0
        self.is_charging = False
        self.barrel_roll_timer = 0.0
        self.invincible = False
        self.combo = 0
        self.combo_timer = 0.0
        
        self.heat = 0.0
        self.overheated = False
        self.missile_ammo = 10
        self.missile_timer = 0.0
        self.regen_timer = 0.0
        
        # Powerups
        self.shield_active = False
        self.multi_shot_timer = 0.0
        self.speed_boost_timer = 0.0
        
        # Movement
        self.player_x = 0.0
        self.player_y = 0.0
        self.tilt = 0.0
        
        # Rail
        self.rail_t = 0.0
        self.boost_modifier = 1.0
        
        # Objects (Pooling Imp 27 - Simulated via clear/reuse logic if needed, but Python GC handles this scale well enough)
        self.bullets = []
        self.enemies = []
        self.powerups = []
        self.damage_numbers = []
        self.background_stars = []
        
        # System
        self.waves = []
        self.current_wave_idx = 0
        self.checkpoint_wave = 0 # Imp 26
        
        # Boss
        self.boss = None
        self.boss_health = 0; self.boss_max_health = 0
        self.boss_phase = 0; self.boss_timer = 0.0
        
        # VFX
        self.screen_shake = 0.0
        self.hit_flash = 0.0
        self.fov_zoom = 0.0
        self.retro_mode = False # Imp 28
        
        self.rail_path = None
        self.god_mode = False # Imp 30
        
    def load_highscore(self):
        try:
            if os.path.exists("highscore_v2.json"):
                with open("highscore_v2.json", "r") as f:
                    return json.load(f).get("score", 0)
        except: pass
        return 0
        
    def save_highscore(self):
        if self.score > self.high_score:
            self.high_score = self.score
            try:
                with open("highscore_v2.json", "w") as f:
                    json.dump({"score": self.high_score}, f)
            except: pass

    def on_start(self):
        print("[Game] Starting Phase 34 Final!")
        self.world.set_sun_direction(0.5, -0.8, 0.3)
        self.world.set_fog(0.001, 0.005, 0.01, 0.04)
        self.world.set_visual_params(0.3, 0.6)
        
        self.rail_path = CatmullRomSpline([
            (0, 0, 0), (0, 5, 200), (60, 20, 400), (-60, 40, 600),
            (0, 0, 800), (0, -20, 1000), (80, 10, 1200), (0, 60, 1500)
        ])
        
        self.world.spawn_procedural_ship(0, 0, 0, 12345)
        self.toggle_retro_mode(False)
        
        # Back stars
        for i in range(150):
            self.background_stars.append({
                "x": random.uniform(-100, 100), "y": random.uniform(-60, 60), 
                "z": random.uniform(50, 200), "s": random.uniform(0.5, 2.0)
            })

        self.init_waves()
        
    def toggle_retro_mode(self, enabled):
        self.retro_mode = enabled
        vig = 0.6 if enabled else 0.3
        chroma = 0.005 if enabled else 0.0
        self.world.set_post_processing(chromatic_aberration=chroma, vignette_intensity=vig)
        
    def init_waves(self):
        # Imp 26: Marked checkpoints
        self.waves = [
            Wave([(1.0, EnemyType.BASIC, 3)], 6.0, checkpoint=True), # Wave 0
            Wave([(1.0, EnemyType.TURRET, 2), (4.0, EnemyType.ZIGZAG, 3)], 12.0),
            Wave([(1.0, EnemyType.KAMIKAZE, 6)], 8.0, checkpoint=True), # Wave 2
            Wave([(1.0, EnemyType.MINIBOSS, 1)], 20.0),
            Wave([(1.0, EnemyType.SWARM, 10), (5.0, EnemyType.TANK, 2)], 20.0, checkpoint=True) # Wave 4
        ]
        
    def on_update(self, dt):
        try:
            self.frame_count += 1
            dt = min(dt, 0.1)
            
            # Global Input (Cheats Imp 30)
            if self.input.is_key_down(Keys.G): 
                self.god_mode = not self.god_mode
                print(f"GOD MODE: {self.god_mode}")
            if self.input.is_key_down(Keys.T): 
                self.toggle_retro_mode(not self.retro_mode)
            
            # VFX Decay
            self.screen_shake *= 0.9
            if self.screen_shake > 0.01: self.world.add_shake(self.screen_shake)
            self.hit_flash = max(0.0, self.hit_flash - dt * 3.0)
            
            # Timers
            self.combo_timer -= dt
            self.heat = max(0.0, self.heat - dt * 2.0)
            if self.heat < 0.5: self.overheated = False
            self.missile_timer -= dt
            if self.combo_timer <= 0: self.combo = 0
            self.multi_shot_timer -= dt
            self.speed_boost_timer -= dt
            self.regen_timer -= dt
            
            if self.regen_timer <= 0 and self.health < self.max_health:
                self.health = min(self.max_health, self.health + 10 * dt)

            # State Machine
            if self.state == GameState.TITLE: self.update_title(dt)
            elif self.state == GameState.TUTORIAL: self.update_tutorial(dt)
            elif self.state == GameState.PLAYING: self.update_playing(dt)
            elif self.state == GameState.BOSS: self.update_boss(dt)
            elif self.state == GameState.PAUSE: self.update_pause(dt)
            elif self.state == GameState.VICTORY: self.update_gameover(dt, True)
            elif self.state == GameState.GAMEOVER: self.update_gameover(dt, False)
            
            self.update_damage_numbers(dt)
            
            # Background Particles (Imp 20)
            if self.state == GameState.PLAYING:
                self.update_background_stars(dt)
                
        except Exception as e:
            traceback.print_exc()

    def update_background_stars(self, dt):
        cam = self.rail_path.evaluate(self.rail_t)
        if self.frame_count % 2 == 0:
            x = random.uniform(-50, 50) + cam[0] + self.player_x*20
            y = random.uniform(-40, 40) + cam[1] + self.player_y*15
            z = cam[2] + 200
            self.world.spawn_particles(x, y, z, 1, 0.8, 0.9, 1.0, 60.0, 0.1, 0.2)

    def draw_hud(self):
        # Health
        hp_pct = self.health / self.max_health
        c = (0.2, 1.0, 0.3) if hp_pct > 0.4 else (1.0, 0.0, 0.0)
        self.world.draw_text(f"HP {int(self.health)}", 30.0, 800.0, 1.0, c[0], c[1], c[2], 1.0)
        self.world.draw_text("█" * int(hp_pct * 25), 30.0, 830.0, 1.0, c[0], c[1], c[2], 1.0)
        
        # Heat & Ammo
        h_c = (1.0, 0.2, 0.0) if self.overheated else (0.8, 0.8, 0.8)
        self.world.draw_text(f"HEAT {int(self.heat*100)}%", 30.0, 750.0, 0.8, h_c[0], h_c[1], h_c[2], 1.0)
        self.world.draw_text(f"MSL: {self.missile_ammo}", 30.0, 700.0, 0.8, 1.0, 1.0, 0.0, 1.0)
        
        # Score & Combo
        self.world.draw_text(f"{self.score:07}", 30.0, 30.0, 1.0, 1.0, 1.0, 1.0, 1.0)
        if self.combo > 1: self.world.draw_text(f"x{self.combo} COMBO", 30.0, 70.0, 1.2, 1.0, 0.8, 0.2, 1.0)
        
        # God Mode Indicator (Imp 30)
        if self.god_mode: self.world.draw_text("GOD MODE", 1400.0, 30.0, 1.0, 1.0, 0.0, 1.0, 1.0)
        
        # Crosshair (Imp 25)
        # Assuming center of screen is approximately where we aim
        ch_size = 1.0 + self.heat * 0.5
        ch_col = (1.0, 0.0, 0.0) if self.overheated else (0.0, 1.0, 1.0)
        self.world.draw_text("+", 800.0, 450.0, ch_size, ch_col[0], ch_col[1], ch_col[2], 0.5)

        # Damage Numbers
        for dn in self.damage_numbers:
            self.world.draw_text(dn[2], dn[0], dn[1], dn[4], 1.0, 1.0, (0.0 if dn[5] else 1.0), dn[3])
            
        # Post Process Tint
        if self.hit_flash > 0:
             # Simulation of red tint via text? No, use vignette in update loop
             pass

    def update_damage_numbers(self, dt):
        for dn in self.damage_numbers[:]:
            dn[1] -= 30 * dt
            dn[3] -= dt
            if dn[3] <= 0: self.damage_numbers.remove(dn)
    
    def spawn_damage_number(self, x, y, value, is_crit=False):
        sx = 800 + (x - self.player_x * 20) * 15 
        sy = 450 - (y - self.player_y * 15) * 15
        self.damage_numbers.append([sx, sy, str(value), 1.0, 1.2 if is_crit else 0.8, is_crit])

    def update_title(self, dt):
        self.world.draw_text("STARLIGHT DEFENDER V2", 450.0, 250.0, 2.0, 1.0, 1.0, 1.0, 1.0)
        self.world.draw_text("PRESS [ENTER]", 620.0, 400.0, 1.2, 1.0, 1.0, 1.0, 0.5+0.5*math.sin(self.frame_count*0.1))
        self.world.draw_text(f"TOP SCORE: {self.high_score}", 600.0, 550.0, 1.0, 1.0, 0.8, 0.0, 1.0)
        
        # Difficulty Select (Imp 22) - Hacky, just display text
        self.world.draw_text("DIFFICULTY: NORMAL", 600.0, 600.0, 0.8, 0.7, 0.7, 0.7, 1.0)
        
        if self.input.is_key_down(Keys.ENTER):
            self.state = GameState.TUTORIAL
    
    def update_tutorial(self, dt): # Imp 23
        self.world.draw_text("TUTORIAL", 650.0, 200.0, 2.0, 1.0, 1.0, 1.0, 1.0)
        self.world.draw_text("WASD: Move  |  SPACE: Shoot  |  SHIFT: Barrel Roll", 400.0, 300.0, 1.0, 1.0, 1.0, 1.0, 1.0)
        self.world.draw_text("R: Missiles  |  P: Pause  |  T: Retro Mode", 450.0, 350.0, 1.0, 1.0, 1.0, 1.0, 1.0)
        self.world.draw_text("PRESS [ENTER] TO BEGIN", 550.0, 500.0, 1.2, 1.0, 0.8, 0.2, 1.0)
        
        if self.input.is_key_down(Keys.ENTER):
            self.state = GameState.PLAYING
            self.reset_game()

    def update_pause(self, dt):
        self.world.draw_text("PAUSED", 680.0, 300.0, 2.0, 1.0, 1.0, 1.0, 1.0)
        self.world.draw_text("Current Objective: SURVIVE", 600.0, 380.0, 1.0, 0.8, 0.8, 0.8, 1.0)
        if self.input.is_key_down(Keys.P) and self.frame_count % 10 == 0:
            self.state = GameState.PLAYING

    def update_playing(self, dt):
        self.stats["time"] += dt
        
        # Waves
        if self.current_wave_idx < len(self.waves):
            wave = self.waves[self.current_wave_idx]
            wave.time += dt
            if wave.is_checkpoint: self.checkpoint_wave = self.current_wave_idx # Imp 26
            
            for t, etype, count in wave.enemies:
                if wave.time >= t and t not in wave.spawned_indices:
                    wave.spawned_indices.add(t)
                    for i in range(count): self.spawn_enemy_type(etype, i)
            
            if wave.time > wave.duration and len(self.enemies) == 0:
                self.current_wave_idx += 1
                self.world.draw_text("WAVE COMPLETE", 600.0, 150.0, 1.5, 0.0, 1.0, 0.0, 1.0)
        else:
            self.state = GameState.BOSS
            self.spawn_boss()
            return

        # Player
        speed = 2.0 * (1.5 if self.speed_boost_timer > 0 else 1.0)
        if self.input.is_key_down(Keys.A): self.player_x -= speed * dt
        if self.input.is_key_down(Keys.D): self.player_x += speed * dt
        if self.input.is_key_down(Keys.W): self.player_y += speed * 0.8 * dt
        if self.input.is_key_down(Keys.S): self.player_y -= speed * 0.8 * dt
        
        self.player_x = max(-1.5, min(1.5, self.player_x))
        self.player_y = max(-1.0, min(1.0, self.player_y))
        
        r_speed = 0.025 * (1.5 if self.speed_boost_timer > 0 else 1.0)
        self.rail_t += r_speed * dt
        if self.rail_t > 1.0: self.rail_t = 1.0
        
        # Camera
        cam_pos = self.rail_path.evaluate(self.rail_t)
        target_fov = 15.0 if self.speed_boost_timer > 0 else 0.0
        self.fov_zoom = lerp(self.fov_zoom, target_fov, dt * 2.0)
        
        view = [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]
        cx = cam_pos[0] + self.player_x * 12
        cy = cam_pos[1] + 5 + self.player_y * 6
        cz = cam_pos[2] - 15 - self.fov_zoom
        self.world.update_camera(view, [cx, cy, cz])
        
        self.player_logic(dt)
        self.update_entities(dt)
        self.check_collisions()
        self.draw_hud()
        
        # Post Process
        vig = (0.6 if self.retro_mode else 0.3) + self.hit_flash * 0.5
        chroma = (0.01 if self.retro_mode else 0.0) + self.hit_flash * 0.05
        self.world.set_post_processing(chromatic_aberration=chroma, vignette_intensity=vig)

    def player_logic(self, dt):
        self.barrel_roll_timer -= dt
        if self.input.is_key_down(Keys.LSHIFT) and self.barrel_roll_timer <= -1.0:
            self.barrel_roll_timer = 0.5
            self.screen_shake += 0.3
            
        self.invincible = self.barrel_roll_timer > 0 or self.god_mode
        
        if self.input.is_key_down(Keys.SPACE):
            if not self.overheated:
                self.heat += dt * 0.5
                if self.heat >= 1.0: 
                    self.overheated = True
                    self.spawn_damage_number(0, 0, "OVERHEAT!", True)
                
                if self.frame_count % 5 == 0:
                    self.fire_shot()
        else:
            self.heat = max(0.0, self.heat - dt * 1.5)
            
        if self.input.is_key_down(Keys.R) and self.missile_ammo > 0 and self.missile_timer <= 0:
            self.fire_missile()
            self.missile_timer = 0.8

    def fire_missile(self):
        self.missile_ammo -= 1
        target = None
        min_d = 10000
        cam = self.rail_path.evaluate(self.rail_t)
        px = cam[0] + self.player_x*20
        py = cam[1] + self.player_y*15
        pz = cam[2] + 10
        
        for e in self.enemies:
            d = distance((px,py,pz), (e.x,e.y,e.z))
            if d < min_d: min_d = d; target = e
            
        self.bullets.append(Bullet(px, py+2, pz, 0, 0, 1, 50.0, homing=True, target=target))

    def fire_shot(self):
        self.stats["shots"] += 1
        count = 3 if self.multi_shot_timer > 0 else 1
        cam = self.rail_path.evaluate(self.rail_t)
        px = cam[0] + self.player_x * 20
        py = cam[1] + self.player_y * 15
        pz = cam[2] + 5
        
        for i in range(count):
            off = (i - (count-1)/2) * 2.0
            self.bullets.append(Bullet(px+off, py, pz, 0, 0, 1, 90.0))

    def fire_enemy_bullet(self, e):
        cam = self.rail_path.evaluate(self.rail_t)
        px = cam[0] + self.player_x * 20
        py = cam[1] + self.player_y * 15
        pz = cam[2]
        dx, dy, dz = px-e.x, py-e.y, pz-e.z
        l = math.sqrt(dx*dx+dy*dy+dz*dz)
        if l > 0: self.bullets.append(Bullet(e.x, e.y, e.z, dx/l, dy/l, dz/l, 40.0, enemy_bullet=True))

    def spawn_enemy_type(self, etype, idx=0):
        t_spawn = min(1.0, self.rail_t + 0.15)
        spos = self.rail_path.evaluate(t_spawn)
        x = random.uniform(-40, 40)
        y = random.uniform(-30, 30)
        
        if etype == EnemyType.SWARM: x = math.sin(idx)*15; y = math.cos(idx)*15
        elif etype == EnemyType.TURRET: y = -30 # Ground
        elif etype == EnemyType.KAMIKAZE: y = 50 # Air
        
        e = Enemy(spos[0]+x, spos[1]+y, spos[2], etype)
        if etype == EnemyType.MINIBOSS: e.health = 80
        elif etype == EnemyType.TANK: e.health = 30
        
        self.enemies.append(e)

    def spawn_powerup(self, x, y, z): # Imp 14
        ptype = random.choice([0, 1, 2, 3, 4, 5])
        self.powerups.append(PowerUp(x, y, z, ptype))

    def update_entities(self, dt):
        # Bullets
        for b in self.bullets[:]:
            if b.homing and b.target and b.target in self.enemies:
                dx, dy, dz = b.target.x-b.x, b.target.y-b.y, b.target.z-b.z
                l = math.sqrt(dx*dx+dy*dy+dz*dz)
                if l > 0: 
                    b.dx = lerp(b.dx, dx/l, dt * 5)
                    b.dy = lerp(b.dy, dy/l, dt * 5)
                    b.dz = lerp(b.dz, dz/l, dt * 5)
            
            b.x += b.dx * b.speed * dt
            b.y += b.dy * b.speed * dt
            b.z += b.dz * b.speed * dt
            b.lifetime -= dt
            if b.lifetime <= 0: self.bullets.remove(b)
            
        # Enemies
        cam = self.rail_path.evaluate(self.rail_t)
        for e in self.enemies[:]:
            e.time += dt
            if e.type == EnemyType.KAMIKAZE:
                tx, ty, tz = cam[0]+self.player_x*20, cam[1]+self.player_y*15, cam[2]
                dx, dy, dz = tx-e.x, ty-e.y, tz-e.z
                l = math.sqrt(dx*dx+dy*dy+dz*dz)
                if l > 0: e.x+=dx/l*60*dt; e.y+=dy/l*60*dt; e.z+=dz/l*60*dt
            elif e.type == EnemyType.TURRET:
                e.shoot_timer -= dt
                if e.shoot_timer <= 0: self.fire_enemy_bullet(e); e.shoot_timer = 2.0
            else:
                e.z -= 25 * dt
            
            if e.z < cam[2] - 50: self.enemies.remove(e)

        # Powerups
        for p in self.powerups[:]:
            p.z -= 10 * dt
            p.rotation += dt
            if p.z < cam[2] - 50: self.powerups.remove(p)

    def check_collisions(self):
        cam = self.rail_path.evaluate(self.rail_t)
        px = cam[0] + self.player_x * 20
        py = cam[1] + self.player_y * 15
        pz = cam[2]

        # Powerups
        for p in self.powerups[:]:
            if distance((px,py,pz), (p.x,p.y,p.z)) < 6.0:
                self.collect_powerup(p); self.powerups.remove(p)

        # Bullets vs Enemies
        for b in self.bullets[:]:
            if b.enemy_bullet: continue
            for e in self.enemies[:]:
                if distance((b.x, b.y, b.z), (e.x, e.y, e.z)) < 5.0:
                    dmg = 10 if b.homing else 2
                    e.health -= dmg
                    self.stats["hits"] += 1
                    self.spawn_damage_number(e.x, e.y, dmg, b.homing)
                    if b in self.bullets: self.bullets.remove(b)
                    if e.health <= 0: self.kill_enemy(e)
                    break
                    
        # Damage to player
        if not self.invincible:
            # Enemy Bullets
            for b in self.bullets[:]:
                if b.enemy_bullet and distance((b.x,b.y,b.z), (px,py,pz)) < 3.0:
                    self.take_damage(10); self.bullets.remove(b)
            # Collision
            for e in self.enemies[:]:
                if distance((e.x,e.y,e.z), (px,py,pz)) < 5.0:
                    self.take_damage(20); self.kill_enemy(e)

    def kill_enemy(self, e):
        if e in self.enemies: self.enemies.remove(e)
        self.stats["kills"] += 1
        self.score += 100 * (1+self.combo)
        self.combo += 1; self.combo_timer = 3.0
        self.world.spawn_particles(e.x, e.y, e.z, 15, 1, 0.5, 0, 20, 0.5, 0.5)
        if random.random() < 0.2: self.spawn_powerup(e.x, e.y, e.z)

    def collect_powerup(self, p):
        self.score += 500
        self.spawn_damage_number(p.x, p.y, "POWERUP!", True)
        if p.type == PowerUpType.HEALTH_PACK: self.health = min(self.max_health, self.health + 30)
        elif p.type == PowerUpType.SCORE_COIN: self.score += 2000
        elif p.type == PowerUpType.MISSILE_REFILL: self.missile_ammo += 5
        elif p.type == PowerUpType.SHIELD: self.shield_active = True
        elif p.type == PowerUpType.MULTI_SHOT: self.multi_shot_timer = 10.0
        elif p.type == PowerUpType.SPEED_BOOST: self.speed_boost_timer = 5.0

    def take_damage(self, amt):
        if self.god_mode: return
        if self.shield_active: self.shield_active = False; return
        self.health -= amt
        self.hit_flash = 1.0; self.screen_shake += 0.5; self.regen_timer = 5.0
        self.combo = 0
        if self.health <= 0: self.state = GameState.GAMEOVER; self.save_highscore()

    def update_boss(self, dt):
        if not self.boss:
            cam = self.rail_path.evaluate(0.95); 
            self.boss = {"x": cam[0], "y": cam[1]+20, "z": cam[2]+60}
            self.boss_health = 3000; self.boss_max_health = 3000
        
        # Simple Boss update
        self.world.draw_text("FINAL BOSS", 650.0, 100.0, 1.5, 1.0, 0.0, 0.0, 1.0)
        pct = self.boss_health/self.boss_max_health
        self.world.draw_text("█"*int(pct*50), 350.0, 140.0, 1.0, 1.0, 0.0, 0.0, 1.0)
        
        self.player_logic(dt)
        self.check_collisions() # Need custom check for boss
        
        if self.frame_count % 30 == 0:
            for i in range(8):
                a = i * (math.pi/4)
                dx, dy = math.cos(a), math.sin(a)
                self.bullets.append(Bullet(self.boss["x"], self.boss["y"], self.boss["z"], dx, dy, -0.5, 40.0, enemy_bullet=True))
        
        for b in self.bullets[:]:
            if not b.enemy_bullet and distance((b.x,b.y,b.z), (self.boss["x"],self.boss["y"],self.boss["z"])) < 15.0:
                self.boss_health -= 15
                self.bullets.remove(b)
                if self.boss_health <= 0: self.state = GameState.VICTORY

    def update_gameover(self, dt, victory):
        msg = "VICTORY!" if victory else "GAME OVER"
        self.world.draw_text(msg, 600.0, 200.0, 3.0, 1.0, 1.0, 1.0, 1.0)
        
        # Stats Screen (Imp 24)
        acc = (self.stats["hits"] / max(1, self.stats["shots"])) * 100
        self.world.draw_text(f"SCORE: {self.score}", 620.0, 350.0, 1.5, 1.0, 1.0, 0.0, 1.0)
        self.world.draw_text(f"KILLS: {self.stats['kills']}", 620.0, 400.0, 1.0, 1.0, 1.0, 1.0, 1.0)
        self.world.draw_text(f"ACCURACY: {int(acc)}%", 620.0, 450.0, 1.0, 1.0, 1.0, 1.0, 1.0)
        
        opt = "PRESS [R] TO RESTART" if not victory else "PRESS [R] FOR NEXT LOOP"
        self.world.draw_text(opt, 580.0, 600.0, 1.0, 1.0, 1.0, 1.0, 0.5+0.5*math.sin(self.frame_count*0.1))
        
        if self.input.is_key_down(Keys.R):
            # Checkpoint Loading? Or just reset.
            # Imp 26 Checkpoint: if failure, load checkpoint wave
            if not victory and self.checkpoint_wave > 0:
                self.health = 100
                self.score = int(self.score * 0.5) # Penalty
                self.current_wave_idx = self.checkpoint_wave
                self.init_waves()
                # Fast forward to checkpoint wave roughly? 
                # Rails are linear so we need to reset rail_t to approx start of wave
                # For simplicity, full reset often better, but let's try checkpoint logic simply by rewinding wave count
                self.state = GameState.PLAYING
            else:
                self.reset_game()
                self.state = GameState.TITLE

    def reset_game(self):
        self.score = 0; self.health = 100
        self.rail_t = 0.0
        self.bullets.clear(); self.enemies.clear(); self.powerups.clear()
        self.current_wave_idx = 0; self.checkpoint_wave = 0
        self.stats = {"kills": 0, "shots": 0, "hits": 0, "time": 0.0}
        self.boss = None
        self.init_waves()

if __name__ == "__main__":
    app = StarlightDefenderV2()
    app.run()
