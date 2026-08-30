-- tech_demo_ui.lua
-- Outrun Synthwave Telemetry HUD & Module Status Overlay

local UI = {
    showOverlay = true,
    fps = 60,
    frameTime = 16.6,
    activeShowroom = 1,
    particleCount = 12000
}

function UI.draw(fps, dt, showroomIndex, cameraPos, lightCount, physicsBodyCount)
    local screenW = (window and window.get_width) and window.get_width() or 1920
    local screenH = (window and window.get_height) and window.get_height() or 1080

    -- Cyberpunk Neon Theme Colors
    local cDarkBg    = {0.05, 0.04, 0.09, 0.88}
    local cCyan      = {0.0, 0.85, 1.0, 1.0}
    local cMagenta   = {1.0, 0.0, 0.60, 1.0}
    local cViolet    = {0.65, 0.15, 1.0, 1.0}
    local cGold      = {1.0, 0.82, 0.0, 1.0}
    local cGreen     = {0.2, 0.95, 0.4, 1.0}
    local cWhite     = {0.95, 0.95, 0.98, 1.0}
    local cMuted     = {0.6, 0.6, 0.7, 1.0}

    -- 1. Top Header Banner
    draw.rect_filled(20, 15, 480, 50, cDarkBg[1], cDarkBg[2], cDarkBg[3], cDarkBg[4])
    draw.rect(20, 15, 480, 50, cCyan[1], cCyan[2], cCyan[3], 0.9)
    draw.text(35, 23, "FUSION ENGINE // NEXT-GEN TECH DEMO", cCyan[1], cCyan[2], cCyan[3], 1.0)
    draw.text(35, 42, "C++20 COMMERCIAL DOD / ECS ARCHITECTURE", cMagenta[1], cMagenta[2], cMagenta[3], 0.9)

    -- 2. Telemetry & Engine Performance Card (Top Right)
    local cardW = 340
    local cardH = 140
    local cardX = screenW - cardW - 20
    local cardY = 15

    draw.rect_filled(cardX, cardY, cardW, cardH, cDarkBg[1], cDarkBg[2], cDarkBg[3], cDarkBg[4])
    draw.rect(cardX, cardY, cardW, cardH, cMagenta[1], cMagenta[2], cMagenta[3], 0.8)

    draw.text(cardX + 15, cardY + 12, "REAL-TIME TELEMETRY", cGold[1], cGold[2], cGold[3], 1.0)
    draw.text(cardX + 15, cardY + 36, string.format("FPS: %d  (%.2f ms)", math.floor(fps), dt * 1000.0), cGreen[1], cGreen[2], cGreen[3], 1.0)
    draw.text(cardX + 15, cardY + 58, string.format("Physics Bodies: %d | Point Lights: %d", physicsBodyCount, lightCount), cWhite[1], cWhite[2], cWhite[3], 0.9)
    draw.text(cardX + 15, cardY + 80, string.format("Camera: X:%.1f Y:%.1f Z:%.1f", cameraPos.x, cameraPos.y, cameraPos.z), cMuted[1], cMuted[2], cMuted[3], 0.9)
    draw.text(cardX + 15, cardY + 102, "Render Pipeline: 3D PBR Cook-Torrance (Deferred/Fwd+)", cCyan[1], cCyan[2], cCyan[3], 0.85)

    -- 3. 7-Module Architecture Status Matrix (Left Panel)
    local modX = 20
    local modY = 80
    local modW = 380
    local modH = 220

    draw.rect_filled(modX, modY, modW, modH, cDarkBg[1], cDarkBg[2], cDarkBg[3], cDarkBg[4])
    draw.rect(modX, modY, modW, modH, cViolet[1], cViolet[2], cViolet[3], 0.7)

    draw.text(modX + 15, modY + 12, "COMMERCIAL ARCHITECTURE MODULES", cCyan[1], cCyan[2], cCyan[3], 1.0)

    local modules = {
        "M1: Memory Allocators (Pool, Stack, Arena) & SIMD",
        "M2: Windowing (SDL/GLFW), Event Broker & RHI Base",
        "M3: Core ECS Handles & Scene Graph Hierarchy",
        "M4: 2D Batching & 3D PBR Cook-Torrance Shaders",
        "M5: Asset Pipeline & Hot-Reloading Watcher",
        "M6: Physics Subsystem (Jolt/Sim) & Lua Scripting",
        "M7: WYSIWYG ImGui Editor, Viewport & Profiler"
    }

    for i, m in ipairs(modules) do
        local lineY = modY + 36 + (i - 1) * 24
        draw.text(modX + 15, lineY, "●", cGreen[1], cGreen[2], cGreen[3], 1.0)
        draw.text(modX + 32, lineY, m, cWhite[1], cWhite[2], cWhite[3], 0.9)
    end

    -- 4. Interactive Showroom Selector Tabs (Center-Bottom)
    local tabNames = {
        "[1] PBR Grid",
        "[2] 2D Particle Waves",
        "[3] Physics Arena",
        "[4] Solar Hierarchy",
        "[5] Free Fly Cam"
    }
    local tabW = 160
    local tabH = 34
    local totalTabsW = #tabNames * (tabW + 10)
    local startTabX = (screenW - totalTabsW) * 0.5
    local tabY = screenH - 85

    for idx, name in ipairs(tabNames) do
        local tx = startTabX + (idx - 1) * (tabW + 10)
        local isActive = (showroomIndex == idx)

        if isActive then
            draw.rect_filled(tx, tabY, tabW, tabH, cCyan[1] * 0.3, cCyan[2] * 0.3, cCyan[3] * 0.3, 0.9)
            draw.rect(tx, tabY, tabW, tabH, cCyan[1], cCyan[2], cCyan[3], 1.0)
            draw.text(tx + 18, tabY + 8, name, cCyan[1], cCyan[2], cCyan[3], 1.0)
        else
            draw.rect_filled(tx, tabY, tabW, tabH, cDarkBg[1], cDarkBg[2], cDarkBg[3], 0.7)
            draw.rect(tx, tabY, tabW, tabH, cMuted[1], cMuted[2], cMuted[3], 0.4)
            draw.text(tx + 18, tabY + 8, name, cMuted[1], cMuted[2], cMuted[3], 0.8)
        end
    end

    -- 5. Interactive Keymap Banner (Bottom Center)
    local helpY = screenH - 35
    local helpText = "[W/A/S/D] Fly Cam  |  [F] Fire Kinetic Sphere  |  [G] Spawn Pyramid  |  [Space] Reverse Gravity  |  [F5] Live Hot-Reload"
    draw.rect_filled(screenW * 0.5 - 460, helpY - 5, 920, 30, cDarkBg[1], cDarkBg[2], cDarkBg[3], 0.85)
    draw.rect(screenW * 0.5 - 460, helpY - 5, 920, 30, cGold[1], cGold[2], cGold[3], 0.5)
    draw.text(screenW * 0.5 - 440, helpY + 2, helpText, cGold[1], cGold[2], cGold[3], 0.95)
end

return UI
