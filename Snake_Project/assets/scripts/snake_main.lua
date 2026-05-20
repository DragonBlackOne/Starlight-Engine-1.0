-- ============================================================================
-- STARLIGHT ODYSSEY: CYBER SNAKE 2D (SBA v4.0 Industrial)
-- Uses Scene Manager, Color utilities, ScreenShake, Timer
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
    speed = 0.10,
    food = {x=10, y=10},
    alive = true,
    score = 0,
    highScore = Save.read("snake_highscore", 0),
    started = false,
    combo = 0,
    comboTimer = 0,
    foodEaten = 0
}

local InputCooldown = 0

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
    Snake.speed = 0.10
    Snake.alive = true
    Snake.score = 0
    Snake.started = false
    Snake.combo = 0
    Snake.comboTimer = 0
    Snake.foodEaten = 0
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
end

function UpdateSnake2D(dt)
    if not Snake.alive then return end
    
    -- Combo decay
    if Snake.comboTimer > 0 then
        Snake.comboTimer = Snake.comboTimer - dt
        if Snake.comboTimer <= 0 then Snake.combo = 0 end
    end
    
    InputCooldown = InputCooldown - dt
    
    if InputCooldown <= 0 then
        if input.is_down("W") or input.is_down("Up") or Input.axis("LeftY") < -0.5 then
            if Snake.dir.y ~= 1 then Snake.nextDir = {x=0, y=-1}; InputCooldown = 0.03; Snake.started = true end
        end
        if input.is_down("S") or input.is_down("Down") or Input.axis("LeftY") > 0.5 then
            if Snake.dir.y ~= -1 then Snake.nextDir = {x=0, y=1}; InputCooldown = 0.03; Snake.started = true end
        end
        if input.is_down("A") or input.is_down("Left") or Input.axis("LeftX") < -0.5 then
            if Snake.dir.x ~= 1 then Snake.nextDir = {x=-1, y=0}; InputCooldown = 0.03; Snake.started = true end
        end
        if input.is_down("D") or input.is_down("Right") or Input.axis("LeftX") > 0.5 then
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
        if Snake.score > Snake.highScore then Snake.highScore = Snake.score end
        ScreenShake.trigger(15, 0.5)
        audio.beep(100, 0.4, 4) -- Noise/Sawtooth fail
        return
    end
    
    -- Self collision
    for i = 1, #Snake.body do
        if Snake.body[i].x == newX and Snake.body[i].y == newY then
            Snake.alive = false
            if Snake.score > Snake.highScore then Snake.highScore = Snake.score end
            ScreenShake.trigger(15, 0.5)
            audio.beep(100, 0.4, 4)
            return
        end
    end
    
    local ate = (newX == Snake.food.x and newY == Snake.food.y)
    table.insert(Snake.body, 1, {x = newX, y = newY})
    
    if ate then
        Snake.foodEaten = Snake.foodEaten + 1
        Snake.combo = Snake.combo + 1
        Snake.comboTimer = 3.0 -- 3 second combo window
        
        local points = 10 * Snake.combo -- Combo multiplier!
        Snake.score = Snake.score + points
        
        if Snake.speed > 0.05 then Snake.speed = Snake.speed - 0.002 end
        PlaceFood()
        ScreenShake.trigger(4, 0.1)
        audio.beep(600 + Snake.combo * 50, 0.08, 0) -- Square wave eat
        
        if Snake.score > Snake.highScore then
            Snake.highScore = Snake.score
            Save.write("snake_highscore", Snake.highScore)
            Save.flush()
        end
    else
        table.remove(Snake.body)
    end
end

-- ==========================================
-- DRAWING WITH ENGINE Renderer2D + Color utils
-- ==========================================
function DrawGrid(ox, oy_off)
    gfx.draw_rect(ox + OFFSET_X - 3, oy_off + OFFSET_Y - 3, COLS * CELL + 6, ROWS * CELL + 6, 0.15, 0.25, 0.15)
    gfx.draw_rect(ox + OFFSET_X, oy_off + OFFSET_Y, COLS * CELL, ROWS * CELL, 0.03, 0.05, 0.03)
    
    for c = 1, COLS - 1 do
        for r = 1, ROWS - 1 do
            local px = ox + OFFSET_X + c * CELL
            local py = oy_off + OFFSET_Y + r * CELL
            gfx.draw_rect(px, py, 1, 1, 0.08, 0.12, 0.08)
        end
    end
end

