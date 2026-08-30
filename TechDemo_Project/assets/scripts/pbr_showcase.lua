-- pbr_showcase.lua
-- Module 4 PBR & Cook-Torrance Showcase

local PBR = {
    spheres = {},
    lights = {},
    time = 0
}

function PBR.init()
    PBR.spheres = {}
    PBR.lights = {}
    PBR.time = 0

    -- 1. Create Ground Floor with Retro Grid & Metallic finish
    local floor = engine.create_entity("PBR_Ground_Grid")
    engine.set_position(floor, 0.0, -1.0, 0.0)
    engine.set_scale(floor, 60.0, 0.2, 60.0)
    engine.add_mesh(floor, "cube", {
        albedo = {0.04, 0.03, 0.08, 1.0},
        metallic = 0.9,
        roughness = 0.15,
        ao = 1.0,
        emission = {0.0, 0.0, 0.0}
    })

    -- 2. 5x5 PBR Sphere Grid (Varying Metallic on X, Roughness on Y)
    local rows = 5
    local cols = 5
    local spacing = 3.0
    local startX = -((cols - 1) * spacing) * 0.5
    local startY = 1.5

    for r = 0, rows - 1 do
        local roughness = math.max(0.05, r / (rows - 1))
        for c = 0, cols - 1 do
            local metallic = c / (cols - 1)
            local sphere = engine.create_entity(string.format("PBR_Sphere_M%.2f_R%.2f", metallic, roughness))
            local px = startX + c * spacing
            local py = startY + r * 2.5
            local pz = -5.0

            engine.set_position(sphere, px, py, pz)
            engine.set_scale(sphere, 1.0, 1.0, 1.0)
            
            -- Color tint: Retro Synthwave Sunset / Cyber Gradient
            local t = c / (cols - 1)
            local cr = 0.9 * t + 0.1 * (1.0 - t)
            local cg = 0.2 * t + 0.8 * (1.0 - t)
            local cb = 0.9 * (1.0 - t) + 0.2 * t

            engine.add_mesh(sphere, "sphere", {
                albedo = {cr, cg, cb, 1.0},
                metallic = metallic,
                roughness = roughness,
                ao = 1.0,
                emission = {0.0, 0.0, 0.0}
            })
            table.insert(PBR.spheres, {id = sphere, baseX = px, baseY = py, baseZ = pz, m = metallic, r = roughness})
        end
    end

    -- 3. Orbiting Neon Point Lights
    local lightColors = {
        {name = "CyberCyan",   color = {0.0, 0.85, 1.0}, radius = 12.0, speed = 1.2, height = 4.0, phase = 0.0},
        {name = "HotMagenta",  color = {1.0, 0.0, 0.65}, radius = 14.0, speed = -0.9, height = 6.0, phase = math.pi * 0.5},
        {name = "NeonViolet",  color = {0.6, 0.1, 1.0},  radius = 10.0, speed = 1.5, height = 3.0, phase = math.pi},
        {name = "SunsetGold",  color = {1.0, 0.8, 0.1},  radius = 16.0, speed = -1.1, height = 5.0, phase = math.pi * 1.5}
    }

    for _, l in ipairs(lightColors) do
        local lightEntity = engine.create_entity("Light_" .. l.name)
        engine.set_position(lightEntity, 0.0, l.height, 0.0)
        engine.set_scale(lightEntity, 0.4, 0.4, 0.4)
        engine.add_mesh(lightEntity, "sphere", {
            albedo = {l.color[1], l.color[2], l.color[3], 1.0},
            metallic = 0.0,
            roughness = 0.0,
            ao = 1.0,
            emission = {l.color[1] * 5.0, l.color[2] * 5.0, l.color[3] * 5.0}
        })
        table.insert(PBR.lights, {
            id = lightEntity,
            color = l.color,
            radius = l.radius,
            speed = l.speed,
            height = l.height,
            phase = l.phase
        })
    end
end

function PBR.update(dt)
    PBR.time = PBR.time + dt

    -- Update orbiting light positions
    for _, l in ipairs(PBR.lights) do
        local angle = PBR.time * l.speed + l.phase
        local lx = math.cos(angle) * l.radius
        local lz = -5.0 + math.sin(angle) * l.radius
        local ly = l.height + math.sin(PBR.time * 2.0 + l.phase) * 1.5
        engine.set_position(l.id, lx, ly, lz)
    end

    -- Floating gentle wave on spheres
    for idx, s in ipairs(PBR.spheres) do
        local wave = math.sin(PBR.time * 2.5 + idx * 0.2) * 0.15
        engine.set_position(s.id, s.baseX, s.baseY + wave, s.baseZ)
        engine.set_rotation(s.id, 0.0, math.rad((PBR.time * 20.0 + idx * 10) % 360), 0.0)
    end
end

return PBR
