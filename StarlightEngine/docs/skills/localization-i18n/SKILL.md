---
name: localization-i18n
description: Handling multiple languages in the Starlight Engine.
---

# Localization (i18n)

## Approach
Use JSON files per language:

```
assets/localization/
├── en.json    # English (default)
├── pt.json    # Portuguese
└── es.json    # Spanish
```

## Format
```json
{
    "menu.start": "Start Game",
    "menu.options": "Options",
    "menu.quit": "Quit",
    "hud.health": "Health: {value}",
    "dialog.greeting": "Hello, traveler!"
}
```

## Usage in Python
```c
import json

class Localizer:
    def __init__(self, lang: str = "en"):
        path = f"assets/localization/{lang}.json"
        with open(path) as f:
            self.strings = json.load(f)

    def get(self, key: str, **kwargs) -> str:
        text = self.strings.get(key, f"[{key}]")
        for k, v in kwargs.items():
            text = text.replace(f"{{{k}}}", str(v))
        return text

# Usage
i18n = Localizer("pt")
print(i18n.get("hud.health", value=85))  # "Saúde: 85"
```

## Best Practices
- Never hardcode user-facing strings
- Use dot-notation keys: `category.item`
- Support variables with `{name}` placeholders
- Keep default (en) complete; other languages can fall back


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
