-- ============================================================================
-- Portfolio.lua — Player Financial System (SBA v2.0)
-- ============================================================================
Player = {}
Player.Cash = 10000.0
Player.Shares = {}
Player.TradeHistory = {}

function Player.Init()
    Player.Cash = 10000.0
    Player.Shares = {}
    Player.TradeHistory = {}
    Engine.log("Portfolio: Initialized with $" .. Player.Cash)
end

function Player.Buy(companyName, quantity)
    if quantity <= 0 then return false end
    local comp = Market.GetCompany(companyName)
    if not comp then
        Engine.log("Portfolio: Company not found: " .. companyName)
        return false
    end
    
    local totalCost = comp.price * quantity
    if Player.Cash >= totalCost then
        Player.Cash = Player.Cash - totalCost
        Player.Shares[companyName] = (Player.Shares[companyName] or 0) + quantity
        table.insert(Player.TradeHistory, {
            action = "BUY", company = companyName,
            quantity = quantity, price = comp.price,
            total = totalCost
        })
        Engine.log("Portfolio: Bought " .. quantity .. " shares of " .. companyName)
        Events.emit("trade_executed", { action="BUY", company=companyName, qty=quantity })
        return true
    end
    Engine.log("Portfolio: Insufficient funds for " .. companyName)
    return false
end

function Player.Sell(companyName, quantity)
    if quantity <= 0 then return false end
    local owned = Player.Shares[companyName] or 0
    if owned >= quantity then
        local comp = Market.GetCompany(companyName)
        if not comp then return false end
        
        local totalGain = comp.price * quantity
        Player.Cash = Player.Cash + totalGain
        Player.Shares[companyName] = owned - quantity
        if Player.Shares[companyName] == 0 then Player.Shares[companyName] = nil end
        table.insert(Player.TradeHistory, {
            action = "SELL", company = companyName,
            quantity = quantity, price = comp.price,
            total = totalGain
        })
        Engine.log("Portfolio: Sold " .. quantity .. " shares of " .. companyName)
        Events.emit("trade_executed", { action="SELL", company=companyName, qty=quantity })
        return true
    end
    Engine.log("Portfolio: Not enough shares of " .. companyName)
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

function Player.GetShareCount(companyName)
    return Player.Shares[companyName] or 0
end

function Player.GetPortfolioValue()
    local value = 0
    for name, qty in pairs(Player.Shares) do
        local comp = Market.GetCompany(name)
        if comp then value = value + (comp.price * qty) end
    end
    return value
end
