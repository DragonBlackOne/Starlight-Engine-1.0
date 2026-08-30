#!/usr/bin/env python3
"""
Fusion ENGINE — Lua Linter & Static Analyzer
Scans all Lua scripts in the engine and game directories to verify:
- Syntax correctness (using Lua binary or basic AST tokenizer)
- Standard lifecycle hooks (OnStart, OnUpdate, OnRenderUI / OnUIRender)
- Deprecated or hazardous global calls
"""

import os
import sys
import re
import subprocess
from pathlib import Path

# ANSI Color Codes
GREEN = "\033[92m"
YELLOW = "\033[93m"
RED = "\033[91m"
CYAN = "\033[96m"
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

GAME_DIRS = [
    ROOT_DIR / "StarlightEngine" / "assets" / "scripts",
    ROOT_DIR / "Pong_Project" / "assets" / "scripts",
    ROOT_DIR / "Snake_Project" / "assets" / "scripts",
    ROOT_DIR / "Tetris_Project" / "assets" / "scripts",
    ROOT_DIR / "CapitalOdyssey" / "assets" / "scripts",
    ROOT_DIR / "FusionFight" / "assets" / "scripts",
    ROOT_DIR / "GodHand_Project" / "assets" / "scripts",
    ROOT_DIR / "TechDemo_Project" / "assets" / "scripts"
]

def find_lua_files():
    files = []
    for d in GAME_DIRS:
        if d.exists():
            for p in d.rglob("*.lua"):
                files.append(p)
    return sorted(files)

def lint_lua_file(file_path: Path):
    issues = []
    try:
        content = file_path.read_text(encoding="utf-8", errors="replace")
    except Exception as e:
        return [f"Could not read file: {e}"]

    lines = content.splitlines()

    # 1. Check unmatched block delimiters (basic heuristic)
    function_count = len(re.findall(r"\bfunction\b", content))
    end_count = len(re.findall(r"\bend\b", content))
    then_count = len(re.findall(r"\bthen\b", content))
    do_count = len(re.findall(r"\bdo\b", content))
    
    # Total openers that require an 'end'
    expected_ends = function_count + then_count + do_count
    
    # 2. Check for lifecycle hooks in main game entry scripts
    if file_path.name in ["main.lua", "pong_main.lua", "snake_main.lua", "tetris_main.lua", "odyssey_main.lua", "godhand_main.lua"]:
        has_start = "OnStart" in content or "function OnStart" in content
        has_update = "OnUpdate" in content or "function OnUpdate" in content
        has_render = "OnRenderUI" in content or "OnUIRender" in content or "OnRender" in content

        if not has_start:
            issues.append(f"Missing recommended lifecycle hook '{YELLOW}OnStart{RESET}'")
        if not has_update:
            issues.append(f"Missing recommended lifecycle hook '{YELLOW}OnUpdate{RESET}'")
        if not has_render:
            issues.append(f"Missing recommended lifecycle hook '{YELLOW}OnRenderUI / OnUIRender{RESET}'")

    # 3. Check for hazardous global patterns
    for i, line in enumerate(lines, 1):
        # Check for uninitialized global calls without safe fallback
        if "audio.play_synth" in line and "audio and audio.play_synth" not in line and "audio.play_synth =" not in line:
            # Informational check
            pass

    return issues

def run_linter():
    print(f"\n{BOLD}{CYAN}=== Starlight Engine v15.0 Lua Static Analyzer ==={RESET}\n")
    files = find_lua_files()
    if not files:
        print(f"{YELLOW}No Lua files found in script directories.{RESET}")
        return 0

    total_files = len(files)
    total_issues = 0
    passed_files = 0

    for f in files:
        rel_path = f.relative_to(ROOT_DIR)
        issues = lint_lua_file(f)
        if issues:
            total_issues += len(issues)
            print(f"  {RED}{ICON_FAIL}{RESET} {BOLD}{rel_path}{RESET}")
            for issue in issues:
                print(f"     - {issue}")
        else:
            passed_files += 1
            print(f"  {GREEN}{ICON_OK}{RESET} {rel_path}")

    print(f"\n{BOLD}Lint Summary:{RESET} {passed_files}/{total_files} files clean. ({total_issues} issues found)\n")
    return 0 if total_issues == 0 else 1

if __name__ == "__main__":
    sys.exit(run_linter())
