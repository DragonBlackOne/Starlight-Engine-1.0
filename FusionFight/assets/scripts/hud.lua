local HUD = {}

local p1Catchup = 1000
local p2Catchup = 1000
local comboPulseP1 = 0
local comboPulseP2 = 0
local lastComboP1 = 0
local lastComboP2 = 0
local comboScaleP1 = 1.0
local comboScaleP2 = 1.0

local function draw_health_fill(x, y, w, h, r, g, b, isLeftAligned, isRightAligned)
    if w <= 1.5 then return end
    
    -- Main backing
    gfx.draw_rect(x, y, w, h, r * 0.6, g * 0.6, b * 0.6, 1.0)
    -- Top shadow
    gfx.draw_rect(x, y, w, h * 0.25, r * 0.7, g * 0.7, b * 0.7, 1.0)
    -- Middle glowing core
    local cr = math.min(1.0, r * 1.4 + 0.1)
    local cg = math.min(1.0, g * 1.4 + 0.1)
    local cb = math.min(1.0, b * 1.4 + 0.1)
    gfx.draw_rect(x, y + h * 0.25, w, h * 0.5, cr, cg, cb, 1.0)
    -- Highlights shine line
    gfx.draw_rect(x, y + h * 0.35, w, h * 0.12, 1.0, 1.0, 1.0, 0.45)
    -- Bottom shadow
    gfx.draw_rect(x, y + h * 0.75, w, h * 0.25, r * 0.45, g * 0.45, b * 0.45, 1.0)
    
    -- Rounding overlays for ends of the health bar
    local radius = 4
    if isLeftAligned then
        gfx.draw_rounded_rect(x, y, radius * 2, h, radius, r * 0.6, g * 0.6, b * 0.6, 1.0)
        gfx.draw_rounded_rect(x, y, radius * 2, h * 0.25, radius, r * 0.7, g * 0.7, b * 0.7, 1.0)
        gfx.draw_rounded_rect(x, y + h * 0.25, radius * 2, h * 0.5, radius, cr, cg, cb, 1.0)
        gfx.draw_rounded_rect(x, y + h * 0.75, radius * 2, h * 0.25, radius, r * 0.45, g * 0.45, b * 0.45, 1.0)
    elseif isRightAligned then
        local rx = x + w - radius * 2
        gfx.draw_rounded_rect(rx, y, radius * 2, h, radius, r * 0.6, g * 0.6, b * 0.6, 1.0)
        gfx.draw_rounded_rect(rx, y, radius * 2, h * 0.25, radius, r * 0.7, g * 0.7, b * 0.7, 1.0)
        gfx.draw_rounded_rect(rx, y + h * 0.25, radius * 2, h * 0.5, radius, cr, cg, cb, 1.0)
        gfx.draw_rounded_rect(rx, y + h * 0.75, radius * 2, h * 0.25, radius, r * 0.45, g * 0.45, b * 0.45, 1.0)
    end
end

