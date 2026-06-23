-- ============================================================================
-- STARLIGHT ODYSSEY: CYBER SNAKE 2D (SBA v4.0 Industrial)
-- Uses Scene Manager, Color utilities, ScreenShake, Timer, Particles
-- ============================================================================

local State = { Time = 0 }



-- Snake 2D game state
local CELL = 28
local COLS = 25
local ROWS = 20
local OFFSET_X = 140
local OFFSET_Y = 80

local Snake = {
    body = {},
    dir = {x=1, y=0},
    nextDir = {x=1, y=0},
    timer = 0,
    speed = 0.11,
    food = {x=10, y=10, type="NORMAL"},
    alive = true,
    score = 0,
    highScore = Save.read("snake_highscore", 0),
    started = false,
    combo = 0,
    comboTimer = 0,
    foodEaten = 0,
    speedLevel = 1,
    deathTimer = 0,
    growSegments = 0,
    speedUpTimer = 0.0,
    goldTimer = 0.0,
    toxicTimer = 0.0
}

local InputCooldown = 0
local Particles = {}
local Leaderboard = {}

-- Load Leaderboard
function LoadLeaderboard()
    Leaderboard = Save.read("snake_leaderboard", {0, 0, 0, 0, 0})
    table.sort(Leaderboard, function(a, b) return a > b end)
end

-- Update and Save Leaderboard
function CheckLeaderboardScore(score)
    table.insert(Leaderboard, score)
    table.sort(Leaderboard, function(a, b) return a > b end)
    while #Leaderboard > 5 do
        table.remove(Leaderboard)
    end
    Save.write("snake_leaderboard", Leaderboard)
    Save.flush()
end

-- Button class from core.lua (global Button)

function SpawnDeathParticles(x, y)
    for i = 1, 30 do
        local angle = math.random() * math.pi * 2
        local speed = math.random(100, 500)
        table.insert(Particles, {
            x = x,
            y = y,
            vx = math.cos(angle) * speed,
            vy = math.sin(angle) * speed,
            life = 1.0,
            color = {Color.hsv(math.random(), 0.8, 1.0)},
            size = math.random(6, 14)
        })
    end
end

function SpawnFoodParticles(x, y, r, g, b)
    for i = 1, 15 do
        local angle = math.random() * math.pi * 2
        local speed = math.random(80, 300)
        table.insert(Particles, {
            x = x,
            y = y,
            vx = math.cos(angle) * speed,
            vy = math.sin(angle) * speed,
            life = 0.8,
            color = {r, g, b},
            size = math.random(4, 10)
        })
    end
end

function UpdateParticles(dt)
    for i = #Particles, 1, -1 do
        local p = Particles[i]
        p.x = p.x + p.vx * dt
        p.y = p.y + p.vy * dt
        p.life = p.life - dt * 1.5
        if p.life <= 0 then
            table.remove(Particles, i)
        end
    end
end

function DrawParticles()
    for _, p in ipairs(Particles) do
        local c = p.color
        gfx.draw_rect(p.x - p.size/2, p.y - p.size/2, p.size, p.size, c[1], c[2], c[3], p.life)
    end
end

function ResetSnake()
    Snake.body = {}
    local cx = math.floor(COLS / 2)
    local cy = math.floor(ROWS / 2)
    for i = 0, 3 do
        table.insert(Snake.body, {x = cx - i, y = cy})
    end
    Snake.dir = {x=1, y=0}
    Snake.nextDir = {x=1, y=0}
    Snake.timer = 0
    Snake.speed = 0.12
    Snake.alive = true
    Snake.score = 0
    Snake.started = false
    Snake.combo = 0
    Snake.comboTimer = 0
    Snake.foodEaten = 0
    Snake.speedLevel = 1
    Snake.deathTimer = 0
    Snake.growSegments = 0
    Snake.speedUpTimer = 0.0
    Snake.goldTimer = 0.0
    Snake.toxicTimer = 0.0
    Particles = {}
    LoadLeaderboard()
    PlaceFood()
end

function PlaceFood()
    local valid = false
    while not valid do
        Snake.food.x = math.random(0, COLS - 1)
        Snake.food.y = math.random(0, ROWS - 1)
        valid = true
        for _, seg in ipairs(Snake.body) do
            if seg.x == Snake.food.x and seg.y == Snake.food.y then
                valid = false
                break
            end
        end
    end

    -- 15% Gold, 8% Toxic, 77% Normal
    local r = math.random()
    if r < 0.15 then
        Snake.food.type = "GOLD"
    elseif r < 0.23 then
        Snake.food.type = "TOXIC"
    else
        Snake.food.type = "NORMAL"
    end
