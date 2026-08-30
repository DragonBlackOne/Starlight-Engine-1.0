-- snake_test.lua
-- Testes unitarios para a logica do jogo Snake

test.describe("Snake - Logica de Grid e Movimento", function()
    -- 1. Carrega o script principal do Snake
    local path = "../Snake_Project/assets/scripts/snake_main.lua"
    local loaded, err = load_game_script(path, {
        ["local Snake = {"] = "Snake = {",
        ["local Leaderboard ="] = "Leaderboard ="
    })
    test.assert(loaded ~= nil, "Deveria carregar o script principal do Snake. Erro: " .. tostring(err))
    
    if loaded then
        -- Carrega no ambiente global
        loaded()
        
        -- 2. Testar Reset da Cobra
        ResetSnake()
        test.assert(#Snake.body > 0, "A cobra deveria possuir segmentos no corpo apos ResetSnake")
        test.assertEqual(Snake.score, 0, "Placar inicial do Snake deveria ser 0")
        test.assert(Snake.alive, "A cobra deveria estar viva apos ResetSnake")
        
        -- 3. Testar reposicionamento de comida
        PlaceFood()
        test.assert(Snake.food.x >= 0, "Coordenada X da comida deveria ser positiva")
        test.assert(Snake.food.y >= 0, "Coordenada Y da comida deveria ser positiva")
        
        -- 4. Testar direcao inicial
        test.assertEqual(Snake.dir.x, 1, "Direcao X inicial deveria ser 1 (direita)")
        test.assertEqual(Snake.dir.y, 0, "Direcao Y inicial deveria ser 0")
        
        -- 5. Testar Leaderboard
        LoadLeaderboard()
        test.assert(Leaderboard ~= nil, "A tabela de Leaderboard deveria ser inicializada")
    end
end)
