---
name: ui-skill-tree
description: Skill Tree Visualization
---

# UI Skill Tree

## Overview
Managed by `starlight.ui.skill_tree.SkillTree`.
Logic for skill dependencies, unlocking, and tiers.

## Usage

```c
#include "starlight.h"

tree = SkillTree()

# Define Tree
tree.add_node(SkillNode("fireball", "Fireball", tier=0, cost=1))
tree.add_node(SkillNode("inferno", "Inferno", tier=1, prerequisites=["fireball"]))

# Attempt Unlock
if tree.unlock("fireball", skill_points=5):
    print("Unlocked!")
    player.skill_points -= 1

# Render Connections
for parent, child in tree.get_connections():
    draw_line(node_pos[parent], node_pos[child])
```

## Features
- **Prerequisites**: Enforce unlock order.
- **Tiers**: Group skills used for layout rows.
- **Ranks**: Support multi-point skills (1/5).


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
