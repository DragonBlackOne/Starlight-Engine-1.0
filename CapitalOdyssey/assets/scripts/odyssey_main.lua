-- odyssey_main.lua — Capital Odyssey v10.0 (SBA v4.0 Industrial)
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
GameTime = { year = 1990, tick = 0, era = "1990s" }
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
        Player.Cash = Save.read("odyssey_cash", 10000.0)
        Player.Init()
        News.Init()

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
        -- 1. Time & Income
        GameTime.tick = GameTime.tick + dt
        if GameTime.tick >= 10.0 then
            GameTime.tick = 0
            GameTime.year = GameTime.year + 1
            if GameTime.year == 2000 then GameTime.era = "2000s" end
            if GameTime.year == 2020 then GameTime.era = "2020s" end

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
            if Player.Cash >= 1500 then
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
        ui.begin(1600, 900)

        gfx.draw_rect(0, 0, 1600, 60, 0.05, 0.05, 0.08, 0.9)
        gfx.draw_text("CAPITAL ODYSSEY // SBA v3.0 POWERED", 30, 20, 1.5, 0, 1, 0.8, 1)
        gfx.draw_text("YEAR: " .. GameTime.year .. " | " .. GameTime.era, 1300, 20, 1.2, 1, 0.8, 0, 1)

        ui.panel(10, 70, 420, 820, 0.02, 0.02, 0.05, 0.85)
        ui.label(">> LIVE EXCHANGE (GBM)", 30, 110, 0.5, 0.5, 0.8, 1)

        local y = 160
        for _, c in ipairs(Market.Companies) do
            local col = (c.price >= (c.prevPrice or 0)) and {0,1,0.5} or {1,0.2,0.2}
            local shares = Player.Shares[c.name] or 0
            ui.label(c.name .. " (Own: " .. shares .. ")", 30, y, 1, 1, 1, 1)
            ui.label(string.format("$%.2f", c.price), 30, y+30, col[1], col[2], col[3], 1)
            if ui.button("BUY", 310, y-10, 80, 30) then 
                if Player.Buy(c.name, 10) then audio.fm_note(440, 0.1, 0) end
            end
            if ui.button("SELL", 310, y+25, 80, 30) then 
                if Player.Sell(c.name, 10) then audio.fm_note(330, 0.1, 1) end
            end
            y = y + 115
        end

        ui.panel(1150, 70, 440, 300, 0.02, 0.02, 0.05, 0.85)
        ui.label("FINANCIAL REPORT", 1170, 110, 1, 0.8, 0, 1)
        ui.label("CASH:    $" .. string.format("%.0f", Player.Cash), 1170, 160, 1,1,1,1)
        ui.label("NET:     $" .. string.format("%.0f", Player.GetNetWorth()), 1170, 210, 0,1,0,1)
        ui.label("BUILDINGS: " .. #Buildings, 1170, 260, 0.5, 0.8, 1, 1)
        
        gfx.draw_text("Hover Mouse | Build: LEFT CLICK ($1500)", 1170, 310, 0.8, 0, 1, 0.5, 1)

        ui.finish()

        -- Mini stock graphs
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
    end,
})

-- ============================================================================
-- Entry Points (Called by C++ Engine)
-- ============================================================================
function OnStart()
    Scene.switch("Game")
end

function OnUpdate(dt)
    Scene.update(dt)
end

function OnRenderUI()
    Scene.renderUI()
end
