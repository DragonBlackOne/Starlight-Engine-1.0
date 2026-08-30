-- player_controller.lua
-- Gene (Player) Controller with Directional Dodges, 5-Hit Combos, Guard Breaker, Launcher, Taunt & God Hand

local SFX = require("assets/scripts/sfx_manager")
local Combat = require("assets/scripts/combat_engine")
local Difficulty = require("assets/scripts/difficulty_system")
local GodReel = require("assets/scripts/god_reel")
local HumanoidRig = require("assets/scripts/humanoid_rig")
local AnimationTree = require("assets/scripts/animation_tree")

local Player = {
    id = 1,
    isPlayer = true,
    pos = { x = 0.0, y = 0.0, z = 0.0 },
    velocity = { x = 0.0, y = 0.0, z = 0.0 },
    facingAngle = 0.0, -- degrees (0 = looking down -Z)
    hp = 300,
    maxHp = 300,
    radius = 0.45,
    isGrounded = true,
    isInvincible = false,
    isGodHand = false,

    -- State Machine: "idle", "run", "attack", "dodge", "taunt", "grovel", "special", "hitstun", "knockdown", "pummel"
    state = "idle",
    stateTimer = 0.0,

    -- Combos
    comboStep = 0,
    comboWindow = 0.0,
    currentAttack = nil,
    attackHasHit = false,

    -- Groveling (holding space to beg for mercy and lower difficulty)
    grovelHoldTimer = 0.0,

    -- Procedural Humanoid Rig & Animation Blend Tree
    rig = nil,
    animTree = nil,

    -- Move Definitions
    comboChain = {
        { name = "LEFT JAB", anim = "jab", startup = 0.05, active = 0.10, recovery = 0.14, damage = 24, hitboxRadius = 1.2, type = "light", hitstop = 2, knockback = 1.0 },
        { name = "RIGHT STRAIGHT", anim = "straight", startup = 0.06, active = 0.12, recovery = 0.16, damage = 30, hitboxRadius = 1.3, type = "light", hitstop = 2, knockback = 1.2 },
        { name = "BODY BLOW", anim = "body_blow", startup = 0.08, active = 0.14, recovery = 0.18, damage = 38, hitboxRadius = 1.3, type = "medium", isMedium = true, hitstop = 3, knockback = 1.5 },
        { name = "RIGHT HOOK", anim = "hook", startup = 0.10, active = 0.16, recovery = 0.20, damage = 46, hitboxRadius = 1.4, type = "heavy", isHeavy = true, hitstop = 4, knockback = 2.4 },
        { name = "DRAGON UPPERCUT", anim = "uppercut", startup = 0.14, active = 0.18, recovery = 0.28, damage = 72, hitboxRadius = 1.5, isLauncher = true, isHeavy = true, launchHeight = 12.0, hitstop = 6, knockback = 3.8 }
    },

    guardBreaker = {
        name = "GUARD BREAKER", anim = "guard_break", startup = 0.16, active = 0.16, recovery = 0.26, damage = 48, isGuardBreak = true, isHeavy = true, hitboxRadius = 1.4, hitstop = 6, knockback = 2.2
    },

    launcher = {
        name = "HIGH ROUNDHOUSE", anim = "high_kick", startup = 0.14, active = 0.16, recovery = 0.26, damage = 58, isLauncher = true, isHeavy = true, launchHeight = 12.5, hitboxRadius = 1.5, hitstop = 5, knockback = 3.5
    },

    lowSweep = {
        name = "LOW SWEEP TRIP", anim = "low_sweep", startup = 0.12, active = 0.15, recovery = 0.24, damage = 36, isKnockdown = true, hitboxRadius = 1.4, hitstop = 4, knockback = 2.0
    },

    dropkick = {
        name = "FLYING DROPKICK", anim = "dropkick", startup = 0.10, active = 0.22, recovery = 0.32, damage = 65, isLauncher = true, isHeavy = true, launchHeight = 8.0, hitboxRadius = 1.6, hitstop = 6, knockback = 5.0
    }
}

