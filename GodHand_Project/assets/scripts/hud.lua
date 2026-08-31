-- hud.lua
-- 1:1 Authentic PS2 God Hand HUD (Bottom-Left Circular Skull Gauge, Health/Tension Bars, Radar & God Reel Menu)

local Difficulty = require("assets/scripts/difficulty_system")
local GodReel = require("assets/scripts/god_reel")
local Combat = require("assets/scripts/combat_engine")

local HUD = {}

function HUD.draw(player, enemies, stageWave, gameStageState)
    local screenW = window.get_width()
    local screenH = window.get_height()

    -- 1. Authentic PS2 Bottom-Left God Hand HUD Cluster (Skull Dial, HP, Tension, Orbs)
    HUD.drawClassicGodHandCluster(player, 35, screenH - 85)

    -- 2. Target Enemy Health Bar (Top-Center)
    HUD.drawTargetEnemy(player, enemies, screenW * 0.5 - 170, 20, 340, 16)

    -- 3. Top-Right Circular Compass Radar & Mini-Map
    HUD.drawTopRightRadar(player, enemies, screenW - 85, 80, 48)

    -- 4. Floating Combat Damage & Popups
    HUD.drawFloatingTexts()

    -- 5. God Reel Roulette Menu Overlay (Right-Aligned Stack)
    if GodReel.isOpen then
        HUD.drawGodReelMenu(screenW, screenH)
    end

    -- 6. Controls Legend (Bottom-Center)
    if not GodReel.isOpen then
        HUD.drawControlsLegend(screenW * 0.5 - 340, screenH - 22)
    end

    -- 7. Wave / Stage Announcements
    if gameStageState and gameStageState.announcementTimer > 0 then
        HUD.drawAnnouncement(screenW, screenH, gameStageState.announcementText, gameStageState.announcementSub)
    end
end

