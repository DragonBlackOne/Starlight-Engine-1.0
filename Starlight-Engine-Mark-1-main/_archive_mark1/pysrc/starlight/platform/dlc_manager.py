"""DLC Management System.

Usage:
    dlc = DLCManager()
    if dlc.is_installed(DLC_MAP_PACK):
        load_map()
"""
from __future__ import annotations

class DLCManager:
    """Checks ownership and installation status of DLC."""
    def __init__(self, provider_check_func=None) -> None:
        self.check_func = provider_check_func or (lambda id: True) # Default allow

    def is_owned(self, dlc_id: int) -> bool:
        # Check Steam/Epic
        return self.check_func(dlc_id)

    def is_installed(self, dlc_id: int) -> bool:
        # Check file presence
        return self.is_owned(dlc_id) # Assume installed if owned for now
