local FighterRenderer = {}

-- Helper functions for drawing with reflection and outline support
local function draw_rect_t(rx, ry, rw, rh, r, g, b, a, isReflecting, groundScreenY, alphaScale)
    local finalAlpha = a * (alphaScale or 1.0)
    if isReflecting then
        local ry_ref = 2 * groundScreenY - (ry + rh)
        local rr = r * 0.65 + 0.12
        local rg = g * 0.35
        local rb = b * 0.75 + 0.16
        local t_val = time.get_time()
        local step = 4
        for sy = 0, rh - 1, step do
            local cur_h = math.min(step, rh - sy)
            local cur_y = ry_ref + sy
            local shift = math.sin(t_val * 7.5 + cur_y * 0.08) * 6.2
            local fade = math.max(0.0, 1.0 - (cur_y - groundScreenY) / 200.0)
            gfx.draw_rect(rx + shift, cur_y, rw, cur_h, rr, rg, rb, finalAlpha * 0.16 * fade)
        end
    else
        gfx.draw_rect(rx - 1, ry - 1, rw + 2, rh + 2, 0.05, 0.05, 0.06, finalAlpha * 0.95)
        gfx.draw_rect(rx, ry, rw, rh, r, g, b, finalAlpha)
    end
end

local function draw_circle_filled_t(cx, cy, radius, r, g, b, a, isReflecting, groundScreenY, alphaScale)
    local finalAlpha = a * (alphaScale or 1.0)
    if isReflecting then
        local cy_ref = 2 * groundScreenY - cy
        local rr = r * 0.65 + 0.12
        local rg = g * 0.35
        local rb = b * 0.75 + 0.16
        local t_val = time.get_time()
        local step = 4
        for sy = -radius, radius - 1, step do
            local cur_y = cy_ref + sy
            local half_w = math.sqrt(math.max(0, radius * radius - sy * sy))
            local shift = math.sin(t_val * 7.5 + cur_y * 0.08) * 6.2
            local fade = math.max(0.0, 1.0 - (cur_y - groundScreenY) / 200.0)
            gfx.draw_rect(cx - half_w + shift, cur_y, half_w * 2, math.min(step, radius - sy), rr, rg, rb, finalAlpha * 0.16 * fade)
        end
    else
        gfx.draw_circle_filled(cx, cy, radius + 1.2, 0.05, 0.05, 0.06, finalAlpha * 0.95)
        gfx.draw_circle_filled(cx, cy, radius, r, g, b, finalAlpha)
    end
end

local function draw_circle_t(cx, cy, radius, r, g, b, a, isReflecting, groundScreenY, alphaScale)
    local finalAlpha = a * (alphaScale or 1.0)
    if isReflecting then
        local cy_ref = 2 * groundScreenY - cy
        local rr = r * 0.65 + 0.12
        local rg = g * 0.35
        local rb = b * 0.75 + 0.16
        local t_val = time.get_time()
        local step = 4
        for sy = -radius, radius - 1, step do
            local cur_y = cy_ref + sy
            local half_w = math.sqrt(math.max(0, radius * radius - sy * sy))
            local shift = math.sin(t_val * 7.5 + cur_y * 0.08) * 6.2
            local fade = math.max(0.0, 1.0 - (cur_y - groundScreenY) / 200.0)
            gfx.draw_rect(cx - half_w + shift, cur_y, 2, math.min(step, radius - sy), rr, rg, rb, finalAlpha * 0.16 * fade)
            gfx.draw_rect(cx + half_w - 2 + shift, cur_y, 2, math.min(step, radius - sy), rr, rg, rb, finalAlpha * 0.16 * fade)
        end
    else
        gfx.draw_circle(cx, cy, radius, r, g, b, finalAlpha)
    end
end

local function draw_line_t(x1, y1, x2, y2, thickness, r, g, b, a, isReflecting, groundScreenY, alphaScale)
    local finalAlpha = a * (alphaScale or 1.0)
    if isReflecting then
        local y1_ref = 2 * groundScreenY - y1
        local y2_ref = 2 * groundScreenY - y2
        local rr = r * 0.65 + 0.12
        local rg = g * 0.35
        local rb = b * 0.75 + 0.16
        local t_val = time.get_time()
        
        local min_y = math.min(y1_ref, y2_ref)
        local max_y = math.max(y1_ref, y2_ref)
        local dy = max_y - min_y
        local step = 4
        if dy < 1 then
            local shift = math.sin(t_val * 7.5 + min_y * 0.08) * 6.2
            local min_x = math.min(x1, x2)
            local max_x = math.max(x1, x2)
            local fade = math.max(0.0, 1.0 - (min_y - groundScreenY) / 200.0)
            gfx.draw_rect(min_x + shift, min_y - thickness * 0.5, max_x - min_x, thickness, rr, rg, rb, finalAlpha * 0.16 * fade)
        else
            for cur_y = min_y, max_y - 1, step do
                local pct = (cur_y - min_y) / dy
                local cur_x = x1 + (x2 - x1) * (y1_ref < y2_ref and pct or (1 - pct))
                local shift = math.sin(t_val * 7.5 + cur_y * 0.08) * 6.2
                local fade = math.max(0.0, 1.0 - (cur_y - groundScreenY) / 200.0)
                gfx.draw_rect(cur_x - thickness * 0.5 + shift, cur_y, thickness, math.min(step, max_y - cur_y), rr, rg, rb, finalAlpha * 0.16 * fade)
            end
        end
    else
        gfx.draw_line(x1, y1, x2, y2, thickness + 1.8, 0.05, 0.05, 0.06, finalAlpha * 0.95)
        gfx.draw_line(x1, y1, x2, y2, thickness, r, g, b, finalAlpha)
    end
end

local function draw_triangle_t(x1, y1, x2, y2, x3, y3, r, g, b, a, isReflecting, groundScreenY, alphaScale)
    local finalAlpha = a * (alphaScale or 1.0)
    if isReflecting then
        -- Sort vertices by y coordinate so y1 <= y2 <= y3
        if y1 > y2 then x1, y1, x2, y2 = x2, y2, x1, y1 end
        if y1 > y3 then x1, y1, x3, y3 = x3, y3, x1, y1 end
        if y2 > y3 then x2, y2, x3, y3 = x3, y3, x2, y2 end
        
        local rr = r * 0.65 + 0.12
        local rg = g * 0.35
        local rb = b * 0.75 + 0.16
        local t_val = time.get_time()
        
        local step = 4
        local function get_x(y, xa, ya, xb, yb)
            if math.abs(ya - yb) < 0.0001 then return xa end
            return xa + (xb - xa) * (y - ya) / (yb - ya)
        end
        
        for y = math.floor(y1), math.ceil(y3), step do
            local xl, xr
            if y < y2 then
                xl = get_x(y, x1, y1, x3, y3)
                xr = get_x(y, x1, y1, x2, y2)
            else
                xl = get_x(y, x1, y1, x3, y3)
                xr = get_x(y, x2, y2, x3, y3)
            end
            if xl > xr then xl, xr = xr, xl end
            
            local cur_y = 2 * groundScreenY - y
            local shift = math.sin(t_val * 7.5 + cur_y * 0.08) * 6.2
            local fade = math.max(0.0, 1.0 - (cur_y - groundScreenY) / 200.0)
            gfx.draw_rect(xl + shift, cur_y - step, xr - xl, step, rr, rg, rb, finalAlpha * 0.16 * fade)
        end
    else
        gfx.draw_line(x1, y1, x2, y2, 2.0, 0.05, 0.05, 0.06, finalAlpha * 0.95)
        gfx.draw_line(x2, y2, x3, y3, 2.0, 0.05, 0.05, 0.06, finalAlpha * 0.95)
        gfx.draw_line(x3, y3, x1, y1, 2.0, 0.05, 0.05, 0.06, finalAlpha * 0.95)
        gfx.draw_triangle(x1, y1, x2, y2, x3, y3, r, g, b, finalAlpha)
    end
end

