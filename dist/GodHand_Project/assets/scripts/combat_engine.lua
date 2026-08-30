-- combat_engine.lua
-- 3D Combat, Hitbox Collision, Launch & Juggle Physics, Hit-stop & Frame Data for God Hand

local SFX = require("assets/scripts/sfx_manager")
local Difficulty = require("assets/scripts/difficulty_system")

local Combat = {
    hitStops = {},       -- Active hit-freeze timers per entity
    floatingTexts = {},  -- Impact popups ("CRITICAL!", "GUARD BREAK!", "GOD HAND!")
    sparks = {},
    comboCount = 0,
    comboTimer = 0.0
}

function Combat.init()
    Combat.hitStops = {}
    Combat.floatingTexts = {}
    Combat.sparks = {}
    Combat.comboCount = 0
    Combat.comboTimer = 0.0
end

-- Checks if a 3D defender is within an attacker's forward strike hitbox
function Combat.checkHit(attackerPos, attackerFacingAngle, attackDef, defenderPos, defenderRadius)
    local rad = math.rad(attackerFacingAngle)
    local fwdX = -math.sin(rad)
    local fwdZ = -math.cos(rad)
    local rightX = math.cos(rad)
    local rightZ = -math.sin(rad)

    local offset = attackDef.hitboxOffset or { x = 0, y = 1.0, z = 1.1 }
    local strikeX = attackerPos.x + fwdX * offset.z + rightX * offset.x
    local strikeY = attackerPos.y + offset.y
    local strikeZ = attackerPos.z + fwdZ * offset.z + rightZ * offset.x

    local dx = strikeX - defenderPos.x
    local dy = strikeY - (defenderPos.y + 1.0)
    local dz = strikeZ - defenderPos.z
    local distSq = dx * dx + dy * dy + dz * dz

    local hitRadius = (attackDef.hitboxRadius or 1.2) + (defenderRadius or 0.5)
    if distSq <= (hitRadius * hitRadius) then
        return true, strikeX, strikeY, strikeZ
    end
    return false, 0, 0, 0
end

function Combat.addHitStop(entityId, frames)
    Combat.hitStops[entityId] = frames
end

function Combat.isHitStopped(entityId)
    local f = Combat.hitStops[entityId]
    return f and f > 0
end

function Combat.addFloatingText(text, x, y, z, color, size)
    table.insert(Combat.floatingTexts, {
        text = text,
        x = x,
        y = y,
        z = z,
        vy = 1.6,
        color = color or { 1.0, 0.85, 0.1, 1.0 }, -- Gold Yellow
        size = size or 1.8,
        life = 0.9,
        maxLife = 0.9
    })
end

function Combat.spawnHitSparks(x, y, z, hitType, isGodHand)
    if isGodHand then
        -- Golden divine explosion
        vfx.burst(x, y, z, 1.0, 0.85, 0.15, 30, 7.0, 0.25, 0.7)
        vfx.burst(x, y, z, 1.0, 0.45, 0.05, 20, 5.0, 0.18, 0.5)
    elseif hitType == "guard_break" then
        -- Glass/armor shatter magenta/cyan burst
        vfx.burst(x, y, z, 1.0, 0.2, 0.95, 24, 6.0, 0.20, 0.5)
        vfx.burst(x, y, z, 0.0, 0.95, 0.95, 24, 6.0, 0.20, 0.5)
    elseif hitType == "launcher" or hitType == "heavy" or hitType == "special" then
        -- Heavy orange/red impact blast
        vfx.burst(x, y, z, 1.0, 0.45, 0.05, 22, 5.5, 0.20, 0.5)
        vfx.burst(x, y, z, 1.0, 0.90, 0.20, 16, 4.0, 0.15, 0.4)
    elseif hitType == "medium" then
        vfx.burst(x, y, z, 1.0, 0.75, 0.15, 14, 3.8, 0.14, 0.35)
    else
        -- Standard crisp punch spark
        vfx.burst(x, y, z, 1.0, 0.85, 0.20, 10, 3.2, 0.10, 0.3)
    end
end

