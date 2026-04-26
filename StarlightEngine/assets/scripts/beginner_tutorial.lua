-- STARLIGHT ENGINE: BEGINNER TUTORIAL
-- Criando um jogo simples em poucas linhas

function OnStart()
    Say("Bem-vindo ao Starlight Engine!")
    
    -- Cria o jogador (uma caixa ciano)
    player = Object("Player", 0, 0, 0)
    SetColor(player, 0, 1, 1)
    
    -- Cria um objetivo (uma caixa dourada)
    goal = Object("Goal", 10, 0, 10)
    SetColor(goal, 1, 0.8, 0)
    
    score = 0
end

function OnUpdate(dt)
    -- Movimentação simples
    if IsDown("up")    then Move(player, 0, 0, -10 * dt) end
    if IsDown("down")  then Move(player, 0, 0, 10 * dt) end
    if IsDown("left")  then Move(player, -10 * dt, 0, 0) end
    if IsDown("right") then Move(player, 10 * dt, 0, 0) end
    
    -- Verifica distância (colisão simples)
    if Distance(player, goal) < 1.5 then
        score = score + 1
        Say("Ponto! Score: " .. score)
        
        -- Move o objetivo para um lugar aleatório
        SetPos(goal, math.random(-15, 15), 0, math.random(-15, 15))
        
        -- Toca um som retro
        Sound(660, 0.1)
    end
end
