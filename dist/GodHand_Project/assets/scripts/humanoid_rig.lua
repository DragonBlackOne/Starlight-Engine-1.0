-- humanoid_rig.lua
-- Procedural Humanoid Character Rig with Joint Hierarchy, PBR Skin & Clothing Materials

local HumanoidRig = {}

function HumanoidRig.create(name, charType, posX, posY, posZ, scale)
    scale = scale or 1.0

    local rig = {
        name = name,
        charType = charType or "gene", -- "gene", "punk", "bruiser", "boss"
        pos = { x = posX or 0, y = posY or 0, z = posZ or 0 },
        rotY = 0.0,
        scale = scale,
        parts = {},
        jointAngles = {
            spine = { pitch = 0, yaw = 0, roll = 0 },
            head = { pitch = 0, yaw = 0, roll = 0 },
            leftArm = { pitch = 0, yaw = 0, roll = 0 },
            leftForearm = { pitch = 0, yaw = 0, roll = 0 },
            rightArm = { pitch = 0, yaw = 0, roll = 0 },
            rightForearm = { pitch = 0, yaw = 0, roll = 0 },
            leftThigh = { pitch = 0, yaw = 0, roll = 0 },
            leftCalf = { pitch = 0, yaw = 0, roll = 0 },
            rightThigh = { pitch = 0, yaw = 0, roll = 0 },
            rightCalf = { pitch = 0, yaw = 0, roll = 0 },
        }
    }

    -- 1. Determine Skin and Clothing Material Palettes based on Character Type
    local skinAlbedo = { 0.90, 0.72, 0.58 }     -- Natural human skin
    local skinRoughness = 0.42
    local clothAlbedo = { 0.15, 0.15, 0.18 }    -- Dark brawler vest
    local clothRoughness = 0.75
    local metalAlbedo = { 1.00, 0.85, 0.15 }    -- Polished Golden God Hand
    local metalRoughness = 0.22
    local pantsAlbedo = { 0.15, 0.22, 0.45 }    -- Denim blue jeans

    if charType == "gene" then
        skinAlbedo = { 0.92, 0.75, 0.62 }
        clothAlbedo = { 0.12, 0.12, 0.15 }      -- Black vest
        pantsAlbedo = { 0.16, 0.24, 0.48 }      -- Denim jeans
    elseif charType == "punk" then
        skinAlbedo = { 0.88, 0.65, 0.50 }
        clothAlbedo = { 0.85, 0.15, 0.15 }      -- Crimson biker vest
        pantsAlbedo = { 0.12, 0.12, 0.14 }      -- Dark leather pants
    elseif charType == "bruiser" then
        skinAlbedo = { 0.80, 0.60, 0.45 }
        clothAlbedo = { 0.35, 0.38, 0.42 }      -- Steel grey tank top
        pantsAlbedo = { 0.25, 0.28, 0.22 }      -- Camo olive trousers
    elseif charType == "boss" then
        skinAlbedo = { 0.95, 0.78, 0.65 }
        clothAlbedo = { 0.52, 0.10, 0.68 }      -- Imperial purple velvet
        pantsAlbedo = { 0.18, 0.08, 0.24 }      -- Dark violet trousers
    end

    -- 2. Spawn Humanoid Body Parts
    -- Torso / Chest
    local torso = engine.spawn_primitive(name .. "_Torso", "box", 0.50 * scale, 0.56 * scale, 0.30 * scale)
    engine.set_pbr(torso, clothAlbedo[1], clothAlbedo[2], clothAlbedo[3], 0.05, clothRoughness)
    rig.parts.torso = torso

    -- Pelvis / Belt
    local pelvis = engine.spawn_primitive(name .. "_Pelvis", "box", 0.42 * scale, 0.22 * scale, 0.26 * scale)
    engine.set_pbr(pelvis, pantsAlbedo[1], pantsAlbedo[2], pantsAlbedo[3], 0.05, 0.85)
    rig.parts.pelvis = pelvis

    -- Head
    local head = engine.spawn_primitive(name .. "_Head", "sphere", 0.18 * scale, 12, 16)
    if engine.set_pbr then engine.set_pbr(head, skinAlbedo[1], skinAlbedo[2], skinAlbedo[3], 0.02, skinRoughness) end
    if engine.set_skin then engine.set_skin(head, true, 0.80, 0.32, 0.22) end
    rig.parts.head = head

    -- Hair / Adornment
    if charType == "gene" then
        local hair = engine.spawn_primitive(name .. "_Hair", "wedge", 0.38 * scale, 0.22 * scale, 0.38 * scale)
        engine.set_pbr(hair, 0.32, 0.20, 0.10, 0.02, 0.92) -- Brown spiky hair
        rig.parts.hair = hair
    elseif charType == "punk" then
        local mohawk = engine.spawn_primitive(name .. "_Mohawk", "box", 0.10 * scale, 0.24 * scale, 0.36 * scale)
        engine.set_pbr(mohawk, 0.95, 0.15, 0.20, 0.05, 0.88) -- Bright red mohawk
        rig.parts.hair = mohawk
    elseif charType == "boss" then
        local crown = engine.spawn_primitive(name .. "_Crown", "cylinder", 0.19 * scale, 0.12 * scale, 8)
        engine.set_pbr(crown, 1.0, 0.85, 0.15, 0.85, 0.25) -- Gold circlet
        rig.parts.hair = crown
    end

    -- Left Shoulder & Arm
    local leftArm = engine.spawn_primitive(name .. "_LeftArm", "cylinder", 0.09 * scale, 0.34 * scale, 8)
    if engine.set_pbr then engine.set_pbr(leftArm, skinAlbedo[1], skinAlbedo[2], skinAlbedo[3], 0.02, skinRoughness) end
    if engine.set_skin then engine.set_skin(leftArm, true, 0.80, 0.32, 0.22) end
    rig.parts.leftArm = leftArm

    local leftForearm = engine.spawn_primitive(name .. "_LeftForearm", "cylinder", 0.08 * scale, 0.32 * scale, 8)
    if engine.set_pbr then engine.set_pbr(leftForearm, skinAlbedo[1], skinAlbedo[2], skinAlbedo[3], 0.02, skinRoughness) end
    if engine.set_skin then engine.set_skin(leftForearm, true, 0.80, 0.32, 0.22) end
    rig.parts.leftForearm = leftForearm

    -- Right Shoulder & Arm (The GOD HAND for Gene)
    local rightArm = engine.spawn_primitive(name .. "_RightArm", "cylinder", 0.09 * scale, 0.34 * scale, 8)
    if engine.set_pbr then engine.set_pbr(rightArm, skinAlbedo[1], skinAlbedo[2], skinAlbedo[3], 0.02, skinRoughness) end
    if engine.set_skin then engine.set_skin(rightArm, true, 0.80, 0.32, 0.22) end
    rig.parts.rightArm = rightArm

    local rightForearm = engine.spawn_primitive(name .. "_RightForearm", "cylinder", 0.095 * scale, 0.32 * scale, 10)
    if charType == "gene" then
        -- The Legendary God Hand Gauntlet (Polished Gold)
        if engine.set_pbr then engine.set_pbr(rightForearm, metalAlbedo[1], metalAlbedo[2], metalAlbedo[3], 0.85, metalRoughness) end
    elseif charType == "boss" then
        if engine.set_pbr then engine.set_pbr(rightForearm, 1.0, 0.85, 0.15, 0.80, 0.25) end -- Gold bracer
    else
        if engine.set_pbr then engine.set_pbr(rightForearm, skinAlbedo[1], skinAlbedo[2], skinAlbedo[3], 0.02, skinRoughness) end
        if engine.set_skin then engine.set_skin(rightForearm, true, 0.80, 0.32, 0.22) end
    end
    rig.parts.rightForearm = rightForearm

    -- Left Leg
    local leftThigh = engine.spawn_primitive(name .. "_LeftThigh", "cylinder", 0.10 * scale, 0.44 * scale, 8)
    engine.set_pbr(leftThigh, pantsAlbedo[1], pantsAlbedo[2], pantsAlbedo[3], 0.05, 0.85)
    rig.parts.leftThigh = leftThigh

    local leftCalf = engine.spawn_primitive(name .. "_LeftCalf", "cylinder", 0.09 * scale, 0.44 * scale, 8)
    engine.set_pbr(leftCalf, 0.12, 0.08, 0.05, 0.05, 0.85) -- Boots
    rig.parts.leftCalf = leftCalf

    -- Right Leg
    local rightThigh = engine.spawn_primitive(name .. "_RightThigh", "cylinder", 0.10 * scale, 0.44 * scale, 8)
    engine.set_pbr(rightThigh, pantsAlbedo[1], pantsAlbedo[2], pantsAlbedo[3], 0.05, 0.85)
    rig.parts.rightThigh = rightThigh

    local rightCalf = engine.spawn_primitive(name .. "_RightCalf", "cylinder", 0.09 * scale, 0.44 * scale, 8)
    engine.set_pbr(rightCalf, 0.12, 0.08, 0.05, 0.05, 0.85)
    rig.parts.rightCalf = rightCalf

    HumanoidRig.updateTransform(rig)
    return rig
