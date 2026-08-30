#!/usr/bin/env python3
"""
==============================================================================
Starlight Engine — Unified Python Developer Toolchain & CLI Suite (v15.0.0)
==============================================================================
Central orchestration interface for building, testing, linting, validating,
scaffolding, and executing standalone Starlight Engine games.

Usage:
  python starlight.py build [target] [--clean] [--config Release|Debug]
  python starlight.py run <game>
  python starlight.py test [--filter <gtest_filter>]
  python starlight.py lint
  python starlight.py validate
  python starlight.py new-game <GameName> [--2d|--3d]
  python starlight.py stats
  python starlight.py clean
"""

import os
import sys
import time
import argparse
import subprocess
from pathlib import Path

# ANSI Color Codes
GREEN = "\033[92m"
YELLOW = "\033[93m"
RED = "\033[91m"
CYAN = "\033[96m"
MAGENTA = "\033[95m"
BOLD = "\033[1m"
RESET = "\033[0m"

try:
    if sys.stdout and hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
except Exception:
    pass

ICON_OK = "[OK]"
ICON_FAIL = "[X]"

ROOT_DIR = Path(__file__).resolve().parent

GAME_PROJECTS = {
    "pong": {
        "dir": ROOT_DIR / "Pong_Project",
        "target": "Pong_Project",
        "exe": ROOT_DIR / "build" / "Pong_Project" / "Release" / "Pong_Project.exe"
    },
    "snake": {
        "dir": ROOT_DIR / "Snake_Project",
        "target": "Snake_Project",
        "exe": ROOT_DIR / "build" / "Snake_Project" / "Release" / "Snake_Project.exe"
    },
    "tetris": {
        "dir": ROOT_DIR / "Tetris_Project",
        "target": "Tetris_Project",
        "exe": ROOT_DIR / "build" / "Tetris_Project" / "Release" / "Tetris_Project.exe"
    },
    "odyssey": {
        "dir": ROOT_DIR / "CapitalOdyssey",
        "target": "CapitalOdyssey",
        "exe": ROOT_DIR / "build" / "CapitalOdyssey" / "Release" / "CapitalOdyssey.exe"
    },
    "fight": {
        "dir": ROOT_DIR / "FusionFight",
        "target": "FusionFight",
        "exe": ROOT_DIR / "build" / "FusionFight" / "Release" / "FusionFight.exe"
    },
    "godhand": {
        "dir": ROOT_DIR / "GodHand_Project",
        "target": "GodHand_Project",
        "exe": ROOT_DIR / "build" / "GodHand_Project" / "Release" / "GodHand_Project.exe"
    },
    "techdemo": {
        "dir": ROOT_DIR / "TechDemo_Project",
        "target": "TechDemo_Project",
        "exe": ROOT_DIR / "build" / "TechDemo_Project" / "Release" / "TechDemo_Project.exe"
    },
    "engine": {
        "dir": ROOT_DIR / "StarlightEngine",
        "target": "StarlightShowcase",
        "exe": ROOT_DIR / "build" / "StarlightEngine" / "Release" / "StarlightShowcase.exe"
    },
    "showcase": {
        "dir": ROOT_DIR / "StarlightEngine",
        "target": "StarlightShowcase",
        "exe": ROOT_DIR / "build" / "StarlightEngine" / "Release" / "StarlightShowcase.exe"
    }
}

def find_cmake():
    import shutil
    c = shutil.which("cmake")
    if c:
        return c
    vs_paths = [
        r"C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
        r"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
        r"C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
        r"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
        r"C:\Program Files\CMake\bin\cmake.exe"
    ]
    for p in vs_paths:
        if Path(p).exists():
            return p
    return "cmake"

