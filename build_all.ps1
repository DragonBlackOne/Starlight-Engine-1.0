# ============================================================================
# Build All Projects - Fusion ENGINE
# Syncs framework scripts and builds all projects
#
# Usage:
#   powershell -File build_all.ps1                  # Default: Release build
#   powershell -File build_all.ps1 -Clean           # Clean build (remove build dirs first)
#   powershell -File build_all.ps1 -Debug           # Debug configuration
#   powershell -File build_all.ps1 -Deps <path>     # Override dependency root
#   powershell -File build_all.ps1 -Help
# ============================================================================

param(
    [switch]$Clean,
    [switch]$Debug,
    [switch]$Release,
    [string]$Deps,
    [switch]$Help
)

if ($Help) {
    Write-Host "Usage: build_all.ps1 [-Clean] [-Debug|-Release] [-Deps <path>] [-Help]" -ForegroundColor Cyan
    Write-Host "  -Clean    Remove build directories before building"
    Write-Host "  -Debug    Build in Debug configuration (default: Release)"
    Write-Host "  -Release  Build in Release configuration (default)"
    Write-Host "  -Deps     Override the dependency root (default: d:/deps or env STARLIGHT_DEPS_DIR)"
    Write-Host "  -Help     Show this help message"
    exit 0
}

$ErrorActionPreference = "Continue"
$Root = $PSScriptRoot
$Engine = Join-Path $Root "StarlightEngine"
$Projects = @("CapitalOdyssey", "Pong_Project", "Tetris_Project", "Snake_Project", "FusionFight")
$Config = if ($Debug) { "Debug" } else { "Release" }

