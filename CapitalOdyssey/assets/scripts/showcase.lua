-- ============================================================================
-- Starlight Engine - Showcase Demo (SBA v2.0)
-- Sentinel flutuante com pulsação de energia e órbita procedural
-- ============================================================================
package.path = package.path .. ";assets/scripts/?.lua"
require("sba_bridge")

local sentinel = nil
local orbiters = {}
local sun = nil
local gameTime = 0

function OnStart()
    Say("Showcase: Cyber-Industrial Sentinel Active")
    Engine.set_camera_pos(0, 12, 18)
    Engine.look_at(0, 3, 0)

    -- Sentinela Central
    sentinel = Entity("Sentinel", 0, 5, 0)
    sentinel:setColor(0.8, 0.2, 1.0)
    sentinel:setScale(2, 2, 2)
    sentinel:setMaterial(0.95, 0.05)
    sentinel._scaleX = 2; sentinel._scaleY = 2; sentinel._scaleZ = 2

    -- Orbitadores
    for i = 1, 6 do
        local orb = Entity("Orbiter", 0, 3, 0)
        local r, g, b = Color.hsv(i / 6.0, 0.9, 1.0)
        orb:setColor(r, g, b)
        orb:setScale(0.4)
        orb:setMaterial(1.0, 0.0)
        orb._angle = (i / 6.0) * math.pi * 2
        orb._radius = 5.0
        orb._speed = 1.0 + i * 0.3
        orb._heightOffset = i * 0.5
        table.insert(orbiters, orb)
    end

    -- Plataforma
    local platform = Entity("Platform", 0, -0.5, 0)
    platform:setColor(0.06, 0.06, 0.1)
    platform:setScale(20, 0.2, 20)
    platform:setMaterial(0.3, 0.7)

    -- Pilares
    for i = 1, 4 do
        local angle = (i / 4.0) * math.pi * 2
        local px = math.cos(angle) * 8
        local pz = math.sin(angle) * 8
        local pillar = Entity("Pillar", px, 2, pz)
        pillar:setColor(0.1, 0.1, 0.15)
        pillar:setScale(0.5, 4, 0.5)
        pillar:setMaterial(0.5, 0.5)
    end

    sun = Light(0, 15, 5, 1, 0.9, 0.8, 1000)
end

function OnUpdate(dt)
    gameTime = gameTime + dt

    -- Sentinel flutuação
    local sY = 5.0 + math.sin(gameTime * 2.0) * 0.5
    sentinel:setPos(0, sY, 0)

    -- Sentinel cor pulsante
    local pr, pg, pb = Color.pulse(0.8, 0.2, 1.0, gameTime, 3, 0.3)
    sentinel:setColor(pr, pg, pb)

    -- Orbitadores girando
    for _, orb in ipairs(orbiters) do
        orb._angle = orb._angle + dt * orb._speed
        local ox = math.cos(orb._angle) * orb._radius
        local oz = math.sin(orb._angle) * orb._radius
        local oy = 3.0 + math.sin(gameTime * 2 + orb._heightOffset) * 1.5
        orb:setPos(ox, oy, oz)
    end

    -- Alerta de proximidade ao centro
    local dist = MathX.distance(sentinel.x, sentinel.z, 0, 0)
    if dist < 2.0 then
        -- Perto do núcleo — intensificar luz
        sun:setIntensity(1200 + math.sin(gameTime * 10) * 200)
    end

    Tween.update(dt)
end

function OnRenderUI()
    ui.begin(1600, 900)
    ui.panel(10, 10, 400, 50, 0.02, 0.02, 0.05, 0.85)
    ui.label("SHOWCASE // SBA v2.0 | Sentinel Demo", 30, 45, 0.8, 0.2, 1.0, 1)
    ui.finish()
end
