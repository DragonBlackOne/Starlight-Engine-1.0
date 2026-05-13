-- core.lua
-- Starlight Engine Core Standard Library
-- Automatically loaded by the engine before game scripts

Engine.log("Loading Starlight Standard Library (core.lua)...")

-- ============================================================================
-- Object-Oriented Framework (Class Factory)
-- ============================================================================
function Class(base)
    local c = {}
    if type(base) == 'table' then
        for i,v in pairs(base) do c[i] = v end
        c._base = base
    end
    c.__index = c
    local mt = {}
    mt.__call = function(class_tbl, ...)
        local obj = {}
        setmetatable(obj, c)
        if c.Init then c.Init(obj, ...) end
        return obj
    end
    c.Init = c.Init or function() end
    setmetatable(c, mt)
    return c
end

-- ============================================================================
-- MathX
-- ============================================================================
MathX = {}
function MathX.clamp(val, min, max) 
    return math.max(min, math.min(val, max)) 
end

function MathX.lerp(a, b, t) 
    return a + (b - a) * t 
end

function MathX.sign(x) 
    return x < 0 and -1 or (x > 0 and 1 or 0) 
end

function MathX.distance(x1, y1, x2, y2)
    local dx = x2 - x1
    local dy = y2 - y1
    return math.sqrt(dx * dx + dy * dy)
end

-- ============================================================================
-- Physics2D
-- ============================================================================
Physics2D = {}
function Physics2D.CheckAABB(x1, y1, w1, h1, x2, y2, w2, h2)
    return x1 < x2 + w2 and
           x1 + w1 > x2 and
           y1 < y2 + h2 and
           y1 + h1 > y2
end

function Physics2D.CheckCircle(x1, y1, r1, x2, y2, r2)
    local dist = MathX.distance(x1, y1, x2, y2)
    return dist < (r1 + r2)
end

-- ============================================================================
-- Timer System
-- ============================================================================
Timer = { timers = {} }

function Timer.after(seconds, callback)
    table.insert(Timer.timers, {
        time = seconds,
        callback = callback
    })
end

function Timer.update(dt)
    for i = #Timer.timers, 1, -1 do
        local t = Timer.timers[i]
        t.time = t.time - dt
        if t.time <= 0 then
            t.callback()
            table.remove(Timer.timers, i)
        end
    end
end

Engine.log("Starlight Standard Library Loaded Successfully.")
