# Retro Genesis Development Guide 🕹️

Starlight Mark-C features a dedicated suite for authentic 16-bit era game development.

## 1. Mode 7 (3D Projection) 🏎️
The **Modern Mode 7** allows for infinite 3D floor projection.

```c
// Rendering a Mode 7 floor
starlight_ui_draw_mode7(
    app, 
    texture_id, 
    cam_x, cam_y, 
    map_z,     // Z height
    horizon,   // Screen Y of horizon (0.0 to 1.0)
    angle,     // Yaw rotation
    pitch      // Floor tilt
);
```

---

## 2. Parallax Engine 🌄
Create depth with infinitely scrolling layers.

```c
SLF_ParallaxLayer clouds;
// Load texture and set scroll speeds (relative to camera)
starlight_parallax_layer_init(&clouds, tex_clouds, 0.2f, 0.0f, 1280, 720);

// Update in loop
starlight_parallax_layer_update(&clouds, cam_delta_x, 0);

// Draw in UI pass
starlight_ui_draw_parallax(app, &clouds, 0, 0, COLOR_WHITE);
```

---

## 3. Chiptune Synthesis
Mathematical wave generation for that perfect retro sound. No audio files needed!

```c
// Play a square wave (type 1) at 440Hz for 100ms
audio_play_chiptune(1, 440.0f, 100.0f, 0.5f);
```
**Types**: 0 (Sine), 1 (Square), 2 (Triangle), 3 (Sawtooth).

---

## 4. Tilemaps & 2D Physics
High-performance grid-based levels with AABB collision.

### Initializing
```c
SLF_Tilemap map;
starlight_tilemap_init(&map, cols, rows, 32.0f, &sprite_sheet);
starlight_tilemap_set_data(&map, level_data_array);
```

### Dynamic Collisions
Resolve player movement against solid tiles (indices > 0).
```c
starlight_physics_tilemap_collide(&map, &player_x, &player_y, width, height, &vx, &vy);
```

---

## 5. Sprite Animation
Automated frame-based animation for character sprites.

```c
SLF_SpriteSheet sheet;
starlight_sprite_sheet_init(&sheet, texture, 8, 4); // 8 cols, 4 rows

SLF_SpriteAnimator run_anim;
starlight_sprite_animator_init(&run_anim, &sheet, 0, 7, 0.1f, true);

// Update
starlight_sprite_animator_update(&run_anim, dt);

// Draw
starlight_ui_draw_sprite(app, &sheet, run_anim.current_frame, x, y, w, h, color, false);
```

---
**🕹️ Relive the magic of the golden age with modern power.**