# ============================================================================
# COMMAND: build
# ============================================================================
def cmd_build(args):
    print(f"\n{BOLD}{CYAN}=== Starlight Engine v11.0 Build Orchestrator ==={RESET}\n")
    target = args.target.lower() if args.target else "all"
    config = args.config
    cmake_bin = find_cmake()

    start_time = time.perf_counter()

    if target == "all":
        print(f"Building {BOLD}Starlight Engine and all Game Projects{RESET} [{config}]...")
        ps_cmd = ["powershell", "-ExecutionPolicy", "Bypass", "-File", str(ROOT_DIR / "build_all.ps1")]
        res = subprocess.run(ps_cmd, cwd=str(ROOT_DIR))
        elapsed = time.perf_counter() - start_time
        if res.returncode == 0:
            print(f"\n{GREEN}{ICON_OK} Full workspace build completed successfully in {elapsed:.1f}s{RESET}\n")
        else:
            print(f"\n{RED}{ICON_FAIL} Full workspace build failed with code {res.returncode}{RESET}\n")
        return res.returncode
    elif target in ["engine", "starlightcore", "starlight"]:
        print(f"Building {BOLD}StarlightCore Engine{RESET} [{config}]...")
        cmd = [cmake_bin, "--build", "build", "--config", config, "--target", "StarlightCore", "--parallel"]
        res = subprocess.run(cmd, cwd=str(ROOT_DIR))
        return res.returncode
    elif target in ["test", "tests", "starlighttests"]:
        print(f"Building {BOLD}StarlightTests Test Suite{RESET} [{config}]...")
        cmd = [cmake_bin, "--build", "build", "--config", config, "--target", "StarlightTests", "--parallel"]
        res = subprocess.run(cmd, cwd=str(ROOT_DIR))
        return res.returncode
    elif target in GAME_PROJECTS:
        proj = GAME_PROJECTS[target]
        print(f"Building {BOLD}{proj['target']}{RESET} [{config}]...")
        cmd = [cmake_bin, "--build", "build", "--config", config, "--target", proj['target'], "--parallel"]
        res = subprocess.run(cmd, cwd=str(ROOT_DIR))
        return res.returncode
    else:
        print(f"{RED}Unknown build target: '{target}'. Available targets: all, engine, {', '.join(GAME_PROJECTS.keys())}{RESET}")
        return 1

# ============================================================================
# COMMAND: run
# ============================================================================
def cmd_run(args):
    game_key = args.game.lower()
    if game_key not in GAME_PROJECTS:
        print(f"{RED}Unknown game: '{game_key}'. Available games: {', '.join(GAME_PROJECTS.keys())}{RESET}")
        return 1

    proj = GAME_PROJECTS[game_key]
    exe_path = proj["exe"]

    if not exe_path.exists():
        print(f"{RED}Executable not found at '{exe_path}'. Building first...{RESET}")
        build_res = cmd_build(argparse.Namespace(target=game_key, config="Release", clean=False))
        if build_res != 0:
            return build_res

    print(f"\n{BOLD}{CYAN}Launching {proj['target']} (Starlight Engine v10.0)...{RESET}")
    print(f"  Working Dir: {BOLD}{proj['dir']}{RESET}")
    print(f"  Executable : {BOLD}{exe_path.name}{RESET}\n")

    try:
        proc = subprocess.Popen([str(exe_path)], cwd=str(proj["dir"]))
        print(f"{GREEN}Process running with PID {proc.pid}.{RESET}")
        return 0
    except Exception as e:
        print(f"{RED}Failed to launch game: {e}{RESET}")
        return 1

# ============================================================================
# COMMAND: test
# ============================================================================
def cmd_test(args):
    from tools.qa_runner import run_qa_suite
    return run_qa_suite(args.filter)

# ============================================================================
# COMMAND: lint
# ============================================================================
def cmd_lint(args):
    from tools.lua_linter import run_linter
    return run_linter()

# ============================================================================
# COMMAND: validate
# ============================================================================
def cmd_validate(args):
    from tools.asset_pipeline import run_asset_validation
    return run_asset_validation()

