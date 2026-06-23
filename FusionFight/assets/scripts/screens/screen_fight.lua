local ScreenFight = Class()

local function DrawDebugBox(box, color, screenW, drawCamX, drawCamY, finalZoom)
    if not box or not box.x then return end
    local screenCX = (screenW / 2) + (box.x - drawCamX) * finalZoom
    local sw = box.w * finalZoom
    local sh = box.h * finalZoom
    local sx = screenCX - sw / 2
    local sy = 500 + (box.y - 200 - drawCamY) * finalZoom - sh
    
    gfx.draw_rect_outline(sx, sy, sw, sh, 2.0, color[1], color[2], color[3], 0.8)
    gfx.draw_rect(sx, sy, sw, sh, color[1], color[2], color[3], 0.15)
end

function ScreenFight:Init()
    self.camX = 0
    self.camY = 0
    self.camZoom = 1.2
    
    self.endTimer = nil
    self.paused = false
    
    self.sparks = {}
    self.shockwaves = {}
    self.p1HealthPrev = 1000
    self.p2HealthPrev = 1000
    self.koFlashAlpha = 0
    self.prevFightState = "none"
    
    local cx = window.get_width() / 2
    local cy = window.get_height() / 2
    
    self.btnResume = Button("RESUME FIGHT", cx - 120, cy - 40, 240, 45, {0.1, 0.7, 1.0})
    self.btnQuit = Button("QUIT TO MENU", cx - 120, cy + 25, 240, 45, {0.9, 0.1, 0.4})
end

function ScreenFight:Enter()
    self.camX = 0
    self.camY = 0
    self.camZoom = 1.2
    self.endTimer = nil
    self.paused = false
    self.sparks = {}
    self.shockwaves = {}
    self.p1HealthPrev = fight.get_p1_health()
    self.p2HealthPrev = fight.get_p2_health()
    self.p1ParryPrev = false
    self.p2ParryPrev = false
    self.koFlashAlpha = 0
    self.prevFightState = fight.get_state()
    
    -- Configura bloom e exposicao neon premium na engine conforme o preset
    local bloomSteps = 3
    if GameSettings.graphicsPreset == 0 then bloomSteps = 2
    elseif GameSettings.graphicsPreset == 1 then bloomSteps = 4
    else bloomSteps = 8
    end
    if Engine and Engine.set_bloom then
        Engine.set_bloom(0.35, bloomSteps)
    end
    if Engine and Engine.set_exposure then
        Engine.set_exposure(1.1, 2.0)
    end
    
    -- Ensure fight system is active
    fight.set_active(true)
    
    if events then
        events.publish("on_fight_start", {
            p1 = (fight.get_p1_character and fight.get_p1_character()) or "kaito",
            p2 = (fight.get_p2_character and fight.get_p2_character()) or "rin"
        })
    end
end

