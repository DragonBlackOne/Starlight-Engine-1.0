-- procedural_world.lua
-- Realistic 3D Western Desert Environment with Saloon Architecture, Dunes, Lighting & Props

local ProceduralWorld = {
    entities = {},
    lights = {},
    props = {}
}

function ProceduralWorld.init()
    ProceduralWorld.entities = {}
    ProceduralWorld.lights = {}
    ProceduralWorld.props = {}

    -- 1. Realistic Procedural Desert Dunes Terrain
    local terrain = engine.spawn_primitive("Desert_Terrain", "terrain", 55.0, 55.0, 48, 1.8)
    engine.set_pos(terrain, 0.0, -0.05, 0.0)
    engine.set_pbr(terrain, 0.78, 0.60, 0.42, 0.02, 0.95) -- Warm Desert Sandstone
    table.insert(ProceduralWorld.entities, terrain)

    -- 2. Western Saloon Building (North side: z = -14.0)
    -- Main Saloon Facade & Walls
    local mainWall = engine.spawn_primitive("Saloon_MainWall", "box", 20.0, 8.5, 2.5)
    engine.set_pos(mainWall, 0.0, 4.25, -14.5)
    engine.set_pbr(mainWall, 0.42, 0.26, 0.14, 0.05, 0.90) -- Weathered cedar wood
    table.insert(ProceduralWorld.entities, mainWall)

    -- Saloon Second-Floor Balcony
    local balcony = engine.spawn_primitive("Saloon_Balcony", "box", 20.4, 0.35, 3.0)
    engine.set_pos(balcony, 0.0, 4.4, -12.8)
    engine.set_pbr(balcony, 0.36, 0.22, 0.12, 0.05, 0.88)
    table.insert(ProceduralWorld.entities, balcony)

    -- Saloon Slanted Cedar Roof
    local roof = engine.spawn_primitive("Saloon_Roof", "wedge", 21.0, 2.8, 4.8)
    engine.set_pos(roof, 0.0, 9.8, -14.0)
    engine.set_pbr(roof, 0.28, 0.16, 0.09, 0.05, 0.92)
    table.insert(ProceduralWorld.entities, roof)

    -- Saloon Signboard ("GOLDEN ARM SALOON")
    local sign = engine.spawn_primitive("Saloon_Sign", "box", 9.0, 1.4, 0.2)
    engine.set_pos(sign, 0.0, 7.8, -13.0)
    engine.set_pbr(sign, 0.90, 0.75, 0.25, 0.65, 0.35) -- Golden saloon placard
    table.insert(ProceduralWorld.entities, sign)

    -- Porch Support Columns (Cedar log pillars)
    for colX = -9.0, 9.0, 4.5 do
        local col = engine.spawn_primitive("Porch_Col_" .. colX, "cylinder", 0.20, 4.4, 8)
        engine.set_pos(col, colX, 2.2, -11.4)
        engine.set_pbr(col, 0.46, 0.28, 0.15, 0.05, 0.85)
        table.insert(ProceduralWorld.entities, col)
    end

    -- Saloon Double Swing Doors
    local doorL = engine.spawn_primitive("Saloon_DoorL", "box", 1.2, 2.2, 0.1)
    engine.set_pos(doorL, -0.65, 1.1, -13.2)
    engine.set_pbr(doorL, 0.55, 0.35, 0.20, 0.05, 0.82)
    table.insert(ProceduralWorld.entities, doorL)

    local doorR = engine.spawn_primitive("Saloon_DoorR", "box", 1.2, 2.2, 0.1)
    engine.set_pos(doorR, 0.65, 1.1, -13.2)
    engine.set_pbr(doorR, 0.55, 0.35, 0.20, 0.05, 0.82)
    table.insert(ProceduralWorld.entities, doorR)

    -- 3. Realistic Atmosphere Lights & Saloon Warm Amber Lanterns
    local lantern1 = engine.spawn_light(-4.5, 3.4, -11.2, 1.0, 0.72, 0.35, 5.0)
    local lantern2 = engine.spawn_light(4.5, 3.4, -11.2, 1.0, 0.72, 0.35, 5.0)
    table.insert(ProceduralWorld.lights, lantern1)
    table.insert(ProceduralWorld.lights, lantern2)

    -- 4. Perimeter Wooden Fencing (East & West)
    local function spawnFenceSection(x, z, rotY)
        local post = engine.spawn_primitive("Fence_Post", "cylinder", 0.14, 1.5, 6)
        engine.set_pos(post, x, 0.75, z)
        engine.set_pbr(post, 0.44, 0.28, 0.15, 0.05, 0.92)
        table.insert(ProceduralWorld.entities, post)

        local railUpper = engine.spawn_primitive("Fence_RailU", "box", 3.0, 0.14, 0.1)
        engine.set_pos(railUpper, x + 1.5, 1.15, z)
        engine.set_rotation(railUpper, 0, math.rad(rotY), 0)
        engine.set_pbr(railUpper, 0.40, 0.25, 0.14, 0.05, 0.92)
        table.insert(ProceduralWorld.entities, railUpper)

        local railLower = engine.spawn_primitive("Fence_RailL", "box", 3.0, 0.14, 0.1)
        engine.set_pos(railLower, x + 1.5, 0.55, z)
        engine.set_rotation(railLower, 0, math.rad(rotY), 0)
        engine.set_pbr(railLower, 0.40, 0.25, 0.14, 0.05, 0.92)
        table.insert(ProceduralWorld.entities, railLower)
    end

    for fenceZ = -10.0, 10.0, 3.0 do
        spawnFenceSection(-15.0, fenceZ, 0)
        spawnFenceSection(15.0, fenceZ, 0)
    end

    -- 5. Desert Saguaro Cacti & Sandstone Boulders
    local function spawnCactus(x, z, h)
        local trunk = engine.spawn_primitive("Cactus_Trunk", "cylinder", 0.28, h, 8)
        engine.set_pos(trunk, x, h * 0.5, z)
        engine.set_pbr(trunk, 0.24, 0.48, 0.22, 0.02, 0.88)
        table.insert(ProceduralWorld.entities, trunk)

        local arm = engine.spawn_primitive("Cactus_Arm", "cylinder", 0.20, h * 0.45, 8)
        engine.set_pos(arm, x + 0.38, h * 0.65, z)
        engine.set_rotation(arm, 0, 0, math.rad(-35.0))
        engine.set_pbr(arm, 0.24, 0.48, 0.22, 0.02, 0.88)
        table.insert(ProceduralWorld.entities, arm)
    end

    spawnCactus(-12.5, 6.0, 3.8)
    spawnCactus(12.0, 8.0, 4.2)
    spawnCactus(-11.0, -8.0, 3.2)
    spawnCactus(13.0, -6.0, 4.0)

    -- Sandstone Boulders
    local function spawnBoulder(x, z, s)
        local rock = engine.spawn_primitive("Desert_Rock", "sphere", s, 6, 8)
        engine.set_pos(rock, x, s * 0.65, z)
        engine.set_pbr(rock, 0.65, 0.48, 0.35, 0.05, 0.95)
        table.insert(ProceduralWorld.entities, rock)
    end

    spawnBoulder(-13.5, -2.0, 0.8)
    spawnBoulder(13.5, 1.0, 0.9)
    spawnBoulder(-10.0, 11.0, 1.1)
    spawnBoulder(9.0, 12.0, 0.85)
end

function ProceduralWorld.destroy()
    for _, e in ipairs(ProceduralWorld.entities) do
        if e and e ~= 0 then
            engine.destroy(e)
        end
    end
    for _, p in ipairs(ProceduralWorld.props) do
        if p and p ~= 0 then
            engine.destroy(p)
        end
    end
    ProceduralWorld.entities = {}
    ProceduralWorld.props = {}
    ProceduralWorld.lights = {}
end

return ProceduralWorld
