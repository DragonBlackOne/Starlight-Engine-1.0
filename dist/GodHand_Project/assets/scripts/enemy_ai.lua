-- enemy_ai.lua
-- 3D Brawler Enemy AI Archetypes (Thug Punks, Heavy Bruisers, Mad Midget Boss)

local SFX = require("assets/scripts/sfx_manager")
local Combat = require("assets/scripts/combat_engine")
local Difficulty = require("assets/scripts/difficulty_system")
local HumanoidRig = require("assets/scripts/humanoid_rig")
local AnimationTree = require("assets/scripts/animation_tree")

local EnemyAI = {
    enemies = {},
    nextId = 100
}

function EnemyAI.init()
    for _, e in ipairs(EnemyAI.enemies) do
        if e.rig then
            HumanoidRig.destroy(e.rig)
        end
    end
    EnemyAI.enemies = {}
    EnemyAI.nextId = 100
end

function EnemyAI.spawn(type, x, y, z)
    EnemyAI.nextId = EnemyAI.nextId + 1
    local id = EnemyAI.nextId

    local e = {
        id = id,
        type = type or "punk",
        pos = { x = x or 0.0, y = y or 0.0, z = z or 0.0 },
        velocity = { x = 0.0, y = 0.0, z = 0.0 },
        facingAngle = 0.0,
        radius = 0.5,
        hp = 110,
        maxHp = 110,
        isGrounded = true,
        state = "idle",
        stateTimer = 0.0,
        attackTimer = math.random() * 2.0,
        blockStun = 0,
        hitStun = 0,
        dizzyMeter = 0,
        dizzyTimer = 0,
        isInvincible = false,
        rig = nil,
        animTree = nil
    }

    local scale = 1.0

    -- Configure Type Specifics
    if type == "bruiser" then
        e.hp = 260
        e.maxHp = 260
        e.radius = 0.75
        e.speed = 3.4
        e.attackRange = 1.9
        e.attackDef = { name = "BRUISER SMASH", anim = "straight", startup = 0.38, active = 0.20, recovery = 0.48, damage = 38, hitboxRadius = 1.6, isHeavy = true, knockback = 4.2 }
        scale = 1.25
    elseif type == "boss" then
        e.hp = 550
        e.maxHp = 550
        e.radius = 0.60
        e.speed = 5.8
        e.attackRange = 1.8
        e.attackDef = { name = "IRON TORNADO", anim = "high_kick", startup = 0.18, active = 0.18, recovery = 0.30, damage = 45, hitboxRadius = 1.7, isLauncher = true, isHeavy = true, knockback = 5.5 }
        scale = 1.05
    else
        -- Standard Thug Punk
        e.hp = 120
        e.maxHp = 120
        e.radius = 0.48
        e.speed = 4.4
        e.attackRange = 1.4
        e.attackDef = { name = "PUNK PUNCH", anim = "jab", startup = 0.22, active = 0.16, recovery = 0.32, damage = 20, hitboxRadius = 1.2, knockback = 1.8 }
        scale = 1.0
    end

    -- Create Procedural Rig & Animation State Machine
    e.rig = HumanoidRig.create(type .. "_" .. id, type, e.pos.x, e.pos.y, e.pos.z, scale)
    e.animTree = AnimationTree.create()
    AnimationTree.play(e.animTree, "idle", "none", 1.0)

    table.insert(EnemyAI.enemies, e)
    return e
end

function EnemyAI.update(dt, player)
    local aggression = Difficulty.getEnemyAggression()
    local blockRate = Difficulty.getEnemyBlockRate()

    for i = #EnemyAI.enemies, 1, -1 do
        local e = EnemyAI.enemies[i]
        if e.hp <= 0 and e.state == "ko" then
            e.koTimer = (e.koTimer or 3.5) - dt
            if e.koTimer <= 0 then
                -- Destroy rig and remove enemy
                if e.rig then
                    HumanoidRig.destroy(e.rig)
                end
                table.remove(EnemyAI.enemies, i)
            else
                -- Update KO tumble transforms
                if e.rig then
                    e.rig.pos.x = e.pos.x
                    e.rig.pos.y = e.pos.y
                    e.rig.pos.z = e.pos.z
                    AnimationTree.update(e.animTree, dt, e.rig)
                    HumanoidRig.updateTransform(e.rig)
                end
            end
        else
            EnemyAI.updateEnemy(e, dt, player, aggression, blockRate)
        end
    end
end

