# Starlight Engine (Python API)

The `starlight` package provides a high-level wrapper around the `starlight_rust` core.

## 📦 `starlight.App`

The main entry point for creating applications.

```python
from starlight import App

class MyGame(App):
    def on_start(self):
        print("Game Started!")
        
    def on_update(self, dt):
        print(f"Frame time: {dt}")
        
    def on_render(self):
        pass # Custom render commands
```

### Methods
- **`__init__(self, title, width, height)`**: Configure window.
- **`run()`**: Starts the engine loop. **Blocks** until the window closes.

---

## 🌎 `starlight.World` (The ECS)

Accessed via `self.world` in your App. This is a direct binding to the Rust `World` struct.

### Entity Management
- **`spawn_entity(x, y, z)` -> int**: Creates a new entity and returns its ID.
- **`set_sprite(id, texture_id, w, h)`**: Attaches a sprite to an entity.
- **`set_visual_params(ibl_intensity, skybox_intensity)`**: Controls global lighting.
- **`quit()`**: Requests the engine to close the window gracefully.

### Camera
- **`update_camera(view_proj_matrix, camera_pos)`**: Updates the camera uniform buffer.
    *   *Usage:* Use helpful math functions in `starlight.math` to generate these matrices.

---

## ⌨️ `starlight.Input`

Accessed via `self.input`.

- **`is_key_down(key_code)`**: Check if a key is held.
- **`is_key_pressed(key_code)`**: Check if a key was just pressed this frame.
- **`get_mouse_pos()` -> (x, y)**: Screen coordinates.

### Key Codes
Available in `starlight.Keys`.
- `Keys.W`, `Keys.A`, `Keys.S`, `Keys.D`
- `Keys.SPACE`, `Keys.ESCAPE`, etc.

---

## 📐 `starlight.math`

Helper module for 3D mathematics (Wraps NumPy).

- **`create_perspective_projection(width, height, fov)`**: Returns 4x4 Proj Matrix.
- **`look_at(eye, target, up)`**: Returns 4x4 View Matrix.
- **`multiply_matrices(a, b)`**: Multiplies two 4x4 matrices.