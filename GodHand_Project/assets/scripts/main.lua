-- main.lua
-- God Hand 3D: Open-World Brawler Edition with Full Exploration & Realistic Humanoid Anatomy

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
    state = "playing", -- "playing", "victory", "gameover"
    announcementTimer = 4.0,
    announcementText = "GOLDEN ARM OPEN WORLD",
    announcementSub = "EXPLORE 5 ZONES & DEFEAT WARLORDS!",
    waveEnemiesSpawned = false
}

function OnStart()
    engine.log("God Hand: Initializing Open-World Brawler World...")

    -- 1. Initialize Systems & Atmospheric Lighting
    if Atmosphere and Atmosphere.apply and Atmosphere.DesertNoon then
        Atmosphere.apply(Atmosphere.DesertNoon)
    end
    SFX.init()
    Difficulty.init()
    Combat.init()
    Camera.init()
    GodReel.init()
    Stage.init()
    EnemyAI.init()

    -- 2. Spawn Gene at Town Square Center
    Player.init(0.0, 0.0, 2.0)

    -- 3. Game State & Announcement
    Game.state = "playing"
    Game.announcementTimer = 4.5
    Game.announcementText = "GOLDEN ARM DESERT FRONTIER"
    Game.announcementSub = "WASD: 3D Move | Mouse: 360 Look | Shift: Sprint | Space: Jump"

    -- 4. Populate Open-World Zones with Roaming Enemies
    Game.populateOpenWorld()
end

function Game.populateOpenWorld()
    -- Exact enemy positioning to match image.png 1:1
    local e1 = EnemyAI.spawn("punk", 0.6, 0.0, -3.2)
    local e2 = EnemyAI.spawn("bruiser", 1.8, 0.0, -3.0)
    e1.facingAngle = 220.0
    e2.facingAngle = 215.0

    -- Additional enemies across open world zones

    -- Zone 2: Red Rock Mesa & Saguaro Canyon
    EnemyAI.spawn("bruiser", 56.0, 0.0, -52.0)
    EnemyAI.spawn("punk", 68.0, 0.0, -62.0)

    -- Zone 3: Outlaw Fort & Bandit Outpost
    EnemyAI.spawn("punk", -54.0, 0.0, -54.0)
    EnemyAI.spawn("punk", -64.0, 0.0, -50.0)
    EnemyAI.spawn("bruiser", -50.0, 0.0, -62.0)

    -- Zone 4: Desert Oasis & Crystal Springs
    EnemyAI.spawn("punk", 56.0, 0.0, 56.0)
    EnemyAI.spawn("punk", 68.0, 0.0, 62.0)

    -- Zone 5: Ancient Gladiator Arena (The Boss Encounter!)
    EnemyAI.spawn("boss", -60.0, 0.0, 60.0)
    EnemyAI.spawn("bruiser", -54.0, 0.0, 54.0)
    EnemyAI.spawn("bruiser", -66.0, 0.0, 66.0)
end

function OnUpdate(dt)
    -- Global Restart on Return / Enter
    if (Game.state == "gameover" or Game.state == "victory") and (input.is_just_pressed("return") or input.is_just_pressed("space")) then
        OnStart()
        return
    end

    -- 1. Announcements
    if Game.announcementTimer > 0 then
        Game.announcementTimer = Game.announcementTimer - dt
    end

    -- 2. God Reel Roulette Menu Navigation
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

    -- 3. Dynamic Audio
    SFX.update(dt)

    -- 4. Props & Stage
    Stage.update(dt, Player)

    -- 5. Player Controller
    Player.update(dt, EnemyAI.enemies, Stage.props)

    -- 6. Enemy AI
    EnemyAI.update(dt, Player)

    -- 7. Combat & Hitstop
    Combat.update(dt)

    -- 8. Dynamic Difficulty
    Difficulty.update(dt)

    -- 9. God Hand Reel Mode
    GodReel.update(dt)

    -- 10. 360 Orbit Camera Tracking
    Camera.update(dt, Player.pos, Player.facingAngle, GodReel.isOpen or GodReel.isGodHandActive)

    -- 11. Check Victory / Defeat Conditions
    if Player.hp <= 0 and Game.state == "playing" then
        Game.state = "gameover"
        Game.announcementTimer = 999.0
        Game.announcementText = "YOU DIED!"
        Game.announcementSub = "PRESS [ENTER] TO RESTART"
        SFX.playDefeat()
    end

    -- Check if Boss in Arena is defeated
    local bossAlive = false
    for _, e in ipairs(EnemyAI.enemies) do
        if e.type == "boss" and e.hp > 0 and e.state ~= "ko" then
            bossAlive = true
            break
        end
    end
    if not bossAlive and Game.state == "playing" and #EnemyAI.enemies > 0 then
        -- Boss defeated
        Game.state = "victory"
        Game.announcementTimer = 999.0
        Game.announcementText = "WARLORD DEFEATED! VICTORY!"
        Game.announcementSub = "THE GOLDEN ARM SAVED THE WEST! [ENTER] TO RESTART"
        SFX.playVictory()
    end
end

function OnRenderUI()
    -- 2D HUD & Exploration UI
    HUD.draw(Player, EnemyAI.enemies, Game.currentWave, Game)
end

function OnRender()
    OnRenderUI()
end
