"""
Tropical Rainforest Simulation - Floresta Tropical Realista

Demo avançada do Starlight Engine simulando uma floresta tropical
com 5 camadas de vegetação, fauna animada e atmosfera imersiva.

Features:
- 5 camadas de vegetação (Emergente → Solo)
- Fauna animada (pássaros, borboletas, vagalumes)
- Iluminação tropical com neblina
- Ciclo dia/noite
- Character controller com física
"""

import math
import random
import sys
import os
from loguru import logger

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, Entity, Keys, backend
from starlight import audio
from starlight.ui import ui
from starlight.input_map import input_map
from starlight.components import FirstPersonController
from starlight.config import GameConfig
from starlight.procgen import (
    VegetationSpawner,
    FaunaSystem,
    FloraLayer
)


# =============================================================================
# CONFIGURAÇÃO
# =============================================================================

# Modo Performance (Ryzen 5500 + GTX 1050)
PERFORMANCE_MODE = True

FOREST_CONFIG = {
    "area_size": 30.0,
    "clearing_radius": 5.0,
    "vegetation_seed": 42,
    "fauna_seed": 123,
    "density_multiplier": 0.2,
}

ATMOSPHERE_CONFIG = {
    "fog_density": 0.025,
    "fog_color": (0.05, 0.08, 0.05),
    "ambient_light": (0.15, 0.2, 0.15),
    "sun_direction": (0.3, -0.7, -0.4),
    "sun_intensity": 0.6,
}

TIME_CONFIG = {
    "day_length_seconds": 120.0,  # 2 minutos = 1 dia
    "start_time": 0.35,           # Começar de manhã
}


# =============================================================================
# SISTEMAS AUXILIARES
# =============================================================================

class DayNightCycle:
    """
    Gerencia ciclo dia/noite com transições suaves.
    
    Afeta iluminação, cor do céu e ativação de fauna.
    """
    
    def __init__(self, day_length: float = 120.0, start_time: float = 0.5):
        self.day_length = day_length
        self.time = start_time  # 0-1 onde 0.5 = meio-dia
        
    def update(self, dt: float) -> None:
        """Avança o tempo."""
        self.time += dt / self.day_length
        if self.time > 1.0:
            self.time -= 1.0
    
    @property
    def sun_height(self) -> float:
        """Altura do sol (-1 a 1)."""
        # Seno para movimento suave
        return math.sin(self.time * math.pi * 2)
    
    @property
    def is_day(self) -> bool:
        """True se é dia."""
        return 0.2 < self.time < 0.8
    
    @property
    def is_night(self) -> bool:
        """True se é noite."""
        return not self.is_day
    
    def get_ambient_color(self) -> tuple:
        """Cor ambiente baseada na hora."""
        if self.is_day:
            # Dia: luz esverdeada filtrada pelo dossel
            intensity = 0.3 + 0.4 * self.sun_height
            return (0.15 * intensity, 0.25 * intensity, 0.15 * intensity)
        else:
            # Noite: azul escuro
            return (0.02, 0.02, 0.05)
    
    def get_fog_color(self) -> tuple:
        """Cor da neblina baseada na hora."""
        if self.is_day:
            return (0.1, 0.15, 0.1)  # Verde claro
        else:
            return (0.02, 0.02, 0.04)  # Azul escuro


class AtmosphereController:
    """
    Controla atmosfera: neblina, iluminação, céu.
    """
    
    def __init__(self, day_night: DayNightCycle):
        self.day_night = day_night
        
    def apply(self) -> None:
        """Aplica configurações atmosféricas atuais."""
        try:
            # Neblina
            fog_r, fog_g, fog_b = self.day_night.get_fog_color()
            backend.set_fog(
                ATMOSPHERE_CONFIG["fog_density"],
                fog_r, fog_g, fog_b
            )
            
            # Direção do sol (varia com hora do dia)
            sun_x, _, sun_z = ATMOSPHERE_CONFIG["sun_direction"]
            sun_y = -0.3 - 0.7 * max(0, self.day_night.sun_height)
            backend.set_sun_direction(sun_x, sun_y, sun_z)
            
            # Cor do céu
            if self.day_night.is_night:
                backend.set_skybox_color(0.01, 0.01, 0.03)
            else:
                intensity = 0.1 + 0.2 * self.day_night.sun_height
                backend.set_skybox_color(0.2 * intensity, 0.3 * intensity, 0.5 * intensity)
                
        except Exception as e:
            logger.debug(f"Atmosphere update failed: {e}")


# =============================================================================
# DEMO PRINCIPAL
# =============================================================================

