import math
import time

import starlight


class RiverSafari(starlight.App):
    def __init__(self, title, width=1280, height=720):
        super().__init__(title, width, height)
        self.start_time = time.time()
        self.water_y = 1.0

    def on_start(self):
        print("River Safari Started")
        try:
            # Load assets
            print("Loading models...")
            self.world.load_model("assets/models/sphere.glb")
            print("Loading skybox...")
            self.world.load_skybox(
                "assets/skybox/right.jpg", "assets/skybox/left.jpg",
                "assets/skybox/top.jpg", "assets/skybox/bottom.jpg",
                "assets/skybox/front.jpg", "assets/skybox/back.jpg"
            )

            # Water setup
            # self.water_y = 1.0 # Moved to __init__
            print("Spawning water...")
            # x, y, z, sx, sz
            if hasattr(self.world, 'spawn_water'):
                self.world.spawn_water(0.0, self.water_y, 0.0, 50.0, 200.0)
            else:
                print("ERROR: spawn_water method missing (Build required?)")

            # Spawn Banks
            # (Skipping ground spawning for now to focus on water)
            # for z in range(-100, 100, 10):
            #    pass


            # Environment
            self.world.set_sun_direction(0.5, -1.0, 0.2)
            self.world.set_fog(0.015, 0.5, 0.6, 0.7)
            self.world.set_weather(0.6, 0.2)

            # self.start_time = time.time() # Moved to __init__
        except Exception as e:
            print(f"Error in on_start: {e}")

    def on_update(self, dt):
        now = time.time() - self.start_time

        # Camera Moves
        cam_z = -80.0 + (now * 5.0)
        if cam_z > 80.0:
            self.start_time = time.time() # Loop
            cam_z = -80.0

        self.cam_pos = [
            math.sin(now * 0.3) * 5.0,
            5.0 + math.sin(now * 0.5) * 0.5,
            cam_z
        ]
        self.cam_target = [0.0, self.water_y, cam_z + 10.0]

        # Sun animation
        self.world.set_sun_direction(math.sin(now * 0.1), -0.8, math.cos(now * 0.1))

        # Auto-Screenshot & Quit for Verification
        if now > 2.0 and not hasattr(self, 'done'):
            try:
                # Call Rust directly
                self.world.capture_screenshot("river_verify.png")
            except Exception as e:
                print(f"Screenshot failed: {e}")

            print("Demo finished successfully (Auto-Quit)")
            # self.world.quit()
            self.done = True


if __name__ == "__main__":
    app = RiverSafari(title="Starlight Engine - River Safari")
    app.run()
