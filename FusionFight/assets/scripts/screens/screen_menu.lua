local ScreenMenu = Class()

function ScreenMenu:Init()
    local cx = window.get_width() / 2
    local cy = window.get_height() / 2
    
    self.btnVsCpu = Button("VS CPU", cx - 180, cy - 110, 360, 45, {0.1, 0.7, 1.0})
    self.btnVsLocal = Button("VS LOCAL (2 PLAYER)", cx - 180, cy - 55, 360, 45, {0.1, 0.9, 0.4})
    self.btnWatchReplay = Button("WATCH REPLAY", cx - 180, cy, 360, 45, {0.9, 0.1, 0.9})
    self.btnOptions = Button("OPTIONS", cx - 180, cy + 55, 360, 45, {1.0, 0.7, 0.1})
    self.btnQuit = Button("QUIT GAME", cx - 180, cy + 110, 360, 45, {0.9, 0.1, 0.4})
end

function ScreenMenu:Enter()
    self.cursor = 1
end

function ScreenMenu:Update(dt)
    -- Handle keyboard navigation
    if input.is_just_pressed("W") or input.is_just_pressed("Up") then
        self.cursor = self.cursor - 1
        if self.cursor < 1 then self.cursor = 5 end
        audio.play_note(440, 0.05, 0)
    elseif input.is_just_pressed("S") or input.is_just_pressed("Down") then
        self.cursor = self.cursor + 1
        if self.cursor > 5 then self.cursor = 1 end
        audio.play_note(440, 0.05, 0)
    end

    -- Run standard button updates to check for mouse hover & clicks
    local clickVsCpu = self.btnVsCpu:Update()
    local clickVsLocal = self.btnVsLocal:Update()
    local clickWatchReplay = self.btnWatchReplay:Update()
    local clickOptions = self.btnOptions:Update()
    local clickQuit = self.btnQuit:Update()

    -- If mouse hovers over any button, let it override the cursor
    if self.btnVsCpu.hover then self.cursor = 1
    elseif self.btnVsLocal.hover then self.cursor = 2
    elseif self.btnWatchReplay.hover then self.cursor = 3
    elseif self.btnOptions.hover then self.cursor = 4
    elseif self.btnQuit.hover then self.cursor = 5
    end

    -- Force hover state for the active cursor item (so it draws as selected)
    self.btnVsCpu.hover = (self.cursor == 1)
    self.btnVsLocal.hover = (self.cursor == 2)
    self.btnWatchReplay.hover = (self.cursor == 3)
    self.btnOptions.hover = (self.cursor == 4)
    self.btnQuit.hover = (self.cursor == 5)

    -- Confirm selection
    local confirm = input.is_just_pressed("Space") or input.is_just_pressed("Return") or input.is_just_pressed("U")
    
    if confirm or clickVsCpu or clickVsLocal or clickWatchReplay or clickOptions or clickQuit then
        audio.play_note(660, 0.1, 0)
        if self.cursor == 1 then
            if fight and fight.stop_replay then fight.stop_replay() end
            GameSettings.aiDifficulty = 1 -- normal
            App:SwitchTo("CHARSELECT")
        elseif self.cursor == 2 then
            if fight and fight.stop_replay then fight.stop_replay() end
            GameSettings.aiDifficulty = -1 -- VS Local
            App:SwitchTo("CHARSELECT")
        elseif self.cursor == 3 then
            if fight and fight.load_replay and fight.load_replay("assets/replay.rep") then
                GameSettings.p1Char = fight.get_replay_p1_char()
                GameSettings.p2Char = fight.get_replay_p2_char()
                GameSettings.stage = fight.get_replay_stage()
                fight.play_replay()
                App:SwitchTo("FIGHT")
            else
                Engine.log_warn("Falha ao carregar replay: assets/replay.rep inexistente ou corrompido.")
            end
        elseif self.cursor == 4 then
            App:SwitchTo("OPTIONS")
        elseif self.cursor == 5 then
            engine.quit()
        end
    end
end

function ScreenMenu:Draw()
    local screenW = window.get_width()
    local screenH = window.get_height()
    local cx = screenW / 2
    local cy = screenH / 2
    
    -- 1. Dark outrun background
    gfx.draw_rect(0, 0, screenW, screenH, 0.04, 0.02, 0.08, 1.0)
    
    -- 2. Retro Synthwave Sun (centered slightly higher)
    local sunX, sunY, sunR = cx, cy - 120, 110
    gfx.draw_circle_filled(sunX, sunY, sunR, 0.95, 0.12, 0.38, 0.8)
    gfx.draw_circle_filled(sunX, sunY, sunR - 4, 1.0, 0.45, 0.12, 0.9)
    for yCut = sunY + 8, sunY + sunR, 12 do
        local thickness = 2 + (yCut - sunY) / 10
        gfx.draw_rect(sunX - sunR - 10, yCut, (sunR + 10) * 2, thickness, 0.04, 0.02, 0.08, 1.0)
    end
    
    -- 3. Perspective Grid Floor (bottom half)
    local horizonY = cy + 100
    for i = -12, 12 do
        local pstartX = cx + i * 20
        local pendX = cx + i * 115
        gfx.draw_line(pstartX, horizonY, pendX, screenH, 1.8, 0.45, 0.12, 0.65, 0.4)
    end
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
    
    -- 4. Text with shadows
    gfx.draw_text("GAME SELECT", cx - 118, cy - 158, 2.0, 0.0, 0.0, 0.0, 0.9)
    gfx.draw_text("GAME SELECT", cx - 120, cy - 160, 2.0, 0.0, 0.85, 1.0, 1.0)
    gfx.draw_text("CHOOSE GAMEPLAY MODE", cx - 119, cy - 129, 0.9, 0.0, 0.0, 0.0, 0.9)
    gfx.draw_text("CHOOSE GAMEPLAY MODE", cx - 120, cy - 130, 0.9, 0.5, 0.6, 0.7, 0.8)
    
    -- 5. Draw Buttons
    self.btnVsCpu:Draw()
    self.btnVsLocal:Draw()
    self.btnWatchReplay:Draw()
    self.btnOptions:Draw()
    self.btnQuit:Draw()
end

function ScreenMenu:Exit()
end

return ScreenMenu
