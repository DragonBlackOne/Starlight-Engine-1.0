-- god_reel.lua
-- 1:1 Authentic PS2 God Hand God Reel (Roulette Wheel) & Tension Unleash System

local SFX = require("assets/scripts/sfx_manager")
local Combat = require("assets/scripts/combat_engine")

local GodReel = {
    isOpen = false,
    selectedIndex = 1,
    rouletteTimer = 8.0,
    maxRouletteTime = 8.0,
    cards = {
        { name = "GOD STOMP", cost = 1, desc = "Devastating diving heel smash crushing enemies into the dirt!", damage = 220, range = 5.0, isKnockdown = true, isHeavy = true, isGuardBreak = true, type = "special" },
        { name = "LA BOMBA", cost = 2, desc = "Massive explosive holy blast launching surrounding foes!", damage = 340, range = 6.5, isLauncher = true, isHeavy = true, launchHeight = 14.0, type = "special" },
        { name = "DIVINE SMASH", cost = 1, desc = "High-flying overhead fist strike creating shockwaves!", damage = 190, range = 5.5, isHeavy = true, isGuardBreak = true, type = "special" },
        { name = "BALL BUSTER", cost = 1, desc = "Legendary groin kick. Instantly causes Dizzy state for PUMMEL!", damage = 140, range = 3.5, isGuardBreak = true, isKnockdown = false, dizzyValue = 100, type = "special" },
        { name = "100-FIST RUSH", cost = 3, desc = "Supersonic 100-punch barrage + explosive dragon finisher!", damage = 480, range = 7.0, isLauncher = true, isHeavy = true, isGuardBreak = true, type = "special" },
        { name = "DRAGON KICK", cost = 2, desc = "Fiery explosive roundhouse sending target into orbit!", damage = 310, range = 5.0, isLauncher = true, isHeavy = true, launchHeight = 16.0, type = "special" }
    },
    godReelCardsCount = 3,
    maxCards = 5,

    -- God Hand Tension Meter (0.0 to 100.0)
    tension = 0.0,
    isGodHandActive = false,
    godHandTimer = 0.0,
    maxGodHandDuration = 12.0
}

function GodReel.init()
    GodReel.isOpen = false
    GodReel.selectedIndex = 1
    GodReel.rouletteTimer = 8.0
    GodReel.godReelCardsCount = 3
    GodReel.tension = 0.0
    GodReel.isGodHandActive = false
    GodReel.godHandTimer = 0.0
    if time and time.set_scale then
        time.set_scale(1.0)
    end
end

function GodReel.addTension(amount)
    if not GodReel.isGodHandActive then
        GodReel.tension = math.min(100.0, GodReel.tension + amount)
    end
end

function GodReel.toggle(player)
    if GodReel.isGodHandActive then return end

    if not GodReel.isOpen then
        GodReel.isOpen = true
        GodReel.selectedIndex = 1
        GodReel.rouletteTimer = 8.0
        if time and time.set_scale then
            time.set_scale(0.06) -- Matrix Slow-Motion
        end
        SFX.playGodReelOpen()
    else
        GodReel.close()
    end
end

function GodReel.close()
    GodReel.isOpen = false
    if time and time.set_scale then
        time.set_scale(1.0)
    end
end

function GodReel.nextCard()
    GodReel.selectedIndex = (GodReel.selectedIndex % #GodReel.cards) + 1
    SFX.playGodReelSelect()
end

function GodReel.prevCard()
    GodReel.selectedIndex = GodReel.selectedIndex - 1
    if GodReel.selectedIndex < 1 then GodReel.selectedIndex = #GodReel.cards end
    SFX.playGodReelSelect()
end

function GodReel.selectAndExecute(player, enemies)
    local card = GodReel.cards[GodReel.selectedIndex]
    if not card then return false end

    if GodReel.godReelCardsCount < card.cost then
        if audio and audio.beep then
            audio.beep(150.0, 0.1, 2)
        end
        return false
    end

    GodReel.godReelCardsCount = GodReel.godReelCardsCount - card.cost
    GodReel.close()

    SFX.playGodReelExecute()
    Combat.addFloatingText(card.name .. "!!", player.pos.x, player.pos.y + 2.4, player.pos.z, { 1.0, 0.85, 0.1, 1.0 }, 3.0)

    player.executeSpecial(card, enemies)
    return true
end

function GodReel.unleashGodHand(player)
    if GodReel.tension >= 100.0 and not GodReel.isGodHandActive then
        GodReel.isGodHandActive = true
        GodReel.godHandTimer = GodReel.maxGodHandDuration
        GodReel.tension = 0.0
        player.isGodHand = true
        player.isInvincible = true

        if player.rig then
            local HumanoidRig = require("assets/scripts/humanoid_rig")
            HumanoidRig.setGodHandMode(player.rig, true)
        end

        SFX.playGodHandActivate()
        Combat.addFloatingText("★ GOD HAND UNLEASHED!! ★", player.pos.x, player.pos.y + 2.5, player.pos.z, { 1.0, 0.9, 0.1, 1.0 }, 3.2)
        vfx.burst(player.pos.x, player.pos.y + 1.2, player.pos.z, 1.0, 0.85, 0.15, 45, 8.5, 0.30, 0.9)
        return true
    end
    return false
end

function GodReel.update(dt, player)
    if GodReel.isOpen then
        GodReel.rouletteTimer = GodReel.rouletteTimer - (dt / (time.get_scale and time.get_scale() or 1.0)) * 0.06
        if GodReel.rouletteTimer <= 0 then
            GodReel.close()
        end
    end

    if GodReel.isGodHandActive then
        GodReel.godHandTimer = GodReel.godHandTimer - dt

        if player then
            local rad = math.rad(player.facingAngle or 0)
            local fwdX = -math.sin(rad)
            local fwdZ = -math.cos(rad)
            local rightX = math.cos(rad)
            local rightZ = -math.sin(rad)

            local armX = player.pos.x + rightX * 0.45 + fwdX * 0.2
            local armY = player.pos.y + 1.1
            local armZ = player.pos.z + rightZ * 0.45 + fwdZ * 0.2

            vfx.emit_trail(armX, armY, armZ, 1.0, 0.85, 0.15, 0.20, 0.40)
            if math.random() < 0.3 then
                vfx.burst(armX, armY, armZ, 1.0, 0.9, 0.3, 4, 1.5, 0.08, 0.2)
            end
        end

        if GodReel.godHandTimer <= 0 then
            GodReel.isGodHandActive = false
            if player then
                player.isGodHand = false
                player.isInvincible = false
                if player.rig then
                    local HumanoidRig = require("assets/scripts/humanoid_rig")
                    HumanoidRig.setGodHandMode(player.rig, false)
                end
                Combat.addFloatingText("GOD HAND EXPIRED", player.pos.x, player.pos.y + 2.0, player.pos.z, { 0.8, 0.8, 0.8, 1.0 }, 1.8)
            end
        end
    end
end

return GodReel
