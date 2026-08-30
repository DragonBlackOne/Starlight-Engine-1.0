"""Camera Controllers — Orbit, Follow, and Cinematic cameras.

Usage:
    from starlight import backend
    from starlight.game.camera_system import OrbitCamera, FollowCamera

    orbit = OrbitCamera(distance=10, pitch=-30, yaw=0)
    orbit.set_target((0, 2, 0))
    # In game loop:
    orbit.update(dt, mouse_dx, mouse_dy, scroll)
    orbit.apply()  # Sends position/rotation to backend
"""
from __future__ import annotations
import math
from typing import Any, TYPE_CHECKING
from starlight import Keys, Entity

if TYPE_CHECKING:
    from starlight.app import App

try:
    from starlight import backend
except ImportError:
    backend = None  # type: ignore


class OrbitCamera:
    """Third-person orbit camera that rotates around a target point.

    Example:
        cam = OrbitCamera(distance=8, pitch=-25)
        cam.set_target((0, 1.5, 0))
        cam.update(dt, mouse_dx, mouse_dy, scroll_delta)
        cam.apply()
    """

    def __init__(self, distance: float = 10.0, pitch: float = -30.0,
                 yaw: float = 0.0, min_pitch: float = -85.0,
                 max_pitch: float = 85.0, min_dist: float = 2.0,
                 max_dist: float = 50.0, sensitivity: float = 0.2,
                 zoom_speed: float = 2.0) -> None:
        self.distance = distance
        self.pitch = pitch
        self.yaw = yaw
        self.min_pitch = min_pitch
        self.max_pitch = max_pitch
        self.min_dist = min_dist
        self.max_dist = max_dist
        self.sensitivity = sensitivity
        self.zoom_speed = zoom_speed
        self.target = [0.0, 0.0, 0.0]
        self.position = [0.0, 0.0, 0.0]
        self._cam_id: int | None = None

    def set_target(self, pos: tuple[float, float, float]) -> None:
        self.target = list(pos)

    def update(self, dt: float, mouse_dx: float = 0, mouse_dy: float = 0,
               scroll: float = 0) -> None:
        self.yaw += mouse_dx * self.sensitivity
        self.pitch -= mouse_dy * self.sensitivity
        self.pitch = max(self.min_pitch, min(self.max_pitch, self.pitch))
        self.distance -= scroll * self.zoom_speed
        self.distance = max(self.min_dist, min(self.max_dist, self.distance))

        pitch_rad = math.radians(self.pitch)
        yaw_rad = math.radians(self.yaw)

        self.position[0] = self.target[0] + self.distance * math.cos(pitch_rad) * math.sin(yaw_rad)
        self.position[1] = self.target[1] + self.distance * math.sin(-pitch_rad)
        self.position[2] = self.target[2] + self.distance * math.cos(pitch_rad) * math.cos(yaw_rad)

    def apply(self) -> None:
        if backend is None:
            return
        if self._cam_id is None:
            self._cam_id = backend.get_main_camera_id()
        backend.set_transform(self._cam_id, *self.position)
        pitch_rad = math.radians(-self.pitch)
        yaw_rad = math.radians(self.yaw) + math.pi
        backend.set_rotation(self._cam_id, pitch_rad, yaw_rad, 0)


class FollowCamera:
    """Smooth follow camera for third-person games.

    Example:
        cam = FollowCamera(offset=(0, 5, -8), smoothing=5.0)
        cam.update(dt, player_position)
        cam.apply()
    """

    def __init__(self, offset: tuple[float, float, float] = (0, 5, -8),
                 smoothing: float = 5.0, look_ahead: float = 2.0) -> None:
        self.offset = list(offset)
        self.smoothing = smoothing
        self.look_ahead = look_ahead
        self.position = [0.0, 5.0, -8.0]
        self.target = [0.0, 0.0, 0.0]
        self._cam_id: int | None = None

    def update(self, dt: float, target_pos: tuple[float, float, float],
               target_velocity: tuple[float, float, float] = (0, 0, 0)) -> None:
        # Desired position = target + offset + look-ahead
        desired = [
            target_pos[0] + self.offset[0] + target_velocity[0] * self.look_ahead,
            target_pos[1] + self.offset[1],
            target_pos[2] + self.offset[2] + target_velocity[2] * self.look_ahead,
        ]
        # Smooth interpolation
        t = 1.0 - math.exp(-self.smoothing * dt)
        self.position[0] += (desired[0] - self.position[0]) * t
        self.position[1] += (desired[1] - self.position[1]) * t
        self.position[2] += (desired[2] - self.position[2]) * t
        self.target = list(target_pos)

    def apply(self) -> None:
        if backend is None:
            return
        if self._cam_id is None:
            self._cam_id = backend.get_main_camera_id()
        backend.set_transform(self._cam_id, *self.position)
        # Look at target
        dx = self.target[0] - self.position[0]
        dy = self.target[1] - self.position[1]
        dz = self.target[2] - self.position[2]
        dist = math.sqrt(dx * dx + dy * dy + dz * dz)
        if dist > 0.01:
            pitch = -math.asin(dy / dist)
            yaw = math.atan2(dx, -dz)
            backend.set_rotation(self._cam_id, pitch, yaw, 0)


