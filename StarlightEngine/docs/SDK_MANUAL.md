# Starlight Engine SDK: Development Manual 📑

> This project is AI-driven with human creative vision.

Welcome to the **Starlight Engine SDK** manual. This document teaches you how to create games using the **SBA v2.0** framework.

---

## 🏁 1. Creating a New Project

```powershell
cd StarlightEngine
.\create_project.ps1 -ProjectName "MyGame"
```

This creates a fully configured project with:
- `src/main.cpp`: Game entry point with BaseScene boilerplate.
- `assets/scripts/core.lua`: Standard library (MathX, Physics2D, Timer, Color, ScreenShake, ValueTween).
- `assets/scripts/sba_bridge.lua`: Game SDK (Entity, Light, Tween, Scene, Events, Coroutine).
- `assets/scripts/MyGame_main.lua`: Starter script with Scene+Entity+Tween ready to go.
- `CMakeLists.txt`: Pre-configured build with StarlightCore linkage.
- `README.md`: Project documentation.

---

## 🎮 2. SBA v2.0 — Game Development Framework

### Entity System (3D Objects)
```lua
local player = Entity("Player", 0, 1, 0)
player:setColor(0, 1, 1)        -- Cyan
player:setScale(1, 2, 1)         -- Tall
player:setMaterial(0.8, 0.2)     -- Metallic, smooth
player:move(0, 0, -5 * dt)       -- Move forward
local x, y, z = player:getPos()  -- Read position
player:destroy()                  -- Remove from world
```

### Light System
```lua
local sun = Light(0, 15, 0, 1, 0.9, 0.7, 1200)
sun:setColor(0.3, 0, 0.8)       -- Purple light
sun:setIntensity(800)             -- Dim it
```

### Scene Manager (State Machine)
```lua
Scene.register("Menu", {
    onEnter = function() Say("Menu loaded!") end,
    onUpdate = function(dt) end,
    onRenderUI = function() end,
    onExit = function() end,
})
Scene.register("Game", { ... })
Scene.switch("Menu")  -- Transition with onExit → onEnter
```

### Tween System (8 Easing Functions)
```lua
Tween.to(building, { y = 5.0, scaleY = 4.0 }, 1.5, "easeOutElastic")
-- Available: linear, easeInQuad, easeOutQuad, easeInOutQuad,
--            easeInCubic, easeOutCubic, easeInOutCubic,
--            easeOutBounce, easeOutElastic
```

### Event Bus (Pub/Sub)
```lua
Events.on("player_hit", function(data) 
    Say("Damage: " .. data.amount) 
end)
Events.emit("player_hit", { amount = 25 })
```

### Coroutine Runner
```lua
Coroutine.start(function()
    Say("Loading...")
    Coroutine.wait(2.0)
    Say("Ready!")
end)
```

### Color Utilities
```lua
local r, g, b = Color.hsv(0.5, 0.8, 1.0)               -- HSV → RGB
local r, g, b = Color.pulse(1, 0, 0, time, 5, 0.3)      -- Pulsing red
local r, g, b = Color.lerpRGB(r1,g1,b1, r2,g2,b2, 0.5)  -- 50% blend
```

### ScreenShake
```lua
ScreenShake.trigger(15, 0.3)      -- Intensity 15, duration 0.3s
ScreenShake.update(dt)             -- Call each frame
local ox, oy = ScreenShake.getOffset()  -- Apply to rendering
```

---

## 🏛️ 3. C++ Scene Architecture

```cpp
class MyGame : public starlight::BaseScene {
    void OnEnter() override {
        auto& scripting = Engine::Get().GetScripting();
        scripting.ExecuteFile("assets/scripts/mygame_main.lua");
        sol::function onStart = scripting.GetLua()["OnStart"];
        if (onStart.valid()) onStart();
    }
    void OnUpdate(float dt) override {
        auto& scripting = Engine::Get().GetScripting();
        sol::function onUpdate = scripting.GetLua()["OnUpdate"];
        if (onUpdate.valid()) onUpdate(dt);
    }
};
```

---

## 🛠️ 4. Build & Distribution

```powershell
cmake -B build -S .
cmake --build build --config Release
.\build\Release\MyGame.exe
```

### Build All Projects
```powershell
# From repository root:
.\build_all.ps1
# Automatically syncs SBA framework + builds Engine + all games
```

---

## 📚 5. Standard Library Reference (core.lua)

| Module | Functions |
|--------|-----------|
| **MathX** | `clamp`, `lerp`, `sign`, `distance`, `distance3D`, `smoothstep`, `remap`, `random_range`, `normalize2D`, `angle`, `wrap` |
| **Physics2D** | `CheckAABB`, `CheckCircle`, `PointInRect`, `RayCircle` |
| **Timer** | `Timer.after(sec, fn)`, `Timer.every(sec, fn)`, `Timer.cancel(id)` |
| **Color** | `Color.hsv(h,s,v)`, `Color.pulse(r,g,b,t,speed,intensity)`, `Color.lerpRGB(...)` |
| **ScreenShake** | `trigger(intensity, duration)`, `update(dt)`, `getOffset()` |
| **ValueTween** | `ValueTween.to(table, key, endVal, duration, easing)`, `ValueTween.update(dt)` |
| **Class** | `Class(base)` — OO factory with `is_a()` inheritance checking |

---
**Support**: Consult `StarlightEngine/src/core/ScriptSystem.cpp` for C++ binding details.
