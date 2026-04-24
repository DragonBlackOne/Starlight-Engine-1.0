# Starlight API Reference 📖

A complete list of the essential functions for building games with Starlight Mark-C.

## Core Lifecycle
| Function | Description |
|---|---|
| `starlight_framework_init` | Initializes the engine, window, and starts the main loop. |
| `starlight_framework_shutdown` | Manually stops the engine and clean up resources. |

## Input Actions
| Function | Description |
|---|---|
| `starlight_bind_action` | Binds a virtual name to a key AND a mouse/controller button. |
| `starlight_bind_action_key` | Binds a virtual name to a specific keyboard scancode. |
| `starlight_bind_action_button` | Binds a virtual name to a controller button. |
| `starlight_is_action_pressed` | Returns true if any assigned bind is currently held down. |
| `starlight_is_action_just_pressed` | Returns true only on the first frame of a press. |

## Scene Management
| Function | Description |
|---|---|
| `starlight_scene_push` | Adds a new scene to the top of the stack. |
| `starlight_scene_pop` | Removes the current scene and returns to the one below. |
| `starlight_scene_change` | Replaces the current scene with a new one. |

## UI & 2D Rendering
| Function | Description |
|---|---|
| `starlight_ui_draw_rect` | Draws a simple colored rectangle. |
| `starlight_ui_draw_texture` | Draws a texture at specific coordinates. |
| `starlight_ui_draw_text` | Renders a string using a loaded font. |
| `starlight_ui_anchor_pos` | Calculates screen coordinates based on an anchor (e.g. SLF_ANCHOR_CENTER). |
| `starlight_ui_draw_rects_instanced` | GPU Batched draw for thousands of rectangles in one call. |

## Asset Management
| Function | Description |
|---|---|
| `slf_asset_pool_load_texture` | Loads (or retrieves from cache) a texture. |
| `slf_asset_pool_load_font` | Loads (or retrieves from cache) a TTF font. |
| `slf_asset_pool_load_sound` | Loads (or retrieves from cache) an audio sample. |

## Tween & Easing
| Function | Description |
|---|---|
| `slf_tween_start` | Initializes a new value transition. |
| `slf_tween_update` | Advances the transition based on delta time. |

---

## Audio Systems
| Function | Description |
|---|---|
| `audio_play_sfx` | Plays a one-shot global sound effect. |
| `audio_play_music` | Starts a looping background track. |
| `audio_set_master_volume` | Sets the overall engine gain (0.0 to 1.0). |
| `audio_create_source` | Creates a 3D positional audio source. |
| `audio_source_set_position` | Updates the world position of a 3D source. |
| `audio_source_play` | Triggers playback of a specific 3D source. |
| `audio_play_chiptune` | Generates a 16-bit synth wave in real-time. |

## Progression & Persistence
| Function | Description |
|---|---|
| `save_system_load` | Loads saved data from disk (Total hits, unlocks). |
| `save_system_save` | Commits current progress to persistent storage. |
| `save_system_is_unlocked` | Checks if a specific game/gallery piece is available. |
| `save_system_unlock` | Grants access to a game or gallery item. |
| `save_system_add_stat` | Increments a persistent counter (e.g. "total_hits").|

## Retro & Visual FX
| Function | Description |
|---|---|
| `starlight_ui_draw_mode7` | Modern 3D floor projection engine. |
| `starlight_ui_draw_parallax` | Infinite multi-layer scrolling renderer. |
| `starlight_parallax_layer_init` | Setup a parallax layer with specific speed. |
| `starlight_tilemap_init` | Creates a Tilemap object for grid rendering. |
| `starlight_tilemap_set_data` | Uploads an array of indices to a tilemap. |
| `starlight_physics_tilemap_collide` | Kinematic AABB resolution against tiles. |

---
**🛡️ For full data structure definitions, review the `.h` files in `/include`.**
