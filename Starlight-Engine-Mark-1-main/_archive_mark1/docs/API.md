# Starlight Engine API Reference

## `starlight.App`

The main entry point for creating a game application.

### `class App(title: str, width: int, height: int)`

**Methods:**
- `run()`: Starts the game loop. This method blocks until the window is closed.
- `on_start()`: Called once when the engine initializes. Override this to set up your scene.
- `on_update(dt: float)`: Called every frame. `dt` is the time in seconds since the last frame.
- `on_render()`: Called every frame after update. Use this for custom drawing commands (UI, etc).

**Properties:**
- `world`: Access to the `WorldProxy` for global environment controls.
- `input`: Access to the `InputProxy` for keyboard/mouse state.

---

## `starlight.Entity`

Represents a game object in the world.

### `static spawn(app, x, y, z, scale=1.0, model="cube") -> Entity`
Spawns a new entity.
- `model`: "cube", "floor", "ship", or empty for an invisible node.

### Properties
- `transform`: Returns a `Transform` object with `x`, `y`, `z`.
- `scale`: The scale of the entity (list `[x, y, z]`).

### Methods
- `update_transform()`: Pushes changes made to `.transform` back to the native engine (and physics system).

---

## `starlight.WorldProxy`

Controls global environment settings. Accessed via `app.world`.

- `set_sun_direction(x, y, z)`: Sets the direction of the main directional light.
- `set_fog(density, r, g, b)`: Enables atmospheric fog. `density` > 0 enables it.
- `spawn_ground(x, y, z, scale)`: Helper to spawn a static physics floor.

---

## `starlight.Keys`

Constants for keyboard input.

- `Keys.W`, `Keys.A`, `Keys.S`, `Keys.D`
- `Keys.SPACE`
- `Keys.Q`, `Keys.E`
- `Keys.R`

---

## `starlight.math`

Helper math functions.

- `look_at(eye, target, up)`: Returns a 4x4 View Matrix.
- `perspective(fov, aspect, near, far)`: Returns a 4x4 Projection Matrix.
