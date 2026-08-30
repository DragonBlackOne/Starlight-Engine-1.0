-- hierarchy_showcase.lua
-- Module 3 Scene Graph & Planetary ECS Hierarchy

local Hierarchy = {
    sun = nil,
    planets = {},
    time = 0
}

function Hierarchy.init()
    Hierarchy.planets = {}
    Hierarchy.time = 0

    local centerX = -18.0
    local centerY = 4.0
    local centerZ = -5.0

    -- 1. Central Core Sun
    local sun = engine.create_entity("Hierarchy_CoreSun")
    engine.set_position(sun, centerX, centerY, centerZ)
    engine.set_scale(sun, 2.0, 2.0, 2.0)
    engine.add_mesh(sun, "sphere", {
        albedo = {1.0, 0.7, 0.1, 1.0},
        metallic = 0.1,
        roughness = 0.1,
        ao = 1.0,
        emission = {3.0, 2.0, 0.2}
    })
    Hierarchy.sun = {id = sun, x = centerX, y = centerY, z = centerZ}

    -- 2. Orbiting Planets
    local planetConfigs = {
        {name = "PlanetCyber", color = {0.0, 0.9, 1.0}, dist = 4.5, speed = 0.8, scale = 0.8, moons = 2},
        {name = "PlanetNeon",  color = {0.9, 0.1, 0.8}, dist = 7.0, speed = 0.5, scale = 1.0, moons = 3},
        {name = "PlanetGold",  color = {1.0, 0.85, 0.2}, dist = 9.5, speed = 0.35, scale = 1.2, moons = 1}
    }

    for _, pc in ipairs(planetConfigs) do
        local pEntity = engine.create_entity(pc.name)
        engine.set_scale(pEntity, pc.scale, pc.scale, pc.scale)
        engine.add_mesh(pEntity, "sphere", {
            albedo = {pc.color[1], pc.color[2], pc.color[3], 1.0},
            metallic = 0.4,
            roughness = 0.3,
            ao = 1.0,
            emission = {0.0, 0.0, 0.0}
        })

        local moons = {}
        for m = 1, pc.moons do
            local mEntity = engine.create_entity(string.format("%s_Moon_%d", pc.name, m))
            local mScale = pc.scale * 0.35
            engine.set_scale(mEntity, mScale, mScale, mScale)
            engine.add_mesh(mEntity, "sphere", {
                albedo = {0.8, 0.8, 0.9, 1.0},
                metallic = 0.8,
                roughness = 0.2,
                ao = 1.0,
                emission = {0.1, 0.1, 0.2}
            })
            table.insert(moons, {
                id = mEntity,
                dist = 1.2 + m * 0.5,
                speed = 2.0 + m * 0.8,
                phase = (m / pc.moons) * math.pi * 2.0
            })
        end

        table.insert(Hierarchy.planets, {
            id = pEntity,
            dist = pc.dist,
            speed = pc.speed,
            scale = pc.scale,
            moons = moons
        })
    end
end

function Hierarchy.update(dt)
    Hierarchy.time = Hierarchy.time + dt

    if not Hierarchy.sun then return end

    -- Rotate Core Sun
    engine.set_rotation(Hierarchy.sun.id, 0.0, math.rad((Hierarchy.time * 15.0) % 360), 0.0)

    -- Update Planets and their child Moons
    for _, p in ipairs(Hierarchy.planets) do
        local pAngle = Hierarchy.time * p.speed
        local px = Hierarchy.sun.x + math.cos(pAngle) * p.dist
        local pz = Hierarchy.sun.z + math.sin(pAngle) * p.dist
        local py = Hierarchy.sun.y + math.sin(Hierarchy.time * p.speed * 2.0) * 0.5

        engine.set_position(p.id, px, py, pz)
        engine.set_rotation(p.id, 0.0, math.rad((Hierarchy.time * 45.0) % 360), 0.0)

        -- Update Moons relative to Planet position
        for _, m in ipairs(p.moons) do
            local mAngle = Hierarchy.time * m.speed + m.phase
            local mx = px + math.cos(mAngle) * m.dist
            local mz = pz + math.sin(mAngle) * m.dist
            local my = py + math.sin(mAngle * 1.5) * 0.4

            engine.set_position(m.id, mx, my, mz)
        end
    end
end

return Hierarchy
