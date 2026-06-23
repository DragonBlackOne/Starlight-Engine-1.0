local ScreenCharSelect = Class()

function ScreenCharSelect:Init()
    self.selectedP1 = nil
    self.selectedP2 = nil
    self.p1Cursor = 0 -- 0=Kaito, 1=Gorou, 2=Rin
    self.p2Cursor = 1
    self.selectionPhase = 1 -- 1: P1 selecting, 2: P2/CPU selecting
    
    local cx = window.get_width() / 2
    local cy = window.get_height() / 2
    
    self.btnBack = Button("BACK TO MENU", 50, window.get_height() - 80, 200, 45, {0.5, 0.5, 0.5})
end

function ScreenCharSelect:Enter()
    self.selectedP1 = nil
    self.selectedP2 = nil
    self.selectionPhase = 1
end

function ScreenCharSelect:Update(dt)
    -- Back button or Escape key
    if self.btnBack:Update() or input.is_just_pressed("Escape") then
        audio.play_note(660, 0.1, 0)
        App:SwitchTo("MENU")
        return
    end
    
    -- Movement and confirmation depending on active selecting player
    if self.selectionPhase == 1 then
        -- P1 inputs (WASD)
        if input.is_just_pressed("A") then
            self.p1Cursor = (self.p1Cursor - 1 + 3) % 3
            audio.play_note(440, 0.05, 0)
        elseif input.is_just_pressed("D") then
            self.p1Cursor = (self.p1Cursor + 1) % 3
            audio.play_note(440, 0.05, 0)
        end
        
        -- LP (U) or Space/Enter to confirm P1
        if input.is_just_pressed("U") or input.is_just_pressed("Space") or input.is_just_pressed("Return") then
            self.selectedP1 = self.p1Cursor
            GameSettings.p1Char = self.selectedP1
            audio.play_note(660, 0.12, 0)
            
            -- If VS CPU, CPU selects instantly
            if GameSettings.aiDifficulty >= 0 then
                self.selectedP2 = (self.selectedP1 + 1) % 3
                GameSettings.p2Char = self.selectedP2
                self.selectionPhase = 3 -- Done
            else
                self.selectionPhase = 2 -- P2 selects next
            end
        end
    elseif self.selectionPhase == 2 then
        -- P2 inputs (Arrows)
        if input.is_just_pressed("Left") then
            self.p2Cursor = (self.p2Cursor - 1 + 3) % 3
            audio.play_note(440, 0.05, 0)
        elseif input.is_just_pressed("Right") then
            self.p2Cursor = (self.p2Cursor + 1) % 3
            audio.play_note(440, 0.05, 0)
        end
        
        -- LP (Num4) or Enter to confirm P2
        if input.is_just_pressed("Num4") or input.is_just_pressed("Return") then
            self.selectedP2 = self.p2Cursor
            GameSettings.p2Char = self.selectedP2
            audio.play_note(660, 0.12, 0)
            self.selectionPhase = 3
        end
    end
    
    -- Go to stage select when selections complete
    if self.selectionPhase == 3 then
        App:SwitchTo("STAGESELECT")
    end
end

