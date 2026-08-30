-- difficulty_system.lua
-- God Hand Dynamic Difficulty Meter ("Level 1 -> Level 2 -> Level 3 -> Level DIE")

local Difficulty = {
    level = 1,          -- 1, 2, 3, 4 (4 = LEVEL DIE)
    gauge = 0.0,        -- 0.0 to 100.0 within the current level
    score = 0,
    maxLevel = 4,
    levelNames = { "LEVEL 1", "LEVEL 2", "LEVEL 3", "LEVEL DIE" },
    levelMultipliers = { 1.0, 1.5, 2.2, 3.5 },
    enemyDamageMults = { 0.75, 1.0, 1.35, 1.85 },
    enemyAggression = { 0.65, 1.05, 1.45, 2.10 },
    enemyBlockRate = { 0.15, 0.35, 0.55, 0.75 }
}

function Difficulty.init()
    Difficulty.level = 1
    Difficulty.gauge = 0.0
    Difficulty.score = 0
end

function Difficulty.addHit(hitType)
    local points = 4.0
    if hitType == "medium" then points = 7.0
    elseif hitType == "heavy" then points = 11.0
    elseif hitType == "guard_break" then points = 16.0
    elseif hitType == "launcher" then points = 15.0
    elseif hitType == "special" then points = 22.0
    elseif hitType == "dodge" then points = 14.0
    end

    Difficulty.score = Difficulty.score + math.floor(100 * Difficulty.getScoreMultiplier())
    Difficulty.gauge = Difficulty.gauge + points

    if Difficulty.gauge >= 100.0 then
        if Difficulty.level < Difficulty.maxLevel then
            Difficulty.level = Difficulty.level + 1
            Difficulty.gauge = 0.0

            local SFX = require("assets/scripts/sfx_manager")
            local Combat = require("assets/scripts/combat_engine")
            SFX.playLevelUp()

            if Difficulty.isLevelDie() then
                Combat.addFloatingText("☠ LEVEL DIE REACHED!! ☠", 0.0, 3.0, 0.0, { 1.0, 0.1, 0.1, 1.0 }, 3.5)
            else
                Combat.addFloatingText("★ DIFFICULTY UP: " .. Difficulty.getLevelName() .. " ★", 0.0, 2.8, 0.0, { 1.0, 0.85, 0.1, 1.0 }, 2.8)
            end
            return true
        else
            Difficulty.gauge = 100.0
        end
    end
    return false
end

function Difficulty.takeDamage(damage)
    local penalty = 24.0
    Difficulty.gauge = Difficulty.gauge - penalty

    if Difficulty.gauge < 0.0 then
        if Difficulty.level > 1 then
            Difficulty.level = Difficulty.level - 1
            Difficulty.gauge = 65.0
            local Combat = require("assets/scripts/combat_engine")
            Combat.addFloatingText("DIFFICULTY DOWN: " .. Difficulty.getLevelName(), 0.0, 2.5, 0.0, { 0.4, 0.8, 1.0, 1.0 }, 2.2)
        else
            Difficulty.gauge = 0.0
        end
    end
end

function Difficulty.grovelReset()
    Difficulty.level = 1
    Difficulty.gauge = 0.0
end

function Difficulty.getScoreMultiplier()
    return Difficulty.levelMultipliers[Difficulty.level] or 1.0
end

function Difficulty.getEnemyDamageMultiplier()
    return Difficulty.enemyDamageMults[Difficulty.level] or 1.0
end

function Difficulty.getEnemyAggression()
    return Difficulty.enemyAggression[Difficulty.level] or 1.0
end

function Difficulty.getEnemyBlockRate()
    return Difficulty.enemyBlockRate[Difficulty.level] or 0.2
end

function Difficulty.getLevelName()
    return Difficulty.levelNames[Difficulty.level] or "LEVEL 1"
end

function Difficulty.isLevelDie()
    return Difficulty.level >= 4
end

function Difficulty.update(dt)
    -- Slow idle decay of difficulty gauge
    if Difficulty.gauge > 0 then
        Difficulty.gauge = math.max(0.0, Difficulty.gauge - dt * 0.8)
    end
end

return Difficulty
