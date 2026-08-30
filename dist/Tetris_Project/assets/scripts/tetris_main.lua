-- tetris_main.lua
-- RADICAL RECONSTRUCTION v6.0: INDUSTRIAL NEON EDITION
-- Fully complete with Hold Piece, Next Piece Preview, DAS, and Neon HUD

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
-- Standard Tetris Guideline Rotations (SRS Bounding Boxes)
-- ============================================================================
local Shapes = {
    { color = {0.0, 1.0, 1.0}, metal = 0.1, rough = 0.05, name = "I", -- Cyan
        rotations = {
            {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
            {{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}},
            {{0,0,0,0}, {0,0,0,0}, {1,1,1,1}, {0,0,0,0}},
            {{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}
        }
    },
    { color = {0.2, 0.4, 1.0}, metal = 0.3, rough = 0.1, name = "J", -- Blue
        rotations = {
            {{1,0,0}, {1,1,1}, {0,0,0}},
            {{0,1,1}, {0,1,0}, {0,1,0}},
            {{0,0,0}, {1,1,1}, {0,0,1}},
            {{0,1,0}, {0,1,0}, {1,1,0}}
        }
    },
    { color = {1.0, 0.5, 0.0}, metal = 0.8, rough = 0.4, name = "L", -- Orange
        rotations = {
            {{0,0,1}, {1,1,1}, {0,0,0}},
            {{0,1,0}, {0,1,0}, {0,1,1}},
            {{0,0,0}, {1,1,1}, {1,0,0}},
            {{1,1,0}, {0,1,0}, {0,1,0}}
        }
    },
    { color = {1.0, 0.9, 0.0}, metal = 0.9, rough = 0.1, name = "O", -- Gold
        rotations = {
            {{1,1}, {1,1}},
            {{1,1}, {1,1}},
            {{1,1}, {1,1}},
            {{1,1}, {1,1}}
        }
    },
    { color = {0.1, 1.0, 0.2}, metal = 0.0, rough = 0.2, name = "S", -- Green
        rotations = {
            {{0,1,1}, {1,1,0}, {0,0,0}},
            {{0,1,0}, {0,1,1}, {0,0,1}},
            {{0,0,0}, {0,1,1}, {1,1,0}},
            {{1,0,0}, {1,1,0}, {0,1,0}}
        }
    },
    { color = {0.6, 0.0, 1.0}, metal = 0.7, rough = 0.05, name = "T", -- Purple
        rotations = {
            {{0,1,0}, {1,1,1}, {0,0,0}},
            {{0,1,0}, {0,1,1}, {0,1,0}},
            {{0,0,0}, {1,1,1}, {0,1,0}},
            {{0,1,0}, {1,1,0}, {0,1,0}}
        }
    },
    { color = {1.0, 0.1, 0.1}, metal = 0.2, rough = 0.05, name = "Z", -- Red
        rotations = {
            {{1,1,0}, {0,1,1}, {0,0,0}},
            {{0,0,1}, {0,1,1}, {0,1,0}},
            {{0,0,0}, {1,1,0}, {0,1,1}},
            {{0,1,0}, {1,1,0}, {1,0,0}}
        }
    }
}

-- ============================================================================
-- SRS (Super Rotation System) Wall Kick Tables
-- Represented as { dc, dr } where dr is positive downwards
-- ============================================================================
local SRS_Kicks = {
    ["1_2"] = { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} },
    ["2_1"] = { {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2} },
    ["2_3"] = { {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2} },
    ["3_2"] = { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} },
    ["3_4"] = { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} },
    ["4_3"] = { {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2} },
    ["4_1"] = { {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2} },
    ["1_4"] = { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} }
}

local SRS_Kicks_I = {
    ["1_2"] = { {0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2} },
    ["2_1"] = { {0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2} },
    ["2_3"] = { {0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1} },
    ["3_2"] = { {0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1} },
    ["3_4"] = { {0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2} },
    ["4_3"] = { {0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2} },
    ["4_1"] = { {0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1} },
    ["1_4"] = { {0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1} }
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
    highscore = Save.read("tetris_highscore", 0),
    state = "MENU",
    tick = 0, tickRate = 0.8,
    active = nil, next = math.random(1, 7), held = nil, canHold = true,
    camPhase = 0,
    musicTime = 0, musicIndex = 1, playNextNote = true,
    dasDirection = 0, dasTimer = 0.0,
    lockTimer = 0.0, lockResets = 0,
    lastActionWasRotation = false,
    tspinMessage = nil, tspinTimer = 0.0
}

