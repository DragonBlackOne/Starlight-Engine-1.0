import sys
import os
import random
from loguru import logger

# Ensure pysrc is in path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, Entity, Keys, Input
from starlight.components import SpectatorCamera
from starlight.skills.manager import SkillManager
from starlight.skills.library import GravityFlipSkill, ZeroGravitySkill, HeavyGravitySkill
from starlight.ui import ui

class AntigravityDemo(App):
    def __init__(self):
        super().__init__("Antigravity Skills Demo", 1280, 720)
        self.camera_controller = None
        self.skill_manager = None
        self.player_entity = None
        
        logger.add("antigravity.log", rotation="1 MB")
        logger.info("Starting Antigravity Demo")

    def on_start(self):
        # UI Instructions
        ui.draw_instructions([
            "WASD + Mouse: Move",
            "Q: Flip Gravity",
            "E: Zero Gravity",
            "R: Heavy Gravity"
        ])

        # 1. Setup Environment
        self.world.spawn_ground(0.0, 0.0, 0.0, 1.0)
        self.world.set_sun_direction(0.5, 0.8, 0.2)
        self.world.set_fog(0.02, 0.1, 0.1, 0.2)
        
        # 2. Setup Player/Camera
        self.camera_controller = SpectatorCamera(self, position=[0, 5, 20])
        self.player_entity = self.camera_controller.entity
        
        if self.player_entity:
            # 3. Setup Skills
            self.skill_manager = SkillManager(self.player_entity, self.input)
            
            # Add Skills
            flip_idx = self.skill_manager.add_skill(GravityFlipSkill())
            zero_idx = self.skill_manager.add_skill(ZeroGravitySkill())
            heavy_idx = self.skill_manager.add_skill(HeavyGravitySkill())
            
            # Bind Keys
            self.skill_manager.bind_key("Q", flip_idx)
            self.skill_manager.bind_key("E", zero_idx)
            self.skill_manager.bind_key("R", heavy_idx)
            
            logger.info("Keys: Q=Flip Gravity, E=Zero G, R=Heavy World")
        
        # 4. Spawn MANY Physics Objects to demonstrate gravity
        for i in range(50):
            x = (random.random() * 20) - 10
            y = 5 + (random.random() * 20)
            z = (random.random() * 20) - 10
            Entity.spawn(self, x, y, z, model="cube")

    def on_update(self, dt: float):
        if self.camera_controller:
            self.camera_controller.update(dt)
            
        if self.skill_manager:
            self.skill_manager.update(dt)

if __name__ == "__main__":
    app = AntigravityDemo()
    app.run()