class TropicalForestDemo(App):
    """
    Simulação de Floresta Tropical Realista.
    
    Combina geração procedural de vegetação, fauna animada e
    atmosfera dinâmica para criar uma experiência imersiva.
    """
    
    def __init__(self):
        self.config = GameConfig(
            title="Tropical Rainforest Simulation",
            width=1920,
            height=1080
        )
        super().__init__(
            self.config.title, 
            self.config.width, 
            self.config.height
        )
        
        # Sistemas
        self.vegetation: VegetationSpawner = None
        self.fauna: FaunaSystem = None
        self.day_night: DayNightCycle = None
        self.atmosphere: AtmosphereController = None
        self.camera_controller: FirstPersonController = None
        
        # Estado
        self.char_id = None
        self.debug_mode = False
        self.paused = False
        
    def on_start(self):
        """Inicialização da cena."""
        logger.info("=" * 60)
        logger.info("TROPICAL RAINFOREST SIMULATION")
        logger.info("=" * 60)
        
        # 1. Character Controller
        logger.info("Spawning player character...")
        try:
            self.char_id = backend.spawn_character(
                0.0, 5.0, 0.0,  # Posição inicial
                6.0,           # Velocidade
                0.8,           # Max slope
                0.1            # Offset
            )
            logger.info(f"Player spawned: ID {self.char_id}")
        except Exception as e:
            logger.error(f"Failed to spawn character: {e}")
            self.char_id = None
        
        # 2. Camera Controller
        self.camera_controller = FirstPersonController(self)
        if self.char_id is not None:
            self.camera_controller.posses(self.char_id)
        
        # 3. Floor
        backend.spawn_floor(-0.5)
        
        # 4. Vegetação
        logger.info("Generating tropical vegetation...")
        self.vegetation = VegetationSpawner(
            self, 
            seed=FOREST_CONFIG["vegetation_seed"]
        )
        
        # Spawna vegetação
        self.vegetation.spawn_forest(
            area_size=FOREST_CONFIG["area_size"],
            clearing_radius=FOREST_CONFIG["clearing_radius"],
            density_multiplier=FOREST_CONFIG["density_multiplier"],
            layers=[
                FloraLayer.EMERGENT,
                FloraLayer.CANOPY,
                FloraLayer.UNDERSTORY,
                FloraLayer.SHRUB,
                # FloraLayer.FLOOR,
            ]
        )
        
        # 5. Fauna
        logger.info("Initializing fauna system...")
        self.fauna = FaunaSystem(
            self,
            area_size=FOREST_CONFIG["area_size"]
        )
        self.fauna.initialize()
        
        # 6. Ciclo Dia/Noite
        self.day_night = DayNightCycle(
            day_length=TIME_CONFIG["day_length_seconds"],
            start_time=TIME_CONFIG["start_time"]
        )
        
        # 7. Atmosfera
        self.atmosphere = AtmosphereController(self.day_night)
        self.atmosphere.apply()
        
        # 8. Audio
        try:
            audio.play_sound("forest_ambience", loop=True, volume=0.4)
        except:
            logger.debug("Forest ambience not found")
        
        # 9. Input
        input_map.bind("toggle_debug", Keys.F3)
        input_map.bind("toggle_pause", Keys.P)
        input_map.bind("fast_forward", Keys.F)
        
        # 10. Cursor
        try:
            backend.set_cursor_grab(True)
            backend.set_cursor_visible(False)
        except:
            pass
        
        logger.info("=" * 60)
        logger.info("SIMULATION READY - Use WASD to move, mouse to look")
        logger.info("Press F3 for debug info, P to pause, F to fast-forward time")
        logger.info("=" * 60)
    
    def on_update(self, dt: float):
        """Loop principal."""
        
        # Escape para sair
        if self.app.input.is_key_just_pressed(Keys.ESCAPE):
            logger.info("Exiting simulation...")
            sys.exit(0)
        
        # Auto-Screenshot validation
        if self.app.input.frame_count == 60:
            logger.info("Auto-Capturing screenshot for verification...")
            try:
                self.capture_screenshot("forest_verify.png")
                logger.info("Screenshot taken: forest_verify.png")
            except Exception as e:
                logger.error(f"Screenshot failed: {e}")

        
        # Toggle debug
        if self.app.input.is_action_just_pressed("toggle_debug"):
            self.debug_mode = not self.debug_mode
            logger.info(f"Debug mode: {self.debug_mode}")
        
        # Toggle pause
        if self.app.input.is_action_just_pressed("toggle_pause"):
            self.paused = not self.paused
            logger.info(f"Paused: {self.paused}")
        
        if self.paused:
            return
        
        # Fast forward time
        time_dt = dt
        if self.app.input.is_action_down("fast_forward"):
            time_dt = dt * 10.0
        
        # Atualizar sistemas
        if self.camera_controller:
            self.camera_controller.update(dt)
        
        if self.day_night:
            self.day_night.update(time_dt)
        
        if self.fauna:
            self.fauna.set_time_of_day(self.day_night.time if self.day_night else 0.5)
            self.fauna.update(dt)
        
        if self.atmosphere:
            self.atmosphere.apply()
        
        # Debug HUD
        if self.debug_mode:
            self._draw_debug_hud()
    
    def _draw_debug_hud(self):
        """Desenha informações de debug."""
        try:
            # FPS Calculation
            fps = 1.0 / self.app.input.dt if self.app.input.dt > 0 else 0
            
            # Update Title with FPS every 60 frames roughly
            if self.app.input.frame_count % 60 == 0:
                # backend.set_window_title(f"Tropical Rainforest - FPS: {fps:.0f}") # Not implemented
                pass

            time_str = f"Time: {self.day_night.time:.2f}" if self.day_night else ""
            day_str = "Day" if (self.day_night and self.day_night.is_day) else "Night"
            
            fauna_stats = self.fauna.get_statistics() if self.fauna else {}
            fauna_str = ", ".join(f"{k}:{v}" for k, v in fauna_stats.items())
            
            # Console log debug info occasionally
            if self.debug_mode and self.app.input.frame_count % 60 == 0:
                logger.debug(f"FPS: {fps:.1f} | {time_str} ({day_str}) | Fauna: {fauna_str}")

        except:
            pass


# =============================================================================
# ENTRY POINT
# =============================================================================

if __name__ == "__main__":
    logger.info("Starting Tropical Rainforest Simulation...")
    demo = TropicalForestDemo()
    demo.run()
