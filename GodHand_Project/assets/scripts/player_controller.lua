-- player_controller.lua
-- Gene (Player) Controller with Camera-Relative 3D Omnidirectional Locomotion, Walk/Run/Sprint, Jump, Dodges, 5-Hit Combos & God Hand

local SFX = require("assets/scripts/sfx_manager")
local Combat = require("assets/scripts/combat_engine")
local Difficulty = require("assets/scripts/difficulty_system")
local GodReel = require("assets/scripts/god_reel")
local HumanoidRig = require("assets/scripts/humanoid_rig")
local AnimationTree = require("assets/scripts/animation_tree")
local Camera = require("assets/scripts/camera_controller")

local Player = {
    id = 1,
    isPlayer = true,
    pos = { x = 0.0, y = 0.0, z = 0.0 },
    velocity = { x = 0.0, y = 0.0, z = 0.0 },
    facingAngle = 0.0,    -- degrees (0 = looking down -Z)
    targetAngle = 0.0,
    leanAngle = 0.0,
    hp = 300,
    maxHp = 300,
    radius = 0.45,
    isGrounded = true,
    isInvincible = false,
    isGodHand = false,

    -- Locomotion & State: "idle", "walk", "run", "sprint", "jump", "attack", "dodge", "taunt", "grovel", "special", "hitstun", "knockdown", "pummel"
    state = "idle",
    stateTimer = 0.0,
    locomotionTier = "run", -- "walk", "run", "sprint"

    -- Combos
    comboStep = 0,
    comboWindow = 0.0,
    currentAttack = nil,
    attackHasHit = false,

    -- Groveling
    grovelHoldTimer = 0.0,

    -- Rig & Animation
    rig = nil,
    animTree = nil,

    -- Move Definitions
    comboChain = {
        { name = "LEFT JAB", anim = "jab", startup = 0.05, active = 0.10, recovery = 0.14, damage = 26, hitboxRadius = 1.3, type = "light", hitstop = 2, knockback = 1.0 },
        { name = "RIGHT STRAIGHT", anim = "straight", startup = 0.06, active = 0.12, recovery = 0.16, damage = 32, hitboxRadius = 1.35, type = "light", hitstop = 2, knockback = 1.2 },
        { name = "BODY BLOW", anim = "body_blow", startup = 0.08, active = 0.14, recovery = 0.18, damage = 40, hitboxRadius = 1.4, type = "medium", isMedium = true, hitstop = 3, knockback = 1.5 },
        { name = "RIGHT HOOK", anim = "hook", startup = 0.10, active = 0.16, recovery = 0.20, damage = 48, hitboxRadius = 1.45, type = "heavy", isHeavy = true, hitstop = 4, knockback = 2.4 },
        { name = "DRAGON UPPERCUT", anim = "uppercut", startup = 0.14, active = 0.18, recovery = 0.28, damage = 75, hitboxRadius = 1.6, isLauncher = true, isHeavy = true, launchHeight = 13.0, hitstop = 6, knockback = 4.0 }
    },

    guardBreaker = {
        name = "GUARD BREAKER", anim = "guard_break", startup = 0.16, active = 0.16, recovery = 0.26, damage = 50, isGuardBreak = true, isHeavy = true, hitboxRadius = 1.5, hitstop = 6, knockback = 2.4
    },

    launcher = {
        name = "HIGH ROUNDHOUSE", anim = "high_kick", startup = 0.14, active = 0.16, recovery = 0.26, damage = 60, isLauncher = true, isHeavy = true, launchHeight = 13.0, hitboxRadius = 1.5, hitstop = 5, knockback = 3.6
    },

    lowSweep = {
        name = "LOW SWEEP TRIP", anim = "low_sweep", startup = 0.12, active = 0.15, recovery = 0.24, damage = 38, isKnockdown = true, hitboxRadius = 1.45, hitstop = 4, knockback = 2.2
    },

    dropkick = {
        name = "FLYING DROPKICK", anim = "dropkick", startup = 0.10, active = 0.22, recovery = 0.32, damage = 68, isLauncher = true, isHeavy = true, launchHeight = 8.5, hitboxRadius = 1.7, hitstop = 6, knockback = 5.2
    }
}

