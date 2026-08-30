local ScreenOptions = Class()

function ScreenOptions:Init()
    local cx = window.get_width() / 2
    local cy = window.get_height() / 2
    
    self.btnRounds = Button("ROUNDS TO WIN: 2 (BO3)", cx - 180, cy - 110, 360, 45, {0.1, 0.7, 1.0})
    self.btnDifficulty = Button("CPU DIFFICULTY: NORMAL", cx - 180, cy - 55, 360, 45, {0.1, 0.9, 0.4})
    self.btnGraphics = Button("GRAPHICS QUALITY: BALANCED", cx - 180, cy, 360, 45, {1.0, 0.1, 0.8})
    self.btnDebug = Button("DEBUG HITBOXES: OFF", cx - 180, cy + 55, 360, 45, {0.9, 0.6, 0.1})
    self.btnBack = Button("SAVE & BACK", cx - 180, cy + 110, 360, 45, {0.5, 0.5, 0.5})
end

function ScreenOptions:Enter()
    self.cursor = 1
    if cvar then
        GameSettings.roundsToWin = cvar.get("g_roundsToWin") or GameSettings.roundsToWin
        GameSettings.aiDifficulty = cvar.get("g_aiDifficulty") or GameSettings.aiDifficulty
        GameSettings.graphicsPreset = cvar.get("g_graphicsPreset") or GameSettings.graphicsPreset
        GameSettings.debugBoxes = cvar.get("g_debugBoxes") or GameSettings.debugBoxes
    end
    self:UpdateButtonLabels()
end

function ScreenOptions:UpdateButtonLabels()
    -- Rounds
    local roundsLabel = "ROUNDS TO WIN: " .. GameSettings.roundsToWin .. " (Best of " .. (GameSettings.roundsToWin*2 - 1) .. ")"
    self.btnRounds.text = roundsLabel
    
    -- Difficulty
    local diffs = { [0] = "EASY", [1] = "NORMAL", [2] = "HARD", [-1] = "VS LOCAL" }
    local diffText = diffs[GameSettings.aiDifficulty] or "NORMAL"
    self.btnDifficulty.text = "CPU DIFFICULTY: " .. diffText

    -- Graphics Quality Preset
    local presets = { [0] = "PERFORMANCE (LOW)", [1] = "BALANCED (MEDIUM)", [2] = "FIDELITY (ULTRA)" }
    local presetText = presets[GameSettings.graphicsPreset] or "BALANCED (MEDIUM)"
    self.btnGraphics.text = "GRAPHICS QUALITY: " .. presetText

    -- Debug Boxes
    self.btnDebug.text = "DEBUG HITBOXES: " .. (GameSettings.debugBoxes and "ON" or "OFF")
end