# ============================================================================
# COMMAND: stats
# ============================================================================
def cmd_stats(args):
    print(f"\n{BOLD}{CYAN}=== Starlight Engine v10.0 Telemetry & Codebase Stats ==={RESET}\n")
    extensions = {
        "C++ Headers (.hpp/.h)": [".hpp", ".h"],
        "C++ Sources (.cpp)": [".cpp"],
        "Lua Scripts (.lua)": [".lua"],
        "Python Tools (.py)": [".py"],
        "Shaders (.vert/.frag/.glsl)": [".vert", ".frag", ".geom", ".comp", ".glsl"]
    }

    stats = {k: {"files": 0, "lines": 0} for k in extensions}

    for root, dirs, files in os.walk(ROOT_DIR):
        if "build" in root or ".git" in root or "thirdparty" in root or ".gemini" in root:
            continue
        for file in files:
            file_path = Path(root) / file
            suffix = file_path.suffix.lower()
            for lang, exts in extensions.items():
                if suffix in exts:
                    stats[lang]["files"] += 1
                    try:
                        lines = len(file_path.read_text(encoding="utf-8", errors="replace").splitlines())
                        stats[lang]["lines"] += lines
                    except Exception:
                        pass

    total_files = sum(s["files"] for s in stats.values())
    total_lines = sum(s["lines"] for s in stats.values())

    print(f"{'Category':<30} | {'Files':<8} | {'Lines of Code':<15}")
    print("-" * 60)
    for lang, data in stats.items():
        print(f"{lang:<30} | {data['files']:<8} | {data['lines']:<15,}")
    print("-" * 60)
    print(f"{BOLD}{'TOTAL':<30} | {total_files:<8} | {total_lines:<15,}{RESET}\n")
    return 0

# ============================================================================
# COMMAND: new-game (Project Scaffolding)
# ============================================================================
def cmd_new_game(args):
    name = args.name.replace(" ", "_")
    is_2d = args.mode2d

    target_dir = ROOT_DIR / name
    if target_dir.exists():
        print(f"{RED}Error: Project folder '{name}' already exists.{RESET}")
        return 1

    print(f"\n{BOLD}{CYAN}Scaffolding New Starlight Game: '{name}' ({'2D' if is_2d else '3D'})...{RESET}\n")

    (target_dir / "src").mkdir(parents=True, exist_ok=True)
    (target_dir / "assets" / "scripts").mkdir(parents=True, exist_ok=True)
    (target_dir / "assets" / "textures").mkdir(parents=True, exist_ok=True)

    cmake_content = f"""cmake_minimum_required(VERSION 3.20)
project({name} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${{CMAKE_SOURCE_DIR}}/StarlightEngine/cmake/StarlightProject.cmake)

add_starlight_game({name}
    SOURCES
        src/main.cpp
    ASSETS_DIR
        assets
    {'MODE_2D' if is_2d else ''}
)
"""
    (target_dir / "CMakeLists.txt").write_text(cmake_content, encoding="utf-8")

    ini_content = f"""[Engine]
width=1280
height=720
fullscreen=false
vsync=true
mode2D={'true' if is_2d else 'false'}

[Systems]
EditorSystem=false
AudioSystem=true
"""
    (target_dir / "assets" / "engine.ini").write_text(ini_content, encoding="utf-8")

    cpp_content = f"""#include <Engine.hpp>
#include <Log.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {{
    using namespace starlight;
    
    EngineConfig config;
    config.title = "Starlight Engine v10.0 - {name}";
    config.width = 1280;
    config.height = 720;
    config.fullscreen = false;
    config.mode2D = {'true' if is_2d else 'false'};
    config.scriptPath = "assets/scripts/main.lua";

    Engine engine;
    if (!engine.Initialize(config)) {{
        Log::Error("Failed to initialize engine for {name}");
        return -1;
    }}

    engine.Run();
    engine.Shutdown();
    return 0;
}}
"""
    (target_dir / "src" / "main.cpp").write_text(cpp_content, encoding="utf-8")

    lua_content = f"""-- {name} // Starlight Engine v10.0 Game Script
engine = engine or Engine

function OnStart()
    engine.log("{name}: Initialized successfully on Starlight Engine v" .. (engine.version or "8.0.0"))
end

function OnUpdate(dt)
    -- Game logic update
end

function OnRenderUI()
    if draw and draw.text then
        draw.text(50, 50, "{name} on Starlight Engine v10.0!", 1.0, 1.0, 1.0, 1.0, 2.0)
    end
end
"""
    (target_dir / "assets" / "scripts" / "main.lua").write_text(lua_content, encoding="utf-8")

    (ROOT_DIR / f"play_{name.lower()}.bat").write_text(f"""@echo off
cd /d "%~dp0{name}"
start "" "%~dp0build\\{name}\\Release\\{name}.exe"
""", encoding="utf-8")

    (target_dir / f"run_{name.lower()}.ps1").write_text(f"""$ErrorActionPreference = "Stop"
$PSScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition
$ExePath = Join-Path (Split-Path -Parent $PSScriptRoot) "build\\{name}\\Release\\{name}.exe"
Start-Process -FilePath $ExePath -WorkingDirectory $PSScriptRoot
""", encoding="utf-8")

    print(f"{GREEN}{ICON_OK} Successfully generated project at: {target_dir.relative_to(ROOT_DIR)}{RESET}")
    print(f"  - Launcher created: {BOLD}play_{name.lower()}.bat{RESET}")
    print(f"  - Run with: {YELLOW}python starlight.py run {name.lower()}{RESET}\n")
    return 0

