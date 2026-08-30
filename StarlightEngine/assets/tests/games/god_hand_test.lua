-- god_hand_test.lua
-- Testes unitarios e integridade para o God Hand 3D Brawler

test.describe("GodHand 3D - Sistemas de Combate, Rigging e Dificuldade", function()
    -- 1. Mocks de suporte 3D
    engine = engine or Engine or {}
    engine.spawn_primitive = engine.spawn_primitive or function() return 1 end
    engine.set_pbr = engine.set_pbr or function() end
    engine.set_pos = engine.set_pos or function() end
    engine.set_rotation = engine.set_rotation or function() end
    engine.spawn_light = engine.spawn_light or function() return 2 end
    engine.destroy = engine.destroy or function() end
    time = time or { set_scale = function() end, get_time = function() return 1.0 end }
    camera = camera or { set_pos = function() end, look_at = function() end }

    -- 2. Ajustar package.path para apontar para a pasta do GodHand_Project
    package.path = package.path .. ";../GodHand_Project/?.lua;../GodHand_Project/assets/scripts/?.lua;./GodHand_Project/?.lua;./GodHand_Project/assets/scripts/?.lua"

    -- 3. Carregar e testar modulos de God Hand
    local SFX = require("assets/scripts/sfx_manager")
    local Difficulty = require("assets/scripts/difficulty_system")
    local Combat = require("assets/scripts/combat_engine")
    local GodReel = require("assets/scripts/god_reel")
    local Player = require("assets/scripts/player_controller")
    local EnemyAI = require("assets/scripts/enemy_ai")
    local Stage = require("assets/scripts/stage_desert")
    local HumanoidRig = require("assets/scripts/humanoid_rig")
    local AnimationTree = require("assets/scripts/animation_tree")

    test.assert(Combat ~= nil, "Combat engine deveria carregar com sucesso")
    test.assert(Difficulty ~= nil, "Difficulty system deveria carregar com sucesso")
    test.assert(GodReel ~= nil, "GodReel system deveria carregar com sucesso")

    -- 4. Testar Inicializacao do Player
    Difficulty.init()
    Combat.init()
    GodReel.init()
    Player.init(0.0, 0.0, 2.0)

    test.assertEqual(Player.hp, 300, "Gene deveria iniciar com 300 HP")
    test.assertEqual(Player.state, "idle", "Gene deveria iniciar em estado idle")
    test.assert(Player.rig ~= nil, "Rig procedural do Player deveria ser instanciado")
    test.assert(Player.animTree ~= nil, "AnimationTree do Player deveria ser instanciado")

    -- 5. Testar Dynamic Difficulty ("Level DIE")
    test.assertEqual(Difficulty.level, 1, "Deveria iniciar em Level 1")
    Difficulty.addHit("special")
    Difficulty.addHit("special")
    Difficulty.addHit("special")
    Difficulty.addHit("special")
    Difficulty.addHit("special")
    test.assert(Difficulty.level >= 2, "Apos sequencia de golpes, a dificuldade deveria subir")
    
    Difficulty.grovelReset()
    test.assertEqual(Difficulty.level, 1, "Groveling (pedir perdao) deveria resetar a dificuldade para Level 1")

    -- 6. Testar Inimigos e Guard Break
    EnemyAI.init()
    local punk = EnemyAI.spawn("punk", 0.0, 0.0, -3.0)
    local bruiser = EnemyAI.spawn("bruiser", 3.0, 0.0, -3.0)
    local boss = EnemyAI.spawn("boss", -3.0, 0.0, -3.0)

    test.assertEqual(#EnemyAI.enemies, 3, "Deveriam existir 3 inimigos spawnados")
    test.assertEqual(punk.type, "punk", "Inimigo 1 deveria ser punk")
    test.assertEqual(bruiser.type, "bruiser", "Inimigo 2 deveria ser bruiser")
    test.assertEqual(boss.type, "boss", "Inimigo 3 deveria ser boss")

    -- 7. Testar Resolucao de Ataque e Guard Break
    bruiser.state = "block"
    local broken = Combat.resolveAttack(Player, bruiser, Player.guardBreaker)
    test.assert(broken, "Guard Breaker deveria quebrar a guarda do inimigo")
    test.assertEqual(bruiser.state, "guard_broken", "Inimigo bloqueando deveria entrar em guard_broken")

    -- 8. Testar God Reel e Unleash God Hand
    GodReel.addTension(100.0)
    test.assertEqual(GodReel.tension, 100.0, "Tensao deveria atingir 100%")
    local unleashed = GodReel.unleashGodHand(Player)
    test.assert(unleashed, "God Hand deveria ser liberado com 100% de tensao")
    test.assert(Player.isGodHand, "Gene deveria estar no modo God Hand")
    test.assert(Player.isInvincible, "Gene deveria estar invencivel durante o modo God Hand")
end)
