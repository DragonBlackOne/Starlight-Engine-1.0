local ScreenStageSelect = Class()

function ScreenStageSelect:Init()
    self.selectedStage = nil
    self.cursor = 0 -- 0=Neon Dojo, 1=Steel Arena
    
    local cx = window.get_width() / 2
    local cy = window.get_height() / 2
    
    self.btnBack = Button("BACK", 50, window.get_height() - 80, 200, 45, {0.5, 0.5, 0.5})
end

function ScreenStageSelect:Enter()
    self.selectedStage = nil
end

function ScreenStageSelect:Update(dt)
    if self.btnBack:Update() or input.is_just_pressed("Escape") then
        audio.play_note(660, 0.1, 0)
        App:SwitchTo("CHARSELECT")
        return
    end
    
    -- Selection movement
    if input.is_just_pressed("A") or input.is_just_pressed("Left") then
        self.cursor = 0
        audio.play_note(440, 0.05, 0)
    elseif input.is_just_pressed("D") or input.is_just_pressed("Right") then
        self.cursor = 1
        audio.play_note(440, 0.05, 0)
    end
    
    -- Confirmation
    if input.is_just_pressed("U") or input.is_just_pressed("Space") or input.is_just_pressed("Return") then
        self.selectedStage = self.cursor
        GameSettings.stage = self.selectedStage
        audio.play_note(880, 0.15, 0)
        
        -- START MATCH!
        -- Register match settings to FightingSystem
        fight.set_round_count(GameSettings.roundsToWin)
        fight.set_ai_difficulty(GameSettings.aiDifficulty)
        fight.start_match(GameSettings.p1Char, GameSettings.p2Char, GameSettings.stage)
        
        App:SwitchTo("FIGHT")
    end
end

