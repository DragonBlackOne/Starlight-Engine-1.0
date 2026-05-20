-- tetris_main.lua
-- RADICAL RECONSTRUCTION v5.0: INDUSTRIAL NEON EDITION

-- ============================================================================
-- Design System
-- ============================================================================
local Colors = {
    Grid = {0.0, 0.4, 0.8, 0.2},
    Ghost = {1.0, 1.0, 1.0, 0.2},
    Text = {0.0, 0.8, 1.0, 1.0},
    Background = {0.02, 0.02, 0.05}
}

local Constants = {
    Cols = 10,
    Rows = 20,
    BlockSize = 1.0,
    BoardX = -5.0,
    ShakeIntensity = 0
}

-- ============================================================================
-- Piece Definitions (PBR Industrial Properties)
-- ============================================================================
local Shapes = {
    { color = {0.0, 1.0, 1.0}, metal = 0.1, rough = 0.05, name = "I", -- Cyan
        rotations = {{{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}, {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}}
    },
    { color = {0.2, 0.4, 1.0}, metal = 0.3, rough = 0.1, name = "J", -- Blue
        rotations = {{{0,1,0}, {0,1,0}, {1,1,0}}, {{1,0,0}, {1,1,1}, {0,0,0}}, {{0,1,1}, {0,1,0}, {0,1,0}}, {{0,0,0}, {1,1,1}, {0,0,1}}}
    },
    { color = {1.0, 0.5, 0.0}, metal = 0.8, rough = 0.4, name = "L", -- Orange
        rotations = {{{0,1,0}, {0,1,0}, {0,1,1}}, {{0,0,0}, {1,1,1}, {1,0,0}}, {{1,1,0}, {0,1,0}, {0,1,0}}, {{0,0,1}, {1,1,1}, {0,0,0}}}
    },
    { color = {1.0, 0.9, 0.0}, metal = 0.9, rough = 0.1, name = "O", -- Gold
        rotations = {{{1,1}, {1,1}}}
    },
    { color = {0.1, 1.0, 0.2}, metal = 0.0, rough = 0.2, name = "S", -- Green
        rotations = {{{0,1,1}, {1,1,0}, {0,0,0}}, {{0,1,0}, {0,1,1}, {0,0,1}}}
    },
    { color = {0.6, 0.0, 1.0}, metal = 0.7, rough = 0.05, name = "T", -- Purple
        rotations = {{{0,0,0}, {1,1,1}, {0,1,0}}, {{0,1,0}, {1,1,0}, {0,1,0}}, {{0,1,0}, {1,1,1}, {0,0,0}}, {{0,1,0}, {0,1,1}, {0,1,0}}}
    },
    { color = {1.0, 0.1, 0.1}, metal = 0.2, rough = 0.05, name = "Z", -- Red
        rotations = {{{1,1,0}, {0,1,1}, {0,0,0}}, {{0,0,1}, {0,1,1}, {0,1,0}}}
    }
}

-- ============================================================================
-- Tetris Chiptune Music Kernel (Procedural YM2612 / Square Wave Theme)
-- ============================================================================
local Melody = {
    {659, 1.0}, {494, 0.5}, {523, 0.5}, {587, 1.0}, {523, 0.5}, {494, 0.5},
    {440, 1.0}, {440, 0.5}, {523, 0.5}, {659, 1.0}, {587, 0.5}, {523, 0.5},
    {494, 1.5}, {523, 0.5}, {587, 1.0}, {659, 1.0}, {523, 1.0}, {440, 1.0}, {440, 1.5},
    {587, 1.5}, {698, 0.5}, {880, 1.0}, {784, 0.5}, {698, 0.5},
    {659, 1.5}, {523, 0.5}, {659, 1.0}, {587, 0.5}, {523, 0.5},
    {494, 1.0}, {494, 0.5}, {523, 0.5}, {587, 1.0}, {659, 1.0}, {523, 1.0}, {440, 1.0}, {440, 1.5}
}

-- ============================================================================
-- Tetris Kernel
-- ============================================================================
local Game = {
    grid = {},
    score = 0, level = 1, lines = 0,
    state = "MENU",
    tick = 0, tickRate = 0.8,
    active = nil, next = math.random(1, 7),
    camPhase = 0,
    musicTime = 0, musicIndex = 1, playNextNote = true
}

function Game:Init()
    for r = 1, Constants.Rows do
        self.grid[r] = {}
        for c = 1, Constants.Cols do self.grid[r][c] = 0 end
    end
    self:Spawn()
    camera.set_pos(0, 10, 18)
    camera.look_at(0, 9, 0)
    self.musicTime = 0
    self.musicIndex = 1
    self.playNextNote = true
end

function Game:UpdateMusic(dt)
    if not self.musicTime then self.musicTime = 0 end
    if not self.musicIndex then self.musicIndex = 1 end
    if self.playNextNote == nil then self.playNextNote = true end
    if not self.noteDuration then self.noteDuration = 0.25 end

    if self.playNextNote then
        self.playNextNote = false
        local note = Melody[self.musicIndex]
        if note and note[1] > 0 then
            -- Play melody (Triangle wave = 2 for soft lead, Square = 0 for retro feel)
            audio.play_note(note[1], note[2] * 0.25 * 0.9, 2)
            
            -- Play bass accompaniment (Square wave = 0, one octave below)
            audio.play_note(note[1] / 2, note[2] * 0.25 * 0.8, 0)
        end
        self.noteDuration = (note and note[2] or 1.0) * 0.25
    end

    self.musicTime = self.musicTime + dt
    if self.musicTime >= self.noteDuration then
        self.musicTime = self.musicTime - self.noteDuration
        self.musicIndex = self.musicIndex + 1
        if self.musicIndex > #Melody then
            self.musicIndex = 1
        end
        self.playNextNote = true
    end
end

function Game:Spawn()
    self.active = { type = self.next, r = 1, c = 4, rot = 1 }
    self.next = math.random(1, 7)
    if self:Collision(0, 0, 0) then self.state = "OVER" end
end

function Game:Collision(dr, dc, drot)
    local shape = Shapes[self.active.type]
    local nextRot = ((self.active.rot - 1 + drot) % #shape.rotations) + 1
    local mat = shape.rotations[nextRot]
    
    for r = 1, #mat do
        for c = 1, #mat[r] do
            if mat[r][c] == 1 then
                local gr = self.active.r + r - 1 + dr
                local gc = self.active.c + c - 1 + dc
                if gc < 1 or gc > Constants.Cols or gr > Constants.Rows then return true end
                if gr > 0 and self.grid[gr][gc] > 0 then return true end
            end
        end
    end
    return false
end

function Game:Lock()
    local shape = Shapes[self.active.type]
    local mat = shape.rotations[self.active.rot]
    for r = 1, #mat do
        for c = 1, #mat[r] do
            if mat[r][c] == 1 then
                local gr = self.active.r + r - 1
                local gc = self.active.c + c - 1
                if gr > 0 then self.grid[gr][gc] = self.active.type end
            end
        end
    end
    self:Clear()
    self:Spawn()
end

function Game:Clear()
    local cleared = 0
    for r = Constants.Rows, 1, -1 do
        local full = true
        for c = 1, Constants.Cols do if self.grid[r][c] == 0 then full = false; break end end
        if full then
            cleared = cleared + 1
            table.remove(self.grid, r)
            local newRow = {}
            for c = 1, Constants.Cols do newRow[c] = 0 end
            table.insert(self.grid, 1, newRow)
            vfx.emit(0, Constants.Rows - r, 0, 0, 5, 0, 0, 1, 1, 50, 0.1)
        end
    end
    if cleared > 0 then
        self.lines = self.lines + cleared
        self.score = self.score + (cleared * 100 * self.level)
        self.level = math.floor(self.lines / 10) + 1
        self.tickRate = math.max(0.1, 0.8 - (self.level * 0.05))
        audio.fm_note(440 + cleared * 100, 0.2, 7)
    end
end

function Game:Update(dt)
    self:UpdateMusic(dt)

    if self.state ~= "PLAY" then 
        if input.is_just_pressed("Space") or input.is_just_pressed("Enter") then 
            if self.state == "OVER" then self:Init() end
            self.state = "PLAY" 
        end
        return 
    end

    -- Input
    if input.is_just_pressed("Left") and not self:Collision(0, -1, 0) then self.active.c = self.active.c - 1 end
    if input.is_just_pressed("Right") and not self:Collision(0, 1, 0) then self.active.c = self.active.c + 1 end
    if input.is_just_pressed("Up") and not self:Collision(0, 0, 1) then 
        self.active.rot = (self.active.rot % #Shapes[self.active.type].rotations) + 1 
    end
    
    if input.is_just_pressed("Space") then
        local drop = 0
        while not self:Collision(drop + 1, 0, 0) do drop = drop + 1 end
        self.active.r = self.active.r + drop
        self:Lock()
        Constants.ShakeIntensity = 0.5
    end

    -- Tick
    self.tick = self.tick + dt * (input.is_down("Down") and 10 or 1)
    if self.tick >= self.tickRate then
        self.tick = 0
        if not self:Collision(1, 0, 0) then self.active.r = self.active.r + 1
        else self:Lock() end
    end

    -- Dynamic Camera
    self.camPhase = self.camPhase + dt * 0.5
    local shakeX = (math.random() - 0.5) * Constants.ShakeIntensity
    local shakeY = (math.random() - 0.5) * Constants.ShakeIntensity
    camera.set_pos(math.sin(self.camPhase) * 1.5 + shakeX, 11 + shakeY, 18)
    camera.look_at(0, 8.5, 0)
    Constants.ShakeIntensity = Constants.ShakeIntensity * 0.9
end

function Game:DrawBlock(r, c, type, alpha)
    local s = Shapes[type]
    local px = Constants.BoardX + (c - 0.5)
    local py = Constants.Rows - r + 0.5
    gfx.draw_pbr_cube(px, py, 0, 0.45, s.color[1], s.color[2], s.color[3], s.metal, s.rough)
end

function Game:Draw()
    -- Grid Floor
    gfx.draw_pbr_cube(0, -0.1, 0, 5, 0.1, 0.1, 0.1, 0.5, 0.8)
    
    -- Blocks
    for r = 1, Constants.Rows do
        for c = 1, Constants.Cols do
            if self.grid[r][c] > 0 then self:DrawBlock(r, c, self.grid[r][c], 1.0) end
        end
    end
    
    -- Active
    if self.active then
        local mat = Shapes[self.active.type].rotations[self.active.rot]
        for r = 1, #mat do
            for c = 1, #mat[r] do
                if mat[r][c] == 1 then self:DrawBlock(self.active.r + r - 1, self.active.c + c - 1, self.active.type, 1.0) end
            end
        end
        
        -- Ghost
        local drop = 0
        while not self:Collision(drop + 1, 0, 0) do drop = drop + 1 end
        for r = 1, #mat do
            for c = 1, #mat[r] do
                if mat[r][c] == 1 then
                    local gr = self.active.r + drop + r - 1
                    local gc = self.active.c + c - 1
                    local px = Constants.BoardX + (gc - 0.5)
                    local py = Constants.Rows - gr + 0.5
                    gfx.draw_pbr_cube(px, py, 0, 0.42, 1, 1, 1, 0, 1)
                end
            end
        end
    end
end

-- ============================================================================
-- Engine Hooks
-- ============================================================================
function OnStart()
    Game:Init()
end

function OnUpdate(dt)
    Game:Update(dt)
end

function OnRender()
    Game:Draw()
end

function OnRenderUI()
    local w, h = gfx.screen_width(), gfx.screen_height()
    if Game.state == "MENU" then
        gfx.draw_rect_alpha(0, 0, w, h, 0, 0, 0, 0.8)
        gfx.draw_text("INDUSTRIAL TETRIS", w/2 - 150, h/2 - 50, 2.5, 0, 1, 1, 1)
        gfx.draw_text("PRESS SPACE TO START", w/2 - 120, h/2 + 20, 1.2, 1, 1, 1, 0.8)
    elseif Game.state == "OVER" then
        gfx.draw_rect_alpha(0, 0, w, h, 0.2, 0, 0, 0.8)
        gfx.draw_text("GAME OVER", w/2 - 100, h/2 - 50, 3.0, 1, 0, 0, 1)
        gfx.draw_text("SCORE: " .. Game.score, w/2 - 60, h/2 + 20, 1.5, 1, 1, 1, 1)
        gfx.draw_text("SPACE TO RESTART", w/2 - 80, h/2 + 60, 1.0, 1, 1, 1, 0.6)
    else
        gfx.draw_rect_outline(10, 10, 200, 120, 2, 0, 0.5, 1)
        gfx.draw_text("SCORE", 30, 30, 1.2, 0, 1, 1, 1)
        gfx.draw_text(tostring(Game.score), 30, 60, 2.0, 1, 1, 0, 1)
        gfx.draw_text("LEVEL " .. Game.level, 30, 100, 1.0, 0.5, 1, 0.5, 1)
    end
end
