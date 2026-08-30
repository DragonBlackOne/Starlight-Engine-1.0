-- ============================================================================
-- GodHand_Project — High-Fidelity 3D Character Rig & Anatomical Mesh Loader
-- ============================================================================

local HumanoidRig = {}

function HumanoidRig.create(name, charType, posX, posY, posZ, scale)
    scale = scale or 1.0
    charType = charType or "gene"

    local rig = {
        name = name,
        charType = charType,
        pos = { x = posX or 0, y = posY or 0, z = posZ or 0 },
        rotY = 0.0,
        scale = scale,
        leanAngle = 0.0,
        isGodHand = false,
        bobTimer = 0.0,
        parts = {},
        jointAngles = {
            spine = { pitch = 0, yaw = 0, roll = 0 },
            chest = { pitch = 0, yaw = 0, roll = 0 },
            neck = { pitch = 0, yaw = 0, roll = 0 },
            head = { pitch = 0, yaw = 0, roll = 0 },
            leftShoulder = { pitch = 0, yaw = 0, roll = 0 },
            leftArm = { pitch = 0, yaw = 0, roll = 0 },
            leftForearm = { pitch = 0, yaw = 0, roll = 0 },
            leftHand = { pitch = 0, yaw = 0, roll = 0 },
            rightShoulder = { pitch = 0, yaw = 0, roll = 0 },
            rightArm = { pitch = 0, yaw = 0, roll = 0 },
            rightForearm = { pitch = 0, yaw = 0, roll = 0 },
            rightHand = { pitch = 0, yaw = 0, roll = 0 },
            leftThigh = { pitch = 0, yaw = 0, roll = 0 },
            leftCalf = { pitch = 0, yaw = 0, roll = 0 },
            leftFoot = { pitch = 0, yaw = 0, roll = 0 },
            rightThigh = { pitch = 0, yaw = 0, roll = 0 },
            rightCalf = { pitch = 0, yaw = 0, roll = 0 },
            rightFoot = { pitch = 0, yaw = 0, roll = 0 }
        }
    }

    -- 1. Determine Model & Texture Paths
    local modelPath = "assets/models/gene_hd.obj"
    local texPath = "assets/textures/gene_atlas.jpg"
    local normPath = "assets/textures/gene_normal.tga"
    local albedoColor = { 1.0, 1.0, 1.0 }
    local metalness = 0.05
    local roughness = 0.55

    if charType == "punk" then
        modelPath = "assets/models/punk_hd.obj"
        texPath = "assets/textures/punk_atlas.jpg"
        normPath = "assets/textures/punk_normal.tga"
        albedoColor = { 0.95, 0.85, 0.85 }
    elseif charType == "bruiser" then
        scale = scale * 1.28
        rig.scale = scale
        modelPath = "assets/models/punk_hd.obj"
        texPath = "assets/textures/punk_atlas.jpg"
        normPath = "assets/textures/punk_normal.tga"
        albedoColor = { 0.70, 0.65, 0.60 }
        metalness = 0.20
        roughness = 0.70
    elseif charType == "boss" then
        scale = scale * 1.45
        rig.scale = scale
        modelPath = "assets/models/gene_hd.obj"
        texPath = "assets/textures/gene_atlas.jpg"
        normPath = "assets/textures/gene_normal.tga"
        albedoColor = { 0.85, 0.60, 0.95 }
        metalness = 0.40
        roughness = 0.40
    end

    -- 2. Spawn Unified Sculpted 3D Mesh Body
    local body = engine.spawn_model(name .. "_Body", modelPath, scale)
    engine.set_pos(body, rig.pos.x, rig.pos.y, rig.pos.z)
    engine.set_pbr(body, albedoColor[1], albedoColor[2], albedoColor[3], metalness, roughness)
    if engine.set_texture then engine.set_texture(body, texPath) end
    if engine.set_normal_map then engine.set_normal_map(body, normPath) end
    rig.parts.body = body

    -- 3. The God Hand: Armored Gold Gauntlet & Celestial Energy Aura
    if charType == "gene" then
        local gauntlet = engine.spawn_primitive(name .. "_Gauntlet", "cylinder", 0.095 * scale, 0.30 * scale, 10)
        engine.set_pbr(gauntlet, 1.00, 0.86, 0.22, 0.96, 0.08) -- Polished gold
        rig.parts.gauntlet = gauntlet

        local aura = engine.spawn_primitive(name .. "_Aura", "sphere", 0.16 * scale, 8, 12)
        engine.set_pbr(aura, 1.0, 0.92, 0.30, 0.98, 0.05) -- Mystic glowing aura
        rig.parts.aura = aura
    end

    return rig
end

function HumanoidRig.update(rig, dt)
    if not rig or not rig.parts.body then return end

    local px = rig.pos.x
    local py = rig.pos.y
    local pz = rig.pos.z
    local rot = rig.rotY or 0.0
    local lean = rig.leanAngle or 0.0
    local s = rig.scale or 1.0

    rig.bobTimer = (rig.bobTimer or 0.0) + dt

    -- 1. Update Primary 3D Body Mesh Transform
    engine.set_pos(rig.parts.body, px, py, pz)
    engine.set_rotation(rig.parts.body, 0, math.rad(rot), math.rad(lean))

    -- 2. Update God Hand Gauntlet & Energy Aura Position
    if rig.parts.gauntlet and rig.parts.aura then
        local rad = math.rad(rot)
        local rightX = math.cos(rad)
        local rightZ = -math.sin(rad)
        local fwdX = -math.sin(rad)
        local fwdZ = -math.cos(rad)

        -- Position on Gene's right forearm & fist
        local gx = px + rightX * (0.36 * s) + fwdX * (0.05 * s)
        local gy = py + (1.10 * s)
        local gz = pz + rightZ * (0.36 * s) + fwdZ * (0.05 * s)

        engine.set_pos(rig.parts.gauntlet, gx, gy, gz)
        engine.set_rotation(rig.parts.gauntlet, 0, math.rad(rot), 0)

        -- Pulsing celestial aura
        local pulse = (0.15 + math.sin(rig.bobTimer * 10.0) * 0.035) * s
        local fx = px + rightX * (0.38 * s) + fwdX * (0.10 * s)
        local fy = py + (0.96 * s)
        local fz = pz + rightZ * (0.38 * s) + fwdZ * (0.10 * s)

        engine.set_pos(rig.parts.aura, fx, fy, fz)
        if engine.set_scale then
            engine.set_scale(rig.parts.aura, pulse, pulse, pulse)
        end
    end
end

function HumanoidRig.updateTransform(rig)
    HumanoidRig.update(rig, 0.016)
end

function HumanoidRig.setGodHandMode(rig, active)
    if not rig then return end
    rig.isGodHand = active
    if rig.parts.aura then
        if active then
            engine.set_pbr(rig.parts.aura, 1.0, 0.95, 0.20, 0.99, 0.02)
        else
            engine.set_pbr(rig.parts.aura, 1.0, 0.90, 0.30, 0.95, 0.10)
        end
    end
end

function HumanoidRig.destroy(rig)
    if not rig or not rig.parts then return end
    for _, part in pairs(rig.parts) do
        if part and part ~= 0 then
            engine.destroy(part)
        end
    end
    rig.parts = {}
end

return HumanoidRig