function Game:Init()
    for r = 1, Constants.Rows do
        self.grid[r] = {}
        for c = 1, Constants.Cols do self.grid[r][c] = 0 end
    end
    self.score = 0
    self.level = 1
    self.lines = 0
    self.highscore = Save.read("tetris_highscore", 0)
    self.held = nil
    self.canHold = true
    self.lockTimer = 0.0
    self.lockResets = 0
    self.lastActionWasRotation = false
    self.tspinMessage = nil
    self.tspinTimer = 0.0
    self:Spawn()
    camera.set_pos(0, 10, 18)
    camera.look_at(0, 9, 0)
    self.musicTime = 0
    self.musicIndex = 1
    self.playNextNote = true
    self.dasDirection = 0
    self.dasTimer = 0.0
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
            audio.play_note(note[1], note[2] * 0.25 * 0.9, 2)
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
    self.canHold = true
    self.lockTimer = 0.0
    self.lockResets = 0
    self.lastActionWasRotation = false
    if self:Collision(0, 0, 0) then 
        self.state = "OVER" 
        Audio.playExplosion()
        if self.score > self.highscore then
            self.highscore = self.score
            Save.write("tetris_highscore", self.highscore)
            Save.flush()
        end
    end
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

function Game:CheckTSpin()
    if not self.active or Shapes[self.active.type].name ~= "T" then return false end
    if not self.lastActionWasRotation then return false end
    
    local occupied = 0
    local corners = {
        {self.active.r, self.active.c},
        {self.active.r, self.active.c + 2},
        {self.active.r + 2, self.active.c},
        {self.active.r + 2, self.active.c + 2}
    }
    for _, corner in ipairs(corners) do
        local r, c = corner[1], corner[2]
        if r < 1 or r > Constants.Rows or c < 1 or c > Constants.Cols then
            occupied = occupied + 1
        elseif self.grid[r][c] > 0 then
            occupied = occupied + 1
        end
    end
    return occupied >= 3
end

function Game:Lock()
    local isTspin = self:CheckTSpin()
    
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
    
    self:Clear(isTspin)
    self:Spawn()
end

function Game:Clear(isTspin)
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
            for c = 1, Constants.Cols do
                vfx.emit(c - Constants.Cols / 2, Constants.Rows - r, 0, 0, 5, 0, 0, 1, 1, 5, 0.15)
            end
        end
    end
    
    if cleared > 0 or isTspin then
        local points = 0
        if isTspin then
            if cleared == 1 then
                points = 800
                self.tspinMessage = "T-SPIN SINGLE!"
            elseif cleared == 2 then
                points = 1200
                self.tspinMessage = "T-SPIN DOUBLE!!"
            elseif cleared == 3 then
                points = 1600
                self.tspinMessage = "T-SPIN TRIPLE!!!"
            else
                points = 400
                self.tspinMessage = "T-SPIN!"
            end
            self.tspinTimer = 1.5
            Audio.playPowerup()
        else
            if cleared == 1 then
                points = 100
            elseif cleared == 2 then
                points = 300
            elseif cleared == 3 then
                points = 500
            elseif cleared == 4 then
                points = 800
                self.tspinMessage = "TETRIS!"
                self.tspinTimer = 1.5
            end
        end
        
        self.lines = self.lines + cleared
        self.score = self.score + (points * self.level)
        self.level = math.floor(self.lines / 10) + 1
        self.tickRate = math.max(0.1, 0.8 - (self.level * 0.05))
        
        if not isTspin then
            if cleared == 4 then
                if audio and audio.fm_note then
                    audio.fm_note(523.25, 0.35, 0)
                    audio.fm_note(659.25, 0.35, 1)
                    audio.fm_note(783.99, 0.35, 2)
                    audio.fm_note(1046.50, 0.45, 3)
                else
                    Audio.playPowerup()
                end
            elseif cleared == 3 then
                if audio and audio.play_synth then audio.play_synth(783.99, 0.16, "square") end
            elseif cleared == 2 then
                if audio and audio.play_synth then audio.play_synth(659.25, 0.12, "square") end
            elseif cleared == 1 then
                if audio and audio.play_synth then audio.play_synth(523.25, 0.08, "square") end
            end
        end
    end
