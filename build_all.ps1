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
$Projects = @("CapitalOdyssey", "Pong_Project", "Tetris_Project", "Snake_Project", "FusionFight", "GodHand_Project", "TechDemo_Project")
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
Write-Host " Starlight Engine Build System v11.0" -ForegroundColor Cyan
Write-Host " Configuration: $Config" -ForegroundColor Cyan
Write-Host " Clean Build:   $($Clean.IsPresent)" -ForegroundColor Cyan
Write-Host " Deps root:     $DepsDir" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

# 0. Clean if requested
if ($Clean) {
    Write-Host ">> Cleaning build directories..." -ForegroundColor Yellow
    $buildDir = Join-Path $Root "build"
    if (Test-Path $buildDir) {
        Remove-Item -Recurse -Force $buildDir
        Write-Host "   Removed: $buildDir" -ForegroundColor DarkGray
    }
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

# 1. Assets framework são copiados no pós-build de cada projeto (add_fusion_game),
# para o diretório de build. Não robocopy mais assets da engine para dentro dos
# source trees dos jogos (evita poluição e duplicatas aninhadas).

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

# 3. Configure Root Workspace
$failedProjects = @()
Write-Host "`n>> Configuring FusionEngine root workspace ($Config)..." -ForegroundColor Green
$configStart = Get-Date
& $CMakeCmd -B build -S . -DSTARLIGHT_DEPS_DIR="$DepsDir" -DSTARLIGHT_BUILD_TESTS=ON
if ($LASTEXITCODE -ne 0) {
    Write-Host "   FAILED: CMake configure returned exit code $LASTEXITCODE" -ForegroundColor Red
    exit 1
}
$configElapsed = (Get-Date) - $configStart
Write-Host "   Configure time: $($configElapsed.TotalSeconds.ToString('F1'))s" -ForegroundColor DarkGray

# 4. Build Targets
Write-Host "`n>> Building StarlightEngine targets ($Config)..." -ForegroundColor Green
$engineStart = Get-Date
& $CMakeCmd --build build --config $Config --target StarlightShowcase --parallel
if ($LASTEXITCODE -ne 0) {
    Write-Host "   FAILED: StarlightShowcase build returned exit code $LASTEXITCODE" -ForegroundColor Red
    $failedProjects += "StarlightEngine"
}
& $CMakeCmd --build build --config $Config --target StarlightTests --parallel
if ($LASTEXITCODE -ne 0) {
    Write-Host "   FAILED: StarlightTests build returned exit code $LASTEXITCODE" -ForegroundColor Red
    if (!($failedProjects -contains "StarlightEngine")) {
        $failedProjects += "StarlightEngine"
    }
}
$engineElapsed = (Get-Date) - $engineStart
Write-Host "   Engine build time: $($engineElapsed.TotalSeconds.ToString('F1'))s" -ForegroundColor DarkGray

# Build all game projects
foreach ($proj in $Projects) {
    Write-Host "`n>> Building $proj ($Config)..." -ForegroundColor Green
    $projStart = Get-Date
    & $CMakeCmd --build build --config $Config --target $proj --parallel
    if ($LASTEXITCODE -ne 0) {
        Write-Host "   FAILED: $proj build returned exit code $LASTEXITCODE" -ForegroundColor Red
        $failedProjects += $proj
    }
    $projElapsed = (Get-Date) - $projStart
    Write-Host "   $proj build time: $($projElapsed.TotalSeconds.ToString('F1'))s" -ForegroundColor DarkGray
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