function EnemyAI.updateEnemy(e, dt, player, aggression, blockRate)
    if Combat.isHitStopped(e.id) then return end

    -- Vector to Player
    local dx = player.pos.x - e.pos.x
    local dz = player.pos.z - e.pos.z
    local dist = math.sqrt(dx * dx + dz * dz)

    -- Update Facing Angle toward Player (when standing)
    if e.state ~= "launched" and e.state ~= "knockdown" and e.state ~= "ko" then
        e.facingAngle = math.deg(math.atan(dx, dz))
    end

    -- AI State Machine
    if e.state == "idle" or e.state == "advance" then
        e.attackTimer = e.attackTimer - dt * aggression

        if dist > e.attackRange then
            -- Walk/Chase towards player
            local rad = math.rad(e.facingAngle)
            local fwdX = math.sin(rad)
            local fwdZ = math.cos(rad)
            e.velocity.x = fwdX * e.speed
            e.velocity.z = fwdZ * e.speed
            if e.state ~= "advance" then
                e.state = "advance"
                AnimationTree.play(e.animTree, "run", "none", 1.0)
            end
        else
            -- In strike range
            e.velocity.x = 0
            e.velocity.z = 0
            if e.state ~= "idle" then
                e.state = "idle"
                AnimationTree.play(e.animTree, "idle", "none", 1.0)
            end

            -- Attack or Defensive Block decision
            if e.attackTimer <= 0 then
                if math.random() < blockRate and player.state == "attack" then
                    -- Defensive Guard
                    e.state = "block"
                    e.blockTimer = 0.75
                    AnimationTree.play(e.animTree, "reaction", "block", 1.0)
                else
                    -- Execute Strike
                    e.state = "attack"
                    e.attackPhase = "startup"
                    e.startupTimer = e.attackDef.startup / aggression
                    e.activeTimer = e.attackDef.active
                    e.recoveryTimer = e.attackDef.recovery
                    e.attackHasHit = false
                    e.attackTimer = (math.random() * 1.6 + 0.8) / aggression
                    AnimationTree.play(e.animTree, "attack", e.attackDef.anim or "jab", 1.0)
                end
            end
        end
    elseif e.state == "block" then
        e.blockTimer = (e.blockTimer or 0.75) - dt
        if e.blockTimer <= 0 then
            e.state = "idle"
            AnimationTree.play(e.animTree, "idle", "none", 1.0)
        end
    elseif e.state == "attack" then
        if e.attackPhase == "startup" then
            e.startupTimer = e.startupTimer - dt
            if e.startupTimer <= 0 then e.attackPhase = "active" end
        elseif e.attackPhase == "active" then
            e.activeTimer = e.activeTimer - dt
            if not e.attackHasHit then
                local hit = Combat.checkHit(e.pos, e.facingAngle, e.attackDef, player.pos, player.radius)
                if hit then
                    Combat.resolveAttack(e, player, e.attackDef)
                    e.attackHasHit = true
                    Difficulty.takeDamage(e.attackDef.damage)
                end
            end
            if e.activeTimer <= 0 then e.attackPhase = "recovery" end
        elseif e.attackPhase == "recovery" then
            e.recoveryTimer = e.recoveryTimer - dt
            if e.recoveryTimer <= 0 then
                e.state = "idle"
                AnimationTree.play(e.animTree, "idle", "none", 1.0)
            end
        end
    elseif e.state == "guard_broken" then
        e.stunFrames = (e.stunFrames or 35) - 1
        if e.stunFrames <= 0 then
            e.state = "idle"
            AnimationTree.play(e.animTree, "idle", "none", 1.0)
        end
    elseif e.state == "hitstun" then
        e.hitStun = (e.hitStun or 14) - 1
        if e.hitStun <= 0 then
            e.state = "idle"
            AnimationTree.play(e.animTree, "idle", "none", 1.0)
        end
    elseif e.state == "dizzy" then
        e.dizzyTimer = e.dizzyTimer - dt
        -- Tweet bird particles
        if math.random() < 0.25 then
            vfx.burst(e.pos.x, e.pos.y + 2.1, e.pos.z, 1.0, 0.9, 0.15, 2, 1.2, 0.08, 0.35)
        end
        if e.dizzyTimer <= 0 then
            e.state = "idle"
            AnimationTree.play(e.animTree, "idle", "none", 1.0)
        end
    elseif e.state == "launched" or e.state == "knockdown" then
        -- Air Juggle Physics
        if not e.isGrounded then
            e.velocity.y = e.velocity.y - 28.0 * dt
            e.pos.y = e.pos.y + e.velocity.y * dt
            if e.pos.y <= 0.0 then
                e.pos.y = 0.0
                e.velocity.y = 0.0
                e.isGrounded = true
                e.state = "knockdown"
                e.knockdownTimer = 1.3
                AnimationTree.play(e.animTree, "reaction", "knockdown", 1.0)
                SFX.playPunchHeavy()
                -- Ground slam shockwave
                vfx.burst(e.pos.x, 0.1, e.pos.z, 0.75, 0.6, 0.45, 16, 4.0, 0.16, 0.45)
            end
        else
            e.knockdownTimer = (e.knockdownTimer or 1.3) - dt
            if e.knockdownTimer <= 0 then
                e.state = "getup"
                e.getupTimer = 0.55
                e.isInvincible = true
                AnimationTree.play(e.animTree, "reaction", "getup", 1.0)
            end
        end
    elseif e.state == "getup" then
        e.getupTimer = (e.getupTimer or 0.55) - dt
        if e.getupTimer <= 0 then
            e.state = "idle"
            e.isInvincible = false
            AnimationTree.play(e.animTree, "idle", "none", 1.0)
        end
    end

    -- Physics Velocity Integration
    e.pos.x = e.pos.x + e.velocity.x * dt
    e.pos.z = e.pos.z + e.velocity.z * dt

    if e.isGrounded then
        e.velocity.x = e.velocity.x * math.max(0.0, 1.0 - dt * 8.5)
        e.velocity.z = e.velocity.z * math.max(0.0, 1.0 - dt * 8.5)
    end

    -- Arena Bounds Clamping & Wall Splat
    e.pos.x = math.max(-16.8, math.min(16.8, e.pos.x))
    e.pos.z = math.max(-16.8, math.min(16.8, e.pos.z))
    Combat.checkWallSplat(e, 16.5)

    -- Update Procedural 3D Rig Transforms
    if e.rig then
        e.rig.pos.x = e.pos.x
        e.rig.pos.y = e.pos.y
        e.rig.pos.z = e.pos.z
        e.rig.rotY = e.facingAngle

        AnimationTree.update(e.animTree, dt, e.rig)
        HumanoidRig.updateTransform(e.rig)
    end
end

return EnemyAI
