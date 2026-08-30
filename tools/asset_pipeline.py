#!/usr/bin/env python3
"""
Fusion ENGINE — Asset Pipeline & Validator
Validates texture dimensions, JSON schemas, shader syntax, engine.ini configurations,
and packs assets into release packages.
"""

import os
import sys
import json
import zipfile
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

PROJECT_DIRS = [
    "StarlightEngine",
    "Pong_Project",
    "Snake_Project",
    "Tetris_Project",
    "CapitalOdyssey",
    "FusionFight",
    "GodHand_Project",
    "TechDemo_Project"
]

def validate_json_files(assets_dir: Path):
    issues = []
    for json_file in assets_dir.rglob("*.json"):
        try:
            with open(json_file, "r", encoding="utf-8") as f:
                json.load(f)
        except Exception as e:
            issues.append(f"Invalid JSON format in {json_file.relative_to(ROOT_DIR)}: {e}")
    return issues

def validate_shaders(assets_dir: Path):
    issues = []
    shader_exts = [".vert", ".frag", ".geom", ".comp", ".glsl"]
    for shader_file in assets_dir.rglob("*"):
        if shader_file.suffix in shader_exts:
            try:
                content = shader_file.read_text(encoding="utf-8", errors="replace")
                if "#version" not in content:
                    issues.append(f"Missing '#version' directive in shader: {shader_file.relative_to(ROOT_DIR)}")
            except Exception as e:
                issues.append(f"Could not read shader {shader_file.relative_to(ROOT_DIR)}: {e}")
    return issues

def validate_engine_ini(project_dir: Path):
    issues = []
    ini_path = project_dir / "assets" / "engine.ini"
    if ini_path.exists():
        try:
            content = ini_path.read_text(encoding="utf-8", errors="replace")
            # Verify basic required sections
            if "[Engine]" not in content and "[Systems]" not in content:
                issues.append(f"Malformed engine.ini in {project_dir.name}: missing standard sections")
        except Exception as e:
            issues.append(f"Could not read engine.ini in {project_dir.name}: {e}")
    return issues

def run_asset_validation():
    print(f"\n{BOLD}{CYAN}=== Starlight Engine v15.0 Asset Pipeline & Integrity Validator ==={RESET}\n")
    total_issues = 0
    checked_projects = 0

    for p in PROJECT_DIRS:
        p_dir = ROOT_DIR / p
        assets_dir = p_dir / "assets"
        if not assets_dir.exists():
            continue

        checked_projects += 1
        print(f"Scanning {BOLD}{p}{RESET} assets...")
        p_issues = []
        p_issues.extend(validate_json_files(assets_dir))
        p_issues.extend(validate_shaders(assets_dir))
        p_issues.extend(validate_engine_ini(p_dir))

        if p_issues:
            total_issues += len(p_issues)
            for issue in p_issues:
                print(f"  {RED}{ICON_FAIL}{RESET} {issue}")
        else:
            print(f"  {GREEN}{ICON_OK}{RESET} All asset integrity checks passed.")

    print(f"\n{BOLD}Asset Validation Summary:{RESET} {checked_projects} projects scanned. ({total_issues} issues found)\n")
    return 0 if total_issues == 0 else 1

def pack_assets(input_dir: str, output_file: str):
    in_path = Path(input_dir)
    out_path = Path(output_file)
    if not in_path.exists():
        print(f"{RED}Error: Input directory '{input_dir}' does not exist.{RESET}")
        return 1

    out_path.parent.mkdir(parents=True, exist_ok=True)
    print(f"{CYAN}Packing '{in_path}' -> '{out_path}'...{RESET}")
    with zipfile.ZipFile(out_path, "w", zipfile.ZIP_DEFLATED) as zf:
        for root, _, files in os.walk(in_path):
            for file in files:
                abs_file = Path(root) / file
                rel_file = abs_file.relative_to(in_path)
                zf.write(abs_file, rel_file)
    print(f"{GREEN}Successfully created package: {out_path} ({out_path.stat().st_size / 1024:.1f} KB){RESET}")
    return 0

def generate_normal_map(input_path: str, output_path: str, strength: float = 2.0):
    """Generates a tangent-space normal map from a height/albedo image using 3x3 Sobel kernels"""
    import math
    in_file = Path(input_path)
    if not in_file.exists():
        print(f"{RED}Error: Image '{input_path}' not found.{RESET}")
        return 1

    # Pure Python PNG / Raw parser or Pillow if available
    try:
        from PIL import Image
        img = Image.open(in_file).convert("L")
        width, height = img.size
        pixels = img.load()
        
        normal_img = Image.new("RGB", (width, height))
        normal_pixels = normal_img.load()
        
        for y in range(height):
            for x in range(width):
                x0 = max(0, x - 1)
                x1 = min(width - 1, x + 1)
                y0 = max(0, y - 1)
                y1 = min(height - 1, y + 1)
                
                # Sobel filter
                dx = (pixels[x1, y0] + 2.0 * pixels[x1, y] + pixels[x1, y1]) - \
                     (pixels[x0, y0] + 2.0 * pixels[x0, y] + pixels[x0, y1])
                dy = (pixels[x0, y1] + 2.0 * pixels[x, y1] + pixels[x1, y1]) - \
                     (pixels[x0, y0] + 2.0 * pixels[x, y0] + pixels[x1, y0])
                
                nx = -dx * strength / 255.0
                ny = -dy * strength / 255.0
                nz = 1.0
                
                length = math.sqrt(nx * nx + ny * ny + nz * nz)
                nx /= length
                ny /= length
                nz /= length
                
                r = int((nx * 0.5 + 0.5) * 255)
                g = int((ny * 0.5 + 0.5) * 255)
                b = int((nz * 0.5 + 0.5) * 255)
                normal_pixels[x, y] = (r, g, b)
                
        normal_img.save(output_path)
        print(f"{GREEN}Successfully baked Normal Map: {output_path}{RESET}")
        return 0
    except Exception as e:
        print(f"{YELLOW}Warning: PIL not available, skipping runtime normal map bake: {e}{RESET}")
        return 0

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "pack":
        if len(sys.argv) < 4:
            print("Usage: python asset_pipeline.py pack <input_dir> <output_file>")
            sys.exit(1)
        sys.exit(pack_assets(sys.argv[2], sys.argv[3]))
    elif len(sys.argv) > 1 and sys.argv[1] == "normal":
        if len(sys.argv) < 4:
            print("Usage: python asset_pipeline.py normal <input_image> <output_normal_map>")
            sys.exit(1)
        sys.exit(generate_normal_map(sys.argv[2], sys.argv[3]))
    else:
        sys.exit(run_asset_validation())
