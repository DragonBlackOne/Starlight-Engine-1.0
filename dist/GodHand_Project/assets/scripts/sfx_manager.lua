-- sfx_manager.lua
-- Sound effects & Audio synthesis manager for God Hand 3D Brawler

local SFX = {}

function SFX.init()
    SFX.bgmTimer = 0
    SFX.bgmPattern = 0
    SFX.bgmNextNote = 0
    SFX.bassNotes = { 110.0, 110.0, 130.81, 146.83, 164.81, 146.83, 130.81, 98.0 }
    SFX.leadNotes = { 220.0, 261.63, 293.66, 329.63, 392.0, 329.63, 293.66, 261.63 }
    SFX.bgmPlaying = true
    SFX.bgmTempo = 0.16 -- 16th notes
end

function SFX.playPunchLight()
    if audio and audio.play_impact then
        audio.play_impact(10.0, 3)
    else
        audio.beep(380.0, 0.04, 2)
        audio.play_note(220.0, 0.05, 1)
    end
end

function SFX.playPunchMedium()
    if audio and audio.play_impact then
        audio.play_impact(18.0, 3)
    else
        audio.beep(240.0, 0.06, 2)
        audio.play_note(140.0, 0.08, 1)
    end
    if camera and camera.shake then camera.shake(0.2) end
end

function SFX.playPunchHeavy()
    if audio and audio.play_impact then
        audio.play_impact(28.0, 3)
    else
        audio.beep(160.0, 0.12, 2)
        audio.play_note(85.0, 0.15, 0)
    end
    if camera and camera.shake then camera.shake(0.5) end
end

function SFX.playGuardBreak()
    if audio and audio.play_impact then
        audio.play_impact(24.0, 5) -- Glass/Armor shatter
    else
        audio.beep(880.0, 0.08, 2)
        audio.play_note(1200.0, 0.12, 3)
        audio.play_note(440.0, 0.18, 0)
    end
    if camera and camera.shake then camera.shake(0.35) end
end

function SFX.playLauncher()
    -- Ascending launch whoosh & impact
    audio.play_note(180.0, 0.06, 1)
    audio.play_note(360.0, 0.09, 3)
    audio.play_note(720.0, 0.15, 0)
end

function SFX.playGodHandActivate()
    -- Holy golden tension unleash chime
    audio.play_note(523.25, 0.25, 0) -- C5
    audio.play_note(659.25, 0.25, 0) -- E5
    audio.play_note(783.99, 0.35, 0) -- G5
    audio.play_note(1046.50, 0.50, 0) -- C6
    audio.beep(800.0, 0.2, 2)
end

function SFX.playGodReelOpen()
    -- Time-dilation warp hum
    audio.play_note(300.0, 0.25, 0)
    audio.play_note(200.0, 0.30, 0)
    audio.play_note(100.0, 0.40, 0)
end

function SFX.playGodReelSelect()
    audio.play_note(880.0, 0.08, 0)
end

function SFX.playGodReelExecute()
    -- Devastating God special release
    audio.play_note(110.0, 0.5, 3)
    audio.beep(200.0, 0.4, 2)
    audio.play_note(440.0, 0.4, 0)
    audio.play_note(880.0, 0.6, 0)
end

function SFX.playDodge()
    -- Fast air whoosh
    audio.beep(600.0, 0.05, 2)
end

function SFX.playDizzyBirds()
    -- Comical tweeting chirps
    audio.play_note(1500.0, 0.05, 0)
    audio.play_note(1800.0, 0.05, 0)
end

function SFX.playGrovel()
    -- Comical bowing whine
    audio.play_note(400.0, 0.15, 1)
    audio.play_note(250.0, 0.25, 1)
    audio.play_note(150.0, 0.35, 0)
end

function SFX.playLevelUp()
    -- Level DIE fanfare
    audio.play_note(440.0, 0.1, 3)
    audio.play_note(554.37, 0.1, 3)
    audio.play_note(659.25, 0.2, 3)
end

function SFX.playPropBreak()
    if audio and audio.play_impact then
        audio.play_impact(16.0, 1) -- Wood shatter
    else
        audio.beep(300.0, 0.15, 2)
        audio.play_note(90.0, 0.12, 1)
    end
    if camera and camera.shake then camera.shake(0.2) end
end

function SFX.playItemPickup()
    audio.play_note(600.0, 0.08, 0)
    audio.play_note(900.0, 0.15, 0)
end

function SFX.update(dt)
    if not SFX.bgmPlaying then return end

    SFX.bgmTimer = SFX.bgmTimer + dt
    if SFX.bgmTimer >= SFX.bgmTempo then
        SFX.bgmTimer = SFX.bgmTimer - SFX.bgmTempo
        SFX.bgmPattern = SFX.bgmPattern + 1

        local step = (SFX.bgmPattern % 8) + 1
        local bass = SFX.bassNotes[step]
        local lead = SFX.leadNotes[step]

        -- Rock/Western fighting bassline
        if step % 2 == 1 then
            audio.play_note(bass, 0.12, 3) -- Saw bass
            -- Kick drum simulation
            audio.beep(80.0, 0.06, 2)
        else
            -- Snare drum simulation on 2 and 4
            if step == 4 or step == 8 then
                audio.beep(350.0, 0.08, 2)
            end
        end

        -- Lead hook accents
        if SFX.bgmPattern % 4 == 0 then
            audio.play_note(lead * 1.5, 0.14, 0)
        end
    end
end

return SFX
