from typing import Callable, List, Dict, Any
from .utils import Singleton

class EventManager(metaclass=Singleton):
    """
    Global Event System using Observer Pattern.
    Skill: event-system
    """
    def __init__(self):
        self._listeners: Dict[str, List[Callable]] = {}

    def subscribe(self, event_type: str, callback: Callable):
        if event_type not in self._listeners:
            self._listeners[event_type] = []
        self._listeners[event_type].append(callback)

    def emit(self, event_type: str, data: Any = None):
        """
        Broadcasting event to all listeners.
        """
        if event_type in self._listeners:
            for callback in self._listeners[event_type]:
                try:
                    callback(data)
                except Exception as e:
                    print(f"Error in event listener for {event_type}: {e}")

# Global Access
events = EventManager()
