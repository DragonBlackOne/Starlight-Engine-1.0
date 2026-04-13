"""Editor Timeline (Cinematic/Animation)."""
from __future__ import annotations
from dataclasses import dataclass
import dearpygui.dearpygui as dpg

@dataclass
class Keyframe:
    time: float
    value: list

class Track:
    def __init__(self, entity_id: int, property_name: str) -> None:
        self.entity_id = entity_id
        self.property_name = property_name
        self.keyframes: list[Keyframe] = []
        
    def add_key(self, time: float, value: list) -> None:
        # Replace if very close
        for k in self.keyframes:
            if abs(k.time - time) < 0.05:
                k.value = list(value)
                return
        self.keyframes.append(Keyframe(time, list(value)))
        self.keyframes.sort(key=lambda k: k.time)

    def evaluate(self, time: float) -> list | None:
        if not self.keyframes:
            return None
        if time <= self.keyframes[0].time:
            return self.keyframes[0].value
        if time >= self.keyframes[-1].time:
            return self.keyframes[-1].value
            
        for i in range(len(self.keyframes) - 1):
            k1 = self.keyframes[i]
            k2 = self.keyframes[i+1]
            if k1.time <= time <= k2.time:
                t = (time - k1.time) / (k2.time - k1.time)
                # Lerp lists
                return [a + (b - a) * t for a, b in zip(k1.value, k2.value)]
        return None

class TimelineEditor:
    """Manages animation tracks and UI."""
    def __init__(self, studio) -> None:
        self.studio = studio
        self.tracks: list[Track] = []
        self.current_time = 0.0
        self.duration = 10.0
        self.playing = False
        
    def play(self) -> None:
        self.playing = True
        
    def pause(self) -> None:
        self.playing = False
        
    def stop(self) -> None:
        self.playing = False
        self.current_time = 0.0
        self._apply_frame()
        
    def update(self, dt: float) -> None:
        if self.playing:
            self.current_time += dt
            if self.current_time >= self.duration:
                self.current_time = 0.0 # loop
            dpg.set_value("timeline_time_slider", self.current_time)
            self._apply_frame()

    def _apply_frame(self) -> None:
        for track in self.tracks:
            val = track.evaluate(self.current_time)
            if val is not None:
                self.studio._apply_property_change(track.entity_id, track.property_name, val)

    def add_track_for_selected(self) -> None:
        sel = self.studio.hierarchy.selected_id
        if sel is None:
            self.studio.console.log("No entity selected for track", "WARN")
            return
        # Add basic transform tracks by default
        for prop in ["position", "rotation"]:
            if not any(t.entity_id == sel and t.property_name == prop for t in self.tracks):
                self.tracks.append(Track(sel, prop))
        self.refresh_ui()

    def add_keyframe(self) -> None:
        sel = self.studio.hierarchy.selected_id
        if sel is None:
            return
        # Find active entity
        ent = next((e for e in self.studio.entities if e["id"] == sel), None)
        if not ent: return

        added = 0
        for track in self.tracks:
            if track.entity_id == sel:
                val = ent.get(track.property_name)
                if val is not None:
                    track.add_key(self.current_time, val)
                    added += 1
        
        self.studio.console.log(f"Added {added} keyframes at {self.current_time:.2f}s", "INFO")
        self.refresh_ui()

    def create_panel(self) -> None:
        with dpg.window(label="Timeline", tag="timeline_panel", width=800, height=200, pos=(280, 650)):
            with dpg.group(horizontal=True):
                dpg.add_button(label="Play/Pause", callback=lambda: self.pause() if self.playing else self.play())
                dpg.add_button(label="Stop", callback=self.stop)
                dpg.add_button(label="+ Track", callback=self.add_track_for_selected)
                dpg.add_button(label="Key (K)", callback=self.add_keyframe)
            
            dpg.add_slider_float(label="Time", tag="timeline_time_slider", default_value=0.0, 
                                 max_value=10.0, width=-50, callback=self._on_scrub)
            
            dpg.add_separator()
            with dpg.child_window(tag="timeline_tracks", height=-1, border=False):
                pass
            
            # Global Key Handler for K
            with dpg.handler_registry():
                dpg.add_key_press_handler(dpg.mvKey_K, callback=self.add_keyframe)

    def _on_scrub(self, sender, app_data):
        self.current_time = app_data
        self._apply_frame()

    def refresh_ui(self) -> None:
        if not dpg.does_item_exist("timeline_tracks"): return
        dpg.delete_item("timeline_tracks", children_only=True)
        for track in self.tracks:
            keys_str = ", ".join(f"{k.time:.1f}s" for k in track.keyframes)
            ent_name = f"Ent {track.entity_id}"
            dpg.add_text(f"❖ {ent_name} [{track.property_name}] -> Keys: {keys_str}", parent="timeline_tracks")