function ScreenCharSelect:Draw()
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
    gfx.draw_text("SELECT YOUR FIGHTER", cx - 208, 52, 2.0, 0.0, 0.0, 0.0, 0.9)
    gfx.draw_text("SELECT YOUR FIGHTER", cx - 210, 50, 2.0, 1.0, 1.0, 1.0, 1.0)
    
    -- Status info / who is selecting
    if self.selectionPhase == 1 then
        gfx.draw_text("PLAYER 1: SELECT YOUR CHARACTER (WASD + Space)", cx - 239, 96, 1.0, 0.0, 0.0, 0.0, 0.9)
        gfx.draw_text("PLAYER 1: SELECT YOUR CHARACTER (WASD + Space)", cx - 240, 95, 1.0, 0.1, 0.7, 1.0, 1.0)
    elseif self.selectionPhase == 2 then
        gfx.draw_text("PLAYER 2: SELECT YOUR CHARACTER (ARROWS + Enter)", cx - 239, 96, 1.0, 0.0, 0.0, 0.0, 0.9)
        gfx.draw_text("PLAYER 2: SELECT YOUR CHARACTER (ARROWS + Enter)", cx - 240, 95, 1.0, 0.1, 0.9, 0.4, 1.0)
    end
    
    -- ─── DRAW CHARACTER GRID ─────────────────────────────────────────────────
    -- 3 boxes: Kaito (0), Gorou (1), Rin (2)
    local boxW = 185
    local boxH = 300
    local gridY = 170
    local spacing = 30
    local totalW = (boxW * 3) + (spacing * 2)
    local startX = cx - (totalW / 2)
    
    -- Box 0: KAITO
    local x0 = startX
    local borderCol0 = {0.2, 0.2, 0.2}
    local isHoverP1_0 = (self.p1Cursor == 0 and self.selectionPhase == 1)
    local isHoverP2_0 = (self.p2Cursor == 0 and self.selectionPhase == 2)
    
    if isHoverP1_0 then borderCol0 = {0.1, 0.6, 1.0} end
    if isHoverP2_0 then borderCol0 = {0.1, 0.9, 0.4} end
    if self.selectedP1 == 0 then borderCol0 = {0.1, 0.6, 1.0} end
    if self.selectedP2 == 0 then borderCol0 = {0.1, 0.9, 0.4} end
    
    gfx.draw_rounded_rect(x0 - 3, gridY - 3, boxW + 6, boxH + 6, 10, borderCol0[1], borderCol0[2], borderCol0[3], 0.8)
    gfx.draw_rounded_rect(x0, gridY, boxW, boxH, 8, 0.05, 0.07, 0.14, 0.9) -- Glassmorphic background
    
    -- Draw Kaito Live Human Preview
    FighterRenderer.DrawCharPreview("KAITO", x0 + boxW*0.5, gridY + 165, 1.1, 1, time.get_time() * 10)
    
    -- Text with shadows
    local name0 = "KAITO"
    local desc0 = "THE SHADOW BALANCED"
    gfx.draw_text(name0, x0 + boxW*0.5 - 35 + 1, gridY + boxH - 45 + 1, 1.2, 0, 0, 0, 0.8)
    gfx.draw_text(name0, x0 + boxW*0.5 - 35, gridY + boxH - 45, 1.2, 0.1, 0.7, 1.0, 1.0)
    gfx.draw_text(desc0, x0 + boxW*0.5 - 75 + 1, gridY + boxH - 20 + 1, 0.75, 0, 0, 0, 0.8)
    gfx.draw_text(desc0, x0 + boxW*0.5 - 75, gridY + boxH - 20, 0.75, 0.6, 0.6, 0.6, 0.8)
    
    -- Stats Kaito
    local statsX0 = x0 + 15
    gfx.draw_text("HEALTH: |||||||| 1000", statsX0, gridY + 180, 0.75, 0.7, 0.7, 0.7, 0.9)
    gfx.draw_text("SPEED:  |||||||||| 3.5", statsX0, gridY + 200, 0.75, 0.7, 0.7, 0.7, 0.9)
    gfx.draw_text("POWER:  |||||| 70", statsX0, gridY + 220, 0.75, 0.7, 0.7, 0.7, 0.9)
    
    
    -- Box 1: GOROU
    local x1 = startX + boxW + spacing
    local borderCol1 = {0.2, 0.2, 0.2}
    local isHoverP1_1 = (self.p1Cursor == 1 and self.selectionPhase == 1)
    local isHoverP2_1 = (self.p2Cursor == 1 and self.selectionPhase == 2)
    
    if isHoverP1_1 then borderCol1 = {0.1, 0.6, 1.0} end
    if isHoverP2_1 then borderCol1 = {0.1, 0.9, 0.4} end
    if self.selectedP1 == 1 then borderCol1 = {0.1, 0.6, 1.0} end
    if self.selectedP2 == 1 then borderCol1 = {0.1, 0.9, 0.4} end
    
    gfx.draw_rounded_rect(x1 - 3, gridY - 3, boxW + 6, boxH + 6, 10, borderCol1[1], borderCol1[2], borderCol1[3], 0.8)
    gfx.draw_rounded_rect(x1, gridY, boxW, boxH, 8, 0.05, 0.07, 0.14, 0.9)
    
    -- Draw Gorou Live Human Preview
    FighterRenderer.DrawCharPreview("GOROU", x1 + boxW*0.5, gridY + 165, 1.0, 1, time.get_time() * 10)
    
    -- Text with shadows
    local name1 = "GOROU"
    local desc1 = "THE IRON WALL GRAPPLER"
    gfx.draw_text(name1, x1 + boxW*0.5 - 38 + 1, gridY + boxH - 45 + 1, 1.2, 0, 0, 0, 0.8)
    gfx.draw_text(name1, x1 + boxW*0.5 - 38, gridY + boxH - 45, 1.2, 0.9, 0.2, 0.2, 1.0)
    gfx.draw_text(desc1, x1 + boxW*0.5 - 80 + 1, gridY + boxH - 20 + 1, 0.75, 0, 0, 0, 0.8)
    gfx.draw_text(desc1, x1 + boxW*0.5 - 80, gridY + boxH - 20, 0.75, 0.6, 0.6, 0.6, 0.8)
    
    -- Stats Gorou
    local statsX1 = x1 + 15
    gfx.draw_text("HEALTH: |||||||||| 1200", statsX1, gridY + 180, 0.75, 0.7, 0.7, 0.7, 0.9)
    gfx.draw_text("SPEED:  |||||| 2.4", statsX1, gridY + 200, 0.75, 0.7, 0.7, 0.7, 0.9)
    gfx.draw_text("POWER:  |||||||||| 150", statsX1, gridY + 220, 0.75, 0.7, 0.7, 0.7, 0.9)
    
    
    -- Box 2: RIN
    local x2 = startX + (boxW + spacing) * 2
    local borderCol2 = {0.2, 0.2, 0.2}
    local isHoverP1_2 = (self.p1Cursor == 2 and self.selectionPhase == 1)
    local isHoverP2_2 = (self.p2Cursor == 2 and self.selectionPhase == 2)
    
    if isHoverP1_2 then borderCol2 = {0.1, 0.6, 1.0} end
    if isHoverP2_2 then borderCol2 = {0.1, 0.9, 0.4} end
    if self.selectedP1 == 2 then borderCol2 = {0.1, 0.6, 1.0} end
    if self.selectedP2 == 2 then borderCol2 = {0.1, 0.9, 0.4} end
    
    gfx.draw_rounded_rect(x2 - 3, gridY - 3, boxW + 6, boxH + 6, 10, borderCol2[1], borderCol2[2], borderCol2[3], 0.8)
    gfx.draw_rounded_rect(x2, gridY, boxW, boxH, 8, 0.05, 0.07, 0.14, 0.9)
    
    -- Draw Rin Live Human Preview
    FighterRenderer.DrawCharPreview("RIN", x2 + boxW*0.5, gridY + 165, 1.15, 1, time.get_time() * 10)
    
    -- Text with shadows
    local name2 = "RIN"
    local desc2 = "THE NEON SPEED BLADE"
    gfx.draw_text(name2, x2 + boxW*0.5 - 20 + 1, gridY + boxH - 45 + 1, 1.2, 0, 0, 0, 0.8)
    gfx.draw_text(name2, x2 + boxW*0.5 - 20, gridY + boxH - 45, 1.2, 0.8, 0.2, 0.9, 1.0)
    gfx.draw_text(desc2, x2 + boxW*0.5 - 75 + 1, gridY + boxH - 20 + 1, 0.75, 0, 0, 0, 0.8)
    gfx.draw_text(desc2, x2 + boxW*0.5 - 75, gridY + boxH - 20, 0.75, 0.6, 0.6, 0.6, 0.8)
    
    -- Stats Rin
    local statsX2 = x2 + 15
    gfx.draw_text("HEALTH: ||||||| 850", statsX2, gridY + 180, 0.75, 0.7, 0.7, 0.7, 0.9)
    gfx.draw_text("SPEED:  |||||||||||| 4.2", statsX2, gridY + 200, 0.75, 0.7, 0.7, 0.7, 0.9)
    gfx.draw_text("POWER:  |||||||| 95", statsX2, gridY + 220, 0.75, 0.7, 0.7, 0.7, 0.9)
    
    
    -- Draw selections confirmations
    if self.selectedP1 then
        local sx = (self.selectedP1 == 0) and x0 or ((self.selectedP1 == 1) and x1 or x2)
        gfx.draw_text("P1 READY", sx + 10, gridY + 10, 1.0, 0.1, 0.7, 1.0, 1.0)
    end
    if self.selectedP2 then
        local sx = (self.selectedP2 == 0) and x0 or ((self.selectedP2 == 1) and x1 or x2)
        gfx.draw_text("P2 READY", sx + boxW - 90, gridY + 10, 1.0, 0.1, 0.9, 0.4, 1.0)
    end
    
    self.btnBack:Draw()
end

function ScreenCharSelect:Exit()
end

return ScreenCharSelect
