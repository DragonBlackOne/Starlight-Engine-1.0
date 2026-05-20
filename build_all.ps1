# ============================================================================
# Build All Projects - Fusion ENGINE
# Syncs framework scripts and builds all projects
# ============================================================================

$ErrorActionPreference = "Continue"
$Root = $PSScriptRoot
$Engine = Join-Path $Root "StarlightEngine"
$Projects = @("CapitalOdyssey", "Pong_Project", "Tetris_Project", "Snake_Project")

# 1. Sync Framework Assets (scripts, shaders, fonts) to all projects
Write-Host ">> Syncing SBA Framework to all projects..." -ForegroundColor Yellow
foreach ($proj in $Projects) {
    # Ensure assets directory exists
    $projAssets = Join-Path $Root "$proj\assets"
    if (!(Test-Path $projAssets)) { New-Item -ItemType Directory -Path $projAssets -Force | Out-Null }

    # Sync Scripts
    $destScripts = Join-Path $projAssets "scripts"
    if (!(Test-Path $destScripts)) { New-Item -ItemType Directory -Path $destScripts -Force | Out-Null }
    Copy-Item (Join-Path $Engine "assets\scripts\core.lua") -Destination $destScripts -Force
    Copy-Item (Join-Path $Engine "assets\scripts\sba_bridge.lua") -Destination $destScripts -Force -ErrorAction SilentlyContinue
    Write-Host "   Synced Scripts: $proj" -ForegroundColor DarkGray

    # Sync Shaders
    $destShaders = Join-Path $projAssets "shaders"
    if (!(Test-Path $destShaders)) { New-Item -ItemType Directory -Path $destShaders -Force | Out-Null }
    Copy-Item (Join-Path $Engine "assets\shaders\*.vert") -Destination $destShaders -Force -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $Engine "assets\shaders\*.frag") -Destination $destShaders -Force -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $Engine "assets\shaders\*.comp") -Destination $destShaders -Force -ErrorAction SilentlyContinue
    
    # Sync Compute Shaders (sub-directory)
    $destCompute = Join-Path $destShaders "compute"
    if (!(Test-Path $destCompute)) { New-Item -ItemType Directory -Path $destCompute -Force | Out-Null }
    Copy-Item (Join-Path $Engine "assets\shaders\compute\*.comp") -Destination $destCompute -Force -ErrorAction SilentlyContinue
    Write-Host "   Synced Shaders: $proj" -ForegroundColor DarkGray

    # Sync Fonts
    $destFonts = Join-Path $projAssets "fonts"
    if (!(Test-Path $destFonts)) { New-Item -ItemType Directory -Path $destFonts -Force | Out-Null }
    Copy-Item (Join-Path $Engine "assets\fonts\*") -Destination $destFonts -Force -Recurse
    Write-Host "   Synced Fonts: $proj" -ForegroundColor DarkGray

    # Sync Audio
    $destAudio = Join-Path $projAssets "audio"
    if (!(Test-Path $destAudio)) { New-Item -ItemType Directory -Path $destAudio -Force | Out-Null }
    Copy-Item (Join-Path $Engine "assets\audio\*") -Destination $destAudio -Force -Recurse -ErrorAction SilentlyContinue
    Write-Host "   Synced Audio: $proj" -ForegroundColor DarkGray
}

# 2. Build Engine (shared library)
Write-Host "`n>> Building StarlightEngine..." -ForegroundColor Green
Push-Location $Engine
cmake -B build -S . 2>$null
cmake --build build --config Release
Pop-Location

# 3. Build all game projects
foreach ($proj in $Projects) {
    $projDir = Join-Path $Root $proj
    if (Test-Path $projDir) {
        Write-Host "`n>> Building $proj..." -ForegroundColor Green
        Push-Location $projDir
        cmake -B build -S . 2>$null
        cmake --build build --config Release
        Pop-Location

        # Sync assets to release folder explicitly (handles cases where MSBuild doesn't trigger POST_BUILD)
        $srcAssets = Join-Path $projDir "assets"
        $destReleaseAssets = Join-Path $projDir "build\Release\assets"
        if (Test-Path $srcAssets) {
            if (!(Test-Path $destReleaseAssets)) { New-Item -ItemType Directory -Path $destReleaseAssets -Force | Out-Null }
            Copy-Item -Path "$srcAssets\*" -Destination $destReleaseAssets -Force -Recurse -ErrorAction SilentlyContinue
            Write-Host "   Synced source assets to Release folder: $proj" -ForegroundColor DarkGray
        }
    }
}

Write-Host "`n>> All builds completed!" -ForegroundColor Cyan
Write-Host "   Engine: StarlightEngine" -ForegroundColor DarkGray
foreach ($proj in $Projects) {
    Write-Host "   Game:   $proj" -ForegroundColor DarkGray
}
