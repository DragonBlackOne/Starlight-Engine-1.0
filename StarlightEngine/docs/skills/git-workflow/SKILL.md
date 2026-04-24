---
name: git-workflow
description: Standards for git commits and branching in this project.
---

# Git Workflow

## 1. Commit Messages
Use present tense imperative:
- "Add physics system" (Good)
- "Added physics" (Avoid)
- "Fixing bug" (Avoid)

Prefixes:
- `feat:` New features
- `fix:` Bug fixes
- `refactor:` Code cleanup
- `docs:` Documentation

## 2. Ignore Rules
Never commit:
- `__pycache__/`
- `target/`
- `*.pyd` (unless specifically tracking binary releases)
- `*.log`


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