function Player.init(startX, startY, startZ)
    if Player.rig then
        HumanoidRig.destroy(Player.rig)
    end

    Player.pos = { x = startX or 0.0, y = startY or 0.0, z = startZ or 0.0 }
    Player.velocity = { x = 0.0, y = 0.0, z = 0.0 }
    Player.facingAngle = 0.0
    Player.targetAngle = 0.0
    Player.leanAngle = 0.0
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
    Player.isGrounded = true

    -- Create Gene's High-Definition Procedural Rig
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
    if Player.state == "idle" or Player.state == "walk" or Player.state == "run" or Player.state == "sprint" or Player.state == "jump" then
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

    -- 3D Velocity & Airborne Gravity Physics
    Player.pos.x = Player.pos.x + Player.velocity.x * dt
    Player.pos.z = Player.pos.z + Player.velocity.z * dt

    if not Player.isGrounded then
        Player.velocity.y = Player.velocity.y - 28.0 * dt
        Player.pos.y = Player.pos.y + Player.velocity.y * dt
        if Player.pos.y <= 0.0 then
            Player.pos.y = 0.0
            Player.velocity.y = 0.0
            Player.isGrounded = true
            if Player.state == "jump" then
                Player.state = "idle"
                AnimationTree.play(Player.animTree, "jump_land", "none", 1.0)
                SFX.playHit(1.0)
            end
        end
    else
        -- Ground friction damping
        local friction = (Player.state == "sprint") and 6.0 or 10.0
        Player.velocity.x = Player.velocity.x * math.max(0.0, 1.0 - dt * friction)
        Player.velocity.z = Player.velocity.z * math.max(0.0, 1.0 - dt * friction)
    end

    -- Expansive Open-World Exploration Bounds (-95m to +95m)
    Player.pos.x = math.max(-95.0, math.min(95.0, Player.pos.x))
    Player.pos.z = math.max(-95.0, math.min(95.0, Player.pos.z))

    -- Update Procedural Rig Transforms, Lean Angle & Animation State
    if Player.rig then
        Player.rig.pos.x = Player.pos.x
        Player.rig.pos.y = Player.pos.y
        Player.rig.pos.z = Player.pos.z
        Player.rig.rotY = Player.facingAngle
        Player.rig.leanAngle = Player.leanAngle

        AnimationTree.update(Player.animTree, dt, Player.rig)
        HumanoidRig.updateTransform(Player.rig)
    end
end

