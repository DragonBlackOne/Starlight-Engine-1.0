-- ============================================================================
-- core.lua — Starlight Engine Standard Library v2.0
-- Automatically loaded by the engine before any game script.
-- ============================================================================
Engine.log("Loading Starlight Standard Library (core.lua)...")

-- ============================================================================
-- Object-Oriented Framework (Class Factory with Inheritance)
-- ============================================================================
function Class(base)
    local c = {}
    if type(base) == 'table' then
        for k, v in pairs(base) do c[k] = v end
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
    c.is_a = function(self, klass)
        local m = getmetatable(self)
        while m do
            if m == klass then return true end
            m = m._base
        end
        return false
    end
    setmetatable(c, mt)
    return c
end

-- ============================================================================
-- MathX — Extended Math Utilities
-- ============================================================================
MathX = {}

function MathX.clamp(val, lo, hi)
    return math.max(lo, math.min(val, hi))
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

function MathX.distance3D(x1, y1, z1, x2, y2, z2)
    local dx = x2 - x1
    local dy = y2 - y1
    local dz = z2 - z1
    return math.sqrt(dx*dx + dy*dy + dz*dz)
end

function MathX.smoothstep(edge0, edge1, x)
    local t = MathX.clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)
end

function MathX.remap(value, fromLo, fromHi, toLo, toHi)
    return toLo + (value - fromLo) * (toHi - toLo) / (fromHi - fromLo)
end

function MathX.random_range(lo, hi)
    return lo + math.random() * (hi - lo)
end

function MathX.normalize2D(x, y)
    local len = math.sqrt(x*x + y*y)
    if len == 0 then return 0, 0 end
    return x / len, y / len
end

function MathX.angle(x1, y1, x2, y2)
    return math.atan(y2 - y1, x2 - x1)
end

function MathX.wrap(value, lo, hi)
    local range = hi - lo
    return lo + (value - lo) % range
end

-- ============================================================================
-- Physics2D — Extended 2D Collision Detection
-- ============================================================================
Physics2D = {}

function Physics2D.CheckAABB(x1, y1, w1, h1, x2, y2, w2, h2)
    return x1 < x2 + w2 and x1 + w1 > x2 and y1 < y2 + h2 and y1 + h1 > y2
end

function Physics2D.CheckCircle(x1, y1, r1, x2, y2, r2)
    return MathX.distance(x1, y1, x2, y2) < (r1 + r2)
end

function Physics2D.PointInRect(px, py, rx, ry, rw, rh)
    return px >= rx and px <= rx + rw and py >= ry and py <= ry + rh
end

function Physics2D.RayCircle(ox, oy, dx, dy, cx, cy, cr)
    local fx, fy = ox - cx, oy - cy
    local a = dx*dx + dy*dy
    local b = 2 * (fx*dx + fy*dy)
    local c = fx*fx + fy*fy - cr*cr
    local disc = b*b - 4*a*c
    if disc < 0 then return false, nil end
    disc = math.sqrt(disc)
    local t = (-b - disc) / (2 * a)
    if t >= 0 then return true, t end
    t = (-b + disc) / (2 * a)
    if t >= 0 then return true, t end
    return false, nil
end

-- ============================================================================
-- Timer System v2 — One-shot, Repeating, Cancelable
-- ============================================================================
Timer = { _timers = {}, _nextId = 1 }

function Timer.after(seconds, callback)
    local id = Timer._nextId
    Timer._nextId = Timer._nextId + 1
    table.insert(Timer._timers, {
        id = id,
        time = seconds,
        callback = callback,
        repeating = false,
    })
    return id
end

function Timer.every(seconds, callback)
    local id = Timer._nextId
    Timer._nextId = Timer._nextId + 1
    table.insert(Timer._timers, {
        id = id,
        time = seconds,
        interval = seconds,
        callback = callback,
        repeating = true,
    })
    return id
end

function Timer.cancel(id)
    for i = #Timer._timers, 1, -1 do
        if Timer._timers[i].id == id then
            table.remove(Timer._timers, i)
            return true
        end
    end
    return false
end

function Timer.update(dt)
    for i = #Timer._timers, 1, -1 do
        local t = Timer._timers[i]
        t.time = t.time - dt
        if t.time <= 0 then
            t.callback()
            if t.repeating then
                t.time = t.time + t.interval
            else
                table.remove(Timer._timers, i)
            end
        end
    end
end

-- ============================================================================
-- Color — HSV, pulse, and blending utilities
-- ============================================================================
Color = {}

function Color.hsv(h, s, v)
    local i = math.floor(h * 6)
    local f = h * 6 - i
    local p = v * (1 - s)
    local q = v * (1 - f * s)
    local t = v * (1 - (1 - f) * s)
    i = i % 6
    if i == 0 then return v, t, p end
    if i == 1 then return q, v, p end
    if i == 2 then return p, v, t end
    if i == 3 then return p, q, v end
    if i == 4 then return t, p, v end
    return v, p, q
end

function Color.pulse(r, g, b, time, speed, intensity)
    local factor = 1.0 + math.sin(time * (speed or 5)) * (intensity or 0.3)
    return r * factor, g * factor, b * factor
end

function Color.lerpRGB(r1, g1, b1, r2, g2, b2, t)
    return MathX.lerp(r1, r2, t), MathX.lerp(g1, g2, t), MathX.lerp(b1, b2, t)
end

-- ============================================================================
-- ScreenShake — Reusable camera shake for 2D games
-- ============================================================================
ScreenShake = { _time = 0, _intensity = 0 }

function ScreenShake.trigger(intensity, duration)
    ScreenShake._intensity = intensity
    ScreenShake._time = duration
end

function ScreenShake.update(dt)
    if ScreenShake._time > 0 then
        ScreenShake._time = ScreenShake._time - dt
    else
        ScreenShake._intensity = 0
    end
end

function ScreenShake.getOffset()
    if ScreenShake._intensity > 0 then
        return (math.random() * 2 - 1) * ScreenShake._intensity,
               (math.random() * 2 - 1) * ScreenShake._intensity
    end
    return 0, 0
end

-- ============================================================================
-- ValueTween — Animate arbitrary values (for 2D games without Entity)
-- ============================================================================
ValueTween = { _tweens = {} }

function ValueTween.to(target, key, endVal, duration, easingName)
    local easeFn = (Easing and Easing[easingName or "linear"]) or function(t) return t end
    table.insert(ValueTween._tweens, {
        target = target,
        key = key,
        startVal = target[key],
        endVal = endVal,
        duration = duration,
        elapsed = 0,
        easeFn = easeFn,
    })
end

function ValueTween.update(dt)
    for i = #ValueTween._tweens, 1, -1 do
        local tw = ValueTween._tweens[i]
        tw.elapsed = tw.elapsed + dt
        local t = MathX.clamp(tw.elapsed / tw.duration, 0, 1)
        tw.target[tw.key] = MathX.lerp(tw.startVal, tw.endVal, tw.easeFn(t))
        if t >= 1.0 then table.remove(ValueTween._tweens, i) end
    end
end

Engine.log("Starlight Standard Library Loaded Successfully.")
