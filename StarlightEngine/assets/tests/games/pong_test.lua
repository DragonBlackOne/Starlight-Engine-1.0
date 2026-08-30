-- pong_test.lua
-- Testes unitarios para a logica e classes do Pong

test.describe("Pong - Logica de Jogo e Entidades", function()
    -- 1. Carrega o arquivo principal do jogo
    local path = "../Pong_Project/assets/scripts/pong_main.lua"
    local loaded, err = load_game_script(path, {
        ["local Paddle = Class%(%)"] = "Paddle = Class()",
        ["local Ball = Class%(%)"] = "Ball = Class()",
        ["local ParticleSystem = Class%(%)"] = "ParticleSystem = Class()",
        ["local PowerUp = Class%(%)"] = "PowerUp = Class()",
        ["local GameManager = Class%(%)"] = "GameManager = Class()"
    })
    test.assert(loaded ~= nil, "Deveria carregar o script principal do Pong. Erro: " .. tostring(err))
    
    if loaded then
        -- Executa no ambiente global para registrar as classes Paddle e Ball
        loaded()
        
        -- 2. Testar instanciacao do Paddle (raquete)
        local paddle = Paddle(50, 200, "W", "S", {0.0, 1.0, 1.0}, false)
        test.assert(paddle ~= nil, "Deveria instanciar um Paddle corretamente")
        test.assertEqual(paddle.x, 50, "Coordenada X inicial do Paddle deveria ser 50")
        test.assertEqual(paddle.y, 200, "Coordenada Y inicial do Paddle deveria ser 200")
        
        -- 3. Testar instanciacao do ParticleSystem
        local ps = ParticleSystem()
        test.assert(ps ~= nil, "Deveria instanciar o ParticleSystem do jogo")
        
        -- 4. Testar instanciacao da Ball (bola)
        local ball = Ball(ps)
        test.assert(ball ~= nil, "Deveria instanciar a Ball corretamente")
        
        -- Inicializar bola com velocidade
        ball:Reset(1)
        ball:Serve(1)
        test.assert(ball.x > 0, "Coordenada X da bola apos Reset deveria ser maior que 0")
        test.assert(ball.y > 0, "Coordenada Y da bola apos Reset deveria ser maior que 0")
        
        -- 5. Simular colisao simples / ricochete
        local oldVx = ball.vx
        local mockGM = { Shake = function() end }
        ball:HandlePaddleBounce(paddle, true, mockGM)
        test.assert(ball.vx ~= oldVx, "A velocidade horizontal X da bola deveria mudar apos a colisao")
        
        -- 6. Testar hit timer do paddle
        local testPaddle = Paddle(50, 200, "W", "S", {0.0, 1.0, 1.0}, false)
        test.assertEqual(testPaddle.hitTimer, 0, "hitTimer inicial do paddle deveria ser 0")
        testPaddle:OnHit()
        test.assertEqual(testPaddle.hitTimer, 0.2, "hitTimer apos OnHit deveria ser 0.2")
    end
end)
