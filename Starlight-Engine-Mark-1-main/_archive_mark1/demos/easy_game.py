import math
import os
import sys

# Add pysrc to path so we can import 'starlight'
# In a real install, this would be installed via pip
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../pysrc"))

from starlight import App, Entity, Keys


class EasyGame(App):
    def on_start(self):
        try:
            print("DEBUG: on_start entered", flush=True)
            print("Game Started!")

            # Spawn Ground
            Entity.spawn(self, 0, 0, 0, 1.0, model="ground")

            # Spawn Trees
            for i in range(10):
                angle = i * (360/10) * (3.14/180)
                x = math.cos(angle) * 10
                z = math.sin(angle) * 10
                Entity.spawn(self, x, 0, z, 0.5 + (i%3)*0.2, model="tree")

            self.t = 0.0
            print("DEBUG: on_start finished", flush=True)
        except Exception:
            import traceback
            traceback.print_exc()

    def on_update(self, dt):
        self.t += dt

        # Orbit Camera logic is now strictly Python side, simplified!
        radius = 20.0
        self.cam_pos[0] = math.sin(self.t * 0.5) * radius
        self.cam_pos[2] = math.cos(self.t * 0.5) * radius
        self.cam_pos[1] = 10.0

        # Input Check
        if self.input.is_key_down(Keys.SPACE):
            print("Space Pressed!")

if __name__ == "__main__":
    EasyGame(title="Easy Game Demo").run()
