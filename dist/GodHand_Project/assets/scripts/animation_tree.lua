-- animation_tree.lua
-- Procedural Humanoid Animation State Machine & Blend Tree for God Hand 3D

local AnimationTree = {}

function AnimationTree.create()
    return {
        state = "idle",     -- "idle", "run", "attack", "dodge", "reaction", "taunt", "grovel"
        animTime = 0.0,
        subState = "none",  -- "jab", "straight", "body_blow", "hook", "uppercut", "guard_break", "high_kick", "low_sweep", "dropkick", "pummel", "special", "duck", "sway_left", "sway_right", "backflip", "hitstun", "guard_broken", "block", "dizzy", "launched", "knockdown", "getup"
        speed = 1.0
    }
end

function AnimationTree.play(tree, state, subState, speed)
    tree.state = state
    tree.subState = subState or "none"
    tree.animTime = 0.0
    tree.speed = speed or 1.0
end

function AnimationTree.update(tree, dt, rig)
    if not rig or not rig.jointAngles then return end

    tree.animTime = tree.animTime + dt * tree.speed
    local t = tree.animTime
    local joints = rig.jointAngles

    -- Default zero angles
    joints.spine.pitch = 0
    joints.spine.yaw = 0
    joints.spine.roll = 0
    joints.head.pitch = 0
    joints.head.yaw = 0
    joints.head.roll = 0

    if tree.state == "idle" then
        -- Martial arts rhythm sway & breathing
        local breathe = math.sin(t * 3.0) * 3.0
        local sway = math.cos(t * 2.0) * 2.0

        joints.spine.pitch = 4.0 + breathe
        joints.spine.yaw = sway
        joints.head.pitch = -2.0 - breathe * 0.5
        joints.head.yaw = -sway * 0.5

        -- Raised boxing guard
        joints.leftArm.pitch = 48.0 + math.sin(t * 3.5) * 2.0
        joints.leftForearm.pitch = 65.0
        joints.rightArm.pitch = 42.0 + math.cos(t * 3.5) * 2.0
        joints.rightForearm.pitch = 70.0

        joints.leftThigh.pitch = 8.0
        joints.leftCalf.pitch = -10.0
        joints.rightThigh.pitch = -5.0
        joints.rightCalf.pitch = 6.0

    elseif tree.state == "run" then
        -- High-energy brawler sprint
        local runFreq = 10.0
        local legCycle = math.sin(t * runFreq) * 42.0
        local armCycle = math.sin(t * runFreq) * 45.0
        local bodyBob = math.abs(math.sin(t * runFreq)) * 5.0

        joints.spine.pitch = 14.0 + bodyBob
        joints.spine.yaw = math.sin(t * runFreq * 0.5) * 8.0

        joints.leftThigh.pitch = legCycle
        joints.leftCalf.pitch = math.max(-50.0, -legCycle * 1.3)
        joints.rightThigh.pitch = -legCycle
        joints.rightCalf.pitch = math.max(-50.0, legCycle * 1.3)

        joints.leftArm.pitch = -armCycle * 0.85 + 25.0
        joints.leftForearm.pitch = 50.0
        joints.rightArm.pitch = armCycle * 0.85 + 25.0
        joints.rightForearm.pitch = 50.0

    elseif tree.state == "attack" then
        if tree.subState == "jab" then
            -- Left Jab snap (Square #1)
            local progress = math.min(1.0, t / 0.18)
            local punchReach = math.sin(progress * math.pi) * 88.0
            joints.leftArm.pitch = 12.0 + punchReach
            joints.leftForearm.pitch = math.max(10.0, 65.0 - punchReach * 0.65)
            joints.spine.yaw = punchReach * 0.22
            joints.rightArm.pitch = 45.0
            joints.rightForearm.pitch = 75.0

        elseif tree.subState == "straight" then
            -- Right Straight Cross (Square #2)
            local progress = math.min(1.0, t / 0.20)
            local punchReach = math.sin(progress * math.pi) * 98.0
            joints.rightArm.pitch = 15.0 + punchReach
            joints.rightForearm.pitch = math.max(5.0, 70.0 - punchReach * 0.75)
            joints.spine.yaw = -punchReach * 0.28
            joints.leftArm.pitch = 45.0
            joints.leftForearm.pitch = 75.0

        elseif tree.subState == "body_blow" then
            -- Heavy Body Blow (Square #3)
            local progress = math.min(1.0, t / 0.22)
            local punchReach = math.sin(progress * math.pi) * 92.0
            joints.spine.pitch = 12.0
            joints.spine.yaw = punchReach * 0.3
            joints.leftArm.pitch = 30.0 + punchReach
            joints.leftForearm.pitch = 30.0
            joints.rightArm.pitch = 50.0
            joints.rightForearm.pitch = 80.0

        elseif tree.subState == "hook" then
            -- Heavy Right Hook (Square #4)
            local progress = math.min(1.0, t / 0.24)
            local swing = math.sin(progress * math.pi) * 105.0
            joints.rightArm.pitch = 35.0
            joints.rightArm.yaw = -swing * 0.4
            joints.rightForearm.pitch = 85.0
            joints.spine.yaw = -swing * 0.35
            joints.leftArm.pitch = 45.0

        elseif tree.subState == "uppercut" then
            -- Dragon Uppercut (Square #5 Finisher)
            local progress = math.min(1.0, t / 0.36)
            local lift = math.sin(progress * math.pi) * 125.0
            joints.rightArm.pitch = -40.0 + lift
            joints.rightForearm.pitch = 25.0 + lift * 0.35
            joints.spine.pitch = -lift * 0.18
            joints.spine.yaw = -lift * 0.15
            joints.leftThigh.pitch = lift * 0.25
            joints.leftCalf.pitch = -lift * 0.3

        elseif tree.subState == "guard_break" then
            -- Heavy Piercing Guard Breaker (Triangle)
            local progress = math.min(1.0, t / 0.32)
            local thrust = math.sin(progress * math.pi) * 115.0
            joints.rightArm.pitch = thrust
            joints.rightForearm.pitch = 5.0
            joints.spine.pitch = 15.0
            joints.spine.yaw = -thrust * 0.25
            joints.leftArm.pitch = -20.0

        elseif tree.subState == "high_kick" or tree.subState == "launcher" then
            -- High Roundhouse Launcher Kick (Circle)
            local progress = math.min(1.0, t / 0.34)
            local kick = math.sin(progress * math.pi) * 110.0
            joints.rightThigh.pitch = kick
            joints.rightCalf.pitch = -kick * 0.3
            joints.spine.roll = -kick * 0.2
            joints.spine.pitch = -kick * 0.15
            joints.leftArm.pitch = 40.0
            joints.rightArm.pitch = -30.0

        elseif tree.subState == "low_sweep" then
            -- Sweeping Low Kick
            local progress = math.min(1.0, t / 0.30)
            local sweep = math.sin(progress * math.pi) * 95.0
            joints.spine.pitch = 25.0
            joints.leftThigh.pitch = -30.0
            joints.rightThigh.pitch = sweep
            joints.rightCalf.pitch = 0.0

        elseif tree.subState == "dropkick" then
            -- Flying Sprint Dropkick
            local progress = math.min(1.0, t / 0.40)
            local fly = math.sin(progress * math.pi) * 85.0
            joints.spine.pitch = -60.0
            joints.leftThigh.pitch = fly
            joints.rightThigh.pitch = fly
            joints.leftArm.pitch = -50.0
            joints.rightArm.pitch = -50.0

        elseif tree.subState == "pummel" then
            -- Gatling 20-Punch Pummel Barrage
            local punchCycle = math.sin(t * 28.0) * 85.0
            joints.leftArm.pitch = 20.0 + punchCycle
            joints.leftForearm.pitch = 40.0
            joints.rightArm.pitch = 20.0 - punchCycle
            joints.rightForearm.pitch = 40.0
            joints.spine.yaw = punchCycle * 0.15

        elseif tree.subState == "special" then
            -- God Special Burst
            local progress = math.min(1.0, t / 0.45)
            local surge = math.sin(progress * math.pi) * 120.0
            joints.rightArm.pitch = surge
            joints.rightForearm.pitch = 10.0
            joints.leftArm.pitch = -40.0
            joints.spine.pitch = -surge * 0.15
        end

    elseif tree.state == "dodge" then
        if tree.subState == "duck" then
            -- Duck & Weave
            local progress = math.min(1.0, t / 0.28)
            local dip = math.sin(progress * math.pi) * 38.0
            joints.spine.pitch = dip * 1.3
            joints.head.pitch = -dip * 0.8
            joints.leftThigh.pitch = dip * 0.9
            joints.rightThigh.pitch = dip * 0.9
            joints.leftCalf.pitch = -dip * 0.95
            joints.rightCalf.pitch = -dip * 0.95

        elseif tree.subState == "sway_left" then
            local progress = math.min(1.0, t / 0.26)
            local sway = math.sin(progress * math.pi) * 32.0
            joints.spine.roll = -sway
            joints.spine.yaw = -sway * 0.5
            joints.leftArm.pitch = 50.0
            joints.rightArm.pitch = 40.0

        elseif tree.subState == "sway_right" then
            local progress = math.min(1.0, t / 0.26)
            local sway = math.sin(progress * math.pi) * 32.0
            joints.spine.roll = sway
            joints.spine.yaw = sway * 0.5
            joints.leftArm.pitch = 40.0
            joints.rightArm.pitch = 50.0

        elseif tree.subState == "backflip" then
            local progress = math.min(1.0, t / 0.42)
            local flip = progress * 360.0
            joints.spine.pitch = -flip * 0.45
            joints.leftThigh.pitch = -45.0
            joints.rightThigh.pitch = -45.0
            joints.leftArm.pitch = -60.0
            joints.rightArm.pitch = -60.0
        end

    elseif tree.state == "reaction" then
        if tree.subState == "hitstun" then
            local progress = math.min(1.0, t / 0.22)
            local recoil = math.sin(progress * math.pi) * 30.0
            joints.head.pitch = -recoil * 1.2
            joints.spine.pitch = -recoil
            joints.leftArm.pitch = -recoil * 0.8
            joints.rightArm.pitch = -recoil * 0.8

        elseif tree.subState == "guard_broken" then
            -- Guard broken: wide open vulnerability
            local progress = math.min(1.0, t / 0.65)
            local stagger = math.sin(progress * math.pi) * 45.0
            joints.leftArm.pitch = -stagger
            joints.rightArm.pitch = -stagger
            joints.spine.pitch = -stagger * 0.5
            joints.head.pitch = -stagger * 0.6

        elseif tree.subState == "block" then
            -- Tight cross block
            joints.leftArm.pitch = 65.0
            joints.leftForearm.pitch = 85.0
            joints.rightArm.pitch = 65.0
            joints.rightForearm.pitch = 85.0
            joints.spine.pitch = 10.0

        elseif tree.subState == "dizzy" then
            -- Comical dizzy sway
            local dizzy = math.sin(t * 5.5) * 22.0
            joints.head.yaw = dizzy * 1.6
            joints.head.pitch = 15.0 + math.cos(t * 3.5) * 10.0
            joints.spine.roll = dizzy * 0.7
            joints.spine.pitch = 8.0
            joints.leftArm.pitch = 12.0
            joints.rightArm.pitch = 12.0

        elseif tree.subState == "launched" then
            -- Air juggle spin
            local spin = (t * 540.0) % 360.0
            joints.spine.pitch = spin
            joints.leftThigh.pitch = 40.0
            joints.rightThigh.pitch = -30.0
            joints.leftArm.pitch = -70.0
            joints.rightArm.pitch = -70.0

        elseif tree.subState == "knockdown" then
            joints.spine.pitch = 90.0
            joints.head.pitch = 20.0
            joints.leftThigh.pitch = 15.0
            joints.rightThigh.pitch = 15.0

        elseif tree.subState == "getup" then
            local progress = math.min(1.0, t / 0.40)
            joints.spine.pitch = (1.0 - progress) * 90.0
        end

    elseif tree.state == "taunt" then
        -- Iconic "Bring it on!" finger beckon / chest tap
        local wave = math.sin(t * 12.0) * 25.0
        joints.rightArm.pitch = 55.0
        joints.rightForearm.pitch = 75.0 + wave
        joints.leftArm.pitch = -15.0
        joints.spine.yaw = -15.0
        joints.head.yaw = 15.0

    elseif tree.state == "grovel" then
        -- Deep bowing kowtow begging on the ground
        joints.spine.pitch = 75.0
        joints.head.pitch = 45.0
        joints.leftThigh.pitch = -75.0
        joints.rightThigh.pitch = -75.0
        joints.leftCalf.pitch = 80.0
        joints.rightCalf.pitch = 80.0
        joints.leftArm.pitch = 70.0
        joints.rightArm.pitch = 70.0
    end
end

return AnimationTree