function Player.init(startX, startY, startZ)
    if Player.rig then
        HumanoidRig.destroy(Player.rig)
    end

    Player.pos = { x = startX or 0.0, y = startY or 0.0, z = startZ or 0.0 }
    Player.velocity = { x = 0.0, y = 0.0, z = 0.0 }
    Player.facingAngle = 0.0
    Player.hp = Player.maxHp
    Player.state = "idle"
    Player.stateTimer = 0.0
    Player.comboStep = 0
    Player.comboWindow = 0.0
    Player.currentAttack = nil
    Player.attackHasHit = false
    Player.grovelHoldTimer = 0.0
    Player.isInvincible = false
    Player.isGodHand = false

    -- Create Gene's Procedural Humanoid Rig & Animation Tree
    Player.rig = HumanoidRig.create("Gene", "gene", Player.pos.x, Player.pos.y, Player.pos.z, 1.0)
    Player.animTree = AnimationTree.create()
    AnimationTree.play(Player.animTree, "idle", "none", 1.0)
end

function Player.update(dt, enemies, props)
    if Combat.isHitStopped(Player.id) then return end

    -- Combo Window Timer
    if Player.comboWindow > 0 then
        Player.comboWindow = Player.comboWindow - dt
        if Player.comboWindow <= 0 then
            Player.comboStep = 0
        end
    end

    -- Process State Actions
    if Player.state == "idle" or Player.state == "run" then
        Player.handleMovementInput(dt)
        Player.handleCombatInput(enemies, props)
    elseif Player.state == "dodge" then
        Player.handleDodge(dt)
    elseif Player.state == "attack" or Player.state == "special" then
        Player.handleAttack(dt, enemies, props)
    elseif Player.state == "pummel" then
        Player.handlePummel(dt)
    elseif Player.state == "taunt" then
        Player.handleTaunt(dt)
    elseif Player.state == "grovel" then
        Player.handleGrovel(dt)
    elseif Player.state == "hitstun" then
        Player.stateTimer = Player.stateTimer - dt
        if Player.stateTimer <= 0 then
            Player.state = "idle"
            AnimationTree.play(Player.animTree, "idle", "none", 1.0)
        end
    elseif Player.state == "knockdown" then
        Player.handleKnockdown(dt)
    end

    -- Apply Velocity & Gravity Physics
    Player.pos.x = Player.pos.x + Player.velocity.x * dt
    Player.pos.z = Player.pos.z + Player.velocity.z * dt

    if not Player.isGrounded then
        Player.velocity.y = Player.velocity.y - 26.0 * dt
        Player.pos.y = Player.pos.y + Player.velocity.y * dt
        if Player.pos.y <= 0.0 then
            Player.pos.y = 0.0
            Player.velocity.y = 0.0
            Player.isGrounded = true
        end
    else
        -- Ground friction
        Player.velocity.x = Player.velocity.x * math.max(0.0, 1.0 - dt * 10.0)
        Player.velocity.z = Player.velocity.z * math.max(0.0, 1.0 - dt * 10.0)
    end

    -- Arena Bounds Clamping (-17.0 to 17.0)
    Player.pos.x = math.max(-16.8, math.min(16.8, Player.pos.x))
    Player.pos.z = math.max(-16.8, math.min(16.8, Player.pos.z))

    -- Wall Splat check on Player if launched
    Combat.checkWallSplat(Player, 16.5)

    -- Update Procedural Rig Transforms & Animation State
    if Player.rig then
        Player.rig.pos.x = Player.pos.x
        Player.rig.pos.y = Player.pos.y
        Player.rig.pos.z = Player.pos.z
        Player.rig.rotY = Player.facingAngle

        AnimationTree.update(Player.animTree, dt, Player.rig)
        HumanoidRig.updateTransform(Player.rig)
    end
end

