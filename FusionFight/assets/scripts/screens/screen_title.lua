local ScreenTitle = Class()

function ScreenTitle:Init()
    local cx = window.get_width() / 2
    local cy = window.get_height() / 2
    
    self.btnStart = Button("PRESS START", cx - 100, cy + 80, 200, 50, {0.1, 0.7, 1.0})
    self.particles = {}
    
    -- Spawn floating title screen particles
    for i = 1, 20 do
        table.insert(self.particles, {
            x = math.random(0, window.get_width()),
            y = math.random(0, window.get_height()),
            vx = math.random(-15, 15),
            vy = math.random(-15, 15),
            size = math.random(4, 10),
            life = math.random()
        })
    end
end

function ScreenTitle:Enter()
    audio.play_note(330, 0.1, 0) -- Welcome chord notes
    audio.play_note(440, 0.1, 0)
    audio.play_note(660, 0.2, 0)
end

function ScreenTitle:Update(dt)
    -- Update floating particles
    local screenW = window.get_width()
    local screenH = window.get_height()
    for _, p in ipairs(self.particles) do
        p.x = (p.x + p.vx * dt) % screenW
        p.y = (p.y + p.vy * dt) % screenH
    end
    
    if self.btnStart:Update() or input.is_just_pressed("Space") or input.is_just_pressed("Return") then
        App:SwitchTo("MENU")
    end
end

function ScreenTitle:Draw()
    local screenW = window.get_width()
    local screenH = window.get_height()
    local cx = screenW / 2
    local cy = screenH / 2
    
    -- 1. Dark outrun background
    gfx.draw_rect(0, 0, screenW, screenH, 0.04, 0.02, 0.08, 1.0)
    
    -- 2. Retro Synthwave Sun
    local sunX, sunY, sunR = cx, cy - 80, 130
    gfx.draw_circle_filled(sunX, sunY, sunR, 0.95, 0.12, 0.38, 0.9)
    gfx.draw_circle_filled(sunX, sunY, sunR - 4, 1.0, 0.45, 0.12, 0.95)
    for yCut = sunY + 10, sunY + sunR, 14 do
        local thickness = 2 + (yCut - sunY) / 10
        gfx.draw_rect(sunX - sunR - 10, yCut, (sunR + 10) * 2, thickness, 0.04, 0.02, 0.08, 1.0)
    end
    
    -- 3. Perspective Grid Floor (bottom half)
    local horizonY = cy + 100
    -- Radial perspective lines
    for i = -12, 12 do
        local pstartX = cx + i * 20
        local pendX = cx + i * 115
        gfx.draw_line(pstartX, horizonY, pendX, screenH, 1.8, 0.45, 0.12, 0.65, 0.4)
    end
    -- Scrolling horizontal lines
    local scrollTime = (time.get_time() * 25) % 50
    for j = 0, 10 do
        local progress = (j * 12 + scrollTime) / 160
        if progress <= 1.0 then
            local py = horizonY + progress * (screenH - horizonY)
            local thickness = 1.0 + progress * 2.5
            local alpha = progress * 0.38
            gfx.draw_rect(0, py - thickness*0.5, screenW, thickness, 0.45, 0.12, 0.65, alpha)
        end
    end
    -- Glowing Cyan Horizon Border
    gfx.draw_rect(0, horizonY - 1, screenW, 2.5, 0.0, 0.85, 1.0, 0.75)
    gfx.draw_rect(0, horizonY - 3, screenW, 6.0, 0.0, 0.85, 1.0, 0.25)
    
    -- 4. Floating Neon Dust Particles
    for _, p in ipairs(self.particles) do
        local pAlpha = 0.15 + math.abs(math.sin(time.get_time() + p.life)) * 0.25
        gfx.draw_rect(p.x, p.y, p.size, p.size, 0.0, 0.85, 1.0, pAlpha)
    end
    
    -- Pulsing title size
    local t = time.get_time() * 2.5
    local pulse = math.abs(math.sin(t)) * 0.08
    local baseScale = 2.4
    
    -- Glowing Title "FUSION FIGHT"
    local ts = (baseScale + pulse) * 1.5
    gfx.draw_text("FUSION FIGHT", cx - 208, cy - 78, ts, 0.0, 0.0, 0.0, 0.9)
    gfx.draw_text("FUSION FIGHT", cx - 210, cy - 80, ts, 0.0, 0.85, 1.0, 1.0)
    gfx.draw_text("FUSION FIGHT", cx - 212, cy - 82, ts, 0.85, 0.0, 0.85, 0.4)
    
    gfx.draw_text("2D FIGHTING GAME SHOWCASE // C++ ENGINE", cx - 230, cy - 10, 0.9, 0.5, 0.6, 0.7, 0.8)
    
    self.btnStart:Draw()
    
    -- Footer info
    gfx.draw_text("Fusion ENGINE v1.0.0 MVP", 20, screenH - 35, 0.8, 0.4, 0.4, 0.4, 0.8)
    gfx.draw_text("Use WASD/Arrows to Navigate | Enter/Space to Confirm", screenW - 450, screenH - 35, 0.8, 0.4, 0.4, 0.4, 0.8)
end

function ScreenTitle:Exit()
end

return ScreenTitle
