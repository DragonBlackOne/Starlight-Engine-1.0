-- camera_controller.lua
-- 360-Degree Spherical Third-Person Orbit Camera with Mouse Look, Zoom, Spring-Arm & Trauma Shake

local Camera = {
    pos = { x = 0.0, y = 1.8, z = 4.0 },
    targetPos = { x = 0.0, y = 1.30, z = 0.0 },
    yaw = 0.0,             -- horizontal angle in degrees (0 = looking forward down -Z)
    pitch = 8.0,           -- horizontal over-the-shoulder angle
    distance = 3.2,        -- cinematic third person distance
    targetDistance = 3.2,  -- user zoom setting
    minDistance = 1.8,
    maxDistance = 6.5,
    minPitch = -20.0,
    maxPitch = 32.0,       -- clamp pitch to prevent top-down view
    shoulderOffset = 0.28, -- over-the-shoulder offset
    heightOffset = 1.30,   -- focus height at chest level
    trauma = 0.0,          -- Screen shake trauma
    zoomFactor = 1.0,
    prevMouseX = 0,
    prevMouseY = 0,
    firstMouse = true,
    firstFrame = true
}

function Camera.init()
    Camera.pos = { x = 0.0, y = 1.8, z = 4.0 }
    Camera.targetPos = { x = 0.0, y = 1.30, z = 0.0 }
    Camera.yaw = 0.0
    Camera.pitch = 8.0
    Camera.distance = 3.2
    Camera.targetDistance = 3.2
    Camera.trauma = 0.0
    Camera.zoomFactor = 1.0
    Camera.firstMouse = true
    Camera.firstFrame = true
end

function Camera.addTrauma(amount)
    Camera.trauma = math.min(1.0, Camera.trauma + amount)
end

function Camera.update(dt, playerPos, playerFacingAngle, isGodReel)
    -- 1. Mouse Look & Orbit Control
    local mx = input.get_mouse_x() or 0
    local my = input.get_mouse_y() or 0

    if Camera.firstMouse then
        Camera.prevMouseX = mx
        Camera.prevMouseY = my
        Camera.firstMouse = false
    end

    local deltaX = mx - Camera.prevMouseX
    local deltaY = my - Camera.prevMouseY
    Camera.prevMouseX = mx
    Camera.prevMouseY = my

    -- Orbit when moving mouse
    local mouseSens = 0.22
    if math.abs(deltaX) > 0.01 or math.abs(deltaY) > 0.01 then
        Camera.yaw = (Camera.yaw - deltaX * mouseSens) % 360.0
        Camera.pitch = math.max(Camera.minPitch, math.min(Camera.maxPitch, Camera.pitch + deltaY * mouseSens))
    end

    -- Arrow keys / Controller alternative for camera orbit
    local camSpeed = 120.0
    if input.is_down("left") and not input.is_down("lshift") and not input.is_down("shift") then
        Camera.yaw = (Camera.yaw + camSpeed * dt) % 360.0
    elseif input.is_down("right") and not input.is_down("lshift") and not input.is_down("shift") then
        Camera.yaw = (Camera.yaw - camSpeed * dt) % 360.0
    end
    if input.is_down("up") and input.is_down("rctrl") then
        Camera.pitch = math.min(Camera.maxPitch, Camera.pitch + camSpeed * 0.6 * dt)
    elseif input.is_down("down") and input.is_down("rctrl") then
        Camera.pitch = math.max(Camera.minPitch, Camera.pitch - camSpeed * 0.6 * dt)
    end

    -- 2. Mouse Wheel Zoom Control
    local scroll = input.get_scroll() or 0
    if scroll ~= 0 then
        Camera.targetDistance = math.max(Camera.minDistance, math.min(Camera.maxDistance, Camera.targetDistance - scroll * 0.65))
    end
    Camera.distance = Camera.distance + (Camera.targetDistance - Camera.distance) * math.min(1.0, dt * 10.0)

    -- 3. Dynamic Zoom for God Reel & Finishers
    if isGodReel then
        Camera.zoomFactor = math.max(0.60, Camera.zoomFactor - dt * 3.0)
    else
        Camera.zoomFactor = math.min(1.0, Camera.zoomFactor + dt * 3.0)
    end

    local currentDist = Camera.distance * Camera.zoomFactor

    -- 4. Calculate 3D Orbit Position
    local yawRad = math.rad(Camera.yaw)
    local pitchRad = math.rad(Camera.pitch)

    local cosPitch = math.cos(pitchRad)
    local sinPitch = math.sin(pitchRad)
    local cosYaw = math.cos(yawRad)
    local sinYaw = math.sin(yawRad)

    -- Forward vector pointing FROM camera TO target
    local fwdX = -sinYaw * cosPitch
    local fwdY = -sinPitch
    local fwdZ = -cosYaw * cosPitch

    -- Right vector
    local rightX = cosYaw
    local rightZ = -sinYaw

    -- Focus target on player's chest
    local focusX = playerPos.x
    local focusY = playerPos.y + Camera.heightOffset
    local focusZ = playerPos.z

    -- Desired Camera Position in spherical coordinates behind focus point
    local idealCamX = focusX - fwdX * currentDist + rightX * Camera.shoulderOffset
    local idealCamY = focusY - fwdY * currentDist
    local idealCamZ = focusZ - fwdZ * currentDist + rightZ * Camera.shoulderOffset

    -- Keep camera above ground
    idealCamY = math.max(0.45, idealCamY)

    -- 5. Smooth Spring-Arm Interpolation (or Snap on First Frame)
    if Camera.firstFrame then
        Camera.pos.x = idealCamX
        Camera.pos.y = idealCamY
        Camera.pos.z = idealCamZ
        Camera.targetPos.x = focusX
        Camera.targetPos.y = focusY
        Camera.targetPos.z = focusZ
        Camera.firstFrame = false
    else
        local lerpRate = math.min(1.0, dt * 16.0)
        Camera.pos.x = Camera.pos.x + (idealCamX - Camera.pos.x) * lerpRate
        Camera.pos.y = Camera.pos.y + (idealCamY - Camera.pos.y) * lerpRate
        Camera.pos.z = Camera.pos.z + (idealCamZ - Camera.pos.z) * lerpRate

        Camera.targetPos.x = Camera.targetPos.x + (focusX - Camera.targetPos.x) * lerpRate
        Camera.targetPos.y = Camera.targetPos.y + (focusY - Camera.targetPos.y) * lerpRate
        Camera.targetPos.z = Camera.targetPos.z + (focusZ - Camera.targetPos.z) * lerpRate
    end

    -- 6. Trauma Screen Shake
    local shakeX = 0
    local shakeY = 0
    if Camera.trauma > 0 then
        local shakeMag = Camera.trauma * Camera.trauma * 0.40
        shakeX = (math.random() * 2.0 - 1.0) * shakeMag
        shakeY = (math.random() * 2.0 - 1.0) * shakeMag
        Camera.trauma = math.max(0, Camera.trauma - dt * 2.5)
    end

    -- Apply to Engine Camera
    camera.set_pos(Camera.pos.x + shakeX, Camera.pos.y + shakeY, Camera.pos.z)
    camera.look_at(Camera.targetPos.x, Camera.targetPos.y, Camera.targetPos.z)
end

function Camera.getYaw()
    return Camera.yaw
end

return Camera
