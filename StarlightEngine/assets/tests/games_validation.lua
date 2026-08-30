-- games_validation.lua
-- Orquestrador de testes de integridade e QA para os jogos do Fusion ENGINE

-- ============================================================================
-- 1. Mocks de Segurança para APIs Headless/CLI
-- ============================================================================
test = test or {
    describe = function(name, cb) 
        print("Running mock test: " .. name)
        pcall(cb)
    end,
    assert = function(cond, msg) if not cond then error(msg or "Assertion failed") end end,
    assertEqual = function(v1, v2, msg) if v1 ~= v2 then error(msg or "Assertion failed: not equal") end end,
    save_screenshot = function() return true end,
    compare_screenshot = function() return true end
}

window = window or {
    get_width = function() return 1280 end,
    get_height = function() return 720 end
}

Save = Save or {
    _data = {},
    read = function(key, default) 
        if Save._data[key] ~= nil then return Save._data[key] end
        return default 
    end,
    write = function(key, val) Save._data[key] = val end,
    flush = function() end
}

gfx = gfx or {
    draw_rect = function() end,
    draw_sprite = function() end,
    draw_sprite_clean = function() end,
    load_texture = function() return 42 end,
    draw_text = function() end,
    load_font = function() return 100 end,
    draw_line = function() end
}

audio = audio or {
    play_sound = function() end,
    load_sound = function() return 7 end,
    play_music = function() end,
    stop_music = function() end,
    play_note = function() end,
    fm_note = function() end,
    beep = function() end,
    set_envelope = function() end,
    play = function() return 1 end,
    play_3d = function() end,
    set_volume = function() end,
    beep3d = function() end,
    play_note_3d = function() end,
    set_low_pass = function() end
}

Input = Input or {
    getAxis = function() return 0 end,
    getKey = function() return false end,
    vibrate = function() end
}

input = input or {
    is_just_pressed = function() return false end,
    is_pressed = function() return false end,
    is_down = function() return false end,
    vibrate = function() end,
    get_mouse_pos = function() return 0, 0 end,
    get_mouse_x = function() return 0 end,
    get_mouse_y = function() return 0 end,
    get_axis = function() return 0 end,
    is_gamepad_down = function() return false end,
    is_mouse_down = function() return false end,
    get_scroll = function() return 0 end
}

camera = camera or {
    set_pos = function() end,
    look_at = function() end
}

time = time or {
    get_time = function() return 1.0 end
}

local mockReverbZones = {}

Engine = Engine or {
    log = function(...) end,
    log_info = function(msg) print("[INFO] " .. tostring(msg)) end,
    log_warn = function(msg) print("[WARN] " .. tostring(msg)) end,
    log_error = function(msg) print("[ERROR] " .. tostring(msg)) end,
    spawn = function() return 1 end,
    destroy = function() end,
    set_pos = function() end,
    get_pos = function() return 0, 0, 0 end,
    set_scale = function() end,
    rotate = function() end,
    add_reverb_zone = function(id, min, max, factor) mockReverbZones[id] = { min = min, max = max, factor = factor, active = true } end,
    remove_reverb_zone = function(id) if mockReverbZones[id] then mockReverbZones[id].active = false end end,
    set_reverb_zone_factor = function(id, factor) if mockReverbZones[id] then mockReverbZones[id].factor = factor end end,
    set_reverb_zone_bounds = function(id, min, max) if mockReverbZones[id] then mockReverbZones[id].min = min; mockReverbZones[id].max = max end end,
    get_reverb_zone = function(id) local z = mockReverbZones[id]; if z then return z.min, z.max, z.factor, z.active else return 0, 0, 0, false end end,
    has_reverb_zone = function(id) local z = mockReverbZones[id]; return z and z.active or false end,
    set_bloom = function() end,
    set_exposure = function() end
}

file = file or {
    read = function(path) return "" end,
    write = function(path, content) return true end,
    exists = function(path) return false end
}

Physics2D = Physics2D or {
    raycast = function() return true, 40, nil end,
    aabbQuery = function() return { {} } end,
    CheckAABB = function() return false end,
    CheckCircle = function() return false end,
    PointInRect = function() return false end,
    RayCircle = function() return false end,
    SegmentIntersect = function() return false end,
    CircleVsAABB = function() return false end,
    Overlaps = function() return false end,
    CircleVsAABB_MTV = function() return false end,
    AABBs_MTV = function() return false end
}

VFX = VFX or {
    emit = function() end,
    burst = function() end
}

vfx = vfx or {
    emit = function() end,
    burst = function() end
}

