local Stages = {}

-- Helper functions for horizontal wave distortion (sinusoidal water ripple) on floor reflections
local function draw_reflected_rect(rx, ry, rw, rh, r, g, b, a, groundY, t)
    local ry_ref = 2 * groundY - (ry + rh)
    local rr = r * 0.65 + 0.12
    local rg = g * 0.35
    local rb = b * 0.75 + 0.16
    local step = 4
    for sy = 0, rh - 1, step do
        local cur_h = math.min(step, rh - sy)
        local cur_y = ry_ref + sy
        local shift = math.sin(t * 7.5 + cur_y * 0.08) * 6.2
        local fade = math.max(0.0, 1.0 - (cur_y - groundY) / 250.0)
        gfx.draw_rect(rx + shift, cur_y, rw, cur_h, rr, rg, rb, a * fade)
    end
end

local function draw_reflected_circle_filled(cx, cy, radius, r, g, b, a, groundY, t)
    local cy_ref = 2 * groundY - cy
    local rr = r * 0.65 + 0.12
    local rg = g * 0.35
    local rb = b * 0.75 + 0.16
    local step = 4
    for sy = -radius, radius - 1, step do
        local cur_y = cy_ref + sy
        local half_w = math.sqrt(math.max(0, radius * radius - sy * sy))
        local shift = math.sin(t * 7.5 + cur_y * 0.08) * 6.2
        local fade = math.max(0.0, 1.0 - (cur_y - groundY) / 250.0)
        gfx.draw_rect(cx - half_w + shift, cur_y, half_w * 2, math.min(step, radius - sy), rr, rg, rb, a * fade)
    end
end

local function draw_reflected_triangle(x1, y1, x2, y2, x3, y3, r, g, b, a, groundY, t)
    -- Sort vertices by y coordinate so y1 <= y2 <= y3
    if y1 > y2 then x1, y1, x2, y2 = x2, y2, x1, y1 end
    if y1 > y3 then x1, y1, x3, y3 = x3, y3, x1, y1 end
    if y2 > y3 then x2, y2, x3, y3 = x3, y3, x2, y2 end
    
    local rr = r * 0.65 + 0.12
    local rg = g * 0.35
    local rb = b * 0.75 + 0.16
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
        
        local cur_y = 2 * groundY - y
        local shift = math.sin(t * 7.5 + cur_y * 0.08) * 6.2
        local fade = math.max(0.0, 1.0 - (cur_y - groundY) / 250.0)
        gfx.draw_rect(xl + shift, cur_y - step, xr - xl, step, rr, rg, rb, a * fade)
    end
end

local function draw_reflected_rounded_rect(rx, ry, rw, rh, radius, r, g, b, a, groundY, t)
    local ry_ref = 2 * groundY - (ry + rh)
    local rr = r * 0.65 + 0.12
    local rg = g * 0.35
    local rb = b * 0.75 + 0.16
    local step = 4
    for sy = 0, rh - 1, step do
        local cur_h = math.min(step, rh - sy)
        local cur_y = ry_ref + sy
        local shift = math.sin(t * 7.5 + cur_y * 0.08) * 6.2
        local fade = math.max(0.0, 1.0 - (cur_y - groundY) / 250.0)
        local offset = 0
        if sy < radius then
            offset = radius - math.sqrt(radius * radius - (radius - sy) * (radius - sy))
        elseif sy > rh - radius then
            offset = radius - math.sqrt(radius * radius - (sy - (rh - radius)) * (sy - (rh - radius)))
        end
        gfx.draw_rect(rx + offset + shift, cur_y, rw - 2 * offset, cur_h, rr, rg, rb, a * fade)
    end
end

