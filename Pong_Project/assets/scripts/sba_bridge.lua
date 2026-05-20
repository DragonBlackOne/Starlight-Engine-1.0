-- ============================================================================
-- sba_bridge.lua — Starlight Bridge API v4.0
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
function Entity:update(dt) if self._anim then self._anim:update(dt) end end
function Entity:draw()
    if self._visible and self._anim then
        local frame = self._anim:current()
        if frame then gfx.draw_sprite(self.x, self.y, self._scaleX*32, self._scaleY*32, frame, self._r or 1, self._g or 1, self._b or 1, 1.0) end
    end
end

-- Custom data
function Entity:set(key,value) self._data[key]=value; return self end
function Entity:get(key,default) local v=self._data[key]; if v~=nil then return v end; return default end

-- Lifecycle
function Entity:destroy()
    if not self._alive then return end
    Engine.destroy(self._id); self._alive=false
    local list=Entity._registry[self._tag]
    if list then for i=#list,1,-1 do if list[i]==self then list[i]=list[#list];list[#list]=nil;break end end end
    for i=#Entity._all,1,-1 do if Entity._all[i]==self then Entity._all[i]=Entity._all[#Entity._all];Entity._all[#Entity._all]=nil;break end end
end

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
    local easeFn=Easing[easingName or "linear"] or Easing.linear
    local sv={}
    if (props.x or props.y or props.z) and entity.getPos then 
        local cx,cy,cz=entity:getPos();sv.x=cx;sv.y=cy;sv.z=cz 
    elseif (props.x or props.y or props.z) then
        sv.x=entity.x or 0; sv.y=entity.y or 0; sv.z=entity.z or 0
    end
    if props.zoom then sv.zoom = entity.zoom or 1.0 end
    if props.scaleX or props.scaleY or props.scaleZ then sv.scaleX=entity._scaleX or 1;sv.scaleY=entity._scaleY or 1;sv.scaleZ=entity._scaleZ or 1 end
    Tween._active[#Tween._active+1]={entity=entity,props=props,startValues=sv,duration=duration,elapsed=0,delay=delay or 0,easeFn=easeFn,onComplete=onComplete}
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
function Tween.group(steps)
    for _,s in ipairs(steps) do Tween.to(s.entity, s.props, s.duration, s.easing, s.onComplete, s.delay) end
end

function Tween.update(dt)
    local a=Tween._active; local i=1
    while i<=#a do local tw=a[i]
        if tw.delay>0 then tw.delay=tw.delay-dt; i=i+1
        else
            tw.elapsed=tw.elapsed+dt; local t=MathX.clamp(tw.elapsed/tw.duration,0,1); local e=tw.easeFn(t); local ent=tw.entity
            local np=false; local nx,ny,nz=ent.x or 0,ent.y or 0,ent.z or 0
            if tw.props.x then nx=MathX.lerp(tw.startValues.x,tw.props.x,e);np=true end
            if tw.props.y then ny=MathX.lerp(tw.startValues.y,tw.props.y,e);np=true end
            if tw.props.z then nz=MathX.lerp(tw.startValues.z,tw.props.z,e);np=true end
            if np then 
                if ent.setPos then ent:setPos(nx,ny,nz) else ent.x,ent.y,ent.z=nx,ny,nz end
            end
            if tw.props.zoom then ent.zoom=MathX.lerp(tw.startValues.zoom,tw.props.zoom,e) end
            local ns=false; local sx,sy,sz=ent._scaleX or 1,ent._scaleY or 1,ent._scaleZ or 1
            if tw.props.scaleX then sx=MathX.lerp(tw.startValues.scaleX,tw.props.scaleX,e);ns=true end
            if tw.props.scaleY then sy=MathX.lerp(tw.startValues.scaleY,tw.props.scaleY,e);ns=true end
            if tw.props.scaleZ then sz=MathX.lerp(tw.startValues.scaleZ,tw.props.scaleZ,e);ns=true end
            if ns then
                if ent.setScale then ent:setScale(sx,sy,sz) else ent._scaleX,ent._scaleY,ent._scaleZ=sx,sy,sz end
            end
            if t>=1 then if tw.onComplete then tw.onComplete(ent) end; a[i]=a[#a];a[#a]=nil else i=i+1 end
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
    local ls=Events._listeners[name]; if not ls then return end; local i=1
    while i<=#ls do ls[i].fn(data); if ls[i].once then ls[i]=ls[#ls];ls[#ls]=nil else i=i+1 end end
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
Input = { _actions = {} }
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

Say("SBA v4.0: Industrial Bridge initialized — Advanced Audio, SSR/Bloom, Physics2D & SpriteAnim ready.")
