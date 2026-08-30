-- hud.lua
-- Retro PS2 God Hand HUD (Gene HP, Level DIE Gauge, Tension Flame, God Reel, Enemy Bars)

local Difficulty = require("assets/scripts/difficulty_system")
local GodReel = require("assets/scripts/god_reel")
local Combat = require("assets/scripts/combat_engine")

local HUD = {}

function HUD.draw(player, enemies, stageWave, gameStageState)
    local screenW = window.get_width()
    local screenH = window.get_height()

    -- 1. Gene Player HP Bar (Top-Left)
    HUD.drawPlayerHealth(player, 25, 25, 300, 24)

    -- 2. God Hand Tension Meter (Below HP)
    HUD.drawTensionMeter(player, 25, 58, 260, 18)

    -- 3. God Reel Card Counter (Below Tension)
    HUD.drawGodReelCards(25, 84)

    -- 4. Target Enemy Health Bar (Top-Center)
    HUD.drawTargetEnemy(player, enemies, screenW * 0.5 - 170, 25, 340, 18)

    -- 5. Level DIE Dynamic Difficulty Gauge (Bottom-Right)
    HUD.drawDifficultyGauge(screenW - 250, screenH - 120, 225, 95)

    -- 6. Combo Counter (Right Side)
    HUD.drawComboCounter(player, screenW - 250, screenH * 0.45)

    -- 7. Floating Combat Popups
    HUD.drawFloatingTexts()

    -- 8. God Reel Roulette Menu Overlay
    if GodReel.isOpen then
        HUD.drawGodReelMenu(screenW, screenH)
    end

    -- 9. Control Legend / Help Bar (Bottom-Left)
    if not GodReel.isOpen then
        HUD.drawControlsLegend(25, screenH - 35)
    end

    -- 10. Wave / Stage Announcements
    if gameStageState and gameStageState.announcementTimer > 0 then
        HUD.drawAnnouncement(screenW, screenH, gameStageState.announcementText, gameStageState.announcementSub)
    end
end

function HUD.drawPlayerHealth(player, x, y, w, h)
    -- Border & Dark Backing
    gfx.draw_rect(x - 4, y - 4, w + 8, h + 8, 0.04, 0.04, 0.06, 0.95)
    gfx.draw_rect_outline(x - 4, y - 4, w + 8, h + 8, 2.5, 0.90, 0.70, 0.20, 1.0)

    -- Empty HP Background
    gfx.draw_rect(x, y, w, h, 0.25, 0.06, 0.06, 0.90)

    -- Fill Ratio
    local ratio = math.max(0.0, math.min(1.0, player.hp / player.maxHp))
    local fillW = w * ratio

    -- Yellow-Orange to Green Gradient Fill
    if ratio > 0.45 then
        gfx.draw_rect(x, y, fillW, h, 1.0, 0.85, 0.15, 1.0) -- Gold Yellow
    elseif ratio > 0.20 then
        gfx.draw_rect(x, y, fillW, h, 1.0, 0.45, 0.10, 1.0) -- Warning Orange
    else
        local flash = 0.6 + math.sin(engine.get_time() * 12.0) * 0.4
        gfx.draw_rect(x, y, fillW, h, 1.0, 0.15 * flash, 0.15 * flash, 1.0) -- Critical Red Flash
    end

    -- Label
    gfx.draw_text("GENE", x + 10, y + 4, 1.3, 0.05, 0.05, 0.08, 1.0)
    gfx.draw_text(math.floor(player.hp) .. " / " .. player.maxHp, x + w - 85, y + 4, 1.15, 1.0, 1.0, 1.0, 0.95)
end

function HUD.drawTensionMeter(player, x, y, w, h)
    gfx.draw_rect(x - 3, y - 3, w + 6, h + 6, 0.04, 0.04, 0.06, 0.90)
    gfx.draw_rect_outline(x - 3, y - 3, w + 6, h + 6, 1.8, 0.70, 0.45, 0.10, 0.85)

    local tensionRatio = GodReel.tension / 100.0
    if GodReel.isGodHandActive then
        tensionRatio = GodReel.godHandTimer / GodReel.maxGodHandDuration
    end

    local fillW = w * math.max(0.0, math.min(1.0, tensionRatio))

    if GodReel.isGodHandActive then
        -- Pulsing Divine Gold
        local pulse = 0.8 + math.sin(engine.get_time() * 14.0) * 0.2
        gfx.draw_rect(x, y, fillW, h, 1.0 * pulse, 0.88 * pulse, 0.15, 1.0)
        gfx.draw_text("★ GOD HAND ACTIVE (" .. string.format("%.1f", GodReel.godHandTimer) .. "s) ★", x + 8, y + 2, 1.05, 0.05, 0.05, 0.08, 1.0)
    else
        gfx.draw_rect(x, y, fillW, h, 1.0, 0.48, 0.08, 0.92)
        if GodReel.tension >= 100.0 then
            local pulse = 0.7 + math.sin(engine.get_time() * 10.0) * 0.3
            gfx.draw_text("★ GOD HAND READY! [F] ★", x + 8, y + 2, 1.15, 1.0, 0.95, 0.20, pulse)
        else
            gfx.draw_text("TENSION " .. math.floor(GodReel.tension) .. "%", x + 8, y + 2, 1.0, 1.0, 1.0, 1.0, 0.85)
        end
    end
