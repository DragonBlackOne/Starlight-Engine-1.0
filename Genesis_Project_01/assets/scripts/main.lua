-- TitanEngine Advanced Showcase Script 🛡️✨
-- 2026 Fusion Masterpiece | Quimera + Starlight 🛡️✨

Log.info("TitanEngine: Script-driven logic initialized!")

local totalTime = 0

-- Main Update Loop
function Update(dt)
    totalTime = totalTime + dt
    
    -- 1. Get Core Systems
    local input = Engine:get_input()
    local audio = Engine:get_audio()
    local registry = Engine:get_registry()
    
    -- 2. Trigger FM Bass on "Jump" (Space)
    if input:is_pressed("Jump") then
        Log.info("Lua: Space pressed! Triggering 4-Op FM Sound.")
        audio:play_fm_note(55.0, 0.2, 0)
    end
    
    -- 3. Play a chime every 3 seconds
    if math.floor(totalTime % 3.0) == 0 and math.floor((totalTime - dt) % 3.0) ~= 0 then
        Log.info("Lua: Periodic chime triggered.")
        audio:play_fm_note(880.0, 0.1, 7)
    end

    -- 4. Scene Serialization (F5 = Save, F9 = Load)
    if input:is_pressed("Save") then
        save_scene("assets/scenes/showcase.json")
        Log.info("Lua: Scene saved to showcase.json!")
        audio:play_fm_note(1200.0, 0.05, 7)
    end
    if input:is_pressed("Load") then
        load_scene("assets/scenes/showcase.json")
        Log.info("Lua: Scene loaded from showcase.json!")
        audio:play_fm_note(600.0, 0.05, 7)
    end
end

Log.info("TitanEngine: Lua Phase 5 logic live.")
