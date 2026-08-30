-- core.lua — Starlight Engine Standard Library v7.0
-- Automatically loaded by the engine before any game script.
-- ============================================================================
math.randomseed(os and os.time and os.time() or time and time.get_time() or 0)
Engine.log("Loading Starlight Standard Library v7.0 (Premium Outrun)...")

-- ============================================================================
-- Class Factory with Inheritance
-- ============================================================================
function Class(base)
    local c = {}
    if type(base) == 'table' then
        for k, v in pairs(base) do c[k] = v end
        c._base = base
    end
    c.__index = c
    local mt = {}
    mt.__call = function(_, ...)
        local obj = {}
        setmetatable(obj, c)
        if c.Init then c.Init(obj, ...) end
        return obj
    end
    c.Init = c.Init or function() end
    c.is_a = function(self, klass)
        local m = getmetatable(self)
        while m do if m == klass then return true end; m = m._base end
        return false
    end
    setmetatable(c, mt)
    return c
end

-- ============================================================================
-- MathX v3 — 17 utilities (including fractal noise)
-- ============================================================================
MathX = {}
function MathX.clamp(v, lo, hi) return math.max(lo, math.min(v, hi)) end
function MathX.lerp(a, b, t) return a + (b - a) * t end
function MathX.sign(x) return x < 0 and -1 or (x > 0 and 1 or 0) end
function MathX.distance(x1,y1,x2,y2) local dx,dy=x2-x1,y2-y1; return math.sqrt(dx*dx+dy*dy) end
function MathX.distance3D(x1,y1,z1,x2,y2,z2) local dx,dy,dz=x2-x1,y2-y1,z2-z1; return math.sqrt(dx*dx+dy*dy+dz*dz) end
function MathX.smoothstep(e0,e1,x) local t=MathX.clamp((x-e0)/(e1-e0),0,1); return t*t*(3-2*t) end
function MathX.remap(v,fL,fH,tL,tH) return tL+(v-fL)*(tH-tL)/(fH-fL) end
function MathX.random_range(lo,hi) return lo+math.random()*(hi-lo) end
function MathX.normalize2D(x,y) local l=math.sqrt(x*x+y*y); if l==0 then return 0,0 end; return x/l,y/l end
function MathX.angle(x1,y1,x2,y2) return math.atan(y2-y1,x2-x1) end
function MathX.wrap(v,lo,hi) return lo+(v-lo)%(hi-lo) end
function MathX.inverseLerp(a,b,v) if a==b then return 0 end; return (v-a)/(b-a) end
function MathX.moveTowards(cur,tgt,md) if math.abs(tgt-cur)<=md then return tgt end; return cur+MathX.sign(tgt-cur)*md end
function MathX.pingpong(t,len) t=t%(len*2); return len-math.abs(t-len) end
function MathX.rotate2D(x,y,a) local c,s=math.cos(a),math.sin(a); return x*c-y*s,x*s+y*c end
function MathX.raycastRect(ox,oy,dx,dy,rx,ry,rw,rh)
    local tx1,tx2,ty1,ty2=(rx-ox)/dx, (rx+rw-ox)/dx, (ry-oy)/dy, (ry+rh-oy)/dy
    local tmin=math.max(math.min(tx1,tx2),math.min(ty1,ty2))
    local tmax=math.min(math.max(tx1,tx2),math.max(ty1,ty2))
    if tmax<0 or tmin>tmax then return false,nil end; return true,tmin
end

-- 1D and 2D Fractal Wave Noise
function MathX.noise1D(x)
    return math.sin(x) * 0.5 + math.sin(x * 2.3) * 0.25 + math.sin(x * 5.7) * 0.15 + math.sin(x * 11.2) * 0.1
end
function MathX.noise2D(x, y)
    return (math.sin(x) * math.cos(y)) * 0.5 + 
           (math.sin(x * 2.1) * math.cos(y * 1.9)) * 0.25 + 
           (math.sin(x * 5.3) * math.cos(y * 5.1)) * 0.15 + 
           (math.sin(x * 10.7) * math.cos(y * 11.3)) * 0.1
end

