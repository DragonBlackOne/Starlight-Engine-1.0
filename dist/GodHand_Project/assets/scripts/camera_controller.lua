-- camera_controller.lua
-- 3D Over-the-shoulder Camera with Spring Arm, Trauma Screen Shake and God Reel Zoom

local Camera = {
    pos = { x = 0, y = 2.0, z = 5.0 },
    targetPos = { x = 0, y = 1.0, z = 0 },
    yaw = 0.0,          -- in degrees
    pitch = 8.0,        -- slight downward angle
    distance = 3.6,     -- default distance behind player
    shoulderOffset = 0.55, -- over right shoulder
    heightOffset = 1.65,
    trauma = 0.0,       -- Screen shake trauma (0.0 to 1.0)
    fov = 65.0,
    zoomFactor = 1.0
}

function Camera.init()
    Camera.pos = { x = 0, y = 2.0, z = 5.0 }
    Camera.yaw = 0.0
    Camera.pitch = 8.0
    Camera.distance = 3.6
    Camera.trauma = 0.0
    Camera.zoomFactor = 1.0
end

function Camera.addTrauma(amount)
    Camera.trauma = math.min(1.0, Camera.trauma + amount)
end

function Camera.update(dt, playerPos, playerFacingAngle, isGodReel)
    -- Zoom control during God Reel or Finishers
    if isGodReel then
        Camera.zoomFactor = math.max(0.65, Camera.zoomFactor - dt * 2.5) -- Zoom closer
        Camera.pitch = 4.0
    else
        Camera.zoomFactor = math.min(1.0, Camera.zoomFactor + dt * 3.0)
        Camera.pitch = 8.0
    end

    local currentDist = Camera.distance * Camera.zoomFactor

    -- Calculate camera target focus point on player
    local rad = math.rad(playerFacingAngle)
    local fwdX = -math.sin(rad)
    local fwdZ = -math.cos(rad)
    local rightX = math.cos(rad)
    local rightZ = -math.sin(rad)

    -- Player head/chest center
    local focusX = playerPos.x + fwdX * 0.8
    local focusY = playerPos.y + 1.25
    local focusZ = playerPos.z + fwdZ * 0.8

    -- Desired camera position behind right shoulder
    local idealCamX = playerPos.x - fwdX * currentDist + rightX * Camera.shoulderOffset
    local idealCamY = playerPos.y + Camera.heightOffset + (Camera.pitch * 0.05)
    local idealCamZ = playerPos.z - fwdZ * currentDist + rightZ * Camera.shoulderOffset

    -- Smooth follow interpolation
    local lerpRate = math.min(1.0, dt * 14.0)
    Camera.pos.x = Camera.pos.x + (idealCamX - Camera.pos.x) * lerpRate
    Camera.pos.y = Camera.pos.y + (idealCamY - Camera.pos.y) * lerpRate
    Camera.pos.z = Camera.pos.z + (idealCamZ - Camera.pos.z) * lerpRate

    Camera.targetPos.x = Camera.targetPos.x + (focusX - Camera.targetPos.x) * lerpRate
    Camera.targetPos.y = Camera.targetPos.y + (focusY - Camera.targetPos.y) * lerpRate
    Camera.targetPos.z = Camera.targetPos.z + (focusZ - Camera.targetPos.z) * lerpRate

    -- Trauma Screen Shake
    local shakeX = 0
    local shakeY = 0
    if Camera.trauma > 0 then
        local shakeMag = Camera.trauma * Camera.trauma * 0.35
        shakeX = (math.random() * 2.0 - 1.0) * shakeMag
        shakeY = (math.random() * 2.0 - 1.0) * shakeMag
        Camera.trauma = math.max(0, Camera.trauma - dt * 2.2)
    end

    -- Apply to Engine Camera
    camera.set_pos(Camera.pos.x + shakeX, Camera.pos.y + shakeY, Camera.pos.z)
    camera.look_at(Camera.targetPos.x, Camera.targetPos.y, Camera.targetPos.z)
end

return Camera
