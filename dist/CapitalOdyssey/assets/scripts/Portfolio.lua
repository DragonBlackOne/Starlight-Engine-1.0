-- ============================================================================
-- Portfolio.lua — Player Financial System (SBA v2.0)
-- ============================================================================
Player = {}
Player.Cash = 10000.0
Player.Shares = {}
Player.TradeHistory = {}
Player.AutoInvestEnabled = false
Player.SelectedCompany = "Starlight Tech"

function Player.Init()
    Player.Cash = 10000.0
    Player.Shares = {}
    Player.TradeHistory = {}
    Player.AutoInvestEnabled = (Save.read("odyssey_autoinvest", 0) == 1)
    Player.SelectedCompany = "Starlight Tech"
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

function Player.UpdateAutoInvest()
    if not Player.AutoInvestEnabled then return end
    if Player.Cash < 3500 then return end -- Keep buffer
    
    local bestComp = nil
    local bestRatio = 1.0
    
    for _, c in ipairs(Market.Companies) do
        if c.history and #c.history >= 10 then
            local sum = 0
            local count = 10
            for i = #c.history, #c.history - 9, -1 do
                sum = sum + c.history[i]
            end
            local ma = sum / count
            local ratio = c.price / ma
            if ratio < 0.95 and ratio < bestRatio then -- at least 5% dip below MA
                bestRatio = ratio
                bestComp = c
            end
        end
    end
    
    if bestComp then
        Player.Buy(bestComp.name, 10)
    end
end
