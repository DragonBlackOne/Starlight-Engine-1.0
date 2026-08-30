-- fusion_fight_test.lua
-- Testes unitarios e integridade de tela para o FusionFight

test.describe("FusionFight - Inicializacao de Game Manager e Telas", function()
    -- 1. Mocks de suporte
    fight = fight or {
        stop_replay = function() end
    }
    Engine.set_graphics_preset = Engine.set_graphics_preset or function() end

    -- 2. Ajustar package.path para apontar para a pasta do FusionFight
    package.path = package.path .. ";../FusionFight/?.lua;../FusionFight/assets/scripts/?.lua;../FusionFight/assets/scripts/screens/?.lua"
    
    -- 3. Carrega o script principal do FusionFight
    local path = "../FusionFight/assets/scripts/main.lua"
    local loaded, err = loadfile(path)
    test.assert(loaded ~= nil, "Deveria carregar o script principal do FusionFight sem erros. Erro: " .. tostring(err))
    
    if loaded then
        loaded()
        
        -- 4. Executar OnStart
        local okStart, errStart = pcall(OnStart)
        test.assert(okStart, "OnStart do FusionFight deveria rodar sem crashes. Erro: " .. tostring(errStart))
        
        -- 5. Testar se inicializou na tela TITLE
        test.assertEqual(App.currentScreenName, "TITLE", "Deveria iniciar o jogo na tela TITLE")
        test.assert(App.currentScreen ~= nil, "A tela atual (TITLE) deveria estar instanciada")
        
        -- 6. Testar troca de tela
        local okSwitch, errSwitch = pcall(function()
            App:SwitchTo("MENU")
        end)
        test.assert(okSwitch, "Deveria alternar para a tela MENU sem erros. Erro: " .. tostring(errSwitch))
        test.assertEqual(App.currentScreenName, "MENU", "Estado da tela atual deveria ser MENU")
    end
end)
