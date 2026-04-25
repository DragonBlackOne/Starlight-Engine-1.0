-- Celestial Nexus: 3D Showcase Script
-- Starlight Engine Pure Showcase Mode

local Monolith = nil
local Crystals = {}
local LightEntities = {}
local Time = 0

function OnStart()
    print("Celestial Nexus: Awakening Core...")
    
    -- 1. Create the Sovereign Monolith (Center)
    Monolith = Entity.Create()
    local mt = Monolith:GetTransform()
    mt.position = vec3(0, 0, 0)
    mt.scale = vec3(1.5, 4.0, 1.5)
    
    local mm = Monolith:GetMesh()
    mm.color = vec3(0.1, 0.4, 0.8) -- Deep Ether Blue
    mm.metallic = 0.9
    mm.roughness = 0.1
    
    -- 2. Create the 8 Orbital Crystals
    for i = 1, 8 do
        local crystal = Entity.Create()
        local ct = crystal:GetTransform()
        local angle = (i-1) * (math.pi * 2 / 8)
        ct.position = vec3(math.cos(angle) * 6, math.sin(angle * 0.5) * 2, math.sin(angle) * 6)
        ct.scale = vec3(0.4, 0.8, 0.4)
        
        local cm = crystal:GetMesh()
        cm.color = vec3(0.0, 1.0, 1.0) -- Cyan Glow
        cm.metallic = 1.0
        cm.roughness = 0.05
        
        -- Add a light to each crystal (SUPER INTENSE)
        local light = Entity.Create()
        local lt = light:GetTransform()
        lt.position = ct.position
        local lc = light:GetLight()
        lc.color = vec3(0.0, 0.8, 1.0)
        lc.intensity = 500.0 -- Was 100.0, now much brighter
        
        Crystals[i] = crystal
        LightEntities[i] = light
    end

    -- 3. Position the Camera (Move back so we can see everything)
    local cam = Camera.GetPrimary()
    if cam then
        local ct = cam:GetTransform()
        ct.position = vec3(0, 5, 20) -- Move camera back and up
    end
end

function OnUpdate(dt)
    Time = Time + dt
    
    -- Rotate Monolith slowly
    if Monolith then
        local mt = Monolith:GetTransform()
        mt.rotation = mt.rotation * quat.euler(0, dt * 0.2, 0)
    end
    
    -- Orbit Crystals
    for i = 1, #Crystals do
        local crystal = Crystals[i]
        local light = LightEntities[i]
        local ct = crystal:GetTransform()
        local lt = light:GetTransform()
        
        local angle = ((i-1) * (math.pi * 2 / 8)) + (Time * 0.5)
        local radius = 7.0 + math.sin(Time + i) * 1.5
        
        local newPos = vec3(
            math.cos(angle) * radius,
            math.sin(Time * 0.7 + i) * 3.0,
            math.sin(angle) * radius
        )
        
        ct.position = newPos
        lt.position = newPos
        
        -- Correct rotation to face center
        ct.rotation = quat.euler(Time * 2, angle, 0)
    end
end