-- ============================================================================
-- Physics2D v3 — Collision functions & MTV (Minimum Translation Vector)
-- ============================================================================
Physics2D = {}
function Physics2D.CheckAABB(x1,y1,w1,h1,x2,y2,w2,h2) return x1<x2+w2 and x1+w1>x2 and y1<y2+h2 and y1+h1>y2 end
function Physics2D.CheckCircle(x1,y1,r1,x2,y2,r2) return MathX.distance(x1,y1,x2,y2)<(r1+r2) end
function Physics2D.PointInRect(px,py,rx,ry,rw,rh) return px>=rx and px<=rx+rw and py>=ry and py<=ry+rh end
function Physics2D.RayCircle(ox,oy,dx,dy,cx,cy,cr)
    local fx,fy=ox-cx,oy-cy; local a=dx*dx+dy*dy; local b=2*(fx*dx+fy*dy)
    local c=fx*fx+fy*fy-cr*cr; local d=b*b-4*a*c
    if d<0 then return false,nil end; d=math.sqrt(d)
    local t=(-b-d)/(2*a); if t>=0 then return true,t end
    t=(-b+d)/(2*a); if t>=0 then return true,t end; return false,nil
end
function Physics2D.SegmentIntersect(ax,ay,bx,by,cx,cy,dx,dy)
    local r_x,r_y=bx-ax,by-ay; local s_x,s_y=dx-cx,dy-cy
    local d=r_x*s_y-r_y*s_x; if math.abs(d)<1e-10 then return false end
    local t=((cx-ax)*s_y-(cy-ay)*s_x)/d; local u=((cx-ax)*r_y-(cy-ay)*r_x)/d
    return t>=0 and t<=1 and u>=0 and u<=1
end
function Physics2D.CircleVsAABB(cx,cy,cr,rx,ry,rw,rh)
    local nx=MathX.clamp(cx,rx,rx+rw); local ny=MathX.clamp(cy,ry,ry+rh)
    return MathX.distance(cx,cy,nx,ny)<cr
end
function Physics2D.Overlaps(e1,e2)
    if e1.radius and e2.radius then return Physics2D.CheckCircle(e1.x,e1.y,e1.radius,e2.x,e2.y,e2.radius) end
    if e1.w and e2.w then return Physics2D.CheckAABB(e1.x,e1.y,e1.w,e1.h,e2.x,e2.y,e2.w,e2.h) end
    return false
end

-- Minimum Translation Vector solvers for pushing bodies apart
function Physics2D.CircleVsAABB_MTV(cx, cy, cr, rx, ry, rw, rh)
    local nx = MathX.clamp(cx, rx, rx+rw)
    local ny = MathX.clamp(cy, ry, ry+rh)
    local dist = MathX.distance(cx, cy, nx, ny)
    if dist < cr then
        local dx, dy = cx - nx, cy - ny
        local len = math.sqrt(dx*dx + dy*dy)
        if len == 0 then
            local dl = cx - rx
            local dr = rx + rw - cx
            local dt = cy - ry
            local db = ry + rh - cy
            local min = math.min(dl, dr, dt, db)
            if min == dl then return true, -(cr + dl), 0 end
            if min == dr then return true, (cr + dr), 0 end
            if min == dt then return true, 0, -(cr + dt) end
            return true, 0, (cr + db)
        end
        local pushDist = cr - len
        return true, (dx / len) * pushDist, (dy / len) * pushDist
    end
    return false, 0, 0
end

function Physics2D.AABBs_MTV(x1, y1, w1, h1, x2, y2, w2, h2)
    if not Physics2D.CheckAABB(x1, y1, w1, h1, x2, y2, w2, h2) then
        return false, 0, 0
    end
    local dx = (x1 + w1/2) - (x2 + w2/2)
    local totalW = (w1 + w2) / 2
    local overlapX = totalW - math.abs(dx)
    
    local dy = (y1 + h1/2) - (y2 + h2/2)
    local totalH = (h1 + h2) / 2
    local overlapY = totalH - math.abs(dy)
    
    if overlapX < overlapY then
        local sign = dx > 0 and 1 or -1
        return true, overlapX * sign, 0
    else
        local sign = dy > 0 and 1 or -1
        return true, 0, overlapY * sign
    end
end

