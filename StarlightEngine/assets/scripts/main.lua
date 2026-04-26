-- Celestial Nexus: 3D Showcase Script (Migrated to SBA)
local Crystals = {}
local Time = 0

function OnStart()
    Say("Celestial Nexus: SBA Core Awakening...")
    
    -- 1. Sovereign Monolith
    Monolith = Object("Monolith", 0, 0, 0)
    SetScale(Monolith, 1.5, 4.0, 1.5)
    SetColor(Monolith, 0.1, 0.4, 0.8)
    
    -- 2. Orbital Crystals
    for i = 1, 8 do
        local angle = (i-1) * (math.pi * 2 / 8)
        local x = math.cos(angle) * 6
        local z = math.sin(angle) * 6
        
        local crystal = Object("Crystal", x, 2, z)
        SetScale(crystal, 0.4, 0.8, 0.4)
        SetColor(crystal, 0.0, 1.0, 1.0)
        
        -- Intense SBA Light
        SetLight(crystal, 0.0, 0.8, 1.0, 500)
        
        Crystals[i] = crystal
    end

    -- 3. Camera Position
    local cam = Engine:get_renderer():get_camera_transform()
    cam.position = vec3(0, 5, 20)
end

function OnUpdate(dt)
    Time = Time + dt
    
    if Monolith then
        Rotate(Monolith, 0, dt * 0.2, 0)
    end
    
    for i = 1, #Crystals do
        local crys = Crystals[i]
        if crys then
            local angle = ((i-1) * (math.pi * 2 / 8)) + (Time * 0.5)
            local radius = 7.0 + math.sin(Time + i) * 1.5
            local x = math.cos(angle) * radius
            local y = math.sin(Time * 0.7 + i) * 3.0
            local z = math.sin(angle) * radius
            
            SetPos(crys, x, y, z)
            Rotate(crys, dt * 2, 0, 0)
        end
    end
end
