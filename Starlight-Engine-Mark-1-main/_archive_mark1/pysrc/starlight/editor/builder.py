import os
import shutil
import json
import sys
import glob

def build_project(scene_data: dict, out_dir: str = "build"):
    """
    Packages the current project into a standalone executable directory.
    """
    print(f"[Builder] Starting build process to {out_dir}...")
    
    # Clean previous build
    if os.path.exists(out_dir):
        shutil.rmtree(out_dir)
    os.makedirs(out_dir)
    
    # 1. Copy engine source
    engine_src = os.path.join("pysrc", "starlight")
    build_engine_dst = os.path.join(out_dir, "starlight")
    if os.path.exists(engine_src):
        # We don't need the editor in the final build
        shutil.copytree(engine_src, build_engine_dst, ignore=shutil.ignore_patterns("editor", "__pycache__"))
    else:
        print("[Builder] Warning: 'pysrc/starlight' not found.")
        
    # 2. Copy compiled backend DLL
    # Look for backend.*.pyd or similar
    pyd_files = glob.glob("pysrc/starlight/backend*.pyd")
    if pyd_files:
        for pyd in pyd_files:
            shutil.copy(pyd, build_engine_dst)
    else:
        print("[Builder] ERROR: Missing compiled backend.pyd. Are you sure Rust is built?")
        
    # 3. Copy user scripts
    if os.path.exists("scripts"):
        shutil.copytree("scripts", os.path.join(out_dir, "scripts"), ignore=shutil.ignore_patterns("__pycache__"))
        
    # 4. Copy assets
    if os.path.exists("assets"):
        shutil.copytree("assets", os.path.join(out_dir, "assets"))
        
    # 5. Save the primary scene
    os.makedirs(os.path.join(out_dir, "scenes"), exist_ok=True)
    with open(os.path.join(out_dir, "scenes", "main.json"), "w") as f:
        json.dump(scene_data, f, indent=4)
        
    # 6. Generate entrypoint `main.py`
    # We write a runner that reconstructs the Scene and Behaviours just like the editor's PlayMode
    main_py_code = """
import os
import sys

# Ensure our local starlight package is found
sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))

from starlight.app import App
from starlight.framework import Engine, Scene
import json
import importlib.util

class GameRuntime(App):
    def on_init(self):
        self.scene = Scene()
        self.run_behaviours = []
        
        # Load main scene
        scene_path = os.path.join("scenes", "main.json")
        try:
            with open(scene_path, 'r') as f:
                data = json.load(f)
                
            from starlight.framework import Entity, Transform
            from starlight.behaviour import StarlightBehaviour
            from starlight import backend
            
            # Setup scene config
            sun = data.get("sun", {})
            if sun:
                self.scene.set_sun(
                    tuple(sun.get("direction", (-0.5, -1.0, -0.5))),
                    tuple(sun.get("color", (1.0, 1.0, 1.0))),
                    sun.get("intensity", 1.0)
                )

            for ent_data in data.get("entities", []):
                # Restore Transform -> Entity
                x, y, z = ent_data.get("position", [0,0,0])
                entity = Entity(ent_data.get("name", "Entity"), x, y, z)
                entity.set_rotation(*ent_data.get("rotation", [0,0,0]))
                entity.set_scale(*ent_data.get("scale", [1,1,1]))
                
                color = ent_data.get("color", [1,1,1,1])
                entity.set_color(*color)
                
                # Native Components
                native_comps = ent_data.get("native_components", {})
                for c_type, c_data in native_comps.items():
                    if c_type == "Health":
                        try: backend.add_component_health(entity.id, c_data.get("current", 100.0), c_data.get("max", 100.0))
                        except Exception: pass
                    elif c_type == "Mana":
                        try: backend.add_component_mana(entity.id, c_data.get("current", 100.0), c_data.get("max", 100.0))
                        except Exception: pass
                
                # Script Attachments
                b_paths = ent_data.get("behaviours", [])
                for b_path in b_paths:
                    if not os.path.exists(b_path): continue
                    module_name = os.path.splitext(os.path.basename(b_path))[0]
                    spec = importlib.util.spec_from_file_location(module_name, b_path)
                    if spec and spec.loader:
                        mod = importlib.util.module_from_spec(spec)
                        try:
                            spec.loader.exec_module(mod)
                            for name in dir(mod):
                                obj = getattr(mod, name)
                                if isinstance(obj, type) and issubclass(obj, StarlightBehaviour) and obj is not StarlightBehaviour:
                                    inst = obj()
                                    inst.entity = entity
                                    inst.on_start()
                                    self.run_behaviours.append(inst)
                        except Exception as e:
                            print(f"[Runtime] Error loading {b_path}: {e}")
                            
                self.scene.add(entity)
                
        except Exception as e:
            print(f"[Runtime] Failed to load scene: {e}")

    def on_update(self, dt):
        self.scene.update(dt)
        for b in self.run_behaviours:
            try: b.update(dt)
            except Exception: pass

    def on_shutdown(self):
        for b in self.run_behaviours:
            try: b.on_destroy()
            except Exception: pass

if __name__ == "__main__":
    game = Engine(GameRuntime)
    game.run()
"""
    with open(os.path.join(out_dir, "main.py"), "w") as f:
        f.write(main_py_code.strip())
        
    # 7. Generate bat file for quick running on windows
    with open(os.path.join(out_dir, "run.bat"), "w") as f:
        f.write("@echo off\\npython main.py\\npause")
        
    print(f"[Builder] Successfully packaged Game to '{out_dir}/'!")
    return True
