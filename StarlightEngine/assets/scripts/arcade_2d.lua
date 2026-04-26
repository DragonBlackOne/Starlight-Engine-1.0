-- STARLIGHT 2D ARCADE (MIGRATED TO SBA)
local State = "MENU"
local Score = 0

function OnUpdate(dt)
    if State == "MENU" then
        Rect(0, 0, 1280, 720, 0.05, 0.05, 0.1)
        Say2D(500, 300, "STARLIGHT 2D ARCADE", 1, 1, 1)
        if IsDown("space") then State = "PLAY" end
    elseif State == "PLAY" then
        -- Simple Invaders Logic in SBA
        Rect(0, 0, 1280, 720, 0, 0, 0.1)
        Rect(playerX, 650, 60, 20, 0, 1, 0)
        
        if IsDown("left") then playerX = playerX - 400 * dt end
        if IsDown("right") then playerX = playerX + 400 * dt end
    end
end

function Say2D(x, y, msg, r, g, b)
    local dl = imgui.get_foreground_draw_list()
    dl:add_text(vec2.new(x, y), imgui.color(r, g, b, 1), msg)
end
