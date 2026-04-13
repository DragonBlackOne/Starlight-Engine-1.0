
import sys
import os

project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(os.path.join(project_root, "pysrc"))

from starlight import App, Entity

class TestApp(App):
    def on_start(self):
        print("Checking World attributes...")
        print(dir(self.world))
        if hasattr(self.world, 'draw_text'):
            print("draw_text FOUND!")
            try:
                self.world.draw_text("TEST", 10.0, 10.0, 1.0, 1.0, 1.0, 1.0, 1.0)
                print("draw_text CALLED SUCCESS!")
            except Exception as e:
                print(f"draw_text FAILED: {e}")
        else:
            print("draw_text NOT FOUND")
        sys.exit(0)

if __name__ == "__main__":
    app = TestApp()
    app.run()