# Resolve dependency root with priority: -Deps > env STARLIGHT_DEPS_DIR > d:/deps
if ($Deps) {
    $DepsDir = $Deps
} elseif ($env:STARLIGHT_DEPS_DIR) {
    $DepsDir = $env:STARLIGHT_DEPS_DIR
} else {
    $DepsDir = "d:/deps"
}
$DepsDir = $DepsDir.TrimEnd('/', '\').Replace('\', '/')
$env:STARLIGHT_DEPS_DIR = $DepsDir

$buildStart = Get-Date

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host " Fusion ENGINE Build System v2.1" -ForegroundColor Cyan
Write-Host " Configuration: $Config" -ForegroundColor Cyan
Write-Host " Clean Build:   $($Clean.IsPresent)" -ForegroundColor Cyan
Write-Host " Deps root:     $DepsDir" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

# 0. Clean if requested
if ($Clean) {
    Write-Host ">> Cleaning build directories..." -ForegroundColor Yellow
    $cleanDirs = @(Join-Path $Engine "build")
    foreach ($proj in $Projects) {
        $cleanDirs += Join-Path $Root "$proj\build"
    }
    foreach ($dir in $cleanDirs) {
        if (Test-Path $dir) {
            Remove-Item -Recurse -Force $dir
            Write-Host "   Removed: $dir" -ForegroundColor DarkGray
        }
    }
}

# 1. Sync Framework Assets (scripts, shaders subset, fonts) to all projects
Write-Host ">> Syncing SBA Framework to all projects..." -ForegroundColor Yellow
foreach ($proj in $Projects) {
    $projAssets = Join-Path $Root "$proj\assets"
    if (!(Test-Path $projAssets)) { New-Item -ItemType Directory -Path $projAssets -Force | Out-Null }

    # Scripts: SBA framework (always full sync)
    $destScripts = Join-Path $projAssets "scripts"
    if (!(Test-Path $destScripts)) { New-Item -ItemType Directory -Path $destScripts -Force | Out-Null }
    foreach ($script in @("core.lua", "sba_bridge.lua", "hud.rml", "hud.rcss")) {
        $src = Join-Path $Engine "assets\scripts\$script"
        if (Test-Path $src) {
            Copy-Item $src -Destination $destScripts -Force
        }
    }
    Write-Host "   Synced Scripts: $proj" -ForegroundColor DarkGray

    # Shaders: copy only what 2D projects actually use. 2D projects do not
    # need PBR / IBL / Voxel / Terrain / Volumetric / SSR / SSGI / SSAO /
    # Atmosphere / BlackHole / Bloom pipelines. Keep the union that the
    # engine may ask for at runtime.
    $destShaders = Join-Path $projAssets "shaders"
    if (!(Test-Path $destShaders)) { New-Item -ItemType Directory -Path $destShaders -Force | Out-Null }

    $shaders2D = @(
        "basic.vert", "basic.frag",
        "batch.vert", "batch.frag",
        "default.vert", "default.frag",
        "mode7.vert", "mode7.frag",
        "pong.vert", "pong.frag",
        "postprocess.vert", "postprocess.frag",
        "screen.vert", "screen.frag",
        "ui.vert", "ui.frag",
        "ui_simple.vert", "ui_simple.frag",
        "quad.vert",
        "screen_quad.vert",
        "fullscreen_quad.vert",
        "selection.vert", "selection.frag",
        "bloom_downsample.frag", "bloom_upsample.frag", "composition.frag", "post_compose.frag",
        "particle_update.comp", "particle.vert", "particle.frag", "taa.frag"
    )
    foreach ($shader in $shaders2D) {
        $src = Join-Path $Engine "assets\shaders\$shader"
        if (Test-Path $src) {
            Copy-Item $src -Destination $destShaders -Force
        }
    }
    Write-Host "   Synced Shaders (2D subset): $proj" -ForegroundColor DarkGray

    # Fonts
    $destFonts = Join-Path $projAssets "fonts"
    if (!(Test-Path $destFonts)) { New-Item -ItemType Directory -Path $destFonts -Force | Out-Null }
    if (Test-Path (Join-Path $Engine "assets\fonts")) {
        Copy-Item (Join-Path $Engine "assets\fonts\*") -Destination $destFonts -Force -Recurse
        Write-Host "   Synced Fonts: $proj" -ForegroundColor DarkGray
    }

    # Audio (sounds, music) - copy only if engine ships with them
    $destAudio = Join-Path $projAssets "audio"
    if (!(Test-Path $destAudio)) { New-Item -ItemType Directory -Path $destAudio -Force | Out-Null }
    if (Test-Path (Join-Path $Engine "assets\audio")) {
        Copy-Item (Join-Path $Engine "assets\audio\*") -Destination $destAudio -Force -Recurse -ErrorAction SilentlyContinue
        Write-Host "   Synced Audio: $proj" -ForegroundColor DarkGray
    }
}

# 2. Resolve CMake Executable
$CMakeCmd = "cmake"
if (!(Get-Command cmake -ErrorAction SilentlyContinue)) {
    $VSPaths = @(
        "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
        "C:\Program Files\CMake\bin\cmake.exe"
    )
    foreach ($path in $VSPaths) {
        if (Test-Path $path) {
            $CMakeCmd = $path
            Write-Host ">> Found CMake at: $CMakeCmd" -ForegroundColor Gray
            break
        }
    }
}

# 3. Build Engine (static library)
$failedProjects = @()
Write-Host "`n>> Building StarlightEngine ($Config)..." -ForegroundColor Green
$engineStart = Get-Date
Push-Location $Engine
& $CMakeCmd -B build -S . -DSTARLIGHT_DEPS_DIR="$DepsDir"
if ($LASTEXITCODE -ne 0) {
    Write-Host "   FAILED: StarlightEngine CMake configure returned exit code $LASTEXITCODE" -ForegroundColor Red
    $failedProjects += "StarlightEngine"
    Pop-Location
} else {
    & $CMakeCmd --build build --config $Config --parallel
    if ($LASTEXITCODE -ne 0) {
        Write-Host "   FAILED: StarlightEngine build returned exit code $LASTEXITCODE" -ForegroundColor Red
        $failedProjects += "StarlightEngine"
    }
    Pop-Location
}
$engineElapsed = (Get-Date) - $engineStart
Write-Host "   Engine build time: $($engineElapsed.TotalSeconds.ToString('F1'))s" -ForegroundColor DarkGray

# Abort if engine failed
if ($failedProjects.Count -gt 0) {
    Write-Host "`n>> BUILD ABORTED: Engine build failed!" -ForegroundColor Red
    exit 1
}

# 4. Build all game projects
foreach ($proj in $Projects) {
    $projDir = Join-Path $Root $proj
    if (Test-Path $projDir) {
        Write-Host "`n>> Building $proj ($Config)..." -ForegroundColor Green
        $projStart = Get-Date
        Push-Location $projDir
        & $CMakeCmd -B build -S . -DSTARLIGHT_DEPS_DIR="$DepsDir"
        if ($LASTEXITCODE -ne 0) {
            Write-Host "   FAILED: $proj CMake configure returned exit code $LASTEXITCODE" -ForegroundColor Red
            $failedProjects += $proj
            Pop-Location
            continue
        }
        & $CMakeCmd --build build --config $Config --parallel
        if ($LASTEXITCODE -ne 0) {
            Write-Host "   FAILED: $proj build returned exit code $LASTEXITCODE" -ForegroundColor Red
            $failedProjects += $proj
        }
        Pop-Location

        # Sync assets to release folder explicitly (belt-and-braces; POST_BUILD
        # in the helper does this too but copying from source keeps the build
        # output in sync even after editing assets in dev)
        $srcAssets = Join-Path $projDir "assets"
        $destReleaseAssets = Join-Path $projDir "build\$Config\assets"
        if (Test-Path $srcAssets) {
            if (!(Test-Path $destReleaseAssets)) { New-Item -ItemType Directory -Path $destReleaseAssets -Force | Out-Null }
            Copy-Item -Path "$srcAssets\*" -Destination $destReleaseAssets -Force -Recurse -ErrorAction SilentlyContinue
            Write-Host "   Synced source assets to $Config folder: $proj" -ForegroundColor DarkGray
        }

        $projElapsed = (Get-Date) - $projStart
        Write-Host "   $proj build time: $($projElapsed.TotalSeconds.ToString('F1'))s" -ForegroundColor DarkGray
    }
}

# 5. Summary
$totalElapsed = (Get-Date) - $buildStart
Write-Host "`n========================================" -ForegroundColor Cyan
if ($failedProjects.Count -eq 0) {
    Write-Host " BUILD SUCCESS" -ForegroundColor Green
} else {
    Write-Host " BUILD COMPLETED WITH ERRORS" -ForegroundColor Red
    foreach ($fp in $failedProjects) {
        Write-Host "   FAILED: $fp" -ForegroundColor Red
    }
}
Write-Host " Total build time: $($totalElapsed.TotalSeconds.ToString('F1'))s" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   Engine: StarlightEngine" -ForegroundColor DarkGray
foreach ($proj in $Projects) {
    $status = if ($failedProjects -contains $proj) { "[FAILED]" } else { "[OK]" }
    Write-Host "   Game:   $proj $status" -ForegroundColor DarkGray
}

if ($failedProjects.Count -gt 0) { exit 1 }
