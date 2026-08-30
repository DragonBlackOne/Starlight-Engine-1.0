#!/usr/bin/env python3
"""
==============================================================================
Starlight Engine — Automated Game Exporter & Release Packaging Subsystem
==============================================================================
Produces 100% self-contained, distribution-ready packages for Steam (SteamPipe),
Itch.io, GOG, and Windows Standalone x64.

Usage:
  python tools/game_exporter.py <game_name> [--zip] [--steam] [--appid <ID>] [--pak] [--output <dir>]
"""

import os
import sys
import shutil
import zipfile
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

ROOT_DIR = Path(__file__).resolve().parent.parent

GAME_REGISTRY = {
    "pong": {
        "title": "Pong: Cyberpunk Neon Arcade",
        "target": "Pong_Project",
        "dir": ROOT_DIR / "Pong_Project",
        "exe": ROOT_DIR / "build" / "Pong_Project" / "Release" / "Pong_Project.exe",
        "default_appid": "480"
    },
    "snake": {
        "title": "Snake: Synthwave 2D Edition",
        "target": "Snake_Project",
        "dir": ROOT_DIR / "Snake_Project",
        "exe": ROOT_DIR / "build" / "Snake_Project" / "Release" / "Snake_Project.exe",
        "default_appid": "480"
    },
    "tetris": {
        "title": "Tetris: Cyber Matrix Edition",
        "target": "Tetris_Project",
        "dir": ROOT_DIR / "Tetris_Project",
        "exe": ROOT_DIR / "build" / "Tetris_Project" / "Release" / "Tetris_Project.exe",
        "default_appid": "480"
    },
    "odyssey": {
        "title": "Capital Odyssey: Market Simulation",
        "target": "CapitalOdyssey",
        "dir": ROOT_DIR / "CapitalOdyssey",
        "exe": ROOT_DIR / "build" / "CapitalOdyssey" / "Release" / "CapitalOdyssey.exe",
        "default_appid": "480"
    },
    "fight": {
        "title": "Fusion Fight: Cyber Clash 2.5D",
        "target": "FusionFight",
        "dir": ROOT_DIR / "FusionFight",
        "exe": ROOT_DIR / "build" / "FusionFight" / "Release" / "FusionFight.exe",
        "default_appid": "480"
    },
    "godhand": {
        "title": "God Hand: Cyber Brawler 3D",
        "target": "GodHand_Project",
        "dir": ROOT_DIR / "GodHand_Project",
        "exe": ROOT_DIR / "build" / "GodHand_Project" / "Release" / "GodHand_Project.exe",
        "default_appid": "480"
    },
    "techdemo": {
        "title": "Starlight Engine 3D Tech Demo",
        "target": "TechDemo_Project",
        "dir": ROOT_DIR / "TechDemo_Project",
        "exe": ROOT_DIR / "build" / "TechDemo_Project" / "Release" / "TechDemo_Project.exe",
        "default_appid": "480"
    }
}

def resolve_game(query: str):
    q = query.strip().lower()
    if q in GAME_REGISTRY:
        return GAME_REGISTRY[q]
    for key, info in GAME_REGISTRY.items():
        if info["target"].lower() == q or info["dir"].name.lower() == q:
            return info
    custom_dir = ROOT_DIR / query
    if custom_dir.exists() and (custom_dir / "assets").exists():
        target_name = custom_dir.name
        return {
            "title": target_name,
            "target": target_name,
            "dir": custom_dir,
            "exe": ROOT_DIR / "build" / target_name / "Release" / f"{target_name}.exe",
            "default_appid": "480"
        }
    return None

def build_game_if_needed(game_info: dict) -> bool:
    exe_path = game_info["exe"]
    if exe_path.exists():
        return True
    
    print(f"{YELLOW}Target executable '{exe_path.name}' not found. Initiating Release build...{RESET}")
    cmd = [sys.executable, str(ROOT_DIR / "starlight.py"), "build", game_info["target"], "--config", "Release"]
    res = subprocess.run(cmd)
    return res.returncode == 0 and exe_path.exists()

