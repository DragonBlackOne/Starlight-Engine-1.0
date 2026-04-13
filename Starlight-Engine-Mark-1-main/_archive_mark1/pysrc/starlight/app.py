from typing import Callable, Any, Optional
from . import backend
import time
import sys
from .editor.ipc import EditorServer

class WorldProxy:
    def set_sun_direction(self, x: float, y: float, z: float) -> None:
        backend.set_sun_direction(x, y, z)

    def set_visual_params(self, ibl: float, sky: float) -> None:
        pass

    def set_fog(self, density: float, r: float, g: float, b: float) -> None:
        backend.set_fog(density, r, g, b)

    def spawn_ground(self, x: float, y: float, z: float, scale: float) -> None:
        backend.spawn_floor(y)

    def capture_screenshot(self, filename: str) -> None:
        backend.capture_screenshot(filename)

    def draw_text(self, text: str, x: float, y: float, scale: float, r: float, g: float, b: float, a: float) -> None:
        if hasattr(backend, 'draw_text'):
            backend.draw_text(text, x, y, scale, r, g, b, a)

    def spawn_particles(self, x: float, y: float, z: float, count: int, r: float, g: float, b: float, speed: float, spread: float, life: float) -> None:
        if hasattr(backend, 'spawn_particles'):
            backend.spawn_particles(x, y, z, count, r, g, b, speed, spread, life)

    def update_camera(self, view_matrix: Any, position: Any) -> None:
        if hasattr(backend, 'update_camera'):
            backend.update_camera(view_matrix, position)

from .input import Input

class App:
    def __init__(self, title: str = "Starlight App", width: int = 1280, height: int = 720, headless: bool = False) -> None:
        self.title = title
        self.width = width
        self.height = height
        self.world = WorldProxy()
        self.input = Input()
        self._started = False
        self._started = False
        self.headless = headless
        
        # Editor Integration
        self.editor_mode = "--editor" in sys.argv
        self.editor_server = None
        if self.editor_mode:
            self.editor_server = EditorServer(self)

    def run(self) -> None:
        if self.editor_server:
            self.editor_server.start()
            
        if self.headless:
            self._run_headless()
        else:
            try:
                backend.run_engine(
                    self.title,
                    self.width,
                    self.height,
                    "",
                    self._internal_update,
                    self._internal_fixed_update,
                    self._internal_render
                )
            finally:
                if self.editor_server:
                    self.editor_server.stop()

    def _run_headless(self):
        print(f"Initializing Headless Engine {self.width}x{self.height}...")
        backend.init_headless(self.width, self.height)

        # Main Loop Simulation
        self._internal_update(0.016, None, None) # Start

        last_time = time.time()
        accumulator = 0.0
        fixed_dt = 0.02

        frame = 0
        try:
            while frame < 1000: # Run for 1000 frames (approx 16s)
                now = time.time()
                dt = now - last_time
                last_time = now
                if dt > 0.1: dt = 0.1

                accumulator += dt
                while accumulator >= fixed_dt:
                    self._internal_fixed_update(fixed_dt)
                    if hasattr(backend, 'step_fixed'):
                        backend.step_fixed()
                    accumulator -= fixed_dt

                backend.update_game() # Variable Update
                self._internal_update(dt, None, None)
                # self._internal_render() # Disable rendering in execution/headless mode
                frame += 1
                time.sleep(0.016)
        except KeyboardInterrupt:
            pass
        print("Headless run complete.")

    def _internal_update(self, dt: float, backend_mod: Any, input_obj: Any) -> None:
        self.input.dt = dt
        self.input.frame_count += 1
        if not self._started:
            self.on_start()
            self._started = True
        if self.editor_server:
            self.editor_server.update()
            
        self.on_update(dt)
        self.on_late_update(dt)

    def _internal_fixed_update(self, dt: float) -> None:
        if self._started:
            # Poll collisions
            collisions = backend.get_collision_events()
            for (e1, e2, started) in collisions:
                self.on_collision(e1, e2, started)

            self.on_fixed_update(dt)

    def on_collision(self, entity1_id: int, entity2_id: int, started: bool) -> None:
        pass

    def _internal_render(self) -> None:
        self.on_render()

    def on_start(self) -> None:
        pass

    def on_update(self, dt: float) -> None:
        pass

    def on_late_update(self, dt: float) -> None:
        pass

    def on_fixed_update(self, dt: float) -> None:
        pass

    def on_render(self) -> None:
        pass