function Player.handleMovementInput(dt)
    local moveSpeed = 6.4
    if Player.isGodHand then moveSpeed = 9.2 end

    local isMoving = false
    local turnSpeed = 230.0

    -- Turning (A/D or Left/Right Arrow)
    if input.is_down("a") or input.is_down("left") then
        Player.facingAngle = (Player.facingAngle + turnSpeed * dt) % 360.0
    elseif input.is_down("d") or input.is_down("right") then
        Player.facingAngle = (Player.facingAngle - turnSpeed * dt) % 360.0
    end

    local rad = math.rad(Player.facingAngle)
    local fwdX = -math.sin(rad)
    local fwdZ = -math.cos(rad)

    -- 180 Quick Turn (Down + Space tap or S + Space)
    if input.is_just_pressed("s") and input.is_down("space") then
        Player.facingAngle = (Player.facingAngle + 180.0) % 360.0
        SFX.playDodge()
        return
    end

    -- Forward Run (W / Up Arrow)
    if input.is_down("w") or input.is_down("up") then
        Player.velocity.x = fwdX * moveSpeed
        Player.velocity.z = fwdZ * moveSpeed
        isMoving = true

        -- Footstep dust particles
        if math.random() < 0.28 then
            vfx.burst(Player.pos.x, 0.05, Player.pos.z, 0.8, 0.65, 0.45, 2, 0.8, 0.08, 0.25)
        end
    elseif input.is_down("s") or input.is_down("down") then
        -- Backstep
        Player.velocity.x = -fwdX * (moveSpeed * 0.55)
        Player.velocity.z = -fwdZ * (moveSpeed * 0.55)
        isMoving = true
    end

    if isMoving then
        if Player.state ~= "run" then
            Player.state = "run"
            AnimationTree.play(Player.animTree, "run", "none", Player.isGodHand and 1.4 or 1.0)
        end
    else
        if Player.state ~= "idle" then
            Player.state = "idle"
            AnimationTree.play(Player.animTree, "idle", "none", 1.0)
        end
    end
end

function Player.handleCombatInput(enemies, props)
    -- 1. Directional Dodges (IJKL or Arrow Keys with Shift)
    if input.is_just_pressed("i") or (input.is_just_pressed("up") and input.is_down("lshift")) then
        Player.startDodge("duck")
        return
    elseif input.is_just_pressed("u") or (input.is_just_pressed("left") and input.is_down("lshift")) then
        Player.startDodge("sway_left")
        return
    elseif input.is_just_pressed("o") or (input.is_just_pressed("right") and input.is_down("lshift")) then
        Player.startDodge("sway_right")
        return
    elseif input.is_just_pressed("k") and (input.is_down("lshift") or input.is_down("s")) then
        Player.startDodge("backflip")
        return
    end

    -- 2. God Reel Roulette Trigger (Tab / Q)
    if input.is_just_pressed("tab") or input.is_just_pressed("q") then
        GodReel.toggle(Player)
        return
    end

    -- 3. Unleash God Hand (F / R)
    if input.is_just_pressed("f") or input.is_just_pressed("r") then
        GodReel.unleashGodHand(Player)
    end

    -- 4. Taunt ("Bring it on!") (T / G)
    if input.is_just_pressed("t") or input.is_just_pressed("g") then
        Player.startTaunt()
        return
    end

    -- 5. Grovel / Beg for Mercy (Hold Space)
    if input.is_down("space") then
        Player.grovelHoldTimer = Player.grovelHoldTimer + 0.016
        if Player.grovelHoldTimer >= 0.5 then
            Player.startGrovel()
            return
        end
    else
        Player.grovelHoldTimer = 0.0
    end

    -- 6. Context Finisher: Pummel Dizzy Enemy
    local dizzyTarget = Player.findClosestDizzyEnemy(enemies)
    if dizzyTarget and (input.is_just_pressed("j") or input.is_mouse_down(1)) then
        Player.startPummel(dizzyTarget)
        return
    end

    -- 7. Sprint Flying Dropkick (W + Shift + J)
    if (input.is_down("w") or input.is_down("up")) and input.is_down("lshift") and (input.is_just_pressed("j") or input.is_mouse_down(1)) then
        Player.startAttack(Player.dropkick)
        local rad = math.rad(Player.facingAngle)
        Player.velocity.x = -math.sin(rad) * 9.0
        Player.velocity.z = -math.cos(rad) * 9.0
        Player.velocity.y = 3.5
        Player.isGrounded = false
        return
    end

    -- 8. Low Sweeping Kick (S + E / Down + L)
    if (input.is_down("s") or input.is_down("down")) and (input.is_just_pressed("l") or input.is_just_pressed("e")) then
        Player.startAttack(Player.lowSweep)
        return
    end

    -- 9. Triangle Guard Breaker (K / Right Click)
    if input.is_just_pressed("k") or input.is_mouse_down(3) then
        Player.startAttack(Player.guardBreaker)
        return
    end

    -- 10. Circle High Launcher Roundhouse (L / E)
    if input.is_just_pressed("l") or input.is_just_pressed("e") then
        Player.startAttack(Player.launcher)
        return
    end

    -- 11. Square Combo Chain (J / Left Click)
    if input.is_just_pressed("j") or input.is_mouse_down(1) then
        Player.startComboAttack()
        return
    end