function HUD.drawClassicGodHandCluster(player, baseX, baseY)
    local level = Difficulty.level
    local levelName = Difficulty.getLevelName()

    -- 1. Circular Skull Level DIE Gauge (Far-Left Dial)
    local dialRadius = 36
    local dialCenterX = baseX + dialRadius
    local dialCenterY = baseY

    -- Outer Metallic Bronze/Silver Ring
    if gfx.draw_circle_filled then
        gfx.draw_circle_filled(dialCenterX, dialCenterY, dialRadius + 5, 0.12, 0.10, 0.14, 0.95)
    else
        gfx.draw_rect(dialCenterX - dialRadius, dialCenterY - dialRadius, dialRadius * 2, dialRadius * 2, 0.12, 0.10, 0.14, 0.95)
    end

    if gfx.draw_circle_outline then
        gfx.draw_circle_outline(dialCenterX, dialCenterY, dialRadius + 5, 2.5, 0.85, 0.72, 0.35, 1.0)
        gfx.draw_circle_outline(dialCenterX, dialCenterY, dialRadius - 2, 1.5, 0.35, 0.28, 0.20, 0.85)
    end

    -- Inner Skull Glow Background
    local skullColor = { 0.15, 0.95, 0.35 } -- Level 1: Luminous Green
    if level == 2 then
        skullColor = { 0.95, 0.85, 0.15 }  -- Level 2: Amber Yellow
    elseif level == 3 then
        skullColor = { 1.00, 0.45, 0.08 }  -- Level 3: Fiery Orange
    elseif level >= 4 then
        local pulse = 0.75 + math.sin(engine.get_time() * 14.0) * 0.25
        skullColor = { 1.00 * pulse, 0.08, 0.08 } -- Level DIE: Pulsing Crimson Demon
    end

    if gfx.draw_circle_filled then
        gfx.draw_circle_filled(dialCenterX, dialCenterY, dialRadius - 4, skullColor[1] * 0.25, skullColor[2] * 0.25, skullColor[3] * 0.25, 0.95)
    end

    -- Stylized Skull Icon Center
    gfx.draw_text("☠", dialCenterX - 14, dialCenterY - 18, 2.6, skullColor[1], skullColor[2], skullColor[3], 1.0)

    -- Level Badge Text in Magenta / Pink (e.g. "Lv.1", "Lv.DIE")
    local lvlText = "Lv." .. (level >= 4 and "DIE" or level)
    gfx.draw_text(lvlText, dialCenterX - 18, dialCenterY + 12, 1.25, 0.98, 0.32, 0.75, 1.0)

    -- 2. Floating God Reel Orbs (Positioned between Dial and Health Bar)
    local orbStartX = dialCenterX + dialRadius + 12
    for i = 1, 3 do
        local ox = orbStartX + (i - 1) * 22
        local oy = dialCenterY - 14
        if i <= GodReel.godReelCardsCount then
            local pulse = 0.85 + math.sin(engine.get_time() * 8.0 + i) * 0.15
            if gfx.draw_circle_filled then
                gfx.draw_circle_filled(ox, oy, 7.5, 1.0 * pulse, 0.35 * pulse, 0.35, 0.95)
            end
            if gfx.draw_circle_outline then
                gfx.draw_circle_outline(ox, oy, 7.5, 1.5, 1.0, 1.0, 1.0, 1.0)
            end
        else
            if gfx.draw_circle_outline then
                gfx.draw_circle_outline(ox, oy, 6.5, 1.2, 0.4, 0.4, 0.4, 0.6)
            end
        end
    end

    -- 3. Upper Gene Health Bar (Gold-Yellow Fill with Bronze Frame)
    local barX = dialCenterX + dialRadius + 8
    local hpY = dialCenterY - 2
    local barW = 210
    local barH = 14

    gfx.draw_rect(barX - 2, hpY - 2, barW + 4, barH + 4, 0.08, 0.06, 0.08, 0.95)
    gfx.draw_rect_outline(barX - 2, hpY - 2, barW + 4, barH + 4, 2.0, 0.85, 0.70, 0.25, 1.0)
    gfx.draw_rect(barX, hpY, barW, barH, 0.25, 0.05, 0.05, 0.90)

    local hpRatio = math.max(0.0, math.min(1.0, player.hp / player.maxHp))
    local hpFillW = barW * hpRatio
    if hpRatio > 0.35 then
        gfx.draw_rect(barX, hpY, hpFillW, barH, 1.0, 0.84, 0.16, 1.0)
    else
        local flash = 0.6 + math.sin(engine.get_time() * 12.0) * 0.4
        gfx.draw_rect(barX, hpY, hpFillW, barH, 1.0, 0.15 * flash, 0.15 * flash, 1.0)
    end
    gfx.draw_text("GENE", barX + 6, hpY + 1, 1.0, 0.05, 0.05, 0.08, 1.0)

    -- 4. Lower God Hand Tension Bar (Fiery Orange/Flame)
    local tensionY = dialCenterY + 16
    local tensionH = 12
    gfx.draw_rect(barX - 2, tensionY - 2, barW + 4, tensionH + 4, 0.08, 0.06, 0.08, 0.95)
    gfx.draw_rect_outline(barX - 2, tensionY - 2, barW + 4, tensionH + 4, 1.8, 0.70, 0.45, 0.15, 0.90)
    gfx.draw_rect(barX, tensionY, barW, tensionH, 0.18, 0.10, 0.06, 0.90)

    local tensionRatio = GodReel.tension / 100.0
    if GodReel.isGodHandActive then
        tensionRatio = GodReel.godHandTimer / GodReel.maxGodHandDuration
    end
    local tensionFillW = barW * math.max(0.0, math.min(1.0, tensionRatio))

    if GodReel.isGodHandActive then
        local pulse = 0.8 + math.sin(engine.get_time() * 16.0) * 0.2
        gfx.draw_rect(barX, tensionY, tensionFillW, tensionH, 1.0 * pulse, 0.90 * pulse, 0.20, 1.0)
        gfx.draw_text("★ GOD HAND ACTIVE! ★", barX + 24, tensionY + 1, 0.95, 0.05, 0.05, 0.08, 1.0)
    else
        gfx.draw_rect(barX, tensionY, tensionFillW, tensionH, 1.0, 0.46, 0.08, 1.0)
        if GodReel.tension >= 100.0 then
            local pulse = 0.7 + math.sin(engine.get_time() * 10.0) * 0.3
            gfx.draw_text("★ GOD HAND READY! [F] ★", barX + 16, tensionY + 1, 0.95, 1.0, 0.95, 0.20, pulse)
        else
            gfx.draw_text("TENSION", barX + 6, tensionY + 1, 0.85, 1.0, 1.0, 1.0, 0.85)
        end
    end
