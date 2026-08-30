-- ============================================================================
-- STARLIGHT SHOWCASE: LAUNCHER & OUTRUN DASHBOARD (SBA v7.0 Industrial)
-- ============================================================================
require("assets/scripts/core")
require("assets/scripts/sba_bridge")

local State = {
    Time = 0,
    SelectedIdx = 1,
    TransitionAlpha = 0.0,
    CardOffset = 0.0
}

local Games = {
    {
        id = "pong",
        name = "NEON CYBER PONG",
        desc = "Vibrant synthwave local PvP pong with big-paddle power-ups and glowing physics.",
        tagline = "Local Multiplayer / C++ Physics",
        color = {0.0, 1.0, 0.9},
        stats = "Max Score: 11 | Speed: Warp"
    },
    {
        id = "snake",
        name = "CYBER SNAKE 2D",
        desc = "Classic Snake rebuilt with combo multiplier scoring, gold/toxic food types, and explosion VFX.",
        tagline = "High-Score Arcade / Speed Boosts",
        color = {0.2, 1.0, 0.4},
        stats = "Gold Food: 15% | Combo Window: 3s"
    },
    {
        id = "tetris",
        name = "QUANTUM TETRIS",
        desc = "Futuristic block-stacking puzzle game with neon visual updates and adaptive drop speeds.",
        tagline = "Retro Puzzle / Drop Speed: Hard",
        color = {1.0, 0.85, 0.1},
        stats = "Perfect Grid Alignment / 2D Ortho"
    },
    {
        id = "capital",
        name = "CAPITAL ODYSSEY",
        desc = "Starlight's 3D economic market simulation with dynamic news events, portfolio trading, and real-time PBR rendering.",
        tagline = "3D Economic Simulation / PBR",
        color = {1.0, 0.5, 0.0},
        stats = "Real-time News / Jolt Physics"
    },
    {
        id = "fight",
        name = "FUSION FIGHT",
        desc = "Dynamic fighting game demo with character select screens, custom stages, and native particle hit sparks.",
        tagline = "2D Fighting Arena / Action VFX",
        color = {1.0, 0.0, 0.6},
        stats = "Obsidian Floor / Sprite Sheets"
    }
}

function OnStart()
    Engine.log("Launcher: Started Outrun Dashboard.")
    Audio.playPowerup()
end

function OnUpdate(dt)
    State.Time = State.Time + dt
    ScreenShake.update(dt)
    
    -- Smooth transition for the carousel card slide
    local targetOffset = (State.SelectedIdx - 1) * -380
    State.CardOffset = MathX.lerp(State.CardOffset, targetOffset, 12 * dt)
    
    -- Carousel controls
    if Input.isJustPressed("D") or Input.isJustPressed("Right") then
        State.SelectedIdx = State.SelectedIdx + 1
        if State.SelectedIdx > #Games then State.SelectedIdx = 1 end
        Audio.beep(600, 0.05, 0)
    elseif Input.isJustPressed("A") or Input.isJustPressed("Left") then
        State.SelectedIdx = State.SelectedIdx - 1
        if State.SelectedIdx < 1 then State.SelectedIdx = #Games end
        Audio.beep(500, 0.05, 0)
    end
    
    -- Enter to launch
    if Input.isJustPressed("Return") or Input.isJustPressed("Space") then
        local game = Games[State.SelectedIdx]
        Audio.playCoin()
        Engine.load_game(game.id)
    end
end

local function DrawRetroGrid()
    local time = State.Time
    local w, h = window.get_width(), window.get_height()
    
    -- Drawing a wireframe landscape/grid at the bottom half
    local horizon = 380
    gfx.draw_rect(0, horizon, w, h - horizon, 0.02, 0.01, 0.04)
    
    -- Perspective grid lines
    local numLines = 24
    for i = 0, numLines do
        local x_start = (w / numLines) * i
        local x_end = w/2 + (x_start - w/2) * 5.0
        gfx.draw_line(x_start, horizon, x_end, h, 2.0, 0.7, 0.0, 0.5, 0.3)
    end
    
    -- Scrolling horizontal lines
    local lineSpacing = 32
    local scroll = (time * 80) % lineSpacing
    for y = horizon, h, 6 do
        local progress = (y - horizon) / (h - horizon)
        local py = horizon + (progress * progress) * (h - horizon) + scroll
        if py < h then
            local alpha = progress * 0.45
            gfx.draw_line(0, py, w, py, 1.5, 0.0, 0.8, 1.0, alpha)
        end
    end
end