function Player.handleMovementInput(dt)
    -- 1. Determine Target Move Speed based on Locomotion Tier
    local baseSpeed = 6.8
    local isWalking = input.is_down("lalt") or input.is_down("ralt")
    local isSprinting = input.is_down("lshift") or input.is_down("rshift")

    local currentSpeed = baseSpeed
    if isWalking then
        currentSpeed = 3.2
        Player.locomotionTier = "walk"
    elseif isSprinting then
        currentSpeed = Player.isGodHand and 14.5 or 11.5
        Player.locomotionTier = "sprint"
    else
        currentSpeed = Player.isGodHand and 9.5 or 6.8
        Player.locomotionTier = "run"
    end

    -- 2. Camera-Relative Omnidirectional Input Vectors
    local camYaw = Camera.getYaw()
    local camRad = math.rad(camYaw)
    local camFwdX = -math.sin(camRad)
    local camFwdZ = -math.cos(camRad)
    local camRightX = math.cos(camRad)
    local camRightZ = -math.sin(camRad)

    local inputX = 0.0
    local inputZ = 0.0

    if input.is_down("w") or input.is_down("up") then
        inputX = inputX + camFwdX
        inputZ = inputZ + camFwdZ
    end
    if input.is_down("s") or input.is_down("down") then
        inputX = inputX - camFwdX
        inputZ = inputZ - camFwdZ
    end
    if input.is_down("d") or input.is_down("right") then
        inputX = inputX + camRightX
        inputZ = inputZ + camRightZ
    end
    if input.is_down("a") or input.is_down("left") then
        inputX = inputX - camRightX
        inputZ = inputZ - camRightZ
    end

    local inputLen = math.sqrt(inputX * inputX + inputZ * inputZ)
    local isMoving = inputLen > 0.01

    -- 3. 3D Jump Trigger (Space)
    if input.is_just_pressed("space") and Player.isGrounded then
        Player.velocity.y = 10.8
        Player.isGrounded = false
        Player.state = "jump"
        AnimationTree.play(Player.animTree, "jump_air", "none", 1.0)
        SFX.playDodge()
        if math.random() < 0.6 then
            vfx.burst(Player.pos.x, 0.1, Player.pos.z, 0.9, 0.7, 0.4, 4, 1.2, 0.1, 0.3)
        end
    end

    if isMoving then
        inputX = inputX / inputLen
        inputZ = inputZ / inputLen

        -- Accelerate towards movement vector
        Player.velocity.x = inputX * currentSpeed
        Player.velocity.z = inputZ * currentSpeed

        -- Calculate desired character heading in degrees (0 = -Z, 90 = +X, 180 = +Z, 270 = -X)
        local atanFunc = math.atan2 or math.atan
        local targetHeading = math.deg(atanFunc(inputX, -inputZ))
        if targetHeading < 0 then targetHeading = targetHeading + 360.0 end
        Player.targetAngle = targetHeading

        -- Smooth Angular Slerp towards target heading
        local angleDiff = (Player.targetAngle - Player.facingAngle + 540.0) % 360.0 - 180.0
        local turnRate = 680.0 * dt
        if math.abs(angleDiff) < turnRate then
            Player.facingAngle = Player.targetAngle
        else
            local sign = (angleDiff >= 0) and 1.0 or -1.0
            Player.facingAngle = (Player.facingAngle + sign * turnRate) % 360.0
        end

        -- Dynamic banking lean into sharp turns
        local targetLean = -math.max(-25.0, math.min(25.0, angleDiff * 0.4))
        Player.leanAngle = Player.leanAngle + (targetLean - Player.leanAngle) * math.min(1.0, dt * 12.0)

        -- Particle dust trails on run/sprint
        if Player.isGrounded then
            local dustChance = (Player.locomotionTier == "sprint") and 0.45 or 0.20
            if math.random() < dustChance then
                local particleColor = Player.isGodHand and {1.0, 0.85, 0.2} or {0.85, 0.68, 0.45}
                vfx.burst(Player.pos.x, 0.05, Player.pos.z, particleColor[1], particleColor[2], particleColor[3], 2, 0.9, 0.08, 0.25)
            end
        end

        -- Update Animation Tree State
        if Player.isGrounded and Player.state ~= "attack" and Player.state ~= "dodge" and Player.state ~= "special" then
            if Player.state ~= Player.locomotionTier then
                Player.state = Player.locomotionTier
                local animSpeed = (Player.locomotionTier == "sprint") and 1.3 or 1.0
                AnimationTree.play(Player.animTree, Player.locomotionTier, "none", animSpeed)
            end
        end
    else
        -- Decay lean angle when stopped
        Player.leanAngle = Player.leanAngle * math.max(0.0, 1.0 - dt * 10.0)

        if Player.isGrounded and Player.state ~= "idle" and Player.state ~= "attack" and Player.state ~= "dodge" and Player.state ~= "special" and Player.state ~= "taunt" and Player.state ~= "grovel" then
            Player.state = "idle"
            AnimationTree.play(Player.animTree, "idle", "none", 1.0)
        end
    end
end

function Player.handleCombatInput(enemies, props)
    -- 1. Directional Dodges (IJKL or Arrow Keys)
    if input.is_just_pressed("i") then
        Player.startDodge("duck")
        return
    elseif input.is_just_pressed("u") then
        Player.startDodge("sway_left")
        return
    elseif input.is_just_pressed("o") then
        Player.startDodge("sway_right")
        return
    elseif input.is_just_pressed("k") then
        Player.startDodge("backflip")
        return
    end

    -- 2. God Reel Roulette Trigger (Tab / Q)
    if input.is_just_pressed("tab") or input.is_just_pressed("q") then
        GodReel.toggle(Player)
        return
    end

    -- 3. Unleash God Hand (F / R / E)
    if input.is_just_pressed("f") or input.is_just_pressed("r") or input.is_just_pressed("e") then
        GodReel.unleashGodHand(Player)
    end

    -- 4. Taunt ("Bring it on!") (T / G)
    if input.is_just_pressed("t") or input.is_just_pressed("g") then
        Player.startTaunt()
        return
    end

    -- 5. Standard Attack Inputs (J / Square / Mouse Left)
    local isAttackPressed = input.is_just_pressed("j") or input.is_just_pressed("z") or input.is_just_pressed("mouse_left")
    local isLauncherPressed = input.is_just_pressed("l") or input.is_just_pressed("c")
    local isGuardBreakPressed = input.is_just_pressed("k") or input.is_just_pressed("x")

    if isAttackPressed then
        Player.startComboAttack(enemies, props)
    elseif isLauncherPressed then
        Player.startSpecialAttack(Player.launcher, enemies, props)
    elseif isGuardBreakPressed then
        Player.startSpecialAttack(Player.guardBreaker, enemies, props)
    end
