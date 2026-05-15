-- tetris_main.lua
-- Starlight Engine SBA - Tetris Clone (Neon Edition - Arcade Rebuild)

local function S(v) return math.floor(v * (window.get_height() / 1000.0)) end

local Constants = {
    ScreenWidth = window.get_width(),
    ScreenHeight = window.get_height(),
    Cols = 10,
    Rows = 20,
    BlockSize = S(41),
    BoardX = S(195),
    BoardY = S(95)
}

local Textures = {}

-- ============================================================================
-- Button Class (Neon UI)
-- ============================================================================
local Button = Class()
function Button:Init(text, x, y, w, h, color)
    self.text = text
    self.x = x
    self.y = y
    self.w = w
    self.h = h
    self.color = color
    self.hover = false
end

function Button:Update()
    local mx = input.get_mouse_x()
    local my = input.get_mouse_y()
    
    self.hover = (mx >= self.x and mx <= self.x + self.w and
                  my >= self.y and my <= self.y + self.h)
                  
    if self.hover and input.is_just_pressed("MouseLeft") then
        audio.play_sound("assets/audio/hit.wav")
        return true
    end
    return false
end

function Button:Draw()
    local c = self.hover and {self.color[1]*1.5, self.color[2]*1.5, self.color[3]*1.5} or self.color
    gfx.draw_quad(self.x, self.y, self.w, self.h, c[1], c[2], c[3], self.hover and 0.8 or 0.4)
    gfx.draw_quad(self.x - 4, self.y - 4, self.w + 8, self.h + 8, c[1], c[2], c[3], 0.2)
    
    if imgui then
        imgui.text(self.x + self.w/2 - string.len(self.text)*4.5, self.y + self.h/2 - 8, 1.0, 1.0, 1.0, self.text)
    end
end

