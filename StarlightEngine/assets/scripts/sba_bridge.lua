-- ============================================================================
-- sba_bridge.lua — Starlight Bridge API v2.0
-- The high-level Game Development SDK for the Fusion Engine.
-- ============================================================================

-- ============================================================================
-- Entity — Object-Oriented Wrapper for ECS Entities
-- ============================================================================
Entity = Class()

function Entity:Init(tag, x, y, z)
    self._id = Engine.spawn(tag or "Entity")
    self._tag = tag or "Entity"
    self._alive = true
    self.x = x or 0
    self.y = y or 0
    self.z = z or 0
    Engine.set_pos(self._id, self.x, self.y, self.z)
end

function Entity:id() return self._id end
function Entity:tag() return self._tag end
function Entity:isAlive() return self._alive end

function Entity:setPos(x, y, z)
    self.x = x; self.y = y; self.z = z
    Engine.set_pos(self._id, x, y, z)
    return self
end

function Entity:getPos()
    local px, py, pz = Engine.get_pos(self._id)
    if px then self.x, self.y, self.z = px, py, pz end
    return self.x, self.y, self.z
end

function Entity:setColor(r, g, b)
    Engine.set_color(self._id, r, g, b)
    return self
end

function Entity:setScale(x, y, z)
    Engine.set_scale(self._id, x, y or x, z or x)
    return self
end

function Entity:setMaterial(metallic, roughness)
    Engine.set_material(self._id, metallic, roughness)
    return self
end

function Entity:setPBR(metallic, roughness, ao)
    Engine.set_pbr(self._id, metallic, roughness, ao or 1.0)
    return self
end

function Entity:move(dx, dy, dz)
    self.x = self.x + (dx or 0)
    self.y = self.y + (dy or 0)
    self.z = self.z + (dz or 0)
    Engine.set_pos(self._id, self.x, self.y, self.z)
    return self
end

function Entity:destroy()
    if self._alive then
        Engine.destroy(self._id)
        self._alive = false
    end
end

-- ============================================================================
-- Light — Wrapper for PointLightComponent
-- ============================================================================
Light = Class()

function Light:Init(x, y, z, r, g, b, intensity)
    self._id = Engine.spawn_light(x, y, z, r or 1, g or 1, b or 1, intensity or 500)
    self._alive = true
end

function Light:id() return self._id end

function Light:setColor(r, g, b)
    Engine.set_light_color(self._id, r, g, b)
    return self
end

function Light:setIntensity(i)
    Engine.set_light_intensity(self._id, i)
    return self
end

-- ============================================================================
-- Easing Functions
-- ============================================================================
Easing = {}
function Easing.linear(t) return t end
function Easing.easeInQuad(t) return t * t end
function Easing.easeOutQuad(t) return t * (2 - t) end
function Easing.easeInOutQuad(t) return t < 0.5 and 2*t*t or -1+(4-2*t)*t end
function Easing.easeInCubic(t) return t * t * t end
function Easing.easeOutCubic(t) local u = t-1; return u*u*u + 1 end
function Easing.easeInOutCubic(t) return t < 0.5 and 4*t*t*t or (t-1)*(2*t-2)*(2*t-2)+1 end
function Easing.easeOutBounce(t)
    if t < 1/2.75 then return 7.5625*t*t
    elseif t < 2/2.75 then t=t-1.5/2.75; return 7.5625*t*t+0.75
    elseif t < 2.5/2.75 then t=t-2.25/2.75; return 7.5625*t*t+0.9375
    else t=t-2.625/2.75; return 7.5625*t*t+0.984375 end
end
function Easing.easeOutElastic(t)
    if t == 0 or t == 1 then return t end
    return (2 ^ (-10*t)) * math.sin((t-0.075)*2*math.pi/0.3) + 1
end

-- ============================================================================
-- Tween System — Animate any Entity property over time
-- ============================================================================
Tween = { _active = {} }

function Tween.to(entity, props, duration, easingName, onComplete)
    local easeFn = Easing[easingName or "linear"] or Easing.linear
    local startValues = {}
    
    -- Capture starting values
    if props.x or props.y or props.z then
        local cx, cy, cz = entity:getPos()
        startValues.x = cx; startValues.y = cy; startValues.z = cz
    end
    if props.scaleX or props.scaleY or props.scaleZ then
        -- We track scale internally for tweening
        startValues.scaleX = entity._scaleX or 1
        startValues.scaleY = entity._scaleY or 1
        startValues.scaleZ = entity._scaleZ or 1
    end
    
    table.insert(Tween._active, {
        entity = entity,
        props = props,
        startValues = startValues,
        duration = duration,
        elapsed = 0,
        easeFn = easeFn,
        onComplete = onComplete,
    })
end