Audio = Audio or {
    sequence = function() end
}

function load_game_script(path, replacements)
    local f = io.open(path, "r")
    if not f then return nil, "Nao foi possivel abrir o arquivo: " .. path end
    local code = f:read("*all")
    f:close()
    
    if replacements then
        for target, repl in pairs(replacements) do
            code = code:gsub(target, repl)
        end
    end
    
    local chunk, err = load(code, "@" .. path)
    if not chunk then return nil, err end
    return chunk
end

-- Estatísticas consolidadas de testes
local stats = {
    total = 0,
    passed = 0,
    failed = 0,
    suites = {}
}

-- Override das funções de teste para acumular métricas
local origDescribe = test.describe
local origAssert = test.assert
local origAssertEqual = test.assertEqual

local currentSuiteName = ""
test.describe = function(name, cb)
    currentSuiteName = name
    stats.suites[name] = { passed = 0, failed = 0, total = 0, errors = {} }
    origDescribe(name, function()
        local ok, err = pcall(cb)
        if not ok then
            stats.failed = stats.failed + 1
            stats.suites[currentSuiteName].failed = stats.suites[currentSuiteName].failed + 1
            table.insert(stats.suites[currentSuiteName].errors, tostring(err))
            Engine.log_error("Erro na suite '" .. name .. "': " .. tostring(err))
        end
    end)
end

test.assert = function(cond, msg)
    stats.total = stats.total + 1
    stats.suites[currentSuiteName].total = stats.suites[currentSuiteName].total + 1
    
    local ok, err = pcall(origAssert, cond, msg)
    if ok and cond then
        stats.passed = stats.passed + 1
        stats.suites[currentSuiteName].passed = stats.suites[currentSuiteName].passed + 1
    else
        stats.failed = stats.failed + 1
        stats.suites[currentSuiteName].failed = stats.suites[currentSuiteName].failed + 1
        local errMsg = msg or "Assercao falhou"
        table.insert(stats.suites[currentSuiteName].errors, errMsg)
    end
end

test.assertEqual = function(v1, v2, msg)
    stats.total = stats.total + 1
    stats.suites[currentSuiteName].total = stats.suites[currentSuiteName].total + 1
    
    local ok, err = pcall(origAssertEqual, v1, v2, msg)
    if ok and (v1 == v2) then
        stats.passed = stats.passed + 1
        stats.suites[currentSuiteName].passed = stats.suites[currentSuiteName].passed + 1
    else
        stats.failed = stats.failed + 1
        stats.suites[currentSuiteName].failed = stats.suites[currentSuiteName].failed + 1
        local errMsg = (msg or "Assercao de igualdade falhou") .. " (Esperado: " .. tostring(v1) .. ", Obtido: " .. tostring(v2) .. ")"
        table.insert(stats.suites[currentSuiteName].errors, errMsg)
    end
end

-- ============================================================================
-- 2. Carregar Componentes Base (core.lua / sba_bridge.lua)
-- ============================================================================
Engine.log_info("AutomationSystem: Carregando infraestrutura SBA em Lua...")
dofile("assets/scripts/core.lua")
dofile("assets/scripts/sba_bridge.lua")

-- ============================================================================
-- 3. Executar as Suites de Testes de Cada Jogo
-- ============================================================================
local gameTests = {
    { name = "Pong Game", file = "assets/tests/games/pong_test.lua" },
    { name = "Snake Game", file = "assets/tests/games/snake_test.lua" },
    { name = "Tetris Game", file = "assets/tests/games/tetris_test.lua" },
    { name = "CapitalOdyssey", file = "assets/tests/games/capital_odyssey_test.lua" },
    { name = "FusionFight", file = "assets/tests/games/fusion_fight_test.lua" },
    { name = "GodHand 3D", file = "assets/tests/games/god_hand_test.lua" },
    { name = "Regressao Visual", file = "assets/tests/games/visual_regression_test.lua" }
}

for _, gt in ipairs(gameTests) do
    Engine.log_info("AutomationSystem: Iniciando testes do " .. gt.name .. "...")
    local ok, err = pcall(dofile, gt.file)
    if not ok then
        Engine.log_error("Falha critica ao executar o arquivo de teste '" .. gt.file .. "': " .. tostring(err))
        stats.failed = stats.failed + 1
        if not stats.suites[gt.name] then
            stats.suites[gt.name] = { passed = 0, failed = 1, total = 1, errors = { tostring(err) } }
        else
            table.insert(stats.suites[gt.name].errors, tostring(err))
        end
    end
