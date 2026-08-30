-- main.lua
-- God Hand 3D Beat 'Em Up (PS2 Retro Style) for Fusion ENGINE

package.path = package.path .. ";assets/scripts/?.lua;./assets/scripts/?.lua"
engine = engine or Engine
Engine = Engine or engine

local SFX = require("assets/scripts/sfx_manager")
local Difficulty = require("assets/scripts/difficulty_system")
local Combat = require("assets/scripts/combat_engine")
local Camera = require("assets/scripts/camera_controller")
local Player = require("assets/scripts/player_controller")
local EnemyAI = require("assets/scripts/enemy_ai")
local Stage = require("assets/scripts/stage_desert")
local GodReel = require("assets/scripts/god_reel")
local HUD = require("assets/scripts/hud")

local Game = {
    currentWave = 1,
    maxWaves = 3,
    state = "intro", -- "intro", "playing", "victory", "gameover"
    announcementTimer = 3.5,
    announcementText = "STAGE 1: DUSTY SALOON",
    announcementSub = "DEFEAT ALL BRAWLERS!",
    waveEnemiesSpawned = false
}

function OnStart()
    engine.log("God Hand: Starting 3D Brawler Game Scene...")

    -- 1. Initialize Subsystems
    SFX.init()
    Difficulty.init()
    Combat.init()
    Camera.init()
    GodReel.init()
    Stage.init()
    EnemyAI.init()

    -- 2. Initialize Player at Desert Saloon Center
    Player.init(0.0, 0.0, 2.0)

    -- 3. Game Flow State
    Game.currentWave = 1
    Game.state = "playing"
    Game.announcementTimer = 3.5
    Game.announcementText = "STAGE 1: DUSTY SALOON"
    Game.announcementSub = "FIGHT!"
    Game.waveEnemiesSpawned = false

    -- Spawn Wave 1
    Game.spawnWave(1)
end

function Game.spawnWave(waveNum)
    Game.currentWave = waveNum
    Game.waveEnemiesSpawned = true

    if waveNum == 1 then
        Game.announcementTimer = 3.2
        Game.announcementText = "WAVE 1: THUG INVASION"
        Game.announcementSub = "PUNKS INCOMING! [J] TO COMBO"

        EnemyAI.spawn("punk", -5.5, 0.0, -5.0)
        EnemyAI.spawn("punk", 5.5, 0.0, -5.0)
        EnemyAI.spawn("punk", 0.0, 0.0, -8.0)
    elseif waveNum == 2 then
        Game.announcementTimer = 3.5
        Game.announcementText = "WAVE 2: HEAVY BRUISERS"
        Game.announcementSub = "USE GUARD BREAK [K] & DODGES [I/U/O]!"

        EnemyAI.spawn("punk", -6.5, 0.0, -6.0)
        EnemyAI.spawn("punk", 6.5, 0.0, -6.0)
        EnemyAI.spawn("bruiser", -3.5, 0.0, -8.5)
        EnemyAI.spawn("bruiser", 3.5, 0.0, -8.5)
    elseif waveNum == 3 then
        Game.announcementTimer = 4.0
        Game.announcementText = "FINAL WAVE: MAD MIDGET BOSS"
        Game.announcementSub = "UNLEASH THE GOD HAND [F]!"

        EnemyAI.spawn("boss", 0.0, 0.0, -9.5)
        EnemyAI.spawn("bruiser", -6.0, 0.0, -5.5)
        EnemyAI.spawn("bruiser", 6.0, 0.0, -5.5)
        EnemyAI.spawn("punk", -3.0, 0.0, -4.0)
        EnemyAI.spawn("punk", 3.0, 0.0, -4.0)
    end
end

function OnUpdate(dt)
    -- Global Restart Trigger on Enter / Return if Game Over or Victory
    if (Game.state == "gameover" or Game.state == "victory") and (input.is_just_pressed("return") or input.is_just_pressed("space")) then
        OnStart()
        return
    end

    -- 1. Handle Announcement Timers
    if Game.announcementTimer > 0 then
        Game.announcementTimer = Game.announcementTimer - dt
    end

    -- 2. God Reel Menu Navigation
    if GodReel.isOpen then
        if input.is_just_pressed("up") or input.is_just_pressed("i") or input.is_just_pressed("w") then
            GodReel.prevCard()
        elseif input.is_just_pressed("down") or input.is_just_pressed("k") or input.is_just_pressed("s") then
            GodReel.nextCard()
        elseif input.is_just_pressed("return") or input.is_just_pressed("j") or input.is_mouse_down(1) then
            GodReel.selectAndExecute(Player, EnemyAI.enemies)
        elseif input.is_just_pressed("escape") or input.is_just_pressed("tab") or input.is_just_pressed("q") then
            GodReel.close()
        end
        return
    end

    -- 3. Update BGM Synthesizer
    SFX.update(dt)

    -- 4. Update Stage & Props
    Stage.update(dt, Player)

    -- 5. Update Player
    if Player.hp > 0 then
        Player.update(dt, EnemyAI.enemies, Stage.props)
    else
        if Game.state ~= "gameover" then
            Game.state = "gameover"
            Game.announcementTimer = 100.0
            Game.announcementText = "YOU DIED!"
            Game.announcementSub = "PRESS [ENTER] TO TRY AGAIN"
        end
    end

    -- 6. Update Enemy AI
    EnemyAI.update(dt, Player)

    -- 7. Update Combat Subsystem & Hit-Stops
    Combat.update(dt)

    -- 8. Update God Reel Tension Timer
    GodReel.update(dt, Player)

    -- 9. Update 3D Over-the-shoulder Camera
    Camera.update(dt, Player.pos, Player.facingAngle, GodReel.isOpen)

    -- 10. Check Wave Completion
    if Game.state == "playing" then
        local allDead = true
        for _, e in ipairs(EnemyAI.enemies) do
            if e.hp > 0 then
                allDead = false
                break
            end
        end

        if allDead and Game.waveEnemiesSpawned then
            if Game.currentWave < Game.maxWaves then
                Game.spawnWave(Game.currentWave + 1)
            else
                Game.state = "victory"
                Game.announcementTimer = 100.0
                Game.announcementText = "★ STAGE CLEARED! ★"
                Game.announcementSub = "LEGENDARY GOD HAND! FINAL SCORE: " .. Difficulty.score
            end
        end
    end
end

function OnRender()
    -- 3D Mesh Components rendered automatically by StarlightCore pipeline
end

function OnRenderUI()
    -- Render Full PS2 God Hand HUD
    HUD.draw(Player, EnemyAI.enemies, Game.currentWave, Game)
end

OnUIRender = OnRenderUI
