-- main.lua
-- Next-Gen Architectural Showcase for Fusion ENGINE

package.path = package.path .. ";assets/scripts/?.lua;./assets/scripts/?.lua"
engine = engine or Engine
Engine = Engine or engine

local PBR = require("assets/scripts/pbr_showcase")
local Physics = require("assets/scripts/physics_showcase")
local Hierarchy = require("assets/scripts/hierarchy_showcase")
local UI = require("assets/scripts/tech_demo_ui")

local TechDemo = {
    camera = {
        pos = {x = 0.0, y = 6.0, z = 16.0},
        target = {x = 0.0, y = 3.0, z = -5.0},
        pitch = -12.0,
        yaw = 0.0,
        speed = 12.0
    },
    showroomIndex = 1,
    fpsHistory = {},
    fps = 60.0,
    time = 0.0
}

function OnStart()
    engine.log("TechDemo: Initializing all architectural showcases...")

    -- 1. Initialize Showroom Subsystems
    PBR.init()
    Physics.init()
    Hierarchy.init()

    -- 2. Initial Camera Setup
    TechDemo.setCameraPreset(1)
end

function TechDemo.setCameraPreset(index)
    TechDemo.showroomIndex = index
    if index == 1 then
        -- PBR Grid View
        TechDemo.camera.pos = {x = 0.0, y = 7.5, z = 15.0}
        TechDemo.camera.target = {x = 0.0, y = 4.0, z = -5.0}
    elseif index == 2 then
        -- 2D Particle Waves View
        TechDemo.camera.pos = {x = 0.0, y = 14.0, z = 6.0}
        TechDemo.camera.target = {x = 0.0, y = 0.0, z = -5.0}
    elseif index == 3 then
        -- Physics Arena View
        TechDemo.camera.pos = {x = 12.0, y = 6.0, z = 12.0}
        TechDemo.camera.target = {x = 12.0, y = 2.5, z = -5.0}
    elseif index == 4 then
        -- Solar Hierarchy View
        TechDemo.camera.pos = {x = -18.0, y = 9.0, z = 14.0}
        TechDemo.camera.target = {x = -18.0, y = 4.0, z = -5.0}
    elseif index == 5 then
        -- Free Fly Camera Mode
        TechDemo.camera.pos = {x = 0.0, y = 12.0, z = 22.0}
        TechDemo.camera.target = {x = 0.0, y = 3.0, z = -5.0}
    end

    engine.set_camera_position(TechDemo.camera.pos.x, TechDemo.camera.pos.y, TechDemo.camera.pos.z)
    engine.set_camera_target(TechDemo.camera.target.x, TechDemo.camera.target.y, TechDemo.camera.target.z)
end

function OnUpdate(dt)
    TechDemo.time = TechDemo.time + dt
    TechDemo.fps = 1.0 / math.max(dt, 0.001)

    -- 1. Showroom Key Switching (1 - 5)
    if input.is_key_pressed("1") or input.is_key_pressed("NUMPAD_1") then
        TechDemo.setCameraPreset(1)
    elseif input.is_key_pressed("2") or input.is_key_pressed("NUMPAD_2") then
        TechDemo.setCameraPreset(2)
    elseif input.is_key_pressed("3") or input.is_key_pressed("NUMPAD_3") then
        TechDemo.setCameraPreset(3)
    elseif input.is_key_pressed("4") or input.is_key_pressed("NUMPAD_4") then
        TechDemo.setCameraPreset(4)
    elseif input.is_key_pressed("5") or input.is_key_pressed("NUMPAD_5") then
        TechDemo.setCameraPreset(5)
    end

    -- 2. Kinetic Projectile Fire (F Key)
    if input.is_key_pressed("F") then
        local camPos = TechDemo.camera.pos
        local camTarget = TechDemo.camera.target
        local dirX = camTarget.x - camPos.x
        local dirY = camTarget.y - camPos.y
        local dirZ = camTarget.z - camPos.z
        local len = math.sqrt(dirX*dirX + dirY*dirY + dirZ*dirZ)
        if len > 0.001 then
            dirX = dirX / len
            dirY = dirY / len
            dirZ = dirZ / len
        end
        Physics.fireProjectile(camPos, {x = dirX, y = dirY, z = dirZ}, 24.0)
    end

    -- 3. Spawn Pyramid (G Key)
    if input.is_key_pressed("G") then
        Physics.spawnPyramid(12.0 + (math.random() - 0.5) * 4.0, 0.0, -5.0 + (math.random() - 0.5) * 4.0, 4)
    end

    -- 4. Reverse Gravity (Space Key)
    if input.is_key_pressed("SPACE") then
        Physics.gravityY = -Physics.gravityY
    end

    -- 5. Fly Camera Navigation (W, A, S, D, Q, E)
    local moveSpeed = TechDemo.camera.speed * dt
    if input.is_key_down("W") then
        TechDemo.camera.pos.z = TechDemo.camera.pos.z - moveSpeed
        TechDemo.camera.target.z = TechDemo.camera.target.z - moveSpeed
    end
    if input.is_key_down("S") then
        TechDemo.camera.pos.z = TechDemo.camera.pos.z + moveSpeed
        TechDemo.camera.target.z = TechDemo.camera.target.z + moveSpeed
    end
    if input.is_key_down("A") then
        TechDemo.camera.pos.x = TechDemo.camera.pos.x - moveSpeed
        TechDemo.camera.target.x = TechDemo.camera.target.x - moveSpeed
    end
    if input.is_key_down("D") then
        TechDemo.camera.pos.x = TechDemo.camera.pos.x + moveSpeed
        TechDemo.camera.target.x = TechDemo.camera.target.x + moveSpeed
    end
    if input.is_key_down("E") then
        TechDemo.camera.pos.y = TechDemo.camera.pos.y + moveSpeed
        TechDemo.camera.target.y = TechDemo.camera.target.y + moveSpeed
    end
    if input.is_key_down("Q") then
        TechDemo.camera.pos.y = TechDemo.camera.pos.y - moveSpeed
        TechDemo.camera.target.y = TechDemo.camera.target.y - moveSpeed
    end

    engine.set_camera_position(TechDemo.camera.pos.x, TechDemo.camera.pos.y, TechDemo.camera.pos.z)
    engine.set_camera_target(TechDemo.camera.target.x, TechDemo.camera.target.y, TechDemo.camera.target.z)

    -- 6. Update Subsystem Showcases
    PBR.update(dt)
    Physics.update(dt)
    Hierarchy.update(dt)
end

function OnRender()
    -- 3D Mesh Components rendered automatically by StarlightCore pipeline
end

function OnRenderUI()
    UI.draw(
        TechDemo.fps,
        1.0 / math.max(TechDemo.fps, 1.0),
        TechDemo.showroomIndex,
        TechDemo.camera.pos,
        #PBR.lights,
        #Physics.bodies + #Physics.projectiles
    )
end

OnUIRender = OnRenderUI
