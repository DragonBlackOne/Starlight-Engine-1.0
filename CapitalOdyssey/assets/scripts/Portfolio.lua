-- Portfolio.lua
Player = {}
Player.Cash = 10000.0 -- Starting capital
Player.Shares = {}     -- companyName -> quantity

function Player.Init()
    Engine.log("Portfolio: Initialized with $" .. Player.Cash)
end

function Player.Buy(companyName, quantity)
    local comp = Market.GetCompany(companyName)
    if not comp then return false end
    
    local totalCost = comp.price * quantity
    if Player.Cash >= totalCost then
        Player.Cash = Player.Cash - totalCost
        Player.Shares[companyName] = (Player.Shares[companyName] or 0) + quantity
        Engine.log("Portfolio: Bought " .. quantity .. " shares of " .. companyName)
        return true
    end
    return false
end

function Player.Sell(companyName, quantity)
    local owned = Player.Shares[companyName] or 0
    if owned >= quantity then
        local comp = Market.GetCompany(companyName)
        if not comp then return false end
        
        local totalGain = comp.price * quantity
        Player.Cash = Player.Cash + totalGain
        Player.Shares[companyName] = owned - quantity
        Engine.log("Portfolio: Sold " .. quantity .. " shares of " .. companyName)
        return true
    end
    return false
end

function Player.GetNetWorth()
    local worth = Player.Cash
    for name, qty in pairs(Player.Shares) do
        local comp = Market.GetCompany(name)
        if comp then
            worth = worth + (comp.price * qty)
        end
    end
    return worth
end