end

function Player.startDodge(dodgeType)
    Player.state = "dodge"
    Player.dodgeType = dodgeType
    Player.stateTimer = 0.32
    Player.isInvincible = true
    AnimationTree.play(Player.animTree, "dodge", dodgeType, 1.0)

    SFX.playDodge()
    Difficulty.addHit("dodge")

    local rad = math.rad(Player.facingAngle)
    local fwdX = -math.sin(rad)
    local fwdZ = -math.cos(rad)
    local rightX = math.cos(rad)
    local rightZ = -math.sin(rad)

    if dodgeType == "duck" then
        -- Weave forward under attacks
        Player.velocity.x = fwdX * 3.8
        Player.velocity.z = fwdZ * 3.8
        vfx.emit_trail(Player.pos.x, Player.pos.y + 0.8, Player.pos.z, 0.4, 0.8, 1.0, 0.15, 0.3)
    elseif dodgeType == "sway_left" then
        Player.velocity.x = -rightX * 4.8
        Player.velocity.z = -rightZ * 4.8
        vfx.emit_trail(Player.pos.x, Player.pos.y + 1.0, Player.pos.z, 0.4, 0.8, 1.0, 0.15, 0.3)
    elseif dodgeType == "sway_right" then
        Player.velocity.x = rightX * 4.8
        Player.velocity.z = rightZ * 4.8
        vfx.emit_trail(Player.pos.x, Player.pos.y + 1.0, Player.pos.z, 0.4, 0.8, 1.0, 0.15, 0.3)
    elseif dodgeType == "backflip" then
        Player.velocity.x = -fwdX * 8.0
        Player.velocity.z = -fwdZ * 8.0
        Player.velocity.y = 4.5
        Player.isGrounded = false
        vfx.emit_trail(Player.pos.x, Player.pos.y + 1.0, Player.pos.z, 1.0, 0.85, 0.2, 0.2, 0.4)
    end
end

function Player.handleDodge(dt)
    Player.stateTimer = Player.stateTimer - dt
    if Player.stateTimer <= 0 then
        Player.state = "idle"
        Player.isInvincible = false
        AnimationTree.play(Player.animTree, "idle", "none", 1.0)
    end
end

