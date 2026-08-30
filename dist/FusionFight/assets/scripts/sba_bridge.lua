-- ============================================================================
-- sba_bridge.lua — Starlight Bridge API v7.0
-- The high-level Game Development SDK for the Fusion Engine.
-- ============================================================================

-- ============================================================================
-- Entity v3 — Full OO Wrapper with tag registry, components, pooling
-- ============================================================================
Entity = Class()
Entity._registry = {}
Entity._all = {}

function Entity:Init(tag, x, y, z)
    self._id = Engine.spawn(tag or "Entity")
    self._tag = tag or "Entity"
    self._alive = true; self._visible = true
    self.x = x or 0; self.y = y or 0; self.z = z or 0
    self._scaleX=1; self._scaleY=1; self._scaleZ=1
    self._data = {} -- custom component data
    self._components = {} -- Lua ECS Components
    Engine.set_pos(self._id, self.x, self.y, self.z)
    if not Entity._registry[self._tag] then Entity._registry[self._tag]={} end
    Entity._registry[self._tag][#Entity._registry[self._tag]+1]=self
    Entity._all[#Entity._all+1]=self
end

function Entity:id() return self._id end
function Entity:tag() return self._tag end
function Entity:isAlive() return self._alive end

-- Transform
function Entity:setPos(x,y,z) self.x=x;self.y=y;self.z=z;Engine.set_pos(self._id,x,y,z);return self end
function Entity:getPos() local px,py,pz=Engine.get_pos(self._id);if px then self.x,self.y,self.z=px,py,pz end;return self.x,self.y,self.z end
function Entity:move(dx,dy,dz) self.x=self.x+(dx or 0);self.y=self.y+(dy or 0);self.z=self.z+(dz or 0);Engine.set_pos(self._id,self.x,self.y,self.z);return self end
function Entity:setScale(x,y,z) self._scaleX=x;self._scaleY=y or x;self._scaleZ=z or x;Engine.set_scale(self._id,self._scaleX,self._scaleY,self._scaleZ);return self end
function Entity:setRotation(rx,ry,rz) Engine.rotate(self._id,rx or 0,ry or 0,rz or 0);return self end
function Entity:rotate(rx,ry,rz) Engine.rotate(self._id,rx or 0,ry or 0,rz or 0);return self end

-- Rendering
function Entity:setColor(r,g,b) self._r=r;self._g=g;self._b=b;Engine.set_color(self._id,r,g,b);return self end
function Entity:setMaterial(m,r) Engine.set_material(self._id,m,r);return self end
function Entity:setPBR(m,r,ao) Engine.set_pbr(self._id,m,r,ao or 1.0);return self end
function Entity:setVisible(v)
    self._visible=v
    if v then Engine.set_scale(self._id,self._scaleX,self._scaleY,self._scaleZ)
    else Engine.set_scale(self._id,0,0,0) end; return self
end
function Entity:isVisible() return self._visible end
function Entity:setAnim(anim) self._anim=anim; return self end

-- Update loops including Lua components
function Entity:update(dt)
    if self._anim then self._anim:update(dt) end
    for name, comp in pairs(self._components) do
        if comp.update then
            local ok, err = pcall(comp.update, comp, dt)
            if not ok then Engine.log_error("Error updating component " .. tostring(name) .. ": " .. tostring(err)) end
        end
    end
end

-- Draw loops including Lua components
function Entity:draw()
    if self._visible then
        if self._anim then
            local frame = self._anim:current()
            if frame then gfx.draw_sprite(self.x, self.y, self._scaleX*32, self._scaleY*32, frame, self._r or 1, self._g or 1, self._b or 1, 1.0) end
        end
        for name, comp in pairs(self._components) do
            if comp.draw then
                local ok, err = pcall(comp.draw, comp)
                if not ok then Engine.log_error("Error drawing component " .. tostring(name) .. ": " .. tostring(err)) end
            end
        end
    end
end

-- Custom data
function Entity:set(key,value) self._data[key]=value; return self end
function Entity:get(key,default) local v=self._data[key]; if v~=nil then return v end; return default end

-- Lua ECS Component Model
function Entity:addComponent(comp)
    if not comp or not comp.name then
        Engine.log_error("Cannot add an invalid component (missing name)")
        return self
    end
    if self._components[comp.name] then self:removeComponent(comp.name) end
    self._components[comp.name] = comp
    comp.entity = self
    if comp.onAdd then
        local ok, err = pcall(comp.onAdd, comp, self)
        if not ok then Engine.log_error("Error in component onAdd for " .. tostring(comp.name) .. ": " .. tostring(err)) end
    end
    return self
end

function Entity:getComponent(name)
    return self._components[name]
end

function Entity:removeComponent(name)
    local comp = self._components[name]
    if comp then
        if comp.onRemove then
            local ok, err = pcall(comp.onRemove, comp)
            if not ok then Engine.log_error("Error in component onRemove for " .. tostring(name) .. ": " .. tostring(err)) end
        end
        comp.entity = nil
        self._components[name] = nil
    end
    return self
end

-- Lifecycle
function Entity:destroy()
    if not self._alive then return end
    for name, _ in pairs(self._components) do self:removeComponent(name) end
    Engine.destroy(self._id); self._alive=false
    local list=Entity._registry[self._tag]
    if list then for i=#list,1,-1 do if list[i]==self then list[i]=list[#list];list[#list]=nil;break end end end
    for i=#Entity._all,1,-1 do if Entity._all[i]==self then Entity._all[i]=Entity._all[#Entity._all];Entity._all[#Entity._all]=nil;break end end
end

-- Reverb Zones
function Entity:addReverbZone(min, max, factor)
    Engine.add_reverb_zone(self._id, min, max, factor)
    return self
end

function Entity:removeReverbZone()
    Engine.remove_reverb_zone(self._id)
    return self
end

function Entity:setReverbZoneFactor(factor)
    Engine.set_reverb_zone_factor(self._id, factor)
    return self
end

function Entity:setReverbZoneBounds(min, max)
    Engine.set_reverb_zone_bounds(self._id, min, max)
    return self
end

function Entity:getReverbZone()
    return Engine.get_reverb_zone(self._id)
end

function Entity:hasReverbZone()
    return Engine.has_reverb_zone(self._id)
end

-- ============================================================================
-- Component — Base Class for Lua ECS Components
-- ============================================================================
Component = Class()
function Component:Init(name)
    self.name = name or "Component"
    self.entity = nil
end
function Component:onAdd(entity) end
function Component:onRemove() end
function Component:update(dt) end
function Component:draw() end

-- Static queries
function Entity.findByTag(tag) return Entity._registry[tag] or {} end
function Entity.countByTag(tag) local l=Entity._registry[tag]; return l and #l or 0 end
function Entity.destroyByTag(tag) local l=Entity._registry[tag]; if not l then return end; for i=#l,1,-1 do l[i]:destroy() end end
function Entity.all() return Entity._all end
function Entity.count() return #Entity._all end
function Entity.destroyAll() for i=#Entity._all,1,-1 do Entity._all[i]:destroy() end end

-- ============================================================================
-- Light v3
-- ============================================================================
Light = Class()
function Light:Init(x,y,z,r,g,b,intensity)
    self._id=Engine.spawn_light(x,y,z,r or 1,g or 1,b or 1,intensity or 500); self._alive=true
end
function Light:id() return self._id end
function Light:setColor(r,g,b) Engine.set_light_color(self._id,r,g,b);return self end
function Light:setIntensity(i) Engine.set_light_intensity(self._id,i);return self end
function Light:destroy() if self._alive then Engine.destroy(self._id);self._alive=false end end

-- ============================================================================
-- Easing v3 — 12 functions
-- ============================================================================
Easing = {}
function Easing.linear(t) return t end
function Easing.easeInQuad(t) return t*t end
function Easing.easeOutQuad(t) return t*(2-t) end
function Easing.easeInOutQuad(t) return t<0.5 and 2*t*t or -1+(4-2*t)*t end
function Easing.easeInCubic(t) return t*t*t end
function Easing.easeOutCubic(t) local u=t-1;return u*u*u+1 end
function Easing.easeInOutCubic(t) return t<0.5 and 4*t*t*t or (t-1)*(2*t-2)*(2*t-2)+1 end
function Easing.easeOutBounce(t)
    if t<1/2.75 then return 7.5625*t*t
    elseif t<2/2.75 then t=t-1.5/2.75;return 7.5625*t*t+0.75
    elseif t<2.5/2.75 then t=t-2.25/2.75;return 7.5625*t*t+0.9375
    else t=t-2.625/2.75;return 7.5625*t*t+0.984375 end
end
function Easing.easeOutElastic(t)
    if t==0 or t==1 then return t end; return (2^(-10*t))*math.sin((t-0.075)*2*math.pi/0.3)+1
end
function Easing.easeOutBack(t) local s=1.70158;t=t-1;return t*t*((s+1)*t+s)+1 end
function Easing.easeInBack(t) local s=1.70158;return t*t*((s+1)*t-s) end
function Easing.easeInOutBack(t)
    local s=1.70158*1.525; t=t*2
    if t<1 then return 0.5*(t*t*((s+1)*t-s)) else t=t-2;return 0.5*(t*t*((s+1)*t+s)+2) end
end

-- ============================================================================
-- Tween v3 — delay, cancel, chain, swap-and-pop
-- ============================================================================
Tween = { _active = {} }

function Tween.to(entity, props, duration, easingName, onComplete, delay)
    local easeFn = Easing[easingName or "linear"] or Easing.linear
    local sv = {}
    
    local cx, cy, cz = 0, 0, 0
    if entity.getPos then
        cx, cy, cz = entity:getPos()
    else
        cx, cy, cz = entity.x or 0, entity.y or 0, entity.z or 0
    end
    
    for k, v in pairs(props) do
        if type(v) == "number" then
            if k == "x" then sv.x = cx
            elseif k == "y" then sv.y = cy
            elseif k == "z" then sv.z = cz
            elseif k == "scaleX" then sv.scaleX = entity._scaleX or 1
            elseif k == "scaleY" then sv.scaleY = entity._scaleY or 1
            elseif k == "scaleZ" then sv.scaleZ = entity._scaleZ or 1
            elseif k == "r" then sv.r = entity._r or 1
            elseif k == "g" then sv.g = entity._g or 1
            elseif k == "b" then sv.b = entity._b or 1
            elseif k == "a" then sv.a = entity._a or 1
            else
                sv[k] = entity[k] or 0
            end
        end
    end
    
    Tween._active[#Tween._active+1] = {
        entity = entity,
        props = props,
        startValues = sv,
        duration = duration,
        elapsed = 0,
        delay = delay or 0,
        easeFn = easeFn,
        onComplete = onComplete,
        paused = false
    }
end

function Tween.sequence(entity, steps)
    if #steps==0 then return end
    local function run(idx)
        if idx>#steps then return end; local s=steps[idx]
        Tween.to(entity,s.props,s.duration,s.easing,function(e)
            if s.onComplete then s.onComplete(e) end; run(idx+1)
        end,s.delay)
    end; run(1)
end

function Tween.cancel(entity)
    local a=Tween._active; local i=1
    while i<=#a do if a[i].entity==entity then a[i]=a[#a];a[#a]=nil else i=i+1 end end
end
function Tween.cancelAll() Tween._active={} end
function Tween.pause(entity)
    for _, tw in ipairs(Tween._active) do
        if tw.entity == entity then
            tw.paused = true
        end
    end
end
function Tween.resume(entity)
    for _, tw in ipairs(Tween._active) do
        if tw.entity == entity then
            tw.paused = false
        end
    end
end
function Tween.group(steps)
    for _,s in ipairs(steps) do Tween.to(s.entity, s.props, s.duration, s.easing, s.onComplete, s.delay) end
end

function Tween.update(dt)
    local a = Tween._active
    local i = 1
    while i <= #a do
        local tw = a[i]
        if tw.paused then
            i = i + 1
        elseif tw.delay > 0 then
            tw.delay = tw.delay - dt
            i = i + 1
        else
            tw.elapsed = tw.elapsed + dt
            local t = MathX.clamp(tw.elapsed / tw.duration, 0, 1)
            local e = tw.easeFn(t)
            local ent = tw.entity
            
            local has_pos = false
            local nx, ny, nz = 0, 0, 0
            if ent.getPos then nx, ny, nz = ent:getPos() else nx, ny, nz = ent.x or 0, ent.y or 0, ent.z or 0 end
            
            local has_scale = false
            local nsx, nsy, nsz = ent._scaleX or 1, ent._scaleY or 1, ent._scaleZ or 1
            
            local has_color = false
            local nr, ng, nb = ent._r or 1, ent._g or 1, ent._b or 1
            
            local has_rotation = false
            local nrx, nry, nrz = 0, 0, 0
            
            for k, target_val in pairs(tw.props) do
                local start_val = tw.startValues[k]
                if start_val then
                    local val
                    if k:find("angle") or k:find("Angle") or k:find("rotation") or k:find("Rotation") or k:find("rot") then
                        local diff = (target_val - start_val) % 360
                        if diff > 180 then diff = diff - 360 end
                        val = (start_val + diff * e) % 360
                    else
                        val = MathX.lerp(start_val, target_val, e)
                    end
                    
                    if k == "x" then nx = val; has_pos = true
                    elseif k == "y" then ny = val; has_pos = true
                    elseif k == "z" then nz = val; has_pos = true
                    elseif k == "scaleX" then nsx = val; has_scale = true
                    elseif k == "scaleY" then nsy = val; has_scale = true
                    elseif k == "scaleZ" then nsz = val; has_scale = true
                    elseif k == "r" then nr = val; has_color = true
                    elseif k == "g" then ng = val; has_color = true
                    elseif k == "b" then nb = val; has_color = true
                    else
                        ent[k] = val
                        if k == "rotX" then nrx = val; has_rotation = true
                        elseif k == "rotY" then nry = val; has_rotation = true
                        elseif k == "rotZ" then nrz = val; has_rotation = true
                        elseif k == "rotation" then nrz = val; has_rotation = true
                        end
                    end
                end
            end
            
            if has_pos then
                if ent.setPos then ent:setPos(nx, ny, nz) else ent.x, ent.y, ent.z = nx, ny, nz end
            end
            if has_scale then
                if ent.setScale then ent:setScale(nsx, nsy, nsz) else ent._scaleX, ent._scaleY, ent._scaleZ = nsx, nsy, nsz end
            end
            if has_color then
                if ent.setColor then ent:setColor(nr, ng, nb) else ent._r, ent._g, ent._b = nr, ng, nb end
            end
            if has_rotation then
                if ent.setRotation then ent:setRotation(nrx, nry, nrz) end
            end
            
            if t >= 1 then
                if tw.onComplete then
                    local ok, err = pcall(tw.onComplete, ent)
                    if not ok then Engine.log_error("Tween onComplete error: " .. tostring(err)) end
                end
                a[i] = a[#a]
                a[#a] = nil
            else
                i = i + 1
            end
        end
    end
end

-- ============================================================================
-- Events v3 — once, off, priority
-- ============================================================================
Events = { _listeners = {} }
function Events.on(name, cb) if not Events._listeners[name] then Events._listeners[name]={} end; local l=Events._listeners[name]; l[#l+1]={fn=cb,once=false} end
function Events.once(name, cb) if not Events._listeners[name] then Events._listeners[name]={} end; local l=Events._listeners[name]; l[#l+1]={fn=cb,once=true} end
function Events.emit(name, data)
    local ls = Events._listeners[name]
    if not ls then return end
    local i = 1
    while i <= #ls do
        local ok, err = pcall(ls[i].fn, data)
        if not ok then Engine.log_error("Events.emit error in " .. tostring(name) .. ": " .. tostring(err)) end
        if ls[i].once then
            ls[i] = ls[#ls]
            ls[#ls] = nil
        else
            i = i + 1
        end
    end
end
function Events.off(name, cb) local ls=Events._listeners[name]; if not ls then return end; for i=#ls,1,-1 do if ls[i].fn==cb then ls[i]=ls[#ls];ls[#ls]=nil;return end end end
function Events.clear(name) if name then Events._listeners[name]=nil else Events._listeners={} end end

-- ============================================================================
-- Scene v4 — State machine with fade transitions
-- ============================================================================
Scene = { _scenes={}, _current=nil, _currentName="", _fadeAlpha=0, _targetScene=nil }
function Scene.register(name, handlers) Scene._scenes[name]=handlers end
function Scene.switch(name, data)
    if Scene._current and Scene._current.onExit then Scene._current.onExit() end
    Scene._current=Scene._scenes[name]; Scene._currentName=name
    if Scene._current and Scene._current.onEnter then Scene._current.onEnter(data) end
end
function Scene.fadeTo(name, duration, data)
    Scene._targetScene = { name=name, data=data }
    Tween.to(Scene, {_fadeAlpha=1}, (duration or 0.5), "linear", function()
        Scene.switch(Scene._targetScene.name, Scene._targetScene.data)
        Tween.to(Scene, {_fadeAlpha=0}, (duration or 0.5), "linear")
    end)
end
function Scene.current() return Scene._currentName end
function Scene.update(dt) if Scene._current and Scene._current.onUpdate then Scene._current.onUpdate(dt) end end
function Scene.renderUI()
    if Scene._current and Scene._current.onRenderUI then Scene._current.onRenderUI() end
    if Scene._fadeAlpha > 0 then
        local w,h = gfx.screen_width(), gfx.screen_height()
        gfx.draw_rect_alpha(0, 0, w, h, 0, 0, 0, Scene._fadeAlpha)
    end
end
function Scene.fixedUpdate(dt) if Scene._current and Scene._current.onFixedUpdate then Scene._current.onFixedUpdate(dt) end end

-- ============================================================================
-- Coroutine v3 — swap-and-pop
-- ============================================================================
Coroutine = { _routines = {} }
function Coroutine.start(fn) Coroutine._routines[#Coroutine._routines+1]={co=coroutine.create(fn),waitTime=0} end
function Coroutine.wait(s) coroutine.yield(s) end
function Coroutine.update(dt)
    local rs=Coroutine._routines; local i=1
    while i<=#rs do local r=rs[i]; r.waitTime=r.waitTime-dt
        if r.waitTime<=0 then local ok,res=coroutine.resume(r.co)
            if not ok then Engine.log_error("Coroutine error: "..tostring(res)); rs[i]=rs[#rs];rs[#rs]=nil
            elseif coroutine.status(r.co)=="dead" then rs[i]=rs[#rs];rs[#rs]=nil
            else r.waitTime=res or 0; i=i+1 end
        else i=i+1 end
    end
end

-- ============================================================================
-- Camera2D v3 — Follow target, zoom, bounds
-- ============================================================================
Camera2D = { x=0, y=0, zoom=1, targetEntity=nil, smoothing=5 }
function Camera2D.follow(entity, smoothing) Camera2D.targetEntity=entity; Camera2D.smoothing=smoothing or 5 end
function Camera2D.setZoom(z) Camera2D.zoom=z end
function Camera2D.update(dt)
    if Camera2D.targetEntity then
        local tx,ty=Camera2D.targetEntity.x,Camera2D.targetEntity.y
        Camera2D.x=MathX.lerp(Camera2D.x,tx,Camera2D.smoothing*dt)
        Camera2D.y=MathX.lerp(Camera2D.y,ty,Camera2D.smoothing*dt)
    end
end
function Camera2D.apply(x,y) return (x-Camera2D.x)*Camera2D.zoom, (y-Camera2D.y)*Camera2D.zoom end

-- ============================================================================
-- Input v3 — Abstraction layer with action mapping
-- ============================================================================
Input = { _actions = {}, _lastPressTimes = {}, _axes = {} }
function Input.map(action, ...) Input._actions[action]={...} end
function Input.isDown(action)
    local keys=Input._actions[action]
    if keys then for _,k in ipairs(keys) do if input.is_down(k) then return true end end; return false end
    return input.is_down(action)
end
function Input.isJustPressed(action)
    local keys=Input._actions[action]
    if keys then for _,k in ipairs(keys) do if input.is_just_pressed(k) then return true end end; return false end
    return input.is_just_pressed(action)
end
function Input.mouseY() return input.get_mouse_y() end
function Input.axis(name) return input.get_axis(name or "LeftX") end
function Input.isGamepadDown(btn) return input.is_gamepad_down(btn or "A") end
function Input.vibrate(l,r,ms) input.vibrate(l or 0.5, r or 0.5, ms or 100) end

-- Eixos Virtuais
function Input.mapAxis(axisName, negativeAction, positiveAction, gamepadAxis)
    Input._axes[axisName] = {
        neg = negativeAction,
        pos = positiveAction,
        gp = gamepadAxis
    }
end

function Input.getAxis(axisName)
    local cfg = Input._axes[axisName]
    if not cfg then return 0 end
    
    if cfg.gp then
        local val = Input.axis(cfg.gp)
        if math.abs(val) > 0.1 then return val end
    end
    
    local val = 0
    if Input.isDown(cfg.neg) then val = val - 1 end
    if Input.isDown(cfg.pos) then val = val + 1 end
    return val
end

-- Detecção de Double-Tap
function Input.checkDoubleTap(action, window)
    window = window or 0.25
    if Input.isJustPressed(action) then
        local now = time.get_time()
        local last = Input._lastPressTimes[action] or 0
        Input._lastPressTimes[action] = now
        if now - last <= window then
            Input._lastPressTimes[action] = 0
            return true
        end
    end
    return false
end

-- ============================================================================
-- Audio v4 — Advanced Sound Controller
-- ============================================================================
Audio = {}
function Audio.play(path) audio.play_sound(path) end
function Audio.beep(f,d,t) audio.beep(f,d,t) end
function Audio.fm(f,d,a) audio.fm_note(f,d,a) end
function Audio.setFilter(c) audio.set_low_pass(c) end
function Audio.setADSR(a,d,s,r) audio.set_envelope(a,d,s,r) end
function Audio.kick() audio.beep(60, 0.1, 3); audio.set_envelope(0.001, 0.1, 0, 0.05) end
function Audio.snare() audio.beep(120, 0.1, 4); audio.set_envelope(0.001, 0.05, 0, 0.05) end

-- Presets de Áudio Procedural
function Audio.playCoin()
    audio.set_envelope(0.005, 0.06, 0.4, 0.08)
    audio.play_note(975, 0.08, 0)
    Timer.after(0.08, function()
        audio.set_envelope(0.005, 0.1, 0.4, 0.1)
        audio.play_note(1300, 0.25, 0)
    end)
end

function Audio.playLaser()
    local steps = 8
    audio.set_envelope(0.001, 0.015, 0.0, 0.01)
    for i = 0, steps - 1 do
        local freq = 1600 - (i * 150)
        Timer.after(i * 0.015, function()
            audio.play_note(freq, 0.02, 0)
        end)
    end
end

function Audio.playExplosion()
    audio.set_envelope(0.01, 0.4, 0.1, 0.2)
    audio.play_note(100, 0.5, 4)
    Timer.after(0.02, function()
        audio.set_envelope(0.05, 0.3, 0.0, 0.1)
        audio.play_note(60, 0.4, 3)
    end)
end

function Audio.playPowerup()
    local freqs = {330, 440, 660, 880, 1320}
    for i, f in ipairs(freqs) do
        Timer.after((i - 1) * 0.07, function()
            audio.set_envelope(0.005, 0.05, 0.5, 0.05)
            audio.play_note(f, 0.08, 2)
        end)
    end
end

function Audio.playHurt()
    audio.set_envelope(0.002, 0.08, 0.1, 0.05)
    audio.play_note(120, 0.15, 1)
    Timer.after(0.05, function()
        audio.set_envelope(0.002, 0.1, 0.0, 0.05)
        audio.play_note(80, 0.12, 4)
    end)
end

function Audio.playLightHit()
    if audio.play_impact then
        audio.play_impact(12.0, 3) -- Flesh impact
    else
        audio.beep(180, 0.08, 1)
    end
end

function Audio.playHeavyHit()
    if audio.play_impact then
        audio.play_impact(25.0, 3)
    else
        audio.beep(110, 0.14, 2)
    end
    if camera and camera.shake then camera.shake(0.4) end
end

function Audio.playBlock()
    if audio.play_impact then
        audio.play_impact(15.0, 0) -- Metal deflect
    else
        audio.beep(440, 0.06, 0)
    end
end

function Audio.playParry()
    if audio.play_impact then
        audio.play_impact(20.0, 4) -- CyberShield chime
    else
        audio.fm_note(880, 0.12, 0)
    end
    if camera and camera.shake then camera.shake(0.2) end
end

function Audio.playSpecial()
    if audio.fm_note then
        audio.fm_note(523.25, 0.2, 1)
        Timer.after(0.06, function() audio.fm_note(659.25, 0.25, 0) end)
        Timer.after(0.12, function() audio.fm_note(783.99, 0.35, 2) end)
    end
    if camera and camera.shake then camera.shake(0.6) end
end

function Audio.sequence(notes)
    local accumulatedDelay = 0
    for _, note in ipairs(notes) do
        local freq = note.freq or 440
        local duration = note.duration or 0.1
        local noteType = note.type or 0
        local delay = note.delay or 0
        accumulatedDelay = accumulatedDelay + delay
        Timer.after(accumulatedDelay, function()
            -- audio.play_note(freq, duration, noteType)
        end)
    end
end

-- ============================================================================
-- SpriteAnim v3 — Frame-based sprite animation
-- ============================================================================
SpriteAnim = Class()
function SpriteAnim:Init(config)
    self.frames=config.frames or {}; self.fps=config.fps or 12
    self.loop=config.loop~=false; self.playing=true
    self.currentFrame=1; self._timer=0; self.onComplete=config.onComplete
end
function SpriteAnim:update(dt)
    if not self.playing or #self.frames==0 then return end
    self._timer=self._timer+dt
    if self._timer>=1/self.fps then
        self._timer=self._timer-1/self.fps; self.currentFrame=self.currentFrame+1
        if self.currentFrame>#self.frames then
            if self.loop then self.currentFrame=1
            else self.currentFrame=#self.frames;self.playing=false;if self.onComplete then self.onComplete() end end
        end
    end
end
function SpriteAnim:current() return self.frames[self.currentFrame] end
function SpriteAnim:play() self.playing=true;self.currentFrame=1;self._timer=0 end
function SpriteAnim:stop() self.playing=false end
function SpriteAnim:pause() self.playing=false end
function SpriteAnim:resume() self.playing=true end

-- ============================================================================
-- Legacy Compatibility Layer
-- ============================================================================
function Object(tag,x,y,z) local e=Engine.spawn(tag);Engine.set_pos(e,x or 0,y or 0,z or 0);return e end
function SetPos(e,x,y,z) Engine.set_pos(e,x,y,z) end
function SetColor(e,r,g,b) Engine.set_color(e,r,g,b) end
function SetScale(e,x,y,z) Engine.set_scale(e,x,y,z) end
function Say(msg) Engine.log(tostring(msg)) end
function IsDown(key) return Engine.is_down(key) end
function Distance(e1,e2) local x1,y1,z1=Engine.get_pos(e1);local x2,y2,z2=Engine.get_pos(e2);if x1 and x2 then return MathX.distance3D(x1,y1,z1,x2,y2,z2) end;return 999999 end
function Sound() Engine.play_sound("assets/audio/beep.wav") end

Say("SBA v7.0: Premium Outrun Bridge initialized — Advanced Audio, Radial Chromatic Aberration & Vignette, Physics2D & SpriteAnim ready.")

-- ============================================================================
-- VFX Systems & Global Draw Shorthands
-- ============================================================================
VFX = {}
function VFX.emit(x, y, z, vx, vy, vz, r, g, b, count, size)
    vfx.emit(x, y, z, vx, vy, vz, r, g, b, count or 1, size or 0.1)
end

function VFX.burst(x, y, z, count, speed, colors)
    count = count or 20
    speed = speed or 2.0
    colors = colors or {{1, 0, 0.6}, {0, 1, 0.9}, {1, 0.7, 0}}
    local col = colors[math.random(1, #colors)]
    vfx.burst(x, y, z, col[1], col[2], col[3], count, speed, 0.15)
end

function VFX.burst_2d(x, y, count, speed, colors)
    count = count or 20
    speed = speed or 2.0
    colors = colors or {{1, 0, 0.6}, {0, 1, 0.9}, {1, 0.7, 0}}
    local col = colors[math.random(1, #colors)]
    vfx.burst_2d(x, y, col[1], col[2], col[3], count, speed, 0.15)
end

function VFX.trail(x, y, z, r, g, b, size, lifetime)
    vfx.emit_trail(x, y, z, r, g, b, size or 0.15, lifetime or 0.5)
end

function DrawLine(x1, y1, x2, y2, thickness, r, g, b, a)
    gfx.draw_line(x1, y1, x2, y2, thickness or 1.0, r or 1.0, g or 1.0, b or 1.0, a or 1.0)
end

function DrawCircle(cx, cy, radius, r, g, b, segments, a)
    gfx.draw_circle(cx, cy, radius, r or 1.0, g or 1.0, b or 1.0, segments, a)
end

function DrawText(text, x, y, scale, r, g, b, a, isSDF)
    gfx.draw_text(text, x, y, scale or 1.0, r or 1.0, g or 1.0, b or 1.0, a or 1.0, isSDF)
end

-- ============================================================================
-- Physics2D Extensions — Raycast & AABB query
-- ============================================================================
function Physics2D.rayVsAABB(ox, oy, dx, dy, rx, ry, rw, rh)
    local tmin = -math.huge
    local tmax = math.huge

    if math.abs(dx) < 1e-6 then
        if ox < rx or ox > rx + rw then return false end
    else
        local t1 = (rx - ox) / dx
        local t2 = (rx + rw - ox) / dx
        tmin = math.max(tmin, math.min(t1, t2))
        tmax = math.min(tmax, math.max(t1, t2))
    end

    if math.abs(dy) < 1e-6 then
        if oy < ry or oy > ry + rh then return false end
    else
        local t1 = (ry - oy) / dy
        local t2 = (ry + rh - oy) / dy
        tmin = math.max(tmin, math.min(t1, t2))
        tmax = math.min(tmax, math.max(t1, t2))
    end

    return tmax >= tmin and tmax >= 0, tmin
end

function Physics2D.raycast(originX, originY, dirX, dirY, maxDistance, filterTag)
    local targets = filterTag and Entity.findByTag(filterTag) or Entity.all()
    local bestEnt = nil
    local bestDist = maxDistance or math.huge

    local len = math.sqrt(dirX*dirX + dirY*dirY)
    if len > 0 then
        dirX, dirY = dirX / len, dirY / len
    else
        return false
    end

    for _, ent in ipairs(targets) do
        local hit = false
        local dist = 0

        if ent.radius then
            local cx, cy, cr = ent.x, ent.y, ent.radius
            local ocx, ocy = cx - originX, cy - originY
            local tca = ocx * dirX + ocy * dirY
            if tca >= 0 then
                local d2 = (ocx*ocx + ocy*ocy) - tca*tca
                local r2 = cr*cr
                if d2 <= r2 then
                    local thc = math.sqrt(r2 - d2)
                    local t0 = tca - thc
                    local t1 = tca + thc
                    if t0 >= 0 then
                        hit = true
                        dist = t0
                    elseif t1 >= 0 then
                        hit = true
                        dist = t1
                    end
                end
            end
        else
            local w = ent.w or (ent._scaleX and ent._scaleX * 32 or 32)
            local h = ent.h or (ent._scaleY and ent._scaleY * 32 or 32)
            local rx, ry = ent.x - w/2, ent.y - h/2
            local isHit, tmin = Physics2D.rayVsAABB(originX, originY, dirX, dirY, rx, ry, w, h)
            if isHit then
                hit = true
                dist = tmin
            end
        end

        if hit and dist < bestDist then
            bestDist = dist
            bestEnt = ent
        end
    end

    if bestEnt then
        return true, bestDist, bestEnt
    end
    return false
end

function Physics2D.aabbQuery(x, y, w, h, filterTag)
    local targets = filterTag and Entity.findByTag(filterTag) or Entity.all()
    local results = {}
    for _, ent in ipairs(targets) do
        local ew = ent.w or (ent._scaleX and ent._scaleX * 32 or 32)
        local eh = ent.h or (ent._scaleY and ent._scaleY * 32 or 32)
        local ex, ey = ent.x - ew/2, ent.y - eh/2
        if Physics2D.CheckAABB(x, y, w, h, ex, ey, ew, eh) then
            results[#results+1] = ent
        end
    end
    return results
end

-- ============================================================================
-- SBA Framework Auto-Verification Self-Tests
-- ============================================================================
local function run_sba_self_tests()
    Engine.log("=== STARTING SBA FRAMEWORK SELF-TESTS ===")
    local passes = 0
    local fails = 0
    local function assert_test(cond, msg)
        if cond then
            passes = passes + 1
        else
            fails = fails + 1
            Engine.log_error("FAIL: " .. msg)
        end
    end

    -- 1. Testar Save (Recursivo)
    local testData = {
        score = 1500,
        name = "Player1",
        unlocked = { true, false, true },
        nested = {
            levels = { high = 5, current = 2 },
            tags = { "cool", "pro" }
        }
    }
    Save.write("test_profile", testData)
    Save.flush()
    Save.load()
    local loadedData = Save.read("test_profile")
    assert_test(loadedData ~= nil, "Save.read returned data")
    if loadedData then
        assert_test(loadedData.score == 1500, "Save.read simple key")
        assert_test(loadedData.name == "Player1", "Save.read string key")
        assert_test(loadedData.unlocked[1] == true and loadedData.unlocked[2] == false and loadedData.unlocked[3] == true, "Save.read simple array")
        assert_test(loadedData.nested.levels.high == 5, "Save.read nested table number")
        assert_test(loadedData.nested.tags[2] == "pro", "Save.read nested table array string")
    end

    -- 2. Testar MathX
    local n1 = MathX.noise1D(2.5)
    local n2 = MathX.noise2D(1.5, 3.5)
    assert_test(type(n1) == "number", "MathX.noise1D returns number")
    assert_test(type(n2) == "number", "MathX.noise2D returns number")

    -- 3. Testar Physics2D MTV
    local hit, mx, my = Physics2D.AABBs_MTV(0, 0, 10, 10, 8, 0, 10, 10)
    assert_test(hit == true, "Physics2D.AABBs_MTV overlap detected")
    assert_test(math.abs(mx) > 0 and my == 0, "Physics2D.AABBs_MTV MTV direction check")

    local hitC, cx, cy = Physics2D.CircleVsAABB_MTV(9, 5, 2, 0, 0, 10, 10)
    assert_test(hitC == true, "Physics2D.CircleVsAABB_MTV overlap detected")
    assert_test(math.abs(cx) > 0 or math.abs(cy) > 0, "Physics2D.CircleVsAABB_MTV MTV has length")

    -- 4. Testar ECS em Lua (Components)
    local testEnt = Entity()
    local addedCalled = false
    local updateCalled = false
    local removeCalled = false
    local testComp = Component("TestComp")
    function testComp:onAdd(ent)
        addedCalled = true
        assert_test(ent == testEnt, "Component onAdd receives entity")
    end
    function testComp:update(dt)
        updateCalled = true
        assert_test(dt == 0.1, "Component update receives dt")
    end
    function testComp:onRemove()
        removeCalled = true
    end

    testEnt:addComponent(testComp)
    assert_test(addedCalled == true, "addComponent triggers onAdd")
    assert_test(testEnt:getComponent("TestComp") == testComp, "getComponent returns the component")

    testEnt:update(0.1)
    assert_test(updateCalled == true, "entity update calls component update")

    testEnt:removeComponent("TestComp")
    assert_test(removeCalled == true, "removeComponent triggers onRemove")
    assert_test(testEnt:getComponent("TestComp") == nil, "getComponent returns nil after removal")
    testEnt:destroy()

    -- 5. Testar Tweens Genéricos
    local dummy = { val = 10, x = 0, y = 0 }
    Tween.to(dummy, { val = 20, x = 100 }, 1, "linear")
    Tween.update(0.5)
    assert_test(dummy.val == 15, "Tween generic number interpolation (50%)")
    assert_test(dummy.x == 50, "Tween x property interpolation (50%)")
    Tween.update(0.5)
    assert_test(dummy.val == 20, "Tween generic number interpolation (100%)")
    assert_test(dummy.x == 100, "Tween x property interpolation (100%)")
    Tween.cancel(dummy)

    local rotDummy = { rotation = 350 }
    Tween.to(rotDummy, { rotation = 10 }, 1, "linear")
    Tween.update(0.5)
    assert_test(math.abs(rotDummy.rotation - 0) < 0.01 or math.abs(rotDummy.rotation - 360) < 0.01, "Tween rotation short angle interpolation (midpoint)")
    Tween.update(0.5)
    assert_test(math.abs(rotDummy.rotation - 10) < 0.01, "Tween rotation completion")

    -- 6. Testar robustez de Eventos (pcall)
    local eventFired = false
    local errorFired = false
    Events.on("testEvent", function(data)
        eventFired = true
        error("Intentional error to test pcall safety")
    end)
    Events.on("testEvent", function(data)
        errorFired = true
        assert_test(data == "ok", "Subsequent event listeners still receive data")
    end)
    Events.emit("testEvent", "ok")
    assert_test(eventFired == true, "First event listener fired")
    assert_test(errorFired == true, "Second event listener fired despite error in first")
    Events.clear("testEvent")

    -- 7. Testar ReverbZone e VFX
    local rzEnt = Entity("ReverbTestEnt")
    rzEnt:addReverbZone(10, 50, 0.75)
    assert_test(rzEnt:hasReverbZone() == true, "Entity hasReverbZone is true after add")
    local rzMin, rzMax, rzFactor, rzActive = rzEnt:getReverbZone()
    assert_test(rzMin == 10 and rzMax == 50 and rzFactor == 0.75 and rzActive == true, "Entity getReverbZone returns correct values")
    
    rzEnt:setReverbZoneFactor(0.5)
    local _, _, rzFactor2 = rzEnt:getReverbZone()
    assert_test(rzFactor2 == 0.5, "Entity setReverbZoneFactor updates factor")
    
    rzEnt:setReverbZoneBounds(5, 25)
    local rzMin2, rzMax2 = rzEnt:getReverbZone()
    assert_test(rzMin2 == 5 and rzMax2 == 25, "Entity setReverbZoneBounds updates bounds")
    
    rzEnt:removeReverbZone()
    assert_test(rzEnt:hasReverbZone() == false, "Entity hasReverbZone is false after remove")
    rzEnt:destroy()
    
    -- VFX Test
    local vfxOk = pcall(function()
        VFX.emit(0, 0, 0, 1, 1, 1, 1, 0, 1, 10, 0.2)
        VFX.burst(0, 0, 0, 15, 3.0)
    end)
    assert_test(vfxOk == true, "VFX.emit and VFX.burst executed without errors")

    -- 8. Testar Physics2D Raycast e AABB Query
    local rayEnt = Entity("RayTarget", 50, 0, 0)
    rayEnt.w = 20
    rayEnt.h = 20
    local hitRay, distRay, entRay = Physics2D.raycast(0, 0, 1, 0, 100)
    assert_test(hitRay == true, "Physics2D.raycast hit entity")
    if hitRay then
        assert_test(entRay == rayEnt, "Physics2D.raycast hit correct entity")
        assert_test(math.abs(distRay - 40) < 0.1, "Physics2D.raycast correct hit distance")
    end

    local queryEnts = Physics2D.aabbQuery(30, -20, 40, 40)
    assert_test(#queryEnts >= 1, "Physics2D.aabbQuery found overlapping entities")
    rayEnt:destroy()

    -- 9. Testar Tween pause/resume
    local tweenDummy = { val = 0 }
    Tween.to(tweenDummy, { val = 100 }, 1.0, "linear")
    Tween.pause(tweenDummy)
    Tween.update(0.5)
    assert_test(tweenDummy.val == 0, "Tween is paused and did not update")
    Tween.resume(tweenDummy)
    Tween.update(0.5)
    assert_test(tweenDummy.val == 50, "Tween is resumed and updated (50%)")
    Tween.cancel(tweenDummy)

    -- 10. Testar Audio Sequence
    local audioSeqOk = pcall(function()
        Audio.sequence({
            { freq = 440, duration = 0.05, type = 0, delay = 0 },
            { freq = 554, duration = 0.05, type = 0, delay = 0.05 },
            { freq = 659, duration = 0.05, type = 0, delay = 0.05 }
        })
    end)
    assert_test(audioSeqOk == true, "Audio.sequence scheduled without errors")

    Engine.log(string.format("=== SBA SELF-TESTS FINISHED: Passes: %d, Fails: %d ===", passes, fails))
    if fails > 0 then
        Engine.log_error("SBA Framework has failing tests!")
    else
        Engine.log("ALL SBA SELF-TESTS PASSED SUCCESSFULLY!")
    end
end

run_sba_self_tests()