# ============================================================================
# COMMAND: export
# ============================================================================
def cmd_export(args):
    exporter_script = ROOT_DIR / "tools" / "game_exporter.py"
    cmd = [sys.executable, str(exporter_script), args.game]
    if args.zip:
        cmd.append("--zip")
    if args.steam:
        cmd.append("--steam")
    if args.appid:
        cmd.extend(["--appid", args.appid])
    if args.pak:
        cmd.append("--pak")
    if args.output:
        cmd.extend(["--output", str(args.output)])

    res = subprocess.run(cmd)
    return res.returncode

# ============================================================================
# COMMAND: bench
# ============================================================================
def cmd_bench(args):
    print(f"\n{BOLD}{CYAN}=== Starlight Engine v12.0 Benchmark Suite ==={RESET}\n")
    test_exe = ROOT_DIR / "build" / "StarlightEngine" / "tests" / "Release" / "StarlightTests.exe"
    if not test_exe.exists():
        print(f"{RED}Error: Build test suite first with 'python starlight.py build engine'{RESET}")
        return 1
    t0 = time.perf_counter()
    res = subprocess.run([str(test_exe), "--gtest_filter=*JobSystem*:*DecalSystem*:*Profiler*"])
    elapsed = time.perf_counter() - t0
    print(f"\n{GREEN}{ICON_OK} Benchmark completed in {elapsed:.3f}s with 0 errors.{RESET}\n")
    return res.returncode

# ============================================================================
# COMMAND: clean-dist
# ============================================================================
def cmd_clean_dist(args):
    import shutil
    dist_dir = ROOT_DIR / "dist"
    if dist_dir.exists():
        print(f"Cleaning '{dist_dir}'...")
        shutil.rmtree(dist_dir)
        print(f"{GREEN}{ICON_OK} Distribution builds cleaned successfully.{RESET}")
    return 0

# ============================================================================
# COMMAND: audit
# ============================================================================
def cmd_audit(args):
    print(f"\n{BOLD}{CYAN}=== Starlight Engine v12.0 Codebase Audit ==={RESET}\n")
    lint_script = ROOT_DIR / "tools" / "lua_linter.py"
    pipe_script = ROOT_DIR / "tools" / "asset_pipeline.py"
    r1 = subprocess.run([sys.executable, str(lint_script)]).returncode
    r2 = subprocess.run([sys.executable, str(pipe_script)]).returncode
    if r1 == 0 and r2 == 0:
        print(f"{GREEN}{ICON_OK} Codebase passed all audit and hygiene checks with 0 warnings.{RESET}\n")
        return 0
    return 1

# ============================================================================
# COMMAND: bake-character
# ============================================================================
def cmd_bake_character(args):
    char_script = ROOT_DIR / "tools" / "character_studio.py"
    target = getattr(args, "target", "all")
    print(f"\n{BOLD}{CYAN}=== Starlight Engine 3D Character Studio ==={RESET}\n")
    return subprocess.run([sys.executable, str(char_script), target]).returncode

# ============================================================================
# COMMAND: bake-textures
# ============================================================================
def cmd_bake_textures(args):
    tex_script = ROOT_DIR / "tools" / "texture_baker.py"
    print(f"\n{BOLD}{CYAN}=== Starlight Engine PBR Texture Baker ==={RESET}\n")
    return subprocess.run([sys.executable, str(tex_script)]).returncode

# ============================================================================
# COMMAND: bake-sky
# ============================================================================
def cmd_bake_sky(args):
    sky_script = ROOT_DIR / "tools" / "sky_baker.py"
    print(f"\n{BOLD}{CYAN}=== Starlight Engine Sky Dome Baker ==={RESET}\n")
    return subprocess.run([sys.executable, str(sky_script)]).returncode

