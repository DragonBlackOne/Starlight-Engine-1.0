"""Cloud Save Hooks — Abstract interface for cloud save providers.

Usage:
    cloud = CloudSaveManager(provider="local")  # or "steam", "custom"
    cloud.save("slot_1", {"level": 5, "gold": 1200})
    data = cloud.load("slot_1")
"""
from __future__ import annotations
import json
import os
import time
from typing import Any


class CloudSaveManager:
    """Cloud save abstraction layer.

    Providers:
        - "local": Saves to local disk (default, always available)
        - "steam": Placeholder for Steam Cloud integration
        - "custom": Placeholder for custom server API

    Example:
        csm = CloudSaveManager()
        csm.save("autosave", game_state)
        csm.list_saves()
    """

    def __init__(self, provider: str = "local", save_dir: str = "saves") -> None:
        self.provider = provider
        self.save_dir = save_dir
        os.makedirs(save_dir, exist_ok=True)

    def save(self, slot: str, data: dict[str, Any]) -> bool:
        """Save game data to a slot."""
        metadata = {
            "slot": slot,
            "timestamp": time.time(),
            "provider": self.provider,
            "data": data,
        }
        try:
            if self.provider == "local":
                return self._save_local(slot, metadata)
            elif self.provider == "steam":
                return self._save_steam(slot, metadata)
            elif self.provider == "custom":
                return self._save_custom(slot, metadata)
            else:
                print(f"[CloudSave] Unknown provider: {self.provider}")
                return False
        except Exception as e:
            print(f"[CloudSave] Error saving: {e}")
            return False

    def load(self, slot: str) -> dict[str, Any] | None:
        """Load game data from a slot."""
        try:
            if self.provider == "local":
                return self._load_local(slot)
            elif self.provider == "steam":
                return self._load_steam(slot)
            else:
                return self._load_local(slot)
        except Exception as e:
            print(f"[CloudSave] Error loading: {e}")
            return None

    def delete(self, slot: str) -> bool:
        path = os.path.join(self.save_dir, f"{slot}.json")
        if os.path.exists(path):
            os.remove(path)
            print(f"[CloudSave] Deleted: {slot}")
            return True
        return False

    def list_saves(self) -> list[dict[str, Any]]:
        """List all save slots with metadata."""
        saves = []
        for f in os.listdir(self.save_dir):
            if f.endswith(".json"):
                try:
                    with open(os.path.join(self.save_dir, f)) as fh:
                        meta = json.load(fh)
                        saves.append({
                            "slot": meta.get("slot", f[:-5]),
                            "timestamp": meta.get("timestamp", 0),
                        })
                except Exception:
                    pass
        return sorted(saves, key=lambda s: s["timestamp"], reverse=True)

    def _save_local(self, slot: str, data: dict) -> bool:
        path = os.path.join(self.save_dir, f"{slot}.json")
        with open(path, "w") as f:
            json.dump(data, f, indent=2)
        print(f"[CloudSave] Saved to {path}")
        return True

    def _load_local(self, slot: str) -> dict[str, Any] | None:
        path = os.path.join(self.save_dir, f"{slot}.json")
        if not os.path.exists(path):
            return None
        with open(path) as f:
            meta = json.load(f)
        return meta.get("data", meta)

    def _save_steam(self, slot: str, data: dict) -> bool:
        # Placeholder: In real implementation, use Steamworks API
        print(f"[CloudSave] Steam save not implemented, falling back to local")
        return self._save_local(slot, data)

    def _load_steam(self, slot: str) -> dict[str, Any] | None:
        print(f"[CloudSave] Steam load not implemented, falling back to local")
        return self._load_local(slot)

    def _save_custom(self, slot: str, data: dict) -> bool:
        # Placeholder: POST to custom server
        print(f"[CloudSave] Custom server save not implemented, falling back to local")
        return self._save_local(slot, data)
