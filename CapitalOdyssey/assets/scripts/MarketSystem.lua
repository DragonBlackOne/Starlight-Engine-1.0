-- MarketSystem.lua
Market = {}

function Market.Init()
    Market.Companies = {
        { name = "Starlight Tech", price = 150.0, sector = "Tech", history = {150.0}, volatility = 0.05 },
        { name = "Fusion Energy", price = 45.0, sector = "Energy", history = {45.0}, volatility = 0.03 },
        { name = "Omega Realty", price = 85.0, sector = "Real Estate", history = {85.0}, volatility = 0.02 },
        { name = "Cyber Pharma", price = 120.0, sector = "Health", history = {120.0}, volatility = 0.04 },
        { name = "Neo Consumer", price = 30.0, sector = "Retail", history = {30.0}, volatility = 0.02 }
    }
end

function Market.Update(dt, era)
    for _, comp in ipairs(Market.Companies) do
        comp.prevPrice = comp.price
        
        -- Base movement
        local change = (math.random() - 0.48) * comp.volatility * comp.price
        
        -- Era-based modifiers
        if era == "1990s" and comp.sector == "Tech" then change = change + 0.1 end
        if era == "2020s" and comp.sector == "Tech" then change = change + 0.5 end
        
        comp.price = comp.price + change
        if comp.price < 0.01 then comp.price = 0.01 end
        
        -- Update history (limit to 20 points)
        table.insert(comp.history, comp.price)
        if #comp.history > 20 then table.remove(comp.history, 1) end
    end
end
