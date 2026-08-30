-- physics_showcase.lua
-- Module 6 Physics & Collision Dynamics Showcase

local Physics = {
    bodies = {},
    projectiles = {},
    laserTarget = {x = 0, y = 0, z = 0},
    laserHit = false,
    laserNormal = {x = 0, y = 1, z = 0},
    gravityY = -9.81,
    time = 0
}

function Physics.init()
    Physics.bodies = {}
    Physics.projectiles = {}
    Physics.time = 0
    Physics.gravityY = -9.81

    Physics.spawnPyramid(12.0, 0.0, -5.0, 4)
end

function Physics.spawnPyramid(startX, startY, startZ, levels)
    local boxSize = 1.0
    local spacing = 1.05

    for level = 0, levels - 1 do
        local count = levels - level
        local y = startY + level * spacing + 0.5
        local xOffset = (level * spacing) * 0.5

        for i = 0, count - 1 do
            local x = startX - (count * spacing * 0.5) + i * spacing + xOffset + 0.5
            local z = startZ

            local box = engine.create_entity(string.format("PhysBox_L%d_%d", level, i))
            engine.set_position(box, x, y, z)
            engine.set_scale(box, boxSize, boxSize, boxSize)

            -- Neon Outrun colors based on level
            local hue = level / levels
            local cr = math.sin(hue * 6.28) * 0.5 + 0.5
            local cg = math.sin(hue * 6.28 + 2.0) * 0.5 + 0.5
            local cb = math.sin(hue * 6.28 + 4.0) * 0.5 + 0.5

            engine.add_mesh(box, "cube", {
                albedo = {cr, cg, cb, 1.0},
                metallic = 0.5,
                roughness = 0.3,
                ao = 1.0,
                emission = {cr * 0.3, cg * 0.3, cb * 0.3}
            })

            table.insert(Physics.bodies, {
                id = box,
                pos = {x = x, y = y, z = z},
                vel = {x = 0, y = 0, z = 0},
                rot = {x = 0, y = 0, z = 0},
                rotVel = {x = 0, y = 0, z = 0},
                mass = 1.0,
                restitution = 0.4
            })
        end
    end
end

function Physics.fireProjectile(origin, direction, speed)
    local proj = engine.create_entity("Phys_Projectile")
    engine.set_position(proj, origin.x, origin.y, origin.z)
    engine.set_scale(proj, 0.6, 0.6, 0.6)

    engine.add_mesh(proj, "sphere", {
        albedo = {1.0, 0.2, 0.8, 1.0},
        metallic = 0.9,
        roughness = 0.1,
        ao = 1.0,
        emission = {5.0, 1.0, 4.0}
    })

    table.insert(Physics.projectiles, {
        id = proj,
        pos = {x = origin.x, y = origin.y, z = origin.z},
        vel = {x = direction.x * speed, y = direction.y * speed, z = direction.z * speed},
        life = 6.0
    })
end

function Physics.update(dt)
    Physics.time = Physics.time + dt

    -- 1. Integrate Projectiles
    for i = #Physics.projectiles, 1, -1 do
        local p = Physics.projectiles[i]
        p.life = p.life - dt
        p.vel.y = p.vel.y + Physics.gravityY * dt * 0.5
        p.pos.x = p.pos.x + p.vel.x * dt
        p.pos.y = p.pos.y + p.vel.y * dt
        p.pos.z = p.pos.z + p.vel.z * dt

        engine.set_position(p.id, p.pos.x, p.pos.y, p.pos.z)

        -- Collision with floor
        if p.pos.y <= 0.3 then
            p.pos.y = 0.3
            p.vel.y = -p.vel.y * 0.6
            p.vel.x = p.vel.x * 0.8
            p.vel.z = p.vel.z * 0.8
        end

        -- Collision with boxes (impulse transfer)
        for _, b in ipairs(Physics.bodies) do
            local dx = b.pos.x - p.pos.x
            local dy = b.pos.y - p.pos.y
            local dz = b.pos.z - p.pos.z
            local dist = math.sqrt(dx*dx + dy*dy + dz*dz)
            if dist < 1.2 then
                -- Apply explosive kinetic blast
                local blastForce = 15.0
                local nx = dx / math.max(dist, 0.01)
                local ny = dy / math.max(dist, 0.01)
                local nz = dz / math.max(dist, 0.01)
                b.vel.x = b.vel.x + nx * blastForce
                b.vel.y = b.vel.y + (ny + 0.5) * blastForce
                b.vel.z = b.vel.z + nz * blastForce
                b.rotVel.x = (math.random() - 0.5) * 10.0
                b.rotVel.y = (math.random() - 0.5) * 10.0
                b.rotVel.z = (math.random() - 0.5) * 10.0
                p.life = 0
            end
        end

        if p.life <= 0 then
            engine.destroy_entity(p.id)
            table.remove(Physics.projectiles, i)
        end
    end

    -- 2. Integrate Physics Bodies (Velocity + Gravity + Ground Collision)
    for _, b in ipairs(Physics.bodies) do
        b.vel.y = b.vel.y + Physics.gravityY * dt
        b.pos.x = b.pos.x + b.vel.x * dt
        b.pos.y = b.pos.y + b.vel.y * dt
        b.pos.z = b.pos.z + b.vel.z * dt

        b.rot.x = b.rot.x + b.rotVel.x * dt
        b.rot.y = b.rot.y + b.rotVel.y * dt
        b.rot.z = b.rot.z + b.rotVel.z * dt

        -- Damping
        b.vel.x = b.vel.x * (1.0 - math.min(1.0, 1.2 * dt))
        b.vel.z = b.vel.z * (1.0 - math.min(1.0, 1.2 * dt))
        b.rotVel.x = b.rotVel.x * (1.0 - math.min(1.0, 2.0 * dt))
        b.rotVel.y = b.rotVel.y * (1.0 - math.min(1.0, 2.0 * dt))
        b.rotVel.z = b.rotVel.z * (1.0 - math.min(1.0, 2.0 * dt))

        -- Ground contact
        if b.pos.y < 0.5 then
            b.pos.y = 0.5
            if b.vel.y < -0.5 then
                b.vel.y = -b.vel.y * b.restitution
            else
                b.vel.y = 0
            end
        end

        engine.set_position(b.id, b.pos.x, b.pos.y, b.pos.z)
        engine.set_rotation(b.id, b.rot.x, b.rot.y, b.rot.z)
    end
end

return Physics
