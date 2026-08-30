-- ============================================================================
-- Fusion Pong // Ultimate Arcade & Classic 1972 Masterpiece (v10.0)
-- Full Commercial & Standalone Architecture for Fusion ENGINE (Windowed 1280x720)
-- ============================================================================

package.path = package.path .. ";assets/scripts/?.lua;./assets/scripts/?.lua"
engine = engine or Engine
Engine = Engine or engine

-- ============================================================================
-- Universal Safe Draw API Fallback
-- ============================================================================
draw = draw or {
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

-- Safe Audio Synth Binding Fallback
if audio and not audio.play_synth then
    audio.play_synth = function(freq, duration, waveType, volume)
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
-- Complete Procedural Sound Suite & Sound Bank 2.0
-- ============================================================================
local SoundBank = {
    enabled = true,
    bgmEnabled = true,
    bgmTimer = 0.0,
    bgmStep = 0,
    bgmStyle = "RETRO_BASS", -- "RETRO_BASS", "CLASSIC_PULSE", "OFF"
    synthNotes = {220, 220, 261, 293, 220, 220, 329, 293}
}

function SoundBank.play(sfxName, pitchMod)
    if not SoundBank.enabled or not (audio and audio.play_synth) then return end
    pitchMod = pitchMod or 1.0

    if sfxName == "UI_NAV" then
        audio.play_synth(580 * pitchMod, 0.04, "sine", 0.3)
    elseif sfxName == "UI_SELECT" then
        audio.play_synth(880 * pitchMod, 0.08, "triangle", 0.5)
    elseif sfxName == "UI_BACK" then
        audio.play_synth(330 * pitchMod, 0.06, "square", 0.4)
    elseif sfxName == "UI_TOGGLE" then
        audio.play_synth(720 * pitchMod, 0.05, "sine", 0.35)
    elseif sfxName == "COUNTDOWN_TICK" then
        audio.play_synth(330, 0.08, "square", 0.5)
    elseif sfxName == "COUNTDOWN_GO" then
        audio.play_synth(880, 0.22, "saw", 0.7)
    elseif sfxName == "PADDLE_HIT_CLASSIC" then
        -- Authentic 1972 Atari Tone (440 Hz square pulse)
        audio.play_synth(440 * pitchMod, 0.06, "square", 0.6)
    elseif sfxName == "PADDLE_HIT_TURBO" then
        audio.play_synth(480 * pitchMod, 0.07, "triangle", 0.55)
    elseif sfxName == "WALL_BOUNCE" then
        -- Authentic 1972 Wall Bounce (220 Hz low tone)
        audio.play_synth(220 * pitchMod, 0.05, "square", 0.45)
    elseif sfxName == "BALL_SMASH" then
        audio.play_synth(920 * pitchMod, 0.16, "saw", 0.8)
    elseif sfxName == "BALL_SERVE" then
        audio.play_synth(520, 0.10, "sine", 0.4)
    elseif sfxName == "BUMPER_DEFLECT" then
        audio.play_synth(1046 * pitchMod, 0.12, "sine", 0.7)
    elseif sfxName == "WARP_GATE" then
        audio.play_synth(784 * pitchMod, 0.14, "triangle", 0.6)
    elseif sfxName == "POWERUP_SPAWN" then
        audio.play_synth(659, 0.10, "sine", 0.35)
    elseif sfxName == "POWERUP_COLLECT" then
        audio.play_synth(880 * pitchMod, 0.18, "sine", 0.7)
    elseif sfxName == "LASER_FIRE" then
        audio.play_synth(980 * pitchMod, 0.07, "saw", 0.5)
    elseif sfxName == "LASER_HIT" then
        audio.play_synth(300, 0.12, "noise", 0.6)
    elseif sfxName == "SHIELD_BLOCK" then
        audio.play_synth(750, 0.09, "sine", 0.65)
    elseif sfxName == "EMP_FREEZE" then
        audio.play_synth(350, 0.20, "saw", 0.5)
    elseif sfxName == "GOAL_EXPLOSION" then
        audio.play_synth(120, 0.45, "noise", 0.85)
    elseif sfxName == "VICTORY_FANFARE" then
        audio.play_synth(587, 0.15, "triangle", 0.7)
        audio.play_synth(740, 0.15, "triangle", 0.7)
        audio.play_synth(880, 0.35, "saw", 0.8)
    elseif sfxName == "DEFEAT_CADENCE" then
        audio.play_synth(293, 0.18, "saw", 0.7)
        audio.play_synth(220, 0.30, "saw", 0.75)
    end
end

function SoundBank.updateBGM(dt, speedFactor)
    if not SoundBank.bgmEnabled or SoundBank.bgmStyle == "OFF" or not (audio and audio.play_synth) then return end

    if SoundBank.bgmStyle == "RETRO_BASS" then
        SoundBank.bgmTimer = SoundBank.bgmTimer + dt * math.max(1.0, speedFactor * 1.35)
        if SoundBank.bgmTimer >= 0.22 then
            SoundBank.bgmTimer = 0.0
            SoundBank.bgmStep = (SoundBank.bgmStep % #SoundBank.synthNotes) + 1
            local note = SoundBank.synthNotes[SoundBank.bgmStep]
            audio.play_synth(note, 0.10, "saw", 0.15)
        end
    elseif SoundBank.bgmStyle == "CLASSIC_PULSE" then
        SoundBank.bgmTimer = SoundBank.bgmTimer + dt * speedFactor
        if SoundBank.bgmTimer >= 0.45 then
            SoundBank.bgmTimer = 0.0
            SoundBank.bgmStep = (SoundBank.bgmStep % 2) + 1
            local note = (SoundBank.bgmStep == 1) and 110 or 146
            audio.play_synth(note, 0.05, "triangle", 0.12)
        end
    end
end

-- ============================================================================
-- Color Palettes & CRT Display Modes
-- ============================================================================
local CRTModes = {
    {
        name = "CLASSIC B&W (1972)",
        bg = {0.02, 0.02, 0.02, 1.0},
        fg = {0.95, 0.95, 0.95, 1.0},
        accent = {0.8, 0.8, 0.8, 1.0},
        dim = {0.35, 0.35, 0.35, 1.0}
    },
    {
        name = "PHOSPHOR GREEN (CRT)",
        bg = {0.01, 0.04, 0.02, 1.0},
        fg = {0.2, 1.0, 0.35, 1.0},
        accent = {0.4, 1.0, 0.55, 1.0},
        dim = {0.05, 0.45, 0.15, 1.0}
    },
    {
        name = "AMBER TERMINAL (VT220)",
        bg = {0.04, 0.02, 0.01, 1.0},
        fg = {1.0, 0.68, 0.1, 1.0},
        accent = {1.0, 0.85, 0.25, 1.0},
        dim = {0.45, 0.25, 0.05, 1.0}
    },
    {
        name = "CYBERPUNK NEON (OUTRUN)",
        bg = {0.04, 0.03, 0.08, 1.0},
        fg = {0.0, 0.90, 1.0, 1.0},
        accent = {1.0, 0.05, 0.65, 1.0},
        dim = {0.5, 0.52, 0.68, 1.0}
    }
}

local Constants = {
    ScreenWidth = 1280,
    ScreenHeight = 720,
    PaddleWidth = 20,
    PaddleHeight = 110,
    PaddleSpeed = 700,
    BallBaseSpeed = 520,
    MaxBallSpeed = 1900,
    SpeedIncrement = 40,
    ClassicMaxScore = 11,
    TurboMaxScore = 7
}

local Palette = {
    BgDark     = {0.04, 0.03, 0.08, 1.0},
    Cyan       = {0.0, 0.90, 1.0, 1.0},
    Magenta    = {1.0, 0.05, 0.65, 1.0},
    Violet     = {0.65, 0.15, 1.0, 1.0},
    Gold       = {1.0, 0.84, 0.0, 1.0},
    Orange     = {1.0, 0.45, 0.0, 1.0},
    Electric   = {0.1, 0.98, 0.55, 1.0},
    White      = {0.98, 0.98, 1.0, 1.0},
    Muted      = {0.5, 0.52, 0.68, 1.0},
    Matrix     = {0.05, 1.0, 0.35, 1.0},
    Crimson    = {1.0, 0.12, 0.25, 1.0}
}

-- ============================================================================
-- 5 Grand Prix Stages
-- ============================================================================
local Stages = {
    {
        name = "STAGE 1: NEO TOKYO DISTRICT",
        bossName = "NEON NOVICE",
        bossLevel = "Novice",
        primaryColor = Palette.Cyan,
        secondaryColor = Palette.Violet,
        sunColor = Palette.Magenta,
        hasBumper = false,
        hasPortals = false
    },
    {
        name = "STAGE 2: SYNTHWAVE HIGHWAY",
        bossName = "CYBER PHANTOM",
        bossLevel = "Pro",
        primaryColor = Palette.Magenta,
        secondaryColor = Palette.Orange,
        sunColor = Palette.Gold,
        hasBumper = true,
        bumperX = 640, bumperY = 240, bumperRadius = 38,
        hasPortals = false
    },
    {
        name = "STAGE 3: OBSIDIAN MATRIX",
        bossName = "VIPER BLADE",
        bossLevel = "Pro",
        primaryColor = Palette.Matrix,
        secondaryColor = Palette.Violet,
        sunColor = Palette.Electric,
        hasBumper = true,
        bumperX = 640, bumperY = 480, bumperRadius = 42,
        hasPortals = false
    },
    {
        name = "STAGE 4: NEON ORBITAL STATION",
        bossName = "AEGIS DREADNOUGHT",
        bossLevel = "Tactical",
        primaryColor = Palette.Cyan,
        secondaryColor = Palette.Crimson,
        sunColor = Palette.Violet,
        hasBumper = false,
        hasPortals = true,
        portal1 = {x = 640, y = 120, radius = 35},
        portal2 = {x = 640, y = 600, radius = 35}
    },
    {
        name = "STAGE 5: SOLAR CORE CITADEL",
        bossName = "CYBER-GOD 9000",
        bossLevel = "CyberGod",
        primaryColor = Palette.Gold,
        secondaryColor = Palette.Crimson,
        sunColor = Palette.Gold,
        hasBumper = true,
        bumperX = 640, bumperY = 360, bumperRadius = 50,
        hasPortals = false
    }
}

-- ============================================================================
-- Screen Shake Manager
-- ============================================================================
local ScreenShake = { trauma = 0.0, offsetX = 0.0, offsetY = 0.0 }
function ScreenShake.add(amount) ScreenShake.trauma = math.min(1.0, ScreenShake.trauma + amount) end
function ScreenShake.update(dt)
    if ScreenShake.trauma > 0 then
        local shake = ScreenShake.trauma * ScreenShake.trauma * 20.0
        ScreenShake.offsetX = (math.random() * 2.0 - 1.0) * shake
        ScreenShake.offsetY = (math.random() * 2.0 - 1.0) * shake
        ScreenShake.trauma = math.max(0.0, ScreenShake.trauma - dt * 2.4)
    else
        ScreenShake.offsetX = 0.0
        ScreenShake.offsetY = 0.0
    end
end

-- ============================================================================
-- ParticleSystem Class (OOP)
-- ============================================================================
local ParticleSystem = Class()
function ParticleSystem:Init()
    self.particles = {}
    self.trails = {}
    self.floatingTexts = {}
end

function ParticleSystem:SpawnHitParticles(x, y, color, count, speedMult)
    count = count or 14
    speedMult = speedMult or 1.0
    for _ = 1, count do
        local angle = math.random() * math.pi * 2
        local speed = math.random(140, 500) * speedMult
        table.insert(self.particles, {
            x = x, y = y,
            vx = math.cos(angle) * speed,
            vy = math.sin(angle) * speed,
            life = 1.0,
            maxLife = 1.0,
            color = color,
            size = math.random(4, 12)
        })
    end
end

function ParticleSystem:AddTrail(x, y, size, color)
    table.insert(self.trails, {
        x = x, y = y,
        size = size,
        life = 1.0,
        color = color or Palette.Cyan
    })
end

function ParticleSystem:AddPopup(text, x, y, color, scale)
    table.insert(self.floatingTexts, {
        text = text,
        x = x, y = y,
        color = color or Palette.Gold,
        life = 1.3,
        scale = scale or 1.0,
        vy = -50.0
    })
end

function ParticleSystem:Update(dt)
    for i = #self.particles, 1, -1 do
        local p = self.particles[i]
        p.x = p.x + p.vx * dt
        p.y = p.y + p.vy * dt
        p.vx = p.vx * (1.0 - dt * 2.2)
        p.vy = p.vy * (1.0 - dt * 2.2)
        p.life = p.life - dt * 2.4
        if p.life <= 0 then table.remove(self.particles, i) end
    end

    for i = #self.trails, 1, -1 do
        local t = self.trails[i]
        t.life = t.life - dt * 4.8
        t.size = t.size - dt * 18.0
        if t.life <= 0 or t.size <= 0 then table.remove(self.trails, i) end
    end

    for i = #self.floatingTexts, 1, -1 do
        local ft = self.floatingTexts[i]
        ft.y = ft.y + ft.vy * dt
        ft.life = ft.life - dt
        if ft.life <= 0 then table.remove(self.floatingTexts, i) end
    end
end

function ParticleSystem:Draw(ox, oy)
    ox = ox or 0
    oy = oy or 0
    for _, t in ipairs(self.trails) do
        local alpha = t.life * 0.65
        draw.rect_filled(ox + t.x - t.size * 0.5, oy + t.y - t.size * 0.5, t.size, t.size,
            t.color[1], t.color[2], t.color[3], alpha)
    end
    for _, p in ipairs(self.particles) do
        local alpha = p.life / p.maxLife
        local sz = p.size * alpha
        draw.rect_filled(ox + p.x - sz * 0.5, oy + p.y - sz * 0.5, sz, sz,
            p.color[1], p.color[2], p.color[3], alpha)
    end
    for _, ft in ipairs(self.floatingTexts) do
        local alpha = math.min(1.0, ft.life * 2.0)
        draw.text(ox + ft.x, oy + ft.y, ft.text, ft.color[1], ft.color[2], ft.color[3], alpha, ft.scale)
    end
end

-- ============================================================================
-- Authentic Segmented Retro Digit Renderer (Classic 1972)
-- ============================================================================
local function DrawClassicScoreDigit(cx, cy, digit, size, color)
    local segW = size * 0.8
    local segH = size * 0.16
    local halfH = size * 0.5
    local r, g, b, a = color[1], color[2], color[3], color[4] or 1.0

    -- Segment Definitions: Top, TopLeft, TopRight, Mid, BotLeft, BotRight, Bot
    local segs = {
        [0] = {true,  true,  true,  false, true,  true,  true},
        [1] = {false, false, true,  false, false, true,  false},
        [2] = {true,  false, true,  true,  true,  false, true},
        [3] = {true,  false, true,  true,  false, true,  true},
        [4] = {false, true,  true,  true,  false, true,  false},
        [5] = {true,  true,  false, true,  false, true,  true},
        [6] = {true,  true,  false, true,  true,  true,  true},
        [7] = {true,  false, true,  false, false, true,  false},
        [8] = {true,  true,  true,  true,  true,  true,  true},
        [9] = {true,  true,  true,  true,  false, true,  true}
    }

    local s = segs[digit % 10] or segs[0]
    if s[1] then draw.rect_filled(cx - segW*0.5, cy - halfH, segW, segH, r, g, b, a) end
    if s[2] then draw.rect_filled(cx - segW*0.5, cy - halfH, segH, halfH, r, g, b, a) end
    if s[3] then draw.rect_filled(cx + segW*0.5 - segH, cy - halfH, segH, halfH, r, g, b, a) end
    if s[4] then draw.rect_filled(cx - segW*0.5, cy - segH*0.5, segW, segH, r, g, b, a) end
    if s[5] then draw.rect_filled(cx - segW*0.5, cy, segH, halfH, r, g, b, a) end
    if s[6] then draw.rect_filled(cx + segW*0.5 - segH, cy, segH, halfH, r, g, b, a) end
    if s[7] then draw.rect_filled(cx - segW*0.5, cy + halfH - segH, segW, segH, r, g, b, a) end
end

local function DrawClassicScore(cx, cy, score, size, color)
    if score >= 10 then
        DrawClassicScoreDigit(cx - size * 0.55, cy, math.floor(score / 10), size, color)
        DrawClassicScoreDigit(cx + size * 0.55, cy, score % 10, size, color)
    else
        DrawClassicScoreDigit(cx, cy, score, size, color)
    end
end

-- ============================================================================
-- Paddle Class (OOP)
-- ============================================================================
local Paddle = Class()
function Paddle:Init(x, y, upKey, downKey, color, isAi, aiLevel)
    self.x = x or 60
    self.y = y or (Constants.ScreenHeight * 0.5 - Constants.PaddleHeight * 0.5)
    self.upKey = upKey or "W"
    self.downKey = downKey or "S"
    self.w = Constants.PaddleWidth
    self.h = Constants.PaddleHeight
    self.baseHeight = Constants.PaddleHeight
    self.color = color or Palette.White
    self.isAi = isAi or false
    self.aiLevel = aiLevel or "Master"
    self.score = 0
    self.vy = 0.0
    self.speed = Constants.PaddleSpeed
    self.laserAmmo = 3
    self.hyperMeter = 0.0
    self.shieldActive = false
    self.shieldTimer = 0.0
    self.freezeTimer = 0.0
    self.magnetActive = false
    self.magnetTimer = 0.0
    self.hitTimer = 0.0
end

function Paddle:OnHit()
    self.hitTimer = 0.2
    self.hyperMeter = math.min(100.0, self.hyperMeter + 18.0)
end

function Paddle:Reset()
    self.y = Constants.ScreenHeight * 0.5 - self.h * 0.5
    self.vy = 0.0
    self.h = self.baseHeight
    self.shieldActive = false
    self.shieldTimer = 0.0
    self.freezeTimer = 0.0
    self.magnetActive = false
    self.magnetTimer = 0.0
    self.hitTimer = 0.0
end

function Paddle:Update(dt, ball, ps, is2P, isClassic)
    if self.hitTimer > 0 then self.hitTimer = math.max(0, self.hitTimer - dt) end

    if self.freezeTimer > 0 then
        self.freezeTimer = self.freezeTimer - dt
        self.speed = Constants.PaddleSpeed * 0.4
    else
        self.speed = Constants.PaddleSpeed
    end

    if self.shieldTimer > 0 then
        self.shieldTimer = self.shieldTimer - dt
        if self.shieldTimer <= 0 then self.shieldActive = false end
    end

    if self.magnetTimer > 0 then
        self.magnetTimer = self.magnetTimer - dt
        if self.magnetTimer <= 0 then self.magnetActive = false end
        local targetY = self.y + self.h * 0.5
        local pdy = targetY - ball.y
        ball.vy = ball.vy + pdy * dt * 4.0
    end

    local moveDir = 0
    if not self.isAi then
        if input.is_down(self.upKey) or (not is2P and (input.is_down("UP") or input.is_down("Up"))) then moveDir = -1 end
        if input.is_down(self.downKey) or (not is2P and (input.is_down("DOWN") or input.is_down("Down"))) then moveDir = 1 end
    else
        local targetY = ball.y - self.h * 0.5
        local diff = targetY - self.y
        local deadZone = 12.0
        local aiReaction = 1.0

        if self.aiLevel == "Rookie" or self.aiLevel == "Novice" then
            aiReaction = 0.52
            deadZone = 32.0
        elseif self.aiLevel == "Veteran" or self.aiLevel == "Pro" then
            aiReaction = 0.85
            deadZone = 14.0
        elseif self.aiLevel == "Master" or self.aiLevel == "Tactical" then
            aiReaction = 1.05
            deadZone = 6.0
            targetY = targetY + ball.spin * 0.2
            diff = targetY - self.y
        elseif self.aiLevel == "Unbeatable" or self.aiLevel == "CyberGod" then
            aiReaction = 1.35
            deadZone = 2.0
            targetY = targetY + ball.spin * 0.35
            diff = targetY - self.y
            if not isClassic and self.hyperMeter >= 100.0 and math.abs(ball.x - self.x) < 320 then
                self.hyperMeter = 0.0
                ball.isSmash = true
                ball.speed = math.min(Constants.MaxBallSpeed, ball.speed + 350)
                SoundBank.play("BALL_SMASH", 1.4)
                ScreenShake.add(0.35)
                if ps then ps:AddPopup("[AI] EX OVERDRIVE!", self.x - 140, self.y - 30, Palette.Magenta, 1.4) end
            end
        end

        if math.abs(diff) > deadZone then
            moveDir = (diff > 0) and 1 or -1
            moveDir = moveDir * aiReaction
        end
    end

    self.vy = moveDir * self.speed
    self.y = self.y + self.vy * dt

    local topLimit = 16
    local bottomLimit = Constants.ScreenHeight - self.h - 16
    if self.y < topLimit then self.y = topLimit self.vy = 0 end
    if self.y > bottomLimit then self.y = bottomLimit self.vy = 0 end

    if ps and not isClassic and math.abs(self.vy) > 80 then
        ps:AddTrail(self.x + self.w * 0.5, self.y + self.h * 0.5, self.w, self.color)
    end
end

function Paddle:Draw(ox, oy, isClassic, crtMode)
    ox = ox or 0
    oy = oy or 0

    if isClassic then
        local c = crtMode.fg
        draw.rect_filled(ox + self.x, oy + self.y, self.w, self.h, c[1], c[2], c[3], 1.0)
        return
    end

    if self.shieldActive then
        local shieldX = (self.x < Constants.ScreenWidth * 0.5) and (self.x - 22) or (self.x + self.w + 14)
        draw.rect_filled(ox + shieldX, oy + 16, 8, Constants.ScreenHeight - 32, Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 0.75)
    end

    if self.freezeTimer > 0 then
        draw.rect_filled(ox + self.x - 4, oy + self.y - 4, self.w + 8, self.h + 8, Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 0.4)
    end

    local hyperFactor = self.hyperMeter / 100.0
    local r = self.color[1] * (1.0 - hyperFactor) + Palette.Gold[1] * hyperFactor
    local g = self.color[2] * (1.0 - hyperFactor) + Palette.Gold[2] * hyperFactor
    local b = self.color[3] * (1.0 - hyperFactor) + Palette.Gold[3] * hyperFactor

    draw.rect_filled(ox + self.x - 3, oy + self.y - 3, self.w + 6, self.h + 6, r, g, b, 0.4 + hyperFactor * 0.3)
    draw.rect_filled(ox + self.x, oy + self.y, self.w, self.h, r, g, b, 1.0)
    draw.rect_filled(ox + self.x + 3, oy + self.y + 3, self.w - 6, self.h - 6, Palette.White[1], Palette.White[2], Palette.White[3], 0.85)
end

-- ============================================================================
-- Ball Class (OOP)
-- ============================================================================
local Ball = Class()
function Ball:Init(ps)
    self.ps = ps
    self.x = Constants.ScreenWidth * 0.5
    self.y = Constants.ScreenHeight * 0.5
    self.vx = 0.0
    self.vy = 0.0
    self.size = 18
    self.speed = Constants.BallBaseSpeed
    self.spin = 0.0
    self.rally = 0
    self.isSmash = false
    self.color = Palette.White
    self.warpCooldown = 0.0
end

function Ball:Reset(dir)
    self.x = Constants.ScreenWidth * 0.5
    self.y = Constants.ScreenHeight * 0.5
    self.speed = Constants.BallBaseSpeed
    self.spin = 0.0
    self.rally = 0
    self.isSmash = false
    self.color = Palette.White
    self.warpCooldown = 0.0
    dir = dir or 1
    self.vx = dir * self.speed
    self.vy = 0
end

function Ball:Serve(dir, isClassic)
    self:Reset(dir)
    SoundBank.play("BALL_SERVE")
    local angle = (math.random() - 0.5) * (isClassic and 0.5 or 0.8)
    self.vx = (dir or 1) * math.cos(angle) * self.speed
    self.vy = math.sin(angle) * self.speed
end

function Ball:HandlePaddleBounce(p, isLeft, isClassic)
    self.rally = self.rally + 1
    if p.OnHit then p:OnHit() end

    -- Authentic 1972 7-Zone Deflection Model
    local relativeY = (self.y - (p.y + p.h * 0.5)) / (p.h * 0.5)
    relativeY = math.max(-1.0, math.min(1.0, relativeY))

    if isClassic then
        -- Progressive speed increments at hits 4, 8, 12
        if self.rally == 4 or self.rally == 8 or self.rally == 12 then
            self.speed = math.min(Constants.MaxBallSpeed, self.speed + 120)
        else
            self.speed = math.min(Constants.MaxBallSpeed, self.speed + 15)
        end

        local pitch = 1.0 + (math.abs(relativeY) * 0.4)
        SoundBank.play("PADDLE_HIT_CLASSIC", pitch)

        local bounceAngle = relativeY * (math.pi / 3.0)
        local dir = isLeft and 1 or -1
        self.vx = dir * math.cos(bounceAngle) * self.speed
        self.vy = math.sin(bounceAngle) * self.speed
    else
        self.speed = math.min(Constants.MaxBallSpeed, self.speed + Constants.SpeedIncrement)
        self.spin = p.vy * 0.5

        if math.abs(relativeY) > 0.7 or math.abs(p.vy) > 300 then
            self.isSmash = true
            self.speed = math.min(Constants.MaxBallSpeed, self.speed + 140)
            SoundBank.play("BALL_SMASH", 1.0 + (self.rally * 0.04))
            ScreenShake.add(0.20)
            if self.ps then
                self.ps:SpawnHitParticles(self.x, self.y, Palette.Gold, 20, 1.5)
                self.ps:AddPopup("HYPER SMASH!", self.x, self.y - 30, Palette.Gold, 1.4)
            end
        else
            self.isSmash = false
            SoundBank.play("PADDLE_HIT_TURBO", 1.0 + (self.rally * 0.03))
            ScreenShake.add(0.08)
            if self.ps then self.ps:SpawnHitParticles(self.x, self.y, p.color, 12) end
        end

        local bounceAngle = relativeY * (math.pi / 3.2)
        local dir = isLeft and 1 or -1
        self.vx = dir * math.cos(bounceAngle) * self.speed
        self.vy = math.sin(bounceAngle) * self.speed
    end

    local half = self.size * 0.5
    if isLeft then
        self.x = p.x + p.w + half
        self.vx = math.abs(self.vx)
    else
        self.x = p.x - half
        self.vx = -math.abs(self.vx)
    end
end

function Ball:Update(dt, p1, p2, isClassic, currentStage)
    if self.warpCooldown > 0 then self.warpCooldown = self.warpCooldown - dt end

    if not isClassic then
        self.vy = self.vy + self.spin * dt * 3.5
        self.spin = self.spin * (1.0 - dt * 0.8)
    end

    self.x = self.x + self.vx * dt
    self.y = self.y + self.vy * dt

    if self.ps and not isClassic then
        local trailColor = self.isSmash and Palette.Orange or (self.rally > 8 and Palette.Magenta or self.color)
        self.ps:AddTrail(self.x, self.y, self.size * 1.3, trailColor)
    end

    local half = self.size * 0.5
    if self.y - half <= 16 then
        self.y = 16 + half
        self.vy = math.abs(self.vy)
        SoundBank.play("WALL_BOUNCE")
        ScreenShake.add(0.06)
        if self.ps and not isClassic then self.ps:SpawnHitParticles(self.x, self.y, Palette.Violet, 8) end
    elseif self.y + half >= Constants.ScreenHeight - 16 then
        self.y = Constants.ScreenHeight - 16 - half
        self.vy = -math.abs(self.vy)
        SoundBank.play("WALL_BOUNCE")
        ScreenShake.add(0.06)
        if self.ps and not isClassic then self.ps:SpawnHitParticles(self.x, self.y, Palette.Violet, 8) end
    end

    -- Stage Hazards (Only in Grand Prix / Turbo)
    if not isClassic and currentStage then
        if currentStage.hasBumper and currentStage.bumperX then
            local bdx = self.x - currentStage.bumperX
            local bdy = self.y - currentStage.bumperY
            local bdist = math.sqrt(bdx*bdx + bdy*bdy)
            if bdist < (currentStage.bumperRadius + half) and bdist > 0.001 then
                local nx = bdx / bdist
                local ny = bdy / bdist
                self.x = currentStage.bumperX + nx * (currentStage.bumperRadius + half + 2.0)
                self.y = currentStage.bumperY + ny * (currentStage.bumperRadius + half + 2.0)
                self.vx = nx * self.speed * 1.15
                self.vy = ny * self.speed * 1.15
                SoundBank.play("BUMPER_DEFLECT", 1.2)
                ScreenShake.add(0.18)
                if self.ps then
                    self.ps:SpawnHitParticles(self.x, self.y, currentStage.primaryColor, 18, 1.4)
                    self.ps:AddPopup("BUMPER DEFLECT!", self.x, self.y - 25, currentStage.primaryColor, 1.3)
                end
            end
        end

        if currentStage.hasPortals and self.warpCooldown <= 0 then
            local p1p = currentStage.portal1
            local p2p = currentStage.portal2
            local d1 = math.sqrt((self.x - p1p.x)^2 + (self.y - p1p.y)^2)
            local d2 = math.sqrt((self.x - p2p.x)^2 + (self.y - p2p.y)^2)

            if d1 < p1p.radius then
                self.x = p2p.x
                self.y = p2p.y + ((self.vy > 0) and 30 or -30)
                self.warpCooldown = 1.0
                SoundBank.play("WARP_GATE")
                ScreenShake.add(0.2)
                if self.ps then
                    self.ps:SpawnHitParticles(self.x, self.y, Palette.Cyan, 20, 1.8)
                    self.ps:AddPopup("WARP GATE!", self.x - 45, self.y - 20, Palette.Cyan, 1.3)
                end
            elseif d2 < p2p.radius then
                self.x = p1p.x
                self.y = p1p.y + ((self.vy > 0) and 30 or -30)
                self.warpCooldown = 1.0
                SoundBank.play("WARP_GATE")
                ScreenShake.add(0.2)
                if self.ps then
                    self.ps:SpawnHitParticles(self.x, self.y, Palette.Crimson, 20, 1.8)
                    self.ps:AddPopup("WARP GATE!", self.x - 45, self.y - 20, Palette.Crimson, 1.3)
                end
            end
        end
    end

    if p1 and self.x - half <= p1.x + p1.w and self.x + half >= p1.x and
       self.y >= p1.y - 8 and self.y <= p1.y + p1.h + 8 and self.vx < 0 then
        self:HandlePaddleBounce(p1, true, isClassic)
    end

    if p2 and self.x + half >= p2.x and self.x - half <= p2.x + p2.w and
       self.y >= p2.y - 8 and self.y <= p2.y + p2.h + 8 and self.vx > 0 then
        self:HandlePaddleBounce(p2, false, isClassic)
    end
end

function Ball:Draw(ox, oy, isClassic, crtMode)
    ox = ox or 0
    oy = oy or 0
    local half = self.size * 0.5

    if isClassic then
        local c = crtMode.fg
        draw.rect_filled(ox + self.x - half, oy + self.y - half, self.size, self.size, c[1], c[2], c[3], 1.0)
        return
    end

    local c = self.isSmash and Palette.Gold or (self.rally > 10 and Palette.Magenta or Palette.Cyan)
    draw.rect_filled(ox + self.x - half - 3, oy + self.y - half - 3, self.size + 6, self.size + 6, c[1], c[2], c[3], 0.35)
    draw.rect_filled(ox + self.x - half, oy + self.y - half, self.size, self.size, c[1], c[2], c[3], 1.0)
    draw.rect_filled(ox + self.x - half + 3, oy + self.y - half + 3, self.size - 6, self.size - 6, Palette.White[1], Palette.White[2], Palette.White[3], 0.95)
end

-- ============================================================================
-- PowerUp Class (OOP)
-- ============================================================================
local PowerUp = Class()
local PowerTypes = {
    {name = "MULTI_BALL",  color = Palette.Gold,     icon = "3X"},
    {name = "LASER_GUN",   color = Palette.Magenta,  icon = "LZ"},
    {name = "BIG_PADDLE",  color = Palette.Electric, icon = "EXT"},
    {name = "SHIELD_WALL", color = Palette.Cyan,     icon = "SHD"},
    {name = "EMP_FREEZE",  color = Palette.Violet,   icon = "EMP"},
    {name = "MAGNET_CORE", color = Palette.Orange,   icon = "MAG"}
}

function PowerUp:Init(x, y)
    self.x = x or 0
    self.y = y or 0
    self.type = PowerTypes[math.random(1, #PowerTypes)]
    self.size = 28
    self.life = 12.0
    SoundBank.play("POWERUP_SPAWN")
end

function PowerUp:Update(dt, ball, p1, p2, ps)
    self.life = self.life - dt
    local dx = ball.x - self.x
    local dy = ball.y - self.y
    local dist = math.sqrt(dx*dx + dy*dy)

    if dist < (self.size * 0.5 + ball.size * 0.5) then
        SoundBank.play("POWERUP_COLLECT")
        ScreenShake.add(0.16)
        if ps then
            ps:SpawnHitParticles(self.x, self.y, self.type.color, 18)
            ps:AddPopup("POWER UP: " .. self.type.name, self.x - 50, self.y - 25, self.type.color, 1.2)
        end

        local targetPaddle = (ball.vx > 0) and p1 or p2
        local otherPaddle  = (ball.vx > 0) and p2 or p1

        if self.type.name == "BIG_PADDLE" then
            targetPaddle.h = targetPaddle.baseHeight * 1.5
        elseif self.type.name == "SHIELD_WALL" then
            targetPaddle.shieldActive = true
            targetPaddle.shieldTimer = 8.0
            SoundBank.play("SHIELD_BLOCK")
        elseif self.type.name == "EMP_FREEZE" then
            otherPaddle.freezeTimer = 3.5
            SoundBank.play("EMP_FREEZE")
        elseif self.type.name == "LASER_GUN" then
            targetPaddle.laserAmmo = targetPaddle.laserAmmo + 3
        elseif self.type.name == "MAGNET_CORE" then
            targetPaddle.magnetActive = true
            targetPaddle.magnetTimer = 6.0
        end

        return true
    end
    return self.life <= 0
end

function PowerUp:Draw(ox, oy)
    ox = ox or 0
    oy = oy or 0
    local half = self.size * 0.5
    local c = self.type.color
    draw.rect_filled(ox + self.x - half - 3, oy + self.y - half - 3, self.size + 6, self.size + 6, c[1], c[2], c[3], 0.4)
    draw.rect_filled(ox + self.x - half, oy + self.y - half, self.size, self.size, c[1], c[2], c[3], 0.9)
    draw.text(ox + self.x - 10, oy + self.y - 6, self.type.icon, Palette.White[1], Palette.White[2], Palette.White[3], 1.0, 0.9)
end

-- ============================================================================
-- Laser Bolts
-- ============================================================================
local Laser = { bolts = {} }
function Laser.fire(x, y, dir, color, shooterId)
    SoundBank.play("LASER_FIRE")
    table.insert(Laser.bolts, {
        x = x, y = y,
        vx = dir * 1400.0,
        color = color,
        shooterId = shooterId,
        life = 2.0
    })
end

function Laser.update(dt, p1, p2, ps)
    for i = #Laser.bolts, 1, -1 do
        local b = Laser.bolts[i]
        b.x = b.x + b.vx * dt
        b.life = b.life - dt

        local target = (b.shooterId == 1) and p2 or p1
        if b.x >= target.x and b.x <= target.x + target.w and
           b.y >= target.y and b.y <= target.y + target.h then
            target.freezeTimer = 2.0
            SoundBank.play("LASER_HIT")
            ScreenShake.add(0.14)
            if ps then
                ps:SpawnHitParticles(b.x, b.y, b.color, 14)
                ps:AddPopup("STUNNED!", target.x, target.y - 18, Palette.Magenta, 1.2)
            end
            b.life = 0
        end

        if b.life <= 0 or b.x < 0 or b.x > Constants.ScreenWidth then
            table.remove(Laser.bolts, i)
        end
    end
end

function Laser.draw(ox, oy)
    for _, b in ipairs(Laser.bolts) do
        draw.rect_filled(ox + b.x - 10, oy + b.y - 2, 20, 4, b.color[1], b.color[2], b.color[3], 1.0)
        draw.rect_filled(ox + b.x - 6, oy + b.y - 1, 12, 2, Palette.White[1], Palette.White[2], Palette.White[3], 1.0)
    end
end

-- ============================================================================
-- GameManager Class (OOP)
-- ============================================================================
local GameManager = Class()
function GameManager:Init()
    self.state = "TITLE"
    self.mode = "CLASSIC_1P" -- "CLASSIC_1P", "CLASSIC_2P", "GRAND_PRIX", "CHAOS", "SURVIVAL", "SOUND_TEST"
    self.crtIndex = 1
    self.currentStageIndex = 1
    self.ps = ParticleSystem()
    self.p1 = Paddle(60, Constants.ScreenHeight * 0.5 - Constants.PaddleHeight * 0.5, "W", "S", Palette.White, false)
    self.p2 = Paddle(Constants.ScreenWidth - 60 - Constants.PaddleWidth, Constants.ScreenHeight * 0.5 - Constants.PaddleHeight * 0.5, "UP", "DOWN", Palette.White, true, "Master")
    self.ball = Ball(self.ps)
    self.powerups = {}
    self.powerupSpawnTimer = 6.0
    self.countdownTimer = 3.0
    self.lastCountdownTick = 4
    self.goalTimer = 0.0
    self.winner = 0
    self.maxRally = 0
    self.maxSpeed = 0
    self.survivalScore = 0
    self.soundTestIndex = 1
    self.showTelemetry = false
end

function GameManager:IsClassic()
    return (self.mode == "CLASSIC_1P" or self.mode == "CLASSIC_2P")
end

function GameManager:StartMatch(mode, stageIndex)
    self.mode = mode
    self.currentStageIndex = stageIndex or 1
    local isClassic = self:IsClassic()
    local currentStage = Stages[self.currentStageIndex]

    self.p1:Reset()
    self.p1.score = 0
    self.p2:Reset()
    self.p2.score = 0

    if isClassic then
        self.p1.color = CRTModes[self.crtIndex].fg
        self.p2.color = CRTModes[self.crtIndex].fg
        self.ball.color = CRTModes[self.crtIndex].fg
    else
        self.p1.color = Palette.Cyan
        self.p2.color = Palette.Magenta
        self.ball.color = currentStage.ballColor
    end

    self.p2.isAI = (mode ~= "CLASSIC_2P")
    self.p2.aiDifficulty = currentStage.aiDifficulty

    self.powerups = {}
    self.winner = 0
    self.maxRally = 0
    self.maxSpeed = 0
    self.survivalScore = 0
    self.ball:Reset(1)
    self.ball.active = false
    self.countdownTimer = 3.0
    self.lastCountdownTick = 4
    self.state = "COUNTDOWN"
    SoundBank.play("MATCH_START")
end

function GameManager:OnGoal(scorer)
    self.goalTimer = 1.2
    self.state = "GOAL"
    SoundBank.play("GOAL_EXPLOSION")
    ScreenShake.add(0.35)

    local isClassic = self:IsClassic()
    local currentStage = Stages[self.currentStageIndex]
    local scorerName = (scorer == 1) and "PLAYER 1" or (mode == "CLASSIC_2P" and "PLAYER 2" or (isClassic and "AI CPU" or currentStage.bossName))
    local color = (scorer == 1) and (isClassic and CRTModes[self.crtIndex].fg or Palette.Cyan) or (isClassic and CRTModes[self.crtIndex].accent or Palette.Magenta)

    if not isClassic then
        self.ps:SpawnHitParticles(Constants.ScreenWidth * 0.5, Constants.ScreenHeight * 0.5, color, 30, 2.0)
        self.ps:AddPopup("POINT SCORED BY " .. scorerName .. "!", Constants.ScreenWidth * 0.5 - 160, Constants.ScreenHeight * 0.5 - 30, color, 1.5)
    end
end

function GameManager:Update(dt)
    local isClassic = self:IsClassic()
    local currentStage = Stages[self.currentStageIndex]
    local crtMode = CRTModes[self.crtIndex]

    ScreenShake.update(dt)
    SoundBank.updateBGM(dt, (self.state == "PLAYING") and (self.ball.speed / Constants.BallBaseSpeed) or 1.0)
    self.ps:Update(dt)

    -- Toggle Telemetry HUD (F3)
    if input.is_just_pressed("F3") then
        self.showTelemetry = not self.showTelemetry
        SoundBank.play("UI_TOGGLE")
    end

    -- Toggle CRT Filter Mode
    if input.is_just_pressed("C") then
        self.crtIndex = (self.crtIndex % #CRTModes) + 1
        SoundBank.play("UI_TOGGLE")
        if isClassic then
            self.p1.color = CRTModes[self.crtIndex].fg
            self.p2.color = CRTModes[self.crtIndex].fg
        end
    end

    -- 1. TITLE SCREEN
    if self.state == "TITLE" then
        if input.is_just_pressed("1") or input.is_just_pressed("Space") or input.is_just_pressed("Return") or input.is_just_pressed("Enter") then
            self:StartMatch("CLASSIC_1P")
        elseif input.is_just_pressed("2") then
            self:StartMatch("CLASSIC_2P")
        elseif input.is_just_pressed("3") then
            SoundBank.play("UI_NAV")
            self.state = "STAGE_SELECT"
        elseif input.is_just_pressed("4") then
            self:StartMatch("CHAOS", 5)
        elseif input.is_just_pressed("5") then
            self:StartMatch("SURVIVAL", 5)
        elseif input.is_just_pressed("6") then
            SoundBank.play("UI_SELECT")
            self.state = "SOUND_TEST"
        end

    -- 2. STAGE SELECT (TURBO GRAND PRIX)
    elseif self.state == "STAGE_SELECT" then
        if input.is_just_pressed("1") then self:StartMatch("GRAND_PRIX", 1) end
        if input.is_just_pressed("2") then self:StartMatch("GRAND_PRIX", 2) end
        if input.is_just_pressed("3") then self:StartMatch("GRAND_PRIX", 3) end
        if input.is_just_pressed("4") then self:StartMatch("GRAND_PRIX", 4) end
        if input.is_just_pressed("5") then self:StartMatch("GRAND_PRIX", 5) end
        if input.is_just_pressed("Escape") then
            SoundBank.play("UI_BACK")
            self.state = "TITLE"
        end

    -- 3. SOUND TEST MODE
    elseif self.state == "SOUND_TEST" then
        local soundList = {
            "UI_NAV", "UI_SELECT", "UI_BACK", "UI_TOGGLE",
            "COUNTDOWN_TICK", "COUNTDOWN_GO",
            "PADDLE_HIT_CLASSIC", "PADDLE_HIT_TURBO", "WALL_BOUNCE",
            "BALL_SMASH", "BALL_SERVE", "BUMPER_DEFLECT", "WARP_GATE",
            "POWERUP_SPAWN", "POWERUP_COLLECT",
            "LASER_FIRE", "LASER_HIT", "SHIELD_BLOCK", "EMP_FREEZE",
            "GOAL_EXPLOSION", "VICTORY_FANFARE", "DEFEAT_CADENCE"
        }
        if input.is_just_pressed("Up") or input.is_just_pressed("W") then
            self.soundTestIndex = (self.soundTestIndex - 2 + #soundList) % #soundList + 1
            SoundBank.play("UI_NAV")
        elseif input.is_just_pressed("Down") or input.is_just_pressed("S") then
            self.soundTestIndex = (self.soundTestIndex % #soundList) + 1
            SoundBank.play("UI_NAV")
        elseif input.is_just_pressed("Space") or input.is_just_pressed("Return") or input.is_just_pressed("Enter") then
            SoundBank.play(soundList[self.soundTestIndex])
        elseif input.is_just_pressed("B") then
            if SoundBank.bgmStyle == "RETRO_BASS" then SoundBank.bgmStyle = "CLASSIC_PULSE"
            elseif SoundBank.bgmStyle == "CLASSIC_PULSE" then SoundBank.bgmStyle = "OFF"
            else SoundBank.bgmStyle = "RETRO_BASS" end
            SoundBank.play("UI_TOGGLE")
        elseif input.is_just_pressed("Escape") then
            SoundBank.play("UI_BACK")
            self.state = "TITLE"
        end

    -- 4. COUNTDOWN
    elseif self.state == "COUNTDOWN" then
        self.countdownTimer = self.countdownTimer - dt
        local currentTick = math.ceil(self.countdownTimer)
        if currentTick < self.lastCountdownTick and currentTick > 0 then
            self.lastCountdownTick = currentTick
            SoundBank.play("COUNTDOWN_TICK")
        end
        if self.countdownTimer <= 0 then
            self.state = "PLAYING"
            SoundBank.play("COUNTDOWN_GO")
        end

    -- 5. PLAYING
    elseif self.state == "PLAYING" then
        self.p1:Update(dt, self.ball, self.ps, false, isClassic)
        self.p2:Update(dt, self.ball, self.ps, (self.mode == "CLASSIC_2P"), isClassic)
        self.ball:Update(dt, self.p1, self.p2, isClassic, currentStage)

        if self.ball.rally > self.maxRally then self.maxRally = self.ball.rally end
        if self.ball.speed > self.maxSpeed then self.maxSpeed = self.ball.speed end
        if self.mode == "SURVIVAL" then
            self.survivalScore = self.survivalScore + math.floor(dt * 100 * (self.ball.speed / Constants.BallBaseSpeed))
        end

        -- Turbo Specific Triggers (Lasers & Overdrive)
        if not isClassic then
            if input.is_just_pressed("Space") and self.p1.laserAmmo > 0 then
                self.p1.laserAmmo = self.p1.laserAmmo - 1
                Laser.fire(self.p1.x + self.p1.w + 10, self.p1.y + self.p1.h * 0.5, 1, Palette.Cyan, 1)
            end
            if (input.is_just_pressed("E") or input.is_just_pressed("Q")) and self.p1.hyperMeter >= 100.0 then
                self.p1.hyperMeter = 0.0
                self.ball.isSmash = true
                self.ball.speed = math.min(Constants.MaxBallSpeed, self.ball.speed + 350)
                SoundBank.play("BALL_SMASH", 1.4)
                ScreenShake.add(0.32)
                self.ps:SpawnHitParticles(self.p1.x, self.p1.y, Palette.Gold, 28, 1.8)
                self.ps:AddPopup("[P1] EX OVERDRIVE!", self.p1.x + 30, self.p1.y - 25, Palette.Gold, 1.6)
            end

            Laser.update(dt, self.p1, self.p2, self.ps)

            if self.mode == "CHAOS" or self.mode == "GRAND_PRIX" or self.mode == "SURVIVAL" then
                self.powerupSpawnTimer = self.powerupSpawnTimer - dt
                if self.powerupSpawnTimer <= 0 and #self.powerups < 3 then
                    self.powerupSpawnTimer = (self.mode == "CHAOS") and 3.5 or 7.5
                    local px = math.random(Constants.ScreenWidth * 0.3, Constants.ScreenWidth * 0.7)
                    local py = math.random(80, Constants.ScreenHeight - 80)
                    table.insert(self.powerups, PowerUp(px, py))
                end
            end

            for i = #self.powerups, 1, -1 do
                if self.powerups[i]:Update(dt, self.ball, self.p1, self.p2, self.ps) then
                    table.remove(self.powerups, i)
                end
            end
        end

        -- Goal Detection
        if self.ball.x < 0 then
            self.p2.score = self.p2.score + 1
            self:TriggerGoal(2)
        elseif self.ball.x > Constants.ScreenWidth then
            self.p1.score = self.p1.score + 1
            self:TriggerGoal(1)
        end

        if input.is_just_pressed("Escape") then
            SoundBank.play("UI_BACK")
            self.state = "TITLE"
        end

    -- 6. GOAL DELAY
    elseif self.state == "GOAL" then
        self.goalTimer = self.goalTimer - dt
        if self.goalTimer <= 0 then
            local maxScore = isClassic and Constants.ClassicMaxScore or Constants.TurboMaxScore
            if self.mode == "SURVIVAL" and self.winner == 2 then
                self.state = "GAMEOVER"
                SoundBank.play("DEFEAT_CADENCE")
            elseif self.p1.score >= maxScore or self.p2.score >= maxScore then
                self.winner = (self.p1.score >= maxScore) and 1 or 2
                self.state = (self.winner == 1) and "VICTORY" or "GAMEOVER"
                if self.winner == 1 then
                    SoundBank.play("VICTORY_FANFARE")
                else
                    SoundBank.play("DEFEAT_CADENCE")
                end
            else
                self.state = "COUNTDOWN"
                self.countdownTimer = 1.5
                self.lastCountdownTick = 2
                self.ball:Serve((self.winner == 1) and -1 or 1, isClassic)
            end
        end

    -- 7. MATCH END
    elseif self.state == "VICTORY" or self.state == "GAMEOVER" then
        if input.is_just_pressed("Return") or input.is_just_pressed("Enter") or input.is_just_pressed("Space") then
            if self.state == "VICTORY" and self.mode == "GRAND_PRIX" and self.currentStageIndex < #Stages then
                self:StartMatch("GRAND_PRIX", self.currentStageIndex + 1)
            else
                self:StartMatch(self.mode, self.currentStageIndex)
            end
        elseif input.is_just_pressed("Escape") then
            SoundBank.play("UI_BACK")
            self.state = "TITLE"
        end
    end
end

function GameManager:Draw()
    local ox = ScreenShake.offsetX
    local oy = ScreenShake.offsetY
    local w = Constants.ScreenWidth
    local h = Constants.ScreenHeight
    local isClassic = self:IsClassic()
    local crtMode = CRTModes[self.crtIndex]
    local currentStage = Stages[self.currentStageIndex]

    -- Background
    if isClassic then
        local bg = crtMode.bg
        draw.rect_filled(ox, oy, w, h, bg[1], bg[2], bg[3], 1.0)
        -- Top/Bottom Border Bars
        draw.rect_filled(ox, oy, w, 14, crtMode.fg[1], crtMode.fg[2], crtMode.fg[3], 1.0)
        draw.rect_filled(ox, oy + h - 14, w, 14, crtMode.fg[1], crtMode.fg[2], crtMode.fg[3], 1.0)
        -- Center Dashed Divider
        for y = 20, h - 20, 28 do
            draw.rect_filled(ox + w * 0.5 - 4, oy + y, 8, 14, crtMode.fg[1], crtMode.fg[2], crtMode.fg[3], 0.85)
        end
    else
        -- Outrun Synthwave Background
        draw.rect_filled(ox, oy, w, h, Palette.BgDark[1], Palette.BgDark[2], Palette.BgDark[3], 1.0)
        -- Retro Sun
        local sc = currentStage.sunColor
        draw.rect_filled(ox + w*0.5 - 80, oy + h*0.38 - 80, 160, 160, sc[1], sc[2]*0.2, sc[3]*0.4, 0.35)
        draw.rect_filled(ox + w*0.5 - 65, oy + h*0.38 - 65, 130, 130, sc[1], sc[2], sc[3]*0.2, 0.75)
        -- Horizon & Floor Grid
        local horizonY = h * 0.55
        draw.rect_filled(ox, oy + horizonY, w, 2, currentStage.primaryColor[1], currentStage.primaryColor[2], currentStage.primaryColor[3], 0.95)
        for i = 1, 9 do
            local p = (i / 9)
            local y = horizonY + (p * p) * (h - horizonY)
            draw.rect_filled(ox, oy + y, w, 2, currentStage.secondaryColor[1], currentStage.secondaryColor[2], currentStage.secondaryColor[3], p * 0.6)
        end
        -- Center Court Divider
        for y = 20, h - 20, 30 do
            draw.rect_filled(ox + w*0.5 - 2, oy + y, 4, 16, currentStage.primaryColor[1], currentStage.primaryColor[2], currentStage.primaryColor[3], 0.5)
        end
    end

    self.ps:Draw(ox, oy)

    -- Gameplay Elements
    if self.state == "PLAYING" or self.state == "COUNTDOWN" or self.state == "GOAL" then
        if not isClassic then
            for _, pu in ipairs(self.powerups) do pu:Draw(ox, oy) end
            Laser.draw(ox, oy)
        end
        self.p1:Draw(ox, oy, isClassic, crtMode)
        self.p2:Draw(ox, oy, isClassic, crtMode)
        self.ball:Draw(ox, oy, isClassic, crtMode)

        -- Score Display
        if isClassic then
            -- Authentic Segmented Scores at Top
            DrawClassicScore(w * 0.28, 80, self.p1.score, 60, crtMode.fg)
            DrawClassicScore(w * 0.72, 80, self.p2.score, 60, crtMode.fg)
            draw.text(w * 0.5 - 90, 24, "[C] FILTER: " .. crtMode.name, crtMode.dim[1], crtMode.dim[2], crtMode.dim[3], 0.9, 0.85)
        else
            -- Turbo Cards
            local barW = 240
            draw.rect_filled(35, 20, barW, 46, Palette.BgDark[1], Palette.BgDark[2], Palette.BgDark[3], 0.88)
            draw.rect(35, 20, barW, 46, Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 0.8)
            draw.text(45, 26, "PLAYER 1", Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 1.0, 1.0)
            draw.text(45, 44, string.format("SCORE: %d/%d | LZ: %d", self.p1.score, Constants.TurboMaxScore, self.p1.laserAmmo), Palette.White[1], Palette.White[2], Palette.White[3], 0.9, 0.85)

            draw.rect_filled(w - 35 - barW, 20, barW, 46, Palette.BgDark[1], Palette.BgDark[2], Palette.BgDark[3], 0.88)
            draw.rect(w - 35 - barW, 20, barW, 46, Palette.Magenta[1], Palette.Magenta[2], Palette.Magenta[3], 0.8)
            local p2Label = (self.mode == "VERSUS") and "PLAYER 2" or currentStage.bossName
            draw.text(w - 25 - barW, 26, p2Label, Palette.Magenta[1], Palette.Magenta[2], Palette.Magenta[3], 1.0, 1.0)
            draw.text(w - 25 - barW, 44, string.format("SCORE: %d/%d", self.p2.score, Constants.TurboMaxScore), Palette.White[1], Palette.White[2], Palette.White[3], 0.9, 0.85)

            local speedMph = math.floor(self.ball.speed * 0.15)
            draw.rect_filled(w * 0.5 - 130, 20, 260, 46, Palette.BgDark[1], Palette.BgDark[2], Palette.BgDark[3], 0.88)
            draw.rect(w * 0.5 - 130, 20, 260, 46, Palette.Gold[1], Palette.Gold[2], Palette.Gold[3], 0.6)
            draw.text(w * 0.5 - 110, 26, currentStage.name, Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 0.9, 0.85)
            draw.text(w * 0.5 - 110, 44, string.format("RALLY: %02d | %d MPH", self.ball.rally, speedMph), Palette.Gold[1], Palette.Gold[2], Palette.Gold[3], 1.0, 0.85)
        end

        if self.state == "COUNTDOWN" then
            local countText = (self.countdownTimer > 2.0) and "3" or ((self.countdownTimer > 1.0) and "2" or "1")
            local textColor = isClassic and crtMode.accent or Palette.Gold
            draw.text(w * 0.5 - 20, h * 0.42, countText, textColor[1], textColor[2], textColor[3], 1.0, 3.0)
        end

    -- 7. TITLE SCREEN (CLASSIC DEFAULT)
    elseif self.state == "TITLE" then
        draw.rect_filled(0, 0, w, h, 0.0, 0.0, 0.0, 0.5)

        draw.text(w * 0.5 - 180, h * 0.12, "FUSION PONG", crtMode.fg[1], crtMode.fg[2], crtMode.fg[3], 1.0, 3.2)
        draw.text(w * 0.5 - 220, h * 0.22, "AUTHENTIC CLASSIC 1972 & TURBO ARCADE SUITE", crtMode.accent[1], crtMode.accent[2], crtMode.accent[3], 1.0, 1.1)

        local boxW = 580
        local boxH = 300
        local boxX = w * 0.5 - boxW * 0.5
        local boxY = h * 0.32

        draw.rect_filled(boxX, boxY, boxW, boxH, crtMode.bg[1], crtMode.bg[2], crtMode.bg[3], 0.94)
        draw.rect(boxX, boxY, boxW, boxH, crtMode.fg[1], crtMode.fg[2], crtMode.fg[3], 0.9)

        draw.text(boxX + 30, boxY + 22, "[1] CLASSIC PONG (1P vs AI) - [DEFAULT]", crtMode.fg[1], crtMode.fg[2], crtMode.fg[3], 1.0, 1.15)
        draw.text(boxX + 30, boxY + 65, "[2] CLASSIC PONG (2-Player Local PvP)", crtMode.accent[1], crtMode.accent[2], crtMode.accent[3], 1.0, 1.1)
        draw.text(boxX + 30, boxY + 108, "[3] TURBO GRAND PRIX (5 Boss Stages)", Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 1.0, 1.1)
        draw.text(boxX + 30, boxY + 151, "[4] CHAOS TURBO (Power-Ups & Lasers)", Palette.Gold[1], Palette.Gold[2], Palette.Gold[3], 1.0, 1.1)
        draw.text(boxX + 30, boxY + 194, "[5] ENDLESS SURVIVAL (Infinite Speed)", Palette.Electric[1], Palette.Electric[2], Palette.Electric[3], 1.0, 1.1)
        draw.text(boxX + 30, boxY + 237, "[6] SOUND TEST & AUDIO SUITE", Palette.Magenta[1], Palette.Magenta[2], Palette.Magenta[3], 1.0, 1.1)

        draw.text(w * 0.5 - 340, h - 50, "[1-6 / Enter] Select Mode | [C] Toggle CRT Filter | [F5] Hot-Reload", crtMode.dim[1], crtMode.dim[2], crtMode.dim[3], 0.9, 0.9)

    -- 8. STAGE SELECT
    elseif self.state == "STAGE_SELECT" then
        draw.rect_filled(0, 0, w, h, 0.0, 0.0, 0.6)
        draw.text(w * 0.5 - 180, h * 0.14, "SELECT GRAND PRIX STAGE", Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 1.0, 2.0)

        local boxW = 560
        local boxH = 280
        local boxX = w * 0.5 - boxW * 0.5
        local boxY = h * 0.28

        draw.rect_filled(boxX, boxY, boxW, boxH, Palette.BgDark[1], Palette.BgDark[2], Palette.BgDark[3], 0.92)
        draw.rect(boxX, boxY, boxW, boxH, Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 0.8)

        draw.text(boxX + 25, boxY + 20, "[1] STAGE 1: NEO TOKYO (vs Neon Novice)", Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 1.0, 1.0)
        draw.text(boxX + 25, boxY + 60, "[2] STAGE 2: SYNTH HIGHWAY (vs Cyber Phantom)", Palette.Magenta[1], Palette.Magenta[2], Palette.Magenta[3], 1.0, 1.0)
        draw.text(boxX + 25, boxY + 100, "[3] STAGE 3: OBSIDIAN MATRIX (vs Viper Blade)", Palette.Matrix[1], Palette.Matrix[2], Palette.Matrix[3], 1.0, 1.0)
        draw.text(boxX + 25, boxY + 140, "[4] STAGE 4: NEON ORBITAL (vs Aegis Dreadnought)", Palette.Crimson[1], Palette.Crimson[2], Palette.Crimson[3], 1.0, 1.0)
        draw.text(boxX + 25, boxY + 180, "[5] STAGE 5: SOLAR CORE (vs CYBER-GOD 9000)", Palette.Gold[1], Palette.Gold[2], Palette.Gold[3], 1.0, 1.0)
        draw.text(boxX + 25, boxY + 235, "[ESC] RETURN TO MAIN MENU", Palette.Muted[1], Palette.Muted[2], Palette.Muted[3], 0.9, 0.9)

    -- 9. SOUND TEST SCREEN
    elseif self.state == "SOUND_TEST" then
        draw.rect_filled(0, 0, w, h, 0.0, 0.0, 0.7)
        draw.text(w * 0.5 - 200, h * 0.12, "PROCEDURAL SOUND TEST", Palette.Magenta[1], Palette.Magenta[2], Palette.Magenta[3], 1.0, 2.2)

        local boxW = 600
        local boxH = 340
        local boxX = w * 0.5 - boxW * 0.5
        local boxY = h * 0.26

        draw.rect_filled(boxX, boxY, boxW, boxH, Palette.BgDark[1], Palette.BgDark[2], Palette.BgDark[3], 0.94)
        draw.rect(boxX, boxY, boxW, boxH, Palette.Magenta[1], Palette.Magenta[2], Palette.Magenta[3], 0.8)

        local soundList = {
            "UI_NAV", "UI_SELECT", "UI_BACK", "UI_TOGGLE",
            "COUNTDOWN_TICK", "COUNTDOWN_GO",
            "PADDLE_HIT_CLASSIC", "PADDLE_HIT_TURBO", "WALL_BOUNCE",
            "BALL_SMASH", "BALL_SERVE", "BUMPER_DEFLECT", "WARP_GATE",
            "POWERUP_SPAWN", "POWERUP_COLLECT",
            "LASER_FIRE", "LASER_HIT", "SHIELD_BLOCK", "EMP_FREEZE",
            "GOAL_EXPLOSION", "VICTORY_FANFARE", "DEFEAT_CADENCE"
        }

        local startIdx = math.max(1, self.soundTestIndex - 4)
        local endIdx = math.min(#soundList, startIdx + 8)

        for i = startIdx, endIdx do
            local isSelected = (i == self.soundTestIndex)
            local yPos = boxY + 20 + (i - startIdx) * 28
            local textColor = isSelected and Palette.Gold or Palette.White
            local prefix = isSelected and "-> " or "   "
            draw.text(boxX + 30, yPos, string.format("%s[%02d] %s", prefix, i, soundList[i]), textColor[1], textColor[2], textColor[3], 1.0, 1.0)
        end

        draw.text(boxX + 30, boxY + boxH - 45, "[W/S / Arrows] Select | [SPACE] Play Sound | [B] BGM: " .. SoundBank.bgmStyle, Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 1.0, 0.95)
        draw.text(boxX + 30, boxY + boxH - 22, "[ESC] Return to Main Menu", Palette.Muted[1], Palette.Muted[2], Palette.Muted[3], 0.9, 0.85)

    -- 10. VICTORY / GAMEOVER
    elseif self.state == "VICTORY" or self.state == "GAMEOVER" then
        draw.rect_filled(0, 0, w, h, 0.0, 0.0, 0.72)

        local isVic = (self.state == "VICTORY")
        local bannerText = isVic and "MATCH COMPLETED - VICTORY!" or (self.mode == "SURVIVAL" and "SURVIVAL RUN OVER!" or "GAME OVER - DEFEATED!")
        local bannerColor = isVic and (isClassic and crtMode.fg or Palette.Gold) or (isClassic and crtMode.accent or Palette.Magenta)

        draw.text(w * 0.5 - 200, h * 0.16, bannerText, bannerColor[1], bannerColor[2], bannerColor[3], 1.0, 2.0)

        local boxW = 480
        local boxH = 240
        local boxX = w * 0.5 - boxW * 0.5
        local boxY = h * 0.30

        draw.rect_filled(boxX, boxY, boxW, boxH, isClassic and crtMode.bg[1] or Palette.BgDark[1], isClassic and crtMode.bg[2] or Palette.BgDark[2], isClassic and crtMode.bg[3] or Palette.BgDark[3], 0.92)
        draw.rect(boxX, boxY, boxW, boxH, bannerColor[1], bannerColor[2], bannerColor[3], 0.8)

        if self.mode == "SURVIVAL" then
            draw.text(boxX + 25, boxY + 25, string.format("SURVIVAL SCORE: %d PTS", self.survivalScore), Palette.Gold[1], Palette.Gold[2], Palette.Gold[3], 1.0, 1.2)
        else
            draw.text(boxX + 25, boxY + 25, string.format("FINAL SCORE: %d - %d", self.p1.score, self.p2.score), Palette.White[1], Palette.White[2], Palette.White[3], 1.0, 1.1)
        end
        draw.text(boxX + 25, boxY + 65, string.format("LONGEST RALLY: %d Hits", self.maxRally), Palette.Cyan[1], Palette.Cyan[2], Palette.Cyan[3], 1.0, 1.0)
        draw.text(boxX + 25, boxY + 100, string.format("MAX BALL SPEED: %d MPH", math.floor(self.maxSpeed * 0.15)), Palette.Orange[1], Palette.Orange[2], Palette.Orange[3], 1.0, 1.0)
        
        local nextText = (isVic and self.mode == "GRAND_PRIX" and self.currentStageIndex < #Stages)
            and "PRESS [ENTER] FOR NEXT STAGE | [ESC] MENU"
            or "PRESS [ENTER] TO PLAY AGAIN | [ESC] MENU"
        draw.text(boxX + 25, boxY + 160, nextText, Palette.Gold[1], Palette.Gold[2], Palette.Gold[3], 1.0, 0.95)
    end

    if self.showTelemetry and engine and engine.get_telemetry then
        local telem = engine.get_telemetry()
        local boxW, boxH = 430, 95
        local bx, by = w - boxW - 20, 20
        draw.rect_filled(bx, by, boxW, boxH, 0.04, 0.07, 0.1, 0.92)
        draw.rect(bx, by, boxW, boxH, 0.0, 0.9, 1.0, 0.85)
        draw.text(bx + 12, by + 10, "STARLIGHT LIVE TELEMETRY (F3)", 0.0, 1.0, 0.9, 1.0, 0.95)
        draw.text(bx + 12, by + 30, string.format("FPS: %d (%.2f ms) | Target: %d | Entities: %d", telem.fps or 60, (telem.avg_frame_time or 0.016)*1000.0, telem.target_fps or 60, telem.entities_count or 0), 0.85, 0.9, 0.95, 1.0, 0.82)
        draw.text(bx + 12, by + 50, string.format("Draw Calls: %d | Quads: %d | Audio Voices: %d", telem.draw_calls or 0, telem.quad_count or 0, telem.active_audio_voices or 0), 0.85, 0.9, 0.95, 1.0, 0.82)
        draw.text(bx + 12, by + 70, string.format("Memory Alloc: %.1f / %.1f KB", telem.memory_used_kb or 0, telem.memory_capacity_kb or 0), 0.3, 1.0, 0.5, 1.0, 0.82)
    end
end

-- Global Engine Instance
local GlobalGame = nil

function OnStart()
    engine.log("Fusion Pong: Initializing Authentic Classic 1972 & Arcade Suite v10.0...")
    GlobalGame = GameManager()
end

function OnUpdate(dt)
    if GlobalGame then GlobalGame:Update(dt) end
end

function OnRenderUI()
    if GlobalGame then GlobalGame:Draw() end
end

OnRender = function() end
OnUIRender = OnRenderUI
