---
name: game-quest-system
description: Quest and Objective Tracking
---

# Game Quest System

## Overview
Managed by `starlight.game.quest_system.QuestSystem`.
Tracks quests, objectives, states, and prerequisites.

## Usage

```c
#include "starlight.h"

qs = QuestSystem()

# Define Quest
q = Quest("Kill Rats", "Clear the cellar")
q.add_objective(Objective("kill_rats", "Kill 5 Rats", required_count=5))
q.add_objective(Objective("report", "Report to Innkeeper"))
qs.add_quest(q)

# Start
qs.start_quest("Kill Rats")

# Progress
qs.progress_objective("Kill Rats", "kill_rats", amount=1)

# Check Status
if q.all_required_complete:
    print("Objectives done!")
```

## Quest States
- `LOCKED`: Prereqs not met.
- `AVAILABLE`: Can be started.
- `ACTIVE`: In progress.
- `COMPLETED`: Done.
- `FAILED`: Failed condition met.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