-- ============================================================================
-- Timer v3 — swap-and-pop, pause/resume, pcall protected callbacks
-- ============================================================================
Timer = { _timers = {}, _nextId = 1 }
function Timer.after(s, cb) local id=Timer._nextId; Timer._nextId=id+1; Timer._timers[#Timer._timers+1]={id=id,time=s,callback=cb,repeating=false,paused=false}; return id end
function Timer.every(s, cb) local id=Timer._nextId; Timer._nextId=id+1; Timer._timers[#Timer._timers+1]={id=id,time=s,interval=s,callback=cb,repeating=true,paused=false}; return id end
function Timer.cancel(id) local ts=Timer._timers; for i=1,#ts do if ts[i].id==id then ts[i]=ts[#ts]; ts[#ts]=nil; return true end end; return false end
function Timer.pause(id) for _,t in ipairs(Timer._timers) do if t.id==id then t.paused=true; return end end end
function Timer.resume(id) for _,t in ipairs(Timer._timers) do if t.id==id then t.paused=false; return end end end
function Timer.update(dt)
    local ts = Timer._timers
    local i = 1
    while i <= #ts do
        local t = ts[i]
        if not t.paused then
            t.time = t.time - dt
            if t.time <= 0 then
                local ok, err = pcall(t.callback)
                if not ok then Engine.log_error("Timer error: " .. tostring(err)) end
                if t.repeating then
                    t.time = t.time + t.interval
                    i = i + 1
                else
                    ts[i] = ts[#ts]
                    ts[#ts] = nil
                end
            else
                i = i + 1
            end
        else
            i = i + 1
        end
    end
end

-- ============================================================================
-- Color v3 — HSV, hex, pulse, blend, rainbow, darken/lighten
-- ============================================================================
Color = {}
function Color.hsv(h,s,v) local i=math.floor(h*6); local f=h*6-i; local p=v*(1-s); local q=v*(1-f*s); local t=v*(1-(1-f)*s); i=i%6; if i==0 then return v,t,p end; if i==1 then return q,v,p end; if i==2 then return p,v,t end; if i==3 then return p,q,v end; if i==4 then return t,p,v end; return v,p,q end
function Color.pulse(r,g,b,time,speed,intensity) local f=1+math.sin(time*(speed or 5))*(intensity or 0.3); return r*f,g*f,b*f end
function Color.lerpRGB(r1,g1,b1,r2,g2,b2,t) return MathX.lerp(r1,r2,t),MathX.lerp(g1,g2,t),MathX.lerp(b1,b2,t) end
function Color.hex(h) h=h:gsub("#",""); return tonumber(h:sub(1,2),16)/255,tonumber(h:sub(3,4),16)/255,tonumber(h:sub(5,6),16)/255 end
function Color.rainbow(t) return Color.hsv((t*0.1)%1,0.8,1.0) end
function Color.darken(r,g,b,amount) local a=1-(amount or 0.3); return r*a,g*a,b*a end
function Color.lighten(r,g,b,amount) local a=amount or 0.3; return r+(1-r)*a,g+(1-g)*a,b+(1-b)*a end
function Color.withAlpha(r,g,b,a) return r,g,b,a end

-- ============================================================================
-- ScreenShake v3 — Presets, Directional Kicks and Zoom pulses
-- ============================================================================
ScreenShake = { _time=0, _intensity=0, _maxTime=0, _mode="RUMBLE", _kickX=0, _kickY=0, _zoomTime=0, _zoomIntensity=0, _zoomMaxTime=0 }
function ScreenShake.trigger(intensity,duration)
    ScreenShake._intensity=intensity
    ScreenShake._time=duration
    ScreenShake._maxTime=duration
    ScreenShake._mode="RUMBLE"
end
function ScreenShake.triggerRumble(intensity,duration)
    ScreenShake.trigger(intensity, duration)
end
function ScreenShake.triggerEarthquake(intensity,duration)
    ScreenShake.trigger(intensity, duration)
    ScreenShake._mode="EARTHQUAKE"
end
function ScreenShake.triggerKick(dx, dy, duration)
    ScreenShake._kickX = dx
    ScreenShake._kickY = dy
    ScreenShake._time = duration
    ScreenShake._maxTime = duration
    ScreenShake._mode = "KICK"
end
function ScreenShake.triggerZoom(intensity, duration)
    ScreenShake._zoomIntensity = intensity
    ScreenShake._zoomTime = duration
    ScreenShake._zoomMaxTime = duration
end
function ScreenShake.update(dt)
    if ScreenShake._time>0 then ScreenShake._time=ScreenShake._time-dt else ScreenShake._intensity=0; ScreenShake._kickX=0; ScreenShake._kickY=0 end
    if ScreenShake._zoomTime>0 then ScreenShake._zoomTime=ScreenShake._zoomTime-dt else ScreenShake._zoomIntensity=0 end
end
function ScreenShake.getOffset()
    local ox, oy = 0, 0
    if ScreenShake._intensity>0 or ScreenShake._mode == "KICK" then
        local factor=ScreenShake._time/(ScreenShake._maxTime or 1)
        local i=ScreenShake._intensity*factor
        if ScreenShake._mode == "EARTHQUAKE" then
            ox = (math.random()*0.2-0.1)*i
            oy = (math.random()*2-1)*i
        elseif ScreenShake._mode == "KICK" then
            ox = (ScreenShake._kickX or 0) * factor
            oy = (ScreenShake._kickY or 0) * factor
        else
            ox = (math.random()*2-1)*i
            oy = (math.random()*2-1)*i
        end
    end; return ox, oy
end
function ScreenShake.getZoomOffset()
    if ScreenShake._zoomIntensity>0 then
        local factor = ScreenShake._zoomTime / (ScreenShake._zoomMaxTime or 1)
        return 1.0 + math.sin(ScreenShake._zoomTime * 25.0) * ScreenShake._zoomIntensity * factor
    end
    return 1.0
end

-- ============================================================================
-- ValueTween v3 — pcall protected callbacks
-- ============================================================================
ValueTween = { _tweens = {} }
function ValueTween.to(target,key,endVal,duration,easingName,onComplete)
    local easeFn=(Easing and Easing[easingName or "linear"]) or function(t) return t end
    ValueTween._tweens[#ValueTween._tweens+1]={target=target,key=key,startVal=target[key],endVal=endVal,duration=duration,elapsed=0,easeFn=easeFn,onComplete=onComplete}
end
function ValueTween.update(dt)
    local tw=ValueTween._tweens; local i=1
    while i<=#tw do local v=tw[i]; v.elapsed=v.elapsed+dt
        local t=MathX.clamp(v.elapsed/v.duration,0,1); v.target[v.key]=MathX.lerp(v.startVal,v.endVal,v.easeFn(t))
        if t>=1 then
            if v.onComplete then
                local ok, err = pcall(v.onComplete)
                if not ok then Engine.log_error("ValueTween callback error: "..tostring(err)) end
            end
            tw[i]=tw[#tw]; tw[#tw]=nil
        else i=i+1 end
    end
end

-- ============================================================================
-- ObjectPool v3 — Reusable object recycling
-- ============================================================================
ObjectPool = Class()
function ObjectPool:Init(factory, resetFn, initialSize)
    self._factory=factory; self._reset=resetFn or function() end; self._pool={}; self._active={}
    for _=1,(initialSize or 0) do self._pool[#self._pool+1]=self._factory() end
end
function ObjectPool:get()
    local obj; if #self._pool>0 then obj=self._pool[#self._pool]; self._pool[#self._pool]=nil
    else obj=self._factory() end; self._active[#self._active+1]=obj; return obj
end
function ObjectPool:release(obj) self._reset(obj)
    for i=1,#self._active do if self._active[i]==obj then self._active[i]=self._active[#self._active]; self._active[#self._active]=nil; break end end
    self._pool[#self._pool+1]=obj
end
function ObjectPool:releaseAll() for i=#self._active,1,-1 do self._reset(self._active[i]); self._pool[#self._pool+1]=self._active[i]; self._active[i]=nil end end
function ObjectPool:activeCount() return #self._active end

-- ============================================================================
-- Signal v3 — Type-safe observable pattern
-- ============================================================================
Signal = Class()
function Signal:Init() self._slots={} end
function Signal:connect(fn) self._slots[#self._slots+1]=fn; return #self._slots end
function Signal:disconnect(id) if self._slots[id] then self._slots[id]=false end end
function Signal:emit(...) for _,fn in ipairs(self._slots) do if fn then fn(...) end end end
function Signal:clear() self._slots={} end

-- ============================================================================
-- Grid2D v3 — 2D array with utilities
-- ============================================================================
Grid2D = Class()
function Grid2D:Init(w,h,default)
    self.width=w; self.height=h; self.cells={}
    for y=1,h do self.cells[y]={}; for x=1,w do self.cells[y][x]=default end end
end
function Grid2D:get(x,y) if x>=1 and x<=self.width and y>=1 and y<=self.height then return self.cells[y][x] end end
function Grid2D:set(x,y,v) if x>=1 and x<=self.width and y>=1 and y<=self.height then self.cells[y][x]=v end end
function Grid2D:fill(v) for y=1,self.height do for x=1,self.width do self.cells[y][x]=v end end end
function Grid2D:forEach(fn) for y=1,self.height do for x=1,self.width do fn(x,y,self.cells[y][x]) end end end
function Grid2D:neighbors4(x,y)
    local n={}; local dirs={{0,-1},{0,1},{-1,0},{1,0}}
    for _,d in ipairs(dirs) do local nx,ny=x+d[1],y+d[2]; local v=self:get(nx,ny); if v~=nil then n[#n+1]={x=nx,y=ny,value=v} end end; return n
end
function Grid2D:neighbors8(x,y)
    local n={}; for dy=-1,1 do for dx=-1,1 do if dx~=0 or dy~=0 then local v=self:get(x+dx,y+dy); if v~=nil then n[#n+1]={x=x+dx,y=y+dy,value=v} end end end end; return n
end

-- ============================================================================
-- Particle2D v3 — Lightweight 2D particle system
-- ============================================================================
Particle2D = Class()
function Particle2D:Init(config)
    self.x=config.x or 0; self.y=config.y or 0
    self.maxParticles=config.max or 100; self.emitRate=config.rate or 10
    self.lifetime=config.lifetime or 1; self.speed=config.speed or 100
    self.spread=config.spread or math.pi*2; self.angle=config.angle or 0
    self.r=config.r or 1; self.g=config.g or 1; self.b=config.b or 1
    self.sizeStart=config.sizeStart or 4; self.sizeEnd=config.sizeEnd or 1
    self.gravity=config.gravity or 0; self.particles={}; self._emit_acc=0
    self.active=true; self.fadeAlpha=config.fadeAlpha~=false
end
function Particle2D:emit(count)
    for _=1,(count or 1) do if #self.particles>=self.maxParticles then return end
        local a=self.angle+MathX.random_range(-self.spread/2,self.spread/2)
        local s=MathX.random_range(self.speed*0.5,self.speed)
        self.particles[#self.particles+1]={x=self.x,y=self.y,vx=math.cos(a)*s,vy=math.sin(a)*s,life=self.lifetime,maxLife=self.lifetime}
    end
end
function Particle2D:update(dt)
    if self.active then self._emit_acc=self._emit_acc+dt*self.emitRate
        while self._emit_acc>=1 do self:emit(1); self._emit_acc=self._emit_acc-1 end
    end
    local ps=self.particles; local i=1
    while i<=#ps do local p=ps[i]; p.life=p.life-dt; if p.life<=0 then ps[i]=ps[#ps]; ps[#ps]=nil
        else p.vy=p.vy+self.gravity*dt; p.x=p.x+p.vx*dt; p.y=p.y+p.vy*dt; i=i+1 end
    end
end
function Particle2D:draw()
    for _,p in ipairs(self.particles) do
        local t=1-p.life/p.maxLife; local sz=MathX.lerp(self.sizeStart,self.sizeEnd,t)
        local alpha=self.fadeAlpha and p.life/p.maxLife or 1
        gfx.draw_quad(p.x-sz/2,p.y-sz/2,sz,sz,self.r,self.g,self.b,alpha)
    end
end
function Particle2D:stop() self.active=false end
function Particle2D:burst(count) self:emit(count or 20) end
function Particle2D:count() return #self.particles end

-- ============================================================================
-- StateMachine v3 — Generic FSM for any object
-- ============================================================================
StateMachine = Class()
function StateMachine:Init(owner)
    self.owner=owner; self.states={}; self.current=nil; self.currentName=""
end
function StateMachine:add(name, state) self.states[name]=state end
function StateMachine:switch(name, ...)
    if self.current and self.current.exit then self.current.exit(self.owner) end
    self.current=self.states[name]; self.currentName=name
    if self.current and self.current.enter then self.current.enter(self.owner, ...) end
end
function StateMachine:update(dt) if self.current and self.current.update then self.current.update(self.owner,dt) end end
function StateMachine:draw() if self.current and self.current.draw then self.current.draw(self.owner) end end

-- ============================================================================
-- Save v3 — key-value persistence with recursive table serialization
-- ============================================================================
local function serializeValue(val)
    local t = type(val)
    if t == "number" or t == "boolean" then
        return tostring(val)
    elseif t == "string" then
        return string.format("%q", val)
    elseif t == "table" then
        local parts = {}
        local isArray = true
        local maxIdx = 0
        local count = 0
        for k, v in pairs(val) do
            count = count + 1
            if type(k) ~= "number" or k <= 0 or math.floor(k) ~= k then
                isArray = false
            else
                if k > maxIdx then maxIdx = k end
            end
        end
        if isArray and maxIdx == count then
            for i = 1, maxIdx do
                table.insert(parts, serializeValue(val[i]))
            end
            return "{" .. table.concat(parts, ",") .. "}"
        else
            for k, v in pairs(val) do
                local kStr
                if type(k) == "string" and k:match("^[a-zA-Z_][a-zA-Z0-9_]*$") then
                    kStr = k
                else
                    kStr = "[" .. serializeValue(k) .. "]"
                end
                table.insert(parts, kStr .. "=" .. serializeValue(v))
            end
            return "{" .. table.concat(parts, ",") .. "}"
        end
    else
        return "nil"
    end
end

local function encrypt(str, key)
    key = key or "StarlightCorePremiumV7"
    local res = {}
    local keyLen = #key
    for i = 1, #str do
        local charVal = string.byte(str, i)
        local keyVal = string.byte(key, (i - 1) % keyLen + 1)
        local encVal = (charVal + keyVal) % 256
        table.insert(res, string.format("%02x", encVal))
    end
    return table.concat(res)
end

local function decrypt(hexStr, key)
    if not hexStr or #hexStr % 2 ~= 0 or not hexStr:match("^[0-9a-fA-F]+$") then
        return hexStr
    end
    key = key or "StarlightCorePremiumV7"
    local res = {}
    local keyLen = #key
    local byteIdx = 1
    for i = 1, #hexStr, 2 do
        local hexPair = hexStr:sub(i, i + 1)
        local encVal = tonumber(hexPair, 16)
        if not encVal then return nil end
        local keyVal = string.byte(key, (byteIdx - 1) % keyLen + 1)
        local charVal = (encVal - keyVal) % 256
        table.insert(res, string.char(charVal))
        byteIdx = byteIdx + 1
    end
    return table.concat(res)
end

Save = { _data = {}, _path = "assets/save.dat" }
function Save.read(key, default) if Save._data[key]~=nil then return Save._data[key] end; return default end
function Save.write(key, value) Save._data[key]=value end
function Save.flush()
    local serialized = serializeValue(Save._data)
    local encrypted = encrypt(serialized)
    file.write(Save._path, encrypted)
end
function Save.load()
    local content = file.read(Save._path)
    if content == "" then return end
    if content:sub(1, 1) ~= "{" then
        local decrypted = decrypt(content)
        if decrypted and decrypted:sub(1, 1) == "{" then
            content = decrypted
        else
            Engine.log_warn("Save file format unrecognized or decryption failed.")
        end
    end
    local chunk, err = (loadstring or load)("return " .. content)
    if chunk then
        local loaded = chunk()
        if type(loaded) == "table" then Save._data = loaded end
    else
        Engine.log_error("Save load error: " .. tostring(err))
    end
end
pcall(Save.load)

-- ============================================================================
-- Debug v3 — Watch variables overlay
-- ============================================================================
Debug = { _watches = {}, enabled = false }
function Debug.watch(label, value) Debug._watches[label]=tostring(value) end
function Debug.toggle() Debug.enabled=not Debug.enabled end
function Debug.draw()
    if not Debug.enabled then return end
    local y=8
    local count = 0
    for _ in pairs(Debug._watches) do count = count + 1 end
    gfx.draw_rect_alpha(4,4,220,12+16*count,0,0,0,0.7)
    for label,val in pairs(Debug._watches) do
        gfx.draw_text(label..": "..val, 8, y, 0.4, 0.0, 1.0, 0.0, 1.0); y=y+16
    end
end

-- ============================================================================
-- Button v1 — Reusable Neon UI Button class
-- ============================================================================
Button = Class()
function Button:Init(text, x, y, w, h, color, opts)
    self.text = text
    self.x = x
    self.y = y
    self.w = w
    self.h = h
    self.color = color or {1, 1, 1}
    self.hover = false
    self.opts = opts or {}
    self.enabled = true
    self.pulse = 0
end

function Button:Update()
    if not self.enabled then return false end
    local mx = input.get_mouse_x()
    local my = input.get_mouse_y()
    self.hover = (mx >= self.x and mx <= self.x + self.w and
                  my >= self.y and my <= self.y + self.h)
    if self.hover and input.is_just_pressed("MouseLeft") then
        audio.beep(440, 0.05, 0)
        return true
    end
    return false
end

function Button:Draw()
    if not self.enabled then return end
    local pulse = math.sin(time.get_time() * 3) * 0.1
    local c = self.hover and {self.color[1]*1.5, self.color[2]*1.5, self.color[3]*1.5} or self.color
    local glow = self.hover and 0.3 or 0.15
    gfx.draw_rect(self.x - 4, self.y - 4, self.w + 8, self.h + 8, c[1], c[2], c[3], glow + (self.opts.glowPulse and pulse or 0))
    gfx.draw_rect(self.x, self.y, self.w, self.h, c[1], c[2], c[3], self.hover and 0.8 or 0.4)
    local label = self.opts.label or self.text
    gfx.draw_text(label, self.x + self.w/2 - string.len(label)*4, self.y + self.h/2 - 4, 1.0, 1.0, 1.0, 1.0, 1.0)
end

-- ============================================================================
-- Table Utilities
-- ============================================================================
function table.shallow_clone(t)
    local c = {}
    for k, v in pairs(t) do c[k] = v end
    return c
end

function table.deep_clone(t)
    if type(t) ~= "table" then return t end
    local c = {}
    for k, v in pairs(t) do
        c[table.deep_clone(k)] = table.deep_clone(v)
    end
    return c
end

function table.keys(t)
    local ks = {}
    for k, _ in pairs(t) do ks[#ks+1] = k end
    return ks
end

function table.values(t)
    local vs = {}
    for _, v in pairs(t) do vs[#vs+1] = v end
    return vs
end

-- ============================================================================
-- String Utilities
-- ============================================================================
function string.split(s, sep)
    if not sep then sep = " " end
    local parts, start = {}, 1
    while true do
        local pos = string.find(s, sep, start, true)
        if not pos then
            parts[#parts+1] = string.sub(s, start)
            break
        end
        parts[#parts+1] = string.sub(s, start, pos - 1)
        start = pos + #sep
    end
    return parts
end

function string.starts(s, prefix) return string.sub(s, 1, #prefix) == prefix end
function string.ends(s, suffix) return suffix == "" or string.sub(s, -#suffix) == suffix end
function string.trim(s) return s:match("^%s*(.-)%s*$") or s end

-- ============================================================================
-- Draw API Wrapper (maps to native gfx / Renderer2D)
-- ============================================================================
draw = {
    rect_filled = function(x, y, w, h, r, g, b, a)
        if gfx and gfx.draw_rect then
            gfx.draw_rect(x, y, w, h, r or 1.0, g or 1.0, b or 1.0, a or 1.0)
        end
    end,
    rect = function(x, y, w, h, r, g, b, a, thickness)
        if gfx and gfx.draw_rect_outline then
            gfx.draw_rect_outline(x, y, w, h, thickness or 2.0, r or 1.0, g or 1.0, b or 1.0, a or 1.0)
        end
    end,
    text = function(x, y, text, r, g, b, a, scale)
        if gfx and gfx.draw_text then
            gfx.draw_text(tostring(text or ""), x, y, scale or 1.0, r or 1.0, g or 1.0, b or 1.0, a or 1.0)
        end
    end,
    circle = function(cx, cy, radius, r, g, b, a, segs)
        if gfx and gfx.draw_circle then
            gfx.draw_circle(cx, cy, radius, r or 1.0, g or 1.0, b or 1.0, segs or 24, a or 1.0)
        end
    end,
    circle_filled = function(cx, cy, radius, r, g, b, a)
        if gfx and gfx.draw_circle_filled then
            gfx.draw_circle_filled(cx, cy, radius, r or 1.0, g or 1.0, b or 1.0, a or 1.0)
        end
    end,
    line = function(x1, y1, x2, y2, r, g, b, a, thickness)
        if gfx and gfx.draw_line then
            gfx.draw_line(x1, y1, x2, y2, thickness or 2.0, r or 1.0, g or 1.0, b or 1.0, a or 1.0)
        end
    end
}

if audio then
    audio.play_synth = audio.play_synth or function(freq, duration, waveType, volume)
        local wt = 0 -- Square by default
        if type(waveType) == "string" then
            local s = string.lower(waveType)
            if s == "sine" then wt = 3
            elseif s == "square" then wt = 0
            elseif s == "saw" or s == "sawtooth" then wt = 1
            elseif s == "triangle" then wt = 2
            elseif s == "noise" then wt = 4
            end
        elseif type(waveType) == "number" then
            wt = waveType
        end
        if audio.beep then
            audio.beep(freq, duration or 0.1, wt)
        elseif audio.play_note then
            audio.play_note(freq, duration or 0.1, wt)
        end
    end
end

-- ============================================================================
-- Legacy Compatibility Support
-- ============================================================================
imgui = {}
function imgui.text(x, y, r, g, b, text)
    if type(r) == "string" then
        gfx.draw_text(r, x, y, 1.0, 1.0, 1.0, 1.0, 1.0)
    else
        gfx.draw_text(text or "", x, y, 1.0, r or 1.0, g or 1.0, b or 1.0, 1.0)
    end
end

-- ============================================================================
-- AssetCache — Reference-counted asset caching system
-- ============================================================================
AssetCache = {
    _textures = {}, -- path -> { id = texID, refCount = number }
    _sounds = {}    -- path -> { refCount = number }
}

function AssetCache.load_texture(path, filter)
    filter = (filter == nil) and true or filter
    local cached = AssetCache._textures[path]
    if cached then
        cached.refCount = cached.refCount + 1
        return cached.id
    end

    if not assets or not assets.load_texture then
        Engine.log_error("AssetCache: assets.load_texture is unavailable in this context.")
        return nil
    end

    local texID = assets.load_texture(path, filter)
    if texID and texID ~= 0 then
        AssetCache._textures[path] = { id = texID, refCount = 1 }
        Engine.log("[AssetCache] Loaded new texture: " .. path .. " (ID: " .. tostring(texID) .. ")")
        return texID
    end
    return nil
end

function AssetCache.release_texture(path)
    local cached = AssetCache._textures[path]
    if cached then
        cached.refCount = cached.refCount - 1
        if cached.refCount <= 0 then
            -- Note: If C++ exposed assets.unload_texture(texID), we'd call it here.
            -- Since it does not, we clear the cache reference so GC or systems can free resources.
            AssetCache._textures[path] = nil
            Engine.log("[AssetCache] Texture reference hit 0 and removed from cache: " .. path)
        end
    end
end

function AssetCache.load_sound(path)
    local cached = AssetCache._sounds[path]
    if cached then
        cached.refCount = cached.refCount + 1
        return path
    end
    AssetCache._sounds[path] = { refCount = 1 }
    Engine.log("[AssetCache] Cached reference for sound: " .. path)
    return path
end

function AssetCache.release_sound(path)
    local cached = AssetCache._sounds[path]
    if cached then
        cached.refCount = cached.refCount - 1
        if cached.refCount <= 0 then
            AssetCache._sounds[path] = nil
            Engine.log("[AssetCache] Sound reference hit 0 and removed from cache: " .. path)
        end
    end
end

function AssetCache.clear()
    AssetCache._textures = {}
    AssetCache._sounds = {}
    Engine.log("[AssetCache] Cache flushed completely.")
end

-- ============================================================================
-- FSM (Finite State Machine) Engine Suite (v12.0.0 Updates 61-75)
-- ============================================================================
FSM = {}
function FSM.create(initialState, states)
    local fsm = {
        current = initialState,
        states = states or {},
        stateTime = 0
    }
    function fsm:change(newState, ...)
        if self.states[self.current] and self.states[self.current].onExit then
            self.states[self.current].onExit(self)
        end
        self.current = newState
        self.stateTime = 0
        if self.states[self.current] and self.states[self.current].onEnter then
            self.states[self.current].onEnter(self, ...)
        end
    end
    function fsm:update(dt, ...)
        self.stateTime = self.stateTime + dt
        if self.states[self.current] and self.states[self.current].onUpdate then
            self.states[self.current].onUpdate(self, dt, ...)
        end
    end
    function fsm:draw(...)
        if self.states[self.current] and self.states[self.current].onDraw then
            self.states[self.current].onDraw(self, ...)
        end
    end
    if fsm.states[fsm.current] and fsm.states[fsm.current].onEnter then
        fsm.states[fsm.current].onEnter(fsm)
    end
    return fsm
end

-- ============================================================================
-- EventBus Pub/Sub Broker
-- ============================================================================
EventBus = { _listeners = {} }
function EventBus.on(event, callback)
    if not EventBus._listeners[event] then EventBus._listeners[event] = {} end
    table.insert(EventBus._listeners[event], callback)
    return function() EventBus.off(event, callback) end
end
function EventBus.off(event, callback)
    if not EventBus._listeners[event] then return end
    for i = #EventBus._listeners[event], 1, -1 do
        if EventBus._listeners[event][i] == callback then
            table.remove(EventBus._listeners[event], i)
        end
    end
end
function EventBus.emit(event, ...)
    if not EventBus._listeners[event] then return end
    for _, cb in ipairs(EventBus._listeners[event]) do
        cb(...)
    end
end
function EventBus.clear()
    EventBus._listeners = {}
end

-- ============================================================================
-- Graphics3D & Atmosphere Presets Suite
-- ============================================================================
Graphics3D = {}
Atmosphere = {
    DesertNoon = {
        clearColor = { 0.35, 0.58, 0.88 },
        exposure = 1.15,
        contrast = 1.22,
        saturation = 1.28,
        gamma = 2.2,
        fogDensity = 0.0018,
        fogColor = { 0.82, 0.72, 0.58 }
    },
    SunsetGold = {
        clearColor = { 0.85, 0.42, 0.22 },
        exposure = 1.25,
        contrast = 1.24,
        saturation = 1.30,
        gamma = 2.2,
        fogDensity = 0.0035,
        fogColor = { 0.95, 0.45, 0.18 }
    },
    CyberNight = {
        clearColor = { 0.04, 0.02, 0.08 },
        exposure = 1.10,
        contrast = 1.28,
        saturation = 1.40,
        gamma = 2.2,
        fogDensity = 0.0045,
        fogColor = { 0.12, 0.04, 0.22 }
    }
}

function Atmosphere.apply(preset)
    if not preset then return end
    if gfx.set_clear_color and preset.clearColor then
        gfx.set_clear_color(preset.clearColor[1], preset.clearColor[2], preset.clearColor[3])
    end
    if gfx.set_color_grading then
        gfx.set_color_grading(preset.exposure, preset.contrast, preset.saturation, preset.gamma, 0.20)
    end
end

Engine.log("Starlight Standard Library v15.0.0 (Cosmos Suite) Loaded Successfully.")