local function DrawSynthwaveSun()
    local w, h = window.get_width(), window.get_height()
    local cx, cy = w / 2, 280
    local radius = 130
    
    -- Draw glowing background
    gfx.draw_circle(cx, cy, radius + 20, 1.0, 0.0, 0.5, 36, 0.15)
    
    -- Sun slices with gradient (Neon Pink to Golden Sun Orange)
    local steps = 30
    for i = 0, steps do
        local sliceH = radius * 2 / steps
        local sy = cy - radius + i * sliceH
        
        -- Slicing/spacing effect at the bottom of the sun
        local spacing = 1.0
        if sy > cy then
            local ratio = (sy - cy) / radius
            spacing = 1.0 - (ratio * 0.75)
        end
        
        local t = i / steps
        local r, g, b = Color.lerpRGB(1.0, 0.0, 0.5, 1.0, 0.7, 0.0, t)
        
        local hVal = sliceH * spacing
        local wVal = math.sqrt(radius*radius - (sy-cy)*(sy-cy)) * 2.0
        if wVal > 0 then
            gfx.draw_rect(cx - wVal/2, sy, wVal, hVal, r, g, b)
        end
    end
end

function OnRenderUI()
    local w, h = window.get_width(), window.get_height()
    local ox, oy = ScreenShake.getOffset()
    
    -- 1. Background
    gfx.draw_rect(0, 0, w, h, 0.03, 0.02, 0.05)
    
    -- Draw Synthwave Sun and Grid
    DrawSynthwaveSun()
    DrawRetroGrid()
    
    -- 2. Reflection on the floor
    gfx.draw_reflected_rect(w/2 - 130, 280, 260, 130, 1.0, 0.0, 0.6, 0.35, 380, State.Time)
    
    -- 3. Header Title
    local tr, tg, tb = Color.hsv((State.Time * 0.05) % 1.0, 0.7, 1.0)
    gfx.draw_rect(w/2 - 300, 40, 600, 4, tr, tg, tb)
    gfx.draw_rect(w/2 - 300, 110, 600, 2, tr*0.4, tg*0.4, tb*0.4)
    
    imgui.text(w/2 - 160, 50, tr, tg, tb, "STARLIGHT SHOWCASE")
    imgui.text(w/2 - 175, 85, 0.0, 0.9, 1.0, "Retro Synthwave Outrun Dashboard")
    
    -- 4. Carousel of Game Cards
    local cardW, cardH = 340, 220
    local startX = w / 2 - cardW / 2
    local startY = 410
    
    for idx, game in ipairs(Games) do
        local localX = startX + (idx - 1) * 380 + State.CardOffset
        local centerDist = math.abs(localX - startX)
        local scale = math.max(0.75, 1.0 - (centerDist / 800))
        local alpha = math.max(0.2, 1.0 - (centerDist / 600))
        
        local isSelected = (idx == State.SelectedIdx)
        local glow = isSelected and (1.0 + math.sin(State.Time * 8) * 0.25) or 0.5
        local borderR = game.color[1] * glow
        local borderG = game.color[2] * glow
        local borderB = game.color[3] * glow
        
        local finalW = cardW * scale
        local finalH = cardH * scale
        local finalX = localX + (cardW - finalW) / 2
        local finalY = startY + (cardH - finalH) / 2
        
        -- Translucent obsidian glass
        gfx.draw_rect_alpha(finalX, finalY, finalW, finalH, 0.02, 0.01, 0.04, 0.85 * alpha)
        
        -- Border
        gfx.draw_rect_outline(finalX, finalY, finalW, finalH, 2.0, borderR, borderG, borderB, alpha)
        
        -- Reflection mapping
        gfx.draw_reflected_rect(finalX, finalY, finalW, finalH, borderR, borderG, borderB, 0.25 * alpha, 380, State.Time)
        
        if scale > 0.85 then
            imgui.text(finalX + 20, finalY + 20, borderR, borderG, borderB, game.name)
            imgui.text(finalX + 20, finalY + 55, 0.5, 0.6, 0.7, game.tagline)
            
            local maxChars = 34
            local d = game.desc
            if #d > maxChars then
                imgui.text(finalX + 20, finalY + 95, 0.4, 0.45, 0.5, string.sub(d, 1, maxChars))
                imgui.text(finalX + 20, finalY + 115, 0.4, 0.45, 0.5, string.sub(d, maxChars + 1))
            else
                imgui.text(finalX + 20, finalY + 95, 0.4, 0.45, 0.5, d)
            end
            
            imgui.text(finalX + 20, finalY + 175, 0.8, 0.8, 0.0, game.stats)
        end
    end
    
    -- 5. Selection Prompt
    local selectedGame = Games[State.SelectedIdx]
    local pulse = math.sin(State.Time * 5.0) * 0.25 + 0.75
    local r = selectedGame.color[1] * pulse
    local g = selectedGame.color[2] * pulse
    local b = selectedGame.color[3] * pulse
    
    imgui.text(w/2 - 170, 650, r, g, b, "PRESS [ENTER] TO LAUNCH " .. selectedGame.name)
    imgui.text(w/2 - 130, 680, 0.4, 0.45, 0.5, "Use [A/D] or [Arrows] to Select Game")
    
    local fpsVal = math.floor(1.0 / math.max(0.0001, time.get_dt()))
    imgui.text(w - 100, 10, 0.4, 0.4, 0.4, "FPS: " .. fpsVal)
end