class CinematicCamera:
    """Keyframe-based cinematic camera for cutscenes.

    Example:
        cam = CinematicCamera()
        cam.add_keyframe(0.0, position=(0, 5, 10), look_at=(0, 0, 0))
        cam.add_keyframe(3.0, position=(10, 3, 0), look_at=(0, 1, 0))
        cam.add_keyframe(6.0, position=(0, 8, -10), look_at=(0, 0, 0))
        cam.play()
    """

    def __init__(self) -> None:
        self._keyframes: list[dict[str, Any]] = []
        self._time: float = 0.0
        self.playing: bool = False
        self.looping: bool = False
        self._cam_id: int | None = None
        self._on_complete: list[Any] = []

    def add_keyframe(self, time: float, position: tuple[float, float, float],
                     look_at: tuple[float, float, float] = (0, 0, 0)) -> None:
        self._keyframes.append({"time": time, "pos": list(position), "look": list(look_at)})
        self._keyframes.sort(key=lambda k: k["time"])

    def play(self) -> None:
        self._time = 0.0
        self.playing = True

    def stop(self) -> None:
        self.playing = False

    def update(self, dt: float) -> None:
        if not self.playing or len(self._keyframes) < 2:
            return
        self._time += dt
        total = self._keyframes[-1]["time"]

        if self._time >= total:
            if self.looping:
                self._time -= total
            else:
                self.playing = False
                for cb in self._on_complete:
                    cb()
                return

        # Find surrounding keyframes
        kf_a = self._keyframes[0]
        kf_b = self._keyframes[1]
        for i in range(len(self._keyframes) - 1):
            if self._keyframes[i]["time"] <= self._time <= self._keyframes[i + 1]["time"]:
                kf_a = self._keyframes[i]
                kf_b = self._keyframes[i + 1]
                break

        # Interpolate
        span = kf_b["time"] - kf_a["time"]
        t = (self._time - kf_a["time"]) / max(span, 0.001)
        t = t * t * (3 - 2 * t)  # Smoothstep

        pos = [kf_a["pos"][i] + (kf_b["pos"][i] - kf_a["pos"][i]) * t for i in range(3)]
        look = [kf_a["look"][i] + (kf_b["look"][i] - kf_a["look"][i]) * t for i in range(3)]

        self._apply(pos, look)

    def _apply(self, pos: list[float], look: list[float]) -> None:
        if backend is None:
            return
        if self._cam_id is None:
            self._cam_id = backend.get_main_camera_id()
        backend.set_transform(self._cam_id, *pos)
        dx = look[0] - pos[0]
        dy = look[1] - pos[1]
        dz = look[2] - pos[2]
        dist = math.sqrt(dx * dx + dy * dy + dz * dz)
        if dist > 0.01:
            pitch = -math.asin(dy / dist)
            yaw = math.atan2(dx, -dz)
            backend.set_rotation(self._cam_id, pitch, yaw, 0)

    def on_complete(self, callback: Any) -> None:
        self._on_complete.append(callback)


class SpectatorCamera:
    """
    A simple spectator camera controller (WASD + Mouse).
    """
    def __init__(self, app: App, position: list[float] | None = None) -> None:
        self.app = app
        self.id: int | None = None
        self.entity: Entity | None = None

        try:
            self.id = backend.get_main_camera_id()
            self.entity = Entity(self.id) # Use main camera ID
        except Exception:
            return

        if position and self.entity.transform:
            self.entity.set_position(*position)

        self.yaw: float = -90.0
        self.pitch: float = 0.0
        self.speed: float = 10.0
        self.sensitivity: float = 0.1

    def update(self, dt: float) -> None:
        if not self.entity:
            return

        # Mouse Look
        mouse_dx, mouse_dy = backend.get_mouse_delta()

        if mouse_dx != 0 or mouse_dy != 0:
            self.yaw += mouse_dx * self.sensitivity
            self.pitch -= mouse_dy * self.sensitivity # Inverted Y

            # Clamp pitch
            self.pitch = max(-89.0, min(89.0, self.pitch))

            # Send rotation to backend
            rad_yaw = math.radians(self.yaw)
            rad_pitch = math.radians(self.pitch)
            self.entity.set_rotation(rad_pitch, rad_yaw, 0.0)

        # Keyboard Move
        rad_yaw = math.radians(self.yaw)

        forward_x = math.cos(rad_yaw)
        forward_z = math.sin(rad_yaw)

        right_x = -forward_z
        right_z = forward_x

        dx = 0.0
        dz = 0.0

        if self.app.input.is_key_down(Keys.W):
            dx += forward_x
            dz += forward_z
        if self.app.input.is_key_down(Keys.S):
            dx -= forward_x
            dz -= forward_z
        if self.app.input.is_key_down(Keys.A):
            dx -= right_x
            dz -= right_z
        if self.app.input.is_key_down(Keys.D):
            dx += right_x
            dz += right_z

        if dx != 0 or dz != 0:
            # Normalize
            length = math.sqrt(dx*dx + dz*dz)
            if length > 0:
                dx /= length
                dz /= length
            
            move_speed = self.speed * dt
            
            # Apply movement relative to current position
            pos = self.entity.transform.position
            new_x = pos[0] + dx * move_speed
            new_y = pos[1] 
            new_z = pos[2] + dz * move_speed
            
            self.entity.set_position(new_x, new_y, new_z)
