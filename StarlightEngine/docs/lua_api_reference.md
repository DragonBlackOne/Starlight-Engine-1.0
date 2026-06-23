# Lua API Reference — Fusion ENGINE

This document details all the Lua bindings exposed by the **StarlightEngine SDK (Fusion ENGINE)** for script-driven game development.

---

## 🛠️ Engine Control (`Engine`)

Methods for controlling the main game loop, window settings, and scene queries.

### `Engine.quit()`

Closes the application and performs a clean shutdown of all systems.

* **Returns:** `void`

### `Engine.set_title(title)`

Updates the window title dynamically at runtime.

* **Arguments:**
  * `title` (string): The new title for the window.

* **Returns:** `void`

### `Engine.set_fullscreen(enable)`

Enables or disables window fullscreen mode.

* **Arguments:**
  * `enable` (boolean): `true` to go fullscreen, `false` for windowed mode.

* **Returns:** `void`

### `Engine.get_fps()`

Returns the current frames-per-second calculated by the engine's main loop.

* **Returns:** `number` (float)

### `Engine.set_pause(paused)`

Pauses or resumes the execution of systems (e.g. physics updates).

* **Arguments:**
  * `paused` (boolean): `true` to pause, `false` to resume.

* **Returns:** `void`

### `Engine.get_entity_count()`

Returns the number of active entities in the EnTT registry.

* **Returns:** `number` (integer)

### `Engine.find_by_tag(tag)`

Searches for an entity containing a matching `TagComponent`.

* **Arguments:**
  * `tag` (string): The tag to search for.

* **Returns:** `Entity` wrapper (or `nil` if not found)

### `Engine.set_rotation(entity, rx, ry, rz)`

Sets the absolute rotation of the given entity.

* **Arguments:**
  * `entity` (Entity): The target entity.
  * `rx`, `ry`, `rz` (number): Euler angles in degrees.

* **Returns:** `void`

### `Engine.get_scale(entity)`

Gets the current local scale of the entity.

* **Arguments:**
  * `entity` (Entity): The target entity.
  * `x`, `y`, `z` (number): The scale dimensions.

* **Returns:** `table` containing `{x, y, z}`

### `Engine.set_visible(entity, visible)`

Sets whether the entity's render components should be drawn.

* **Arguments:**
  * `entity` (Entity): The target entity.
  * `visible` (boolean): Visibility flag.

* **Returns:** `void`

---

## 🎨 Graphics (`gfx`)

Methods for drawing 2D primitives, setting clear color, and controlling camera systems.

### `gfx.set_clear_color(r, g, b)`

Sets the background clear color.

* **Arguments:**
  * `r`, `g`, `b` (number): Color components in `[0.0, 1.0]` range.

* **Returns:** `void`

### `gfx.draw_triangle(x1, y1, x2, y2, x3, y3, r, g, b, a)`

Draws a colored 2D triangle.

* **Arguments:**
  * `x1, y1`, `x2, y2`, `x3, y3` (number): Vertex coordinates.
  * `r, g, b, a` (number): Color and alpha components.

* **Returns:** `void`

### `gfx.draw_rounded_rect(x, y, w, h, radius, r, g, b, a)`

Draws a colored 2D rectangle with rounded corners.

* **Arguments:**
  * `x, y` (number): Top-left position.
  * `w, h` (number): Width and height.
  * `radius` (number): Corner radius.
  * `r, g, b, a` (number): Color and alpha components.

* **Returns:** `void`

### `gfx.set_camera_2d(x, y, zoom)`

Configures the 2D view matrix for 2D rendering.

* **Arguments:**
  * `x, y` (number): Camera position.
  * `zoom` (number): Scale factor.

* **Returns:** `void`

---

## 🔊 Audio (`audio`)

Sound and music controls.

### `audio.set_music_volume(volume)`

Sets the background music volume.

* **Arguments:**
  * `volume` (number): Volume in `[0.0, 1.0]` range.

* **Returns:** `void`

### `audio.is_music_playing()`

Checks if a music track is currently active.

* **Returns:** `boolean`

---

## ⌨️ Input (`input`)

Keyboard, Mouse, and Gamepad polling.

### `input.is_mouse_down(button)`

Checks if a specific mouse button is currently held down.

* **Arguments:**
  * `button` (string): `"Left"`, `"Right"`, or `"Middle"`.

* **Returns:** `boolean`

### `input.get_scroll()`

Returns the scroll wheel delta of the mouse.

* **Returns:** `number` (float)

---

## 📹 Camera 3D (`camera`)

Methods for inspecting and setting the main 3D camera.

### `camera.get_pos()`

Gets the current 3D position of the camera.

* **Returns:** `table` containing `{x, y, z}`

### `camera.get_rotation()`

Gets the current 3D rotation (pitch/yaw/roll) of the camera.

* **Returns:** `table` containing `{pitch, yaw, roll}`

---

## ⏱️ Time (`time`)

Control over the simulation time scale.

### `time.set_scale(scale)`

Sets the speed of the game's clock (useful for slow-motion or fast-forward).

* **Arguments:**
  * `scale` (number): Scale factor (e.g. `0.5` for half speed, `2.0` for double speed).

* **Returns:** `void`

### `time.get_scale()`

Returns the current simulation time scale.

* **Returns:** `number` (float)

---

## ⚛️ Physics (`physics`)

Control over Jolt Physics colliders, collision reporting, and raycasting.

### `physics.on_collision(callback)`

Registers a Lua function to be called on every physics collision event.

* **Arguments:**
  * `callback` (function): The callback function. It receives `(entityA, entityB, px, py, pz, nx, ny, nz)`.

* **Returns:** `void`

### `physics.create_box(entity, w, h, d, is_static)`

Creates a Jolt box collider component and attaches it to the entity.

* **Arguments:**
  * `entity` (Entity): Target entity.
  * `w, h, d` (number): Box dimensions.
  * `is_static` (boolean): `true` if static/immovable, `false` for dynamic rigid bodies.

* **Returns:** `void`

### `physics.raycast(ox, oy, oz, dx, dy, dz, max_dist)`

Casts a ray into the physical world and returns the closest hit entity.

* **Arguments:**
  * `ox, oy, oz` (number): Ray origin.
  * `dx, dy, dz` (number): Ray direction vector.
  * `max_dist` (number): Maximum search distance.

* **Returns:** `Entity` wrapper (or `nil` if no hit)