end

function HUD.drawTopRightRadar(player, enemies, centerX, centerY, radius)
    -- Circular Radar Compass
    if gfx.draw_circle_filled then
        gfx.draw_circle_filled(centerX, centerY, radius, 0.06, 0.05, 0.08, 0.85)
    end
    if gfx.draw_circle_outline then
        gfx.draw_circle_outline(centerX, centerY, radius, 2.2, 0.90, 0.75, 0.25, 0.95)
        gfx.draw_circle_outline(centerX, centerY, radius * 0.5, 1.0, 0.40, 0.35, 0.20, 0.50)
    end

    -- "0 G" Emblem Top-Right Header (1:1 with image.png)
    gfx.draw_text("0 G", centerX + radius - 16, centerY - radius - 14, 1.4, 0.95, 0.82, 0.20, 1.0)

    -- Player Center Indicator
    if gfx.draw_circle_filled then
        gfx.draw_circle_filled(centerX, centerY, 3.5, 0.20, 0.85, 0.95, 1.0)
    end

    -- Blip Enemies on Radar
    if enemies then
        for _, e in ipairs(enemies) do
            if e.hp > 0 and e.state ~= "dead" then
                local dx = e.pos.x - player.pos.x
                local dz = e.pos.z - player.pos.z
                local dist = math.sqrt(dx * dx + dz * dz)

                if dist < 60.0 then
                    local blipX = centerX + (dx / 60.0) * (radius - 6)
                    local blipY = centerY + (dz / 60.0) * (radius - 6)
                    local blipColor = (e.type == "boss") and { 1.0, 0.1, 0.8 } or (e.type == "bruiser" and { 1.0, 0.4, 0.1 } or { 1.0, 0.2, 0.2 })
                    if gfx.draw_circle_filled then
                        gfx.draw_circle_filled(blipX, blipY, (e.type == "boss") and 4.0 or 2.5, blipColor[1], blipColor[2], blipColor[3], 0.95)
                    end
                end
            end
        end
    end
end