end

function Game:Hold()
    if not self.canHold then return end
    
    local currentType = self.active.type
    if self.held then
        self.active = { type = self.held, r = 1, c = 4, rot = 1 }
        self.held = currentType
    else
        self.held = currentType
        self:Spawn()
    end
    self.canHold = false
    self.lockTimer = 0.0
    self.lockResets = 0
    Audio.setADSR(0.005, 0.1, 0.2, 0.05)
    Audio.fm(600, 0.15, 3)
end

function Game:Rotate(dir)
    local shape = Shapes[self.active.type]
    if shape.name == "O" then return end
    
    local prevRot = self.active.rot
    local nextRot
    if dir == 1 then
        nextRot = (prevRot % #shape.rotations) + 1
    else
        nextRot = prevRot - 1
        if nextRot < 1 then nextRot = #shape.rotations end
    end
    
    local kickKey = tostring(prevRot) .. "_" .. tostring(nextRot)
    local kicks = (shape.name == "I") and SRS_Kicks_I[kickKey] or SRS_Kicks[kickKey]
    
    if kicks then
        for i = 1, #kicks do
            local drot = nextRot - prevRot
            local dc = kicks[i][1]
            local dr = kicks[i][2]
            if not self:Collision(dr, dc, drot) then
                self.active.rot = nextRot
                self.active.r = self.active.r + dr
                self.active.c = self.active.c + dc
                
                -- Mark last action as rotation
                self.lastActionWasRotation = true
                
                Audio.setADSR(0.002, 0.03, 0.1, 0.02)
                Audio.fm(400, 0.04, 2)
                
                -- Reset Lock Delay
                if self.lockTimer > 0 and self.lockResets < 15 then
                    self.lockTimer = 0.5
                    self.lockResets = self.lockResets + 1
                end
                return
            end
        end
    end
end

function Game:Update(dt)
    self:UpdateMusic(dt)

    if self.state ~= "PLAY" then 
        if input.is_just_pressed("Space") or input.is_just_pressed("Enter") or input.is_just_pressed("Return") then 
            if self.state == "OVER" then self:Init() end
            self.state = "PLAY" 
        end
        return 
    end

    -- T-Spin text timer
    if self.tspinTimer and self.tspinTimer > 0 then
        self.tspinTimer = self.tspinTimer - dt
        if self.tspinTimer <= 0 then
            self.tspinMessage = nil
        end
    end

    -- Hold Action
    if Input.isJustPressed("Hold") then
        self:Hold()
    end

    -- Rotations
    if Input.isJustPressed("RotateCW") then
        self:Rotate(1)
    elseif Input.isJustPressed("RotateCCW") then
        self:Rotate(-1)
    end

    -- Hard Drop
    if Input.isJustPressed("HardDrop") then
        local drop = 0
        while not self:Collision(drop + 1, 0, 0) do drop = drop + 1 end
        self.active.r = self.active.r + drop
        self:Lock()
        ScreenShake.trigger(12, 0.3)
        Audio.setADSR(0.005, 0.08, 0.1, 0.05)
        Audio.fm(150, 0.1, 1)
    end

    -- DAS (Delayed Auto Shift) Movement
    local horiz = Input.getAxis("Horizontal")
    local leftDown = horiz < -0.5
    local rightDown = horiz > 0.5

    if leftDown or rightDown then
        local dir = leftDown and -1 or 1
        if self.dasDirection ~= dir then
            self.dasDirection = dir
            self.dasTimer = 0.0
            if not self:Collision(0, dir, 0) then
                self.active.c = self.active.c + dir
                self.lastActionWasRotation = false
                Audio.setADSR(0.001, 0.02, 0.1, 0.02)
                Audio.fm(500, 0.03, 0)
                
                -- Reset Lock Delay
                if self.lockTimer > 0 and self.lockResets < 15 then
                    self.lockTimer = 0.5
                    self.lockResets = self.lockResets + 1
                end
            end
        else
            self.dasTimer = self.dasTimer + dt
            if self.dasTimer >= 0.20 then
                local repeats = math.floor((self.dasTimer - 0.20) / 0.05)
                if repeats > 0 then
                    self.dasTimer = 0.20 + (self.dasTimer - 0.20) % 0.05
                    for i = 1, repeats do
                        if not self:Collision(0, dir, 0) then
                            self.active.c = self.active.c + dir
                            self.lastActionWasRotation = false
                            Audio.setADSR(0.001, 0.02, 0.1, 0.02)
                            Audio.fm(500, 0.03, 0)
                            
                            -- Reset Lock Delay
                            if self.lockTimer > 0 and self.lockResets < 15 then
                                self.lockTimer = 0.5
                                self.lockResets = self.lockResets + 1
                            end
                        else
                            break
                        end
                    end
                end
            end
        end
    else
        self.dasDirection = 0
        self.dasTimer = 0.0
    end

    -- Gravity / Soft Drop Tick
    local softDrop = Input.isDown("SoftDrop")
    self.tick = self.tick + dt * (softDrop and 20 or 1)
    if self.tick >= self.tickRate then
        self.tick = 0
        if not self:Collision(1, 0, 0) then 
            self.active.r = self.active.r + 1
            self.lastActionWasRotation = false
            if softDrop then 
                Audio.setADSR(0.001, 0.02, 0.05, 0.01)
                Audio.fm(200, 0.02, 3) 
                self.score = self.score + 1
            end
            self.lockTimer = 0.0
        else 
            if softDrop then
                self:Lock() 
            end
        end
    end

    -- Lock Delay logic
    if self:Collision(1, 0, 0) then
        if self.lockTimer <= 0 then
            self.lockTimer = 0.5
            self.lockResets = 0
        else
            self.lockTimer = self.lockTimer - dt
            if self.lockTimer <= 0 then
                self:Lock()
            end
        end
    else
        self.lockTimer = 0.0
    end

    -- Dynamic Camera
    ScreenShake.update(dt)
    local shakeX, shakeY = ScreenShake.getOffset()
    self.camPhase = self.camPhase + dt * 0.5
    camera.set_pos(math.sin(self.camPhase) * 1.5 + shakeX * 0.1, 11 + shakeY * 0.1, 18)
    camera.look_at(0, 8.5, 0)
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
        
        -- Improved Holographic Ghost Piece matching Tetromino color
        local drop = 0
        while not self:Collision(drop + 1, 0, 0) do drop = drop + 1 end
        local activeColor = Shapes[self.active.type].color
        for r = 1, #mat do
            for c = 1, #mat[r] do
                if mat[r][c] == 1 then
                    local gr = self.active.r + drop + r - 1
                    local gc = self.active.c + c - 1
                    local px = Constants.BoardX + (gc - 0.5)
                    local py = Constants.Rows - gr + 0.5
                    -- Dimmer version of tetromino color, zero metal, high rough
                    gfx.draw_pbr_cube(px, py, 0, 0.42, activeColor[1] * 0.25, activeColor[2] * 0.25, activeColor[3] * 0.25, 0.0, 1.0)
                end
            end
        end
    end
end

-- Draw Tetromino in 2D HUD
local function DrawTetromino2D(type, x, y, blockSize)
    if not type then return end
    local shape = Shapes[type]
    local mat = shape.rotations[1]
    local color = shape.color
    
    -- Centering calculation
    local rows = #mat
    local cols = #mat[1] or 0
    local ox = x - (cols * blockSize) / 2
    local oy = y - (rows * blockSize) / 2
    
    for r = 1, rows do
        for c = 1, cols do
            if mat[r][c] == 1 then
                local px = ox + (c - 1) * blockSize
                local py = oy + (r - 1) * blockSize
                gfx.draw_rect(px, py, blockSize - 2, blockSize - 2, color[1], color[2], color[3], 1.0)
                gfx.draw_rect_outline(px, py, blockSize - 2, blockSize - 2, 2, color[1]*1.5, color[2]*1.5, color[3]*1.5, 0.5)
            end
        end
    end
end

-- ============================================================================
-- Engine Hooks
-- ============================================================================
function OnStart()
    Input.map("Left", "Left", "A")
    Input.map("Right", "Right", "D")
    Input.map("RotateCW", "Up", "W", "E")
    Input.map("RotateCCW", "Z", "Q")
    Input.map("HardDrop", "Space")
    Input.map("SoftDrop", "Down", "S")
    Input.map("Hold", "C", "LeftShift", "RightShift")
    
    Input.mapAxis("Horizontal", "Left", "Right", "LeftX")
    
    Game:Init()
end

local ShowTelemetry = false

function OnUpdate(dt)
    if input.is_just_pressed("F3") then
        ShowTelemetry = not ShowTelemetry
        if audio and audio.play_synth then audio.play_synth(880, 0.05, "sine") end
    end
    Game:Update(dt)
end

function OnRender()
    Game:Draw()
end

function OnRenderUI()
    local w, h = gfx.screen_width(), gfx.screen_height()
    
    if Game.state == "MENU" then
        gfx.draw_rect_alpha(0, 0, w, h, 0.02, 0.02, 0.06, 0.85)
        local cr, cg, cb = Color.hsv((time.get_time() * 0.1) % 1.0, 0.8, 1.0)
        
        gfx.draw_rect(w/2 - 250, h/2 - 100, 500, 4, cr, cg, cb, 1.0)
        gfx.draw_text("INDUSTRIAL TETRIS", w/2 - 180, h/2 - 50, 2.5, cr, cg, cb, 1.0)
        gfx.draw_text("SBA v4.0 NEON ARCHITECTURE", w/2 - 135, h/2 - 10, 1.0, 0.4, 0.5, 0.6, 1.0)
        gfx.draw_text("PRESS SPACE / ENTER TO START", w/2 - 145, h/2 + 50, 1.1, 1, 1, 1, 0.8)
        
        gfx.draw_text("HIGHSCORE: " .. Game.highscore, w/2 - 60, h/2 + 20, 1.2, 1.0, 0.85, 0.3, 1.0)
        
        gfx.draw_text("Controls: Left/Right / A/D to Move | Up/W/E to Rotate CW | Z/Q to Rotate CCW", w/2 - 290, h/2 + 130, 0.9, 0.3, 0.4, 0.5, 1.0)
        gfx.draw_text("Space for Hard Drop | C / Shift to Hold Piece | F3: Live Stats", w/2 - 200, h/2 + 155, 0.9, 0.3, 0.4, 0.5, 1.0)
    elseif Game.state == "OVER" then
        gfx.draw_rect_alpha(0, 0, w, h, 0.15, 0.02, 0.02, 0.9)
        gfx.draw_text("GAME OVER", w/2 - 110, h/2 - 50, 3.0, 1.0, 0.2, 0.2, 1.0)
        gfx.draw_text("FINAL SCORE: " .. Game.score, w/2 - 80, h/2 + 20, 1.3, 1, 1, 1, 1)
        gfx.draw_text("Lines Cleared: " .. Game.lines, w/2 - 60, h/2 + 50, 1.0, 0.5, 0.8, 1, 0.8)
        gfx.draw_text("PRESS SPACE OR ENTER TO RESTART", w/2 - 160, h/2 + 100, 1.1, 0.4, 1.0, 0.5, 1.0)
    else
        Engine.set_bloom(1.1, 10)
        
        -- Left Panel: Score and Info
        gfx.draw_rect_outline(30, 80, 240, 480, 3, 0, 0.5, 1, 0.7)
        gfx.draw_rect(30, 80, 240, 480, 0.02, 0.03, 0.05, 0.6)
        
        gfx.draw_text("SCORE", 50, 100, 1.1, 0, 0.8, 1.0, 1.0)
        gfx.draw_text(string.format("%06d", Game.score), 50, 125, 1.8, 1, 1, 0, 1.0)
        
        gfx.draw_text("HI-SCORE", 50, 170, 1.1, 0, 0.8, 1.0, 1.0)
        gfx.draw_text(string.format("%06d", Game.highscore), 50, 195, 1.8, 1.0, 0.85, 0.3, 1.0)
        
        gfx.draw_text("LEVEL", 50, 255, 1.1, 0, 0.8, 1.0, 1.0)
        gfx.draw_text(tostring(Game.level), 50, 280, 1.8, 0.5, 1.0, 0.5, 1.0)
        
        gfx.draw_text("LINES", 50, 340, 1.1, 0, 0.8, 1.0, 1.0)
        gfx.draw_text(tostring(Game.lines), 50, 365, 1.8, 1.0, 0.5, 0.5, 1.0)
        
        -- Right Panel: Next Piece Preview
        gfx.draw_rect_outline(w - 270, 80, 240, 200, 3, 0, 0.5, 1, 0.7)
        gfx.draw_rect(w - 270, 80, 240, 200, 0.02, 0.03, 0.05, 0.6)
        gfx.draw_text("NEXT PIECE", w - 210, 110, 1.2, 0.0, 0.8, 1.0, 1.0)
        DrawTetromino2D(Game.next, w - 150, 190, 24)
        
        -- Bottom-Right Panel: Hold Piece
        gfx.draw_rect_outline(w - 270, 310, 240, 200, 3, 0, 0.5, 1, 0.7)
        gfx.draw_rect(w - 270, 310, 240, 200, 0.02, 0.03, 0.05, 0.6)
        gfx.draw_text("HOLD PIECE", w - 210, 340, 1.2, 0.0, 0.8, 1.0, 1.0)
        if Game.held then
            DrawTetromino2D(Game.held, w - 150, 420, 24)
        else
            gfx.draw_text("EMPTY (C)", w - 195, 415, 1.0, 0.3, 0.4, 0.5, 0.7)
        end
        
        -- T-Spin/Tetris Popup Notification
        if Game.tspinMessage then
            local cr, cg, cb = Color.hsv((time.get_time() * 2.0) % 1.0, 0.9, 1.0)
            gfx.draw_text(Game.tspinMessage, w/2 - 120, h/2 - 150, 2.0, cr, cg, cb, 1.0)
        end

        -- Mini instructions at the bottom
        gfx.draw_text("A/D - Move | W/E - Rotate CW | Z/Q - Rotate CCW | Space - Hard Drop | C - Hold | F3 - Stats", 170, h - 30, 0.9, 0.4, 0.5, 0.6, 1.0)
    end
    
    if ShowTelemetry and engine and engine.get_telemetry then
        local telem = engine.get_telemetry()
        local boxW, boxH = 430, 95
        local bx, by = 20, h - boxH - 50
        gfx.draw_rect(bx, by, boxW, boxH, 0.04, 0.07, 0.1, 0.92)
        gfx.draw_rect_outline(bx, by, boxW, boxH, 1.5, 0.0, 0.9, 1.0, 0.85)
        gfx.draw_text("STARLIGHT LIVE TELEMETRY (F3)", bx + 12, by + 10, 0.95, 0.0, 1.0, 0.9, 1.0)
        gfx.draw_text(string.format("FPS: %d (%.2f ms) | Target: %d | Entities: %d", telem.fps or 60, (telem.avg_frame_time or 0.016)*1000.0, telem.target_fps or 60, telem.entities_count or 0), bx + 12, by + 30, 0.82, 0.85, 0.9, 0.95, 1.0)
        gfx.draw_text(string.format("Draw Calls: %d | Quads: %d | Audio Voices: %d", telem.draw_calls or 0, telem.quad_count or 0, telem.active_audio_voices or 0), bx + 12, by + 50, 0.82, 0.85, 0.9, 0.95, 1.0)
        gfx.draw_text(string.format("Memory Alloc: %.1f / %.1f KB", telem.memory_used_kb or 0, telem.memory_capacity_kb or 0), bx + 12, by + 70, 0.82, 0.3, 1.0, 0.5, 1.0)
    end
end
