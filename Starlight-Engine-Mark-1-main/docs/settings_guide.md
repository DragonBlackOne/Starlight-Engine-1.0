# Hub Settings & Configuration Guide ⚙️

This guide explains how to add new user-selectable options to the **Settings** menu in the Unified Hub.

## 1. Adding a New Setting
All global settings are stored in the `GameHub` struct and initialized in `Hub_OnStart`.

### Step A: Update Struct
Add your variable to `typedef struct GameHub` in `game_pong_deluxe.c`:
```c
typedef struct {
    // ... other fields
    bool chromatic_aberration_enabled;
} GameHub;
```

### Step B: Initialize
Set the default value in `Hub_OnStart`:
```c
G.chromatic_aberration_enabled = true;
```

## 2. Implementing Logic
In `Hub_OnUpdate`, under the `ST_HUB_SETTINGS` state, add handling for navigation and adjustment.

```c
if (G.settings_sel == 5) { // Assuming 5 is your new index
    if(starlight_is_action_just_pressed(app, "left") || 
       starlight_is_action_just_pressed(app, "right")) {
        G.chromatic_aberration_enabled = !G.chromatic_aberration_enabled;
    }
}
```

## 3. Rendering in UI
In `Hub_OnUI`, add the new row to the settings list.

```c
char* opts[] = {..., "CHROMATIC AB.", "RETURN"};
// ... loop through opts
if (i == 5) {
    starlight_ui_draw_text(app, f_med, G.chromatic_aberration_enabled ? "ON" : "OFF", cw-500, y, col);
}
```

## 4. Applying Settings
Simply wrap the feature logic in an `if` check:
```c
if (G.chromatic_aberration_enabled) {
    // apply shader effect
}
```

---
**💡 Centralized settings make the game more accessible and customizable for all players.**