-- ============================================================================
-- Tetromino Definitions (SRS standard colors)
-- ============================================================================
local Shapes = {
    { -- 1: I (Cyan)
        color = {0.0, 1.0, 1.0},
        rotations = {{{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}, {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}}
    },
    { -- 2: J (Blue)
        color = {0.0, 0.2, 1.0},
        rotations = {{{0,1,0}, {0,1,0}, {1,1,0}}, {{1,0,0}, {1,1,1}, {0,0,0}}, {{0,1,1}, {0,1,0}, {0,1,0}}, {{0,0,0}, {1,1,1}, {0,0,1}}}
    },
    { -- 3: L (Orange)
        color = {1.0, 0.5, 0.0},
        rotations = {{{0,1,0}, {0,1,0}, {0,1,1}}, {{0,0,0}, {1,1,1}, {1,0,0}}, {{1,1,0}, {0,1,0}, {0,1,0}}, {{0,0,1}, {1,1,1}, {0,0,0}}}
    },
    { -- 4: O (Yellow)
        color = {1.0, 1.0, 0.0},
        rotations = {{{1,1}, {1,1}}}
    },
    { -- 5: S (Green)
        color = {0.0, 1.0, 0.0},
        rotations = {{{0,1,1}, {1,1,0}, {0,0,0}}, {{0,1,0}, {0,1,1}, {0,0,1}}}
    },
    { -- 6: T (Purple)
        color = {0.5, 0.0, 1.0},
        rotations = {{{0,0,0}, {1,1,1}, {0,1,0}}, {{0,1,0}, {1,1,0}, {0,1,0}}, {{0,1,0}, {1,1,1}, {0,0,0}}, {{0,1,0}, {0,1,1}, {0,1,0}}}
    },
    { -- 7: Z (Red)
        color = {1.0, 0.0, 0.0},
        rotations = {{{1,1,0}, {0,1,1}, {0,0,0}}, {{0,0,1}, {0,1,1}, {0,1,0}}}
    }
}

-- ============================================================================
-- GameManager Class
-- ============================================================================
local GameManager = Class()
function GameManager:Init()
    self.grid = {}
    self.state = "MAIN_MENU"
    self.score = 0
    self.lines = 0
    self.level = 1
    
    self.tickRate = 0.8
    self.tickTimer = 0

    self.activePiece = nil
    self.nextQueue = { math.random(1,7), math.random(1,7), math.random(1,7) }
    self.holdPiece = nil
    self.canHold = true
    
    self.dasDelay = 0.15
    self.dasRepeat = 0.03
    self.dasTimerLeft = 0
    self.dasTimerRight = 0
    
    self.lockDelay = 0.5
    self.lockTimer = 0

    -- Combo & B2B system
    self.combo = 0
    self.backToBack = false
    self.lineClearFlash = 0
    self.lastClearMsg = ""
    self.lastClearTimer = 0

    local cx = Constants.ScreenWidth / 2
    local cy = Constants.ScreenHeight / 2
    self.btnStart = Button("START GAME", cx - 100, cy, 200, 50, {0.0, 1.0, 1.0})
    self.btnResume = Button("RESUME", cx - 100, cy - 30, 200, 50, {0.0, 1.0, 1.0})
    self.btnQuit = Button("QUIT", cx - 100, cy + 40, 200, 50, {1.0, 0.0, 0.5})
    self.btnRestart = Button("RETRY", cx - 100, cy + 40, 200, 50, {0.0, 1.0, 0.5})
end

function GameManager:ResetBoard()
    for r = 1, Constants.Rows do
        self.grid[r] = {}
        for c = 1, Constants.Cols do
            self.grid[r][c] = 0
        end
    end
end

function GameManager:SpawnPiece()
    local type = table.remove(self.nextQueue, 1)
    table.insert(self.nextQueue, math.random(1, 7))
    
    self.activePiece = { type = type, rotation = 1, c = 4, r = 1 }
    self.canHold = true
    
    if self:CheckCollision(0, 0, 0) then
        self.state = "GAME_OVER"
    end
end

function GameManager:HoldPieceAction()
    if not self.canHold then return end
    if self.holdPiece == nil then
        self.holdPiece = self.activePiece.type
        self:SpawnPiece()
    else
        local temp = self.holdPiece
        self.holdPiece = self.activePiece.type
        self.activePiece = { type = temp, rotation = 1, c = 4, r = 1 }
    end
    self.canHold = false
    self.tickTimer = 0
    self.lockTimer = 0
end

function GameManager:CheckCollision(offsetR, offsetC, rotDelta)
    local p = self.activePiece
    if not p then return false end
    local shape = Shapes[p.type]
    local rotIdx = ((p.rotation - 1 + rotDelta) % #shape.rotations) + 1
    local mat = shape.rotations[rotIdx]

    for r = 1, #mat do
        for c = 1, #mat[r] do
            if mat[r][c] == 1 then
                local boardR = p.r + r - 1 + offsetR
                local boardC = p.c + c - 1 + offsetC
                
                if boardC < 1 or boardC > Constants.Cols or boardR > Constants.Rows then
                    return true
                end
                
                if boardR > 0 and self.grid[boardR][boardC] > 0 then
                    return true
                end
            end
        end
    end
    return false
end

function GameManager:LockPiece()
    local p = self.activePiece
    local shape = Shapes[p.type]
    local rotIdx = p.rotation
    local mat = shape.rotations[rotIdx]

    for r = 1, #mat do
        for c = 1, #mat[r] do
            if mat[r][c] == 1 then
                local boardR = p.r + r - 1
                local boardC = p.c + c - 1
                if boardR > 0 and boardR <= Constants.Rows then
                    self.grid[boardR][boardC] = p.type
                end
            end
        end
    end
    self:ClearLines()
    self:SpawnPiece()
end

function GameManager:ClearLines()
    local linesCleared = 0
    local r = Constants.Rows
    while r > 0 do
        local full = true
        for c = 1, Constants.Cols do
            if self.grid[r][c] == 0 then full = false; break end
        end

        if full then
            linesCleared = linesCleared + 1
            for shiftR = r, 2, -1 do
                for c = 1, Constants.Cols do
                    self.grid[shiftR][c] = self.grid[shiftR - 1][c]
                end
            end
            for c = 1, Constants.Cols do self.grid[1][c] = 0 end
        else
            r = r - 1 
        end
    end

    if linesCleared > 0 then
        self.lines = self.lines + linesCleared
        self.combo = self.combo + 1
        self.lineClearFlash = 0.3
        
        -- Scoring: NES-style base + combo bonus + B2B bonus
        local points = 0
        local msg = ""
        if linesCleared == 1 then points = 100; msg = "SINGLE"
        elseif linesCleared == 2 then points = 300; msg = "DOUBLE"
        elseif linesCleared == 3 then points = 500; msg = "TRIPLE"
        elseif linesCleared == 4 then points = 800; msg = "TETRIS!" end
        
        -- Back-to-Back bonus (consecutive Tetris/Triple)
        local isBig = (linesCleared >= 3)
        if isBig and self.backToBack then
            points = math.floor(points * 1.5)
            msg = "B2B " .. msg
        end
        self.backToBack = isBig
        
        -- Combo bonus
        if self.combo > 1 then
            points = points + (50 * self.combo * self.level)
            msg = msg .. " (" .. self.combo .. "x COMBO)"
        end
        
        self.score = self.score + (points * self.level)
        self.level = math.floor(self.lines / 10) + 1
        self.tickRate = math.max(0.1, 0.8 - ((self.level - 1) * 0.05))
        self.lastClearMsg = msg
        self.lastClearTimer = 2.0
        audio.play_sound("assets/audio/hit.wav")
    else
        self.combo = 0
    end
end

function GameManager:Update(dt)
    if self.state == "MAIN_MENU" then
        if self.btnStart:Update() then
            self:ResetBoard()
            self.score = 0; self.lines = 0; self.level = 1; self.tickRate = 0.8
            self.holdPiece = nil
            self.nextQueue = { math.random(1,7), math.random(1,7), math.random(1,7) }
            self:SpawnPiece()
            self.state = "PLAYING"
        end
        return
    elseif self.state == "GAME_OVER" then
        if self.btnRestart:Update() then self.state = "MAIN_MENU" end
        return
    elseif self.state == "PAUSED" then
        if self.btnResume:Update() or input.is_just_pressed("Escape") then self.state = "PLAYING" end
        if self.btnQuit:Update() then self.state = "MAIN_MENU" end
        return
    end

    if input.is_just_pressed("Escape") then self.state = "PAUSED"; return end

    -- Update clear message timer
    if self.lastClearTimer > 0 then self.lastClearTimer = self.lastClearTimer - dt end
    if self.lineClearFlash > 0 then self.lineClearFlash = self.lineClearFlash - dt end

    local moved = false
    local leftPressed = input.is_just_pressed("Left") or input.is_just_pressed("A")
    local leftDown = input.is_down("Left") or input.is_down("A")
    local rightPressed = input.is_just_pressed("Right") or input.is_just_pressed("D")
    local rightDown = input.is_down("Right") or input.is_down("D")

    if leftPressed then
        if not self:CheckCollision(0, -1, 0) then self.activePiece.c = self.activePiece.c - 1; moved = true end
        self.dasTimerLeft = self.dasDelay
    elseif leftDown then
        self.dasTimerLeft = self.dasTimerLeft - dt
        if self.dasTimerLeft <= 0 then
            if not self:CheckCollision(0, -1, 0) then self.activePiece.c = self.activePiece.c - 1; moved = true end
            self.dasTimerLeft = self.dasRepeat
        end
    end

    if rightPressed then
        if not self:CheckCollision(0, 1, 0) then self.activePiece.c = self.activePiece.c + 1; moved = true end
        self.dasTimerRight = self.dasDelay
    elseif rightDown then
        self.dasTimerRight = self.dasTimerRight - dt
        if self.dasTimerRight <= 0 then
            if not self:CheckCollision(0, 1, 0) then self.activePiece.c = self.activePiece.c + 1; moved = true end
            self.dasTimerRight = self.dasRepeat
        end
    end
    
    local upPressed = input.is_just_pressed("Up") or input.is_just_pressed("W")
    if upPressed then
        local shape = Shapes[self.activePiece.type]
        if not self:CheckCollision(0, 0, 1) then 
            self.activePiece.rotation = ((self.activePiece.rotation) % #shape.rotations) + 1; moved = true
        elseif not self:CheckCollision(0, -1, 1) then
            self.activePiece.c = self.activePiece.c - 1
            self.activePiece.rotation = ((self.activePiece.rotation) % #shape.rotations) + 1; moved = true
        elseif not self:CheckCollision(0, 1, 1) then
            self.activePiece.c = self.activePiece.c + 1
            self.activePiece.rotation = ((self.activePiece.rotation) % #shape.rotations) + 1; moved = true
        end
    end
    
    if input.is_just_pressed("C") or input.is_just_pressed("Shift") then
        self:HoldPieceAction()
    end
    
    if moved then self.lockTimer = 0 end
    
    local downDown = input.is_down("Down") or input.is_down("S")
    local dropSpeed = downDown and 15 or 1
    self.tickTimer = self.tickTimer + dt * dropSpeed

    if input.is_just_pressed("Space") then
        while not self:CheckCollision(1, 0, 0) do
            self.activePiece.r = self.activePiece.r + 1
        end
        self:LockPiece()
        self.tickTimer = 0; self.lockTimer = 0
    end

    if self.tickTimer >= self.tickRate then
        self.tickTimer = 0
        if not self:CheckCollision(1, 0, 0) then
            self.activePiece.r = self.activePiece.r + 1
            self.lockTimer = 0
        end
    end
    
    if self:CheckCollision(1, 0, 0) then
        self.lockTimer = self.lockTimer + dt
        if self.lockTimer >= self.lockDelay then
            self:LockPiece()
            self.lockTimer = 0
        end
    end
end

function GameManager:DrawPieceMat(type, rot, baseR, baseC, ox, oy, alpha)
    local shape = Shapes[type]
    local mat = shape.rotations[rot]
    local col = shape.color
    for r = 1, #mat do
        for c = 1, #mat[r] do
            if mat[r][c] == 1 then
                local px = ox + (baseC + c - 2) * Constants.BlockSize
                local py = oy + (baseR + r - 2) * Constants.BlockSize
                gfx.draw_quad(px + 1, py + 1, Constants.BlockSize - 2, Constants.BlockSize - 2, col[1], col[2], col[3], alpha)
                gfx.draw_quad(px - 1, py - 1, Constants.BlockSize + 2, Constants.BlockSize + 2, col[1], col[2], col[3], alpha * 0.3)
            end
        end
    end
end

function GameManager:DrawBoard()
    gfx.draw_quad(Constants.BoardX - 2, Constants.BoardY - 2, Constants.Cols * Constants.BlockSize + 4, Constants.Rows * Constants.BlockSize + 4, 0.05, 0.05, 0.1, 0.8)
    for r = 1, Constants.Rows do
        for c = 1, Constants.Cols do
            local val = self.grid[r][c]
            if val > 0 then
                local col = Shapes[val].color
                local px = Constants.BoardX + (c - 1) * Constants.BlockSize
                local py = Constants.BoardY + (r - 1) * Constants.BlockSize
                gfx.draw_quad(px + 1, py + 1, Constants.BlockSize - 2, Constants.BlockSize - 2, col[1], col[2], col[3], 0.9)
                gfx.draw_quad(px + 4, py + 4, Constants.BlockSize - 8, Constants.BlockSize - 8, 1, 1, 1, 0.2)
            end
        end
    end
end

function GameManager:Draw()
    gfx.draw_sprite_clean(0, 0, Constants.ScreenWidth, Constants.ScreenHeight, Textures.Bg, 1.0, 1.0, 1.0, 1.0)

    if self.state ~= "MAIN_MENU" then
        self:DrawBoard()
        
        -- Draw Ghost Piece
        if self.activePiece then
            local ghostR = self.activePiece.r
            while not self:CheckCollision(ghostR - self.activePiece.r + 1, 0, 0) do
                ghostR = ghostR + 1
            end
            self:DrawPieceMat(self.activePiece.type, self.activePiece.rotation, ghostR, self.activePiece.c, Constants.BoardX, Constants.BoardY, 0.2)
            
            -- Draw Active Piece
            self:DrawPieceMat(self.activePiece.type, self.activePiece.rotation, self.activePiece.r, self.activePiece.c, Constants.BoardX, Constants.BoardY, 1.0)
        end
        
        -- Draw UI Masks based on precise positions from the background image
        gfx.draw_quad(S(50), S(100), S(130), S(45), 0.05, 0.02, 0.1, 0.9) -- Score Mask
        gfx.draw_quad(S(50), S(205), S(130), S(45), 0.05, 0.02, 0.1, 0.9) -- Level Mask
        gfx.draw_quad(S(50), S(305), S(130), S(45), 0.05, 0.02, 0.1, 0.9) -- Lines Mask
        gfx.draw_quad(S(50), S(840), S(140), S(90), 0.05, 0.02, 0.1, 0.9) -- Highscore Mask
        
        if imgui then
            imgui.text(S(80), S(115), 0.0, 1.0, 1.0, tostring(self.score))
            imgui.text(S(100), S(220), 1.0, 1.0, 0.0, tostring(self.level))
            imgui.text(S(100), S(320), 1.0, 0.0, 1.0, tostring(self.lines))
            imgui.text(S(80), S(860), 0.0, 1.0, 0.5, tostring(self.score))
        end
        
        -- Draw Next Piece in its box
        if self.nextQueue[1] then
            self:DrawPieceMat(self.nextQueue[1], 1, 1, 1, S(630), S(130), 1.0)
        end
        
        -- Draw Hold Piece in its box
        if self.holdPiece then
            self:DrawPieceMat(self.holdPiece, 1, 1, 1, S(630), S(330), self.canHold and 1.0 or 0.3)
        end
        
        -- Line Clear Flash overlay
        if self.lineClearFlash > 0 then
            local flashAlpha = self.lineClearFlash * 2
            gfx.draw_quad(Constants.BoardX, Constants.BoardY, Constants.Cols * Constants.BlockSize, Constants.Rows * Constants.BlockSize, 1, 1, 1, flashAlpha)
        end
        
        -- Combo/B2B Message
        if self.lastClearTimer > 0 and self.lastClearMsg ~= "" then
            local msgAlpha = MathX.clamp(self.lastClearTimer, 0, 1)
            local mr, mg, mb = Color.hsv((time.get_time() * 0.3) % 1.0, 0.7, 1.0)
            if imgui then
                imgui.text(Constants.BoardX + 20, Constants.BoardY + Constants.Rows * Constants.BlockSize + 20, mr, mg, mb, self.lastClearMsg)
            end
        end
        
        -- Combo Counter
        if self.combo > 1 then
            if imgui then
                imgui.text(S(50), S(420), 1, 0.5, 0, "COMBO: " .. self.combo)
            end
        end
    end

    local cx = Constants.ScreenWidth / 2
    local cy = Constants.ScreenHeight / 2

    if self.state == "MAIN_MENU" then
        gfx.draw_quad(0, 0, Constants.ScreenWidth, Constants.ScreenHeight, 0, 0, 0, 0.6)
        if imgui then imgui.text(cx - 100, cy - 100, 0.0, 1.0, 1.0, "STARLIGHT TETRIS") end
        self.btnStart:Draw()

    elseif self.state == "PAUSED" then
        gfx.draw_quad(0, 0, Constants.ScreenWidth, Constants.ScreenHeight, 0, 0, 0, 0.7)
        if imgui then imgui.text(cx - 30, cy - 80, 1.0, 1.0, 1.0, "PAUSED") end
        self.btnResume:Draw()
        self.btnQuit:Draw()

    elseif self.state == "GAME_OVER" then
        gfx.draw_quad(0, 0, Constants.ScreenWidth, Constants.ScreenHeight, 0, 0, 0, 0.6)
        if imgui then 
            imgui.text(cx - 50, cy - 120, 1.0, 0.0, 0.0, "GAME OVER")
            imgui.text(cx - 60, cy - 60, 0.0, 1.0, 1.0, "FINAL SCORE: " .. tostring(self.score))
        end
        self.btnRestart:Draw()
    end
end

local Game = nil

function OnStart()
    Engine.log("Starlight Tetris (Arcade Edition) Initialized!")
    Textures.Bg = assets.load_texture("assets/textures/synthwave_tetris_bg.png")
    Game = GameManager()
end

function OnUpdate(dt)
    if Game then Game:Update(dt) end
end

function OnRenderUI()
    if Game then Game:Draw() end
end
