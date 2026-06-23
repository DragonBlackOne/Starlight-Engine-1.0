
Add-Type -AssemblyName System.Speech
$synth = New-Object System.Speech.Synthesis.SpeechSynthesizer

# Configure Voice
# Try to select a "Robot-like" or distinct voice if available, otherwise default
# $synth.SelectVoiceByHints('Male', 'Adult') 

$synth.Rate = 0  # Slightly slower speech
$synth.Volume = 100

function Save-SpeechToWav($text, $filename) {
    $fullPath = Join-Path (Get-Location) "assets/audio/$filename"
    Write-Host "Generating: $fullPath"
    $synth.SetOutputToWaveFile($fullPath)
    $synth.Speak($text)
    $synth.SetOutputToNull() # Reset output
}

# Ensure directory exists
$audioDir = Join-Path (Get-Location) "assets/audio"
if (-not (Test-Path $audioDir)) {
    New-Item -ItemType Directory -Force -Path $audioDir
}

# Generate Files
Save-SpeechToWav "Three" "announcer_3.wav"
Save-SpeechToWav "Two" "announcer_2.wav"
Save-SpeechToWav "One" "announcer_1.wav"
Save-SpeechToWav "Go!" "announcer_go.wav"
Save-SpeechToWav "Winner" "announcer_winner.wav"
Save-SpeechToWav "Defeat" "announcer_defeat.wav"

# New Lines for Phase 18
Save-SpeechToWav "Nice" "announcer_nice.wav"
Save-SpeechToWav "Ouch" "announcer_ouch.wav"
Save-SpeechToWav "Excellent" "announcer_excellent.wav"
Save-SpeechToWav "Dominating" "announcer_dominating.wav"

$synth.Dispose()
Write-Host "Done."