end

function UpdateSnake2D(dt)
    if not Snake.alive then return end
    
    -- Notification Timers
    if Snake.speedUpTimer > 0 then Snake.speedUpTimer = Snake.speedUpTimer - dt end
    if Snake.goldTimer > 0 then Snake.goldTimer = Snake.goldTimer - dt end
    if Snake.toxicTimer > 0 then Snake.toxicTimer = Snake.toxicTimer - dt end

    -- Combo decay
    if Snake.comboTimer > 0 then
        Snake.comboTimer = Snake.comboTimer - dt
        if Snake.comboTimer <= 0 then Snake.combo = 0 end
    end
    
    InputCooldown = InputCooldown - dt
    
    if InputCooldown <= 0 then
        local vy = Input.getAxis("Vertical")
        local vx = Input.getAxis("Horizontal")
        
        if vy < -0.5 then
            if Snake.dir.y ~= 1 then Snake.nextDir = {x=0, y=-1}; InputCooldown = 0.03; Snake.started = true end
        elseif vy > 0.5 then
            if Snake.dir.y ~= -1 then Snake.nextDir = {x=0, y=1}; InputCooldown = 0.03; Snake.started = true end
        elseif vx < -0.5 then
            if Snake.dir.x ~= 1 then Snake.nextDir = {x=-1, y=0}; InputCooldown = 0.03; Snake.started = true end
        elseif vx > 0.5 then
            if Snake.dir.x ~= -1 then Snake.nextDir = {x=1, y=0}; InputCooldown = 0.03; Snake.started = true end
        end
    end
    
    if not Snake.started then return end
    
    Snake.timer = Snake.timer + dt
    if Snake.timer < Snake.speed then return end
    Snake.timer = 0
    Snake.dir = Snake.nextDir
    
    local head = Snake.body[1]
    local newX = head.x + Snake.dir.x
    local newY = head.y + Snake.dir.y
    
    -- Wall collision
    if newX < 0 or newX >= COLS or newY < 0 or newY >= ROWS then
        Snake.alive = false
        Snake.deathTimer = 1.5
        CheckLeaderboardScore(Snake.score)
        if Snake.score > Snake.highScore then Snake.highScore = Snake.score end
        ScreenShake.trigger(20, 0.6)
        Audio.playExplosion()
        SpawnDeathParticles(OFFSET_X + head.x * CELL + CELL/2, OFFSET_Y + head.y * CELL + CELL/2)
        return
    end
    
    -- Self collision
    for i = 1, #Snake.body do
        if Snake.body[i].x == newX and Snake.body[i].y == newY then
            Snake.alive = false
            Snake.deathTimer = 1.5
            CheckLeaderboardScore(Snake.score)
            if Snake.score > Snake.highScore then Snake.highScore = Snake.score end
            ScreenShake.trigger(20, 0.6)
            Audio.playExplosion()
            SpawnDeathParticles(OFFSET_X + head.x * CELL + CELL/2, OFFSET_Y + head.y * CELL + CELL/2)
            return
        end
    end
    
    local ate = (newX == Snake.food.x and newY == Snake.food.y)
    table.insert(Snake.body, 1, {x = newX, y = newY})
    
    if ate then
        local fx = OFFSET_X + Snake.food.x * CELL + CELL/2
        local fy = OFFSET_Y + Snake.food.y * CELL + CELL/2

        if Snake.food.type == "NORMAL" then
            Snake.foodEaten = Snake.foodEaten + 1
            Snake.combo = Snake.combo + 1
            Snake.comboTimer = 3.0
            
            local points = 10 * Snake.combo
            Snake.score = Snake.score + points
            
            Audio.playCoin()
            SpawnFoodParticles(fx, fy, 0.2, 1.0, 0.4)
            
            -- Speed Up Progression every 5 normal foods
            if Snake.foodEaten % 5 == 0 then
                Snake.speedLevel = Snake.speedLevel + 1
                Snake.speed = math.max(0.045, Snake.speed - 0.010)
                Snake.speedUpTimer = 1.5
                Audio.playPowerup()
            end

        elseif Snake.food.type == "GOLD" then
            Snake.foodEaten = Snake.foodEaten + 1
            Snake.combo = Snake.combo + 2
            Snake.comboTimer = 4.0
            
            local points = 30 * Snake.combo
            Snake.score = Snake.score + points
            Snake.growSegments = Snake.growSegments + 2 -- Grow extra segments!
            
            Snake.goldTimer = 1.5
            Audio.playPowerup()
            SpawnFoodParticles(fx, fy, 1.0, 0.85, 0.2)
            
            if Snake.foodEaten % 5 == 0 then
                Snake.speedLevel = Snake.speedLevel + 1
                Snake.speed = math.max(0.045, Snake.speed - 0.010)
                Snake.speedUpTimer = 1.5
            end

        elseif Snake.food.type == "TOXIC" then
            Snake.combo = 0
            Snake.comboTimer = 0.0
            
            Snake.score = math.max(0, Snake.score - 20)
            Snake.toxicTimer = 1.5
            Audio.playHurt()
            SpawnFoodParticles(fx, fy, 0.8, 0.1, 0.9)
            
            -- Shrink segment (minimum length 3)
            if #Snake.body > 3 then
                table.remove(Snake.body)
            end
            -- Since we inserted the head and didn't remove the tail to simulate normal growth, 
            -- to actually shrink, we remove another segment from the end!
            if #Snake.body > 3 then
                table.remove(Snake.body)
            end
        end

        PlaceFood()
        ScreenShake.trigger(4, 0.1)
        
        if Snake.score > Snake.highScore then
            Snake.highScore = Snake.score
            Save.write("snake_highscore", Snake.highScore)
            Save.flush()
        end
    else
        if Snake.growSegments > 0 then
            Snake.growSegments = Snake.growSegments - 1
        else
            table.remove(Snake.body)
        end
    end
