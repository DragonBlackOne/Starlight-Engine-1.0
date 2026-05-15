# Starlight Engine SDK: Manual de Desenvolvimento 📑

> Este projeto é feito por IA e só o prompt é feito por um humano.

Bem-vindo ao manual do **Starlight Engine SDK**. Este documento ensina como criar jogos usando o framework **SBA v2.0**.

---

## 🏁 1. Criando um Novo Projeto

```powershell
cd StarlightEngine
.\create_project.ps1 -ProjectName "MeuJogo"
```

Isso cria um projeto completo com:
- `src/main.cpp`: Ponto de entrada com boilerplate BaseScene.
- `assets/scripts/core.lua`: Biblioteca padrão (MathX, Physics2D, Timer, Color, ScreenShake, ValueTween).
- `assets/scripts/sba_bridge.lua`: SDK de jogos (Entity, Light, Tween, Scene, Events, Coroutine).
- `assets/scripts/MeuJogo_main.lua`: Script inicial com Scene+Entity+Tween prontos.
- `CMakeLists.txt`: Build pré-configurado.

---

## 🎮 2. SBA v2.0 — Framework de Desenvolvimento

### Sistema de Entidades (Objetos 3D)
```lua
local jogador = Entity("Jogador", 0, 1, 0)
jogador:setColor(0, 1, 1)        -- Ciano
jogador:setScale(1, 2, 1)         -- Alto
jogador:setMaterial(0.8, 0.2)     -- Metálico, liso
jogador:move(0, 0, -5 * dt)       -- Mover para frente
jogador:destroy()                  -- Remover do mundo
```

### Gerenciador de Cenas
```lua
Scene.register("Menu", {
    onEnter = function() Say("Menu carregado!") end,
    onUpdate = function(dt) end,
    onRenderUI = function() end,
})
Scene.switch("Menu")
```

### Sistema de Tween (8 Funções de Easing)
```lua
Tween.to(predio, { y = 5.0 }, 1.5, "easeOutElastic")
```

### Bus de Eventos
```lua
Events.on("player_hit", function(data) Say("Dano: " .. data.amount) end)
Events.emit("player_hit", { amount = 25 })
```

---

## 🛠️ 3. Compilação

```powershell
cmake -B build -S .
cmake --build build --config Release
.\build\Release\MeuJogo.exe
```

### Compilar Todos os Projetos
```powershell
.\build_all.ps1
# Sincroniza framework SBA + compila Engine + todos os jogos
```

---

## 📚 4. Referência da Biblioteca Padrão (core.lua)

| Módulo | Funções |
|--------|---------|
| **MathX** | `clamp`, `lerp`, `sign`, `distance`, `distance3D`, `smoothstep`, `remap`, `random_range`, `normalize2D`, `angle`, `wrap` |
| **Physics2D** | `CheckAABB`, `CheckCircle`, `PointInRect`, `RayCircle` |
| **Timer** | `Timer.after(seg, fn)`, `Timer.every(seg, fn)`, `Timer.cancel(id)` |
| **Color** | `Color.hsv(h,s,v)`, `Color.pulse(r,g,b,t,vel,intensidade)`, `Color.lerpRGB(...)` |
| **ScreenShake** | `trigger(intensidade, duração)`, `update(dt)`, `getOffset()` |
| **ValueTween** | `ValueTween.to(tabela, chave, valorFinal, duração, easing)` |
| **Class** | `Class(base)` — Fábrica OO com herança verificável `is_a()` |
