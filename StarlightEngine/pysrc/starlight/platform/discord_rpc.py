"""Discord Rich Presence Wrapper.

Usage:
    discord = DiscordManager(client_id="123456")
    discord.update_presence(details="In Game", state="Level 1")
"""
from __future__ import annotations
import time

class DiscordManager:
    """Handles connection to Discord RPC."""
    def __init__(self, client_id: str) -> None:
        self.client_id = client_id
        self.connected = False
        self.start_time = time.time()

    def connect(self) -> bool:
        # pypresence or similar would be used here
        print(f"[Discord] Connecting with ID {self.client_id}...")
        self.connected = True
        return True

    def update_presence(self, details: str, state: str, 
                        curr_size: int = 1, max_size: int = 1) -> None:
        if not self.connected: return
        
        # Payload logic
        print(f"[Discord] Updating: {details} - {state} ({curr_size}/{max_size})")

    def shutdown(self) -> None:
        if self.connected:
            print("[Discord] Disconnected")
            self.connected = False
