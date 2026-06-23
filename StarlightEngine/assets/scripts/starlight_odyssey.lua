-- ============================================================================
-- STARLIGHT ODYSSEY: CYBER SNAKE 2D (SBA v2.0)
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
    highScore = 0,
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
    if Engine and Engine.report_active_bt_node then
        Engine.report_active_bt_node("SnakeRoot (Selector)")
        Engine.report_active_bt_node("GameLoop (Sequence)")
    end

    if not Snake.alive then return end
    
    -- Combo decay
    if Snake.comboTimer > 0 then
        Snake.comboTimer = Snake.comboTimer - dt
        if Snake.comboTimer <= 0 then Snake.combo = 0 end
    end
    
    InputCooldown = InputCooldown - dt
    
    if InputCooldown <= 0 then
        if Engine and Engine.report_active_bt_node then
            Engine.report_active_bt_node("CheckInput (Sequence)")
        end
        
        local moved = false
        if input.is_down("W") or input.is_down("Up") then
            if Snake.dir.y ~= 1 then 
                Snake.nextDir = {x=0, y=-1}; InputCooldown = 0.03; Snake.started = true; moved = true
            end
        elseif input.is_down("S") or input.is_down("Down") then
            if Snake.dir.y ~= -1 then 
                Snake.nextDir = {x=0, y=1}; InputCooldown = 0.03; Snake.started = true; moved = true
            end
        elseif input.is_down("A") or input.is_down("Left") then
            if Snake.dir.x ~= 1 then 
                Snake.nextDir = {x=-1, y=0}; InputCooldown = 0.03; Snake.started = true; moved = true
            end
        elseif input.is_down("D") or input.is_down("Right") then
            if Snake.dir.x ~= -1 then 
                Snake.nextDir = {x=1, y=0}; InputCooldown = 0.03; Snake.started = true; moved = true
            end
        end
        
        if moved and Engine and Engine.report_active_bt_node then
            Engine.report_active_bt_node("AnyKeyPressed?")
            Engine.report_active_bt_node("UpdateNextDir")
        end
    end
    
    if not Snake.started then return end
    
    if Engine and Engine.report_active_bt_node then
        Engine.report_active_bt_node("CheckTimer (Sequence)")
    end
    
    Snake.timer = Snake.timer + dt
    if Snake.timer < Snake.speed then return end
    
    if Engine and Engine.report_active_bt_node then
        Engine.report_active_bt_node("IsTimeStepElapsed?")
        Engine.report_active_bt_node("MoveSnakeHead")
    end
    
    Snake.timer = 0
    Snake.dir = Snake.nextDir
    
    local head = Snake.body[1]
    local newX = head.x + Snake.dir.x
    local newY = head.y + Snake.dir.y
    
    if Engine and Engine.report_active_bt_node then
        Engine.report_active_bt_node("CheckCollision (Sequence)")
        Engine.report_active_bt_node("HitWallOrSelf?")
    end
    
    -- Wall collision
    if newX < 0 or newX >= COLS or newY < 0 or newY >= ROWS then
        Snake.alive = false
        if Engine and Engine.report_active_bt_node then
            Engine.report_active_bt_node("TriggerGameOver")
        end
        if Snake.score > Snake.highScore then Snake.highScore = Snake.score end
        ScreenShake.trigger(15, 0.5)
        return
    end
    
    -- Self collision
    for i = 1, #Snake.body do
        if Snake.body[i].x == newX and Snake.body[i].y == newY then
            Snake.alive = false
            if Engine and Engine.report_active_bt_node then
                Engine.report_active_bt_node("TriggerGameOver")
            end
            if Snake.score > Snake.highScore then Snake.highScore = Snake.score end
            ScreenShake.trigger(15, 0.5)
            return
        end
    end
    
    if Engine and Engine.report_active_bt_node then
        Engine.report_active_bt_node("CheckFood (Sequence)")
        Engine.report_active_bt_node("IsHeadOnFood?")
    end
    
    local ate = (newX == Snake.food.x and newY == Snake.food.y)
    table.insert(Snake.body, 1, {x = newX, y = newY})
    
    if ate then
        if Engine and Engine.report_active_bt_node then
            Engine.report_active_bt_node("EatAndGrow")
        end
        Snake.foodEaten = Snake.foodEaten + 1
        Snake.combo = Snake.combo + 1
        Snake.comboTimer = 3.0 -- 3 second combo window
        
        local points = 10 * Snake.combo -- Combo multiplier!
        Snake.score = Snake.score + points
        
        if Snake.speed > 0.05 then Snake.speed = Snake.speed - 0.002 end
        PlaceFood()
        ScreenShake.trigger(4, 0.1)
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

