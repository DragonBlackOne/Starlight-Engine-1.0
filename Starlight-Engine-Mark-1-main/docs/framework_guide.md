# Starlight Framework (SLF) Guide 🏛️

The Starlight Framework (SLF) is the high-level API designed for ergonomic and rapid game development in C11.

## 1. Scene Management (The Stack)
Forget global `switch` statements for game states. SLF uses a **Scene Stack**. 
You can `push` a menu over a game, and `pop` it to return exactly where you were.

### Defining a Scene
```c
void GameEnter(SLF_App* app) { /* Load assets */ }
void GameUpdate(SLF_App* app, float dt) { /* Logic */ }
void GameDraw(SLF_App* app) { /* 3D Render */ }
void GameUI(SLF_App* app) { /* 2D Interface */ }

SLF_Scene game_scene = {
    .on_enter = GameEnter,
    .on_update = GameUpdate,
    .on_draw = GameDraw,
    .on_ui = GameUI
};
```

### Navigating
- `starlight_scene_push(app, &scene)`: Adds a new scene on top.
- `starlight_scene_pop(app)`: Removes the top scene.
- `starlight_scene_change(app, &scene)`: Replaces the current scene.

---

## 2. Asset Pool (VRAM Hygiene)
The `SLF_AssetPool` ensures that textures, fonts, and sounds are cached and reference-counted.

```c
// This will load the texture from disk
GLuint tex1 = slf_asset_pool_load_texture("player.png");

// This will return the EXACT SAME resource from memory (O(1) cache hit)
GLuint tex2 = slf_asset_pool_load_texture("player.png");
```

---

## 3. Action Mapping (Ergonomic Input)
Bind multiple physical keys/buttons to a single virtual "Action".

```c
// Initialization
starlight_bind_action(app, "jump", SDL_SCANCODE_SPACE, SDL_CONTROLLER_BUTTON_A);
starlight_bind_action_key(app, "jump", SDL_SCANCODE_W); // Add another bind

// Usage in Update
if (starlight_is_action_just_pressed(app, "jump")) {
    player_jump();
}
```

---

## 4. Tweening & Easing (Animations)
Smooth mathematical transitions without writing complex math.

```c
SLF_Tween my_tween;

// Start: Animating 'alpha' from 0 to 1 over 0.5 seconds with a Cubic curve
slf_tween_start(&my_tween, &ui_alpha, 0.0f, 1.0f, 0.5f, slf_ease_out_cubic);

// Update every frame
slf_tween_update(&my_tween, dt);
```

### Available Curves:
`linear`, `in_quad`, `out_quad`, `in_out_quad`, `in_cubic`, `out_cubic`, `out_elastic`, `out_bounce`, and more.

---

## 5. UI Anchors (Responsive Layouts)
Position UI elements relative to screen corners or edges.

```c
float x, y;
// Position a 200x50 box in the bottom right with a 20px margin
starlight_ui_anchor_pos(app, SLF_ANCHOR_BOTTOM_RIGHT, 20, 20, 200, 50, &x, &y);
starlight_ui_draw_rect(app, x, y, 200, 50, color);
```

---

## 6. Audio Systems (Miniaudio Powered) 🔊
The engine features a dual-layer audio system supporting both global SFX/Music and spatial 3D audio.

### Master Control & Chiptune
```c
// Set overall gain
audio_set_master_volume(0.8f);

// Background Music
audio_play_music("bgm.mp3", 1.0f);

// Retro Sound Synthesis (0: Sine, 1: Square, 2: Triangle, 3: Saw)
audio_play_chiptune(1, 440.0f, 100.0f, 0.5f); 
```

### Spatial 3D Audio
```c
AudioSource* loop = audio_create_source("fire.wav");
audio_source_set_position(loop, (vec3){10, 0, 5});
audio_source_play(loop, true, 1.0f);
```

---

## 7. SIMD Particle Engine ⚡
Optimized for massive quantities of 2D/3D sprites using mathematical batching.

### Emission & Lifecycle
```c
// Emits 10 particles at (x,y) with a specific color
emit(player_x, player_y, COLOR_CYAN, 10);
```

### Rendering (Instanced)
SLF uses `starlight_ui_draw_rects_instanced` to draw thousands of particles in a single Draw Call, ensuring 60FPS even on low-end hardware.

---

**🏆 Master the SLF to build faster, cleaner, and better.**