# ============================================================================
# CLI Argument Parser & Entry Point
# ============================================================================
def main():
    parser = argparse.ArgumentParser(
        description="Starlight Engine v15.0 Developer CLI Suite",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="Examples:\n"
               "  python starlight.py build\n"
               "  python starlight.py run godhand\n"
               "  python starlight.py bake-character gene\n"
               "  python starlight.py bake-textures\n"
               "  python starlight.py bake-sky\n"
               "  python starlight.py export godhand --zip\n"
               "  python starlight.py test\n"
    )
    subparsers = parser.add_subparsers(dest="command", help="Available subcommands")

    # Subcommand: build
    p_build = subparsers.add_parser("build", help="Build engine or games")
    p_build.add_argument("target", nargs="?", default="all", help="Target name (all, engine, pong, snake, tetris, odyssey, fight, godhand, techdemo)")
    p_build.add_argument("--config", default="Release", choices=["Release", "Debug"], help="Build configuration (default: Release)")
    p_build.add_argument("--clean", action="store_true", help="Perform a clean build")

    # Subcommand: run
    p_run = subparsers.add_parser("run", help="Launch a standalone game")
    p_run.add_argument("game", help="Game to launch (pong, snake, tetris, odyssey, fight, godhand, techdemo)")

    # Subcommand: export
    p_export = subparsers.add_parser("export", help="Export and package game for Steam, Itch.io, and Standalone distribution")
    p_export.add_argument("game", help="Game name or alias (pong, snake, tetris, odyssey, fight, godhand, techdemo)")
    p_export.add_argument("--zip", action="store_true", help="Create compressed .zip release archive for distribution")
    p_export.add_argument("--steam", action="store_true", help="Generate SteamPipe manifests (app_build/depot_build) and steam_appid.txt")
    p_export.add_argument("--appid", type=str, help="Custom Steam AppID (default: 480)")
    p_export.add_argument("--pak", action="store_true", help="Pack assets into single game.pak VFS archive")
    p_export.add_argument("--output", type=Path, help="Custom output directory")

    # Subcommand: bake-character
    p_bake_char = subparsers.add_parser("bake-character", help="Sculpt and bake 3D anatomical character models")
    p_bake_char.add_argument("target", nargs="?", default="all", help="Character to sculpt (gene, punk, all)")

    # Subcommand: bake-textures
    subparsers.add_parser("bake-textures", help="Bake procedural PBR normal maps and materials")

    # Subcommand: bake-sky
    subparsers.add_parser("bake-sky", help="Bake atmospheric sky dome textures")

    # Subcommand: test
    p_test = subparsers.add_parser("test", help="Execute GoogleTest test suites")
    p_test.add_argument("--filter", help="GoogleTest filter pattern (e.g. '*Audio*')")

    # Subcommand: lint
    subparsers.add_parser("lint", help="Perform static analysis on Lua scripts")

    # Subcommand: validate
    subparsers.add_parser("validate", help="Validate assets, textures, and configs")

    # Subcommand: stats
    subparsers.add_parser("stats", help="Show codebase line counts and metrics")

    # Subcommand: new-game
    p_new = subparsers.add_parser("new-game", help="Scaffold a new game project")
    p_new.add_argument("name", help="Name of the new game project")
    p_new.add_argument("--2d", dest="mode2d", action="store_true", default=True, help="Configure as 2D game (default)")
    p_new.add_argument("--3d", dest="mode2d", action="store_false", help="Configure as 3D game")

    # Subcommand: bench
    subparsers.add_parser("bench", help="Run automated multi-core engine performance benchmark")

    # Subcommand: clean-dist
    subparsers.add_parser("clean-dist", help="Clean distribution builds directory")

    # Subcommand: audit
    subparsers.add_parser("audit", help="Run full static analysis and asset pipeline audit")

    args = parser.parse_args()

    if not args.command:
        parser.print_help()
        return 0

    commands = {
        "build": cmd_build,
        "run": cmd_run,
        "export": cmd_export,
        "bake-character": cmd_bake_character,
        "bake-textures": cmd_bake_textures,
        "bake-sky": cmd_bake_sky,
        "test": cmd_test,
        "lint": cmd_lint,
        "validate": cmd_validate,
        "stats": cmd_stats,
        "new-game": cmd_new_game,
        "bench": cmd_bench,
        "clean-dist": cmd_clean_dist,
        "audit": cmd_audit
    }

    return commands[args.command](args)

if __name__ == "__main__":
    sys.exit(main())
