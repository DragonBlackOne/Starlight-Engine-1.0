import json
import os
import pickle
from typing import Any, Dict

class SaveSystem:
    """
    Skill: save-system
    """
    def __init__(self, mode="json"):
        self.mode = mode
        self.save_dir = "saves"
        if not os.path.exists(self.save_dir):
            os.makedirs(self.save_dir)

    def save_game(self, slot: str, data: Dict[str, Any]):
        path = f"{self.save_dir}/{slot}.{self.mode}"
        try:
            if self.mode == "json":
                with open(path, "w") as f:
                    json.dump(data, f, indent=4)
            else:
                with open(path, "wb") as f:
                    pickle.dump(data, f)
            print(f"[Save] Game saved to {path}")
        except Exception as e:
            print(f"[Save] Error: {e}")

    def load_game(self, slot: str) -> Dict[str, Any]:
        path = f"{self.save_dir}/{slot}.{self.mode}"
        try:
            if self.mode == "json":
                with open(path, "r") as f:
                    return json.load(f)
            else:
                with open(path, "rb") as f:
                    return pickle.load(f)
        except Exception as e:
            print(f"[Save] Error loading: {e}")
            return {}

save_manager = SaveSystem()
