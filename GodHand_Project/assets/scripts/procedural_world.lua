-- ============================================================================
-- GodHand_Project — High-Fidelity Western Frontier Town & Desert Environment
-- ============================================================================

ProceduralWorld = {
    entities = {},
    lights = {},
    props = {},
    windmillBlade = nil,
    windmillAngle = 0.0,
    dustParticles = {}
}

function ProceduralWorld.init()
    ProceduralWorld.entities = {}
    ProceduralWorld.lights = {}
    ProceduralWorld.props = {}

    -- 1. Expansive Desert Sand Terrain
    local terrain = engine.spawn_primitive("Desert_Floor", "plane", 300.0, 300.0, 32.0, 32.0)
    engine.set_pos(terrain, 0.0, 0.0, 0.0)
    engine.set_pbr(terrain, 0.78, 0.62, 0.44, 0.01, 0.90) -- Warm golden desert sand
    if engine.set_texture then engine.set_texture(terrain, "assets/textures/desert_ground_pbr.jpg") end
    table.insert(ProceduralWorld.entities, terrain)

    -- 2. Town Street Boardwalk Wooden Planks
    local boardwalk = engine.spawn_primitive("Main_Boardwalk", "box", 32.0, 0.15, 14.0)
    engine.set_pos(boardwalk, 0.0, 0.08, -8.0)
    engine.set_pbr(boardwalk, 0.42, 0.28, 0.18, 0.04, 0.88)
    table.insert(ProceduralWorld.entities, boardwalk)

    -- =========================================================================
    -- Building 1: Main Double-Story Cantina & Saloon (North: z = -15.0)
    -- =========================================================================
    local cantina = engine.spawn_primitive("Cantina_Main", "box", 26.0, 10.0, 6.0)
    engine.set_pos(cantina, 0.0, 5.0, -16.0)
    engine.set_pbr(cantina, 0.86, 0.75, 0.62, 0.02, 0.85)
    if engine.set_texture then engine.set_texture(cantina, "assets/textures/adobe_cantina_facade.jpg") end
    table.insert(ProceduralWorld.entities, cantina)

    -- Saloon Porch Deck & Roof Pergola
    local porchRoof = engine.spawn_primitive("Porch_Roof", "box", 26.5, 0.35, 4.8)
    engine.set_pos(porchRoof, 0.0, 5.0, -11.5)
    engine.set_pbr(porchRoof, 0.36, 0.22, 0.12, 0.05, 0.90)
    table.insert(ProceduralWorld.entities, porchRoof)

    -- Porch Support Columns
    for colX = -12.0, 12.0, 4.0 do
        local col = engine.spawn_primitive("Porch_Col_" .. colX, "cylinder", 0.22, 5.0, 8)
        engine.set_pos(col, colX, 2.5, -9.2)
        engine.set_pbr(col, 0.38, 0.24, 0.14, 0.05, 0.88)
        table.insert(ProceduralWorld.entities, col)
    end

    -- Balcony Railings
    local balconyRail = engine.spawn_primitive("Balcony_Rail", "box", 26.0, 0.9, 0.15)
    engine.set_pos(balconyRail, 0.0, 5.7, -9.2)
    engine.set_pbr(balconyRail, 0.40, 0.26, 0.16, 0.05, 0.88)
    table.insert(ProceduralWorld.entities, balconyRail)

    -- Terracotta Spanish Tile Roof
    local roofTile = engine.spawn_primitive("Cantina_Roof_Tile", "wedge", 27.0, 3.2, 7.5)
    engine.set_pos(roofTile, 0.0, 11.2, -16.0)
    engine.set_pbr(roofTile, 0.72, 0.30, 0.18, 0.02, 0.88)
    table.insert(ProceduralWorld.entities, roofTile)

    -- Gold Cantina Saloon Sign
    local saloonSign = engine.spawn_primitive("Saloon_Sign", "box", 9.0, 1.6, 0.25)
    engine.set_pos(saloonSign, 0.0, 8.8, -12.8)
    engine.set_pbr(saloonSign, 0.95, 0.82, 0.22, 0.85, 0.25)
    table.insert(ProceduralWorld.entities, saloonSign)

    -- Warm Amber Lantern Lights
    local lanternL = engine.spawn_light(-6.0, 4.0, -9.0, 1.0, 0.75, 0.35, 5.5)
    local lanternR = engine.spawn_light(6.0, 4.0, -9.0, 1.0, 0.75, 0.35, 5.5)
    table.insert(ProceduralWorld.lights, lanternL)
    table.insert(ProceduralWorld.lights, lanternR)

    -- =========================================================================
    -- Building 2: Sheriff Office & Jailhouse (West: x = -20.0, z = -4.0)
    -- =========================================================================
    local sheriff = engine.spawn_primitive("Sheriff_Office", "box", 12.0, 7.5, 14.0)
    engine.set_pos(sheriff, -21.0, 3.75, -4.0)
    engine.set_pbr(sheriff, 0.65, 0.58, 0.52, 0.02, 0.92) -- Weathered sandstone
    table.insert(ProceduralWorld.entities, sheriff)

    local sheriffSign = engine.spawn_primitive("Sheriff_Sign", "box", 0.2, 1.2, 5.5)
    engine.set_pos(sheriffSign, -14.8, 5.2, -4.0)
    engine.set_pbr(sheriffSign, 0.85, 0.72, 0.25, 0.65, 0.40)
    table.insert(ProceduralWorld.entities, sheriffSign)

    -- =========================================================================
    -- Building 3: General Store & Gunsmith (East: x = 20.0, z = -4.0)
    -- =========================================================================
    local generalStore = engine.spawn_primitive("General_Store", "box", 12.0, 7.5, 14.0)
    engine.set_pos(generalStore, 21.0, 3.75, -4.0)
    engine.set_pbr(generalStore, 0.78, 0.68, 0.55, 0.02, 0.88)
    table.insert(ProceduralWorld.entities, generalStore)

    local storeSign = engine.spawn_primitive("Store_Sign", "box", 0.2, 1.2, 5.5)
    engine.set_pos(storeSign, 14.8, 5.2, -4.0)
    engine.set_pbr(storeSign, 0.35, 0.55, 0.75, 0.05, 0.80)
    table.insert(ProceduralWorld.entities, storeSign)

    -- =========================================================================
    -- Water Tower & Spinning Windmill (North-East: x = 16.0, z = -24.0)
    -- =========================================================================
    -- Tower Legs
    local leg1 = engine.spawn_primitive("Tower_Leg1", "cylinder", 0.25, 12.0, 6)
    engine.set_pos(leg1, 14.5, 6.0, -22.5)
    table.insert(ProceduralWorld.entities, leg1)

    local leg2 = engine.spawn_primitive("Tower_Leg2", "cylinder", 0.25, 12.0, 6)
    engine.set_pos(leg2, 17.5, 6.0, -22.5)
    table.insert(ProceduralWorld.entities, leg2)

    local leg3 = engine.spawn_primitive("Tower_Leg3", "cylinder", 0.25, 12.0, 6)
    engine.set_pos(leg3, 14.5, 6.0, -25.5)
    table.insert(ProceduralWorld.entities, leg3)

    local leg4 = engine.spawn_primitive("Tower_Leg4", "cylinder", 0.25, 12.0, 6)
    engine.set_pos(leg4, 17.5, 6.0, -25.5)
    table.insert(ProceduralWorld.entities, leg4)

    -- Water Tank
    local tank = engine.spawn_primitive("Water_Tank", "cylinder", 2.2, 4.0, 16)
    engine.set_pos(tank, 16.0, 13.5, -24.0)
    engine.set_pbr(tank, 0.45, 0.32, 0.20, 0.05, 0.85) -- Cedar timber tank
    table.insert(ProceduralWorld.entities, tank)

    -- Windmill Hub & Blades
    local hub = engine.spawn_primitive("Windmill_Hub", "cylinder", 0.35, 0.8, 8)
    engine.set_pos(hub, 16.0, 17.0, -22.0)
    engine.set_rotation(hub, 1.57, 0, 0)
    engine.set_pbr(hub, 0.65, 0.65, 0.68, 0.85, 0.30) -- Galvanized steel
    table.insert(ProceduralWorld.entities, hub)

    ProceduralWorld.windmillBlade = engine.spawn_primitive("Windmill_Blades", "box", 6.5, 0.15, 6.5)
    engine.set_pos(ProceduralWorld.windmillBlade, 16.0, 17.0, -21.5)
    engine.set_pbr(ProceduralWorld.windmillBlade, 0.70, 0.70, 0.75, 0.85, 0.35)
    table.insert(ProceduralWorld.entities, ProceduralWorld.windmillBlade)

    -- =========================================================================
    -- Props: Barrels, Shipping Crates & Hitching Posts
    -- =========================================================================
    local barrelPositions = {
        {-8.5, 0.7, -8.0}, {-7.5, 0.7, -8.2}, {-8.0, 2.0, -8.1}, -- Stack of 3 barrels
        {8.5, 0.7, -8.0}, {9.5, 0.7, -8.2},
        {-13.5, 0.7, 1.0}, {-13.5, 0.7, 2.2},
        {13.5, 0.7, 1.0}, {13.5, 0.7, 2.2}
    }
    for i, pos in ipairs(barrelPositions) do
        local barrel = engine.spawn_primitive("Whiskey_Barrel_" .. i, "cylinder", 0.48, 1.35, 12)
        engine.set_pos(barrel, pos[1], pos[2], pos[3])
        engine.set_pbr(barrel, 0.45, 0.28, 0.16, 0.15, 0.80)
        table.insert(ProceduralWorld.props, barrel)
    end

    local cratePositions = {
        {-10.5, 0.6, -7.8, 1.2}, {-9.2, 0.5, -6.8, 1.0},
        {10.5, 0.6, -7.8, 1.2}, {11.8, 0.5, -6.8, 1.0},
        {-14.0, 0.65, -3.0, 1.3}, {14.0, 0.65, -3.0, 1.3}
    }
    for i, pos in ipairs(cratePositions) do
        local sz = pos[4]
        local crate = engine.spawn_primitive("Wooden_Crate_" .. i, "box", sz, sz, sz)
        engine.set_pos(crate, pos[1], pos[2], pos[3])
        engine.set_pbr(crate, 0.55, 0.38, 0.22, 0.05, 0.85)
        table.insert(ProceduralWorld.props, crate)
    end

    -- Hitching Posts (for tied horses)
    for _, zPos in ipairs({-6.0, 0.0}) do
        local postL = engine.spawn_primitive("Hitch_L_" .. zPos, "cylinder", 0.12, 1.1, 6)
        engine.set_pos(postL, -11.5, 0.55, zPos)
        engine.set_pbr(postL, 0.35, 0.22, 0.12, 0.05, 0.90)
        table.insert(ProceduralWorld.props, postL)

        local railL = engine.spawn_primitive("Hitch_Rail_L_" .. zPos, "cylinder", 0.08, 4.0, 6)
        engine.set_pos(railL, -11.5, 0.95, zPos + 1.5)
        engine.set_rotation(railL, 1.57, 0, 0)
        engine.set_pbr(railL, 0.35, 0.22, 0.12, 0.05, 0.90)
        table.insert(ProceduralWorld.props, railL)

        local postR = engine.spawn_primitive("Hitch_R_" .. zPos, "cylinder", 0.12, 1.1, 6)
        engine.set_pos(postR, 11.5, 0.55, zPos)
        engine.set_pbr(postR, 0.35, 0.22, 0.12, 0.05, 0.90)
        table.insert(ProceduralWorld.props, postR)

        local railR = engine.spawn_primitive("Hitch_Rail_R_" .. zPos, "cylinder", 0.08, 4.0, 6)
        engine.set_pos(railR, 11.5, 0.95, zPos + 1.5)
        engine.set_rotation(railR, 1.57, 0, 0)
        engine.set_pbr(railR, 0.35, 0.22, 0.12, 0.05, 0.90)
        table.insert(ProceduralWorld.props, railR)
    end

    -- =========================================================================
    -- Giant Saguaro Cactuses & Desert Flora
    -- =========================================================================
    local cactusPositions = {
        {-28.0, 4.0, -12.0, 5.5}, {-32.0, 3.5, 8.0, 4.8},
        {28.0, 4.0, -12.0, 5.5}, {32.0, 3.5, 8.0, 4.8},
        {-18.0, 3.0, 18.0, 4.2}, {18.0, 3.0, 18.0, 4.2},
        {0.0, 4.5, 32.0, 6.0}
    }
    for i, pos in ipairs(cactusPositions) do
        local h = pos[4]
        local trunk = engine.spawn_primitive("Cactus_Trunk_" .. i, "cylinder", 0.42, h, 8)
        engine.set_pos(trunk, pos[1], h * 0.5, pos[3])
        engine.set_pbr(trunk, 0.22, 0.48, 0.20, 0.02, 0.80) -- Desert saguaro green
        table.insert(ProceduralWorld.props, trunk)

        -- Arm Left
        local armL = engine.spawn_primitive("Cactus_ArmL_" .. i, "cylinder", 0.26, h * 0.45, 6)
        engine.set_pos(armL, pos[1] - 0.75, h * 0.65, pos[3])
        engine.set_pbr(armL, 0.22, 0.48, 0.20, 0.02, 0.80)
        table.insert(ProceduralWorld.props, armL)

        -- Arm Right
        local armR = engine.spawn_primitive("Cactus_ArmR_" .. i, "cylinder", 0.26, h * 0.40, 6)
        engine.set_pos(armR, pos[1] + 0.75, h * 0.55, pos[3])
        engine.set_pbr(armR, 0.22, 0.48, 0.20, 0.02, 0.80)
        table.insert(ProceduralWorld.props, armR)
    end

    -- =========================================================================
    -- Red Rock Canyon Mesas (Background Horizon: z = -90.0)
    -- =========================================================================
    local mesa1 = engine.spawn_primitive("Mesa_North", "box", 120.0, 38.0, 35.0)
    engine.set_pos(mesa1, 0.0, 19.0, -95.0)
    engine.set_pbr(mesa1, 0.72, 0.38, 0.24, 0.02, 0.95) -- Red canyon rock
    table.insert(ProceduralWorld.entities, mesa1)

    local mesa2 = engine.spawn_primitive("Mesa_West", "box", 40.0, 28.0, 100.0)
    engine.set_pos(mesa2, -85.0, 14.0, 0.0)
    engine.set_pbr(mesa2, 0.70, 0.36, 0.22, 0.02, 0.95)
    table.insert(ProceduralWorld.entities, mesa2)

    local mesa3 = engine.spawn_primitive("Mesa_East", "box", 40.0, 28.0, 100.0)
    engine.set_pos(mesa3, 85.0, 14.0, 0.0)
    engine.set_pbr(mesa3, 0.70, 0.36, 0.22, 0.02, 0.95)
    table.insert(ProceduralWorld.entities, mesa3)

    -- =========================================================================
    -- Animated Dust Wind Ribbon Entities (Particles)
    -- =========================================================================
    for d = 1, 8 do
        local dust = engine.spawn_primitive("Dust_Ribbon_" .. d, "box", 2.2, 0.08, 0.4)
        local dx = (math.random() - 0.5) * 40.0
        local dz = (math.random() - 0.5) * 30.0 - 5.0
        engine.set_pos(dust, dx, 0.15 + math.random() * 0.4, dz)
        engine.set_pbr(dust, 0.85, 0.75, 0.55, 0.0, 1.0)
        table.insert(ProceduralWorld.dustParticles, {
            id = dust,
            x = dx,
            y = 0.2 + math.random() * 0.3,
            z = dz,
            speed = 3.5 + math.random() * 4.0
        })
    end