function ScreenOptions:Update(dt)
    -- Handle vertical keyboard navigation
    if input.is_just_pressed("W") or input.is_just_pressed("Up") then
        self.cursor = self.cursor - 1
        if self.cursor < 1 then self.cursor = 5 end
        audio.play_note(440, 0.05, 0)
    elseif input.is_just_pressed("S") or input.is_just_pressed("Down") then
        self.cursor = self.cursor + 1
        if self.cursor > 5 then self.cursor = 1 end
        audio.play_note(440, 0.05, 0)
    end

    -- Run standard button updates (handles mouse hover/click)
    local clickRounds = self.btnRounds:Update()
    local clickDifficulty = self.btnDifficulty:Update()
    local clickGraphics = self.btnGraphics:Update()
    local clickDebug = self.btnDebug:Update()
    local clickBack = self.btnBack:Update()

    -- Let mouse hover override the cursor
    if self.btnRounds.hover then self.cursor = 1
    elseif self.btnDifficulty.hover then self.cursor = 2
    elseif self.btnGraphics.hover then self.cursor = 3
    elseif self.btnDebug.hover then self.cursor = 4
    elseif self.btnBack.hover then self.cursor = 5
    end

    -- Force hover status based on cursor
    self.btnRounds.hover = (self.cursor == 1)
    self.btnDifficulty.hover = (self.cursor == 2)
    self.btnGraphics.hover = (self.cursor == 3)
    self.btnDebug.hover = (self.cursor == 4)
    self.btnBack.hover = (self.cursor == 5)

    -- Handle value changes using Left/Right or Return/Space/U
    local changeLeft = input.is_just_pressed("A") or input.is_just_pressed("Left")
    local changeRight = input.is_just_pressed("D") or input.is_just_pressed("Right")
    local confirm = input.is_just_pressed("Space") or input.is_just_pressed("Return") or input.is_just_pressed("U")
    local escape = input.is_just_pressed("Escape")

    if escape then
        audio.play_note(660, 0.1, 0)
        App:SwitchTo("MENU")
        return
    end

    if self.cursor == 1 then
        if clickRounds or confirm or changeLeft or changeRight then
            audio.play_note(440, 0.05, 0)
            if changeLeft then
                GameSettings.roundsToWin = GameSettings.roundsToWin - 1
                if GameSettings.roundsToWin < 1 then GameSettings.roundsToWin = 3 end
            else
                GameSettings.roundsToWin = GameSettings.roundsToWin + 1
                if GameSettings.roundsToWin > 3 then GameSettings.roundsToWin = 1 end
            end
            self:UpdateButtonLabels()
        end
    elseif self.cursor == 2 then
        if clickDifficulty or confirm or changeLeft or changeRight then
            audio.play_note(440, 0.05, 0)
            if changeLeft then
                local prev = GameSettings.aiDifficulty - 1
                if prev < -1 then prev = 2 end
                GameSettings.aiDifficulty = prev
            else
                local next = GameSettings.aiDifficulty + 1
                if next > 2 then next = -1 end
                GameSettings.aiDifficulty = next
            end
            self:UpdateButtonLabels()
        end
    elseif self.cursor == 3 then
        if clickGraphics or confirm or changeLeft or changeRight then
            audio.play_note(440, 0.05, 0)
            if changeLeft then
                GameSettings.graphicsPreset = GameSettings.graphicsPreset - 1
                if GameSettings.graphicsPreset < 0 then GameSettings.graphicsPreset = 2 end
            else
                GameSettings.graphicsPreset = GameSettings.graphicsPreset + 1
                if GameSettings.graphicsPreset > 2 then GameSettings.graphicsPreset = 0 end
            end
            self:UpdateButtonLabels()
            -- Sync preset with engine immediately
            if Engine and Engine.set_graphics_preset then
                Engine.set_graphics_preset(GameSettings.graphicsPreset)
            end
        end
    elseif self.cursor == 4 then
        if clickDebug or confirm or changeLeft or changeRight then
            audio.play_note(440, 0.05, 0)
            GameSettings.debugBoxes = not GameSettings.debugBoxes
            self:UpdateButtonLabels()
        end
    elseif self.cursor == 5 then
        if clickBack or confirm then
            Save.write("roundsToWin", GameSettings.roundsToWin)
            Save.write("aiDifficulty", GameSettings.aiDifficulty)
            Save.write("graphicsPreset", GameSettings.graphicsPreset)
            Save.write("debugBoxes", GameSettings.debugBoxes)
            Save.flush()
            if cvar then
                cvar.set("g_roundsToWin", GameSettings.roundsToWin)
                cvar.set("g_aiDifficulty", GameSettings.aiDifficulty)
                cvar.set("g_graphicsPreset", GameSettings.graphicsPreset)
                cvar.set("g_debugBoxes", GameSettings.debugBoxes)
            end
            audio.play_note(660, 0.1, 0)
            App:SwitchTo("MENU")
        end
    end
end

function ScreenOptions:Draw()
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
    local horizonY = cy + 140
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
    gfx.draw_text("GAME CONFIGURATION", cx - 178, cy - 158, 2.0, 0.0, 0.0, 0.0, 0.9)
    gfx.draw_text("GAME CONFIGURATION", cx - 180, cy - 160, 2.0, 0.0, 0.85, 1.0, 1.0)
    
    -- 5. Draw Buttons
    self.btnRounds:Draw()
    self.btnDifficulty:Draw()
    self.btnGraphics:Draw()
    self.btnDebug:Draw()
    self.btnBack:Draw()
end

function ScreenOptions:Exit()
end

return ScreenOptions