-- ============================================================================
-- VISUAL CODING INTEGRATION (BT & SHADER DEMO)
-- ============================================================================
local BT_State = {
    npcX = 500,
    npcY = 360,
    patrolDir = 1,
    patrolTimer = 0,
    statusText = "IDLE",
    lastChangeTime = 0
}

function ResetVisualCodingDemo()
    BT_State.npcX = 500
    BT_State.npcY = 360
    BT_State.patrolDir = 1
    BT_State.patrolTimer = 0
    BT_State.statusText = "IDLE"
end

-- Dicionario de Acoes
local BTActions = {}

function BTActions.Patrol(dt)
    BT_State.statusText = "Patrolling (BT: Patrol)"
    BT_State.patrolTimer = BT_State.patrolTimer + dt
    if BT_State.patrolTimer > 2.0 then
        BT_State.patrolTimer = 0
        BT_State.patrolDir = -BT_State.patrolDir
    end
    BT_State.npcX = BT_State.npcX + BT_State.patrolDir * 120 * dt
    -- Limites de movimento
    if BT_State.npcX < 250 then BT_State.npcX = 250; BT_State.patrolDir = 1 end
    if BT_State.npcX > 850 then BT_State.npcX = 850; BT_State.patrolDir = -1 end
    return "SUCCESS"
end

function BTActions.MoveToTarget(dt)
    BT_State.statusText = "Chasing Target (BT: MoveToTarget)"
    local mx = input.get_mouse_x()
    local my = input.get_mouse_y()
    local dx = mx - BT_State.npcX
    local dy = my - BT_State.npcY
    local len = math.sqrt(dx*dx + dy*dy)
    if len > 8 then
        BT_State.npcX = BT_State.npcX + (dx / len) * 220 * dt
        BT_State.npcY = BT_State.npcY + (dy / len) * 220 * dt
        return "RUNNING"
    end
    return "SUCCESS"
end

-- Dicionario de Condicoes
local BTConditions = {}

function BTConditions.IsEnemyNear()
    local mx = input.get_mouse_x()
    local my = input.get_mouse_y()
    local dx = mx - BT_State.npcX
    local dy = my - BT_State.npcY
    local len = math.sqrt(dx*dx + dy*dy)
    return len < 220.0
end

-- Carregamento dinamico da BT
local cachedBT = nil
local lastFileCheck = 0

function GetActiveBehaviorTree()
    local curTime = time.get_time()
    if not cachedBT or (curTime - lastFileCheck > 0.5) then
        lastFileCheck = curTime
        local fileExists = false
        local f = io.open("assets/scripts/custom_behavior.lua", "r")
        if f then
            fileExists = true
            f:close()
        end
        if fileExists then
            local ok, tree = pcall(dofile, "assets/scripts/custom_behavior.lua")
            if ok and tree then
                cachedBT = tree
            end
        end
    end
    if not cachedBT then
        cachedBT = {
            type = "Selector",
            name = "Root",
            children = {
                { type = "Action", name = "Patrol" }
            }
        }
    end
    return cachedBT
end

-- Execucao recursiva
function RunBehaviorTree(node, dt)
    if not node then return "FAILURE" end
    
    if Engine and Engine.report_active_bt_node then
        Engine.report_active_bt_node(node.name)
    end
    
    if node.type == "Action" then
        local act = BTActions[node.name]
        if act then
            return act(dt)
        end
        return "FAILURE"
        
    elseif node.type == "Condition" then
        local cond = BTConditions[node.name]
        if cond and cond() then
            return "SUCCESS"
        end
        return "FAILURE"
        
    elseif node.type == "Selector" then
        for _, child in ipairs(node.children or {}) do
            local status = RunBehaviorTree(child, dt)
            if status == "SUCCESS" or status == "RUNNING" then
                return status
            end
        end
        return "FAILURE"
        
    elseif node.type == "Sequence" then
        for _, child in ipairs(node.children or {}) do
            local status = RunBehaviorTree(child, dt)
            if status == "FAILURE" or status == "RUNNING" then
                return status
            end
        end
        return "SUCCESS"
    end
    
    return "FAILURE"
end

function UpdateVisualCodingDemo(dt)
    local tree = GetActiveBehaviorTree()
    RunBehaviorTree(tree, dt)
end

-- ==========================================
-- INIT
-- ==========================================
local Mode = "SNAKE"