local function draw_rounded_rect_t(rx, ry, rw, rh, radius, r, g, b, a, isReflecting, groundScreenY, alphaScale)
    local finalAlpha = a * (alphaScale or 1.0)
    if isReflecting then
        local ry_ref = 2 * groundScreenY - (ry + rh)
        local rr = r * 0.65 + 0.12
        local rg = g * 0.35
        local rb = b * 0.75 + 0.16
        local t_val = time.get_time()
        local step = 4
        for sy = 0, rh - 1, step do
            local cur_h = math.min(step, rh - sy)
            local cur_y = ry_ref + sy
            local shift = math.sin(t_val * 7.5 + cur_y * 0.08) * 6.2
            local fade = math.max(0.0, 1.0 - (cur_y - groundScreenY) / 200.0)
            local offset = 0
            if sy < radius then
                offset = radius - math.sqrt(radius * radius - (radius - sy) * (radius - sy))
            elseif sy > rh - radius then
                offset = radius - math.sqrt(radius * radius - (sy - (rh - radius)) * (sy - (rh - radius)))
            end
            gfx.draw_rect(rx + offset + shift, cur_y, rw - 2 * offset, cur_h, rr, rg, rb, finalAlpha * 0.16 * fade)
        end
    else
        gfx.draw_rounded_rect(rx - 1, ry - 1, rw + 2, rh + 2, radius + 1, 0.05, 0.05, 0.06, finalAlpha * 0.95)
        gfx.draw_rounded_rect(rx, ry, rw, rh, radius, r, g, b, finalAlpha)
    end
end

