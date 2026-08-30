-- tetris_test.lua
-- Testes de integridade e QA para o Tetris

test.describe("Tetris - Inicializacao e Sanidade de Kernel", function()
    -- 1. Mocks especificos do Tetris
    camera = camera or {
        set_pos = function() end,
        look_at = function() end
    }
    time = time or {
        get_time = function() return 1.0 end
    }
    Engine.set_bloom = Engine.set_bloom or function() end

    -- 2. Carrega o script principal do Tetris
    local path = "../Tetris_Project/assets/scripts/tetris_main.lua"
    local loaded, err = load_game_script(path, {
        ["local Game = {"] = "Game = {"
    })
    test.assert(loaded ~= nil, "Deveria carregar o script do Tetris sem erros de sintaxe. Erro: " .. tostring(err))
    
    if loaded then
        loaded()
        
        -- 3. Executar o ciclo de inicializacao global
        local okStart, errStart = pcall(OnStart)
        test.assert(okStart, "OnStart do Tetris deveria rodar sem erros de runtime. Erro: " .. tostring(errStart))
        
        -- 4. Tentar capturar a tabela Game usando upvalues para testes detalhados
        local GameRef = nil
        if OnStart and debug and debug.getupvalue then
            local idx = 1
            while true do
                local name, val = debug.getupvalue(OnStart, idx)
                if not name then break end
                if name == "Game" then
                    GameRef = val
                    break
                end
                idx = idx + 1
            end
        end
        
        GameRef = GameRef or Game
        if GameRef then
            test.assertEqual(GameRef.score, 0, "Placar inicial do Tetris deveria ser 0")
            test.assertEqual(GameRef.level, 1, "Level inicial do Tetris deveria ser 1")
            test.assert(GameRef.active ~= nil, "Deveria spawnar uma peca ativa inicial")
            
            -- Simular queda da peca (movimento Y)
            local oldRow = GameRef.active.r
            -- Chamando um update curto
            OnUpdate(0.016)
            -- Como o tickRate e 0.8s, uma chamada de 0.016s nao faria ela cair imediatamente.
            -- Mas podemos forcar a queda chamando OnUpdate com dt maior ou chamando GameRef:Tick()
            if GameRef.Tick then
                GameRef:Tick()
                test.assert(GameRef.active.r > oldRow or GameRef.state == "OVER", "A peca ativa deveria descer ou dar Game Over")
            end
        else
            -- Fallback: Apenas roda OnUpdate para checar se nao ha crashes internos
            local okUpdate, errUpdate = pcall(OnUpdate, 0.016)
            test.assert(okUpdate, "OnUpdate do Tetris deveria rodar sem crashes. Erro: " .. tostring(errUpdate))
        end
    end
end)