function ScreenFight:Update(dt)
    -- Screen shake update if present
    if ScreenShake and ScreenShake.update then
        ScreenShake.update(dt)
    end
    
    -- Pause handling
    if input.is_just_pressed("Escape") then
        self.paused = not self.paused
        if self.paused then
            self.pauseCursor = 1
        end
        audio.play_note(600, 0.05, 0)
    end
    
    if self.paused then
        -- Keyboard navigation
        if input.is_just_pressed("W") or input.is_just_pressed("Up") then
            self.pauseCursor = 1
            audio.play_note(440, 0.05, 0)
        elseif input.is_just_pressed("S") or input.is_just_pressed("Down") then
            self.pauseCursor = 2
            audio.play_note(440, 0.05, 0)
        end
        
        -- Run standard updates (for mouse hover)
        local clickResume = self.btnResume:Update()
        local clickQuit = self.btnQuit:Update()
        
        -- Mouse hover overrides keyboard
        if self.btnResume.hover then self.pauseCursor = 1
        elseif self.btnQuit.hover then self.pauseCursor = 2
        end
        
        -- Sync hover states
        self.btnResume.hover = (self.pauseCursor == 1)
        self.btnQuit.hover = (self.pauseCursor == 2)
        
        local confirm = input.is_just_pressed("Space") or input.is_just_pressed("Return") or input.is_just_pressed("U")
        
        if clickResume or (confirm and self.pauseCursor == 1) then
            self.paused = false
            audio.play_note(600, 0.05, 0)
        elseif clickQuit or (confirm and self.pauseCursor == 2) then
            fight.set_active(false)
            App:SwitchTo("MENU")
        end
        return
    end
    
    -- Physics updates for hit sparks (2D)
    for i = #self.sparks, 1, -1 do
        local s = self.sparks[i]
        s.x = s.x + s.vx * dt
        s.y = s.y + s.vy * dt
        s.vy = s.vy + 420 * dt -- gravity pulled downwards (Y-down)
        s.life = s.life - dt
        if s.life <= 0 then
            table.remove(self.sparks, i)
        end
    end
    
    -- Physics updates for shockwaves (2D)
    for i = #self.shockwaves, 1, -1 do
        local sh = self.shockwaves[i]
        sh.life = sh.life - dt
        sh.r = sh.r + (sh.maxR - sh.r) * 16 * dt
        if sh.life <= 0 then
            table.remove(self.shockwaves, i)
        end
    end
    
    -- Camera updates tracking midpoint of players
    local p1X, p1Y = fight.get_p1_pos()
    local p2X, p2Y = fight.get_p2_pos()
    
    local midX = (p1X + p2X) / 2
    local midY = (p1Y + p2Y) / 2
    
    local dist = math.abs(p1X - p2X)
    local targetZoom = 1.8 - (dist / 380)
    targetZoom = math.max(1.0, math.min(1.8, targetZoom))
    
    -- Lerp camera position and zoom
    self.camX = self.camX + (midX - self.camX) * 0.08
    self.camY = self.camY + ((midY - 40) - self.camY) * 0.08
    self.camZoom = self.camZoom + (targetZoom - self.camZoom) * 0.08
    
    -- Damage and Hit detection for spawning dynamic hit sparks
    local p1Health = fight.get_p1_health()
    local p2Health = fight.get_p2_health()
    local p1Facing = fight.get_p1_facing()
    local p2Facing = fight.get_p2_facing()
    
    local p1Parry = fight.get_p1_parry()
    local p2Parry = fight.get_p2_parry()
    
    -- Parry detection
    if p1Parry and not self.p1ParryPrev then
        local impactX = p1X + p1Facing * 25
        local impactY = p1Y - 60
        self:SpawnHitSparks(impactX, impactY, "parry", p1Facing)
        if ScreenShake then
            ScreenShake.trigger(10, 0.2)
        end
    end
    if p2Parry and not self.p2ParryPrev then
        local impactX = p2X + p2Facing * 25
        local impactY = p2Y - 60
        self:SpawnHitSparks(impactX, impactY, "parry", p2Facing)
        if ScreenShake then
            ScreenShake.trigger(10, 0.2)
        end
    end
    
    self.p1ParryPrev = p1Parry
    self.p2ParryPrev = p2Parry
    
    -- Player 1 hit Player 2
    if p2Health < self.p2HealthPrev then
        local impactX = (p1X + p2X) / 2
        local impactY = p2Y - 60 -- Chest high
        
        local hitType = "hit"
        local state2 = fight.get_p2_state()
        if state2 == "blockstun_stand" or state2 == "blockstun_crouch" then
            hitType = "block"
        else
            if fight.get_p1_super_active() then
                hitType = "super"
            elseif fight.get_p1_ex() then
                hitType = "ex"
            end
        end
        self:SpawnHitSparks(impactX, impactY, hitType, p1Facing)
        
        if ScreenShake then
            if hitType == "super" then
                ScreenShake.triggerEarthquake(16, 0.4)
                if ScreenShake.triggerZoom then ScreenShake.triggerZoom(0.12, 0.35) end
            elseif hitType == "ex" then
                ScreenShake.triggerKick(14 * p1Facing, 0, 0.2)
            else
                ScreenShake.trigger(5, 0.15)
            end
        end

        if events then
            events.publish("on_player_hit", {
                player = 2,
                health = p2Health,
                damage = self.p2HealthPrev - p2Health,
                type = hitType
            })
        end
    end
    
    -- Player 2 hit Player 1
    if p1Health < self.p1HealthPrev then
        local impactX = (p1X + p2X) / 2
        local impactY = p1Y - 60
        
        local hitType = "hit"
        local state1 = fight.get_p1_state()
        if state1 == "blockstun_stand" or state1 == "blockstun_crouch" then
            hitType = "block"
        else
            if fight.get_p2_super_active() then
                hitType = "super"
            elseif fight.get_p2_ex() then
                hitType = "ex"
            end
        end
        self:SpawnHitSparks(impactX, impactY, hitType, p2Facing)
        
        if ScreenShake then
            if hitType == "super" then
                ScreenShake.triggerEarthquake(16, 0.4)
                if ScreenShake.triggerZoom then ScreenShake.triggerZoom(0.12, 0.35) end
            elseif hitType == "ex" then
                ScreenShake.triggerKick(14 * p2Facing, 0, 0.2)
            else
                ScreenShake.trigger(5, 0.15)
            end
        end

        if events then
            events.publish("on_player_hit", {
                player = 1,
                health = p1Health,
                damage = self.p1HealthPrev - p1Health,
                type = hitType
            })
        end
    end
    
    self.p1HealthPrev = p1Health
    self.p2HealthPrev = p2Health
    
    -- Check match state for Dramatic KO Flash and sound
    local state = fight.get_state()
    if state == "ko" and self.prevFightState ~= "ko" then
        self.koFlashAlpha = 1.0
        audio.play_note(110, 0.45, 2)
        audio.play_note(55, 0.65, 1)
        if events then
            events.publish("on_fight_ko", {
                winner = (p1Health > p2Health) and 1 or 2
            })
        end
    end
    self.prevFightState = state

    if self.koFlashAlpha and self.koFlashAlpha > 0 then
        self.koFlashAlpha = self.koFlashAlpha - dt * 2.0
        if self.koFlashAlpha < 0 then self.koFlashAlpha = 0 end
    end
    
    -- Check match end
    if state == "match_end" then
        if not self.endTimer then
            self.endTimer = 3.0 -- 3 seconds of victory screen
            if fight and fight.is_replaying and not fight.is_replaying() then
                if fight.save_replay then
                    fight.save_replay("assets/replay.rep")
                end
            end
            if events then
                events.publish("on_fight_end", {
                    winner = (p1Health > p2Health) and 1 or 2
                })
            end
        else
            self.endTimer = self.endTimer - dt
            if self.endTimer <= 0 then
                fight.set_active(false)
                App:SwitchTo("RESULT")
            end
        end
    end