def create_steampipe_manifests(steampipe_dir: Path, game_info: dict, app_id: str, content_root: Path):
    steampipe_dir.mkdir(parents=True, exist_ok=True)
    depot_id = f"{app_id}1"
    
    app_build_content = f""""appbuild"
{{
    "appid" "{app_id}"
    "desc" "Starlight Engine v12.0 Release Build - {game_info['title']}"
    "buildoutput" "{steampipe_dir.as_posix()}/output"
    "contentroot" "{content_root.as_posix()}"
    "setlive" ""
    "preview" "0"
    "local" ""
    "depots"
    {{
        "{depot_id}" "depot_build_{depot_id}.vdf"
    }}
}}
"""
    depot_build_content = f""""DepotBuildConfig"
{{
    "DepotID" "{depot_id}"
    "ContentRoot" "{content_root.as_posix()}"
    "FileMapping"
    {{
        "LocalPath" "*"
        "DepotPath" "."
        "recursive" "1"
    }}
    "FileExclusion" "*.pdb"
    "FileExclusion" "*.log"
    "FileExclusion" "steampipe/*"
}}
"""
    upload_bat_content = f"""@echo off
echo ==============================================================================
echo Uploading {game_info['title']} to Steam via SteamPipe (AppID: {app_id})
echo ==============================================================================
echo Make sure steamcmd is installed and in your PATH.
echo.
steamcmd.exe +login <username> +run_app_build "%~dp0app_build_{app_id}.vdf" +quit
pause
"""

    (steampipe_dir / f"app_build_{app_id}.vdf").write_text(app_build_content, encoding="utf-8")
    (steampipe_dir / f"depot_build_{depot_id}.vdf").write_text(depot_build_content, encoding="utf-8")
    (steampipe_dir / "upload_to_steam.bat").write_text(upload_bat_content, encoding="utf-8")

def generate_readme(dist_dir: Path, game_info: dict, version_str: str = "12.0.0"):
    content = f"""==============================================================================
{game_info['title']} (v{version_str})
Built with Starlight Engine v{version_str} ("Apex")
==============================================================================

SYSTEM REQUIREMENTS:
  - OS: Windows 10/11 64-bit
  - Graphics: OpenGL 4.3+ capable GPU (NVIDIA GTX 960+ / AMD RX 460+ / Intel Iris Xe+)
  - Audio: DirectSound / WASAPI compatible audio device
  - Input: Keyboard & Mouse, Xbox/PlayStation Controller compatible

CONTROLS:
  - F11: Toggle Fullscreen / Windowed
  - F3:  Toggle Live Telemetry & Frame Profiler HUD
  - F12: Instant High-Resolution Framebuffer Screenshot
  - F5:  Hot-Reload Scripts & Assets (Development Mode)
  - ESC: Pause / Menu / Back

DISTRIBUTION NOTICE:
  This standalone release package is self-contained. You may deploy this
  directory directly to Steam, Itch.io, GOG Galaxy, or distribute as a zip.

Engine Architecture & Copyright:
  (c) 2026 Starlight Engine Development Team. High-Performance C++20 Core.
"""
    (dist_dir / "README.txt").write_text(content, encoding="utf-8")

