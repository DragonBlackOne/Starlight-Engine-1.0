# ============================================================================
# Build All Projects - Fusion ENGINE
# Syncs framework scripts and builds all projects
# ============================================================================

$ErrorActionPreference = "Continue"
$Root = $PSScriptRoot
$Engine = Join-Path $Root "StarlightEngine"
$Projects = @("CapitalOdyssey", "Pong_Project", "Tetris_Project", "Snake_Project")

# 1. Sync Framework Scripts (core.lua, sba_bridge.lua) to all projects
Write-Host ">> Syncing SBA Framework to all projects..." -ForegroundColor Yellow
foreach ($proj in $Projects) {
    $dest = Join-Path $Root "$proj\assets\scripts"
    if (Test-Path $dest) {
        Copy-Item (Join-Path $Engine "assets\scripts\core.lua") -Destination $dest -Force
        Copy-Item (Join-Path $Engine "assets\scripts\sba_bridge.lua") -Destination $dest -Force -ErrorAction SilentlyContinue
        Write-Host "   Synced: $proj" -ForegroundColor DarkGray
    }
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
    }
}

Write-Host "`n>> All builds completed!" -ForegroundColor Cyan
Write-Host "   Engine: StarlightEngine" -ForegroundColor DarkGray
foreach ($proj in $Projects) {
    Write-Host "   Game:   $proj" -ForegroundColor DarkGray
}