function ScreenStageSelect:Draw()
    local screenW = window.get_width()
    local screenH = window.get_height()
    local cx = screenW / 2
    local cy = screenH / 2
    
    -- 1. Dark outrun background
    gfx.draw_rect(0, 0, screenW, screenH, 0.04, 0.02, 0.08, 1.0)
    
    -- 2. Retro Synthwave Sun (centered higher, behind the grid)
    local sunX, sunY, sunR = cx, 140, 75
    gfx.draw_circle_filled(sunX, sunY, sunR, 0.95, 0.12, 0.38, 0.8)
    gfx.draw_circle_filled(sunX, sunY, sunR - 3, 1.0, 0.45, 0.12, 0.95)
    for yCut = sunY + 5, sunY + sunR, 10 do
        local thickness = 1.5 + (yCut - sunY) / 10
        gfx.draw_rect(sunX - sunR - 10, yCut, (sunR + 10) * 2, thickness, 0.04, 0.02, 0.08, 1.0)
    end
    
    -- 3. Perspective Grid Floor (bottom half)
    local horizonY = 510
    for i = -12, 12 do
        local pstartX = cx + i * 20
        local pendX = cx + i * 115
        gfx.draw_line(pstartX, horizonY, pendX, screenH, 1.8, 0.45, 0.12, 0.65, 0.4)
    end
    local scrollTime = (time.get_time() * 25) % 50
    for j = 0, 8 do
        local progress = (j * 12 + scrollTime) / 130
        if progress <= 1.0 then
            local py = horizonY + progress * (screenH - horizonY)
            local thickness = 1.0 + progress * 2.5
            local alpha = progress * 0.38
            gfx.draw_rect(0, py - thickness*0.5, screenW, thickness, 0.45, 0.12, 0.65, alpha)
        end
    end
    -- Horizon Border line
    gfx.draw_rect(0, horizonY - 1, screenW, 2.5, 0.0, 0.85, 1.0, 0.75)
    gfx.draw_rect(0, horizonY - 3, screenW, 6.0, 0.0, 0.85, 1.0, 0.25)
    
    -- Headers with drop shadows
    gfx.draw_text("SELECT BATTLE ARENA", cx - 208, 52, 2.0, 0.0, 0.0, 0.0, 0.9)
    gfx.draw_text("SELECT BATTLE ARENA", cx - 210, 50, 2.0, 1.0, 1.0, 1.0, 1.0)
    gfx.draw_text("CHOOSE YOUR STAGE (A/D + Enter)", cx - 139, 96, 1.0, 0.0, 0.0, 0.0, 0.9)
    gfx.draw_text("CHOOSE YOUR STAGE (A/D + Enter)", cx - 140, 95, 1.0, 0.5, 0.6, 0.7, 0.8)
    
    -- Grid of 2 stages
    local boxW = 320
    local boxH = 200
    local gridY = 200
    
    -- Dojo Preview
    local x0 = cx - boxW - 40
    local borderCol0 = {0.2, 0.2, 0.2}
    if self.cursor == 0 then borderCol0 = {0.1, 0.7, 1.0} end
    
    gfx.draw_rounded_rect(x0 - 3, gridY - 3, boxW + 6, boxH + 6, 10, borderCol0[1], borderCol0[2], borderCol0[3], 0.8)
    gfx.draw_rounded_rect(x0, gridY, boxW, boxH, 8, 0.05, 0.07, 0.14, 0.9)
    -- Small geometric lines to mock stage
    gfx.draw_rect(x0 + 10, gridY + 120, boxW - 20, 2, 0.5, 0.0, 0.5, 0.5)
    gfx.draw_rect(x0 + 10, gridY + 150, boxW - 20, 2, 0.5, 0.0, 0.5, 0.5)
    
    gfx.draw_text("NEON DOJO", x0 + 20 + 1, gridY + 30 + 1, 1.4, 0, 0, 0, 0.8)
    gfx.draw_text("NEON DOJO", x0 + 20, gridY + 30, 1.4, 0.1, 0.7, 1.0, 1.0)
    gfx.draw_text("Traditional cyberpunk grid floor", x0 + 20 + 1, gridY + boxH - 45 + 1, 0.75, 0, 0, 0, 0.8)
    gfx.draw_text("Traditional cyberpunk grid floor", x0 + 20, gridY + boxH - 45, 0.75, 0.6, 0.6, 0.6, 0.8)
    
    -- Arena Preview
    local x1 = cx + 40
    local borderCol1 = {0.2, 0.2, 0.2}
    if self.cursor == 1 then borderCol1 = {0.1, 0.7, 1.0} end
    
    gfx.draw_rounded_rect(x1 - 3, gridY - 3, boxW + 6, boxH + 6, 10, borderCol1[1], borderCol1[2], borderCol1[3], 0.8)
    gfx.draw_rounded_rect(x1, gridY, boxW, boxH, 8, 0.05, 0.07, 0.14, 0.9)
    -- Mock giant gear
    gfx.draw_circle(x1 + boxW - 60, gridY + 60, 30, 0.25, 0.25, 0.28, 5, 0.5)
    gfx.draw_circle_filled(x1 + boxW - 60, gridY + 60, 10, 0.12, 0.12, 0.14, 1.0)
    
    gfx.draw_text("STEEL ARENA", x1 + 20 + 1, gridY + 30 + 1, 1.4, 0, 0, 0, 0.8)
    gfx.draw_text("STEEL ARENA", x1 + 20, gridY + 30, 1.4, 0.9, 0.5, 0.1, 1.0)
    gfx.draw_text("Heavy metallic gear structure", x1 + 20 + 1, gridY + boxH - 45 + 1, 0.75, 0, 0, 0, 0.8)
    gfx.draw_text("Heavy metallic gear structure", x1 + 20, gridY + boxH - 45, 0.75, 0.6, 0.6, 0.6, 0.8)
    
    self.btnBack:Draw()
end

function ScreenStageSelect:Exit()
end

return ScreenStageSelect