-- Resolve attack hit between attacker and defender
function Combat.resolveAttack(attacker, defender, attackDef)
    if not defender or defender.state == "ko" or defender.isInvincible then
        return false
    end

    local isGodHand = attacker.isGodHand or false
    local isGuarding = (defender.state == "block") and not attackDef.isGuardBreak

    -- Check Defensive Guard
    if isGuarding and not isGodHand then
        defender.blockStun = attackDef.blockstun or 14
        defender.state = "block"
        if defender.animTree then
            local AnimationTree = require("assets/scripts/animation_tree")
            AnimationTree.play(defender.animTree, "reaction", "block", 1.0)
        end
        SFX.playPunchLight()
        Combat.addFloatingText("GUARD!", defender.pos.x, defender.pos.y + 2.0, defender.pos.z, { 0.4, 0.8, 1.0, 1.0 }, 1.3)
        vfx.burst(defender.pos.x, defender.pos.y + 1.2, defender.pos.z, 0.3, 0.7, 1.0, 10, 3.0, 0.12, 0.3)
        return false
    end

    -- Guard Break Triggered!
    if defender.state == "block" and (attackDef.isGuardBreak or isGodHand) then
        defender.state = "guard_broken"
        defender.stunFrames = 50 -- Extended opening for massive damage!
        if defender.animTree then
            local AnimationTree = require("assets/scripts/animation_tree")
            AnimationTree.play(defender.animTree, "reaction", "guard_broken", 1.0)
        end
        SFX.playGuardBreak()
        Combat.addFloatingText("GUARD BREAK!!", defender.pos.x, defender.pos.y + 2.2, defender.pos.z, { 1.0, 0.2, 0.95, 1.0 }, 2.4)
        Combat.spawnHitSparks(defender.pos.x, defender.pos.y + 1.3, defender.pos.z, "guard_break", isGodHand)
        Combat.addHitStop(attacker.id or 0, 7)
        Combat.addHitStop(defender.id or 1, 9)
        Difficulty.addHit("guard_break")
        return true
    end

    -- Calculate Scaled Damage
    local baseDamage = attackDef.damage or 20
    if isGodHand then baseDamage = baseDamage * 2.5 end
    if attacker.isPlayer then
        baseDamage = baseDamage * Difficulty.getScoreMultiplier()
    else
        baseDamage = baseDamage * Difficulty.getEnemyDamageMultiplier()
    end

    defender.hp = math.max(0, defender.hp - baseDamage)

    -- Knockback & Directional Impulses
    local rad = math.rad(attacker.facingAngle)
    local fwdX = -math.sin(rad)
    local fwdZ = -math.cos(rad)

    local knockback = attackDef.knockback or 2.2
    if isGodHand then knockback = knockback * 2.2 end
    defender.velocity.x = fwdX * knockback
    defender.velocity.z = fwdZ * knockback

    -- Camera Trauma Shake
    local Camera = require("assets/scripts/camera_controller")
    if isGodHand or attackDef.isHeavy or attackDef.isLauncher then
        Camera.addTrauma(0.35)
    else
        Camera.addTrauma(0.12)
    end

    -- Launch & Air Juggle Physics
    if attackDef.isLauncher or (isGodHand and attackDef.isHeavy) then
        defender.velocity.y = attackDef.launchHeight or 11.0
        defender.isGrounded = false
        defender.state = "launched"
        if defender.animTree then
            local AnimationTree = require("assets/scripts/animation_tree")
            AnimationTree.play(defender.animTree, "reaction", "launched", 1.2)
        end
        SFX.playLauncher()
        Combat.addFloatingText("LAUNCH!!", defender.pos.x, defender.pos.y + 2.2, defender.pos.z, { 1.0, 0.5, 0.1, 1.0 }, 2.2)
        Difficulty.addHit("launcher")
    elseif attackDef.isKnockdown then
        defender.velocity.y = 4.0
        defender.isGrounded = false
        defender.state = "knockdown"
        if defender.animTree then
            local AnimationTree = require("assets/scripts/animation_tree")
            AnimationTree.play(defender.animTree, "reaction", "knockdown", 1.0)
        end
        SFX.playPunchHeavy()
    else
        -- Hitstun Recoil
        defender.hitStun = attackDef.hitstun or 16
        defender.state = "hitstun"
        if defender.animTree then
            local AnimationTree = require("assets/scripts/animation_tree")
            AnimationTree.play(defender.animTree, "reaction", "hitstun", 1.2)
        end
        if attackDef.isHeavy then
            SFX.playPunchHeavy()
            Combat.addFloatingText("CRITICAL!", defender.pos.x, defender.pos.y + 2.0, defender.pos.z, { 1.0, 0.85, 0.1, 1.0 }, 2.0)
            Difficulty.addHit("heavy")
        elseif attackDef.isMedium then
            SFX.playPunchMedium()
            Difficulty.addHit("medium")
        else
            SFX.playPunchLight()
            Difficulty.addHit("light")
        end
    end

    -- Dizzy Meter Accumulation (100 = Dizzy Stagger!)
    defender.dizzyMeter = (defender.dizzyMeter or 0) + (attackDef.dizzyValue or 16)
    if defender.dizzyMeter >= 100 and defender.hp > 0 and defender.state ~= "launched" then
        defender.state = "dizzy"
        defender.dizzyTimer = 4.5 -- 4.5 seconds to pummel!
        defender.dizzyMeter = 0
        if defender.animTree then
            local AnimationTree = require("assets/scripts/animation_tree")
            AnimationTree.play(defender.animTree, "reaction", "dizzy", 1.0)
        end
        SFX.playDizzyBirds()
        Combat.addFloatingText("DIZZY! PUMMEL READY [J]!", defender.pos.x, defender.pos.y + 2.4, defender.pos.z, { 1.0, 0.9, 0.1, 1.0 }, 2.4)
    end

    -- Hit-Stop Freeze Frames
    local freezeFrames = attackDef.hitstop or 3
    if isGodHand then freezeFrames = freezeFrames + 5 end
    Combat.addHitStop(attacker.id or 0, freezeFrames)
    Combat.addHitStop(defender.id or 1, freezeFrames + 2)

    -- Hit Sparks
    Combat.spawnHitSparks(defender.pos.x, defender.pos.y + 1.2, defender.pos.z, attackDef.type or "light", isGodHand)

    -- Knockout Check
    if defender.hp <= 0 then
        defender.state = "ko"
        defender.koTimer = 3.5
        if defender.animTree then
            local AnimationTree = require("assets/scripts/animation_tree")
            AnimationTree.play(defender.animTree, "reaction", "knockdown", 1.0)
        end
        SFX.playPunchHeavy()
        Combat.addFloatingText("K.O.!!", defender.pos.x, defender.pos.y + 2.4, defender.pos.z, { 1.0, 0.1, 0.1, 1.0 }, 3.0)
        vfx.burst(defender.pos.x, defender.pos.y + 1.2, defender.pos.z, 1.0, 0.1, 0.1, 28, 6.0, 0.22, 0.8)
    end

    return true