function OnUpdate(dt)
    State.Time = State.Time + dt
    ScreenShake.update(dt)
    
    if Mode == "HUB" then
        if input.is_just_pressed("1") then
            Mode = "SNAKE"
            ResetSnake()
        elseif input.is_just_pressed("2") then
            Mode = "VISUAL_CODING"
            ResetVisualCodingDemo()
        end
    elseif Mode == "SNAKE" then
        UpdateSnake2D(dt)
        if not Snake.alive and input.is_just_pressed("R") then
            ResetSnake()
        end
        if input.is_just_pressed("Escape") then
            Mode = "HUB"
        end
    elseif Mode == "VISUAL_CODING" then
        UpdateVisualCodingDemo(dt)
        if input.is_just_pressed("Escape") then
            Mode = "HUB"
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
        
        gfx.draw_rect(420, 360, 440, 45, 0.05, 0.05, 0.1)
        gfx.draw_rect(422, 362, 436, 41, 0.03, 0.03, 0.06)
        
        gfx.draw_rect(420, 440, 440, 2, 0.06, 0.1, 0.06)
        
        imgui.text(440, 180, tr, tg, tb, "STARLIGHT ENGINE")
        imgui.text(460, 215, 0.4, 0.5, 0.6, "Odyssey Tech Demo v12 (SBA v2.0)")
        imgui.text(460, 305, 0.2, 1.0, 0.4, "[1]  CYBER SNAKE 2D")
        imgui.text(460, 372, 0.0, 1.0, 0.9, "[2]  VISUAL CODING DEMO")
        
        imgui.text(440, 460, 0.12, 0.18, 0.22, "SBA v2.0: Entity, Scene, Tween, Events")
        imgui.text(440, 480, 0.12, 0.18, 0.22, "PBR + CSM Shadows + Color HSV")
        imgui.text(440, 500, 0.12, 0.18, 0.22, "Jolt Physics + Hot-Reload + ECS")
        
    elseif Mode == "SNAKE" then
        if Engine and Engine.report_active_bt_node then
            Engine.report_active_bt_node("RenderGameFrame")
        end
        gfx.draw_rect(0, 0, 1280, 720, 0.01, 0.02, 0.01)
        
        DrawGrid(ox, oy)
        DrawFood(ox, oy)
        DrawSnake(ox, oy)
        
        local rightX = OFFSET_X + COLS * CELL + 20
        gfx.draw_rect(rightX - 5, OFFSET_Y - 5, 210, 340, 0.04, 0.06, 0.04)
        gfx.draw_rect(rightX - 3, OFFSET_Y - 3, 206, 336, 0.02, 0.03, 0.02)
        
        imgui.text(OFFSET_X, 20, 0.2, 1.0, 0.4, "CYBER SNAKE 2D")
        imgui.text(OFFSET_X, 48, 0.3, 0.4, 0.35, "Powered by SBA v2.0")
        
        imgui.text(rightX, OFFSET_Y + 5, 1, 1, 1, "Score: " .. Snake.score)
        imgui.text(rightX, OFFSET_Y + 35, 0.5, 0.5, 0.6, "High Score")
        imgui.text(rightX, OFFSET_Y + 55, 1, 0.85, 0.3, "" .. Snake.highScore)
        imgui.text(rightX, OFFSET_Y + 95, 0.3, 0.3, 0.4, "Length: " .. #Snake.body)
        imgui.text(rightX, OFFSET_Y + 115, 0.3, 0.3, 0.4, string.format("Speed: %.0f%%", (0.10 / Snake.speed) * 100))
        
        -- Combo display
        if Snake.combo > 1 and Snake.comboTimer > 0 then
            local cr, cg, cb = Color.hsv((State.Time * 0.5) % 1.0, 0.8, 1.0)
            imgui.text(rightX, OFFSET_Y + 155, cr, cg, cb, "COMBO x" .. Snake.combo)
        end
        
        imgui.text(rightX, OFFSET_Y + 195, 0.3, 0.3, 0.4, "Controls:")
        imgui.text(rightX, OFFSET_Y + 215, 0.4, 0.7, 0.4, "WASD / Arrows")
        imgui.text(rightX, OFFSET_Y + 235, 0.3, 0.3, 0.4, "ESC quit | R restart")
        
        if not Snake.started and Snake.alive then
            gfx.draw_rect(OFFSET_X + 140, OFFSET_Y + 230, 420, 40, 0.04, 0.08, 0.04)
            imgui.text(OFFSET_X + 185, OFFSET_Y + 240, 0.4, 1.0, 0.5, "Press a direction to start!")
        end
        
        if not Snake.alive then
            gfx.draw_rect(OFFSET_X + 180, OFFSET_Y + 200, 340, 130, 0.15, 0.03, 0.03)
            gfx.draw_rect(OFFSET_X + 183, OFFSET_Y + 203, 334, 124, 0.08, 0.02, 0.02)
            imgui.text(OFFSET_X + 280, OFFSET_Y + 220, 1.0, 0.2, 0.2, "GAME OVER")
            imgui.text(OFFSET_X + 260, OFFSET_Y + 255, 1, 1, 1, "Final Score: " .. Snake.score)
            if Snake.combo > 1 then
                imgui.text(OFFSET_X + 250, OFFSET_Y + 275, 1, 0.8, 0, "Best Combo: x" .. Snake.combo)
            end
            imgui.text(OFFSET_X + 250, OFFSET_Y + 295, 0.4, 1, 0.5, "Press [R] to play again")
        end
        
    elseif Mode == "VISUAL_CODING" then
        -- 1. Desenhar o shader customizado pulsando no fundo da tela
        gfx.draw_custom_shader_quad(0, 0, 1280, 720, State.Time)
        
        -- Camada semi-transparente sobre o shader
        gfx.draw_rect_alpha(0, 0, 1280, 720, 0.05, 0.03, 0.10, 0.45)
        
        -- Grid neon discreto
        local grid_color = {0.0, 1.0, 0.9, 0.15}
        for i = 0, 1280, 60 do
            gfx.draw_line(i, 0, i, 720, 1.0, grid_color[1], grid_color[2], grid_color[3], grid_color[4])
        end
        for j = 0, 720, 60 do
            gfx.draw_line(0, j, 1280, j, 1.0, grid_color[1], grid_color[2], grid_color[3], grid_color[4])
        end
        
        -- Anel sensor de proximidade (raio de 220 pixels) ao redor do NPC
        local cr, cg, cb = 1.0, 0.0, 0.6
        local mouseNear = BTConditions.IsEnemyNear()
        if mouseNear then
            cr, cg, cb = 0.0, 1.0, 0.9
        end
        gfx.draw_circle(BT_State.npcX, BT_State.npcY, 220.0, cr, cg, cb, 32, 0.25)
        
        -- Linha conectando o NPC ao cursor do mouse
        local mx = input.get_mouse_x()
        local my = input.get_mouse_y()
        gfx.draw_line(BT_State.npcX, BT_State.npcY, mx, my, 1.5, cr, cg, cb, 0.4)
        
        -- Target cursor
        local targetPulse = math.sin(State.Time * 8) * 4 + 8
        gfx.draw_circle(mx, my, targetPulse, 1.0, 0.85, 0.1, 16, 0.8)
        
        -- Desenhar o NPC (agente IA) rotacionado em direcao ao cursor
        local dx = mx - BT_State.npcX
        local dy = my - BT_State.npcY
        local angle = math.atan(dy, dx)
        
        local size = 22.0
        local x1 = BT_State.npcX + math.cos(angle) * size
        local y1 = BT_State.npcY + math.sin(angle) * size
        local x2 = BT_State.npcX + math.cos(angle + 2.5) * (size * 0.7)
        local y2 = BT_State.npcY + math.sin(angle + 2.5) * (size * 0.7)
        local x3 = BT_State.npcX + math.cos(angle - 2.5) * (size * 0.7)
        local y3 = BT_State.npcY + math.sin(angle - 2.5) * (size * 0.7)
        
        gfx.draw_triangle(x1, y1, x2, y2, x3, y3, cr, cg, cb, 0.9)
        gfx.draw_circle(BT_State.npcX, BT_State.npcY, 4.0, 1.0, 1.0, 1.0, 8, 1.0)
        
        -- Painel de informacoes
        gfx.draw_rect_alpha(20, 20, 480, 140, 0.04, 0.02, 0.08, 0.85)
        gfx.draw_rect_outline(20, 20, 480, 140, 2.0, 1.0, 0.0, 0.6, 0.65)
        
        imgui.text(35, 30, 0.0, 1.0, 0.9, "VISUAL CODING PLAYBACK INTERFACE")
        imgui.text(35, 60, 0.9, 0.9, 0.9, "Active State: " .. BT_State.statusText)
        imgui.text(35, 85, 0.7, 0.7, 0.7, "Radar Status: " .. (mouseNear and "TARGET IN RANGE!" or "Searching target..."))
        imgui.text(35, 110, 0.4, 0.4, 0.5, "BT File: assets/scripts/custom_behavior.lua")
        
        -- Painel de instrucoes
        gfx.draw_rect_alpha(900, 20, 360, 110, 0.04, 0.02, 0.08, 0.85)
        gfx.draw_rect_outline(900, 20, 360, 110, 2.0, 0.0, 1.0, 0.9, 0.65)
        
        imgui.text(915, 30, 1.0, 0.85, 0.1, "Instructions:")
        imgui.text(915, 55, 0.9, 0.9, 0.9, "Modify Visual Graph in Editor")
        imgui.text(915, 75, 0.9, 0.9, 0.9, "Click 'Export' or 'Compile'")
        imgui.text(915, 95, 0.4, 0.4, 0.5, "Press ESC to return to HUB")
    end
    
    imgui.text(1180, 8, 0.4, 0.4, 0.4, "FPS: " .. fps)
end

ResetSnake()