end

function Player.startComboAttack(enemies, props)
    Player.comboStep = (Player.comboStep % #Player.comboChain) + 1
    local move = Player.comboChain[Player.comboStep]
    Player.startAttack(move, enemies, props)
end

function Player.startSpecialAttack(move, enemies, props)
    Player.comboStep = 0
    Player.startAttack(move, enemies, props)
end

function Player.startAttack(move, enemies, props)
    Player.state = "attack"
    Player.currentAttack = move
    Player.stateTimer = move.startup + move.active + move.recovery
    Player.attackHasHit = false

    local animSpeed = Player.isGodHand and 1.6 or 1.0
    AnimationTree.play(Player.animTree, "attack", move.anim, animSpeed)

    -- Play attack woosh
    SFX.playWoosh(Player.comboStep)

    -- Step forward during attack
    local rad = math.rad(Player.facingAngle)
    local fwdX = -math.sin(rad)
    local fwdZ = -math.cos(rad)
    local forwardStep = (move.isHeavy and 4.8 or 2.8)
    Player.velocity.x = fwdX * forwardStep
    Player.velocity.z = fwdZ * forwardStep
end

function Player.handleAttack(dt, enemies, props)
    if not Player.currentAttack then return end

    Player.stateTimer = Player.stateTimer - dt
    local move = Player.currentAttack
    local timeInMove = (move.startup + move.active + move.recovery) - Player.stateTimer

    -- Active Hitbox Phase
    if timeInMove >= move.startup and timeInMove <= (move.startup + move.active) then
        if not Player.attackHasHit then
            local rad = math.rad(Player.facingAngle)
            local hitX = Player.pos.x - math.sin(rad) * 1.1
            local hitZ = Player.pos.z - math.cos(rad) * 1.1

            -- Check Hit against Enemies
            if enemies then
                for _, enemy in ipairs(enemies) do
                    if enemy.hp > 0 and enemy.state ~= "dead" then
                        local dx = enemy.pos.x - hitX
                        local dz = enemy.pos.z - hitZ
                        local dist = math.sqrt(dx * dx + dz * dz)
                        if dist <= (move.hitboxRadius + enemy.radius) then
                            Player.attackHasHit = true
                            Combat.applyHit(Player, enemy, move)
                            Player.comboWindow = 0.55
                            break
                        end
                    end
                end
            end

            -- Check Hit against Destructible Props
            if props and not Player.attackHasHit then
                for _, prop in ipairs(props) do
                    if not prop.isDestroyed then
                        local dx = prop.pos.x - hitX
                        local dz = prop.pos.z - hitZ
                        local dist = math.sqrt(dx * dx + dz * dz)
                        if dist <= (move.hitboxRadius + prop.radius) then
                            Player.attackHasHit = true
                            prop.destroy(Player.facingAngle)
                            SFX.playCrateBreak()
                            Camera.addTrauma(0.25)
                            break
                        end
                    end
                end
            end
        end
    end

    if Player.stateTimer <= 0 then
        Player.state = "idle"
        Player.currentAttack = nil
        AnimationTree.play(Player.animTree, "idle", "none", 1.0)
    end
end

function Player.startDodge(dodgeType)
    Player.state = "dodge"
    Player.stateTimer = 0.32
    Player.isInvincible = true
    AnimationTree.play(Player.animTree, "dodge", dodgeType, 1.0)
    SFX.playDodge()

    if dodgeType == "backflip" then
        local rad = math.rad(Player.facingAngle)
        Player.velocity.x = math.sin(rad) * 8.5
        Player.velocity.z = math.cos(rad) * 8.5
        Player.velocity.y = 4.2
        Player.isGrounded = false
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

function Player.startTaunt()
    Player.state = "taunt"
    Player.stateTimer = 0.75
    AnimationTree.play(Player.animTree, "taunt", "none", 1.0)
    SFX.playTaunt()
    GodReel.addTension(35)
end

function Player.handleTaunt(dt)
    Player.stateTimer = Player.stateTimer - dt
    if Player.stateTimer <= 0 then
        Player.state = "idle"
        AnimationTree.play(Player.animTree, "idle", "none", 1.0)
    end
end

function Player.handleKnockdown(dt)
    Player.stateTimer = Player.stateTimer - dt
    if Player.stateTimer <= 0 then
        Player.state = "idle"
        Player.isGrounded = true
        AnimationTree.play(Player.animTree, "idle", "none", 1.0)
    end
end

return Player
