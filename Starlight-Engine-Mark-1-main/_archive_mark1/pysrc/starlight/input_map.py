from .keys import Keys

class InputMapper:
    """
    Maps abstract actions to concrete keys.
    Skill: input-mapping
    """
    def __init__(self):
        self._map: dict[str, str] = {} # Action -> KeyName

    def map_action(self, action: str, key_name: str):
        self._map[action] = key_name

    def get_key_for(self, action: str) -> str:
        return self._map.get(action, "")

# Default instance
input_map = InputMapper()
input_map.map_action("MoveForward", "W")
input_map.map_action("MoveBackward", "S")
input_map.map_action("StrafeLeft", "A")
input_map.map_action("StrafeRight", "D")
input_map.map_action("Jump", "Space")
input_map.map_action("Ability1", "Q") # Zero G
input_map.map_action("Ability2", "E") # Gravity Flip
# input_map.map_action("Ability3", "R")
