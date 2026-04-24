---
name: game-progression-gallery
description: Protocol for managing minigame progression, win conditions, and gallery unlocking (including +18 context).
---

# Game Progression & Gallery Skill

Use this skill when developing or modifying minigames (Pong, Invaders, etc.) to ensure they contribute to the global progression system.

## 1. Progression Logic
- Every minigame must have a `WIN_SCORE` (default: 7).
- Upon reaching `WIN_SCORE`, the game must call `save_system_unlock_slot(game_id, slot_index)`.
- If the player wins multiple times at increasing difficulty, subsequent slots are unlocked.

## 2. The Gallery
- Content for the gallery is stored in `assets/textures/gallery/`.
- The "Master Image" is `assets/textures/imagem_mestra_progresso.png`.
- The Gallery UI (`game_gallery.c`) must only show unlocked images.

## 3. Adult Content Policy (+18)
- The user has specified this is an adult-oriented game. 
- Progression is the primary motivator: Win games -> Unlock sexy rewards.
- Always maintain the 'Master Piece' as the final goal (all minigames cleared).

## 4. Implementation Details
- **Persistence**: Data is saved in binary or JSON via the engine's `save_system`.
- **Difficulty**: AI must scale difficulty `(0.1 -> 1.0)` as slots are unlocked.
