# ============================================================================
# create_project.ps1 - Starlight Engine SDK
# Creates a new game project with the full SBA v2.0 framework pre-installed
# ============================================================================

param (
    [Parameter(Mandatory=$true)]
    [string]$ProjectName,
    [ValidateSet("2d", "3d")]
    [string]$Template = "3d"
)

$SDK_DIR = (Get-Location).Path.Replace('\', '/')
$PARENT_DIR = (Split-Path -Path $SDK_DIR -Parent).Replace('\', '/')
$TARGET_DIR = Join-Path $PARENT_DIR $ProjectName

Write-Host ">>> Creating Project: $ProjectName (Template: $Template) <<<" -ForegroundColor Cyan

# 1. Create folder structure
New-Item -ItemType Directory -Path (Join-Path $TARGET_DIR "src") -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $TARGET_DIR "assets\scripts") -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $TARGET_DIR "assets\textures") -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $TARGET_DIR "assets\audio") -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $TARGET_DIR "assets\fonts") -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $TARGET_DIR "assets\shaders") -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $TARGET_DIR "build") -Force | Out-Null

# 2. Copy main.cpp boilerplate
$is2D = ($Template -eq "2d")
$mode2DLine = if ($is2D) { "    config.mode2D = true;" } else { "    // config.mode2D = true; // Uncomment for 2D-only mode" }

$MainContent = @"
#include "Engine.hpp"
#include "Log.hpp"
#include "ScriptSystem.hpp"

using namespace starlight;

class GameProject : public Scene {
public:
    void OnEnter() override {
        Log::Info("$ProjectName: Initialized.");
        auto& scripting = Engine::Get().GetScripting();
        scripting.ExecuteFile("assets/scripts/${ProjectName}_main.lua");
        
        sol::function onStart = scripting.GetLua()["OnStart"];
        if (onStart.valid()) onStart();
    }

    void OnUpdate(float dt) override {
        auto& scripting = Engine::Get().GetScripting();
        sol::function onUpdate = scripting.GetLua()["OnUpdate"];
        if (onUpdate.valid()) onUpdate(dt);
    }

    void OnRenderUI() override {
        // ScriptSystem handles the UI render event automatically
    }

    void OnExit() override {
        Log::Info("$ProjectName: Exited.");
    }
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    try {
        WindowConfig config;
        config.title = "$ProjectName // Fusion ENGINE v1.0";
        config.width = 1280;
        config.height = 720;
        config.vsync = true;
$mode2DLine

        Engine engine;
        engine.Initialize(config);
        engine.GetSceneStack().Push(std::make_shared<GameProject>());
        engine.Run();
        engine.Shutdown();
    } catch (const std::exception& e) {
        Log::Error("Fatal: {}", e.what());
        return 1;
    }
    return 0;
}
"@
$MainContent | Set-Content (Join-Path $TARGET_DIR "src\main.cpp")

# 3. Copy SBA Framework
Copy-Item (Join-Path $SDK_DIR "assets\scripts\core.lua") -Destination (Join-Path $TARGET_DIR "assets\scripts\core.lua") -Force
Copy-Item (Join-Path $SDK_DIR "assets\scripts\sba_bridge.lua") -Destination (Join-Path $TARGET_DIR "assets\scripts\sba_bridge.lua") -Force

# 3b. Copy Shaders (required for rendering)
if (Test-Path (Join-Path $SDK_DIR "assets\shaders")) {
    Copy-Item (Join-Path $SDK_DIR "assets\shaders") -Destination (Join-Path $TARGET_DIR "assets\shaders") -Recurse -Force
}

