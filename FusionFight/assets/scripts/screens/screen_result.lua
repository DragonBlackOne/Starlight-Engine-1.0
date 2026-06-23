local ScreenResult = Class()

function ScreenResult:Init()
    local cx = window.get_width() / 2
    local cy = window.get_height() / 2
    
    self.btnRematch = Button("REMATCH", cx - 180, cy - 10, 360, 50, {0.1, 0.9, 0.4})
    self.btnCharSelect = Button("CHARACTER SELECT", cx - 180, cy + 60, 360, 50, {0.1, 0.7, 1.0})
    self.btnMenu = Button("MAIN MENU", cx - 180, cy + 130, 360, 50, {0.5, 0.5, 0.5})
    
    self.winnerName = ""
    self.winnerColor = {1.0, 1.0, 1.0}
end

function ScreenResult:Enter()
    self.cursor = 1
    local p1Score = fight.get_p1_score()
    local p2Score = fight.get_p2_score()
    
    if p1Score > p2Score then
        self.winnerName = fight.get_p1_name() .. " WINS!"
        local r, g, b = fight.get_p1_colors()
        self.winnerColor = {r, g, b}
    elseif p2Score > p1Score then
        self.winnerName = fight.get_p2_name() .. " WINS!"
        local r, g, b = fight.get_p2_colors()
        self.winnerColor = {r, g, b}
    else
        self.winnerName = "DRAW MATCH!"
        self.winnerColor = {0.8, 0.8, 0.8}
    end
end

function ScreenResult:Update(dt)
    -- Handle vertical keyboard navigation
    if input.is_just_pressed("W") or input.is_just_pressed("Up") then
        self.cursor = self.cursor - 1
        if self.cursor < 1 then self.cursor = 3 end
        audio.play_note(440, 0.05, 0)
    elseif input.is_just_pressed("S") or input.is_just_pressed("Down") then
        self.cursor = self.cursor + 1
        if self.cursor > 3 then self.cursor = 1 end
        audio.play_note(440, 0.05, 0)
    end

    -- Run standard button updates
    local clickRematch = self.btnRematch:Update()
    local clickCharSelect = self.btnCharSelect:Update()
    local clickMenu = self.btnMenu:Update()

    -- Let mouse hover override the cursor
    if self.btnRematch.hover then self.cursor = 1
    elseif self.btnCharSelect.hover then self.cursor = 2
    elseif self.btnMenu.hover then self.cursor = 3
    end

    -- Force hover status based on cursor
    self.btnRematch.hover = (self.cursor == 1)
    self.btnCharSelect.hover = (self.cursor == 2)
    self.btnMenu.hover = (self.cursor == 3)

    -- Confirm selection
    local confirm = input.is_just_pressed("Space") or input.is_just_pressed("Return") or input.is_just_pressed("U")

    if confirm then
        audio.play_note(660, 0.1, 0)
    end

    if (confirm and self.cursor == 1) or clickRematch then
        audio.play_note(880, 0.15, 0)
        fight.set_round_count(GameSettings.roundsToWin)
        fight.set_ai_difficulty(GameSettings.aiDifficulty)
        fight.start_match(GameSettings.p1Char, GameSettings.p2Char, GameSettings.stage)
        App:SwitchTo("FIGHT")
    elseif (confirm and self.cursor == 2) or clickCharSelect then
        App:SwitchTo("CHARSELECT")
    elseif (confirm and self.cursor == 3) or clickMenu then
        App:SwitchTo("MENU")
    end
end

function ScreenResult:Draw()
    local screenW = window.get_width()
    local screenH = window.get_height()
    local cx = screenW / 2
    local cy = screenH / 2
    
    gfx.draw_rect(0, 0, screenW, screenH, 0.05, 0.02, 0.09, 1.0)
    
    for y = 0, screenH, 80 do
        gfx.draw_rect(0, y, screenW, 1, 0.0, 0.5, 0.8, 0.05)
    end
    
    gfx.draw_text("MATCH COMPLETED", cx - 150, cy - 170, 2.0, 1.0, 1.0, 1.0, 1.0)
    
    -- Draw Winner Name large and colored
    gfx.draw_text(self.winnerName, cx - 130, cy - 100, 2.4, self.winnerColor[1], self.winnerColor[2], self.winnerColor[3], 1.0)
    gfx.draw_text(self.winnerName, cx - 132, cy - 102, 2.4, 0.0, 0.0, 0.0, 0.4) -- drop shadow
    
    -- Scores
    local scoreText = "FINAL SCORE: " .. fight.get_p1_score() .. " - " .. fight.get_p2_score()
    gfx.draw_text(scoreText, cx - 90, cy - 60, 1.0, 0.8, 0.8, 0.8, 0.9)
    
    self.btnRematch:Draw()
    self.btnCharSelect:Draw()
    self.btnMenu:Draw()
end

function ScreenResult:Exit()
end

return ScreenResult
