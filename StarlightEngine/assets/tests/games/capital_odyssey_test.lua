-- capital_odyssey_test.lua
-- Testes unitarios para a simulacao de mercado de CapitalOdyssey

test.describe("CapitalOdyssey - Simulador Financeiro", function()
    -- 1. Mocks de suporte
    Events = Events or {
        emit = function() end,
        on = function() end
    }
    
    -- 2. Carregar dependencias de CapitalOdyssey
    -- Como eles fazem require() relativos, podemos carregar diretamente
    local marketPath = "../CapitalOdyssey/assets/scripts/MarketSystem.lua"
    local portfolioPath = "../CapitalOdyssey/assets/scripts/Portfolio.lua"
    
    local loadMarket = loadfile(marketPath)
    local loadPortfolio = loadfile(portfolioPath)
    
    test.assert(loadMarket ~= nil, "Deveria carregar MarketSystem.lua")
    test.assert(loadPortfolio ~= nil, "Deveria carregar Portfolio.lua")
    
    if loadMarket and loadPortfolio then
        loadMarket()
        loadPortfolio()
        
        -- 3. Testar inicializacao do mercado e player
        Market.Init()
        Player.Init()
        
        test.assertEqual(Player.Cash, 10000.0, "Saldo inicial deveria ser $10000.0")
        test.assertEqual(Player.GetShareCount("Starlight Tech"), 0, "Deveria iniciar com 0 acoes")
        
        local comp = Market.GetCompany("Starlight Tech")
        test.assert(comp ~= nil, "Deveria encontrar a empresa Starlight Tech")
        test.assertEqual(comp.price, 150.0, "Preco inicial do Starlight Tech deveria ser $150.0")
        
        -- 4. Testar compra de acoes
        local buySuccess = Player.Buy("Starlight Tech", 10)
        test.assert(buySuccess, "Deveria comprar 10 acoes com sucesso")
        test.assertEqual(Player.GetShareCount("Starlight Tech"), 10, "Deveria possuir 10 acoes agora")
        test.assertEqual(Player.Cash, 10000.0 - (150.0 * 10), "Saldo deveria diminuir pelo custo das acoes")
        
        -- Net worth
        local netWorth = Player.GetNetWorth()
        test.assertEqual(netWorth, 10000.0, "Net worth deveria continuar $10000.0 (dinheiro + acoes)")
        
        -- 5. Testar venda de acoes
        local sellSuccess = Player.Sell("Starlight Tech", 5)
        test.assert(sellSuccess, "Deveria vender 5 acoes com sucesso")
        test.assertEqual(Player.GetShareCount("Starlight Tech"), 5, "Deveria possuir 5 acoes apos venda")
        test.assertEqual(Player.Cash, 10000.0 - (150.0 * 5), "Saldo deveria aumentar pelo valor da venda")
        
        -- 6. Testar variacao de mercado (geometric brownian motion)
        local priceBefore = comp.price
        Market.Update(0.1, "1990s")
        test.assert(comp.price ~= priceBefore, "O preco da empresa deveria flutuar apos o update de mercado")
    end
end)