function Player.startComboAttack()
    Player.comboStep = (Player.comboStep % #Player.comboChain) + 1
    Player.comboWindow = 0.9
    local attack = Player.comboChain[Player.comboStep]
    Player.startAttack(attack)
end

function Player.startAttack(attackDef)
    Player.state = "attack"
    Player.currentAttack = attackDef
    local speedMult = Player.isGodHand and 0.65 or 1.0
    Player.startupTimer = attackDef.startup * speedMult
    Player.activeTimer = attackDef.active * speedMult
    Player.recoveryTimer = attackDef.recovery * speedMult
    Player.attackPhase = "startup"
    Player.attackHasHit = false

    AnimationTree.play(Player.animTree, "attack", attackDef.anim or "jab", 1.0 / speedMult)

    -- Step forward into strike
    local rad = math.rad(Player.facingAngle)
    local fwdX = -math.sin(rad)
    local fwdZ = -math.cos(rad)
    Player.velocity.x = fwdX * 3.2
    Player.velocity.z = fwdZ * 3.2

    SFX.playDodge()
end

function Player.handleAttack(dt, enemies, props)
    -- Instant Dodge Cancelling out of attack startup/recovery!
    if input.is_just_pressed("i") or input.is_just_pressed("u") or input.is_just_pressed("o") or (input.is_just_pressed("k") and input.is_down("lshift")) then
        Player.startDodge("duck")
        return
    end

    if Player.attackPhase == "startup" then
        Player.startupTimer = Player.startupTimer - dt
        if Player.startupTimer <= 0 then
            Player.attackPhase = "active"
        end
    elseif Player.attackPhase == "active" then
        Player.activeTimer = Player.activeTimer - dt

        -- Check Hit against all enemies & destructibles
        if not Player.attackHasHit and Player.currentAttack then
            for _, enemy in ipairs(enemies) do
                if enemy.hp > 0 then
                    local hit, sx, sy, sz = Combat.checkHit(Player.pos, Player.facingAngle, Player.currentAttack, enemy.pos, enemy.radius)
                    if hit then
                        Combat.resolveAttack(Player, enemy, Player.currentAttack)
                        Player.attackHasHit = true
                        GodReel.addTension(Player.isGodHand and 0 or 14.0)
                        break
                    end
                end
            end

            -- Check breakable props
            for _, prop in ipairs(props) do
                if prop.active and not prop.isBroken then
                    local hit = Combat.checkHit(Player.pos, Player.facingAngle, Player.currentAttack, prop.pos, prop.radius)
                    if hit then
                        prop.destroy()
                        Player.attackHasHit = true
                        GodReel.addTension(18.0)
                        break
                    end
                end
            end
        end

        if Player.activeTimer <= 0 then
            Player.attackPhase = "recovery"
        end
    elseif Player.attackPhase == "recovery" then
        Player.recoveryTimer = Player.recoveryTimer - dt
        if Player.recoveryTimer <= 0 then
            Player.state = "idle"
            Player.currentAttack = nil
            AnimationTree.play(Player.animTree, "idle", "none", 1.0)
        end
    end
end

function Player.startPummel(target)
    Player.state = "pummel"
    Player.pummelTarget = target
    Player.pummelCount = 0
    Player.pummelTimer = 2.4
    Player.isInvincible = true
    AnimationTree.play(Player.animTree, "attack", "pummel", 1.5)

    Combat.addFloatingText("PUMMEL BEATDOWN!!", target.pos.x, target.pos.y + 2.4, target.pos.z, { 1.0, 0.85, 0.1, 1.0 }, 2.8)
    SFX.playPunchMedium()
end

function Player.handlePummel(dt)
    Player.pummelTimer = Player.pummelTimer - dt
    local target = Player.pummelTarget

    if target and target.hp > 0 then
        -- Snap player in front of dizzy foe
        local rad = math.rad(Player.facingAngle)
        Player.pos.x = target.pos.x + math.sin(rad) * 1.1
        Player.pos.z = target.pos.z + math.cos(rad) * 1.1

        -- Continuous rapid hits
        if math.random() < 0.35 then
            target.hp = math.max(0, target.hp - 12)
            SFX.playPunchLight()
            Combat.spawnHitSparks(target.pos.x, target.pos.y + 1.2, target.pos.z, "light", Player.isGodHand)
            Combat.addHitStop(target.id or 1, 2)
            GodReel.addTension(4.0)
        end

        if Player.pummelTimer <= 0 or target.hp <= 0 then
            -- Final explosive home-run uppercut!
            Player.state = "idle"
            target.hp = math.max(0, target.hp - 80)
            target.velocity.y = 14.0
            target.velocity.x = -math.sin(rad) * 8.0
            target.velocity.z = -math.cos(rad) * 8.0
            target.isGrounded = false
            target.state = "launched"

            SFX.playLauncher()
            Combat.addFloatingText("HOME RUN KNOCKOUT!!", target.pos.x, target.pos.y + 2.4, target.pos.z, { 1.0, 0.2, 0.2, 1.0 }, 3.0)
            Combat.spawnHitSparks(target.pos.x, target.pos.y + 1.3, target.pos.z, "heavy", true)
            AnimationTree.play(Player.animTree, "attack", "uppercut", 1.2)
            Player.isInvincible = false
        end
    else
        Player.state = "idle"
        Player.isInvincible = false
        AnimationTree.play(Player.animTree, "idle", "none", 1.0)
    end
end

function Player.startTaunt()
    Player.state = "taunt"
    Player.stateTimer = 1.0
    AnimationTree.play(Player.animTree, "taunt", "none", 1.0)
    GodReel.addTension(25.0)
    SFX.playGodReelSelect()
    Combat.addFloatingText("COME ON BABY!! (+25% TENSION)", Player.pos.x, Player.pos.y + 2.2, Player.pos.z, { 1.0, 0.85, 0.2, 1.0 }, 2.2)
    vfx.burst(Player.pos.x, Player.pos.y + 1.2, Player.pos.z, 1.0, 0.85, 0.2, 12, 3.0, 0.12, 0.4)
end

function Player.handleTaunt(dt)
    Player.stateTimer = Player.stateTimer - dt
    if Player.stateTimer <= 0 then
        Player.state = "idle"
        AnimationTree.play(Player.animTree, "idle", "none", 1.0)
    end
end

function Player.startGrovel()
    Player.state = "grovel"
    Player.stateTimer = 1.6
    Player.isInvincible = true
    Difficulty.grovelReset()
    AnimationTree.play(Player.animTree, "grovel", "none", 1.0)
    SFX.playGrovel()
    Combat.addFloatingText("FORGIVE ME!! (LEVEL 1 RESET)", Player.pos.x, Player.pos.y + 1.8, Player.pos.z, { 0.4, 0.9, 1.0, 1.0 }, 2.2)
end

function Player.handleGrovel(dt)
    Player.stateTimer = Player.stateTimer - dt
    if Player.stateTimer <= 0 then
        Player.state = "idle"
        Player.isInvincible = false
        AnimationTree.play(Player.animTree, "idle", "none", 1.0)
    end
end

function Player.findClosestDizzyEnemy(enemies)
    for _, e in ipairs(enemies) do
        if e.hp > 0 and e.state == "dizzy" then
            local dx = e.pos.x - Player.pos.x
            local dz = e.pos.z - Player.pos.z
            local dist = math.sqrt(dx * dx + dz * dz)
            if dist <= 2.4 then
                return e
            end
        end
    end
    return nil
end

function Player.executeSpecial(card, enemies)
    Player.state = "special"
    Player.startAttack(card)
    Player.isInvincible = true
    AnimationTree.play(Player.animTree, "attack", "special", 1.2)

    -- Full camera screen shake
    local Camera = require("assets/scripts/camera_controller")
    Camera.addTrauma(0.8)

    -- Multi-enemy blastwave
    for _, e in ipairs(enemies) do
        if e.hp > 0 then
            local dx = e.pos.x - Player.pos.x
            local dz = e.pos.z - Player.pos.z
            local dist = math.sqrt(dx * dx + dz * dz)
            if dist <= (card.range or 5.5) then
                Combat.resolveAttack(Player, e, card)
            end
        end
    end
end

return Player
