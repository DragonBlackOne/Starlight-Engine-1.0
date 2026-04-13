"""Space Shooter - Simple Demo with WASD Controls."""
import math
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, backend, Keys, Scene, Entity, Camera
from starlight import math as smath


class SpaceShooterDemo(App):
    def on_start(self):
        print("[Demo] Starting Space Shooter (Framework API)...")
        self.frame_count = 0
        
        # Create Scene
        self.scene = Scene()
        
        # === Player ===
        # We manually spawn procedural ship for now as it's a specific backend feature not mapped to generic Entity yet
        # But we can wrap it in an Entity if we want.
        # For this demo, let's use standard Entity with a mesh if possible, OR keep procedural ship but wrap it.
        # Framework Entity uses 'spawn_entity' (primitive).
        # backend.spawn_procedural_ship is special.
        # Let's use backend for spawning BUT wrap in Entity class for management.
        
        ship_id = backend.spawn_procedural_ship(0.0, 0.0, 0.0, 0)
        self.player = Entity("Player")
        self.player.id = ship_id # Hijack ID
        self.player.transform.position = [0.0, 0.0, 0.0]
        self.scene.add(self.player)
        
        print(f"[Demo] Player ID: {self.player.id}")
        
        # === Enemies ===
        for i in range(5):
            x = (i - 2) * 3.0
            enemy = Entity(f"Enemy_{i}", x, 5.0, -10.0)
            # Use dynamic cube for physics? framework Entity spawns static/kinematic by default? 
            # Framework Entity uses spawn_entity which is usually static/kinematic unless rigid body added.
            # backend.spawn_dynamic_cube spawns RB.
            
            # Let's use generic Entity and maybe set mesh.
            enemy.set_mesh("cube")
            enemy.set_material("assets/textures/crate.png") # Placeholder
            enemy.set_scale(1.0, 1.0, 1.0)
            self.scene.add(enemy)
        
        self.cam = Camera()
        self.cam.position = [0.0, 2.0, 15.0]
        self.t = 0.0

    def on_update(self, dt):
        self.t += dt
        self.frame_count += 1
        
        if self.frame_count == 100:
            print("[Demo] Use 'world.capture_screenshot' via backend...")
            backend.capture_screenshot("space_shooter_phase6.png")
            print("[Demo] Captured screenshot. Exiting in 1 second...")
        
        if self.frame_count > 160:
            import sys
            sys.exit(0)

        speed = 5.0 * dt
        vx, vy = 0.0, 0.0
        
        # Input
        if self.input.is_key_down('a'): vx = -speed
        if self.input.is_key_down('d'): vx = speed
        if self.input.is_key_down('w'): vy = speed
        if self.input.is_key_down('s'): vy = -speed
        
        # Auto-movement
        if vx == 0 and vy == 0:
            vx = math.cos(self.t) * speed * 0.5
            vy = math.sin(self.t) * speed * 0.5
        
        # Update Player
        pos = self.player.transform.position
        new_x = pos[0] + vx
        new_y = pos[1] + vy
        self.player.set_position(new_x, new_y, pos[2])
        
        # Rotate player slightly based on movement
        tilt = -vx * 2.0
        self.player.set_rotation(0.0, 0.0, tilt)

    def on_late_update(self, dt):
        # Camera Follow
        pos = self.player.transform.position
        target_cam_x = pos[0] * 0.5
        target_cam_y = pos[1] * 0.5 + 2.0
        
        # Smooth lerp
        self.cam.position[0] += (target_cam_x - self.cam.position[0]) * 5.0 * dt
        self.cam.position[1] += (target_cam_y - self.cam.position[1]) * 5.0 * dt
        
        self.cam.set_transform(*self.cam.position)
        self.cam.look_at([pos[0], pos[1], 0.0])

if __name__ == "__main__":
    SpaceShooterDemo(title="Starlight Space Shooter (Phase 6)", width=800, height=600).run()