function Tween.update(dt)
    for i = #Tween._active, 1, -1 do
        local tw = Tween._active[i]
        tw.elapsed = tw.elapsed + dt
        local t = MathX.clamp(tw.elapsed / tw.duration, 0, 1)
        local eased = tw.easeFn(t)
        local ent = tw.entity
        
        -- Interpolate position
        local needsPos = false
        local nx, ny, nz = ent.x, ent.y, ent.z
        if tw.props.x then nx = MathX.lerp(tw.startValues.x, tw.props.x, eased); needsPos = true end
        if tw.props.y then ny = MathX.lerp(tw.startValues.y, tw.props.y, eased); needsPos = true end
        if tw.props.z then nz = MathX.lerp(tw.startValues.z, tw.props.z, eased); needsPos = true end
        if needsPos then ent:setPos(nx, ny, nz) end
        
        -- Interpolate scale
        local needsScale = false
        local sx = ent._scaleX or 1
        local sy = ent._scaleY or 1
        local sz = ent._scaleZ or 1
        if tw.props.scaleX then sx = MathX.lerp(tw.startValues.scaleX, tw.props.scaleX, eased); needsScale = true end
        if tw.props.scaleY then sy = MathX.lerp(tw.startValues.scaleY, tw.props.scaleY, eased); needsScale = true end
        if tw.props.scaleZ then sz = MathX.lerp(tw.startValues.scaleZ, tw.props.scaleZ, eased); needsScale = true end
        if needsScale then
            ent._scaleX = sx; ent._scaleY = sy; ent._scaleZ = sz
            ent:setScale(sx, sy, sz)
        end
        
        if t >= 1.0 then
            if tw.onComplete then tw.onComplete(ent) end
            table.remove(Tween._active, i)
        end
    end
end

-- ============================================================================
-- Event Bus — Publish / Subscribe
-- ============================================================================
Events = { _listeners = {} }

function Events.on(eventName, callback)
    if not Events._listeners[eventName] then
        Events._listeners[eventName] = {}
    end
    table.insert(Events._listeners[eventName], callback)
end

function Events.emit(eventName, data)
    local listeners = Events._listeners[eventName]
    if listeners then
        for _, cb in ipairs(listeners) do
            cb(data)
        end
    end
end

function Events.clear(eventName)
    if eventName then
        Events._listeners[eventName] = nil
    else
        Events._listeners = {}
    end
end

-- ============================================================================
-- Scene Manager — State Machine
-- ============================================================================
Scene = { _scenes = {}, _current = nil, _currentName = "" }

function Scene.register(name, handlers)
    Scene._scenes[name] = handlers
end

function Scene.switch(name)
    if Scene._current and Scene._current.onExit then
        Scene._current.onExit()
    end
    Scene._current = Scene._scenes[name]
    Scene._currentName = name
    if Scene._current and Scene._current.onEnter then
        Scene._current.onEnter()
    end
end

function Scene.current() return Scene._currentName end

function Scene.update(dt)
    if Scene._current and Scene._current.onUpdate then
        Scene._current.onUpdate(dt)
    end
end

function Scene.renderUI()
    if Scene._current and Scene._current.onRenderUI then
        Scene._current.onRenderUI()
    end
end

-- ============================================================================
-- Coroutine Runner — Async Sequences
-- ============================================================================
Coroutine = { _routines = {} }

function Coroutine.start(fn)
    local co = coroutine.create(fn)
    table.insert(Coroutine._routines, { co = co, waitTime = 0 })
end

function Coroutine.wait(seconds)
    coroutine.yield(seconds)
end

function Coroutine.update(dt)
    for i = #Coroutine._routines, 1, -1 do
        local r = Coroutine._routines[i]
        r.waitTime = r.waitTime - dt
        if r.waitTime <= 0 then
            local ok, result = coroutine.resume(r.co)
            if not ok then
                Engine.log_error("Coroutine error: " .. tostring(result))
                table.remove(Coroutine._routines, i)
            elseif coroutine.status(r.co) == "dead" then
                table.remove(Coroutine._routines, i)
            else
                r.waitTime = result or 0
            end
        end
    end
end

-- ============================================================================
-- Legacy Compatibility Layer
-- ============================================================================
function Object(tag, x, y, z)
    local e = Engine.spawn(tag)
    Engine.set_pos(e, x or 0, y or 0, z or 0)
    return e
end

function SetPos(e, x, y, z) Engine.set_pos(e, x, y, z) end
function SetColor(e, r, g, b) Engine.set_color(e, r, g, b) end
function SetScale(e, x, y, z) Engine.set_scale(e, x, y, z) end
function Say(msg) Engine.log(tostring(msg)) end

function IsDown(key) return Engine.is_down(key) end

function Distance(e1, e2)
    local x1, y1, z1 = Engine.get_pos(e1)
    local x2, y2, z2 = Engine.get_pos(e2)
    if x1 and x2 then return MathX.distance3D(x1, y1, z1, x2, y2, z2) end
    return 999999
end

function Sound(freq, duration)
    Engine.play_sound("assets/audio/beep.wav")
end

Say("SBA v2.0: Bridge initialized — Entity, Tween, Scene, Events, Coroutine ready.")