end

function HumanoidRig.updateTransform(rig)
    if not rig or not rig.parts or not rig.parts.torso then return end

    local px = rig.pos.x
    local py = rig.pos.y
    local pz = rig.pos.z
    local rot = rig.rotY
    local s = rig.scale
    local rad = math.rad(rot)
    local cosR = math.cos(rad)
    local sinR = math.sin(rad)

    local function rotateLocal(lx, lz)
        return lx * cosR - lz * sinR, lx * sinR + lz * cosR
    end

    -- 1. Pelvis / Hips
    local rx, rz = rotateLocal(0, 0)
    engine.set_pos(rig.parts.pelvis, px + rx, py + 0.90 * s, pz + rz)
    engine.set_rotation(rig.parts.pelvis, 0, math.rad(rot), 0)

    -- 2. Torso
    rx, rz = rotateLocal(0, 0)
    engine.set_pos(rig.parts.torso, px + rx, py + 1.28 * s, pz + rz)
    engine.set_rotation(rig.parts.torso, 
        math.rad(rig.jointAngles.spine.pitch), 
        math.rad(rot + rig.jointAngles.spine.yaw), 
        math.rad(rig.jointAngles.spine.roll)
    )

    -- 3. Head & Hair
    rx, rz = rotateLocal(0, 0)
    local headY = py + 1.72 * s
    engine.set_pos(rig.parts.head, px + rx, headY, pz + rz)
    engine.set_rotation(rig.parts.head, 
        math.rad(rig.jointAngles.head.pitch), 
        math.rad(rot + rig.jointAngles.head.yaw), 
        math.rad(rig.jointAngles.head.roll)
    )

    if rig.parts.hair then
        engine.set_pos(rig.parts.hair, px + rx, headY + 0.16 * s, pz + rz)
        engine.set_rotation(rig.parts.hair, 
            math.rad(rig.jointAngles.head.pitch), 
            math.rad(rot + rig.jointAngles.head.yaw), 
            math.rad(rig.jointAngles.head.roll)
        )
    end

    -- 4. Left Arm & Forearm
    local laPitch = rig.jointAngles.leftArm.pitch
    local laRad = math.rad(laPitch)
    local lax, laz = rotateLocal(-0.34 * s, 0)
    engine.set_pos(rig.parts.leftArm, px + lax, py + 1.34 * s, pz + laz)
    engine.set_rotation(rig.parts.leftArm, math.rad(laPitch), math.rad(rot), 0)

    local lfx, lfz = rotateLocal(-0.34 * s, math.sin(laRad) * 0.30 * s)
    engine.set_pos(rig.parts.leftForearm, px + lfx, py + (1.34 - math.cos(laRad) * 0.30) * s, pz + lfz)
    engine.set_rotation(rig.parts.leftForearm, math.rad(laPitch + rig.jointAngles.leftForearm.pitch), math.rad(rot), 0)

    -- 5. Right Arm & Forearm
    local raPitch = rig.jointAngles.rightArm.pitch
    local raRad = math.rad(raPitch)
    local rax, raz = rotateLocal(0.34 * s, 0)
    engine.set_pos(rig.parts.rightArm, px + rax, py + 1.34 * s, pz + raz)
    engine.set_rotation(rig.parts.rightArm, math.rad(raPitch), math.rad(rot), 0)

    local rfx, rfz = rotateLocal(0.34 * s, math.sin(raRad) * 0.30 * s)
    engine.set_pos(rig.parts.rightForearm, px + rfx, py + (1.34 - math.cos(raRad) * 0.30) * s, pz + rfz)
    engine.set_rotation(rig.parts.rightForearm, math.rad(raPitch + rig.jointAngles.rightForearm.pitch), math.rad(rot), 0)

    -- 6. Left Leg
    local ltPitch = rig.jointAngles.leftThigh.pitch
    local ltRad = math.rad(ltPitch)
    local ltx, ltz = rotateLocal(-0.14 * s, 0)
    engine.set_pos(rig.parts.leftThigh, px + ltx, py + 0.65 * s, pz + ltz)
    engine.set_rotation(rig.parts.leftThigh, math.rad(ltPitch), math.rad(rot), 0)

    local lcx, lcz = rotateLocal(-0.14 * s, math.sin(ltRad) * 0.40 * s)
    engine.set_pos(rig.parts.leftCalf, px + lcx, py + (0.65 - math.cos(ltRad) * 0.40) * s, pz + lcz)
    engine.set_rotation(rig.parts.leftCalf, math.rad(ltPitch + rig.jointAngles.leftCalf.pitch), math.rad(rot), 0)

    -- 7. Right Leg
    local rtPitch = rig.jointAngles.rightThigh.pitch
    local rtRad = math.rad(rtPitch)
    local rtx, rtz = rotateLocal(0.14 * s, 0)
    engine.set_pos(rig.parts.rightThigh, px + rtx, py + 0.65 * s, pz + rtz)
    engine.set_rotation(rig.parts.rightThigh, math.rad(rtPitch), math.rad(rot), 0)

    local rcx, rcz = rotateLocal(0.14 * s, math.sin(rtRad) * 0.40 * s)
    engine.set_pos(rig.parts.rightCalf, px + rcx, py + (0.65 - math.cos(rtRad) * 0.40) * s, pz + rcz)
    engine.set_rotation(rig.parts.rightCalf, math.rad(rtPitch + rig.jointAngles.rightCalf.pitch), math.rad(rot), 0)
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
