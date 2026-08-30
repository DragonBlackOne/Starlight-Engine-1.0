-- animation_tree.lua
-- Procedural Humanoid Animation Blend Tree with Multi-speed Locomotion, Jump, IK, and Combat

local AnimationTree = {}

function AnimationTree.create()
    return {
        state = "idle",     -- "idle", "walk", "run", "sprint", "jump_start", "jump_air", "jump_land", "attack", "dodge", "reaction", "taunt", "grovel"
        animTime = 0.0,
        subState = "none",  -- "jab", "straight", "body_blow", "hook", "uppercut", "guard_break", "high_kick", "low_sweep", "dropkick", "pummel", "special", "duck", "sway_left", "sway_right", "backflip", "hitstun", "guard_broken", "block", "dizzy", "launched", "knockdown", "getup"
        speed = 1.0,
        blendWeight = 1.0
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
    local j = rig.jointAngles

    -- Default zero angles
    j.spine.pitch = 0; j.spine.yaw = 0; j.spine.roll = 0
    j.chest.pitch = 0; j.chest.yaw = 0; j.chest.roll = 0
    j.neck.pitch = 0;  j.neck.yaw = 0;  j.neck.roll = 0
    j.head.pitch = 0;  j.head.yaw = 0;  j.head.roll = 0
    j.leftFoot.pitch = 0; j.rightFoot.pitch = 0

    if tree.state == "idle" then
        -- Organic rhythmic martial arts breathing & weight shifting
        local breathe = math.sin(t * 2.8) * 3.5
        local sway = math.cos(t * 1.8) * 2.2
        local weightShift = math.sin(t * 1.4) * 2.0

        j.spine.pitch = 4.0 + breathe
        j.spine.yaw = sway
        j.head.pitch = -2.0 - breathe * 0.4
        j.head.yaw = -sway * 0.6

        -- Raised boxing guard with subtle micro-movements
        j.leftArm.pitch = 46.0 + math.sin(t * 3.0) * 2.0
        j.leftForearm.pitch = 68.0 + math.cos(t * 2.5) * 2.0
        j.rightArm.pitch = 40.0 + math.cos(t * 3.0) * 2.0
        j.rightForearm.pitch = 72.0 + math.sin(t * 2.5) * 2.0

        j.leftThigh.pitch = 8.0 + weightShift
        j.leftCalf.pitch = -12.0
        j.rightThigh.pitch = -6.0 - weightShift
        j.rightCalf.pitch = 8.0

    elseif tree.state == "walk" then
        -- Natural walking locomotion gait (4.5 Hz)
        local walkFreq = 5.2
        local legCycle = math.sin(t * walkFreq) * 28.0
        local armCycle = math.sin(t * walkFreq) * 26.0
        local hipBob = math.abs(math.sin(t * walkFreq)) * 2.5

        j.spine.pitch = 5.0 + hipBob
        j.spine.yaw = math.sin(t * walkFreq * 0.5) * 5.0

        j.leftThigh.pitch = legCycle
        j.leftCalf.pitch = math.max(-35.0, -legCycle * 1.1)
        j.leftFoot.pitch = math.sin(t * walkFreq) * 12.0

        j.rightThigh.pitch = -legCycle
        j.rightCalf.pitch = math.max(-35.0, legCycle * 1.1)
        j.rightFoot.pitch = -math.sin(t * walkFreq) * 12.0

        j.leftArm.pitch = -armCycle * 0.8 + 15.0
        j.leftForearm.pitch = 30.0
        j.rightArm.pitch = armCycle * 0.8 + 15.0
        j.rightForearm.pitch = 30.0

    elseif tree.state == "run" then
        -- Athletic running gait (8.5 Hz)
        local runFreq = 8.8
        local legCycle = math.sin(t * runFreq) * 44.0
        local armCycle = math.sin(t * runFreq) * 48.0
        local bodyBob = math.abs(math.sin(t * runFreq)) * 5.0

        j.spine.pitch = 14.0 + bodyBob
        j.spine.yaw = math.sin(t * runFreq * 0.5) * 8.0

        j.leftThigh.pitch = legCycle
        j.leftCalf.pitch = math.max(-55.0, -legCycle * 1.35)
        j.leftFoot.pitch = math.sin(t * runFreq) * 18.0

        j.rightThigh.pitch = -legCycle
        j.rightCalf.pitch = math.max(-55.0, legCycle * 1.35)
        j.rightFoot.pitch = -math.sin(t * runFreq) * 18.0

        j.leftArm.pitch = -armCycle * 0.9 + 28.0
        j.leftForearm.pitch = 55.0
        j.rightArm.pitch = armCycle * 0.9 + 28.0
        j.rightForearm.pitch = 55.0

    elseif tree.state == "sprint" then
        -- High-velocity sprint (11.0 Hz) with deep forward pitch
        local sprintFreq = 11.5
        local legCycle = math.sin(t * sprintFreq) * 58.0
        local armCycle = math.sin(t * sprintFreq) * 65.0
        local bodyBob = math.abs(math.sin(t * sprintFreq)) * 7.0

        j.spine.pitch = 24.0 + bodyBob
        j.spine.yaw = math.sin(t * sprintFreq * 0.5) * 12.0
        j.head.pitch = -16.0 -- Eyes looking forward

        j.leftThigh.pitch = legCycle
        j.leftCalf.pitch = math.max(-75.0, -legCycle * 1.5)
        j.leftFoot.pitch = math.sin(t * sprintFreq) * 25.0

        j.rightThigh.pitch = -legCycle
        j.rightCalf.pitch = math.max(-75.0, legCycle * 1.5)
        j.rightFoot.pitch = -math.sin(t * sprintFreq) * 25.0

        j.leftArm.pitch = -armCycle * 1.1 + 35.0
        j.leftForearm.pitch = 70.0
        j.rightArm.pitch = armCycle * 1.1 + 35.0
        j.rightForearm.pitch = 70.0

    elseif tree.state == "jump_air" then
        -- Airborne dynamic hangtime pose
        j.spine.pitch = -8.0
        j.head.pitch = 10.0
        j.leftArm.pitch = -35.0
        j.leftForearm.pitch = 25.0
        j.rightArm.pitch = -40.0
        j.rightForearm.pitch = 30.0

        j.leftThigh.pitch = 35.0
        j.leftCalf.pitch = -50.0
        j.rightThigh.pitch = -15.0
        j.rightCalf.pitch = -25.0

    elseif tree.state == "jump_land" then
        -- Impact squash and recovery
        local progress = math.min(1.0, t / 0.16)
        local squash = math.sin(progress * math.pi) * 25.0

        j.spine.pitch = 18.0 + squash * 0.5
        j.leftThigh.pitch = squash
        j.leftCalf.pitch = -squash * 1.2
        j.rightThigh.pitch = squash
        j.rightCalf.pitch = -squash * 1.2

    elseif tree.state == "attack" then
        if tree.subState == "jab" then
            local progress = math.min(1.0, t / 0.18)
            local punchReach = math.sin(progress * math.pi) * 88.0
            j.leftArm.pitch = 12.0 + punchReach
            j.leftForearm.pitch = math.max(10.0, 65.0 - punchReach * 0.65)
            j.spine.yaw = punchReach * 0.22
            j.rightArm.pitch = 45.0
            j.rightForearm.pitch = 75.0

        elseif tree.subState == "straight" then
            local progress = math.min(1.0, t / 0.20)
            local punchReach = math.sin(progress * math.pi) * 98.0
            j.rightArm.pitch = 15.0 + punchReach
            j.rightForearm.pitch = math.max(5.0, 70.0 - punchReach * 0.75)
            j.spine.yaw = -punchReach * 0.28
            j.leftArm.pitch = 45.0
            j.leftForearm.pitch = 75.0

        elseif tree.subState == "body_blow" then
            local progress = math.min(1.0, t / 0.22)
            local punchReach = math.sin(progress * math.pi) * 90.0
            j.spine.pitch = 15.0
            j.spine.yaw = punchReach * 0.32
            j.leftArm.pitch = 5.0 + punchReach
            j.leftForearm.pitch = 85.0
            j.leftThigh.pitch = 22.0
            j.leftCalf.pitch = -30.0

        elseif tree.subState == "hook" then
            local progress = math.min(1.0, t / 0.24)
            local swing = math.sin(progress * math.pi) * 110.0
            j.spine.yaw = -swing * 0.45
            j.rightArm.pitch = 35.0 + swing * 0.5
            j.rightForearm.pitch = 85.0
            j.leftArm.pitch = 50.0
            j.leftForearm.pitch = 75.0

        elseif tree.subState == "uppercut" then
            local progress = math.min(1.0, t / 0.32)
            local lift = math.sin(progress * math.pi) * 125.0
            j.spine.pitch = -lift * 0.25
            j.spine.yaw = -lift * 0.20
            j.rightArm.pitch = 10.0 + lift
            j.rightForearm.pitch = math.max(40.0, 90.0 - lift * 0.3)
            j.rightThigh.pitch = 30.0
            j.rightCalf.pitch = -45.0

        elseif tree.subState == "high_kick" then
            local progress = math.min(1.0, t / 0.30)
            local kick = math.sin(progress * math.pi) * 115.0
            j.spine.roll = -kick * 0.25
            j.rightThigh.pitch = kick
            j.rightCalf.pitch = -kick * 0.25
            j.leftThigh.pitch = -15.0
            j.leftCalf.pitch = 20.0

        elseif tree.subState == "low_sweep" then
            local progress = math.min(1.0, t / 0.28)
            local sweep = math.sin(progress * math.pi) * 95.0
            j.spine.pitch = 35.0
            j.spine.yaw = sweep * 0.5
            j.leftThigh.pitch = 45.0
            j.leftCalf.pitch = -65.0
            j.rightThigh.pitch = sweep
            j.rightCalf.pitch = -10.0

        elseif tree.subState == "dropkick" then
            local progress = math.min(1.0, t / 0.38)
            local fly = math.sin(progress * math.pi) * 85.0
            j.spine.pitch = -60.0
            j.leftThigh.pitch = fly + 20.0
            j.leftCalf.pitch = -10.0
            j.rightThigh.pitch = fly + 20.0
            j.rightCalf.pitch = -10.0
        end

    elseif tree.state == "dodge" then
        if tree.subState == "duck" then
            local progress = math.min(1.0, t / 0.26)
            local duckAmount = math.sin(progress * math.pi) * 45.0
            j.spine.pitch = duckAmount * 0.8
            j.leftThigh.pitch = duckAmount * 0.6
            j.leftCalf.pitch = -duckAmount * 0.8
            j.rightThigh.pitch = duckAmount * 0.6
            j.rightCalf.pitch = -duckAmount * 0.8

        elseif tree.subState == "sway_left" then
            local progress = math.min(1.0, t / 0.28)
            local sway = math.sin(progress * math.pi) * 32.0
            j.spine.roll = -sway
            j.head.roll = sway * 0.5

        elseif tree.subState == "sway_right" then
            local progress = math.min(1.0, t / 0.28)
            local sway = math.sin(progress * math.pi) * 32.0
            j.spine.roll = sway
            j.head.roll = -sway * 0.5

        elseif tree.subState == "backflip" then
            local progress = math.min(1.0, t / 0.42)
            local flip = progress * 360.0
            j.spine.pitch = -flip
        end

    elseif tree.state == "reaction" or tree.state == "hitstun" then
        local progress = math.min(1.0, t / 0.25)
        local recoil = math.sin(progress * math.pi) * 30.0
        j.spine.pitch = -recoil
        j.head.pitch = -recoil * 0.75
        j.leftArm.pitch = -recoil * 0.5
        j.rightArm.pitch = -recoil * 0.5

    elseif tree.state == "knockdown" then
        j.spine.pitch = -85.0
        j.leftThigh.pitch = 10.0
        j.rightThigh.pitch = 10.0
        j.leftArm.pitch = -45.0
        j.rightArm.pitch = -45.0

    elseif tree.state == "taunt" then
        local progress = math.min(1.0, t / 0.60)
        local beckon = math.sin(progress * math.pi * 3.0) * 25.0
        j.spine.yaw = -20.0
        j.rightArm.pitch = 75.0 + beckon
        j.rightForearm.pitch = 45.0 + beckon * 0.5
        j.leftArm.pitch = 20.0
        j.leftForearm.pitch = 40.0
    end
end

return AnimationTree
