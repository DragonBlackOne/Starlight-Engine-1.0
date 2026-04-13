"""Editor Prefab System."""
from __future__ import annotations
import json
import os
import dearpygui.dearpygui as dpg

class PrefabEditor:
    """Edits prefab assets in isolation."""
    def __init__(self, studio) -> None:
        self.studio = studio
        self.is_active = False
        self.prefab_path: str = ""
        self._main_scene_backup: list[dict] = []
        
    def open_prefab(self, path: str) -> None:
        if self.is_active: return
        
        # Backup current scene array
        self._main_scene_backup = [dict(ent) for ent in self.studio.entities]
        
        # Clear engine 
        if self.studio.backend:
            try:
                self.studio.backend.clear_entities()
            except Exception: pass
            
        self.studio.entities.clear()
        self.studio.hierarchy.selected_id = None
        self.is_active = True
        self.prefab_path = path

        # Try load
        try:
            if os.path.exists(path):
                with open(path, "r", encoding="utf-8") as f:
                    data = json.load(f)
                    
                new_id = 9999
                ent = {
                    "id": new_id,
                    "name": data.get("name", "PrefabRoot"),
                    "position": data.get("position", [0, 0, 0]),
                    "rotation": data.get("rotation", [0, 0, 0]),
                    "scale": data.get("scale", [1, 1, 1]),
                    "color": data.get("color", [1, 1, 1, 1])
                }
                
                if self.studio.backend:
                    try:
                        self.studio.backend.spawn_entity(*ent["position"])
                        # Further setup would go here
                    except Exception: pass
                
                self.studio.entities.append(ent)
                self.studio.console.log(f"[Prefab] Loaded {path}", "INFO")
        except Exception as e:
            self.studio.console.log(f"[Prefab] Failed to load {path}: {e}", "ERROR")

        self.studio._rebuild_hierarchy()
        dpg.set_value("viewport_mode", f"Mode: PREFAB ({os.path.basename(path)})")

    def save_and_close(self) -> None:
        if not self.is_active: return
        
        # Save first entity as prefab root
        if self.studio.entities:
            root = self.studio.entities[0]
            data = {
                "name": root.get("name", "Prefab"),
                "position": root.get("position", [0, 0, 0]),
                "rotation": root.get("rotation", [0, 0, 0]),
                "scale": root.get("scale", [1, 1, 1]),
                "color": root.get("color", [1, 1, 1, 1])
            }
            try:
                with open(self.prefab_path, "w", encoding="utf-8") as f:
                    json.dump(data, f, indent=4)
                self.studio.console.log(f"[Prefab] Saved {self.prefab_path}", "INFO")
            except Exception as e:
                self.studio.console.log(f"[Prefab] Save failed: {e}", "ERROR")

        self.discard_and_close()

    def discard_and_close(self) -> None:
        if not self.is_active: return
        
        if self.studio.backend:
            try:
                self.studio.backend.clear_entities()
            except Exception: pass
            
        self.studio.entities = self._main_scene_backup
        
        # Restore backend state for these entities if possible
        if self.studio.backend:
            for ent in self.studio.entities:
                try: self.studio.backend.spawn_entity(*ent.get("position", [0,0,0]))
                except Exception: pass

        self.studio.hierarchy.selected_id = None
        self.is_active = False
        self.prefab_path = ""
        self.studio._rebuild_hierarchy()
        dpg.set_value("viewport_mode", f"Mode: {self.studio.play_mode.state}")
        self.studio.console.log("[Prefab] Closed. Main Scene restored.", "INFO")
