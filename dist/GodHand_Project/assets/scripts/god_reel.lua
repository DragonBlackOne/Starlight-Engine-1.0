-- god_reel.lua
-- God Reel (Roulette Wheel) & God Hand Tension Unleash System

local SFX = require("assets/scripts/sfx_manager")
local Combat = require("assets/scripts/combat_engine")

local GodReel = {
    isOpen = false,
    selectedIndex = 1,
    cards = {
        { name = "GOD IMPACT", cost = 1, desc = "Massive shockwave punch. Blasts enemies across the arena!", damage = 190, range = 6.0, isLauncher = true, isHeavy = true, isGuardBreak = true, type = "special" },
        { name = "DRAGON KICK", cost = 2, desc = "Fiery explosive roundhouse sending target into orbit!", damage = 300, range = 5.0, isLauncher = true, isHeavy = true, launchHeight = 15.0, type = "special" },
        { name = "BALL BUSTER", cost = 1, desc = "Low blow groin kick. Instantly causes Dizzy state for PUMMEL!", damage = 130, range = 3.5, isGuardBreak = true, isKnockdown = false, dizzyValue = 100, type = "special" },
        { name = "100-FIST RUSH", cost = 3, desc = "Supersonic 50-punch barrage + explosive dragon uppercut!", damage = 460, range = 6.5, isLauncher = true, isHeavy = true, isGuardBreak = true, type = "special" },
        { name = "DAISY CUTTER", cost = 2, desc = "360-degree ground shockwave tripping all surrounding foes!", damage = 220, range = 7.0, isKnockdown = true, isHeavy = true, type = "special" },
        { name = "SHAOLIN BLAST", cost = 2, desc = "Divine Chi beam slicing through enemy ranks!", damage = 280, range = 8.5, isLauncher = true, isGuardBreak = true, type = "special" }
    },
    godReelCardsCount = 3, -- Player starts with 3 God Reel orbs/cards
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
        if time and time.set_scale then
            time.set_scale(0.08) -- Matrix Slow-Motion
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
        -- Not enough cards!
        if audio and audio.beep then
            audio.beep(150.0, 0.1, 2)
        end
        return false
    end

    GodReel.godReelCardsCount = GodReel.godReelCardsCount - card.cost
    GodReel.close()

    SFX.playGodReelExecute()
    Combat.addFloatingText(card.name .. "!!", player.pos.x, player.pos.y + 2.4, player.pos.z, { 1.0, 0.85, 0.1, 1.0 }, 3.0)

    -- Trigger special move animation and execution on player
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

        SFX.playGodHandActivate()
        Combat.addFloatingText("★ GOD HAND UNLEASHED!! ★", player.pos.x, player.pos.y + 2.5, player.pos.z, { 1.0, 0.9, 0.1, 1.0 }, 3.2)

        -- Initial holy golden shockwave burst
        vfx.burst(player.pos.x, player.pos.y + 1.2, player.pos.z, 1.0, 0.85, 0.15, 45, 8.5, 0.30, 0.9)
        return true
    end
    return false
end

function GodReel.update(dt, player)
    -- Update God Hand Unleash Duration
    if GodReel.isGodHandActive then
        GodReel.godHandTimer = GodReel.godHandTimer - dt

        -- Constant golden arm aura particles
        local rad = math.rad(player.facingAngle)
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

        if GodReel.godHandTimer <= 0 then
            GodReel.isGodHandActive = false
            player.isGodHand = false
            player.isInvincible = false
            Combat.addFloatingText("GOD HAND EXPIRED", player.pos.x, player.pos.y + 2.0, player.pos.z, { 0.8, 0.8, 0.8, 1.0 }, 1.8)
        end
    end
end

return GodReel