function DrawSnake(ox, oy_off)
    for i, seg in ipairs(Snake.body) do
        local px = ox + OFFSET_X + seg.x * CELL + 1
        local py = oy_off + OFFSET_Y + seg.y * CELL + 1
        local s = CELL - 2
        
        if i == 1 then
            if Snake.alive then
                -- Head color changes with score (HSV rainbow)
                local hr, hg, hb = Color.hsv((Snake.foodEaten * 0.05) % 1.0, 0.8, 1.0)
                gfx.draw_rect(px, py, s, s, hr, hg, hb)
                gfx.draw_rect(px + 4, py + 4, s - 8, s - 8, hr*1.3, hg*1.3, hb*1.3)
            else
                gfx.draw_rect(px, py, s, s, 1.0, 0.2, 0.2)
            end
        else
            local fade = 1.0 - (i / (#Snake.body + 1)) * 0.5
            local br, bg, bb = Color.hsv(((Snake.foodEaten * 0.05) + i * 0.02) % 1.0, 0.6, 0.7 * fade)
            gfx.draw_rect(px, py, s, s, br, bg, bb)
            gfx.draw_rect(px + 2, py + 2, s - 4, s - 4, br*1.2, bg*1.2, bb*1.2)
        end
    end
end

function DrawFood(ox, oy_off)
    local px = ox + OFFSET_X + Snake.food.x * CELL
    local py = oy_off + OFFSET_Y + Snake.food.y * CELL
    local pulse = math.sin(State.Time * 6) * 0.12 + 0.88
    local pad = math.floor((1.0 - pulse) * CELL * 0.3)
    
    -- Food color cycles
    local fr, fg, fb = Color.hsv((State.Time * 0.2) % 1.0, 0.9, 1.0)
    gfx.draw_rect(px + pad - 2, py + pad - 2, CELL - pad*2 + 4, CELL - pad*2 + 4, fr*0.4, fg*0.4, fb*0.4)
    gfx.draw_rect(px + pad, py + pad, CELL - pad*2, CELL - pad*2, fr, fg, fb)
    gfx.draw_rect(px + pad + 4, py + pad + 4, CELL - pad*2 - 8, CELL - pad*2 - 8, 1.0, 1.0, 1.0)
end

-- ==========================================
-- INIT
-- ==========================================
local Mode = "HUB"

function OnUpdate(dt)
    State.Time = State.Time + dt
    ScreenShake.update(dt)
    
    if Mode == "HUB" then
        if input.is_just_pressed("1") then
            Mode = "SNAKE"
            ResetSnake()
        end
    elseif Mode == "SNAKE" then
        UpdateSnake2D(dt)
        if not Snake.alive and input.is_just_pressed("R") then
            ResetSnake()
        end
        if input.is_just_pressed("Escape") then
            Mode = "HUB"
            audio.beep(400, 0.05, 3)
        end
    end
end

function OnRenderUI()
    local dt = time.get_dt()
    local fps = 60
    if dt > 0 then fps = math.floor(1.0 / dt) end
    local ox, oy = ScreenShake.getOffset()
    
    if Mode == "HUB" then
        gfx.draw_rect(0, 0, 1280, 720, 0.02, 0.03, 0.05)
        
        -- Title with gradient bars
        local tr, tg, tb = Color.hsv((State.Time * 0.1) % 1.0, 0.7, 1.0)
        gfx.draw_rect(350, 160, 580, 4, tr, tg, tb)
        gfx.draw_rect(350, 270, 580, 2, tr*0.5, tg*0.5, tb*0.5)
        
        gfx.draw_rect(420, 295, 440, 45, 0.05, 0.1, 0.05)
        gfx.draw_rect(422, 297, 436, 41, 0.03, 0.06, 0.03)
        
        gfx.draw_rect(420, 440, 440, 2, 0.06, 0.1, 0.06)
        
        gfx.draw_text("STARLIGHT ENGINE", 440, 180, 2.0, tr, tg, tb, 1.0)
        gfx.draw_text("Odyssey Tech Demo v14 (SBA v4.0 Industrial)", 460, 215, 1.0, 0.4, 0.5, 0.6, 1.0)
        Engine.set_bloom(1.0, 8)
        gfx.draw_text("[1]  CYBER SNAKE 2D", 460, 305, 1.2, 0.2, 1.0, 0.4, 1.0)
        gfx.draw_text("More games coming soon...", 460, 360, 1.0, 0.2, 0.2, 0.3, 1.0)
        
        gfx.draw_text("SBA v3.0: Procedural Audio, 2D Text", 440, 460, 1.0, 0.12, 0.18, 0.22, 1.0)
        gfx.draw_text("Modular Systems + RenderGraph", 440, 480, 1.0, 0.12, 0.18, 0.22, 1.0)
        gfx.draw_text("Jolt Physics + Hot-Reload + ECS", 440, 500, 1.0, 0.12, 0.18, 0.22, 1.0)
        
    elseif Mode == "SNAKE" then
        gfx.draw_rect(0, 0, 1280, 720, 0.01, 0.02, 0.01)
        
        DrawGrid(ox, oy)
        DrawFood(ox, oy)
        DrawSnake(ox, oy)
        
        local rightX = OFFSET_X + COLS * CELL + 20
        gfx.draw_rect(rightX - 5, OFFSET_Y - 5, 210, 340, 0.04, 0.06, 0.04)
        gfx.draw_rect(rightX - 3, OFFSET_Y - 3, 206, 336, 0.02, 0.03, 0.02)
        
        gfx.draw_text("CYBER SNAKE 2D", OFFSET_X, 20, 1.5, 0.2, 1.0, 0.4, 1.0)
        gfx.draw_text("Powered by SBA v3.0", OFFSET_X, 48, 1.0, 0.3, 0.4, 0.35, 1.0)
        
        gfx.draw_text("Score: " .. Snake.score, rightX, OFFSET_Y + 5, 1.0, 1, 1, 1, 1.0)
        gfx.draw_text("High Score", rightX, OFFSET_Y + 35, 1.0, 0.5, 0.5, 0.6, 1.0)
        gfx.draw_text("" .. Snake.highScore, rightX, OFFSET_Y + 55, 1.2, 1, 0.85, 0.3, 1.0)
        gfx.draw_text("Length: " .. #Snake.body, rightX, OFFSET_Y + 95, 1.0, 0.3, 0.3, 0.4, 1.0)
        gfx.draw_text(string.format("Speed: %.0f%%", (0.10 / Snake.speed) * 100), rightX, OFFSET_Y + 115, 1.0, 0.3, 0.3, 0.4, 1.0)
        
        -- Combo display
        if Snake.combo > 1 and Snake.comboTimer > 0 then
            local cr, cg, cb = Color.hsv((State.Time * 0.5) % 1.0, 0.8, 1.0)
            gfx.draw_text("COMBO x" .. Snake.combo, rightX, OFFSET_Y + 155, 1.2, cr, cg, cb, 1.0)
        end
        
        gfx.draw_text("Controls:", rightX, OFFSET_Y + 195, 1.0, 0.3, 0.3, 0.4, 1.0)
        gfx.draw_text("WASD / Arrows", rightX, OFFSET_Y + 215, 1.0, 0.4, 0.7, 0.4, 1.0)
        gfx.draw_text("ESC quit | R restart", rightX, OFFSET_Y + 235, 1.0, 0.3, 0.3, 0.4, 1.0)
        
        if not Snake.started and Snake.alive then
            gfx.draw_rect(OFFSET_X + 140, OFFSET_Y + 230, 420, 40, 0.04, 0.08, 0.04)
            gfx.draw_text("Press a direction to start!", OFFSET_X + 185, OFFSET_Y + 240, 1.0, 0.4, 1.0, 0.5, 1.0)
        end
        
        if not Snake.alive then
            gfx.draw_rect(OFFSET_X + 180, OFFSET_Y + 200, 340, 130, 0.15, 0.03, 0.03)
            gfx.draw_rect(OFFSET_X + 183, OFFSET_Y + 203, 334, 124, 0.08, 0.02, 0.02)
            gfx.draw_text("GAME OVER", OFFSET_X + 280, OFFSET_Y + 220, 2.0, 1.0, 0.2, 0.2, 1.0)
            gfx.draw_text("Final Score: " .. Snake.score, OFFSET_X + 260, OFFSET_Y + 255, 1.0, 1, 1, 1, 1.0)
            if Snake.combo > 1 then
                gfx.draw_text("Best Combo: x" .. Snake.combo, OFFSET_X + 250, OFFSET_Y + 275, 1.0, 1, 0.8, 0, 1.0)
            end
            gfx.draw_text("Press [R] to play again", OFFSET_X + 250, OFFSET_Y + 295, 1.0, 0.4, 1, 0.5, 1.0)
        end
    end
    
    gfx.draw_text("FPS: " .. fps, 1180, 8, 1.0, 0.4, 0.4, 0.4, 1.0)
end
