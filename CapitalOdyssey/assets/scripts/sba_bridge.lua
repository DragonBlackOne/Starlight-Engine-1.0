-- sba_bridge.lua
-- Bridges the low-level Engine API to the high-level SBA globals

function Object(tag, x, y, z)
    local e = Engine.spawn(tag)
    Engine.set_pos(e, x or 0, y or 0, z or 0)
    return e
end

function SetPos(e, x, y, z)
    Engine.set_pos(e, x, y, z)
end

function Move(e, dx, dy, dz)
    local x, y, z = Engine.get_pos(e)
    if x then
        Engine.set_pos(e, x + (dx or 0), y + (dy or 0), z + (dz or 0))
    end
end

function SetColor(e, r, g, b)
    Engine.set_color(e, r, g, b)
end

function SetScale(e, x, y, z)
    Engine.set_scale(e, x, y, z)
end

function Say(msg)
    Engine.log(tostring(msg))
end

-- 2D Isometric Projection Math
local tileW = 130 -- Slightly larger than 128 to overlap and hide seams
local tileH = 66  -- Slightly larger than 64 to overlap and hide seams
local offsetX = 800
local offsetY = 350

function DrawIsometric(tag, x, y, z, r, g, b, a)
    local scrX = offsetX + (x - z) * (128 / 2)
    local scrY = offsetY + (x + z) * (64 / 2)
    
    gfx.draw_iso_tile(scrX - 64, scrY - 32, 128, 64, r, g, b, a or 1.0)
end

function DrawIsometricSprite(texID, x, y, z, r, g, b, a)
    local scrX = offsetX + (x - z) * (128 / 2)
    local scrY = offsetY + (x + z) * (64 / 2)
    
    local size = 128
    gfx.draw_sprite(scrX - size/2, scrY - size + 32, size, size, texID, r or 1, g or 1, b or 1, a or 1.0)
end

function Sound(freq, duration)
    -- SBA legacy sound call (beep style)
    -- In Starlight, we might map this to a generic UI beep or effect
    Engine.play_sound("assets/audio/beep.wav")
end

function IsDown(key)
    return Engine.is_down(key)
end

function Distance(e1, e2)
    local x1, y1, z1 = Engine.get_pos(e1)
    local x2, y2, z2 = Engine.get_pos(e2)
    if x1 and x2 then
        local dx = x1 - x2
        local dy = y1 - y2
        local dz = z1 - z2
        return math.sqrt(dx*dx + dy*dy + dz*dz)
    end
    return 999999
end

Say("SBA: Bridge initialized and ready.")