def export_game(game_query: str, zip_package: bool = False, steam_mode: bool = False,
                app_id: str = None, pak_mode: bool = False, output_dir: Path = None) -> int:
    print(f"\n{BOLD}{CYAN}=== Starlight Engine v12.0 Commercial Game Exporter ==={RESET}\n")

    game_info = resolve_game(game_query)
    if not game_info:
        print(f"{RED}Error: Game '{game_query}' not recognized.{RESET}")
        print(f"Available registered games: {YELLOW}{', '.join(GAME_REGISTRY.keys())}{RESET}")
        return 1

    print(f"Target Title:   {BOLD}{game_info['title']}{RESET}")
    print(f"Project Dir:    {game_info['dir']}")
    print(f"Steam Mode:     {steam_mode} (AppID: {app_id or game_info['default_appid']})")
    print(f"ZIP Archive:    {zip_package}")
    print(f"VFS PAK Mode:   {pak_mode}\n")

    # 1. Verify / Build Release Executable
    if not build_game_if_needed(game_info):
        print(f"{RED}Failed to build Release executable for '{game_info['target']}'.{RESET}")
        return 1

    # 2. Setup Output Directory
    dist_root = output_dir or (ROOT_DIR / "dist")
    target_dist = dist_root / game_info["target"]

    if target_dist.exists():
        print(f">> Cleaning existing distribution directory '{target_dist}'...")
        shutil.rmtree(target_dist)
    target_dist.mkdir(parents=True, exist_ok=True)

    # 3. Copy Executable
    exe_src = game_info["exe"]
    exe_dst = target_dist / f"{game_info['target']}.exe"
    print(f">> Copying executable: {exe_src.name} -> {exe_dst.relative_to(ROOT_DIR)}")
    shutil.copy2(exe_src, exe_dst)

    # 4. Copy Assets Tree
    assets_src = game_info["dir"] / "assets"
    if not assets_src.exists():
        assets_src = ROOT_DIR / "StarlightEngine" / "assets"

    assets_dst = target_dist / "assets"

    if pak_mode:
        print(f">> Packing assets into standalone archive 'game.pak'...")
        assets_dst.mkdir(parents=True, exist_ok=True)
        pak_file = assets_dst / "game.pak"
        with zipfile.ZipFile(pak_file, "w", zipfile.ZIP_DEFLATED) as zf:
            for root, _, files in os.walk(assets_src):
                for f in files:
                    if f.endswith(".tmp") or f == ".gitkeep":
                        continue
                    full_p = Path(root) / f
                    rel_p = full_p.relative_to(assets_src)
                    zf.write(full_p, rel_p.as_posix())
        print(f"  {GREEN}{ICON_OK}{RESET} Created game.pak ({pak_file.stat().st_size / 1024:.1f} KB)")
    else:
        print(f">> Copying clean asset hierarchy...")
        shutil.copytree(
            assets_src,
            assets_dst,
            ignore=shutil.ignore_patterns("*.tmp", "*.log", ".gitkeep", "*.bak", "scratch*")
        )
        print(f"  {GREEN}{ICON_OK}{RESET} Assets tree copied successfully.")

    # 5. Generate Documentation & Metadata
    generate_readme(target_dist, game_info)

    # 6. SteamPipe Integration
    effective_appid = app_id or game_info["default_appid"]
    if steam_mode:
        print(f">> Generating SteamPipe deployment scripts for AppID {effective_appid}...")
        (target_dist / "steam_appid.txt").write_text(f"{effective_appid}\n", encoding="utf-8")
        steampipe_dir = target_dist / "steampipe"
        create_steampipe_manifests(steampipe_dir, game_info, effective_appid, target_dist)
        print(f"  {GREEN}{ICON_OK}{RESET} SteamPipe manifests generated at: {steampipe_dir.relative_to(ROOT_DIR)}")

    # 7. Create Zip Archive if requested
    if zip_package:
        zip_name = f"{game_info['target']}_v12.0.0_windows_x64.zip"
        zip_path = dist_root / zip_name
        print(f">> Compressing standalone distribution into '{zip_name}'...")
        with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
            for root, _, files in os.walk(target_dist):
                for f in files:
                    full_p = Path(root) / f
                    rel_p = full_p.relative_to(dist_root)
                    zf.write(full_p, rel_p.as_posix())
        print(f"  {GREEN}{ICON_OK}{RESET} ZIP Archive ready: {zip_path} ({zip_path.stat().st_size / (1024*1024):.2f} MB)")

    print(f"\n{BOLD}{GREEN}{ICON_OK} Export complete for '{game_info['title']}'!{RESET}")
    print(f"Distribution Folder: {BOLD}{target_dist}{RESET}\n")
    return 0

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Starlight Engine Automated Game Exporter")
    parser.add_argument("game", help="Name or alias of the game to export (e.g. pong, snake, tetris, fight, godhand, odyssey)")
    parser.add_argument("--zip", action="store_true", help="Create compressed .zip release archive")
    parser.add_argument("--steam", action="store_true", help="Generate SteamPipe manifests and steam_appid.txt")
    parser.add_argument("--appid", type=str, help="Custom Steam AppID (default: 480)")
    parser.add_argument("--pak", action="store_true", help="Pack assets into single game.pak VFS archive")
    parser.add_argument("--output", type=Path, help="Custom destination directory")

    args = parser.parse_args()
    sys.exit(export_game(
        args.game,
        zip_package=args.zip,
        steam_mode=args.steam,
        app_id=args.appid,
        pak_mode=args.pak,
        output_dir=args.output
    ))