end

function HUD.drawGodReelCards(x, y)
    gfx.draw_text("GOD REEL: ", x, y, 1.15, 1.0, 0.88, 0.22, 1.0)
    for i = 1, GodReel.maxCards do
        local cx = x + 85 + (i - 1) * 25
        if i <= GodReel.godReelCardsCount then
            -- Golden Card Orb
            gfx.draw_rect(cx, y + 2, 18, 18, 1.0, 0.85, 0.15, 1.0)
            gfx.draw_rect_outline(cx, y + 2, 18, 18, 1.5, 1.0, 1.0, 0.8, 1.0)
        else
            -- Empty slot
            gfx.draw_rect(cx, y + 2, 18, 18, 0.18, 0.14, 0.10, 0.6)
            gfx.draw_rect_outline(cx, y + 2, 18, 18, 1.0, 0.4, 0.3, 0.2, 0.6)
        end
    end
end

function HUD.drawTargetEnemy(player, enemies, x, y, w, h)
    -- Find closest active enemy
    local closest = nil
    local minDist = 9.0
    for _, e in ipairs(enemies) do
        if e.hp > 0 then
            local dx = e.pos.x - player.pos.x
            local dz = e.pos.z - player.pos.z
            local d = math.sqrt(dx * dx + dz * dz)
            if d < minDist then
                minDist = d
                closest = e
            end
        end
    end

    if closest then
        local name = string.upper(closest.type)
        if closest.type == "boss" then name = "MAD MIDGET BOSS"
        elseif closest.type == "bruiser" then name = "HEAVY BRUISER"
        else name = "THUG PUNK"
        end

        gfx.draw_rect(x - 3, y - 3, w + 6, h + 6, 0.04, 0.04, 0.06, 0.90)
        gfx.draw_rect_outline(x - 3, y - 3, w + 6, h + 6, 2.0, 0.85, 0.20, 0.20, 0.85)

        local ratio = math.max(0.0, math.min(1.0, closest.hp / closest.maxHp))
        gfx.draw_rect(x, y, w * ratio, h, 0.95, 0.18, 0.18, 1.0)

        gfx.draw_text(name, x + 10, y + 2, 1.15, 1.0, 1.0, 1.0, 1.0)
        gfx.draw_text(math.floor(closest.hp) .. " / " .. closest.maxHp, x + w - 75, y + 2, 1.05, 1.0, 1.0, 0.8, 0.95)

        -- Dizzy Bar below if building up
        if closest.dizzyMeter > 0 and closest.state ~= "dizzy" then
            local dizzyRatio = math.min(1.0, closest.dizzyMeter / 100.0)
            gfx.draw_rect(x, y + h + 4, w * dizzyRatio, 5, 1.0, 0.85, 0.15, 0.95)
        elseif closest.state == "dizzy" then
            local pulse = 0.5 + math.sin(engine.get_time() * 12.0) * 0.5
            gfx.draw_rect(x, y + h + 4, w, 6, 1.0, 0.9, 0.1, pulse)
            gfx.draw_text(">> DIZZY! PRESS [J] TO PUMMEL! <<", x + 40, y + h + 12, 1.15, 1.0, 0.9, 0.1, 1.0)
        end
    end
end

function HUD.drawDifficultyGauge(x, y, w, h)
    local isDie = Difficulty.isLevelDie()

    -- Outer Panel
    local borderCol = isDie and { 1.0, 0.15, 0.15, 1.0 } or { 1.0, 0.85, 0.15, 1.0 }
    gfx.draw_rect(x, y, w, h, 0.05, 0.03, 0.07, 0.92)
    gfx.draw_rect_outline(x, y, w, h, 2.5, borderCol[1], borderCol[2], borderCol[3], borderCol[4])

    -- Level Title
    local lvlText = Difficulty.getLevelName()
    local textCol = isDie and { 1.0, 0.20, 0.20, 1.0 } or { 1.0, 0.90, 0.20, 1.0 }
    gfx.draw_text(lvlText, x + 15, y + 10, 1.7, textCol[1], textCol[2], textCol[3], 1.0)

    -- Progress Bar
    local barW = w - 30
    local barH = 15
    gfx.draw_rect(x + 15, y + 40, barW, barH, 0.15, 0.10, 0.08, 0.85)

    local gaugeRatio = math.max(0.0, math.min(1.0, Difficulty.gauge / 100.0))
    if isDie then
        local flamePulse = 0.8 + math.sin(engine.get_time() * 10.0) * 0.2
        gfx.draw_rect(x + 15, y + 40, barW, barH, 1.0 * flamePulse, 0.15, 0.15, 0.95)
    else
        gfx.draw_rect(x + 15, y + 40, barW * gaugeRatio, barH, 0.20, 0.85, 1.0, 0.95)
    end

    -- Multiplier & Score
    local mult = Difficulty.getScoreMultiplier()
    gfx.draw_text("SCORE: " .. Difficulty.score, x + 15, y + 64, 1.15, 1.0, 1.0, 1.0, 0.95)
    gfx.draw_text("MULT: " .. string.format("%.1fx", mult), x + w - 90, y + 64, 1.15, textCol[1], textCol[2], textCol[3], 1.0)