function HUD.Draw()
    local screenW = window.get_width()
    local screenH = window.get_height()
    
    local p1Health = fight.get_p1_health()
    local p2Health = fight.get_p2_health()
    local p1Max = fight.get_p1_max_health()
    local p2Max = fight.get_p2_max_health()
    
    local timer = fight.get_timer()
    local round = fight.get_round()
    local p1Score = fight.get_p1_score()
    local p2Score = fight.get_p2_score()
    local state = fight.get_state()
    
    local p1Combo = fight.get_p1_combo()
    local p2Combo = fight.get_p2_combo()
    
    local p1Name = fight.get_p1_name()
    local p2Name = fight.get_p2_name()
    
    -- Smooth catchup health
    p1Catchup = p1Catchup - (p1Catchup - p1Health) * 0.05
    p2Catchup = p2Catchup - (p2Catchup - p2Health) * 0.05
    
    -- ─── HUD DIMENSIONS ──────────────────────────────────────────────────────
    local barW = 420
    local barH = 22
    local exH = 6
    local totalCapH = barH + exH -- integrated height (touching directly)
    local barY = 34
    local p1X = 80
    local p2X = screenW - 80 - barW
    
    -- Glassmorphic background top banner (subtle overlay)
    gfx.draw_rect(p1X - 70, barY - 26, screenW - (p1X - 70)*2, barH + 60, 0.04, 0.02, 0.08, 0.35)
    gfx.draw_rect(p1X - 70, barY + barH + 28, screenW - (p1X - 70)*2, 1.5, 0.85, 0.0, 0.85, 0.2)
    
    -- ─── CHARACTER NAMES ──────────────────────────────────────────────────────
    -- White retro pixel font above health bars
    gfx.draw_text(p1Name, p1X + 2, barY - 18, 1.2, 1.0, 1.0, 1.0, 0.95)
    
    local p2NameW = #p2Name * 10
    gfx.draw_text(p2Name, p2X + barW - p2NameW - 2, barY - 18, 1.2, 1.0, 1.0, 1.0, 0.95)
    
    -- ─── PLAYER PORTRAITS ────────────────────────────────────────────────────
    -- Sleek cards on the outer edges
    HUD.DrawPortrait(p1Name, p1X - 58, barY - 3, 46, 38, 1)
    HUD.DrawPortrait(p2Name, p2X + barW + 12, barY - 3, 46, 38, -1)
    
    -- ─── PLAYER 1 INTEGRATED CAPSULE ──────────────────────────────────────────
    -- White/gray outer capsule border
    gfx.draw_rounded_rect(p1X - 3, barY - 3, barW + 6, totalCapH + 6, 7, 1.0, 1.0, 1.0, 0.85)
    -- Dark background capsule
    gfx.draw_rounded_rect(p1X - 2, barY - 2, barW + 4, totalCapH + 4, 6, 0.05, 0.05, 0.08, 0.95)
    -- Empty backing for HP
    gfx.draw_rounded_rect(p1X, barY, barW, barH, 5, 0.18, 0.03, 0.03, 1.0)
    
    local fillCatchupP1 = (p1Catchup / p1Max) * barW
    local fillP1 = (p1Health / p1Max) * barW
    
    -- Damaged health section (Red)
    if fillCatchupP1 > fillP1 + 1.5 then
        draw_health_fill(p1X + fillP1, barY, fillCatchupP1 - fillP1, barH, 0.85, 0.1, 0.1, false, false)
    end
    
    -- Actual health (Green/Yellow/Red gradient)
    if fillP1 > 1.5 then
        local pct = p1Health / p1Max
        local r, g, b = 0.0, 0.82, 0.45
        if pct < 0.35 then
            r, g, b = 0.92, 0.12, 0.12
        elseif pct < 0.65 then
            r, g, b = 0.85, 0.8, 0.08
        end
        draw_health_fill(p1X, barY, fillP1, barH, r, g, b, true, false)
    end
    
    -- P1 EX Meter (Touch bottom of HP)
    local p1Super = fight.get_p1_super()
    local exY = barY + barH
    local segW = (barW - 4) / 3
    
    -- Empty backing for EX
    gfx.draw_rect(p1X, exY, barW, exH, 0.04, 0.04, 0.06, 1.0)
    
    for s = 1, 3 do
        local segX = p1X + (s - 1) * (segW + 2)
        -- Draw segment dark backing
        gfx.draw_rect(segX, exY, segW, exH, 0.12, 0.12, 0.15, 1.0)
        
        local fillVal = 0.0
        if p1Super >= s then
            fillVal = 1.0
        elseif p1Super > s - 1 then
            fillVal = p1Super - (s - 1)
        end
        if fillVal > 0 then
            local r, g, b = 0.0, 0.85, 1.0 -- neon cyan
            if p1Super >= 3.0 then
                local pulse = 0.75 + 0.25 * math.abs(math.sin(time.get_time() * 12))
                r, g, b = 1.0, 0.1 * pulse, 0.75 * pulse -- neon magenta at MAX
            end
            gfx.draw_rect(segX, exY, segW * fillVal, exH, r * 0.7, g * 0.7, b * 0.7, 1.0)
            gfx.draw_rect(segX, exY, segW * fillVal, exH * 0.4, 1.0, 1.0, 1.0, 0.35)
        end
    end
    
    -- ─── PLAYER 2 INTEGRATED CAPSULE ──────────────────────────────────────────
    -- White/gray outer capsule border
    gfx.draw_rounded_rect(p2X - 3, barY - 3, barW + 6, totalCapH + 6, 7, 1.0, 1.0, 1.0, 0.85)
    -- Dark background capsule
    gfx.draw_rounded_rect(p2X - 2, barY - 2, barW + 4, totalCapH + 4, 6, 0.05, 0.05, 0.08, 0.95)
    -- Empty backing for HP
    gfx.draw_rounded_rect(p2X, barY, barW, barH, 5, 0.18, 0.03, 0.03, 1.0)
    
    local fillCatchupP2 = (p2Catchup / p2Max) * barW
    local fillP2 = (p2Health / p2Max) * barW
    
    -- Damaged health section (Red)
    if fillCatchupP2 > fillP2 + 1.5 then
        draw_health_fill(p2X + (barW - fillCatchupP2), barY, fillCatchupP2 - fillP2, barH, 0.85, 0.1, 0.1, false, false)
    end
    
    -- Actual health
    if fillP2 > 1.5 then
        local pct = p2Health / p2Max
        local r, g, b = 0.0, 0.82, 0.45
        if pct < 0.35 then
            r, g, b = 0.92, 0.12, 0.12
        elseif pct < 0.65 then
            r, g, b = 0.85, 0.8, 0.08
        end
        draw_health_fill(p2X + (barW - fillP2), barY, fillP2, barH, r, g, b, false, true)
    end
    
    -- P2 EX Meter (Touch bottom of HP)
    local p2Super = fight.get_p2_super()
    
    -- Empty backing for EX
    gfx.draw_rect(p2X, exY, barW, exH, 0.04, 0.04, 0.06, 1.0)
    
    for s = 1, 3 do
        local segX = p2X + (3 - s) * (segW + 2)
        -- Draw segment dark backing
        gfx.draw_rect(segX, exY, segW, exH, 0.12, 0.12, 0.15, 1.0)
        
        local fillVal = 0.0
        if p2Super >= s then
            fillVal = 1.0
        elseif p2Super > s - 1 then
            fillVal = p2Super - (s - 1)
        end
        if fillVal > 0 then
            local r, g, b = 0.0, 0.85, 1.0
            if p2Super >= 3.0 then
                local pulse = 0.75 + 0.25 * math.abs(math.sin(time.get_time() * 12))
                r, g, b = 1.0, 0.1 * pulse, 0.75 * pulse
            end
            local fillX = segX + segW * (1.0 - fillVal)
            gfx.draw_rect(fillX, exY, segW * fillVal, exH, r * 0.7, g * 0.7, b * 0.7, 1.0)
            gfx.draw_rect(fillX, exY, segW * fillVal, exH * 0.4, 1.0, 1.0, 1.0, 0.35)
        end
    end
    
    -- ─── ROUND SCORE STARS ───────────────────────────────────────────────────
    local dotRadius = 5
    local dotY = exY + exH + 8
    
    -- P1 dots
    for i = 1, 2 do
        local color = {0.18, 0.18, 0.22}
        local alpha = 0.8
        if p1Score >= i then 
            color = {1.0, 0.8, 0.1}
            alpha = 1.0
        end
        gfx.draw_circle_filled(p1X + (i-1) * 20 + 8, dotY, dotRadius, color[1], color[2], color[3], alpha)
        gfx.draw_circle(p1X + (i-1) * 20 + 8, dotY, dotRadius + 1, 0.05, 0.05, 0.06, 0.75)
    end
    
    -- P2 dots
    for i = 1, 2 do
        local color = {0.18, 0.18, 0.22}
        local alpha = 0.8
        if p2Score >= i then 
            color = {1.0, 0.8, 0.1}
            alpha = 1.0
        end
        gfx.draw_circle_filled(p2X + barW - (i-1) * 20 - 8, dotY, dotRadius, color[1], color[2], color[3], alpha)
        gfx.draw_circle(p2X + barW - (i-1) * 20 - 8, dotY, dotRadius + 1, 0.05, 0.05, 0.06, 0.75)
    end
    
    -- ─── TIMER & ROUND (Layout matches Concept) ──────────────────────────────
    local timerCX = screenW / 2
    local timerVal = math.ceil(timer)
    if timerVal < 0 then timerVal = 0 end
    local timerText = string.format("%02d", timerVal)
    
    -- "ROUND X" text at top center
    local roundText = "ROUND " .. round
    if state == "intro" or state == "countdown" then roundText = "ROUND " .. round end
    gfx.draw_text(roundText, timerCX - (#roundText * 4), barY - 18, 0.85, 1.0, 1.0, 1.0, 0.9)
    
    -- Digital Timer with Outline Shadow
    gfx.draw_text(timerText, timerCX - 19, barY - 4, 2.3, 0.05, 0.05, 0.08, 0.95) -- Shadow
    gfx.draw_text(timerText, timerCX - 20, barY - 5, 2.3, 1.0, 1.0, 1.0, 1.0) -- Main digital white
    
    -- ─── COMBO COUNTERS (Neon Magenta Popups) ────────────────────────────────
    local dt = time.get_dt()
    
    -- Player 1 Combo UI (desenhado no lado do P2/à direita)
    if p1Combo > 1 then
        if p1Combo > lastComboP1 then
            comboScaleP1 = 2.0 -- Pulo inicial no tamanho
        end
        comboScaleP1 = comboScaleP1 + (1.0 - comboScaleP1) * 15 * dt -- Interpolação suave para 1.0
        
        comboPulseP1 = comboPulseP1 + dt * 10
        local pulse = comboScaleP1 * (1.0 + math.abs(math.sin(comboPulseP1)) * 0.05)
        local comboText = p1Combo .. " HITS"
        
        -- Dano acumulado do combo
        local p1ComboDmg = fight.get_p1_combo_damage and fight.get_p1_combo_damage() or 0
        local dmgText = "DAMAGE: " .. p1ComboDmg
        
        gfx.draw_text(comboText, p2X + 22, barY + barH + 34, pulse * 1.5, 0.0, 0.0, 0.0, 0.9)
        gfx.draw_text(comboText, p2X + 20, barY + barH + 32, pulse * 1.5, 1.0, 0.05, 0.75, 1.0) -- Hot Neon Magenta
        
        -- Dano total em Cyber Cyan
        gfx.draw_text(dmgText, p2X + 22, barY + barH + 62, 1.0, 0.0, 0.0, 0.0, 0.8)
        gfx.draw_text(dmgText, p2X + 20, barY + barH + 60, 1.0, 0.0, 0.9, 1.0, 1.0) -- Cyber Cyan
    else
        comboScaleP1 = 1.0
        comboPulseP1 = 0
    end
    lastComboP1 = p1Combo
    
    -- Player 2 Combo UI (desenhado no lado do P1/à esquerda)
    if p2Combo > 1 then
        if p2Combo > lastComboP2 then
            comboScaleP2 = 2.0 -- Pulo inicial no tamanho
        end
        comboScaleP2 = comboScaleP2 + (1.0 - comboScaleP2) * 15 * dt -- Interpolação suave para 1.0
        
        comboPulseP2 = comboPulseP2 + dt * 10
        local pulse = comboScaleP2 * (1.0 + math.abs(math.sin(comboPulseP2)) * 0.05)
        local comboText = p2Combo .. " HITS"
        
        -- Dano acumulado do combo
        local p2ComboDmg = fight.get_p2_combo_damage and fight.get_p2_combo_damage() or 0
        local dmgText = "DAMAGE: " .. p2ComboDmg
        
        gfx.draw_text(comboText, p1X + barW - 138, barY + barH + 34, pulse * 1.5, 0.0, 0.0, 0.0, 0.9)
        gfx.draw_text(comboText, p1X + barW - 140, barY + barH + 32, pulse * 1.5, 1.0, 0.05, 0.75, 1.0) -- Hot Neon Magenta
        
        -- Dano total em Cyber Cyan
        gfx.draw_text(dmgText, p1X + barW - 138, barY + barH + 62, 1.0, 0.0, 0.0, 0.0, 0.8)
        gfx.draw_text(dmgText, p1X + barW - 140, barY + barH + 60, 1.0, 0.0, 0.9, 1.0, 1.0) -- Cyber Cyan
    else
        comboScaleP2 = 1.0
        comboPulseP2 = 0
    end
    lastComboP2 = p2Combo
    
    -- ─── MATCH PHASE INTRO TEXT ──────────────────────────────────────────────
    if state == "intro" then
        local txt = "ROUND " .. round
        gfx.draw_rect(0, screenH/2 - 60, screenW, 120, 0, 0, 0, 0.6)
        gfx.draw_text(txt, screenW/2 - 90, screenH/2 - 20, 3.0, 1.0, 0.9, 0.1, 1.0)
    elseif state == "countdown" then
        gfx.draw_rect(0, screenH/2 - 60, screenW, 120, 0, 0, 0, 0.6)
        gfx.draw_text("FIGHT!", screenW/2 - 72, screenH/2 - 20, 3.0, 1.0, 0.1, 0.1, 1.0)
    elseif state == "ko" then
        gfx.draw_rect(0, screenH/2 - 60, screenW, 120, 0, 0, 0, 0.6)
        gfx.draw_text("K. O.", screenW/2 - 60, screenH/2 - 20, 3.0, 0.9, 0.1, 0.1, 1.0)
    end
    
    -- ─── INPUT DISPLAY ───────────────────────────────────────────────────────
    HUD.DrawInputs(50, screenH - 260, fight.get_p1_inputs())
    HUD.DrawInputs(screenW - 160, screenH - 260, fight.get_p2_inputs())
end

local dirStrings = {
    [8] = "UP",
    [2] = "DOWN",
    [4] = "LEFT",
    [6] = "RIGHT",
    [7] = "U-LEFT",
    [9] = "U-RIGHT",
    [1] = "D-LEFT",
    [3] = "D-RIGHT",
    [5] = "•"
}

function HUD.DrawInputs(startX, startY, list)
    local numToDraw = 8
    local lineH = 22
    
    gfx.draw_rect(startX - 10, startY - 10, 120, numToDraw * lineH + 20, 0.05, 0.05, 0.05, 0.3)
    
    for i = 1, math.min(#list, numToDraw) do
        local frame = list[i]
        local dir = frame.dir
        local btns = frame.buttons
        
        local text = dirStrings[dir] or "•"
        if text == "•" then text = "  •" end
        
        local btnStr = ""
        if (btns & 1) ~= 0 then btnStr = btnStr .. " LP" end
        if (btns & 2) ~= 0 then btnStr = btnStr .. " MP" end
        if (btns & 4) ~= 0 then btnStr = btnStr .. " HP" end
        if (btns & 8) ~= 0 then btnStr = btnStr .. " LK" end
        if (btns & 16) ~= 0 then btnStr = btnStr .. " MK" end
        if (btns & 32) ~= 0 then btnStr = btnStr .. " HK" end
        
        local r, g, b = 0.5, 0.5, 0.5
        if i == 1 then r, g, b = 1.0, 1.0, 1.0 end
        
        gfx.draw_text(text .. btnStr, startX, startY + (i-1) * lineH, 1.0, r, g, b, 0.8 - (i-1)*0.08)
    end
end

function HUD.DrawPortrait(name, x, y, w, h, facing)
    -- Glass box background
    gfx.draw_rounded_rect(x, y, w, h, 6, 0.05, 0.07, 0.14, 0.9)
    -- Golden frame border
    gfx.draw_rounded_rect(x - 1, y - 1, w + 2, h + 2, 7, 0.85, 0.68, 0.1, 0.75)
    
    local cx = x + w * 0.5
    local cy = y + h * 0.5
    
    local texID = nil
    if FighterRenderer and FighterRenderer.Textures then
        if name == "KAITO" then texID = FighterRenderer.Textures.kaito
        elseif name == "RIN" then texID = FighterRenderer.Textures.rin
        elseif name == "GOROU" then texID = FighterRenderer.Textures.gorou
        end
    end

    if texID then
        -- Draw pixel art face from spritesheet (128x128 head box at col 0, row 0, offset x=64, y=20)
        local srcX = 64
        local srcY = 20
        local srcW = 128
        local srcH = 128
        
        local drawW = w - 4
        local drawH = h - 4
        local finalW = drawW * facing
        local finalX = (facing == 1) and (cx - drawW * 0.5) or (cx + drawW * 0.5)
        local finalY = cy - drawH * 0.5
        
        -- Clip slightly inside the border
        gfx.set_clip_rect(x + 1, y + 1, w - 2, h - 2)
        gfx.draw_sprite_region(finalX, finalY, finalW, drawH, texID, srcX, srcY, srcW, srcH, 1024, 1024, 1.0, 1.0, 1.0, 1.0)
        gfx.clear_clip_rect()
    else
        if name == "KAITO" then
            local skinR, skinG, skinB = 1.0, 0.8, 0.65
            gfx.draw_circle_filled(cx, cy + 2, 13, skinR, skinG, skinB, 1.0)
            -- Red headband
            gfx.draw_rect(cx - 12, cy - 4, 24, 4, 0.9, 0.1, 0.1, 1.0)
            -- Hair bangs
            gfx.draw_triangle(cx - 11, cy - 4, cx + 9, cy - 4, cx - 3 * facing, cy - 14, 0.08, 0.08, 0.08, 1.0)
            gfx.draw_triangle(cx - 6, cy - 4, cx + 11, cy - 4, cx + 5 * facing, cy - 12, 0.08, 0.08, 0.08, 1.0)
            -- White collar
            gfx.draw_triangle(cx - 9, cy + 11, cx + 9, cy + 11, cx, cy + 19, 0.9, 0.9, 0.9, 1.0)
            -- Eye
            gfx.draw_rect(cx + 4 * facing - 1.5, cy, 3, 2, 0.1, 0.1, 0.1, 1.0)
        elseif name == "GOROU" then
            local skinR, skinG, skinB = 0.85, 0.60, 0.45
            gfx.draw_circle_filled(cx, cy + 1, 14, skinR, skinG, skinB, 1.0)
            -- Beard
            gfx.draw_triangle(cx - 11 * facing, cy + 2, cx + 9 * facing, cy + 5, cx, cy + 15, 0.12, 0.12, 0.12, 1.0)
            -- Hair
            gfx.draw_triangle(cx - 7, cy - 9, cx + 7, cy - 9, cx + 1 * facing, cy - 17, 0.12, 0.12, 0.12, 1.0)
            -- Suspenders
            gfx.draw_rect(cx - 8, cy + 11, 3, 8, 0.12, 0.12, 0.12, 1.0)
            gfx.draw_rect(cx + 5, cy + 11, 3, 8, 0.12, 0.12, 0.12, 1.0)
            -- Eye
            gfx.draw_rect(cx + 4 * facing - 1.5, cy, 3, 2, 0.1, 0.1, 0.1, 1.0)
        elseif name == "RIN" then
            local skinR, skinG, skinB = 0.98, 0.85, 0.78
            local hairR, hairG, hairB = 0.45, 0.15, 0.65
            gfx.draw_circle_filled(cx, cy + 1, 13, skinR, skinG, skinB, 1.0)
            -- Mask
            gfx.draw_rect(cx - 12, cy + 1, 24, 12, 0.1, 0.9, 0.9, 1.0)
            -- Hair bangs
            gfx.draw_triangle(cx - 11, cy - 4, cx + 11, cy - 4, cx, cy - 14, hairR, hairG, hairB, 1.0)
            gfx.draw_triangle(cx - 12 * facing, cy - 1, cx - 7 * facing, cy + 9, cx - 13 * facing, cy + 7, hairR, hairG, hairB, 1.0)
            -- Eye
            gfx.draw_rect(cx + 4 * facing - 1.5, cy - 1, 3, 1.5, 0.1, 0.9, 0.9, 1.0)
        end
    end
end

return HUD
