-- ============================================================================
-- Starlight Engine - Rotate Demo (SBA v2.0)
-- Demonstra rotação procedural e VFX de partículas usando Entity wrapper
-- ============================================================================
package.path = package.path .. ";assets/scripts/?.lua"
require("sba_bridge")

local sentinel = nil
local rotAngle = 0
local loopCount = 0

function OnStart()
    Say("Rotate Demo: Entity + Tween System Active")
    Engine.set_camera_pos(0, 8, 12)
    Engine.look_at(0, 2, 0)

    sentinel = Entity("Sentinel", 0, 3, 0)
    sentinel:setColor(0.2, 0.8, 1.0)
    sentinel:setScale(1.5)
    sentinel:setMaterial(0.9, 0.1) -- High metallic

    -- Floor
    local floor = Entity("Floor", 0, -0.5, 0)
    floor:setColor(0.05, 0.05, 0.08)
    floor:setScale(15, 0.1, 15)

    Light(0, 10, 5, 1, 0.9, 0.7, 600)
end

function OnUpdate(dt)
    rotAngle = rotAngle + dt * 90.0
    if rotAngle > 360.0 then
        rotAngle = rotAngle - 360.0
        loopCount = loopCount + 1
        Say("Loop " .. loopCount .. " completo!")
    end

    -- Orbitar a entidade em círculo
    local rad = math.rad(rotAngle)
    local orbitRadius = 4.0
    local x = math.cos(rad) * orbitRadius
    local z = math.sin(rad) * orbitRadius
    local y = 3.0 + math.sin(rotAngle * 0.05) * 0.5

    sentinel:setPos(x, y, z)

    -- Cor pulsando com HSV
    local r, g, b = Color.hsv((rotAngle / 360.0) % 1.0, 0.7, 1.0)
    sentinel:setColor(r, g, b)

    Tween.update(dt)
end