end

function HUD.drawComboCounter(player, x, y)
    if player.comboStep > 0 and player.comboWindow > 0 then
        local hits = player.comboStep
        local txt = hits .. " HITS!"
        if hits >= 5 then txt = "5 HITS! DRAGON UPPERCUT!" end
        gfx.draw_text(txt, x, y, 1.9, 1.0, 0.85, 0.15, 1.0)
    end
end

function HUD.drawFloatingTexts()
    for _, t in ipairs(Combat.floatingTexts) do
        local screenX = 500 + t.x * 24.0
        local screenY = 320 - t.y * 22.0 - t.z * 10.0
        local alpha = math.max(0.0, t.life / t.maxLife)
        gfx.draw_text(t.text, screenX, screenY, t.size, t.color[1], t.color[2], t.color[3], alpha)
    end
end

function HUD.drawGodReelMenu(screenW, screenH)
    -- Darkened Matrix Backdrop
    gfx.draw_rect(0, 0, screenW, screenH, 0.0, 0.0, 0.0, 0.70)

    -- Center Roulette Box
    local boxW = 580
    local boxH = 420
    local bx = (screenW - boxW) * 0.5
    local by = (screenH - boxH) * 0.5

    gfx.draw_rect(bx, by, boxW, boxH, 0.08, 0.05, 0.12, 0.96)
    gfx.draw_rect_outline(bx, by, boxW, boxH, 3.0, 1.0, 0.85, 0.15, 1.0)

    -- Header
    gfx.draw_text("★ GOD REEL ROULETTE ★", bx + 130, by + 18, 2.1, 1.0, 0.90, 0.20, 1.0)
    gfx.draw_text("CARDS AVAILABLE: " .. GodReel.godReelCardsCount, bx + 190, by + 52, 1.25, 1.0, 1.0, 1.0, 0.85)

    -- Card Cards Carousel
    for i, card in ipairs(GodReel.cards) do
        local isSelected = (i == GodReel.selectedIndex)
        local cardY = by + 80 + (i - 1) * 50

        if isSelected then
            gfx.draw_rect(bx + 20, cardY, boxW - 40, 44, 0.95, 0.75, 0.15, 0.95)
            gfx.draw_text("▶ " .. card.name .. " (COST: " .. card.cost .. " ORBS)", bx + 35, cardY + 6, 1.35, 0.05, 0.05, 0.08, 1.0)
            gfx.draw_text(card.desc, bx + 45, cardY + 24, 0.95, 0.15, 0.15, 0.15, 0.95)
        else
            gfx.draw_rect(bx + 20, cardY, boxW - 40, 44, 0.15, 0.12, 0.18, 0.75)
            gfx.draw_text("  " .. card.name .. " (COST: " .. card.cost .. " ORBS)", bx + 35, cardY + 6, 1.25, 0.85, 0.85, 0.85, 0.85)
            gfx.draw_text(card.desc, bx + 45, cardY + 24, 0.95, 0.60, 0.60, 0.60, 0.75)
        end
    end

    -- Footer Navigation Instructions
    gfx.draw_text("[UP / DOWN / I / K] Navigate     [J / ENTER] EXECUTE     [TAB / ESC] Cancel", bx + 50, by + boxH - 28, 1.15, 1.0, 0.85, 0.20, 1.0)
end

function HUD.drawControlsLegend(x, y)
    gfx.draw_text("WASD: Move/Turn | IJKL/Shift: Dodges (Duck/Sway/Backflip) | J: Combo | K: GuardBreak | L: Launcher | T: Taunt | Space: Grovel | Tab: GodReel | F: GodHand", x, y, 0.95, 0.90, 0.90, 0.90, 0.80)
end

function HUD.drawAnnouncement(screenW, screenH, mainTxt, subTxt)
    local y = screenH * 0.35
    gfx.draw_rect(0, y - 12, screenW, 95, 0.0, 0.0, 0.0, 0.80)
    gfx.draw_text(mainTxt or "", screenW * 0.5 - 210, y + 10, 2.9, 1.0, 0.85, 0.15, 1.0)
    if subTxt then
        gfx.draw_text(subTxt, screenW * 0.5 - 150, y + 58, 1.55, 1.0, 1.0, 1.0, 0.95)
    end
end

return HUD