-- Unified drawing function for Kaito, Gorou, and Rin
function FighterRenderer.DrawFighterBody(name, cx, cy, scale, facing, state, br, bg, bb, ar, ag, ab, t, isHit, isReflecting, groundScreenY, alphaScale, stateFrameIn)
    local stateFrame = stateFrameIn or math.floor(t * 60)
    local w, h = 50, 110
    if name == "GOROU" then
        w, h = 60, 125
    elseif name == "RIN" then
        w, h = 45, 115
    end
    
    w = w * scale
    h = h * scale
    
    local bodyW = w
    local bodyH = h * 0.6
    local headR = w * 0.35
    
    local headX = cx
    local headY = cy - h + headR
    
    local torsoX = cx - bodyW * 0.5
    local torsoY = cy - bodyH
    
    -- Local helper for feet wedge flares (sharp neon flares)
    local function draw_feet_flares(fx, fy, isRightFoot)
        if isReflecting or (alphaScale or 1.0) <= 0.5 then return end
        math.randomseed(math.floor(time.get_time() * 12) + (isRightFoot and 1 or 0))
        local dir = facing
        for wl = 1, 3 do
            local len = math.random(10, 20) * scale
            local angle = (math.random() * 0.45 + 0.1) * dir
            local ex = fx + math.sin(angle) * len
            local ey = fy - math.cos(angle) * len
            local baseW = math.random(4, 6) * scale
            
            -- Draw neon wedge glow
            local gr, gg, gb = ar, ag, ab
            if name == "KAITO" then gr, gg, gb = 1.0, 0.2, 0.95 -- Pink wedge
            elseif name == "RIN" then gr, gg, gb = 0.0, 0.85, 1.0 -- Cyan wedge
            end
            
            gfx.draw_triangle(fx, fy, fx + baseW * dir, fy, ex, ey, gr, gg, gb, 0.8)
            gfx.draw_triangle(fx, fy, fx + (baseW * 0.5) * dir, fy, ex, ey, 1.0, 1.0, 1.0, 0.95) -- White core
        end
    end
    
    -- Textures table
    if not FighterRenderer.Textures then
        FighterRenderer.Textures = {
            kaito = nil,
            rin = nil,
            gorou = nil
        }
    end

    local function InitTextures()
        if not FighterRenderer.Textures.kaito then
            FighterRenderer.Textures.kaito = assets.load_texture("assets/textures/kaito_spritesheet.png", true)
        end
        if not FighterRenderer.Textures.rin then
            FighterRenderer.Textures.rin = assets.load_texture("assets/textures/rin_spritesheet.png", true)
        end
        if not FighterRenderer.Textures.gorou then
            FighterRenderer.Textures.gorou = assets.load_texture("assets/textures/gorou_spritesheet.png", true)
        end
    end

    local function GetStateCol(st)
        if st == "idle" or st == "win" or st == "intro" then
            return 0
        elseif st == "walk_fwd" or st == "walk_back" or st == "jump" or st == "jump_startup" or st == "jump_land" then
            return 1
        elseif st == "attack" or st == "special" or st == "throw_startup" or st == "throw_active" then
            return 2
        elseif st == "crouch" then
            return 0
        else -- hitstun, knockdown, ko, blockstun, getup, thrown
            return 3
        end
    end

    InitTextures()

    local texID = nil
    if name == "KAITO" then
        texID = FighterRenderer.Textures.kaito
    elseif name == "RIN" then
        texID = FighterRenderer.Textures.rin
    elseif name == "GOROU" then
        texID = FighterRenderer.Textures.gorou
    end

    if texID then
        local col = GetStateCol(state)
        local frame = 0
        
        -- Animation Logic based on Column and State (4x2 grid, 256x512 frames)
        if col == 0 then
            -- Idle/Crouch/Win
            if state == "crouch" or state == "hitstun_crouch" or state == "blockstun_crouch" then
                frame = 0 -- Crouch posture
            elseif state == "win" then
                frame = 1 -- Win posture
            else
                -- Idle / Intro (2-frame cycle)
                frame = math.floor(stateFrame / 15) % 2
            end
        elseif col == 1 then
            -- Walk/Jump
            if state == "jump" or state == "jump_startup" or state == "jump_land" then
                frame = 1 -- Airborne posture
            else
                -- Walk Forward / Backward (2-frame cycle)
                frame = math.floor(stateFrame / 8) % 2
            end
        elseif col == 2 then
            -- Attack / Special (2-frame sequence)
            if stateFrame < 8 then
                frame = 0 -- Attack wind-up / startup
            else
                frame = 1 -- Active attack strike / recovery
            end
        elseif col == 3 then
            -- Hitstun/KO
            if state == "knockdown" or state == "ko" then
                if stateFrame < 12 then
                    frame = 0 -- Falling down
                else
                    frame = 1 -- Lying on the ground
                end
            else
                -- Hitstun / Blockstun shake (fast 2-frame vibration cycle)
                frame = math.floor(stateFrame / 4) % 2
            end
        end
        
        local srcX = col * 256
        local srcY = frame * 512
        local srcW = 256
        local srcH = 512
        
        local drawW = h * 0.5
        local drawH = h
        
        -- Compress height slightly for crouch state
        if state == "crouch" or state == "hitstun_crouch" or state == "blockstun_crouch" then
            drawH = h * 0.75
        end
        
        local finalW = drawW * facing
        local finalX = (facing == 1) and (cx - drawW * 0.5) or (cx + drawW * 0.5)
        
        local finalH = drawH
        local finalY = cy - drawH
        
        local alpha = alphaScale or 1.0
        
        if isReflecting then
            local ry_ref = 2 * groundScreenY - cy
            local step = 4
            for sy = 0, drawH - 1, step do
                local cur_h = math.min(step, drawH - sy)
                local cur_y = ry_ref + sy
                local shift = math.sin(t * 7.5 + cur_y * 0.08) * 6.2
                local fade = math.max(0.0, 1.0 - (cur_y - groundScreenY) / 200.0)
                
                -- Slice texture vertically (flipped)
                local ratio_start = (sy / drawH)
                local ratio_end = ((sy + cur_h) / drawH)
                local cur_srcY = srcY + srcH - ratio_end * srcH
                local cur_srcH = (ratio_end - ratio_start) * srcH
                
                gfx.draw_sprite_region(finalX + shift, cur_y, finalW, cur_h, texID, srcX, cur_srcY, srcW, cur_srcH, 1024, 1024, 1.0, 1.0, 1.0, alpha * 0.25 * fade)
            end
        else
            -- Draw normal single quad
            gfx.draw_sprite_region(finalX, finalY, finalW, finalH, texID, srcX, srcY, srcW, srcH, 1024, 1024, 1.0, 1.0, 1.0, alpha)
        end
        
        -- Draw ground sparks if active and not reflecting
        if not isReflecting and (state == "idle" or state == "walk_fwd" or state == "walk_back") then
            local lFootX = cx - w * 0.22 * facing
            local rFootX = cx + w * 0.22 * facing
            local lFootY = cy
            local rFootY = cy
            draw_feet_flares(lFootX, lFootY, false)
            draw_feet_flares(rFootX, rFootY, true)
        end
        
        return
    end
    
    -- ─── SKELETON JOINTS DEFINITION ──────────────────────────────────────────
    local lHipX = cx - bodyW * 0.22 * facing
    local rHipX = cx + bodyW * 0.22 * facing
    local hipY = cy - bodyH * 0.15
    
    local lShoulderX = cx - bodyW * 0.4 * facing
    local rShoulderX = cx + bodyW * 0.4 * facing
    local shoulderY = cy - bodyH * 0.85
    
    -- Standing defaults
    local lKneeX, lKneeY = lHipX - 5 * scale * facing, cy - bodyH * 0.5
    local rKneeX, rKneeY = rHipX + 5 * scale * facing, cy - bodyH * 0.5
    local lFootX, lFootY = lHipX - 10 * scale * facing, cy
    local rFootX, rFootY = rHipX + 10 * scale * facing, cy
    
    local lElbowX, lElbowY = lShoulderX - 10 * scale * facing, cy - bodyH * 0.6
    local rElbowX, rElbowY = rShoulderX + 10 * scale * facing, cy - bodyH * 0.6
    local lHandX, lHandY = lElbowX - 6 * scale * facing, cy - bodyH * 0.5
    local rHandX, rHandY = rElbowX + 6 * scale * facing, cy - bodyH * 0.5
    
    local pulse = math.sin(t * 10) * 3 * scale
    
    -- ─── STATE POSES OVERRIDES ────────────────────────────────────────────────
    if state == "idle" then
        headY = headY + pulse * 0.5
        torsoY = torsoY + pulse * 0.3
        hipY = hipY + pulse * 0.3
        shoulderY = shoulderY + pulse * 0.3
        
        if name == "KAITO" then
            -- Karate stance (Zenkutsu-Dachi): low center, bent front leg, extended front fist, chambered back fist.
            rFootX = cx + 25 * facing * scale
            rFootY = cy
            rKneeX = cx + 20 * facing * scale
            rKneeY = cy - 18 * scale
            
            lFootX = cx - 22 * facing * scale
            lFootY = cy
            lKneeX = cx - 18 * facing * scale
            lKneeY = cy - 24 * scale
            
            -- Front arm: extended slightly forward guard
            rHandX = cx + 24 * facing * scale
            rHandY = cy - bodyH * 0.72 + pulse
            rElbowX = cx + 13 * facing * scale
            rElbowY = cy - bodyH * 0.64
            
            -- Back arm: chambered tight near waist
            lHandX = cx - 2 * facing * scale
            lHandY = cy - bodyH * 0.58 + pulse
            lElbowX = cx - 11 * facing * scale
            lElbowY = cy - bodyH * 0.48
        elseif name == "RIN" then
            -- Ninja stance: very low, athletic, knees bent inwards, arms ready.
            torsoY = torsoY + 8 * scale + pulse * 0.3
            hipY = hipY + 8 * scale + pulse * 0.3
            shoulderY = shoulderY + 8 * scale + pulse * 0.3
            headY = headY + 8 * scale + pulse * 0.5
            
            lFootX = cx - 20 * facing * scale
            rFootX = cx + 20 * facing * scale
            lKneeX = cx - 12 * facing * scale
            lKneeY = cy - 14 * scale
            rKneeX = cx + 12 * facing * scale
            rKneeY = cy - 14 * scale
            
            -- Front arm: high guard
            rHandX = cx + 18 * facing * scale
            rHandY = cy - bodyH * 0.85
            rElbowX = cx + 12 * facing * scale
            rElbowY = cy - bodyH * 0.7
            
            -- Back arm: low guard
            lHandX = cx - 16 * facing * scale
            lHandY = cy - bodyH * 0.45
            lElbowX = cx - 10 * facing * scale
            lElbowY = cy - bodyH * 0.55
        else -- GOROU
            -- Grappler: broad chest, feet wide apart, knees flared out, arms ready to grab
            lFootX = cx - 32 * scale
            rFootX = cx + 32 * scale
            lKneeX = cx - 26 * scale
            lKneeY = cy - 20 * scale
            rKneeX = cx + 26 * scale
            rKneeY = cy - 20 * scale
            
            lHandX = cx - 28 * scale
            lHandY = cy - bodyH * 0.65 + pulse
            lElbowX = cx - 22 * scale
            lElbowY = cy - bodyH * 0.5
            
            rHandX = cx + 28 * scale
            rHandY = cy - bodyH * 0.65 + pulse
            rElbowX = cx + 22 * scale
            rElbowY = cy - bodyH * 0.5
        end
        
    elseif state == "walk_fwd" then
        local swing = math.sin(t * 15) * 12 * scale
        torsoX = torsoX + 4 * facing * scale
        
        -- Walk limbs swing
        lFootX = lHipX - 16 * facing * scale + swing
        rFootX = rHipX + 16 * facing * scale - swing
        
        lKneeX = (lHipX + lFootX) * 0.5 - 4 * facing * scale
        rKneeX = (rHipX + rFootX) * 0.5 + 4 * facing * scale
        
        lHandX = lShoulderX - 12 * facing * scale - swing * 0.8
        rHandX = rShoulderX + 14 * facing * scale + swing * 0.8
        lElbowX = (lShoulderX + lHandX) * 0.5
        rElbowX = (rShoulderX + rHandX) * 0.5
        
    elseif state == "walk_back" then
        local swing = math.sin(t * 12) * 8 * scale
        torsoX = torsoX - 3 * facing * scale
        
        lFootX = lHipX - 12 * facing * scale - swing
        rFootX = rHipX + 12 * facing * scale + swing
        
        lKneeX = (lHipX + lFootX) * 0.5 - 2 * facing * scale
        rKneeX = (rHipX + rFootX) * 0.5 + 2 * facing * scale
        
        lHandX = lShoulderX - 8 * facing * scale + swing * 0.5
        rHandX = rShoulderX + 8 * facing * scale - swing * 0.5
        lElbowX = (lShoulderX + lHandX) * 0.5
        rElbowX = (rShoulderX + rHandX) * 0.5
        
    elseif state == "crouch" then
        torsoY = torsoY + 22 * scale
        hipY = hipY + 22 * scale
        shoulderY = shoulderY + 22 * scale
        headY = headY + 22 * scale
        
        lFootX = cx - 25 * facing * scale
        rFootX = cx + 22 * facing * scale
        
        lKneeX = cx - 18 * facing * scale
        lKneeY = cy - 10 * scale
        rKneeX = cx + 18 * facing * scale
        rKneeY = cy - 10 * scale
        
        -- Hands guarding crouched
        rHandX = cx + 16 * facing * scale
        rHandY = cy - bodyH * 0.5
        rElbowX = cx + 10 * facing * scale
        rElbowY = cy - bodyH * 0.4
        
        lHandX = cx - 4 * facing * scale
        lHandY = cy - bodyH * 0.4
        lElbowX = cx - 8 * facing * scale
        lElbowY = cy - bodyH * 0.3
        
    elseif state == "jump" then
        lFootY = cy - 14 * scale
        rFootY = cy - 14 * scale
        lKneeY = cy - 25 * scale
        rKneeY = cy - 25 * scale
        
        -- Hands up
        lHandY = cy - bodyH * 1.1
        rHandY = cy - bodyH * 1.1
        
    elseif state == "attack" or state == "special" then
        -- Extends front arm fully for strike!
        rHandX = cx + 46 * facing * scale
        rHandY = cy - bodyH * 0.8
        rElbowX = cx + 22 * facing * scale
        rElbowY = cy - bodyH * 0.8
        
        -- Back leg straight, front leg bent
        rFootX = cx + 28 * facing * scale
        rKneeX = cx + 22 * facing * scale
        rKneeY = cy - 12 * scale
        
        lFootX = cx - 26 * facing * scale
        lKneeX = cx - 20 * facing * scale
        lKneeY = cy - 25 * scale
        
    elseif state == "blockstun_stand" or state == "blockstun_crouch" then
        -- Both hands guarding face/chest
        rHandX = cx + 12 * facing * scale
        rHandY = cy - bodyH * 0.85
        rElbowX = cx + 6 * facing * scale
        rElbowY = cy - bodyH * 0.7
        
        lHandX = cx + 8 * facing * scale
        lHandY = cy - bodyH * 0.75
        lElbowX = cx + 2 * facing * scale
        lElbowY = cy - bodyH * 0.6
        
    elseif state == "hitstun_stand" or state == "hitstun_crouch" then
        headX = headX - 10 * facing * scale
        torsoX = torsoX - 5 * facing * scale
        lShoulderX = lShoulderX - 5 * facing * scale
        rShoulderX = rShoulderX - 5 * facing * scale
        
        lHandX = cx - 18 * facing * scale
        lHandY = cy - bodyH * 0.95
        rHandX = cx - 8 * facing * scale
        rHandY = cy - bodyH * 1.0
        
    elseif state == "knockdown" or state == "ko" then
        headX = cx + 45 * facing * scale
        headY = cy - 10 * scale
        
        lHipX, rHipX = cx - 15 * scale, cx - 10 * scale
        hipY = cy - 8 * scale
        
        lShoulderX, rShoulderX = cx + 15 * scale, cx + 18 * scale
        shoulderY = cy - 10 * scale
        
        lKneeX, lKneeY = cx - 22 * scale, cy - 8 * scale
        rKneeX, rKneeY = cx - 18 * scale, cy - 8 * scale
        
        lFootX, lFootY = cx - 35 * scale, cy - 6 * scale
        rFootX, rFootY = cx - 30 * scale, cy - 6 * scale
        
        lElbowX, lElbowY = cx + 22 * scale, cy - 10 * scale
        rElbowX, rElbowY = cx + 24 * scale, cy - 10 * scale
        
        lHandX, lHandY = cx + 30 * scale, cy - 8 * scale
        rHandX, rHandY = cx + 32 * scale, cy - 8 * scale
        
    elseif state == "win" then
        rHandX = rShoulderX + 2 * facing * scale
        rHandY = cy - bodyH * 1.55 + pulse
        rElbowX = rShoulderX + 2 * facing * scale
        rElbowY = cy - bodyH * 1.2
        
        lHandX = lShoulderX - 4 * facing * scale
        lHandY = cy - bodyH * 0.5
        lElbowX = lShoulderX - 4 * facing * scale
        lElbowY = cy - bodyH * 0.4
    end

    if name == "KAITO" then
        -- ----------------- KAITO (KARATE GI) -----------------
        local skinR, skinG, skinB = 1.0, 0.80, 0.65
        
        -- Ground Sparks under feet (Sharp Neon Flares)
        draw_feet_flares(lFootX, lFootY, false)
        draw_feet_flares(rFootX, rFootY, true)

        -- 1. Draw Legs (Gi pants)
        -- Left Leg
        draw_line_t(lHipX, hipY, lKneeX, lKneeY, 14 * scale, br * 0.9, bg * 0.9, bb * 0.9, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(lKneeX, lKneeY, lFootX, lFootY, 12 * scale, br * 0.9, bg * 0.9, bb * 0.9, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Double gi-shading folds (Left Leg)
        if not isReflecting then
            draw_line_t(lHipX + 2*scale, hipY + 1*scale, lKneeX + 2*scale, lKneeY, 2*scale, br*1.2, bg*1.2, bb*1.2, 0.8, false, groundScreenY, alphaScale)
            draw_line_t(lHipX - 2*scale, hipY + 1*scale, lKneeX - 2*scale, lKneeY, 2*scale, br*0.6, bg*0.6, bb*0.6, 0.8, false, groundScreenY, alphaScale)
            draw_line_t(lKneeX + 1.8*scale, lKneeY, lFootX + 1.8*scale, lFootY, 2*scale, br*1.2, bg*1.2, bb*1.2, 0.8, false, groundScreenY, alphaScale)
            draw_line_t(lKneeX - 1.8*scale, lKneeY, lFootX - 1.8*scale, lFootY, 2*scale, br*0.6, bg*0.6, bb*0.6, 0.8, false, groundScreenY, alphaScale)
        end
        
        -- Right Leg
        draw_line_t(rHipX, hipY, rKneeX, rKneeY, 14 * scale, br * 0.9, bg * 0.9, bb * 0.9, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(rKneeX, rKneeY, rFootX, rFootY, 12 * scale, br * 0.9, bg * 0.9, bb * 0.9, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Double gi-shading folds (Right Leg)
        if not isReflecting then
            draw_line_t(rHipX + 2*scale, hipY + 1*scale, rKneeX + 2*scale, rKneeY, 2*scale, br*1.2, bg*1.2, bb*1.2, 0.8, false, groundScreenY, alphaScale)
            draw_line_t(rHipX - 2*scale, hipY + 1*scale, rKneeX - 2*scale, rKneeY, 2*scale, br*0.6, bg*0.6, bb*0.6, 0.8, false, groundScreenY, alphaScale)
            draw_line_t(rKneeX + 1.8*scale, rKneeY, rFootX + 1.8*scale, rFootY, 2*scale, br*1.2, bg*1.2, bb*1.2, 0.8, false, groundScreenY, alphaScale)
            draw_line_t(rKneeX - 1.8*scale, rKneeY, rFootX - 1.8*scale, rFootY, 2*scale, br*0.6, bg*0.6, bb*0.6, 0.8, false, groundScreenY, alphaScale)
        end
        
        -- Feet (Bare)
        draw_rect_t(lFootX - 3 * scale, lFootY - 1 * scale, 15 * scale, 5 * scale, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_rect_t(rFootX - 3 * scale, rFootY - 1 * scale, 15 * scale, 5 * scale, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Toes outline
        if not isReflecting then
            for d = 0, 4 do
                local dx = (facing == 1) and (lFootX - 3*scale + d*2.8*scale) or (lFootX + 12*scale - d*2.8*scale)
                gfx.draw_rect(dx, lFootY + 1*scale, 2*scale, 2*scale, skinR*0.85, skinG*0.85, skinB*0.85, 1.0 * (alphaScale or 1.0))
                
                local dx2 = (facing == 1) and (rFootX - 3*scale + d*2.8*scale) or (rFootX + 12*scale - d*2.8*scale)
                gfx.draw_rect(dx2, rFootY + 1*scale, 2*scale, 2*scale, skinR*0.85, skinG*0.85, skinB*0.85, 1.0 * (alphaScale or 1.0))
            end
        end
        
        -- 2. Draw Torso (Gi)
        draw_rect_t(torsoX, torsoY, bodyW, bodyH, br, bg, bb, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- V-Neck chest gap
        draw_triangle_t(torsoX + bodyW * 0.32, torsoY, torsoX + bodyW * 0.68, torsoY, torsoX + bodyW * 0.5, torsoY + bodyH * 0.42, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        if not isReflecting then
            -- Muscle definition inside V-Neck
            local mR, mG, mB = skinR * 0.76, skinG * 0.76, skinB * 0.76
            gfx.draw_line(torsoX + bodyW*0.36, torsoY + bodyH*0.25, torsoX + bodyW*0.48, torsoY + bodyH*0.25, 1.8*scale, mR, mG, mB, 1.0 * (alphaScale or 1.0))
            gfx.draw_line(torsoX + bodyW*0.64, torsoY + bodyH*0.25, torsoX + bodyW*0.52, torsoY + bodyH*0.25, 1.8*scale, mR, mG, mB, 1.0 * (alphaScale or 1.0))
            gfx.draw_line(torsoX + bodyW*0.5, torsoY + bodyH*0.25, torsoX + bodyW*0.5, torsoY + bodyH*0.4, 1.8*scale, mR, mG, mB, 1.0 * (alphaScale or 1.0))
        end
        
        -- Crossed jacket panels
        draw_line_t(torsoX + bodyW * 0.2, torsoY, torsoX + bodyW * 0.6, torsoY + bodyH * 0.45, 3.5 * scale, br * 1.25, bg * 1.25, bb * 1.25, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(torsoX + bodyW * 0.8, torsoY, torsoX + bodyW * 0.4, torsoY + bodyH * 0.45, 3.5 * scale, br * 1.25, bg * 1.25, bb * 1.25, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- 3. Draw Belt (Black / Accent) with 3D knot
        local sashY = torsoY + bodyH * 0.4
        draw_rect_t(torsoX - 2 * scale, sashY, bodyW + 4 * scale, 10 * scale, 0.15, 0.15, 0.15, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Belt central 3D knot
        local knotX = cx - 6 * scale * facing
        draw_rounded_rect_t(knotX, sashY - 2 * scale, 12 * scale, 14 * scale, 3, 0.08, 0.08, 0.08, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Flowing belt ribbons with waving curves
        local beltEndX = knotX + 6 * scale * facing
        draw_line_t(beltEndX, sashY + 5 * scale, beltEndX + 10 * facing * scale + math.sin(t*1.5) * 4 * scale, sashY + 30 * scale, 4.5 * scale, 0.12, 0.12, 0.12, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(beltEndX - 2 * facing * scale, sashY + 5 * scale, beltEndX + 5 * facing * scale + math.sin(t*1.2 + 0.5) * 4 * scale, sashY + 25 * scale, 4.0 * scale, 0.12, 0.12, 0.12, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- 4. Draw Arms (Sleeve + Wrist wraps + Red Gloves)
        -- Left Arm (Baggy sleeves and wraps)
        draw_line_t(lShoulderX, shoulderY, lElbowX, lElbowY, 11 * scale, br * 0.85, bg * 0.85, bb * 0.85, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(lElbowX, lElbowY, lHandX, lHandY, 9.5 * scale, br * 0.85, bg * 0.85, bb * 0.85, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Left Hand wraps (white)
        draw_circle_filled_t(lElbowX + (lHandX - lElbowX)*0.75, lElbowY + (lHandY - lElbowY)*0.75, 5.0 * scale, 1.0, 1.0, 1.0, 1.0, isReflecting, groundScreenY, alphaScale)
        -- Left glove (red)
        draw_circle_filled_t(lHandX, lHandY, 6.2 * scale, 0.95, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Right Arm
        draw_line_t(rShoulderX, shoulderY, rElbowX, rElbowY, 11 * scale, br * 0.85, bg * 0.85, bb * 0.85, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(rElbowX, rElbowY, rHandX, rHandY, 9.5 * scale, br * 0.85, bg * 0.85, bb * 0.85, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Right Hand wraps (white)
        draw_circle_filled_t(rElbowX + (rHandX - rElbowX)*0.75, rElbowY + (rHandY - rElbowY)*0.75, 5.0 * scale, 1.0, 1.0, 1.0, 1.0, isReflecting, groundScreenY, alphaScale)
        -- Right glove (red)
        draw_circle_filled_t(rHandX, rHandY, 6.2 * scale, 0.95, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Double-shaded sleeve folds
        if not isReflecting then
            draw_line_t(lShoulderX + 1.5*scale, shoulderY, lElbowX + 1.5*scale, lElbowY, 2.0*scale, br*1.2, bg*1.2, bb*1.2, 0.75, false, groundScreenY, alphaScale)
            draw_line_t(lShoulderX - 1.5*scale, shoulderY, lElbowX - 1.5*scale, lElbowY, 2.0*scale, br*0.6, bg*0.6, bb*0.6, 0.75, false, groundScreenY, alphaScale)
            
            draw_line_t(rShoulderX + 1.5*scale, shoulderY, rElbowX + 1.5*scale, rElbowY, 2.0*scale, br*1.2, bg*1.2, bb*1.2, 0.75, false, groundScreenY, alphaScale)
            draw_line_t(rShoulderX - 1.5*scale, shoulderY, rElbowX - 1.5*scale, rElbowY, 2.0*scale, br*0.6, bg*0.6, bb*0.6, 0.75, false, groundScreenY, alphaScale)
        end
        
        -- 5. Draw Head & Hair
        draw_circle_filled_t(headX, headY, headR, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Red Headband
        draw_rect_t(headX - headR * 0.9, headY - headR * 0.5, headR * 1.8, 6.5 * scale, 0.95, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Headband ribbons waving curves (Red and dark shadow layers)
        local ribbonAnchorX = headX - headR * 0.85 * facing
        local ribbonAnchorY = headY - headR * 0.3
        
        -- Shadow ribbon behind
        local srx, sry = ribbonAnchorX - 2.5 * facing * scale, ribbonAnchorY + 1.5 * scale
        for k = 1, 6 do
            local segmentTime = t * 1.5 - k * 0.3
            local nextSrx = srx - 5.5 * facing * scale
            local nextSry = sry + 2.5 * scale + math.sin(segmentTime) * 3.8 * scale
            draw_line_t(srx, sry, nextSrx, nextSry, (6 - k * 0.8) * scale, 0.5, 0.05, 0.05, 0.7, isReflecting, groundScreenY, alphaScale)
            srx, sry = nextSrx, nextSry
        end
        -- Main red ribbon
        local rx, ry = ribbonAnchorX, ribbonAnchorY
        for k = 1, 6 do
            local segmentTime = t * 1.5 - k * 0.3
            local nextRx = rx - 5.5 * facing * scale
            local nextRy = ry + 2.5 * scale + math.sin(segmentTime) * 3.8 * scale
            draw_line_t(rx, ry, nextRx, nextRy, (6 - k * 0.8) * scale, 0.95, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
            rx, ry = nextRx, nextRy
        end
        
        -- Spiky Black Hair
        local hairR, hairG, hairB = 0.06, 0.06, 0.06
        draw_triangle_t(headX - headR * 0.4, headY - headR * 0.7, headX + headR * 0.2, headY - headR * 0.7, headX - headR * 0.2 * facing, headY - headR * 1.5, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_triangle_t(headX - headR * 0.8 * facing, headY - headR * 0.5, headX - headR * 0.2 * facing, headY - headR * 0.7, headX - headR * 1.25 * facing, headY - headR * 1.2, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_triangle_t(headX - headR * 0.9 * facing, headY - headR * 0.1, headX - headR * 0.4 * facing, headY - headR * 0.5, headX - headR * 1.35 * facing, headY - headR * 0.7, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_triangle_t(headX + headR * 0.2 * facing, headY - headR * 0.7, headX + headR * 0.7 * facing, headY - headR * 0.3, headX + headR * 0.85 * facing, headY - headR * 0.9, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_triangle_t(headX - headR * 0.1, headY - headR * 0.8, headX + headR * 0.4, headY - headR * 0.8, headX + headR * 0.15 * facing, headY - headR * 1.6, hairR * 1.3, hairG * 1.3, hairB * 1.3, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_triangle_t(headX - headR * 0.6 * facing, headY - headR * 0.7, headX - headR * 0.3 * facing, headY - headR * 0.7, headX - headR * 0.75 * facing, headY - headR * 1.4, hairR * 1.3, hairG * 1.3, hairB * 1.3, 1.0, isReflecting, groundScreenY, alphaScale)

        -- Face details (Eyes & Brows)
        local eyeX = headX + (headR * 0.35) * facing
        local eyeY = headY - (headR * 0.15)
        if isHit then
            draw_rect_t(eyeX - 3.5 * scale, eyeY - 2 * scale, 7 * scale, 4 * scale, 1.0, 0.15, 0.15, 1.0, isReflecting, groundScreenY, alphaScale)
        else
            draw_rect_t(eyeX - 3.5 * scale, eyeY - 2 * scale, 7 * scale, 4 * scale, 1.0, 1.0, 1.0, 1.0, isReflecting, groundScreenY, alphaScale)
            draw_rect_t(eyeX - (facing == 1 and 1 or 2) * scale, eyeY - 1 * scale, 2.5 * scale, 2.5 * scale, 0.1, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
        end
        draw_line_t(eyeX - 5.5 * facing * scale, eyeY - 4 * scale, eyeX + 4.5 * facing * scale, eyeY - 3 * scale, 2.2 * scale, 0.05, 0.05, 0.05, 1.0, isReflecting, groundScreenY, alphaScale)

    elseif name == "GOROU" then
        -- ----------------- GOROU (GRAPPLER) -----------------
        local skinR, skinG, skinB = 0.85, 0.60, 0.45
        local hairR, hairG, hairB = 0.12, 0.12, 0.12
        
        -- Ground sparks under feet (Sharp Neon Flares)
        draw_feet_flares(lFootX, lFootY, false)
        draw_feet_flares(rFootX, rFootY, true)

        -- 1. Draw Legs (Wrestling pants/shorts + Black boots)
        draw_line_t(lHipX, hipY, lKneeX, lKneeY, 15 * scale, br, bg, bb, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(lKneeX, lKneeY, lFootX, lFootY, 13 * scale, br, bg, bb, 1.0, isReflecting, groundScreenY, alphaScale)
        
        draw_line_t(rHipX, hipY, rKneeX, rKneeY, 15 * scale, br, bg, bb, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(rKneeX, rKneeY, rFootX, rFootY, 13 * scale, br, bg, bb, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Boots overlays on calves
        draw_line_t(lKneeX + (lFootX-lKneeX)*0.45, lKneeY + (lFootY-lKneeY)*0.45, lFootX, lFootY, 13.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(rKneeX + (rFootX-rKneeX)*0.45, rKneeY + (rFootY-rKneeY)*0.45, rFootX, rFootY, 13.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Toe pointing
        local lToeX = lFootX + (facing == 1 and 6 * scale or -10 * scale)
        local rToeX = rFootX + (facing == 1 and 6 * scale or -10 * scale)
        draw_rect_t(lToeX, lFootY - 4 * scale, 4 * scale, 5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_rect_t(rToeX, rFootY - 4 * scale, 4 * scale, 5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- 2. Torso (Muscular Chest)
        draw_rect_t(torsoX, torsoY, bodyW, bodyH, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        -- Muscles definition
        local mR, mG, mB = skinR * 0.72, skinG * 0.72, skinB * 0.72
        draw_line_t(torsoX + bodyW * 0.15, torsoY + bodyH * 0.3, torsoX + bodyW * 0.48, torsoY + bodyH * 0.3, 2.2 * scale, mR, mG, mB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(torsoX + bodyW * 0.52, torsoY + bodyH * 0.3, torsoX + bodyW * 0.85, torsoY + bodyH * 0.3, 2.2 * scale, mR, mG, mB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(torsoX + bodyW * 0.5, torsoY + bodyH * 0.32, torsoX + bodyW * 0.5, torsoY + bodyH * 0.7, 2.2 * scale, mR, mG, mB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(torsoX + bodyW * 0.3, torsoY + bodyH * 0.45, torsoX + bodyW * 0.7, torsoY + bodyH * 0.45, 1.8 * scale, mR, mG, mB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(torsoX + bodyW * 0.32, torsoY + bodyH * 0.58, torsoX + bodyW * 0.68, torsoY + bodyH * 0.58, 1.8 * scale, mR, mG, mB, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Leather straps suspenders
        draw_line_t(torsoX + bodyW * 0.25, torsoY, torsoX + bodyW * 0.25, torsoY + bodyH * 0.7, 4.5 * scale, 0.1, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(torsoX + bodyW * 0.75, torsoY, torsoX + bodyW * 0.75, torsoY + bodyH * 0.7, 4.5 * scale, 0.1, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Wrestling championship belt
        local sashY = torsoY + bodyH * 0.68
        draw_rect_t(torsoX - 1 * scale, sashY, bodyW + 2 * scale, 13 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        -- Gold championship plate
        local plateW = bodyW * 0.35
        draw_rect_t(torsoX + bodyW * 0.5 - plateW * 0.5, sashY - 2 * scale, plateW, 17 * scale, 1.0, 0.85, 0.0, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- 3. Draw Arms (Skin + Wristbands)
        draw_line_t(lShoulderX, shoulderY, lElbowX, lElbowY, 13 * scale, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(lElbowX, lElbowY, lHandX, lHandY, 11 * scale, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        
        draw_line_t(rShoulderX, shoulderY, rElbowX, rElbowY, 13 * scale, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(rElbowX, rElbowY, rHandX, rHandY, 11 * scale, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Black wristbands
        draw_line_t(lElbowX + (lHandX-lElbowX)*0.5, lElbowY + (lHandY-lElbowY)*0.5, lHandX, lHandY, 11.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(rElbowX + (rHandX-rElbowX)*0.5, rElbowY + (rHandY-rElbowY)*0.5, rHandX, rHandY, 11.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- 4. Draw Head
        draw_circle_filled_t(headX, headY, headR, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_circle_filled_t(headX, headY - headR * 0.05, headR * 0.92, hairR * 1.5, hairG * 1.5, hairB * 1.5, 0.65, isReflecting, groundScreenY, alphaScale)
        
        draw_triangle_t(headX - headR * 0.5, headY - headR * 0.7, headX + headR * 0.3, headY - headR * 0.7, headX + headR * 0.1 * facing, headY - headR * 1.4, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_triangle_t(headX - headR * 0.2, headY - headR * 0.7, headX + headR * 0.5, headY - headR * 0.6, headX + headR * 0.4 * facing, headY - headR * 1.3, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_triangle_t(headX - headR * 0.85 * facing, headY + headR * 0.1, headX + headR * 0.7 * facing, headY + headR * 0.2, headX, headY + headR * 1.08, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Eyes & Eyebrows
        local eyeX = headX + (headR * 0.35) * facing
        local eyeY = headY - (headR * 0.15)
        if isHit then
            draw_rect_t(eyeX - 3 * scale, eyeY - 2 * scale, 6 * scale, 4 * scale, 1.0, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
        else
            draw_rect_t(eyeX - 3 * scale, eyeY - 2 * scale, 6 * scale, 4 * scale, 1.0, 1.0, 1.0, 1.0, isReflecting, groundScreenY, alphaScale)
            draw_rect_t(eyeX - (facing == 1 and 1 or 2) * scale, eyeY - 1 * scale, 2 * scale, 2 * scale, 0.1, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
        end
        draw_line_t(eyeX - 6 * facing * scale, eyeY - 4 * scale, eyeX + 5 * facing * scale, eyeY - 3 * scale, 3 * scale, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)

    elseif name == "RIN" then
        -- ----------------- RIN (NINJA) -----------------
        local skinR, skinG, skinB = 0.98, 0.85, 0.78
        local hairR, hairG, hairB = 0.45, 0.15, 0.65 -- Purple hair
        
        -- Ground sparks under feet (Sharp Neon Flares)
        draw_feet_flares(lFootX, lFootY, false)
        draw_feet_flares(rFootX, rFootY, true)

        -- 1. Draw Katana Handle on Back (Ninja sword strap/sheath)
        local sheathX1 = cx - 18 * facing * scale
        local sheathY1 = cy - bodyH * 0.9
        local sheathX2 = cx - 35 * facing * scale
        local sheathY2 = cy - bodyH * 1.5
        
        -- Sheath strap
        draw_line_t(sheathX1, sheathY1, sheathX2, sheathY2, 5.2 * scale, 0.1, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Katana Tsuba (Guard) as a thin cyan disk
        draw_line_t(sheathX2 - 4 * facing * scale, sheathY2 - 3 * scale, sheathX2 + 4 * facing * scale, sheathY2 + 3 * scale, 2.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Katana Hilt (Tsuka) wrapped detail
        local hiltEndX = sheathX2 - 10 * facing * scale
        local hiltEndY = sheathY2 - 14 * facing * scale
        draw_line_t(sheathX2, sheathY2, hiltEndX, hiltEndY, 3.5 * scale, 0.12, 0.12, 0.15, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_circle_filled_t(hiltEndX, hiltEndY, 2.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale) -- pommel
        
        -- Sheath tassel
        draw_line_t(sheathX1, sheathY1, sheathX1 - 2 * facing * scale, sheathY1 + 10 * scale, 1.5 * scale, ar, ag, ab, 0.85, isReflecting, groundScreenY, alphaScale)

        -- 2. Draw Legs (Ninja Pants + Knee Guards + Cyan Tabi Shoes)
        -- Left Leg
        draw_line_t(lHipX, hipY, lKneeX, lKneeY, 11 * scale, br * 0.9, bg * 0.9, bb * 0.9, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(lKneeX, lKneeY, lFootX, lFootY, 9 * scale, br * 0.9, bg * 0.9, bb * 0.9, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Left Knee Guard (Cyan with white shine)
        draw_circle_filled_t(lKneeX, lKneeY, 5.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        if not isReflecting then
            draw_circle_filled_t(lKneeX - 1*scale, lKneeY - 1*scale, 2.0 * scale, 1.0, 1.0, 1.0, 0.9, false, groundScreenY, alphaScale)
        end
        
        -- Right Leg
        draw_line_t(rHipX, hipY, rKneeX, rKneeY, 11 * scale, br * 0.9, bg * 0.9, bb * 0.9, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(rKneeX, rKneeY, rFootX, rFootY, 9 * scale, br * 0.9, bg * 0.9, bb * 0.9, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Right Knee Guard (Cyan with white shine)
        draw_circle_filled_t(rKneeX, rKneeY, 5.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        if not isReflecting then
            draw_circle_filled_t(rKneeX - 1*scale, rKneeY - 1*scale, 2.0 * scale, 1.0, 1.0, 1.0, 0.9, false, groundScreenY, alphaScale)
        end
        
        -- Tabi Shoes (Cyan)
        draw_rect_t(lFootX - 2 * scale, lFootY - 1 * scale, 12 * scale, 4 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_rect_t(rFootX - 2 * scale, rFootY - 1 * scale, 12 * scale, 4 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        
        local lToeX = lFootX + (facing == 1 and 6 * scale or -8 * scale)
        local rToeX = rFootX + (facing == 1 and 6 * scale or -8 * scale)
        draw_rect_t(lToeX, lFootY - 3 * scale, 4 * scale, 4 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_rect_t(rToeX, rFootY - 3 * scale, 4 * scale, 4 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- 3. Draw Torso (Purple Suit + Cyan Pauldrons + Sash Crosses)
        draw_rect_t(torsoX, torsoY, bodyW, bodyH, br, bg, bb, 1.0, isReflecting, groundScreenY, alphaScale)
        
        if not isReflecting then
            gfx.draw_rect(torsoX + 2*scale, torsoY + 2*scale, 3*scale, bodyH - 4*scale, 0.85, 0.3, 0.95, 0.25 * (alphaScale or 1.0))
        end
        
        -- Pauldrons (Cyan shoulder armor with white sheen reflection highlight)
        draw_circle_filled_t(lShoulderX, shoulderY, 7.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_circle_filled_t(rShoulderX, shoulderY, 7.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        if not isReflecting then
            draw_circle_filled_t(lShoulderX - 1.5*scale, shoulderY - 1.5*scale, 2.8 * scale, 1.0, 1.0, 1.0, 0.9, false, groundScreenY, alphaScale)
            draw_circle_filled_t(rShoulderX - 1.5*scale, shoulderY - 1.5*scale, 2.8 * scale, 1.0, 1.0, 1.0, 0.9, false, groundScreenY, alphaScale)
        end
        
        -- Cyan Sash Crosses
        draw_line_t(torsoX, torsoY + bodyH * 0.15, torsoX + bodyW, torsoY + bodyH * 0.65, 2.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(torsoX + bodyW, torsoY + bodyH * 0.15, torsoX, torsoY + bodyH * 0.65, 2.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- 4. Draw Arms (Sleeve + Cyan Wrist Guards + Hands)
        -- Left Arm
        draw_line_t(lShoulderX, shoulderY, lElbowX, lElbowY, 9 * scale, br * 0.85, bg * 0.85, bb * 0.85, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(lElbowX, lElbowY, lHandX, lHandY, 8 * scale, br * 0.85, bg * 0.85, bb * 0.85, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Left Wrist guard (Cyan)
        draw_line_t(lElbowX + (lHandX-lElbowX)*0.5, lElbowY + (lHandY-lElbowY)*0.5, lHandX, lHandY, 8.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        -- Left Hand
        draw_circle_filled_t(lHandX, lHandY, 3.5 * scale, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Right Arm
        draw_line_t(rShoulderX, shoulderY, rElbowX, rElbowY, 9 * scale, br * 0.85, bg * 0.85, bb * 0.85, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_line_t(rElbowX, rElbowY, rHandX, rHandY, 8 * scale, br * 0.85, bg * 0.85, bb * 0.85, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Right Wrist guard (Cyan)
        draw_line_t(rElbowX + (rHandX-rElbowX)*0.5, rElbowY + (rHandY-rElbowY)*0.5, rHandX, rHandY, 8.5 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        -- Right Hand
        draw_circle_filled_t(rHandX, rHandY, 3.5 * scale, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- 5. Draw Head (Skin + Mask + Eyes + Bangs)
        draw_circle_filled_t(headX, headY, headR, skinR, skinG, skinB, 1.0, isReflecting, groundScreenY, alphaScale)
        -- Mask (Cyan)
        draw_rect_t(headX - headR * 0.9, headY + headR * 0.15, headR * 1.8, headR * 0.85, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- Spiky side bangs and top hair
        draw_triangle_t(headX - headR * 0.9 * facing, headY - headR * 0.3, headX - headR * 0.4 * facing, headY - headR * 0.6, headX - headR * 1.25 * facing, headY - headR * 0.4, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_triangle_t(headX + headR * 0.4 * facing, headY - headR * 0.5, headX + headR * 0.8 * facing, headY + headR * 0.2, headX + headR * 0.9 * facing, headY - headR * 0.3, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
        draw_triangle_t(headX - headR * 0.4, headY - headR * 0.8, headX + headR * 0.4, headY - headR * 0.8, headX, headY - headR * 1.35, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
        
        -- High swinging ponytail (Double-layered chain for extra volume)
        local hairTieX = headX - headR * 0.4 * facing
        local hairTieY = headY - headR * 0.7
        draw_circle_filled_t(hairTieX, hairTieY, 4 * scale, ar, ag, ab, 1.0, isReflecting, groundScreenY, alphaScale) -- hair tie
        
        -- Layer 1: Background ponytail (darker purple, offset for 3D depth)
        local bpx, bpy = hairTieX - 3.0 * facing * scale, hairTieY + 2.0 * scale
        for k = 1, 8 do
            local segmentTime = t * 2.0 - k * 0.35 - 0.25
            local nextBpx = bpx - 5.5 * facing * scale
            local nextBpy = bpy + 1.5 * scale + math.sin(segmentTime) * 5.2 * scale
            local size = (7.0 - k * 0.65) * scale
            draw_circle_filled_t(bpx, bpy, size, hairR * 0.6, hairG * 0.6, hairB * 0.6, 0.85, isReflecting, groundScreenY, alphaScale)
            bpx, bpy = nextBpx, nextBpy
        end
        
        -- Layer 2: Main ponytail (glowing purple with light sheens)
        local px, py = hairTieX, hairTieY
        for k = 1, 8 do
            local segmentTime = t * 2.0 - k * 0.35
            local nextPx = px - 5.5 * facing * scale
            local nextPy = py + 1.2 * scale + math.sin(segmentTime) * 4.8 * scale
            local size = (7.8 - k * 0.65) * scale
            draw_circle_filled_t(px, py, size, hairR, hairG, hairB, 1.0, isReflecting, groundScreenY, alphaScale)
            -- Ponytail highlights
            if not isReflecting then
                draw_circle_filled_t(px - 0.5*scale, py - 0.5*scale, size * 0.68, 0.7, 0.35, 0.95, 0.6, false, groundScreenY, alphaScale)
            end
            px, py = nextPx, nextPy
        end
        
        -- Eyes
        local eyeX = headX + (headR * 0.35) * facing
        local eyeY = headY - (headR * 0.15)
        if isHit then
            draw_rect_t(eyeX - 3 * scale, eyeY - 1.5 * scale, 6 * scale, 3 * scale, 1.0, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
        else
            draw_rect_t(eyeX - 3 * scale, eyeY - 1.5 * scale, 6 * scale, 3 * scale, 0.1, 0.9, 0.9, 1.0, isReflecting, groundScreenY, alphaScale)
        end
        draw_line_t(eyeX - 5 * facing * scale, eyeY - 3 * scale, eyeX + 4 * facing * scale, eyeY - 2.5 * scale, 1.8 * scale, 0.1, 0.1, 0.1, 1.0, isReflecting, groundScreenY, alphaScale)
    end
end

-- Render function for game screens (Dojo stage matches)
function FighterRenderer.Draw(isP1, camX, camY, camZoom)
    local get_pos = isP1 and fight.get_p1_pos or fight.get_p2_pos
    local get_state = isP1 and fight.get_p1_state or fight.get_p2_state
    local get_facing = isP1 and fight.get_p1_facing or fight.get_p2_facing
    local get_colors = isP1 and fight.get_p1_colors or fight.get_p2_colors
    local get_name = isP1 and fight.get_p1_name or fight.get_p2_name
    
    local isEX = isP1 and fight.get_p1_ex() or fight.get_p2_ex()
    local isSuper = isP1 and fight.get_p1_super_active() or fight.get_p2_super_active()
    local isCounter = isP1 and fight.get_p1_counter() or fight.get_p2_counter()
    local isParry = isP1 and fight.get_p1_parry() or fight.get_p2_parry()
    local isCancel = isP1 and fight.get_p1_cancel and fight.get_p1_cancel() or fight.get_p2_cancel and fight.get_p2_cancel()
    
    local x, y = get_pos()
    local state = get_state()
    local facing = get_facing()
    local br, bg, bb, ar, ag, ab = get_colors()
    local name = get_name()
    
    local get_state_frame = isP1 and fight.get_p1_state_frame or fight.get_p2_state_frame
    local stateFrame = get_state_frame and get_state_frame() or 0
    
    local screenW = window.get_width()
    local screenH = window.get_height()
    
    -- Coordinate mapping
    local screenX = (screenW / 2) + (x - camX) * camZoom
    local screenY = 500 + (y - 200 - camY) * camZoom
    
    local t = time.get_time() * 10
    local isHit = (state == "hitstun_stand" or state == "hitstun_crouch" or state == "knockdown" or state == "ko")
    
    local hitPauseFrames = fight.get_hit_pause_frames and fight.get_hit_pause_frames() or 0
    if hitPauseFrames > 0 and isHit then
        local shake = ((hitPauseFrames % 2 == 0) and 3.5 or -3.5) * camZoom
        screenX = screenX + shake
    end
    
    local w, h = 50, 110
    if name == "GOROU" then
        w, h = 60, 125
    elseif name == "RIN" then
        w, h = 45, 115
    end
    w = w * camZoom
    h = h * camZoom
    
    local groundScreenY = 500 + (-camY) * camZoom
    
    -- Override body colors with Super golden tint / Parry flash / Cancel flash / Counter flash
    if isSuper then
        br, bg, bb = 1.0, 0.85, 0.1
    elseif isParry then
        br, bg, bb = 0.0, 0.9, 1.0 -- Cyber Cyan parry flash!
    elseif isCancel then
        br, bg, bb = 1.0, 0.1, 0.8 -- Hot Neon Magenta cancel flash!
    elseif isCounter then
        br, bg, bb = 0.1, 0.9, 0.9 -- Cyan flash
    end
    
    -- --- PASS 1: Floor reflection (drawn under grid floor) ---
    FighterRenderer.DrawFighterBody(name, screenX, screenY, camZoom, facing, state, br, bg, bb, ar, ag, ab, time.get_time(), isHit, true, groundScreenY, 1.0, stateFrame)
    
    -- --- PASS 2: Normal Fighter Rendering ---
    
    -- 2.1. Draw EX / Super trails
    if isEX then
        local trail1 = -18 * facing * camZoom
        local trail2 = -36 * facing * camZoom
        FighterRenderer.DrawFighterBody(name, screenX + trail1, screenY, camZoom, facing, state, 0.1, 0.7, 1.0, ar, ag, ab, time.get_time(), false, false, groundScreenY, 0.35, stateFrame)
        FighterRenderer.DrawFighterBody(name, screenX + trail2, screenY, camZoom, facing, state, 0.1, 0.7, 1.0, ar, ag, ab, time.get_time(), false, false, groundScreenY, 0.15, stateFrame)
    end
    
    if isSuper then
        local wave = math.sin(time.get_time() * 20) * 6 * camZoom
        local trail1 = -24 * facing * camZoom + wave
        local trail2 = -48 * facing * camZoom - wave
        FighterRenderer.DrawFighterBody(name, screenX + trail1, screenY, camZoom, facing, state, 1.0, 0.8, 0.1, ar, ag, ab, time.get_time(), false, false, groundScreenY, 0.45, stateFrame)
        FighterRenderer.DrawFighterBody(name, screenX + trail2, screenY, camZoom, facing, state, 1.0, 0.8, 0.1, ar, ag, ab, time.get_time(), false, false, groundScreenY, 0.20, stateFrame)
    end
    
    -- 2.2. Counter Sparkles
    if isCounter then
        -- Spawn visual sparkles/sparks procedurally around the body
        for i = 1, 4 do
            local rx = screenX - w*0.5 + math.random(-5, math.floor(w) + 5)
            local ry = screenY - h + math.random(-5, math.floor(h) + 5)
            local rSize = math.random(3, 5) * camZoom
            gfx.draw_rect(rx, ry, rSize, rSize, 1.0, 1.0, 1.0, 0.8)
        end
    end
    
    -- 2.3. Draw Ground Shadow under feet
    local shadowScale = math.max(0.2, 1.0 - (y - 200) / 180.0)
    local shadowAlpha = math.max(0.0, 0.55 * shadowScale)
    local shadowW = w * 1.35 * shadowScale
    local shadowH = 8 * camZoom * shadowScale
    local shadowY = 500 + (-camY) * camZoom
    gfx.draw_rounded_rect(screenX - shadowW * 0.5, shadowY - shadowH * 0.5, shadowW, shadowH, shadowH * 0.5, 0.01, 0.01, 0.02, shadowAlpha)
    
    -- 2.4. Draw the actual high detailed body
    FighterRenderer.DrawFighterBody(name, screenX, screenY, camZoom, facing, state, br, bg, bb, ar, ag, ab, time.get_time(), isHit, false, groundScreenY, 1.0, stateFrame)
end

-- Render function for UI select preview
function FighterRenderer.DrawCharPreview(name, cx, cy, scale, facing, t)
    local br, bg, bb, ar, ag, ab = 0.5, 0.5, 0.5, 0.5, 0.5, 0.5
    if name == "KAITO" then
        br, bg, bb = 0.1, 0.4, 0.9
        ar, ag, ab = 0.9, 0.7, 0.1
    elseif name == "GOROU" then
        br, bg, bb = 0.8, 0.2, 0.2
        ar, ag, ab = 0.3, 0.3, 0.3
    elseif name == "RIN" then
        br, bg, bb = 0.6, 0.1, 0.8
        ar, ag, ab = 0.1, 0.9, 0.9
    end
    
    -- Draw shadow under preview character
    local w, h = 50, 110
    if name == "GOROU" then w, h = 60, 125
    elseif name == "RIN" then w, h = 45, 115 end
    local shadowW = w * scale * 1.3
    local shadowH = 8 * scale
    gfx.draw_rounded_rect(cx - shadowW * 0.5, cy - shadowH * 0.5, shadowW, shadowH, shadowH * 0.5, 0.01, 0.01, 0.02, 0.45)
    
    -- Call the unified DrawFighterBody
    FighterRenderer.DrawFighterBody(name, cx, cy, scale, facing, "idle", br, bg, bb, ar, ag, ab, t, false, false, cy, 1.0, math.floor(t * 60))
end

return FighterRenderer
