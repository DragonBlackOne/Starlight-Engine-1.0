-- ============================================================================
-- beginner_tutorial.lua — SBA v3.0 Showcase
-- A comprehensive demonstration of the new version 3.0 features.
-- ============================================================================
package.path = package.path .. ";assets/scripts/?.lua"
require("sba_bridge")

-- 1. Signals & Events
local onGameStarted = Signal()
onGameStarted:connect(function(msg)
    Say("Signal Received: " .. msg)
end)

-- 2. State Machine for a simple Game Flow
local GameFlow = StateMachine()

GameFlow:add("Menu", {
    enter = function() 
        Say("Entered Menu State")
        Events.emit("ui_change", "MAIN_MENU")
    end,
    update = function(owner, dt)
        if Input.isJustPressed("Space") then
            GameFlow:switch("Play")
        end
    end
})

GameFlow:add("Play", {
    enter = function()
        Say("Game Started!")
        onGameStarted:emit("Let's Go!")
        ScreenShake.trigger(10, 0.5)
    end,
    update = function(owner, dt)
        -- Gameplay logic here
    end
})

-- 3. Particle System
local fire = Particle2D({
    x = 0, y = 0,
    max = 200,
    rate = 50,
    lifetime = 1.5,
    speed = 150,
    gravity = -50,
    angle = -math.pi/2,
    spread = math.pi/4,
    r = 1, g = 0.5, b = 0.2
})

-- 4. Entity with Tag & Components
local player = Entity("Player", 0, 0, 0)
player:set("health", 100)
player:set("score", 0)
player:setColor(Color.hex("#3498db"))

-- 5. Tweens & Sequences
Tween.sequence(player, {
    { props = { x = 5 }, duration = 1, easing = "easeOutQuad" },
    { props = { y = 2 }, duration = 1, easing = "easeInBack", delay = 0.5 },
    { props = { x = 0, y = 0 }, duration = 1, easing = "easeInOutCubic" }
})

-- 6. Main Loop Hooks
function OnUpdate(dt)
    GameFlow:update(nil, dt)
    Timer.update(dt)
    Tween.update(dt)
    Coroutine.update(dt)
    ScreenShake.update(dt)
    fire:update(dt)
    
    -- Update fire position to follow player (2D projected)
    local px, py, pz = player:getPos()
    fire.x = px * 100 -- Arbitrary scale for 2D view
    fire.y = py * 100
end

function OnRenderUI()
    -- New GFX Primitives
    gfx.draw_rect_outline(10, 10, 200, 50, 2, 1, 1, 1)
    gfx.draw_circle(400, 300, 50, 1, 0, 0)
    gfx.draw_line(0, 0, gfx.screen_width(), gfx.screen_height(), 2, 0, 1, 0)
    
    fire:draw()
    
    imgui.text(20, 20, 1, 1, 1, "SBA v3.0 Tutorial")
    imgui.text(20, 40, 1, 1, 0, "Press SPACE to start")
    imgui.text(20, 60, 0.5, 1, 0.5, "Health: " .. player:get("health"))
end

GameFlow:switch("Menu")
Say("Tutorial v3.0 Initialized.")
