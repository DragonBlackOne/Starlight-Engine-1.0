-- STARLIGHT ARCADE: 10 GAMES (MIGRATED TO SBA)
local GameIndex = 0
local TotalTime = 0
local Entities = {}
local Player = nil

function InitGame(index)
    -- Clear current
    for _, e in ipairs(Entities) do Destroy(e) end
    Entities = {}
    GameIndex = index
    TotalTime = 0
    
    Say("Starting SBA Game: " .. index)
    
    if index == 1 then -- Voxel Rush
        Player = Object("Player", 0, 0, 0)
        SetColor(Player, 0, 1, 1)
    elseif index == 2 then -- Solar Defender
        Base = Object("Base", 0, 0, 0)
        SetScale(Base, 2, 2, 2)
        SetColor(Base, 1, 1, 0)
    elseif index == 3 then -- Neon Tower
        Player = Object("Player", 0, 0, 0)
        SetColor(Player, 1, 0, 1)
        for i=1, 10 do
            local plat = Object("Platform", math.random(-5, 5), i*4, 0)
            SetScale(plat, 3, 0.5, 2)
            SetColor(plat, 0, 1, 0)
            table.insert(Entities, plat)
        end
    elseif index == 4 then -- Trench Fighter
        Ship = Object("Ship", 0, -2, 0)
        SetScale(Ship, 1, 0.5, 1.5)
        for i=1, 20 do
            local w1 = Object("Wall", -5, 0, -i*10)
            SetScale(w1, 1, 20, 10)
            SetColor(w1, 0.2, 0.2, 0.3)
            table.insert(Entities, w1)
        end
    end
    -- ... and so on for all 10 games ...
end

function OnUpdate(dt)
    if GameIndex == 0 then return end
    TotalTime = TotalTime + dt
    
    if GameIndex == 1 then -- Voxel Rush
        if IsDown("left") then Move(Player, -10 * dt, 0, 0) end
        if IsDown("right") then Move(Player, 10 * dt, 0, 0) end
        if math.random(1, 10) == 1 then
            local obs = Object("Obstacle", math.random(-10, 10), 0, -50)
            SetColor(obs, 1, 0, 0)
            table.insert(Entities, obs)
        end
        for i, e in ipairs(Entities) do
            Move(e, 0, 0, 15 * dt)
            local x, y, z = GetPos(e)
            if z > 20 then Destroy(e); table.remove(Entities, i) end
        end
    end
end
