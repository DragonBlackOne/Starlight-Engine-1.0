-- STARLIGHT ARCADE MASTER (ROBUST VERSION)
local Games = {
    { name = "Celestial Nexus (3D Showcase)", file = "main.lua" },
    { name = "CyberSnake (2D Retro)", file = "arcade_2d.lua" },
    { name = "Voxel Rush (2.5D Suite)", file = "arcade_25d.lua" },
    { name = "Beginner Tutorial", file = "beginner_tutorial.lua" }
}

local CurrentGame = nil

function OnStart()
    Say("SBA Master: Ready.")
end

function OnUpdate(dt)
    if CurrentGame then
        if IsDown("escape") then 
            CurrentGame = nil 
            Say("Voltando ao Menu...")
            -- Reload master to recover OnUpdate/OnRender hooks
            dofile("assets/scripts/arcade_master.lua")
        end
    end
end

function OnRenderUI()
    if not CurrentGame then
        -- Create a fullscreen invisible window for the menu
        imgui.set_next_window_pos(vec2(0, 0))
        imgui.set_next_window_size(vec2(1280, 720))
        
        if imgui.begin_window("Starlight Arcade", nil, 63) then -- 63 = NoTitle, NoResize, NoMove, etc.
            imgui.text_colored(imgui.color(0, 1, 1, 1), "=== STARLIGHT ENGINE: SBA ARCADE MASTER ===")
            imgui.separator()
            imgui.text("Pressione o numero correspondente no teclado:")
            imgui.spacing()
            
            for i, g in ipairs(Games) do
                if imgui.button(i .. ". " .. g.name, vec2(400, 40)) or IsDown(tostring(i)) then
                    LoadSBA(g.file)
                end
            end
            
            imgui.separator()
            imgui.text_disabled("Dica: Pressione ESC dentro de um jogo para voltar aqui.")
            imgui.end_window()
        end
    end
end

function LoadSBA(file)
    Say("Carregando: " .. file)
    CurrentGame = file
    dofile("assets/scripts/" .. file)
    if OnStart then OnStart() end
end