end

function Combat.checkWallSplat(entity, arenaBounds)
    arenaBounds = arenaBounds or 16.5
    local bounced = false
    local speed = math.sqrt(entity.velocity.x * entity.velocity.x + entity.velocity.z * entity.velocity.z)

    if (math.abs(entity.pos.x) >= arenaBounds or math.abs(entity.pos.z) >= arenaBounds) and speed > 4.5 then
        -- Wall Splat Bounce Impact!
        entity.velocity.x = -entity.velocity.x * 0.4
        entity.velocity.z = -entity.velocity.z * 0.4
        entity.hp = math.max(0, entity.hp - 15)
        bounced = true

        SFX.playPunchHeavy()
        Combat.addFloatingText("WALL SPLAT!!", entity.pos.x, entity.pos.y + 2.0, entity.pos.z, { 1.0, 0.4, 0.1, 1.0 }, 2.2)
        vfx.burst(entity.pos.x, entity.pos.y + 1.2, entity.pos.z, 0.85, 0.7, 0.4, 18, 4.5, 0.18, 0.5)
        Combat.addHitStop(entity.id or 0, 6)
    end
    return bounced
end

function Combat.update(dt)
    -- Update Hit-Stops (decrements each frame)
    for id, frames in pairs(Combat.hitStops) do
        if frames > 0 then
            Combat.hitStops[id] = frames - 1
        end
    end

    -- Update Floating Texts
    for i = #Combat.floatingTexts, 1, -1 do
        local t = Combat.floatingTexts[i]
        t.life = t.life - dt
        t.y = t.y + t.vy * dt
        if t.life <= 0 then
            table.remove(Combat.floatingTexts, i)
        end
    end
end

return Combat