end

function ProceduralWorld.update(dt)
    -- 1. Spin Windmill in the Desert Wind
    if ProceduralWorld.windmillBlade then
        ProceduralWorld.windmillAngle = ProceduralWorld.windmillAngle + dt * 1.8
        engine.set_rotation(ProceduralWorld.windmillBlade, 0, 0, ProceduralWorld.windmillAngle)
    end

    -- 2. Drift Sand & Dust Particles Across the Town Plaza
    for _, dust in ipairs(ProceduralWorld.dustParticles) do
        dust.x = dust.x + dust.speed * dt
        dust.z = dust.z + dt * 0.5
        if dust.x > 25.0 then
            dust.x = -25.0
            dust.z = (math.random() - 0.5) * 30.0 - 5.0
        end
        engine.set_pos(dust.id, dust.x, dust.y + math.sin(dust.x * 0.8) * 0.1, dust.z)
    end
end

function ProceduralWorld.destroy()
    for _, entity in ipairs(ProceduralWorld.entities) do
        if entity and entity ~= 0 then
            engine.destroy(entity)
        end
    end
    for _, prop in ipairs(ProceduralWorld.props) do
        if prop and prop ~= 0 then
            engine.destroy(prop)
        end
    end
    for _, dust in ipairs(ProceduralWorld.dustParticles) do
        if dust.id and dust.id ~= 0 then
            engine.destroy(dust.id)
        end
    end
    for _, light in ipairs(ProceduralWorld.lights) do
        if light and light ~= 0 and engine.destroy_light then
            engine.destroy_light(light)
        end
    end
    ProceduralWorld.entities = {}
    ProceduralWorld.props = {}
    ProceduralWorld.lights = {}
    ProceduralWorld.dustParticles = {}
    ProceduralWorld.windmillBlade = nil
end

return ProceduralWorld