end-- ============================================================================
-- 4. Gerar Relatório Consolidado (system_health_report.md)
-- ============================================================================
Engine.log_info("AutomationSystem: Gerando relatorio consolidado de saude do sistema...")
local reportPath = "d:/Projetos/Fusion ENGINE/system_health_report.md"
local f = io.open(reportPath, "w")
if f then
    -- Obter resultados C++
    local cppResults = {}
    if test and test.getCppResults then
        cppResults = test.getCppResults()
    end
    
    local cppTotal = 0
    local cppPassed = 0
    local cppFailed = 0
    local cppFailDetails = {}
    for name, success in pairs(cppResults) do
        cppTotal = cppTotal + 1
        if success then
            cppPassed = cppPassed + 1
        else
            cppFailed = cppFailed + 1
            table.insert(cppFailDetails, name)
        end
    end
    
    f:write("# Relatorio de Saude do Sistema (QA Report - Fusion ENGINE)\n\n")
    f:write("Gerado automaticamente em: " .. os.date("%Y-%m-%d %H:%M:%S") .. "\n\n")
    
    f:write("## Resumo do Status\n\n")
    local totalErrors = stats.failed + cppFailed
    local statusStr = (totalErrors == 0) and "🟢 SISTEMA INTEGRO (PASSOU SEM ERROS)" or "🔴 FALHAS DETECTADAS NO SISTEMA"
    f:write("- **Status Geral**: " .. statusStr .. "\n")
    f:write("- **Total de Assercoes (Jogos)**: " .. stats.total .. "\n")
    f:write("- **Assercoes Aprovadas (Jogos)**: " .. stats.passed .. "\n")
    f:write("- **Assercoes Reprovadas (Jogos)**: " .. stats.failed .. "\n")
    f:write("- **Total de Testes da Engine (C++)**: " .. cppTotal .. "\n")
    f:write("- **Testes da Engine Aprovados**: " .. cppPassed .. "\n")
    f:write("- **Testes da Engine Reprovados**: " .. cppFailed .. "\n\n")
    
    f:write("## Detalhamento da Game Engine (Subsistemas C++)\n\n")
    f:write("| Subsistema C++ | Status |\n")
    f:write("| :--- | :---: |\n")
    for name, success in pairs(cppResults) do
        local sStatus = success and "🟢 OK (PASSOU)" or "🔴 FALHOU"
        f:write("| " .. name .. " | " .. sStatus .. " |\n")
    end
    f:write("\n")
    
    f:write("## Detalhamento dos Jogos (Suites Lua)\n\n")
    f:write("| Suite de Teste | Aprovados | Falhas | Total | Status |\n")
    f:write("| :--- | :---: | :---: | :---: | :---: |\n")
    
    for sName, sData in pairs(stats.suites) do
        local sStatus = (sData.failed == 0) and "🟢 OK" or "🔴 ERRO"
        f:write("| " .. sName .. " | " .. sData.passed .. " | " .. sData.failed .. " | " .. sData.total .. " | " .. sStatus .. " |\n")
    end
    f:write("\n")
    
    if totalErrors > 0 then
        f:write("## Detalhes dos Erros Encontrados\n\n")
        
        if cppFailed > 0 then
            f:write("### Game Engine (C++)\n")
            for _, name in ipairs(cppFailDetails) do
                f:write("- ❌ O teste de integridade da engine '" .. name .. "' falhou.\n")
            end
            f:write("\n")
        end
        
        for sName, sData in pairs(stats.suites) do
            if #sData.errors > 0 then
                f:write("### " .. sName .. "\n")
                for _, err in ipairs(sData.errors) do
                    f:write("- ❌ " .. err .. "\n")
                end
                f:write("\n")
            end
        end
    end
    
    f:close()
    Engine.log_info("AutomationSystem: Relatorio de saude do sistema gravado em '" .. reportPath .. "'.")
    
    -- Legacy support file
    local fLegacy = io.open("d:/Projetos/Fusion ENGINE/games_health_report.md", "w")
    if fLegacy then
        fLegacy:write("# Relatorio de Saude dos Jogos (QA Report - Fusion ENGINE)\n\n")
        fLegacy:write("Consulte o relatorio unificado completo em: [system_health_report.md](file:///d:/Projetos/Fusion%20ENGINE/system_health_report.md)\n")
        fLegacy:close()
    end
else
    Engine.log_error("AutomationSystem: Nao foi possivel criar o arquivo de relatorio em '" .. reportPath .. "'.")
end