function Stages.Draw(stageIndex, cameraX, cameraZoom)
    local screenW = window.get_width()
    local screenH = window.get_height()
    
    local groundY = 500.0 -- Screen Y ground line
    local t = time.get_time()
    
    if stageIndex == 0 then
        -- Dojo Neon (High Fidelity Cyberpunk/Synthwave Dojo)
        -- Sky Background Gradient (Deep cosmic violet to neon magenta)
        local steps = 15
        local segmentH = groundY / steps
        for i = 0, steps - 1 do
            local yPos = i * segmentH
            local pct = i / (steps - 1)
            local r = 0.05 + 0.35 * pct
            local g = 0.01 + 0.02 * pct
            local b = 0.08 + 0.25 * pct
            gfx.draw_rect(0, yPos, screenW, segmentH, r, g, b, 1.0)
        end
        
        -- Neon Stars/Dust
        math.randomseed(42)
        for i = 1, 30 do
            local x = (math.random(0, screenW) - cameraX * 0.15) % screenW
            local y = math.random(20, 240)
            local sz = math.random(2, 4)
            local pulse = 0.4 + 0.6 * math.abs(math.sin(t * 3.5 + i))
            gfx.draw_rect(x, y, sz, sz, 0.0, 0.85, 1.0, pulse * 0.55)
            
            -- Reflect star
            draw_reflected_rect(x, y, sz, sz, 0.0, 0.85, 1.0, pulse * 0.18, groundY, t)
        end
        
        -- Giant Outrun Sun
        local sunX = screenW * 0.5 - cameraX * 0.1
        local sunY = 220
        local sunR = 100
        gfx.draw_circle_filled(sunX, sunY, sunR, 1.0, 0.25, 0.45, 0.9)
        gfx.draw_circle_filled(sunX, sunY, sunR - 4, 1.0, 0.55, 0.15, 0.95)
        gfx.draw_circle_filled(sunX, sunY, sunR - 12, 1.0, 0.75, 0.05, 0.95)
        -- Iconic outrun stripes
        for cutY = sunY - sunR + 30, sunY + sunR - 5, 12 do
            local thickness = 2.5 + (cutY - (sunY - sunR)) * 0.04
            gfx.draw_rect(sunX - sunR - 10, cutY, (sunR + 10) * 2, thickness, 0.05, 0.01, 0.1, 1.0)
        end
        
        -- Parallax Mountains (Deep back layer)
        math.randomseed(101)
        local baseMtnX = (-cameraX * 0.15)
        for i = 1, 8 do
            local mx = (baseMtnX + (i-1) * 260) % (screenW + 400) - 200
            local mh = 140 + math.sin(i * 1.8) * 50
            gfx.draw_triangle(mx, groundY, mx + 160, groundY - mh, mx + 320, groundY, 0.06, 0.02, 0.12, 1.0)
            -- Mountain reflection on floor with rippling wave distortion
            draw_reflected_triangle(mx, groundY, mx + 160, groundY - mh, mx + 320, groundY, 0.06, 0.02, 0.12, 0.08, groundY, t)
        end
        
        -- Parallax Skyscrapers (Mid layer) with Glowing Window Grids, Antennas, Water Towers, and Slanted Rooftops
        math.randomseed(2026)
        local baseCityX = (-cameraX * 0.25)
        for i = 1, 12 do
            local bx = (baseCityX + (i-1) * 160) % (screenW + 300) - 150
            local bw = 70 + math.random(20, 50)
            local bh = 180 + math.random(60, 150)
            local by = groundY - bh
            
            -- Decide architectural extras deterministically based on index
            local hasSlantRoof = (i % 3 == 0)
            local hasAntenna = (i % 3 == 1)
            local hasWaterTower = (i % 3 == 2)
            
            -- Building body
            gfx.draw_rect(bx, by, bw, bh, 0.04, 0.02, 0.08, 0.95)
            draw_reflected_rect(bx, by, bw, bh, 0.04, 0.02, 0.08, 0.06, groundY, t)
            
            -- Building vertical highlights (Neon borders with glowing outlines)
            local borderR, borderG, borderB = 0.85, 0.0, 0.85 -- Magenta
            if i % 2 == 1 then
                borderR, borderG, borderB = 0.0, 0.85, 1.0 -- Cyan
            end
            
            -- Left edge outline
            gfx.draw_rect(bx - 1.5, by, 4.5, bh, borderR, borderG, borderB, 0.15) -- Outer glow
            gfx.draw_rect(bx, by, 1.5, bh, borderR, borderG, borderB, 0.8) -- Core
            draw_reflected_rect(bx - 1.5, by, 4.5, bh, borderR, borderG, borderB, 0.02, groundY, t)
            draw_reflected_rect(bx, by, 1.5, bh, borderR, borderG, borderB, 0.08, groundY, t)

            -- Right edge outline
            gfx.draw_rect(bx + bw - 3, by, 4.5, bh, borderR, borderG, borderB, 0.15) -- Outer glow
            gfx.draw_rect(bx + bw - 1.5, by, 1.5, bh, borderR, borderG, borderB, 0.8) -- Core
            draw_reflected_rect(bx + bw - 3, by, 4.5, bh, borderR, borderG, borderB, 0.02, groundY, t)
            draw_reflected_rect(bx + bw - 1.5, by, 1.5, bh, borderR, borderG, borderB, 0.08, groundY, t)
            
            -- Flat roof outline highlight
            if not hasSlantRoof then
                gfx.draw_rect(bx, by - 1.5, bw, 4.5, borderR, borderG, borderB, 0.2) -- Outer glow
                gfx.draw_rect(bx, by, bw, 1.5, borderR, borderG, borderB, 0.9) -- Core
                draw_reflected_rect(bx, by - 1.5, bw, 4.5, borderR, borderG, borderB, 0.03, groundY, t)
                draw_reflected_rect(bx, by, bw, 1.5, borderR, borderG, borderB, 0.09, groundY, t)
            end
            
            -- Slanted Roof details
            if hasSlantRoof then
                local slantH = 22
                gfx.draw_triangle(bx, by, bx + bw, by, bx + bw, by - slantH, 0.04, 0.02, 0.08, 0.95)
                draw_reflected_triangle(bx, by, bx + bw, by, bx + bw, by - slantH, 0.04, 0.02, 0.08, 0.06, groundY, t)
                
                -- Slanted roof neon line (with glow)
                gfx.draw_line(bx, by, bx + bw, by - slantH, 4.5, borderR, borderG, borderB, 0.2) -- Glow
                gfx.draw_line(bx, by, bx + bw, by - slantH, 1.8, borderR, borderG, borderB, 0.95) -- Core
                
                local y1_ref = 2 * groundY - by
                local y2_ref = 2 * groundY - (by - slantH)
                local shift1 = math.sin(t * 7.5 + y1_ref * 0.08) * 6.2
                local shift2 = math.sin(t * 7.5 + y2_ref * 0.08) * 6.2
                local rr = borderR * 0.65 + 0.12
                local rg = borderG * 0.35
                local rb = borderB * 0.75 + 0.16
                local fade = math.max(0.0, 1.0 - (y1_ref - groundY) / 250.0)
                gfx.draw_line(bx + shift1, y1_ref, bx + bw + shift2, y2_ref, 4.5, rr, rg, rb, 0.02 * fade)
                gfx.draw_line(bx + shift1, y1_ref, bx + bw + shift2, y2_ref, 1.8, rr, rg, rb, 0.08 * fade)
            end
            
            -- Antenna details
            if hasAntenna then
                local antX = bx + bw * 0.5
                local antH = 36
                gfx.draw_rect(antX - 1, by - antH, 2, antH, 0.15, 0.15, 0.2, 0.95)
                gfx.draw_rect(antX - 6, by - antH + 12, 12, 1.5, 0.15, 0.15, 0.2, 0.95)
                gfx.draw_rect(antX - 4, by - antH + 22, 8, 1.5, 0.15, 0.15, 0.2, 0.95)
                local blink = 0.4 + 0.6 * math.abs(math.sin(t * 6.0 + i))
                gfx.draw_circle_filled(antX, by - antH, 2.5, 1.0 * blink, 0.1, 0.1, 0.95)
                
                draw_reflected_rect(antX - 1, by - antH, 2, antH, 0.15, 0.15, 0.2, 0.06, groundY, t)
                draw_reflected_circle_filled(antX, by - antH, 2.5, 1.0 * blink, 0.1, 0.1, 0.06, groundY, t)
            end
            
            -- Water Tower details
            if hasWaterTower then
                local wtX = bx + bw * 0.4
                gfx.draw_line(wtX - 5, by, wtX - 2, by - 12, 1.5, 0.15, 0.15, 0.2, 0.95)
                gfx.draw_line(wtX + 5, by, wtX + 2, by - 12, 1.5, 0.15, 0.15, 0.2, 0.95)
                gfx.draw_rect(wtX - 7, by - 14, 14, 1.5, 0.2, 0.2, 0.25, 0.95)
                gfx.draw_rect(wtX - 6, by - 26, 12, 12, 0.3, 0.3, 0.35, 0.95)
                gfx.draw_triangle(wtX - 6, by - 26, wtX + 6, by - 26, wtX, by - 31, 0.25, 0.25, 0.3, 0.95)
                
                -- Reflection
                local y1_ref = 2 * groundY - by
                local y2_ref = 2 * groundY - (by - 12)
                local shift1 = math.sin(t * 7.5 + y1_ref * 0.08) * 6.2
                local shift2 = math.sin(t * 7.5 + y2_ref * 0.08) * 6.2
                gfx.draw_line(wtX - 5 + shift1, y1_ref, wtX - 2 + shift2, y2_ref, 1.5, 0.15 * 0.65 + 0.12, 0.05, 0.3, 0.05)
                gfx.draw_line(wtX + 5 + shift1, y1_ref, wtX + 2 + shift2, y2_ref, 1.5, 0.15 * 0.65 + 0.12, 0.05, 0.3, 0.05)
                draw_reflected_rect(wtX - 7, by - 14, 14, 1.5, 0.2, 0.2, 0.25, 0.05, groundY, t)
                draw_reflected_rect(wtX - 6, by - 26, 12, 12, 0.3, 0.3, 0.35, 0.05, groundY, t)
                draw_reflected_triangle(wtX - 6, by - 26, wtX + 6, by - 26, wtX, by - 31, 0.25, 0.25, 0.3, 0.05, groundY, t)
            end
            
            -- Glowing windows grid
            local rows = math.floor(bh / 20) - 1
            local cols = math.floor(bw / 16) - 1
            for r = 1, rows do
                for c = 1, cols do
                    local winRand = math.random()
                    if winRand < 0.45 then
                        local wx = bx + c * 16 + 2
                        local wy = by + r * 20 + 2
                        local wr, wg, wb = 1.0, 0.9, 0.4
                        if winRand < 0.15 then
                            wr, wg, wb = 0.0, 0.85, 1.0
                        elseif winRand < 0.3 then
                            wr, wg, wb = 1.0, 0.1, 0.7
                        end
                        gfx.draw_rect(wx, wy, 6, 8, wr, wg, wb, 0.6)
                        draw_reflected_rect(wx, wy, 6, 8, wr, wg, wb, 0.04, groundY, t)
                    end
                end
            end
        end
        
        -- Vertical Hologram/Neon Signs on Buildings
        math.randomseed(303)
        for i = 1, 4 do
            local bx = (baseCityX + i * 400) % (screenW + 300) - 150
            local by = groundY - 240
            local text = "KAITO"
            local tr, tg, tb = 1.0, 0.1, 0.6 -- pink
            if i == 2 then
                text = "RIN"
                tr, tg, tb = 0.0, 0.85, 1.0 -- cyan
                by = groundY - 280
            elseif i == 3 then
                text = "DOJO"
                tr, tg, tb = 0.0, 0.9, 0.4 -- green
                by = groundY - 220
            elseif i == 4 then
                text = "LOVE"
                tr, tg, tb = 1.0, 0.8, 0.1 -- yellow
                by = groundY - 250
            end
            
            local signW = 18
            local signH = #text * 18 + 10
            
            -- Draw neon banner frame
            gfx.draw_rounded_rect(bx - 2, by - 5, signW + 4, signH, 4, tr, tg, tb, 0.15)
            gfx.draw_rect_outline(bx, by - 3, signW, signH - 4, 1.5, tr, tg, tb, 0.7)
            
            local letterY = by
            for j = 1, #text do
                local char = string.sub(text, j, j)
                gfx.draw_text(char, bx + 4, letterY, 0.8, tr, tg, tb, 0.9)
                letterY = letterY + 16
            end
            
            -- Reflection of vertical signs with sinusoidal rippling distortion
            draw_reflected_rounded_rect(bx - 2, by - 5, signW + 4, signH, 4, tr, tg, tb, 0.03, groundY, t)
            
            -- Frame outline reflection
            draw_reflected_rect(bx, by - 3, signW, 1.5, tr, tg, tb, 0.06, groundY, t) -- top
            draw_reflected_rect(bx, by - 3 + signH - 4 - 1.5, signW, 1.5, tr, tg, tb, 0.06, groundY, t) -- bottom
            draw_reflected_rect(bx, by - 3 + 1.5, 1.5, signH - 4 - 3, tr, tg, tb, 0.06, groundY, t) -- left
            draw_reflected_rect(bx + signW - 1.5, by - 3 + 1.5, 1.5, signH - 4 - 3, tr, tg, tb, 0.06, groundY, t) -- right
            
            -- Mirrored letters reflection
            local rLetterY = by
            for j = 1, #text do
                local char = string.sub(text, j, j)
                local ry_letter = 2 * groundY - rLetterY - 16
                local shift = math.sin(t * 7.5 + ry_letter * 0.08) * 6.2
                gfx.draw_text(char, bx + 4 + shift, ry_letter, 0.8, tr, tg, tb, 0.04)
                rLetterY = rLetterY + 16
            end
        end
        
        -- Hanging Chinese Lanterns on Looping Cables (including detailed paper rib folds)
        local pulseLight = 0.75 + 0.25 * math.abs(math.sin(t * 3.5))
        local baseLanternX = (-cameraX * 0.75)
        for c = 1, 3 do
            local startX = baseLanternX + (c - 1) * (screenW * 0.45) - 100
            local endX = startX + screenW * 0.5
            local sag = 70
            
            local segments = 8
            local prevX, prevY = startX, 40
            for s = 1, segments do
                local progress = s / segments
                local x = startX + progress * (endX - startX)
                local y = 40 + (progress - 0.5)^2 * 4 * sag
                gfx.draw_line(prevX, prevY, x, y, 1.2, 0.1, 0.1, 0.15, 0.6)
                
                if s == 2 or s == 4 or s == 6 then
                    local lx = x
                    local ly = y
                    local length = 25
                    local swing = math.sin(t * 1.5 + s) * 0.08
                    local endLx = lx + math.sin(swing) * length
                    local endLy = ly + math.cos(swing) * length
                    
                    gfx.draw_line(lx, ly, endLx, endLy, 1.2, 0.15, 0.15, 0.2, 0.7)
                    
                    local isOrange = (s % 4 == 0)
                    local lr, lg, lb = 1.0, 0.45, 0.08
                    if not isOrange then
                        lr, lg, lb = 0.0, 0.85, 1.0
                    end
                    
                    local p = pulseLight * (0.9 + 0.1 * math.sin(t * 8.0 + s))
                    local lanternY = endLy + 10
                    gfx.draw_circle_filled(endLx, lanternY, 10, lr * p, lg * p, lb * p, 0.9)
                    gfx.draw_circle(endLx, lanternY, 13, lr, lg, lb, 0.25)
                    
                    gfx.draw_rect(endLx - 6, endLy - 1, 12, 3, 0.1, 0.1, 0.1, 0.9)
                    gfx.draw_rect(endLx - 6, endLy + 19, 12, 3, 0.1, 0.1, 0.1, 0.9)
                    gfx.draw_rect(endLx - 1, endLy + 22, 2, 7, 0.9, 0.15, 0.2, 0.8)
                    
                    -- Folds (costuras) inside the Chinese lantern
                    local foldR, foldG, foldB = 0.02, 0.01, 0.03
                    gfx.draw_line(endLx, lanternY - 9.5, endLx, lanternY + 9.5, 1.0, foldR, foldG, foldB, 0.5)
                    gfx.draw_line(endLx - 4.5, lanternY - 8.0, endLx - 4.5, lanternY + 8.0, 1.0, foldR, foldG, foldB, 0.45)
                    gfx.draw_line(endLx + 4.5, lanternY - 8.0, endLx + 4.5, lanternY + 8.0, 1.0, foldR, foldG, foldB, 0.45)
                    
                    -- Reflection of lantern with sinusoidal wave distortion
                    draw_reflected_circle_filled(endLx, lanternY, 10, lr * 0.25, lg * 0.25, lb * 0.25, 0.12, groundY, t)
                end
                prevX, prevY = x, y
            end
        end
        
        -- Sun Reflection on Shiny Floor (Rippled)
        draw_reflected_circle_filled(sunX, sunY, sunR, 1.0, 0.3, 0.5, 0.07, groundY, t)
        draw_reflected_circle_filled(sunX, sunY, sunR - 8, 1.0, 0.5, 0.2, 0.09, groundY, t)
        -- Mirroring outrun cuts in sun reflection
        for cutY = sunY - sunR + 30, sunY + sunR - 5, 12 do
            local thickness = 2.5 + (cutY - (sunY - sunR)) * 0.04
            draw_reflected_rect(sunX - sunR - 10, cutY, (sunR + 10) * 2, thickness, 0.05, 0.01, 0.1, 0.07, groundY, t)
        end
        
        -- Ground Floor (Shiny Obsidian Grid)
        gfx.draw_rect(0, groundY, screenW, screenH - groundY, 0.04, 0.02, 0.08, 1.0)
        -- Glowing horizon border
        gfx.draw_rect(0, groundY, screenW, 3, 1.0, 0.0, 1.0, 0.95)
        gfx.draw_rect(0, groundY - 2, screenW, 7, 1.0, 0.0, 1.0, 0.4)
        
        -- Perspective Grid Lines
        local gridFloorX = (-cameraX * 1.0) % 50
        for x = gridFloorX - 150, screenW + 150, 50 do
            local x1 = x
            local x2 = (x - screenW * 0.5) * 1.68 + screenW * 0.5
            gfx.draw_line(x1, groundY, x2, screenH, 2.2, 0.85, 0.05, 0.85, 0.38)
        end
        for y = groundY, screenH, 30 do
            local progress = (y - groundY) / (screenH - groundY)
            local py = groundY + progress^1.4 * (screenH - groundY)
            local alpha = (1.0 - progress) * 0.35
            gfx.draw_rect(0, py - 0.5, screenW, 1.0 + (1.0 - progress)*1.5, 0.85, 0.05, 0.85, alpha)
        end
    else
        -- Steel Arena (Industrial Factory)
        -- Sky Background
        gfx.draw_rect(0, 0, screenW, screenH, 0.07, 0.07, 0.09, 1.0)
        
        -- Background Industrial Skyline (Silos/Factories)
        math.randomseed(99)
        local baseIndX = (-cameraX * 0.15)
        for i = 1, 8 do
            local ix = (baseIndX + (i-1) * 220) % (screenW + 300) - 150
            local iw = 45 + math.random(0, 35)
            local ih = 110 + math.random(0, 90)
            gfx.draw_rect(ix, groundY - ih, iw, ih, 0.035, 0.035, 0.045, 1.0)
            
            -- Flashing warning lights on top
            local warningPulse = 0.4 + 0.6 * math.abs(math.sin(t * 4.5 + i))
            gfx.draw_rect(ix + iw * 0.5 - 2, groundY - ih - 3, 4, 4, 1.0 * warningPulse, 0.1, 0.1, 0.9)
        end
        
        -- Metal Encanamentos (Girders and Pipes)
        local basePipesX = (-cameraX * 0.35) % 320
        for x = basePipesX - 320, screenW + 320, 320 do
            gfx.draw_rect(x, 150, 16, 350, 0.13, 0.13, 0.15, 0.8) -- Vertical pipe
            gfx.draw_rect(x - 160, 190, 320, 12, 0.11, 0.11, 0.13, 0.8) -- Horizontal pipe
        end
        
        -- Procedural Steam Vents (Puffing from pipes)
        for x = basePipesX - 320, screenW + 320, 320 do
            for j = 1, 3 do
                local delay = (j - 1) * 1.5
                local prog = ((t + delay) % 4.5) / 4.5
                local steamY = 190 - prog * 150
                local steamX = x + math.sin(prog * 12 + j) * 18
                local steamSize = 12 + prog * 22
                local alpha = (1.0 - prog) * 0.25
                gfx.draw_circle_filled(steamX, steamY, steamSize, 0.8, 0.8, 0.85, alpha)
            end
        end
        
        -- Triple Interconnected Gears (Rotating with teeth)
        local gearX = (screenW * 0.5 - cameraX * 0.5)
        local gearY = 220
        local gearAngle = t * 0.38
        
        local function drawDetailedGear(gx, gy, gr, angle, direction)
            local finalAngle = angle * direction
            -- Outer ring
            gfx.draw_circle_filled(gx, gy, gr, 0.22, 0.22, 0.24, 0.65)
            gfx.draw_circle_filled(gx, gy, gr - 8, 0.12, 0.12, 0.14, 0.85)
            -- Axle hub
            gfx.draw_circle_filled(gx, gy, gr * 0.35, 0.06, 0.06, 0.08, 1.0)
            
            -- Gear teeth
            local numTeeth = 10
            for i = 0, numTeeth - 1 do
                local ta = finalAngle + i * (math.pi * 2 / numTeeth)
                local tx1 = gx + math.cos(ta - 0.12) * (gr - 4)
                local ty1 = gy + math.sin(ta - 0.12) * (gr - 4)
                local tx2 = gx + math.cos(ta + 0.12) * (gr - 4)
                local ty2 = gy + math.sin(ta + 0.12) * (gr - 4)
                local tx3 = gx + math.cos(ta + 0.08) * (gr + 10)
                local ty3 = gy + math.sin(ta + 0.08) * (gr + 10)
                local tx4 = gx + math.cos(ta - 0.08) * (gr + 10)
                local ty4 = gy + math.sin(ta - 0.08) * (gr + 10)
                
                gfx.draw_triangle(tx1, ty1, tx2, ty2, tx3, ty3, 0.22, 0.22, 0.24, 0.65)
                gfx.draw_triangle(tx1, ty1, tx3, ty3, tx4, ty4, 0.22, 0.22, 0.24, 0.65)
            end
        end
        
        drawDetailedGear(gearX, gearY, 78, gearAngle, 1) -- Central
        drawDetailedGear(gearX - 116, gearY + 30, 44, gearAngle * (78/44), -1) -- Left
        drawDetailedGear(gearX + 116, gearY + 30, 44, gearAngle * (78/44), -1) -- Right
        
        -- Falling Welding Sparks (Background ambient effect)
        math.randomseed(55)
        for i = 1, 5 do
            local spawnX = (screenW * 0.2 + i * 150 - cameraX * 0.4) % (screenW + 200) - 100
            local prog = ((t * 0.68 + i * 0.45) % 1.5) / 1.5
            local sparkY = 80 + prog * 400
            local sparkX = spawnX + prog * 35
            local alpha = (1.0 - prog) * 0.95
            
            if prog < 0.92 then
                gfx.draw_rect(sparkX, sparkY, 3, 3, 1.0, 0.6, 0.1, alpha)
                gfx.draw_line(sparkX, sparkY, sparkX - 3, sparkY - 7, 1.2, 1.0, 0.5, 0.15, alpha * 0.65)
            end
        end
        
        -- Ground Floor (Metal Grates)
        gfx.draw_rect(0, groundY, screenW, screenH - groundY, 0.12, 0.12, 0.14, 1.0)
        gfx.draw_rect(0, groundY, screenW, 5, 0.75, 0.32, 0.08, 0.95) -- Rust glow line
        
        local floorLines = (-cameraX * 1.0) % 80
        for x = floorLines - 100, screenW + 100, 80 do
            gfx.draw_rect(x, groundY, 3, screenH - groundY, 0.06, 0.06, 0.07, 0.7)
            gfx.draw_rect(x + 5, groundY + 12, 2, 4, 0.04, 0.04, 0.05, 0.6)
        end
    end
end

return Stages
