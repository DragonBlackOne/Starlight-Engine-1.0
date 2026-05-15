-- ============================================================================
-- STARLIGHT ENGINE: BEGINNER TUTORIAL (SBA v2.0)
-- Um jogo simples usando o framework completo da Fusion Engine
-- ============================================================================
package.path = package.path .. ";assets/scripts/?.lua"
require("sba_bridge")

local player = nil
local goal = nil
local score = 0
local goalPulse = 0

function OnStart()
    Say("Bem-vindo ao Starlight Engine!")
    Engine.set_camera_pos(0, 25, 15)
    Engine.look_at(0, 0, 0)

    -- Cria o jogador usando o Entity wrapper OO
    player = Entity("Player", 0, 0, 0)
    player:setColor(0, 1, 1)
    player:setScale(1)
    player:setMaterial(0.8, 0.2)

    -- Cria um objetivo (uma caixa dourada)
    goal = Entity("Goal", 10, 0.5, 10)
    goal:setColor(1, 0.8, 0)
    goal:setScale(0.8)
    goal:setMaterial(1.0, 0.0) -- Full metallic

    -- Chão
    local floor = Entity("Floor", 0, -0.5, 0)
    floor:setColor(0.08, 0.08, 0.12)
    floor:setScale(30, 0.1, 30)
    floor:setMaterial(0.1, 0.9)

    -- Luz
    Light(0, 15, 0, 1, 1, 1, 800)

    score = 0
end

function OnUpdate(dt)
    -- Movimentação com WASD
    local speed = 10 * dt
    if input.is_down("W") or input.is_down("Up") then player:move(0, 0, -speed) end
    if input.is_down("S") or input.is_down("Down") then player:move(0, 0, speed) end
    if input.is_down("A") or input.is_down("Left") then player:move(-speed, 0, 0) end
    if input.is_down("D") or input.is_down("Right") then player:move(speed, 0, 0) end

    -- Flutuação do objetivo
    goalPulse = goalPulse + dt
    goal:setPos(goal.x, 0.5 + math.sin(goalPulse * 4) * 0.3, goal.z)

    -- Verifica distância (colisão simples)
    local px, py, pz = player:getPos()
    local gx, gy, gz = goal:getPos()
    local dist = MathX.distance3D(px, py, pz, gx, gy, gz)

    if dist < 2.0 then
        score = score + 1
        Say("Ponto! Score: " .. score)

        -- Move o objetivo para um lugar aleatório com Tween!
        local nx = MathX.random_range(-12, 12)
        local nz = MathX.random_range(-12, 12)
        goal:setPos(nx, 0.5, nz)

        -- Cor muda a cada ponto (HSV rainbow)
        local r, g, b = Color.hsv((score * 0.1) % 1.0, 0.8, 1.0)
        goal:setColor(r, g, b)

        Sound(660, 0.1)
    end

    -- Tweens e Timers
    Tween.update(dt)
    Coroutine.update(dt)
end

function OnRenderUI()
    ui.begin(1600, 900)
    ui.panel(10, 10, 300, 50, 0.02, 0.02, 0.05, 0.85)
    ui.label("TUTORIAL | Score: " .. score, 30, 45, 0, 1, 0.8, 1)
    ui.label("Move: WASD | Collect the cubes!", 30, 75, 0.5, 0.5, 0.5, 1)
    ui.finish()
end
