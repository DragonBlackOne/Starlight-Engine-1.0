-- core.lua — Starlight Engine Standard Library v4.0
-- Automatically loaded by the engine before any game script.
-- ============================================================================
Engine.log("Loading Starlight Standard Library v4.0 (Industrial)...")

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
-- MathX v3 — 15 utilities
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

-- ============================================================================
-- Physics2D v3 — 6 collision functions
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

-- ============================================================================
-- Timer v3 — swap-and-pop, pause/resume
-- ============================================================================
Timer = { _timers = {}, _nextId = 1 }
function Timer.after(s, cb) local id=Timer._nextId; Timer._nextId=id+1; Timer._timers[#Timer._timers+1]={id=id,time=s,callback=cb,repeating=false,paused=false}; return id end
function Timer.every(s, cb) local id=Timer._nextId; Timer._nextId=id+1; Timer._timers[#Timer._timers+1]={id=id,time=s,interval=s,callback=cb,repeating=true,paused=false}; return id end
function Timer.cancel(id) local ts=Timer._timers; for i=1,#ts do if ts[i].id==id then ts[i]=ts[#ts]; ts[#ts]=nil; return true end end; return false end
function Timer.pause(id) for _,t in ipairs(Timer._timers) do if t.id==id then t.paused=true; return end end end
function Timer.resume(id) for _,t in ipairs(Timer._timers) do if t.id==id then t.paused=false; return end end end
function Timer.update(dt)
    local ts=Timer._timers; local i=1
    while i<=#ts do local t=ts[i]
        if not t.paused then t.time=t.time-dt
            if t.time<=0 then t.callback()
                if t.repeating then t.time=t.time+t.interval; i=i+1
                else ts[i]=ts[#ts]; ts[#ts]=nil end
            else i=i+1 end
        else i=i+1 end
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
-- ScreenShake v3
-- ============================================================================
ScreenShake = { _time=0, _intensity=0, _decay=true }
function ScreenShake.trigger(intensity,duration) ScreenShake._intensity=intensity; ScreenShake._time=duration; ScreenShake._maxTime=duration end
function ScreenShake.update(dt) if ScreenShake._time>0 then ScreenShake._time=ScreenShake._time-dt else ScreenShake._intensity=0 end end
function ScreenShake.getOffset()
    if ScreenShake._intensity>0 then
        local factor=ScreenShake._time/(ScreenShake._maxTime or 1)
        local i=ScreenShake._intensity*factor
        return (math.random()*2-1)*i,(math.random()*2-1)*i
    end; return 0,0
end

-- ============================================================================
-- ValueTween v3 — swap-and-pop + onComplete
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
        if t>=1 then if v.onComplete then v.onComplete() end; tw[i]=tw[#tw]; tw[#tw]=nil else i=i+1 end
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
-- Save v3 — key-value persistence
-- ============================================================================
Save = { _data = {}, _path = "assets/save.dat" }
function Save.read(key, default) if Save._data[key]~=nil then return Save._data[key] end; return default end
function Save.write(key, value) Save._data[key]=value end
function Save.flush()
    local lines={}; for k,v in pairs(Save._data) do lines[#lines+1]=tostring(k).."="..tostring(v) end
    file.write(Save._path, table.concat(lines,"\n"))
end
function Save.load()
    local content=file.read(Save._path); if content=="" then return end
    for line in content:gmatch("[^\n]+") do local k,v=line:match("^(.-)=(.+)$")
        if k then Save._data[k]=tonumber(v) or v end
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

Engine.log("Starlight Standard Library v4.0 Loaded Successfully.")