end

function ScreenFight:SpawnHitSparks(x, y, hitType, facing)
    local count = 16
    local baseSpeed = 150
    if hitType == "ex" then
        count = 24
        baseSpeed = 220
    elseif hitType == "super" then
        count = 42
        baseSpeed = 320
    elseif hitType == "block" then
        count = 10
        baseSpeed = 110
    elseif hitType == "parry" then
        count = 12
        baseSpeed = 160
    end
    
    for i = 1, count do
        local angle = math.random() * math.pi * 2
        -- Direct sparks somewhat forward from hit direction
        if hitType ~= "block" and math.random() < 0.65 then
            angle = (facing == 1) and (math.random() * 1.5 - 0.75) or (math.random() * 1.5 + math.pi - 0.75)
        end
        
        local speed = math.random(baseSpeed * 0.5, baseSpeed * 1.6)
        local life = math.random(15, 35) / 60
        
        -- Color assignments matching the retro synthwave theme (cyan, magenta, white)
        local r, g, b = 1.0, 1.0, 1.0
        local randColor = math.random()
        
        if hitType == "block" then
            r, g, b = 1.0, 0.5, 0.1 -- Orange
            if randColor < 0.45 then
                r, g, b = 1.0, 0.9, 0.2 -- Yellow
            end
        elseif hitType == "parry" then
            r, g, b = 0.0, 0.9, 1.0 -- Cyber Cyan
            if randColor < 0.4 then
                r, g, b = 0.5, 0.95, 1.0 -- Light Cyan
            end
        else
            if randColor < 0.35 then
                r, g, b = 0.0, 0.88, 1.0 -- Cyan
            elseif randColor < 0.7 then
                r, g, b = 1.0, 0.05, 0.75 -- Magenta
            end
        end
        
        -- Ray length scaling
        local len = math.random(14, 40)
        if hitType == "ex" then len = math.random(20, 56)
        elseif hitType == "super" then len = math.random(28, 80) end
        
        local spark = {
            x = x,
            y = y,
            vx = math.cos(angle) * speed,
            vy = math.sin(angle) * speed - math.random(15, 45),
            life = life,
            maxLife = life,
            size = math.random(3, 5),
            type = hitType,
            r = r,
            g = g,
            b = b,
            len = len
        }
        table.insert(self.sparks, spark)
    end
    
    -- Radial Starburst needle spikes centered at the impact point
    if hitType ~= "block" then
        local starburstCount = 8
        if hitType == "super" then starburstCount = 14 end
        for i = 1, starburstCount do
            local angle = (i - 1) * (math.pi * 2 / starburstCount) + math.random() * 0.15
            local len = math.random(32, 64)
            if hitType == "ex" then len = math.random(42, 90)
            elseif hitType == "super" then len = math.random(65, 130) end
            
            local r, g, b = 1.0, 1.0, 1.0
            local randColor = math.random()
            if hitType == "parry" then
                r, g, b = 0.0, 0.9, 1.0
                if randColor < 0.4 then r, g, b = 0.5, 0.95, 1.0 end
            else
                if randColor < 0.35 then
                    r, g, b = 0.0, 0.88, 1.0 -- Cyan
                elseif randColor < 0.7 then
                    r, g, b = 1.0, 0.05, 0.75 -- Magenta
                end
            end
            
            local spark = {
                x = x,
                y = y,
                vx = 0,
                vy = 0,
                life = 0.22,
                maxLife = 0.22,
                size = 0,
                type = "starburst",
                r = r,
                g = g,
                b = b,
                angle = angle,
                len = len
            }
            table.insert(self.sparks, spark)
        end
    end
    
    -- Spawn shockwave circle
    local shock = {
        x = x,
        y = y,
        r = 4,
        maxR = (hitType == "super") and 110 or ((hitType == "ex") and 60 or 35),
        life = 0.22,
        maxLife = 0.22,
        type = hitType
    }
    table.insert(self.shockwaves, shock)