function HUD.drawGodReelMenu(screenW, screenH)
    local cardW = 340
    local cardH = 54
    local startX = screenW - cardW - 70
    local startY = screenH * 0.36

    -- Circular TIME Gauge Dial
    local timeDialX = startX - 52
    local timeDialY = startY + 54
    local timeDialR = 30
    if gfx.draw_circle_filled then
        gfx.draw_circle_filled(timeDialX, timeDialY, timeDialR, 0.10, 0.08, 0.12, 0.92)
    end
    if gfx.draw_circle_outline then
        gfx.draw_circle_outline(timeDialX, timeDialY, timeDialR, 2.5, 0.95, 0.78, 0.20, 1.0)
    end
    gfx.draw_text("TIME", timeDialX - 16, timeDialY + timeDialR + 6, 1.15, 0.95, 0.85, 0.25, 1.0)

    -- Sweep Hand for Roulette Time
    local timeRatio = GodReel.rouletteTimer / GodReel.maxRouletteTime
    local handAngle = math.rad(timeRatio * 360.0 - 90.0)
    local handX = timeDialX + math.cos(handAngle) * (timeDialR - 6)
    local handY = timeDialY + math.sin(handAngle) * (timeDialR - 6)
    if gfx.draw_line then
        gfx.draw_line(timeDialX, timeDialY, handX, handY, 2.5, 1.0, 0.40, 0.10, 1.0)
    end

    -- 3 Visible God Reel Cards
    for i = 1, 3 do
        local cardIdx = ((GodReel.selectedIndex - 2 + i - 1) % #GodReel.cards) + 1
        local card = GodReel.cards[cardIdx]
        if card then
            local isSelected = (cardIdx == GodReel.selectedIndex)
            local cy = startY + (i - 1) * (cardH + 12)

            if isSelected then
                gfx.draw_rect(startX - 4, cy - 4, cardW + 8, cardH + 8, 0.20, 0.12, 0.04, 0.95)
                gfx.draw_rect(startX, cy, cardW, cardH, 0.45, 0.28, 0.10, 0.95)
                gfx.draw_rect_outline(startX, cy, cardW, cardH, 3.0, 1.0, 0.85, 0.25, 1.0)
                gfx.draw_text(card.name, startX + 22, cy + 12, 2.1, 1.0, 0.92, 0.35, 1.0)
            else
                gfx.draw_rect(startX, cy, cardW, cardH, 0.18, 0.14, 0.10, 0.80)
                gfx.draw_rect_outline(startX, cy, cardW, cardH, 1.5, 0.65, 0.50, 0.20, 0.75)
                gfx.draw_text(card.name, startX + 22, cy + 14, 1.6, 0.75, 0.65, 0.45, 0.85)
            end
        end
    end

    -- PS2 Controller Prompt Legend at Bottom (R1 Select, Cross OK, Triangle Cancel)
    local promptY = startY + 3 * (cardH + 12) + 16
    gfx.draw_rect(startX - 20, promptY - 4, cardW + 40, 36, 0.05, 0.05, 0.08, 0.88)
    gfx.draw_text("R1 Select   ✕ OK   ◯ OK   △ Cancel", startX - 10, promptY + 6, 1.15, 0.95, 0.95, 0.95, 0.95)
end

function HUD.drawTargetEnemy(player, enemies, x, y, w, h)
    if not enemies then return end

    local nearestEnemy = nil
    local nearestDist = 999.0

    for _, e in ipairs(enemies) do
        if e.hp > 0 and e.state ~= "dead" then
            local dx = e.pos.x - player.pos.x
            local dz = e.pos.z - player.pos.z
            local dist = math.sqrt(dx * dx + dz * dz)
            if dist < nearestDist and dist < 28.0 then
                nearestDist = dist
                nearestEnemy = e
            end
        end
    end

    if not nearestEnemy then return end

    gfx.draw_rect(x - 3, y - 3, w + 6, h + 6, 0.04, 0.04, 0.06, 0.90)
    gfx.draw_rect_outline(x - 3, y - 3, w + 6, h + 6, 2.0, 0.85, 0.20, 0.15, 0.95)
    gfx.draw_rect(x, y, w, h, 0.22, 0.05, 0.05, 0.90)

    local ratio = math.max(0.0, math.min(1.0, nearestEnemy.hp / nearestEnemy.maxHp))
    local fillW = w * ratio
    gfx.draw_rect(x, y, fillW, h, 0.95, 0.18, 0.15, 1.0)

    local nameTxt = string.upper(nearestEnemy.type) .. " HP"
    gfx.draw_text(nameTxt, x + 10, y + 1, 1.05, 1.0, 1.0, 1.0, 1.0)
    gfx.draw_text(math.floor(nearestEnemy.hp) .. " / " .. nearestEnemy.maxHp, x + w - 75, y + 1, 0.95, 1.0, 1.0, 1.0, 0.95)
end

function HUD.drawControlsLegend(x, y)
    gfx.draw_text("WASD: 3D Movement | Shift: Sprint | Alt: Walk | Space: Jump | Mouse: 360° Look | J: Combo | K: GuardBreak | L: Launcher | F: GodHand | Tab: GodReel", x, y, 0.92, 0.90, 0.90, 0.90, 0.85)
end

function HUD.drawFloatingTexts()
    for _, t in ipairs(Combat.floatingTexts) do
        local screenX = 500 + t.x * 24.0
        local screenY = 320 - t.y * 22.0 - t.z * 10.0
        local alpha = math.max(0.0, t.life / t.maxLife)
        gfx.draw_text(t.text, screenX, screenY, t.size, t.color[1], t.color[2], t.color[3], alpha)
    end
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
