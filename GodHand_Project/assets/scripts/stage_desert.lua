-- stage_desert.lua
-- 3D Western Saloon Desert Arena with Destructible Props & Item Drops

local SFX = require("assets/scripts/sfx_manager")
local Combat = require("assets/scripts/combat_engine")
local GodReel = require("assets/scripts/god_reel")
local World = require("assets/scripts/procedural_world")

local Stage = {
    props = {},
    items = {},
    dustTimer = 0.0
}

function Stage.init()
    Stage.destroy()

    Stage.props = {}
    Stage.items = {}
    Stage.dustTimer = 0.0

    -- 1. Initialize Realistic 3D World (Dunes Terrain, Saloon, Saguaro Cacti, Lighting)
    World.init()

    -- 2. Spawn Destructible Props (Barrels and Crates) around perimeter
    Stage.spawnBarrel(-12.5, 0.0, -10.0, "heal")
    Stage.spawnBarrel(12.5, 0.0, -10.0, "tension")
    Stage.spawnCrate(-14.5, 0.0, 10.0, "heal")
    Stage.spawnCrate(14.5, 0.0, 10.0, "tension")
    Stage.spawnBarrel(-10.0, 0.0, 12.0, "heal")
    Stage.spawnCrate(10.0, 0.0, 12.0, "tension")
end

function Stage.spawnBarrel(x, y, z, dropType)
    local barrelMesh = engine.spawn_primitive("Destructible_Barrel", "cylinder", 0.48, 1.15, 10)
    engine.set_pos(barrelMesh, x, y + 0.58, z)
    engine.set_pbr(barrelMesh, 0.52, 0.34, 0.20, 0.10, 0.85)

    local prop = {
        pos = { x = x, y = y, z = z },
        radius = 0.55,
        active = true,
        isBroken = false,
        dropType = dropType or "heal",
        entity = barrelMesh
    }

    prop.destroy = function()
        if prop.isBroken then return end
        prop.isBroken = true
        prop.active = false
        if prop.entity and prop.entity ~= 0 then
            engine.destroy(prop.entity)
        end

        SFX.playPropBreak()
        -- Wood splinter explosion
        vfx.burst(prop.pos.x, prop.pos.y + 0.6, prop.pos.z, 0.55, 0.35, 0.2, 24, 5.0, 0.20, 0.6)
        Combat.addFloatingText("CRUSH!!", prop.pos.x, prop.pos.y + 1.4, prop.pos.z, { 1.0, 0.75, 0.2, 1.0 }, 2.0)

        -- Drop Item
        Stage.spawnItem(prop.pos.x, prop.pos.y, prop.pos.z, prop.dropType)
    end

    table.insert(Stage.props, prop)
end

function Stage.spawnCrate(x, y, z, dropType)
    local crateMesh = engine.spawn_primitive("Destructible_Crate", "box", 0.95, 0.95, 0.95)
    engine.set_pos(crateMesh, x, y + 0.48, z)
    engine.set_pbr(crateMesh, 0.48, 0.32, 0.18, 0.05, 0.90)

    local prop = {
        pos = { x = x, y = y, z = z },
        radius = 0.60,
        active = true,
        isBroken = false,
        dropType = dropType or "tension",
        entity = crateMesh
    }

    prop.destroy = function()
        if prop.isBroken then return end
        prop.isBroken = true
        prop.active = false
        if prop.entity and prop.entity ~= 0 then
            engine.destroy(prop.entity)
        end

        SFX.playPropBreak()
        vfx.burst(prop.pos.x, prop.pos.y + 0.5, prop.pos.z, 0.5, 0.35, 0.2, 26, 5.5, 0.22, 0.6)
        Combat.addFloatingText("SHATTER!!", prop.pos.x, prop.pos.y + 1.4, prop.pos.z, { 1.0, 0.85, 0.2, 1.0 }, 2.0)

        Stage.spawnItem(prop.pos.x, prop.pos.y, prop.pos.z, prop.dropType)
    end

    table.insert(Stage.props, prop)
end

function Stage.spawnItem(x, y, z, itemType)
    local itemEnt = nil
    if itemType == "heal" then
        -- Roast Chicken / Healing Melon (Juicy Red/Gold)
        itemEnt = engine.spawn_primitive("Item_Heal", "sphere", 0.28, 8, 10)
        engine.set_pbr(itemEnt, 0.95, 0.25, 0.15, 0.1, 0.35)
    else
        -- Tension God Card (Golden Spinning Cube)
        itemEnt = engine.spawn_primitive("Item_Tension", "box", 0.38, 0.38, 0.38)
        engine.set_pbr(itemEnt, 1.0, 0.85, 0.15, 0.85, 0.20)
    end

    engine.set_pos(itemEnt, x, y + 0.4, z)

    local item = {
        pos = { x = x, y = y + 0.4, z = z },
        type = itemType,
        entity = itemEnt,
        life = 25.0,
        active = true
    }

    table.insert(Stage.items, item)
end

function Stage.update(dt, player)
    -- Ambient Desert Dust Particle Stream
    Stage.dustTimer = Stage.dustTimer + dt
    if Stage.dustTimer >= 0.10 then
        Stage.dustTimer = 0.0
        local rx = (math.random() * 34.0) - 17.0
        local rz = (math.random() * 34.0) - 17.0
        vfx.emit(rx, 0.2 + math.random() * 1.5, rz, 1.6, 0.0, -0.9, 0.85, 0.72, 0.55, 1, 0.12, 1.8)
    end

    -- Check Item Pickups by Player
    for i = #Stage.items, 1, -1 do
        local item = Stage.items[i]
        if item.active then
            -- Hover bobbing & spinning
            local hoverY = 0.38 + math.sin(engine.get_time() * 5.0) * 0.10
            engine.set_pos(item.entity, item.pos.x, hoverY, item.pos.z)
            engine.set_rotation(item.entity, 0.0, math.rad(engine.get_time() * 95.0), 0.0)

            local dx = player.pos.x - item.pos.x
            local dz = player.pos.z - item.pos.z
            local dist = math.sqrt(dx * dx + dz * dz)

            if dist <= 1.3 then
                -- Pick up!
                item.active = false
                if item.entity and item.entity ~= 0 then
                    engine.destroy(item.entity)
                end
                SFX.playItemPickup()

                if item.type == "heal" then
                    player.hp = math.min(player.maxHp, player.hp + 80)
                    Combat.addFloatingText("+80 HP RESTORED!", player.pos.x, player.pos.y + 2.0, player.pos.z, { 0.2, 1.0, 0.4, 1.0 }, 2.2)
                    vfx.burst(player.pos.x, player.pos.y + 1.0, player.pos.z, 0.2, 1.0, 0.4, 16, 3.5, 0.14, 0.45)
                else
                    GodReel.addTension(40.0)
                    Combat.addFloatingText("+40% GOD TENSION!", player.pos.x, player.pos.y + 2.0, player.pos.z, { 1.0, 0.85, 0.1, 1.0 }, 2.2)
                    vfx.burst(player.pos.x, player.pos.y + 1.0, player.pos.z, 1.0, 0.85, 0.1, 20, 4.0, 0.16, 0.50)
                end

                table.remove(Stage.items, i)
            end
        end
    end
end

function Stage.destroy()
    World.destroy()
    for _, prop in ipairs(Stage.props) do
        if prop.entity and prop.entity ~= 0 then
            engine.destroy(prop.entity)
        end
    end
    for _, item in ipairs(Stage.items) do
        if item.entity and item.entity ~= 0 then
            engine.destroy(item.entity)
        end
    end
    Stage.props = {}
    Stage.items = {}
end

return Stage
