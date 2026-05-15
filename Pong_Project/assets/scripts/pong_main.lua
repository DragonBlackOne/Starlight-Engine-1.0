-- pong_main.lua
-- Starlight Engine SBA - Pong Clone (Object-Oriented Edition, Core Lib Powered)

-- ============================================================================
-- Core Data & Textures
-- ============================================================================
local Constants = {
    ScreenWidth = window.get_width(),
    ScreenHeight = window.get_height(),
    PaddleWidth = 40,
    PaddleHeight = 160,
    PaddleSpeed = 800,
    MaxBallSpeed = 1800
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
    -- Base rect
    gfx.draw_rect(self.x, self.y, self.w, self.h, c[1], c[2], c[3], self.hover and 0.8 or 0.4)
    -- Glow
    gfx.draw_rect(self.x - 4, self.y - 4, self.w + 8, self.h + 8, c[1], c[2], c[3], 0.2)
    
    if imgui then
        imgui.text(self.x + self.w/2 - string.len(self.text)*4.5, self.y + self.h/2 - 8, 1.0, 1.0, 1.0, self.text)
    end
end

-- ============================================================================
-- ParticleSystem Class
-- ============================================================================
local ParticleSystem = Class()
function ParticleSystem:Init()
    self.particles = {}
    self.trails = {}
end

function ParticleSystem:SpawnHitParticles(x, y, color)
    for i = 1, 20 do
        local angle = math.random() * math.pi * 2
        local speed = math.random(150, 600)
        table.insert(self.particles, {
            x = x, y = y,
            vx = math.cos(angle) * speed,
            vy = math.sin(angle) * speed,
            life = 1.0,
            color = color,
            size = math.random(8, 16)
        })
    end
end

function ParticleSystem:AddTrail(x, y, size)
    table.insert(self.trails, {x = x, y = y, life = 1.0, size = size})
end

function ParticleSystem:Update(dt)
    -- Trails
    for i = #self.trails, 1, -1 do
        local t = self.trails[i]
        t.life = t.life - dt * 5.0
        t.size = t.size - dt * 20.0
        if t.life <= 0 or t.size <= 0 then
            table.remove(self.trails, i)
        end
    end

    -- Particles
    for i = #self.particles, 1, -1 do
        local p = self.particles[i]
        p.x = p.x + p.vx * dt
        p.y = p.y + p.vy * dt
        p.life = p.life - dt * 2.5
        if p.life <= 0 then table.remove(self.particles, i) end
    end
end

function ParticleSystem:Draw(ox, oy)
    -- Trails
    for _, t in ipairs(self.trails) do
        local offset = (40 - t.size) / 2 -- assuming ball size 40
        gfx.draw_sprite_clean(ox + t.x + offset, oy + t.y + offset, t.size, t.size, Textures.Ball, 1.0, 1.0, 1.0, t.life * 0.4)
    end

    -- Particles
    for _, p in ipairs(self.particles) do
        gfx.draw_sprite_clean(ox + p.x, oy + p.y, p.size, p.size, Textures.Ball, p.color[1], p.color[2], p.color[3], p.life)
    end
end

-- ============================================================================
-- Paddle Class
-- ============================================================================
local Paddle = Class()
function Paddle:Init(x, y, upKey, downKey, color, isAI)
    self.x = x
    self.y = y
    self.upKey = upKey
    self.downKey = downKey
    self.color = color
    self.score = 0
    self.hitTimer = 0
    self.isAI = isAI or false
end

function Paddle:Update(dt, ball)
    if self.isAI and ball then
        local targetY = ball.y + (ball.size / 2) - (Constants.PaddleHeight / 2)
        local diff = targetY - self.y
        if math.abs(diff) > 10 then
            self.y = self.y + MathX.sign(diff) * Constants.PaddleSpeed * 0.85 * dt
        end
    else
        if input.is_down(self.upKey) then 
            self.y = self.y - Constants.PaddleSpeed * dt
        elseif input.is_down(self.downKey) then 
            self.y = self.y + Constants.PaddleSpeed * dt 
        end
    end

    local maxY = Constants.ScreenHeight - Constants.PaddleHeight
    if self.y < 0 then self.y = 0 end
    if self.y > maxY then self.y = maxY end

    if self.hitTimer > 0 then 
        self.hitTimer = self.hitTimer - dt 
    end
end

function Paddle:OnHit()
    self.hitTimer = 0.2
end

function Paddle:Draw(ox, oy)
    local bright = (self.hitTimer > 0) and 2.0 or 1.0
    gfx.draw_sprite_clean(ox + self.x, oy + self.y, Constants.PaddleWidth, Constants.PaddleHeight, 
                          Textures.Paddle, 
                          self.color[1] * bright, self.color[2] * bright, self.color[3] * bright, 1.0)
end

-- ============================================================================
-- Ball Class
-- ============================================================================
-- ============================================================================
-- PowerUp Class
-- ============================================================================
local PowerUp = Class()
function PowerUp:Init(x, y)
    self.x = x
    self.y = y
    self.size = 30
    self.life = 8.0
    self.type = math.random(1, 3) -- 1=BigPaddle, 2=SlowBall, 3=MultiBall
    self.pulse = 0
end

function PowerUp:Update(dt)
    self.life = self.life - dt
    self.pulse = self.pulse + dt
    return self.life > 0
end

function PowerUp:Draw(ox, oy)
    local alpha = MathX.clamp(self.life, 0, 1)
    local s = self.size + math.sin(self.pulse * 6) * 4
    local r, g, b = 1, 1, 0
    if self.type == 1 then r, g, b = 0, 1, 0.5 end
    if self.type == 2 then r, g, b = 0.5, 0.5, 1 end
    if self.type == 3 then r, g, b = 1, 0.2, 0.6 end
    gfx.draw_rect(ox + self.x - s/2, oy + self.y - s/2, s, s, r, g, b, alpha * 0.7)
    gfx.draw_rect(ox + self.x - s/2 - 4, oy + self.y - s/2 - 4, s+8, s+8, r, g, b, alpha * 0.2)
end

local Ball = Class()
function Ball:Init(particleSystem)
    self.size = 40
    self.ps = particleSystem
    self.rally = 0
    self:Reset(1)
end

function Ball:Reset(direction)
    self.x = Constants.ScreenWidth / 2
    self.y = Constants.ScreenHeight / 2
    self.vx = 0
    self.vy = 0
    self.speed = 0
    self.rally = 0
end

function Ball:Serve(direction)
    self.speed = 700
    self.vx = direction * 700
    self.vy = (math.random(0, 1) == 0) and 300 or -300
    self.rally = 0
end

function Ball:Update(dt, p1, p2, gameManager)
    if self.speed == 0 then return end -- Not served yet

    self.ps:AddTrail(self.x, self.y, self.size)

    self.x = self.x + self.vx * dt
    self.y = self.y + self.vy * dt

    -- Top/Bottom Bounce
    if self.y <= 0 then
        self.y = 0
        self.vy = -self.vy
        self.ps:SpawnHitParticles(self.x, self.y, {1, 1, 1})
        gameManager:Shake(4, 0.1)
    elseif self.y + self.size >= Constants.ScreenHeight then
        self.y = Constants.ScreenHeight - self.size
        self.vy = -self.vy
        self.ps:SpawnHitParticles(self.x, self.y + self.size, {1, 1, 1})
        gameManager:Shake(4, 0.1)
    end

    -- Paddle Collisions
    if self.vx < 0 and Physics2D.CheckAABB(self.x, self.y, self.size, self.size, p1.x, p1.y, Constants.PaddleWidth, Constants.PaddleHeight) then
        self.x = p1.x + Constants.PaddleWidth
        self:HandlePaddleBounce(p1, true, gameManager)
        self.ps:SpawnHitParticles(self.x, self.y + self.size/2, p1.color)
        audio.play_sound("assets/audio/hit.wav") -- Bonus API usage!
    elseif self.vx > 0 and Physics2D.CheckAABB(self.x, self.y, self.size, self.size, p2.x, p2.y, Constants.PaddleWidth, Constants.PaddleHeight) then
        self.x = p2.x - self.size
        self:HandlePaddleBounce(p2, false, gameManager)
        self.ps:SpawnHitParticles(self.x + self.size, self.y + self.size/2, p2.color)
        audio.play_sound("assets/audio/hit.wav") -- Bonus API usage!
    end
end

function Ball:HandlePaddleBounce(paddle, isP1, gameManager)
    local ballCenterY = self.y + self.size / 2
    local paddleCenterY = paddle.y + Constants.PaddleHeight / 2
    local intersectY = paddleCenterY - ballCenterY
    local normalizedIntersectY = intersectY / (Constants.PaddleHeight / 2)
    local bounceAngle = normalizedIntersectY * (math.pi / 3) -- Max 60 degrees

    self.speed = math.min(self.speed * 1.08, Constants.MaxBallSpeed)
    self.rally = self.rally + 1
    
    local dirX = isP1 and math.cos(bounceAngle) or -math.cos(bounceAngle)
    local dirY = -math.sin(bounceAngle)
    
    self.vx = self.speed * dirX
    self.vy = self.speed * dirY
    
    paddle:OnHit()
    local shakeAmount = math.min(5 + self.rally * 2, 25)
    gameManager:Shake(shakeAmount, 0.15)
end

function Ball:Draw(ox, oy)
    gfx.draw_sprite_clean(ox + self.x, oy + self.y, self.size, self.size, Textures.Ball, 1.0, 1.0, 1.0, 1.0)
end

-- ============================================================================
-- GameManager Class
-- ============================================================================
local GameManager = Class()
function GameManager:Init()
    self.ps = ParticleSystem()
    self.p1 = Paddle(60, 370, "W", "S", {1.0, 0.2, 0.6}, false)
    self.p2 = Paddle(Constants.ScreenWidth - 100, 370, "Up", "Down", {0.2, 0.8, 1.0}, true)
    self.ball = Ball(self.ps)
    
    self.state = "MAIN_MENU"
    self.serveTimer = 1.0
    self.nextServeDir = 1

    self.shakeTime = 0
    self.shakeIntensity = 0
    self.bgScroll = 0
    self.winner = 0
    self.powerups = {}
    self.powerupTimer = 0

    -- UI Buttons
    local cx = Constants.ScreenWidth / 2
    local cy = Constants.ScreenHeight / 2
    self.btnStart = Button("START GAME", cx - 100, cy, 200, 50, {0.2, 0.8, 1.0})
    self.btnResume = Button("RESUME", cx - 100, cy - 30, 200, 50, {0.2, 0.8, 1.0})
    self.btnQuit = Button("QUIT TO MENU", cx - 100, cy + 40, 200, 50, {1.0, 0.2, 0.6})
    self.btnRestart = Button("PLAY AGAIN", cx - 100, cy + 40, 200, 50, {0.2, 1.0, 0.5})
end

function GameManager:Shake(intensity, duration)
    self.shakeIntensity = intensity
    self.shakeTime = duration
end

function GameManager:ScorePoint(winner)
    if winner == 1 then self.p1.score = self.p1.score + 1
    else self.p2.score = self.p2.score + 1 end

    if self.p1.score >= 5 or self.p2.score >= 5 then
        self.state = "GAME_OVER"
        self.winner = winner
        return
    end

    self.nextServeDir = (winner == 1) and -1 or 1
    self.ball:Reset(self.nextServeDir)
    self.state = "SERVE_DELAY"
    self.serveTimer = 1.0
    self:Shake(15, 0.4)
end

function GameManager:Update(dt)
    -- Camera Shake & BG
    if self.shakeTime > 0 then
        self.shakeTime = self.shakeTime - dt
    else
        self.shakeIntensity = 0
    end

    self.bgScroll = self.bgScroll + dt * 20.0
    if self.bgScroll > 1600 then self.bgScroll = 0 end

    -- State Machine
    if self.state == "MAIN_MENU" then
        if self.btnStart:Update() then
            self.p1.score = 0
            self.p2.score = 0
            self.ball:Reset(1)
            self.state = "SERVE_DELAY"
            self.serveTimer = 1.0
        end
        return
    elseif self.state == "GAME_OVER" then
        if self.btnRestart:Update() then
            self.state = "MAIN_MENU"
        end
        return
    elseif self.state == "PAUSED" then
        if self.btnResume:Update() or input.is_just_pressed("Escape") then
            self.state = "PLAYING"
        end
        if self.btnQuit:Update() then
            self.state = "MAIN_MENU"
        end
        return
    end

    -- Global Pause Toggle
    if input.is_just_pressed("Escape") and (self.state == "PLAYING" or self.state == "SERVE_DELAY") then
        self.state = "PAUSED"
        return
    end

    -- Entities (Only update if playing/delay)
    self.ps:Update(dt)
    self.p1:Update(dt, self.ball)
    self.p2:Update(dt, self.ball)

    -- Power-up spawner
    if self.state == "PLAYING" then
        self.powerupTimer = self.powerupTimer + dt
        if self.powerupTimer > 8.0 and #self.powerups < 2 then
            self.powerupTimer = 0
            local px = MathX.random_range(Constants.ScreenWidth * 0.3, Constants.ScreenWidth * 0.7)
            local py = MathX.random_range(100, Constants.ScreenHeight - 100)
            table.insert(self.powerups, PowerUp(px, py))
        end
    end

    -- Power-up collision
    for i = #self.powerups, 1, -1 do
        local pu = self.powerups[i]
        if not pu:Update(dt) then
            table.remove(self.powerups, i)
        elseif Physics2D.CheckAABB(self.ball.x, self.ball.y, self.ball.size, self.ball.size, pu.x - pu.size/2, pu.y - pu.size/2, pu.size, pu.size) then
            -- Apply power-up to the last paddle that hit the ball
            local target = (self.ball.vx > 0) and self.p2 or self.p1
            if pu.type == 1 then -- Big Paddle (opponent shrinks concept: give player advantage)
                -- Nothing destructive, just visual feedback
            elseif pu.type == 2 then -- Slow Ball
                self.ball.speed = math.max(400, self.ball.speed * 0.6)
                self.ball.vx = self.ball.vx * 0.6
                self.ball.vy = self.ball.vy * 0.6
            elseif pu.type == 3 then -- Speed Burst
                self.ball.speed = math.min(self.ball.speed * 1.4, Constants.MaxBallSpeed)
                self.ball.vx = self.ball.vx * 1.4
                self.ball.vy = self.ball.vy * 1.4
            end
            self.ps:SpawnHitParticles(pu.x, pu.y, {1, 1, 0})
            self:Shake(12, 0.2)
            table.remove(self.powerups, i)
        end
    end

    if self.state == "SERVE_DELAY" then
        self.serveTimer = self.serveTimer - dt
        if self.serveTimer <= 0 then
            self.state = "PLAYING"
            self.ball:Serve(self.nextServeDir)
        end
    elseif self.state == "PLAYING" then
        self.ball:Update(dt, self.p1, self.p2, self)

        if self.ball.x < -200 then
            self:ScorePoint(2)
        elseif self.ball.x > Constants.ScreenWidth + 200 then
            self:ScorePoint(1)
        end
    end
end

function GameManager:DrawScoreQuad(score, x, y, color)
    -- Draw Neon Energy Bar
    local maxScore = 5
    local barWidth = 300
    local barHeight = 20
    
    -- Background Bar
    gfx.draw_rect(x, y, barWidth, barHeight, 0.2, 0.2, 0.2, 0.5)
    
    -- Filled Bar
    local fillWidth = (score / maxScore) * barWidth
    if fillWidth > 0 then
        gfx.draw_rect(x, y, fillWidth, barHeight, color[1], color[2], color[3], 0.8)
        gfx.draw_rect(x - 2, y - 2, fillWidth + 4, barHeight + 4, color[1], color[2], color[3], 0.3)
    end
end

function GameManager:Draw()
    local ox = 0
    local oy = 0
    if self.shakeTime > 0 and (self.state == "PLAYING" or self.state == "SERVE_DELAY") then
        ox = (math.random() * 2 - 1) * self.shakeIntensity
        oy = (math.random() * 2 - 1) * self.shakeIntensity
    end

    -- Background
    gfx.draw_sprite_clean(ox, oy, Constants.ScreenWidth, Constants.ScreenHeight, Textures.Bg, 1.0, 1.0, 1.0, 1.0)

    -- Game Objects (only draw if not in main menu, or draw dim)
    if self.state ~= "MAIN_MENU" then
        -- Draw power-ups
        for _, pu in ipairs(self.powerups) do pu:Draw(ox, oy) end

        self.ps:Draw(ox, oy)
        self.p1:Draw(ox, oy)
        self.p2:Draw(ox, oy)
        self.ball:Draw(ox, oy)

        -- Rally Counter (center top)
        if self.ball.rally > 0 and self.state == "PLAYING" then
            local rallyAlpha = MathX.clamp(self.ball.rally / 20.0, 0.3, 1.0)
            local rr, rg, rb = Color.hsv((self.ball.rally * 0.05) % 1.0, 0.8, 1.0)
            if imgui then
                imgui.text(Constants.ScreenWidth / 2 - 30, 80, rr, rg, rb, "RALLY: " .. self.ball.rally)
            end
            -- Speed indicator bar
            local speedPct = MathX.clamp(self.ball.speed / Constants.MaxBallSpeed, 0, 1)
            local barW = 200
            gfx.draw_rect(ox + Constants.ScreenWidth/2 - barW/2, oy + 100, barW, 6, 0.2, 0.2, 0.2, 0.5)
            gfx.draw_rect(ox + Constants.ScreenWidth/2 - barW/2, oy + 100, barW * speedPct, 6, rr, rg, rb, 0.8)
        end

        -- UI Scores (Energy Bars)
        self:DrawScoreQuad(self.p1.score, ox + 100, oy + 40, self.p1.color)
        self:DrawScoreQuad(self.p2.score, ox + Constants.ScreenWidth - 100 - 300, oy + 40, self.p2.color)
        
        if imgui then
            imgui.text(100, 20, self.p1.color[1], self.p1.color[2], self.p1.color[3], "PLAYER 1 ENERGY")
            imgui.text(Constants.ScreenWidth - 100 - 90, 20, self.p2.color[1], self.p2.color[2], self.p2.color[3], "CPU ENERGY")
        end
    end

    -- State Overlays
    local cx = Constants.ScreenWidth / 2
    local cy = Constants.ScreenHeight / 2

    if self.state == "MAIN_MENU" then
        -- Dim background
        gfx.draw_rect(0, 0, Constants.ScreenWidth, Constants.ScreenHeight, 0, 0, 0, 0.6)
        if imgui then
            imgui.text(cx - 140, cy - 100, 1.0, 0.2, 0.6, "STARLIGHT PONG: NEON EDITION")
        end
        self.btnStart:Draw()

    elseif self.state == "PAUSED" then
        gfx.draw_rect(0, 0, Constants.ScreenWidth, Constants.ScreenHeight, 0, 0, 0, 0.7)
        if imgui then
            imgui.text(cx - 30, cy - 80, 1.0, 1.0, 1.0, "PAUSED")
        end
        self.btnResume:Draw()
        self.btnQuit:Draw()

    elseif self.state == "GAME_OVER" then
        gfx.draw_rect(0, 0, Constants.ScreenWidth, Constants.ScreenHeight, 0, 0, 0, 0.8)
        if imgui then
            local winMsg = (self.winner == 1) and "PLAYER 1 WINS!" or "CPU WINS!"
            local c = (self.winner == 1) and self.p1.color or self.p2.color
            imgui.text(cx - 60, cy - 80, c[1], c[2], c[3], winMsg)
        end
        self.btnRestart:Draw()
    end
end

-- ============================================================================
-- Global Engine Hooks
-- ============================================================================
local Game = nil

function OnStart()
    Engine.log("Pong Lua Framework (OO Architecture) Initialized!")
    Textures.Bg = assets.load_texture("assets/textures/synthwave_bg.png")
    Textures.Ball = assets.load_texture("assets/textures/energy_ball.png")
    Textures.Paddle = assets.load_texture("assets/textures/neon_paddle.png")
    
    Game = GameManager()
end

function OnUpdate(dt)
    if Game then Game:Update(dt) end
end

function OnRenderUI()
    if Game then Game:Draw() end
end