end

function ScreenFight:Draw()
    local screenW = window.get_width()
    local screenH = window.get_height()
    
    -- Get screen shake offsets
    local shakeX, shakeY = 0, 0
    if ScreenShake and ScreenShake.getOffset then
        shakeX, shakeY = ScreenShake.getOffset()
    end
    
    local zoomOffset = 1.0
    if ScreenShake and ScreenShake.getZoomOffset then
        zoomOffset = ScreenShake.getZoomOffset()
    end
    local finalZoom = self.camZoom * zoomOffset
    
    local drawCamX = self.camX + shakeX
    local drawCamY = self.camY + shakeY
    
    -- 1. Draw Parallax Stage
    Stages.Draw(GameSettings.stage, drawCamX, finalZoom)
    
    -- 2. Draw Projectiles
    local projs = fight.get_projectiles()
    for _, p in ipairs(projs) do
        local px = (screenW / 2) + (p.x - drawCamX) * finalZoom
        local py = 500 + (p.y - 200 - drawCamY) * finalZoom
        local pw = p.w * finalZoom
        local ph = p.h * finalZoom
        
        local color = (p.owner == 0) and {0.0, 0.8, 1.0} or {1.0, 0.4, 0.0}
        gfx.draw_circle(px, py + ph*0.5, pw*0.5, color[1], color[2], color[3], 1.0)
        gfx.draw_circle(px, py + ph*0.5, pw*0.75, color[1], color[2], color[3], 0.25) -- outer glow
    end
    
    -- 3. Draw Fighters
    -- Draw P1
    FighterRenderer.Draw(true, drawCamX, drawCamY, finalZoom)
    -- Draw P2
    FighterRenderer.Draw(false, drawCamX, drawCamY, finalZoom)
    
    -- 3.1. Draw Shockwaves
    for _, sh in ipairs(self.shockwaves) do
        local factor = sh.life / sh.maxLife
        local alpha = factor * 0.45
        local px = (screenW / 2) + (sh.x - drawCamX) * finalZoom
        local py = 500 + (sh.y - 200 - drawCamY) * finalZoom
        local pr = sh.r * finalZoom
        
        local r, g, b = 1.0, 1.0, 1.0
        if sh.type == "ex" then r, g, b = 0.1, 0.7, 1.0
        elseif sh.type == "super" then r, g, b = 1.0, 0.8, 0.1
        elseif sh.type == "block" then r, g, b = 0.8, 0.4, 0.1
        elseif sh.type == "parry" then r, g, b = 0.0, 0.9, 1.0 end
        
        gfx.draw_circle(px, py, pr, r, g, b, alpha)
        gfx.draw_circle(px, py, pr * 0.8, r, g, b, alpha * 0.4)
    end
    
    -- 3.2. Draw Hit Sparks (Retro Synthwave Starburst)
    for _, s in ipairs(self.sparks) do
        local factor = s.life / s.maxLife
        local alpha = factor
        local px = (screenW / 2) + (s.x - drawCamX) * finalZoom
        local py = 500 + (s.y - 200 - drawCamY) * finalZoom
        
        local r, g, b = s.r or 1.0, s.g or 1.0, s.b or 1.0
        
        if s.type == "starburst" then
            local pct = 1.0 - factor
            local stretch_factor = math.sin(pct * math.pi)
            local len = s.len * finalZoom * stretch_factor
            local dx = math.cos(s.angle)
            local dy = math.sin(s.angle)
            -- Draw a double-ended spike or single-ended ray radiating outward
            gfx.draw_line(px, py, px + dx * len, py + dy * len, 2.2 * finalZoom, r, g, b, alpha * 0.85)
            gfx.draw_line(px, py, px + dx * len * 0.6, py + dy * len * 0.6, 1.2 * finalZoom, 1.0, 1.0, 1.0, alpha * 0.95)
        else
            -- Draw core particle pixel
            local psz = s.size * finalZoom
            gfx.draw_rect(px - psz*0.5, py - psz*0.5, psz, psz, r, g, b, alpha)
            
            -- Draw long spiky needle rays extending backwards from velocity (stretch and shrink)
            local speed = math.sqrt(s.vx * s.vx + s.vy * s.vy)
            if speed > 1 then
                local dx = s.vx / speed
                local dy = s.vy / speed
                local pct = 1.0 - factor
                local stretch_factor = math.sin(pct * math.pi)
                local len = (s.len or 15) * finalZoom * stretch_factor
                
                -- Outer glowing colored ray
                gfx.draw_line(px, py, px - dx * len, py - dy * len, 1.8 * finalZoom, r, g, b, alpha * 0.7)
                -- Inner bright white core
                gfx.draw_line(px, py, px - dx * len * 0.65, py - dy * len * 0.65, 1.0 * finalZoom, 1.0, 1.0, 1.0, alpha * 0.95)
            end
        end
    end
    
    -- 3.3. Draw Debug Colliders (Hitboxes, Hurtboxes, Pushboxes)
    if GameSettings.debugBoxes then
        DrawDebugBox(fight.get_p1_pushbox(), {0.1, 0.5, 1.0}, screenW, drawCamX, drawCamY, finalZoom)
        DrawDebugBox(fight.get_p1_hurtbox(), {0.1, 0.9, 0.3}, screenW, drawCamX, drawCamY, finalZoom)
        DrawDebugBox(fight.get_p1_hitbox(), {1.0, 0.1, 0.1}, screenW, drawCamX, drawCamY, finalZoom)
        
        DrawDebugBox(fight.get_p2_pushbox(), {0.1, 0.5, 1.0}, screenW, drawCamX, drawCamY, finalZoom)
        DrawDebugBox(fight.get_p2_hurtbox(), {0.1, 0.9, 0.3}, screenW, drawCamX, drawCamY, finalZoom)
        DrawDebugBox(fight.get_p2_hitbox(), {1.0, 0.1, 0.1}, screenW, drawCamX, drawCamY, finalZoom)
    end
    
    -- 4. Draw HUD
    HUD.Draw()
    
    -- Draw Pause Menu Overlay
    if self.paused then
        gfx.draw_rect(0, 0, screenW, screenH, 0, 0, 0, 0.6)
        local cx = screenW / 2
        local cy = screenH / 2
        gfx.draw_text("PAUSED", cx - 40, cy - 100, 1.8, 1.0, 1.0, 1.0, 1.0)
        
        self.btnResume:Draw()
        self.btnQuit:Draw()
    end
    
    -- Draw Dramatic KO Flash Overlay
    if self.koFlashAlpha and self.koFlashAlpha > 0 then
        gfx.draw_rect(0, 0, screenW, screenH, 1.0, 1.0, 1.0, self.koFlashAlpha * 0.75)
    end
end

function ScreenFight:Exit()
end

return ScreenFight