end

-- ==========================================
-- DRAWING WITH ENGINE Renderer2D + Color utils
-- ==========================================
function DrawGrid(ox, oy_off)
    gfx.draw_rect(ox + OFFSET_X - 3, oy_off + OFFSET_Y - 3, COLS * CELL + 6, ROWS * CELL + 6, 0.15, 0.25, 0.15, 1.0)
    gfx.draw_rect(ox + OFFSET_X, oy_off + OFFSET_Y, COLS * CELL, ROWS * CELL, 0.03, 0.05, 0.03, 1.0)
    
    for c = 1, COLS - 1 do
        for r = 1, ROWS - 1 do
            local px = ox + OFFSET_X + c * CELL
            local py = oy_off + OFFSET_Y + r * CELL
            gfx.draw_rect(px, py, 1, 1, 0.08, 0.12, 0.08, 1.0)
        end
    end
end

function DrawSnake(ox, oy_off)
    if not Snake.alive and Snake.deathTimer > 0 then
        local alpha = math.max(0.0, Snake.deathTimer / 1.5)
        for i, seg in ipairs(Snake.body) do
            local px = ox + OFFSET_X + seg.x * CELL + 1
            local py = oy_off + OFFSET_Y + seg.y * CELL + 1
            local s = CELL - 2
            gfx.draw_rect(px, py, s, s, 1.0, 0.2, 0.2, alpha)
        end
        return
    end

    for i, seg in ipairs(Snake.body) do
        local px = ox + OFFSET_X + seg.x * CELL + 1
        local py = oy_off + OFFSET_Y + seg.y * CELL + 1
        local s = CELL - 2
        
        if i == 1 then
            if Snake.alive then
                local hr, hg, hb = Color.hsv((Snake.foodEaten * 0.05) % 1.0, 0.8, 1.0)
                gfx.draw_rect(px, py, s, s, hr, hg, hb, 1.0)
                gfx.draw_rect(px + 4, py + 4, s - 8, s - 8, hr*1.3, hg*1.3, hb*1.3, 1.0)
            else
                gfx.draw_rect(px, py, s, s, 1.0, 0.2, 0.2, 1.0)
            end
        else
            local fade = 1.0 - (i / (#Snake.body + 1)) * 0.5
            local br, bg, bb = Color.hsv(((Snake.foodEaten * 0.05) + i * 0.02) % 1.0, 0.6, 0.7 * fade)
            gfx.draw_rect(px, py, s, s, br, bg, bb, 1.0)
            gfx.draw_rect(px + 2, py + 2, s - 4, s - 4, br*1.2, bg*1.2, bb*1.2, 1.0)
        end
    end
end

function DrawFood(ox, oy_off)
    if not Snake.alive then return end
    local px = ox + OFFSET_X + Snake.food.x * CELL
    local py = oy_off + OFFSET_Y + Snake.food.y * CELL
    local pulse = math.sin(State.Time * 6) * 0.12 + 0.88
    local pad = math.floor((1.0 - pulse) * CELL * 0.3)
    
    local fr, fg, fb = 0.2, 1.0, 0.4
    if Snake.food.type == "GOLD" then
        fr, fg, fb = Color.hsv((State.Time * 0.3) % 1.0, 0.9, 1.0)
    elseif Snake.food.type == "TOXIC" then
        fr, fg, fb = 0.8, 0.1, 0.9
    end

    gfx.draw_rect(px + pad - 2, py + pad - 2, CELL - pad*2 + 4, CELL - pad*2 + 4, fr*0.4, fg*0.4, fb*0.4, 1.0)
    gfx.draw_rect(px + pad, py + pad, CELL - pad*2, CELL - pad*2, fr, fg, fb, 1.0)
    gfx.draw_rect(px + pad + 4, py + pad + 4, CELL - pad*2 - 8, CELL - pad*2 - 8, 1.0, 1.0, 1.0, 1.0)
end

-- ==========================================
-- INIT / RUN
-- ==========================================
local Mode = "MAIN_MENU"
local btnPlay = Button("START GAME", 540, 360, 200, 50, {0.2, 1.0, 0.4})
local btnQuit = Button("QUIT GAME", 540, 430, 200, 50, {1.0, 0.2, 0.4})

local btnResume = Button("RESUME", 540, 300, 200, 50, {0.2, 0.8, 1.0})
local btnMenu = Button("MAIN MENU", 540, 370, 200, 50, {1.0, 0.2, 0.6})

function OnUpdate(dt)
    State.Time = State.Time + dt
    ScreenShake.update(dt)
    
    if Mode == "MAIN_MENU" then
        if btnPlay:Update() or input.is_just_pressed("Enter") or input.is_just_pressed("Return") or input.is_just_pressed("Space") then
            Mode = "PLAYING"
            ResetSnake()
            Audio.playCoin()
        elseif btnQuit:Update() then
            Engine.quit()
        end
    elseif Mode == "PLAYING" then
        if Snake.alive then
            UpdateSnake2D(dt)
            if input.is_just_pressed("Escape") then
                Mode = "PAUSED"
                Audio.setADSR(0.002, 0.05, 0.1, 0.05)
                Audio.fm(400, 0.05, 3)
            end
        else
            UpdateParticles(dt)
            if Snake.deathTimer > 0 then
                Snake.deathTimer = Snake.deathTimer - dt
            else
                if input.is_just_pressed("R") or input.is_just_pressed("Space") or input.is_just_pressed("Enter") then
                    ResetSnake()
                elseif input.is_just_pressed("Escape") then
                    Mode = "MAIN_MENU"
                    LoadLeaderboard()
                end
            end
        end
    elseif Mode == "PAUSED" then
        if btnResume:Update() or input.is_just_pressed("Escape") then
            Mode = "PLAYING"
            Audio.setADSR(0.002, 0.05, 0.1, 0.05)
            Audio.fm(600, 0.05, 2)
        elseif btnMenu:Update() or input.is_just_pressed("Q") then
            Mode = "MAIN_MENU"
            LoadLeaderboard()
            Audio.setADSR(0.002, 0.05, 0.1, 0.05)
            Audio.fm(300, 0.05, 1)
        end
    end
end

function OnRenderUI()
    local dt = time.get_dt()
    local fps = 60
    if dt > 0 then fps = math.floor(1.0 / dt) end
    local ox, oy = ScreenShake.getOffset()
    
    if Mode == "MAIN_MENU" then
        gfx.draw_rect(0, 0, 1280, 720, 0.02, 0.03, 0.05, 1.0)
        
        local tr, tg, tb = Color.hsv((State.Time * 0.15) % 1.0, 0.8, 1.0)
        gfx.draw_rect(300, 110, 680, 6, tr, tg, tb, 1.0)
        gfx.draw_rect(300, 230, 680, 2, tr*0.5, tg*0.5, tb*0.5, 0.5)
        
        gfx.draw_text("CYBER SNAKE 2D", 460, 140, 2.5, tr, tg, tb, 1.0)
        gfx.draw_text("STARLIGHT NEON EDITION // SBA v4.0 INDUSTRIAL", 380, 190, 1.1, 0.4, 0.5, 0.6, 1.0)
        
        Engine.set_bloom(1.2, 10)
        
        btnPlay:Draw()
        btnQuit:Draw()
        
        gfx.draw_text("HIGH SCORE: " .. Snake.highScore, 540, 500, 1.2, 1.0, 0.85, 0.3, 1.0)
        
        -- Draw Local Leaderboard
        gfx.draw_rect(60, 310, 220, 220, 0.04, 0.06, 0.04, 0.6)
        gfx.draw_rect_outline(60, 310, 220, 220, 2, 0.2, 0.8, 0.4, 0.4)
        gfx.draw_text("LOCAL LEADERBOARD", 75, 330, 1.0, 0.2, 1.0, 0.4, 1.0)
        for i = 1, 5 do
            local lScore = Leaderboard[i] or 0
            gfx.draw_text(string.format("%d. %05d PTS", i, lScore), 75, 350 + i * 25, 0.9, 0.8, 0.8, 0.9, 0.85)
        end

        gfx.draw_text("Controls: WASD / Arrow Keys to Move | ESC to Pause", 420, 620, 0.9, 0.3, 0.4, 0.5, 1.0)
        gfx.draw_text("Press ENTER or SPACE to Quick Start", 480, 645, 0.9, 0.2, 0.8, 0.4, 0.7)
        
    elseif Mode == "PLAYING" then
        gfx.draw_rect(0, 0, 1280, 720, 0.01, 0.02, 0.01, 1.0)
        
        DrawGrid(ox, oy)
        DrawFood(ox, oy)
        DrawSnake(ox, oy)
        DrawParticles()
        
        local rightX = OFFSET_X + COLS * CELL + 20
        gfx.draw_rect(rightX - 5, OFFSET_Y - 5, 210, 370, 0.04, 0.06, 0.04, 1.0)
        gfx.draw_rect(rightX - 3, OFFSET_Y - 3, 206, 366, 0.02, 0.03, 0.02, 1.0)
        
        gfx.draw_text("CYBER SNAKE 2D", OFFSET_X, 20, 1.5, 0.2, 1.0, 0.4, 1.0)
        gfx.draw_text("Powered by SBA v4.0 Industrial Engine", OFFSET_X, 48, 1.0, 0.3, 0.4, 0.35, 1.0)
        
        gfx.draw_text("Score: " .. Snake.score, rightX, OFFSET_Y + 15, 1.1, 1, 1, 1, 1.0)
        gfx.draw_text("High Score", rightX, OFFSET_Y + 45, 1.0, 0.5, 0.5, 0.6, 1.0)
        gfx.draw_text("" .. Snake.highScore, rightX, OFFSET_Y + 65, 1.3, 1, 0.85, 0.3, 1.0)
        gfx.draw_text("Length: " .. #Snake.body, rightX, OFFSET_Y + 105, 1.0, 0.3, 0.5, 0.4, 1.0)
        gfx.draw_text("Speed Lvl: " .. Snake.speedLevel, rightX, OFFSET_Y + 125, 1.0, 0.3, 0.5, 0.4, 1.0)
        
        -- Combo display
        if Snake.combo > 1 and Snake.comboTimer > 0 then
            local cr, cg, cb = Color.hsv((State.Time * 0.5) % 1.0, 0.8, 1.0)
            gfx.draw_text("COMBO x" .. Snake.combo, rightX, OFFSET_Y + 160, 1.3, cr, cg, cb, 1.0)
            local barW = 160
            gfx.draw_rect(rightX, OFFSET_Y + 190, barW, 6, 0.2, 0.2, 0.2, 0.5)
            gfx.draw_rect(rightX, OFFSET_Y + 190, barW * (Snake.comboTimer / 3.0), 6, cr, cg, cb, 0.8)
        end
        
        -- Notifications inside HUD panel
        if Snake.speedUpTimer and Snake.speedUpTimer > 0 then
            gfx.draw_text("SPEED UP!", rightX, OFFSET_Y + 215, 1.1, 0.2, 1.0, 0.4, Snake.speedUpTimer)
        elseif Snake.goldTimer and Snake.goldTimer > 0 then
            gfx.draw_text("GOLD CHIME!!", rightX, OFFSET_Y + 215, 1.1, 1.0, 0.8, 0.2, Snake.goldTimer)
        elseif Snake.toxicTimer and Snake.toxicTimer > 0 then
            gfx.draw_text("SHRINK POISON!", rightX, OFFSET_Y + 215, 1.1, 0.8, 0.1, 0.9, Snake.toxicTimer)
        end

        gfx.draw_text("Controls:", rightX, OFFSET_Y + 255, 1.0, 0.4, 0.4, 0.5, 1.0)
        gfx.draw_text("WASD / Arrows", rightX, OFFSET_Y + 275, 1.0, 0.4, 0.7, 0.4, 1.0)
        gfx.draw_text("ESC to Pause", rightX, OFFSET_Y + 295, 1.0, 0.4, 0.4, 0.5, 1.0)
        
        if not Snake.started and Snake.alive then
            gfx.draw_rect(OFFSET_X + 140, OFFSET_Y + 230, 420, 40, 0.04, 0.08, 0.04, 0.8)
            gfx.draw_text("Press a direction to start!", OFFSET_X + 185, OFFSET_Y + 240, 1.0, 0.4, 1.0, 0.5, 1.0)
        end
        
        if not Snake.alive and Snake.deathTimer <= 0 then
            gfx.draw_rect(OFFSET_X + 180, OFFSET_Y + 140, 340, 240, 0.15, 0.03, 0.03, 0.9)
            gfx.draw_rect(OFFSET_X + 183, OFFSET_Y + 143, 334, 234, 0.08, 0.02, 0.02, 0.9)
            
            gfx.draw_text("GAME OVER", OFFSET_X + 280, OFFSET_Y + 160, 2.0, 1.0, 0.2, 0.2, 1.0)
            gfx.draw_text("Final Score: " .. Snake.score, OFFSET_X + 260, OFFSET_Y + 195, 1.0, 1, 1, 1, 1.0)
            if Snake.combo > 1 then
                gfx.draw_text("Best Combo: x" .. Snake.combo, OFFSET_X + 250, OFFSET_Y + 215, 1.0, 1, 0.8, 0, 1.0)
            end
            
            -- Leaderboard on Game Over
            gfx.draw_text("TOP LOCAL SCORES:", OFFSET_X + 200, OFFSET_Y + 245, 0.95, 0.2, 1.0, 0.4, 0.8)
            for i = 1, 3 do
                local lScore = Leaderboard[i] or 0
                gfx.draw_text(string.format("%d. %05d PTS", i, lScore), OFFSET_X + 200, OFFSET_Y + 250 + i * 20, 0.85, 0.8, 0.8, 0.8, 0.75)
            end

            gfx.draw_text("Press [SPACE] / [ENTER] to Restart", OFFSET_X + 200, OFFSET_Y + 340, 0.9, 0.4, 1, 0.5, 1.0)
            gfx.draw_text("Press [ESC] for Main Menu", OFFSET_X + 235, OFFSET_Y + 360, 0.9, 0.5, 0.5, 0.6, 0.8)
        end
    elseif Mode == "PAUSED" then
        gfx.draw_rect(0, 0, 1280, 720, 0.01, 0.01, 0.02, 0.7)
        
        local pr, pg, pb = Color.hsv((State.Time * 0.1) % 1.0, 0.7, 1.0)
        gfx.draw_rect(440, 200, 400, 4, pr, pg, pb, 1.0)
        gfx.draw_text("GAME PAUSED", 490, 220, 2.0, pr, pg, pb, 1.0)
        
        btnResume:Draw()
        btnMenu:Draw()
        
        gfx.draw_text("Press ESC to Resume or Q to Quit", 470, 460, 0.9, 0.5, 0.5, 0.5, 0.8)
    end
    
    gfx.draw_text("FPS: " .. fps, 1180, 8, 1.0, 0.4, 0.4, 0.4, 1.0)
end

-- Initialize leaderboard and inputs on startup
LoadLeaderboard()

Input.map("Up", "W", "Up")
Input.map("Down", "S", "Down")
Input.map("Left", "A", "Left")
Input.map("Right", "D", "Right")

Input.mapAxis("Vertical", "Up", "Down", "LeftY")
Input.mapAxis("Horizontal", "Left", "Right", "LeftX")
