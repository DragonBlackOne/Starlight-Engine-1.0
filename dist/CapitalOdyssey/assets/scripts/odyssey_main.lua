-- odyssey_main.lua — Capital Odyssey v11.0 (SBA v4.0 Industrial)
-- Uses: Entity, Light, Tween, Events, Scene, Coroutine, MathX, Audio
-- ============================================================================
package.path = package.path .. ";assets/scripts/?.lua"

require("sba_bridge")
require("MarketSystem")
require("Portfolio")
require("NewsGenerator")

-- ============================================================================
-- Game State
-- ============================================================================
GameTime = { year = 1990, tick = 0, era = "1990s", transitionTimer = 0, transitionMsg = "" }
Buildings = {}
Grid = {} -- Spatial Grid for O(1) lookup: Grid[x][z] = building_entity
FloorTiles = {}

-- ============================================================================
-- Particle VFX System (uses Entity wrapper)
-- ============================================================================
function SpawnParticles(px, py, pz, r, g, b)
    vfx.emit(px, py, pz, 6, 8, 6, r, g, b, 60, 0.15)
end

-- ============================================================================
-- Scene: Game
-- ============================================================================
Scene.register("Game", {
    onEnter = function()
        Say("Capital Odyssey v11.0 — SBA v4.0 Industrial Powered")
        Engine.set_bloom(0.9, 10)
        Engine.set_exposure(1.3, 2.2)
        Market.Init()
        Player.Init()
        Player.Cash = Save.read("odyssey_cash", 10000.0)
        Player.AutoInvestEnabled = (Save.read("odyssey_autoinvest", 0) == 1)
        Player.SelectedCompany = "Starlight Tech"
        News.Init()
        
        GameTime.year = 1990
        GameTime.tick = 0
        GameTime.era = "1990s"
        GameTime.transitionTimer = 0
        GameTime.transitionMsg = ""

        -- Camera
        Engine.set_camera_pos(15, 20, 15)
        Engine.look_at(0, 0, 0)

        -- Sun (using Light wrapper!)
        Sun = Light(10, 20, 10, 1.0, 0.9, 0.7, 1200)

        -- Floor Grid (using Entity wrapper!)
        for i = -8, 8 do
            for j = -8, 8 do
                local tile = Entity("Floor", i, -0.5, j)
                tile:setScale(0.95, 0.1, 0.95)
                tile:setMaterial(0.2, 0.8)
                if (i + j) % 2 == 0 then
                    tile:setColor(0.1, 0.1, 0.15)
                else
                    tile:setColor(0.05, 0.05, 0.1)
                end
                table.insert(FloorTiles, tile)
            end
        end

        -- Holographic Cursor (Entity!)
        Cursor = Entity("Cursor", 0, 0, 0)
        Cursor:setScale(1.1, 0.2, 1.1)
        Cursor:setColor(0.0, 1.0, 0.5)
        Cursor:setMaterial(1.0, 0.0)
        Cursor._scaleX = 1.1; Cursor._scaleY = 0.2; Cursor._scaleZ = 1.1
        Cursor._gridX = 0; Cursor._gridZ = 0
        Cursor._cooldown = 0

        -- HQ (Entity + Tween!)
        local hq = Entity("HQ", 0, -0.45, 0)
        hq:setColor(0.0, 0.6, 1.0)
        hq:setMaterial(0.8, 0.2)
        hq:setScale(0.8, 0.1, 0.8)
        hq._scaleX = 0.8; hq._scaleY = 0.1; hq._scaleZ = 0.8
        hq._type = "Corp"
        hq._gridX = 0; hq._gridZ = 0
        table.insert(Buildings, hq)

        -- Tween the HQ rising from the ground!
        Tween.to(hq, { y = 1.0, scaleY = 3.0 }, 1.5, "easeOutElastic")
        SpawnParticles(0, 0, 0, 0.0, 0.6, 1.0)

        -- Event subscription
        Events.on("building_placed", function(data)
            Say("EVENT: New " .. data.type .. " at (" .. data.x .. "," .. data.z .. ")")
        end)
    end,

    onUpdate = function(dt)
        -- Decr transition timer
        if GameTime.transitionTimer > 0 then
            GameTime.transitionTimer = GameTime.transitionTimer - dt
        end

        -- 1. Time & Income
        GameTime.tick = GameTime.tick + dt
        if GameTime.tick >= 10.0 then
            GameTime.tick = 0
            GameTime.year = GameTime.year + 1
            
            local oldEra = GameTime.era
            if GameTime.year == 2000 then GameTime.era = "2000s" end
            if GameTime.year == 2020 then GameTime.era = "2020s" end
            
            if oldEra ~= GameTime.era then
                GameTime.transitionTimer = 4.0
                GameTime.transitionMsg = "WELCOME TO THE " .. string.upper(GameTime.era) .. "!"
                audio.fm_note(220, 1.5, 7)
                audio.fm_note(440, 1.5, 7)
                for px = -6, 6, 3 do
                    for pz = -6, 6, 3 do
                        SpawnParticles(px, 1, pz, 0.0, 1.0, 0.8)
                    end
                end
            end

            local income = 0
            for _, b in ipairs(Buildings) do
                if b._type == "Corp" then income = income + 500
                elseif b._type == "Fact" then income = income + 200
                elseif b._type == "Res" then income = income + 50
                end
            end
            Player.Cash = Player.Cash + income
        end

        Market.Update(dt, GameTime.era)
        Player.UpdateAutoInvest()
        News.Update(dt)

        -- 2. Sun Era Transition (using Light wrapper!)
        if Sun then
            local r, g, b = 1.0, 0.9, 0.7
            if GameTime.era == "2000s" then r, g, b = 0.8, 0.8, 1.0 end
            if GameTime.era == "2020s" then r, g, b = 0.3, 0.0, 0.8 end
            Sun:setColor(r, g, b)
        end

        -- 3. Mouse Raycasting Cursor
        Cursor._cooldown = Cursor._cooldown - dt
        local hx, hz = Engine.get_mouse_hit(0.0)
        if hx and hz then
            Cursor._gridX = MathX.clamp(math.floor(hx + 0.5), -8, 8)
            Cursor._gridZ = MathX.clamp(math.floor(hz + 0.5), -8, 8)
        end
        local cy = math.sin(GameTime.tick * 5.0) * 0.2
        Cursor:setPos(Cursor._gridX, cy, Cursor._gridZ)

        -- 4. Build on Click
        if input.is_down("MouseLeft") and Cursor._cooldown <= 0 then
            local gx, gz = Cursor._gridX, Cursor._gridZ
            
            -- Ensure player isn't clicking on a UI panel
            local mx, my = input.get_mouse_x(), input.get_mouse_y()
            local onUI = (mx < 440 or mx > 1140 or my < 80 or my > 620)
            
            if not onUI and Player.Cash >= 1500 then
                local empty = not (Grid[gx] and Grid[gx][gz])
                if empty then
                    Player.Cash = Player.Cash - 1500
                    local randType = math.random(1, 3)
                    local t, cr, cg, cb, height = "Res", 0, 0, 0, 1.0

                    if randType == 1 then
                        t = "Res"; height = MathX.random_range(1, 2); cr,cg,cb = 0.6, 0.8, 0.2
                    elseif randType == 2 then
                        t = "Fact"; height = MathX.random_range(1.5, 2.5); cr,cg,cb = 1.0, 0.4, 0.0
                    else
                        t = "Corp"; height = MathX.random_range(3, 5); cr,cg,cb = 0.0, 0.6, 1.0
                    end

                    local b = Entity("Building", Cursor._gridX, -0.45, Cursor._gridZ)
                    b:setColor(cr, cg, cb)
                    b:setMaterial(0.5, 0.5)
                    b:setScale(0.8, 0.1, 0.8)
                    b._scaleX = 0.8; b._scaleY = 0.1; b._scaleZ = 0.8
                    b._type = t
                    b._gridX = Cursor._gridX
                    b._gridZ = Cursor._gridZ

                    -- Tween it rising!
                    Tween.to(b, { y = height/2 - 0.5, scaleY = height }, 1.0, "easeOutCubic")
                    SpawnParticles(gx, 0, gz, cr, cg, cb)
                    
                    table.insert(Buildings, b)
                    if not Grid[gx] then Grid[gx] = {} end
                    Grid[gx][gz] = b

                    -- Emit event
                    Events.emit("building_placed", { x=Cursor._gridX, z=Cursor._gridZ, type=t })
                    audio.beep3d(300 + height * 100, 0.2, Cursor._gridX, 0, Cursor._gridZ, 0) -- 3D Build Sound
                    Cursor._cooldown = 0.3
                    
                    Save.write("odyssey_cash", Player.Cash)
                    Save.flush()
                end
            end
        end

        -- 5. Tween & Coroutine updates (FRAMEWORK!)
        Tween.update(dt)
        Coroutine.update(dt)
    end,

    onRenderUI = function()
        local dt = time.get_dt()
        ui.begin(1600, 900)

        local mx, my = input.get_mouse_x(), input.get_mouse_y()
        local TooltipText = nil
        local function ShowTooltip(text)
            TooltipText = text
        end

        gfx.draw_rect(0, 0, 1600, 60, 0.05, 0.05, 0.08, 0.9)
        gfx.draw_text("CAPITAL ODYSSEY // Fusion ENGINE", 30, 20, 1.5, 0, 1, 0.8, 1)
        gfx.draw_text("YEAR: " .. GameTime.year .. " | " .. GameTime.era, 1300, 20, 1.2, 1, 0.8, 0, 1)

        -- Live exchange
        ui.panel(10, 70, 420, 760, 0.02, 0.02, 0.05, 0.85)
        ui.label(">> LIVE EXCHANGE (GBM)", 30, 110, 0.5, 0.5, 0.8, 1)

        local y = 160
        for _, c in ipairs(Market.Companies) do
            local col = (c.price >= (c.prevPrice or 0)) and {0,1,0.5} or {1,0.2,0.2}
            local shares = Player.Shares[c.name] or 0
            
            -- Detect Selection Click
            if mx >= 30 and mx <= 300 and my >= y and my <= y + 60 then
                ShowTooltip("Click to select " .. c.name .. " for detailed chart analysis.")
                if input.is_just_pressed("MouseLeft") then
                    Player.SelectedCompany = c.name
                    audio.beep(500, 0.05, 1)
                end
            end

            -- Highlight selected company
            local labelPrefix = (Player.SelectedCompany == c.name) and "> " or ""
            ui.label(labelPrefix .. c.name .. " (Own: " .. shares .. ")", 30, y, 1, 1, 1, 1)
            ui.label(string.format("$%.2f", c.price), 30, y+30, col[1], col[2], col[3], 1)
            
            if ui.button("BUY", 310, y-10, 80, 30) then 
                if Player.Buy(c.name, 10) then audio.fm_note(440, 0.1, 0) end
            end
            if ui.button("SELL", 310, y+25, 80, 30) then 
                if Player.Sell(c.name, 10) then audio.fm_note(330, 0.1, 1) end
            end

            -- Tooltips for buy/sell
            if mx >= 310 and mx <= 390 then
                if my >= y - 10 and my <= y + 20 then
                    ShowTooltip("Buy 10 shares of " .. c.name .. " at current price.")
                elseif my >= y + 25 and my <= y + 55 then
                    ShowTooltip("Sell 10 shares of " .. c.name .. " at current price.")
                end
            end

            y = y + 115
        end

        -- Financial report
        ui.panel(1150, 70, 440, 380, 0.02, 0.02, 0.05, 0.85)
        ui.label("FINANCIAL REPORT", 1170, 110, 1, 0.8, 0, 1)
        ui.label("CASH:    $" .. string.format("%.0f", Player.Cash), 1170, 160, 1,1,1,1)
        ui.label("NET:     $" .. string.format("%.0f", Player.GetNetWorth()), 1170, 210, 0,1,0,1)
        ui.label("BUILDINGS: " .. #Buildings, 1170, 260, 0.5, 0.8, 1, 1)
        
        -- Auto-Invest Button
        local aiText = Player.AutoInvestEnabled and "AUTO-INVEST: ON" or "AUTO-INVEST: OFF"
        local aiColor = Player.AutoInvestEnabled and {0.2, 1.0, 0.5} or {0.6, 0.6, 0.6}
        if ui.button(aiText, 1170, 310, 220, 35) then
            Player.AutoInvestEnabled = not Player.AutoInvestEnabled
            audio.beep(600, 0.05, 2)
            Save.write("odyssey_autoinvest", Player.AutoInvestEnabled and 1 or 0)
            Save.flush()
        end

        if mx >= 1170 and mx <= 1390 and my >= 310 and my <= 345 then
            ShowTooltip("Auto-Invest: buys stock dips below 10-period moving average (retaining $3500 cash buffer).")
        end

        gfx.draw_text("Hover Mouse | Build: LEFT CLICK ($1500)", 1170, 370, 0.8, 0, 1, 0.5, 1)

        -- Raycast/City area hover build instruction
        if mx >= 440 and mx <= 1140 and my >= 70 and my <= 470 then
            ShowTooltip("Left Click on any empty tile to spend $1500 and build a corporate/fact/res block.")
        end

        ui.finish()

        -- Mini stock graphs on Live Exchange
        local gy = 150
        for _, c in ipairs(Market.Companies) do
            if c.history and #c.history >= 2 then
                local maxVal = 0.01
                for i=1,#c.history do if c.history[i] > maxVal then maxVal = c.history[i] end end
                gfx.draw_quad(180, gy, 120, 40, 0, 0, 0, 0.5)
                for i = 1, #c.history do
                    local px = 180 + (i-1) * (120/50)
                    local py = gy + 40 - (c.history[i]/maxVal) * 40
                    gfx.draw_quad(px, py, 2, 2, 0, 1, 0.5, 1)
                end
            end
            gy = gy + 115
        end

        -- ============================================================================
        -- MAIN DETAILED CHART PANEL (Center Bottom)
        -- ============================================================================
        local cx, cy, cw, ch = 450, 480, 680, 340
        ui.panel(cx, cy, cw, ch, 0.02, 0.02, 0.05, 0.85)
        
        local selectedComp = Market.GetCompany(Player.SelectedCompany or "Starlight Tech")
        if selectedComp then
            local cr, cg, cb = 0, 1, 0.8
            if selectedComp.price < (selectedComp.prevPrice or 0) then cr, cg, cb = 1, 0.2, 0.2 end
            
            ui.label("DETAILED EXCHANGE HISTORY: " .. string.upper(selectedComp.name), cx + 25, cy + 20, cr, cg, cb, 1)
            
            local history = selectedComp.history
            if history and #history >= 2 then
                local minVal = 999999
                local maxVal = -999999
                for i = 1, #history do
                    if history[i] < minVal then minVal = history[i] end
                    if history[i] > maxVal then maxVal = history[i] end
                end
                if maxVal == minVal then maxVal = minVal + 1 end
                
                -- Chart area coordinates
                local gx, gy, gw, gh = cx + 80, cy + 70, cw - 120, ch - 110
                
                -- Chart inner grid background
                gfx.draw_rect(gx, gy, gw, gh, 0.01, 0.01, 0.03, 0.6)
                gfx.draw_rect_outline(gx, gy, gw, gh, 1, 0.2, 0.3, 0.4, 0.5)
                
                -- Min/Max Labels
                gfx.draw_text(string.format("$%.2f", maxVal), cx + 12, gy, 0.85, 0.2, 1.0, 0.5, 0.85)
                gfx.draw_text(string.format("$%.2f", minVal), cx + 12, gy + gh - 10, 0.85, 1.0, 0.2, 0.2, 0.85)
                gfx.draw_text(string.format("Price: $%.2f", selectedComp.price), cx + 20, gy + gh/2 - 5, 0.85, 1.0, 1.0, 1.0, 0.9)
                
                -- Horizontal grid lines
                for step = 1, 3 do
                    local hlineY = gy + (gh / 4) * step
                    gfx.draw_rect(gx, hlineY, gw, 1, 0.15, 0.2, 0.25, 0.25)
                end
                
                -- Plot history points
                local points = {}
                for i = 1, #history do
                    local px = gx + (i - 1) * (gw / 50)
                    local py = gy + gh - ((history[i] - minVal) / (maxVal - minVal)) * gh
                    table.insert(points, {x = px, y = py})
                end
                
                -- Draw segments with neon glow
                local col = (selectedComp.price >= (selectedComp.prevPrice or 0)) and {0.0, 1.0, 0.5} or {1.0, 0.2, 0.2}
                for i = 1, #points - 1 do
                    local p1 = points[i]
                    local p2 = points[i+1]
                    
                    local dx = p2.x - p1.x
                    local dy = p2.y - p1.y
                    local steps = math.max(math.floor(math.sqrt(dx*dx + dy*dy)), 1)
                    for s = 0, steps do
                        local t = s / steps
                        local lx = p1.x + dx * t
                        local ly = p1.y + dy * t
                        gfx.draw_quad(lx - 1, ly - 1, 2, 2, col[1], col[2], col[3], 1.0)
                        gfx.draw_quad(lx - 2, ly - 2, 4, 4, col[1], col[2], col[3], 0.2)
                    end
                end
            end
        end

        -- ============================================================================
        -- NEWS TICKER DISPLAY (Functional Alert System)
        -- ============================================================================
        gfx.draw_rect(0, 840, 1600, 60, 0.03, 0.03, 0.05, 0.95)
        gfx.draw_rect_outline(0, 840, 1600, 60, 2, 0.0, 0.8, 1.0, 0.8)
        gfx.draw_text("NEWS TICKER //", 20, 858, 1.1, 0.0, 0.8, 1.0, 1.0)
        
        local headline = "MARKETS STABLE. CONSTRUCT COMMERCIAL DISTRICTS TO MAXIMIZE QUARTERLY REVENUE."
        if #News.ActiveEvents > 0 then
            local ev = News.ActiveEvents[1]
            headline = "ALERT: " .. string.upper(ev.title) .. " IN PROGRESS! DRIFT AFFECTING " .. string.upper(ev.sector) .. " SECTOR."
        end
        
        local pulse = math.sin(time.get_time() * 6) * 0.2 + 0.8
        local hr, hg, hb = 0.8 * pulse, 0.8 * pulse, 0.8 * pulse
        if #News.ActiveEvents > 0 then
            hr, hg, hb = 1.0 * pulse, 0.1, 0.1
        end
        gfx.draw_text(headline, 180, 858, 1.0, hr, hg, hb, 1.0)

        -- ============================================================================
        -- ERA TRANSITION FULLSCREEN EFFECTS
        -- ============================================================================
        if GameTime.transitionTimer > 0 then
            local alpha = math.min(1.0, GameTime.transitionTimer)
            gfx.draw_rect(0, 0, 1600, 900, 0, 0, 0.05, alpha * 0.85)
            
            local tr, tg, tb = Color.hsv((time.get_time() * 0.5) % 1.0, 0.8, 1.0)
            gfx.draw_rect_outline(200, 300, 1200, 300, 6, tr, tg, tb, alpha)
            gfx.draw_rect(203, 303, 1194, 294, 0.02, 0.02, 0.04, alpha * 0.95)
            
            gfx.draw_text("ERA TRANSITION DETECTED", 460, 370, 2.4, tr, tg, tb, alpha)
            gfx.draw_text(GameTime.transitionMsg, 500, 470, 2.0, 1.0, 1.0, 1.0, alpha)
        end

        -- ============================================================================
        -- FLOATING TOOLTIP RENDERER
        -- ============================================================================
        if TooltipText then
            local tx = mx + 15
            local ty = my + 15
            local tw = string.len(TooltipText) * 7.5 + 20
            local th = 30
            
            if tx + tw > 1600 then tx = tx - tw - 25 end
            if ty + th > 900 then ty = ty - th - 25 end
            
            gfx.draw_rect(tx, ty, tw, th, 0.04, 0.05, 0.07, 0.95)
            gfx.draw_rect_outline(tx, ty, tw, th, 1.5, 0.2, 0.8, 1.0, 0.8)
            gfx.draw_text(TooltipText, tx + 10, ty + 9, 0.85, 1.0, 1.0, 1.0, 1.0)
        end

        if ShowTelemetry and engine and engine.get_telemetry then
            local telem = engine.get_telemetry()
            local boxW, boxH = 430, 95
            local bx, by = 1600 - boxW - 20, 20
            gfx.draw_rect(bx, by, boxW, boxH, 0.04, 0.07, 0.1, 0.92)
            gfx.draw_rect_outline(bx, by, boxW, boxH, 1.5, 0.0, 0.9, 1.0, 0.85)
            gfx.draw_text("STARLIGHT LIVE TELEMETRY (F3)", bx + 12, by + 10, 0.95, 0.0, 1.0, 0.9, 1.0)
            gfx.draw_text(string.format("FPS: %d (%.2f ms) | Target: %d | Entities: %d", telem.fps or 60, (telem.avg_frame_time or 0.016)*1000.0, telem.target_fps or 60, telem.entities_count or 0), bx + 12, by + 30, 0.82, 0.85, 0.9, 0.95, 1.0)
            gfx.draw_text(string.format("Draw Calls: %d | Quads: %d | Audio Voices: %d", telem.draw_calls or 0, telem.quad_count or 0, telem.active_audio_voices or 0), bx + 12, by + 50, 0.82, 0.85, 0.9, 0.95, 1.0)
            gfx.draw_text(string.format("Memory Alloc: %.1f / %.1f KB", telem.memory_used_kb or 0, telem.memory_capacity_kb or 0), bx + 12, by + 70, 0.82, 0.3, 1.0, 0.5, 1.0)
        end
    end,
})

local ShowTelemetry = false

-- ============================================================================
-- Entry Points (Called by C++ Engine)
-- ============================================================================
function OnStart()
    -- Testando a nova API de Noise integrada via FastNoiseLite
    local testNoise = Noise.new(1337)
    testNoise:set_frequency(0.05)
    local simplexValue = testNoise:simplex(10.5, 20.2)
    local cellularValue = testNoise:cellular(5.0, 5.0, 5.0)
    Engine.log(string.format("FastNoiseLite Test -> Simplex 2D: %.4f, Cellular 3D: %.4f", simplexValue, cellularValue))

    Scene.switch("Game")
end

function OnUpdate(dt)
    if input.is_just_pressed("F3") then
        ShowTelemetry = not ShowTelemetry
        if audio and audio.play_synth then audio.play_synth(880, 0.05, "sine") end
    end
    if input.is_just_pressed("F12") then
        local snapName = string.format("screenshots/capital_odyssey_%d.ppm", math.floor(time.get_time()))
        if engine and engine.capture_screenshot then
            engine.capture_screenshot(snapName)
            if audio and audio.play_synth then audio.play_synth(1200, 0.08, "triangle") end
        end
    end
    Scene.update(dt)
end

function OnRenderUI()
    Scene.renderUI()
end
