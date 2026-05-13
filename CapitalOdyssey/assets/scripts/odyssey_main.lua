-- odyssey_main.lua
package.path = package.path .. ";assets/scripts/?.lua"

require("sba_bridge")
require("MarketSystem")
require("Portfolio")
require("NewsGenerator")

GameTime = { year = 1990, tick = 0, era = "1990s" }
Textures = {}
Entities = {}

function OnStart()
    Say("Capital Odyssey v6.5-PREMIUM")
    Market.Init()
    Player.Init()
    News.Init()
    
    Textures.TileGrass = assets.load_texture("assets/textures/sprites/iso_tile_grass.png")
    Textures.TileTech  = assets.load_texture("assets/textures/sprites/iso_tile_tech.png")
    
    Textures.Corp = assets.load_texture("assets/textures/sprites/iso_building_corp.png")
    Textures.Res  = assets.load_texture("assets/textures/sprites/iso_building_res.png")
    Textures.Fact = assets.load_texture("assets/textures/sprites/iso_building_fact.png")
    
    table.insert(Entities, { x=0, z=0, tex=Textures.Corp, type="HQ" })
end

function OnUpdate(dt)
    GameTime.tick = GameTime.tick + dt
    if GameTime.tick >= 10.0 then
        GameTime.tick = 0
        GameTime.year = GameTime.year + 1
        
        -- Grow city (limited)
        if #Entities < 30 and Player.GetNetWorth() > #Entities * 5000 then
            local rx = math.random(-6, 6)
            local rz = math.random(-6, 6)
            local randType = math.random(1, 3)
            local texType = Textures.Res
            local t = "Res"
            if randType == 1 then texType = Textures.Res; t = "Res"
            elseif randType == 2 then texType = Textures.Fact; t = "Fact"
            else texType = Textures.Corp; t = "Corp" end

            table.insert(Entities, { x=rx, z=rz, tex=texType, type=t })
        end

        if GameTime.year == 2000 then GameTime.era = "2000s" end
        if GameTime.year == 2020 then GameTime.era = "2020s" end
    end

    Market.Update(dt, GameTime.era)
    News.Update(dt)
end

function DrawGraph(x, y, w, h, history)
    if not history or #history < 2 then return end
    local maxVal = 0.01
    for i=1, #history do if history[i] and history[i] > maxVal then maxVal = history[i] end end
    
    for i = 1, #history do
        local val = history[i] or 0
        local px = x + (i-1) * (w / 20)
        local py = y + h - (val / maxVal) * h
        gfx.draw_quad(px, py, 3, 3, 0, 1, 0.8, 0.8)
    end
end

function OnRenderUI()
    -- 1. Isometric World
    local floor = (GameTime.era == "2020s") and Textures.TileTech or Textures.TileGrass
    for i = -8, 8 do
        for j = -8, 8 do
            local sx = 800 + (i-j)*64
            local sy = 400 + (i+j)*32
            gfx.draw_sprite_clean(sx-64, sy-64, 128, 128, floor, 1,1,1,1)
        end
    end

    -- 1.5 Z-Sort the Entities!
    table.sort(Entities, function(a, b) 
        return (a.x + a.z) < (b.x + b.z)
    end)

    for _, ent in ipairs(Entities) do
        local sx = 800 + (ent.x-ent.z)*64
        local sy = 400 + (ent.x+ent.z)*32
        local size = 200
        gfx.draw_sprite_clean(sx - size/2, sy - size + 40, size, size, ent.tex, 1,1,1,1)
    end

    -- 2. Terminal UI (Glassmorphism & Clean Layout)
    ui.begin(1600, 900)
    
    ui.panel(0, 0, 1600, 60, 0.05, 0.05, 0.1, 0.85)
    ui.label("CAPITAL ODYSSEY // v6.5-PREMIUM", 30, 45, 0, 1, 0.8, 1)
    ui.label("YEAR: " .. GameTime.year .. " | " .. GameTime.era, 1300, 45, 1, 0.8, 0, 1)

    ui.panel(10, 70, 420, 820, 0.05, 0.05, 0.1, 0.75)
    ui.label(">> LIVE EXCHANGE", 30, 110, 0.5, 0.5, 0.8, 1)
    
    local y = 160
    for _, c in ipairs(Market.Companies) do
        local col = (c.price >= (c.prevPrice or 0)) and {0,1,0.5} or {1,0,0.2}
        ui.label(c.name, 30, y, 1, 1, 1, 1)
        ui.label(string.format("$%.2f", c.price), 30, y+30, col[1], col[2], col[3], 1)
        
        if ui.button("BUY", 310, y-10, 80, 35) then Player.Buy(c.name, 10) end
        if ui.button("SELL", 310, y+35, 80, 35) then Player.Sell(c.name, 10) end
        y = y + 115
    end
    
    ui.panel(1150, 70, 440, 250, 0.05, 0.05, 0.1, 0.75)
    ui.label("FINANCIAL REPORT", 1170, 110, 1, 0.8, 0, 1)
    ui.label("CASH:    $" .. string.format("%.0f", Player.Cash), 1170, 160, 1,1,1,1)
    ui.label("NET:     $" .. string.format("%.0f", Player.GetNetWorth()), 1170, 210, 0,1,0,1)

    ui.finish()

    -- 3. Overlay Graphs
    local gy = 160
    for _, c in ipairs(Market.Companies) do
        DrawGraph(180, gy + 10, 100, 30, c.history)
        gy = gy + 115
    end
end
