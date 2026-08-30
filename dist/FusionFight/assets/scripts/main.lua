-- main.lua
-- FusionFight main manager script
package.path = package.path .. ";assets/scripts/?.lua;assets/scripts/screens/?.lua"

require("assets/scripts/core")
require("assets/scripts/sba_bridge")

-- Load modules globally for access in screens
HUD = require("assets/scripts/hud")
Stages = require("assets/scripts/stages")
FighterRenderer = require("assets/scripts/fighter_renderer")

-- Import screens
local ScreenTitle = require("assets/scripts/screens/screen_title")
local ScreenMenu = require("assets/scripts/screens/screen_menu")
local ScreenCharSelect = require("assets/scripts/screens/screen_charselect")
local ScreenStageSelect = require("assets/scripts/screens/screen_stageselect")
local ScreenOptions = require("assets/scripts/screens/screen_options")
local ScreenFight = require("assets/scripts/screens/screen_fight")
local ScreenResult = require("assets/scripts/screens/screen_result")

-- App instance
App = {}
App.currentScreenName = "TITLE"
App.currentScreen = nil

local screens = {
    TITLE = ScreenTitle,
    MENU = ScreenMenu,
    CHARSELECT = ScreenCharSelect,
    STAGESELECT = ScreenStageSelect,
    OPTIONS = ScreenOptions,
    FIGHT = ScreenFight,
    RESULT = ScreenResult
}

-- Global gameplay settings
GameSettings = {
    roundsToWin = Save.read("roundsToWin", 2),
    aiDifficulty = Save.read("aiDifficulty", 1), -- 0=easy, 1=normal, 2=hard, -1=vs human local
    graphicsPreset = Save.read("graphicsPreset", 1), -- 0=perf, 1=balanced, 2=fidelity
    debugBoxes = Save.read("debugBoxes", false),
    p1Char = 0,
    p2Char = 1,
    stage = 0
}

function App:SwitchTo(screenName)
    if self.currentScreen and self.currentScreen.Exit then
        self.currentScreen:Exit()
    end
    
    self.currentScreenName = screenName
    self.currentScreen = screens[screenName]()
    
    if self.currentScreen and self.currentScreen.Enter then
        self.currentScreen:Enter()
    end
end

-- Hook functions
function OnStart()
    Engine.log("FusionFight: Lua Manager Initialized.")
    
    if Engine and Engine.set_graphics_preset then
        Engine.set_graphics_preset(GameSettings.graphicsPreset)
    end
    
    if fight and fight.stop_replay then
        fight.stop_replay()
    end
    
    -- Map keyboard movements
    Input.map("P1_Up", "W")
    Input.map("P1_Down", "S")
    Input.map("P1_Left", "A")
    Input.map("P1_Right", "D")
    
    Input.map("P2_Up", "Up")
    Input.map("P2_Down", "Down")
    Input.map("P2_Left", "Left")
    Input.map("P2_Right", "Right")
    
    App:SwitchTo("TITLE")
end

function OnUpdate(dt)
    if App.currentScreen and App.currentScreen.Update then
        App.currentScreen:Update(dt)
    end
end

function OnRenderUI()
    if App.currentScreen and App.currentScreen.Draw then
        App.currentScreen:Draw()
    end
end