# 4. Generate starter game script
if ($is2D) {
$StarterScript = @"
-- ============================================================================
-- $ProjectName — 2D Starter Script (Fusion ENGINE)
-- ============================================================================
package.path = package.path .. ";assets/scripts/?.lua"
require("sba_bridge")

local score = 0
local playerX, playerY = 640, 360
local speed = 300

Scene.register("Game", {
    onEnter = function()
        Say("$ProjectName: 2D Game Started!")
    end,

    onUpdate = function(dt)
        if input.is_down("W") or input.is_down("UP")    then playerY = playerY - speed * dt end
        if input.is_down("S") or input.is_down("DOWN")  then playerY = playerY + speed * dt end
        if input.is_down("A") or input.is_down("LEFT")  then playerX = playerX - speed * dt end
        if input.is_down("D") or input.is_down("RIGHT") then playerX = playerX + speed * dt end

        Tween.update(dt)
        Coroutine.update(dt)
    end,

    onRenderUI = function()
        ui.begin(1280, 720)

        -- Background
        ui.panel(0, 0, 1280, 720, 0.05, 0.05, 0.1, 1)

        -- Player (rounded rect)
        gfx.draw_rounded_rect(playerX - 20, playerY - 20, 40, 40, 8, 0, 1, 0.8, 1)

        -- HUD
        ui.panel(10, 10, 260, 50, 0.02, 0.02, 0.05, 0.85)
        ui.label("$ProjectName // 2D", 30, 45, 0, 1, 0.8, 1)
        ui.finish()
    end,
})

function OnStart() Scene.switch("Game") end
function OnUpdate(dt) Scene.update(dt) end
function OnRenderUI() Scene.renderUI() end
"@
} else {
$StarterScript = @"
-- ============================================================================
-- $ProjectName — 3D Starter Script (Fusion ENGINE)
-- ============================================================================
package.path = package.path .. ";assets/scripts/?.lua"
require("sba_bridge")

local player = nil

Scene.register("Game", {
    onEnter = function()
        Say("$ProjectName: Game Started!")
        Engine.set_camera_pos(0, 15, 15)
        Engine.look_at(0, 0, 0)

        player = Entity("Player", 0, 0.5, 0)
        player:setColor(0, 1, 1)
        player:setScale(1)
        player:setMaterial(0.8, 0.2)

        local floor = Entity("Floor", 0, -0.5, 0)
        floor:setColor(0.1, 0.1, 0.15)
        floor:setScale(20, 0.1, 20)

        Light(0, 12, 5, 1, 0.9, 0.8, 800)
    end,

    onUpdate = function(dt)
        local speed = 8 * dt
        if input.is_down("W") then player:move(0, 0, -speed) end
        if input.is_down("S") then player:move(0, 0, speed) end
        if input.is_down("A") then player:move(-speed, 0, 0) end
        if input.is_down("D") then player:move(speed, 0, 0) end

        Tween.update(dt)
        Coroutine.update(dt)
    end,

    onRenderUI = function()
        ui.begin(1600, 900)
        ui.panel(10, 10, 300, 50, 0.02, 0.02, 0.05, 0.85)
        ui.label("$ProjectName // Fusion ENGINE", 30, 45, 0, 1, 0.8, 1)
        ui.finish()
    end,
})

function OnStart() Scene.switch("Game") end
function OnUpdate(dt) Scene.update(dt) end
function OnRenderUI() Scene.renderUI() end
"@
}
$StarterScript | Set-Content (Join-Path $TARGET_DIR "assets\scripts\${ProjectName}_main.lua")

# 5. Generate CMakeLists.txt
$mode2DFlag = if ($is2D) { "MODE_2D" } else { "" }
$CMakeContent = "cmake_minimum_required(VERSION 3.20)`n"
$CMakeContent += "project($ProjectName LANGUAGES C CXX)`n`n"
$CMakeContent += "set(CMAKE_CXX_STANDARD 20)`n"
$CMakeContent += "set(CMAKE_CXX_STANDARD_REQUIRED ON)`n"
$CMakeContent += "set(CMAKE_MSVC_RUNTIME_LIBRARY ""MultiThreaded`$<$<CONFIG:Debug>:Debug>DLL"")`n`n"
$CMakeContent += "include(`"${SDK_DIR}/cmake/StarlightProject.cmake`")`n"
$CMakeContent += "add_starlight_game($ProjectName $mode2DFlag)`n"

$CMakeContent | Set-Content (Join-Path $TARGET_DIR "CMakeLists.txt")

# 6. Generate README
$ReadmeContent = @"
# $ProjectName

Built with **Fusion Engine (Starlight Engine SDK)**

## SBA v2.0 Framework Included
- Entity OO Wrapper
- Scene Manager (State Machine)
- Tween System (8 easing functions)
- Event Bus (pub/sub)
- Coroutine Runner
- Color, MathX, Physics2D, Timer, ScreenShake

## Build
``````powershell
cmake -B build -S .
cmake --build build --config Release
.\build\Release\$ProjectName.exe
``````
"@
$ReadmeContent | Set-Content (Join-Path $TARGET_DIR "README.md")

# 7. Generate .gitignore
$GitignoreContent = @"
build/
*.exe
*.obj
*.lib
*.pdb
*.ilk
*.log
.vs/
CMakeCache.txt
CMakeFiles/
"@
$GitignoreContent | Set-Content (Join-Path $TARGET_DIR ".gitignore")

Write-Host ">>> Project $ProjectName created at: $TARGET_DIR <<<" -ForegroundColor Green
Write-Host "   - Template: $Template" -ForegroundColor DarkGray
Write-Host "   - SBA framework pre-installed" -ForegroundColor DarkGray
Write-Host "   - Starter script with Entity, Scene, Tween" -ForegroundColor DarkGray
Write-Host "   - .gitignore included" -ForegroundColor DarkGray
Write-Host "   - Ready to build with cmake" -ForegroundColor DarkGray
