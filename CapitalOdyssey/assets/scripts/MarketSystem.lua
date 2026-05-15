-- MarketSystem.lua
Market = {}

function Market.Init()
    Market.Companies = {
        { name = "Starlight Tech", price = 150.0, sector = "Tech", history = {150.0}, volatility = 0.05, drift = 0.02 },
        { name = "Fusion Energy", price = 45.0, sector = "Energy", history = {45.0}, volatility = 0.03, drift = 0.02 },
        { name = "Omega Realty", price = 85.0, sector = "Real Estate", history = {85.0}, volatility = 0.02, drift = 0.02 },
        { name = "Cyber Pharma", price = 120.0, sector = "Health", history = {120.0}, volatility = 0.04, drift = 0.02 },
        { name = "Neo Consumer", price = 30.0, sector = "Retail", history = {30.0}, volatility = 0.02, drift = 0.02 }
    }
end

-- Box-Muller transform for Standard Normal Distribution
local function randomNormal()
    local u1 = math.random()
    if u1 == 0.0 then u1 = 0.0001 end
    local u2 = math.random()
    return math.sqrt(-2 * math.log(u1)) * math.cos(2 * math.pi * u2)
end

function Market.Update(dt, era)
    for _, comp in ipairs(Market.Companies) do
        comp.prevPrice = comp.price
        
        -- Geometric Brownian Motion (GBM)
        -- comp.drift is modified dynamically by NewsGenerator events
        local drift = comp.drift or 0.02
        if era == "1990s" and comp.sector == "Tech" then drift = drift + 0.13 end
        if era == "2020s" and comp.sector == "Tech" then drift = drift + 0.28 end
        
        local dt_scale = dt * 0.1 -- Scale time for game tick
        local diffusion = comp.volatility * math.sqrt(dt_scale) * randomNormal()
        local percentChange = (drift - 0.5 * comp.volatility * comp.volatility) * dt_scale + diffusion
        
        comp.price = comp.price * math.exp(percentChange)
        if comp.price < 0.01 then comp.price = 0.01 end
        
        -- Update history (limit to 50 points for better graphing)
        table.insert(comp.history, comp.price)
        if #comp.history > 50 then table.remove(comp.history, 1) end
    end
end

function Market.GetCompany(name)
    for _, comp in ipairs(Market.Companies) do
        if comp.name == name then return comp end
    end
    return nil
end
