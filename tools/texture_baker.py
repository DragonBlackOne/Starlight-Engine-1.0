#!/usr/bin/env python3
"""
Fusion ENGINE — Pure Python PBR Texture Baker & Material Synthesizer
Generates 100% self-contained Normal Maps (.tga / .bmp) and PBR materials without third-party dependencies.
"""

import os
import sys
import math
import struct
from pathlib import Path

# ANSI Colors
GREEN = "\033[92m"
YELLOW = "\033[93m"
CYAN = "\033[96m"
BOLD = "\033[1m"
RESET = "\033[0m"

ROOT_DIR = Path(__file__).resolve().parent.parent

class TextureBaker:
    @staticmethod
    def write_tga_rgb(filepath: str, width: int, height: int, rgb_bytes: bytes):
        """Writes uncompressed 24-bit RGB TGA file"""
        header = bytearray(18)
        header[2] = 2  # Uncompressed RGB
        header[12] = width & 0xFF
        header[13] = (width >> 8) & 0xFF
        header[14] = height & 0xFF
        header[15] = (height >> 8) & 0xFF
        header[16] = 24 # 24 bits per pixel
        header[17] = 0x20 # Top-to-bottom orientation

        # Convert RGB to BGR for TGA
        bgr_data = bytearray(len(rgb_bytes))
        for i in range(0, len(rgb_bytes), 3):
            bgr_data[i] = rgb_bytes[i + 2]     # B
            bgr_data[i + 1] = rgb_bytes[i + 1] # G
            bgr_data[i + 2] = rgb_bytes[i]     # R

        with open(filepath, "wb") as f:
            f.write(header)
            f.write(bgr_data)

    @staticmethod
    def generate_procedural_normal_map(output_path: str, width: int = 512, height: int = 512, pattern: str = "organic", strength: float = 2.0):
        """Generates rich procedural microfacet normal maps (organic, stone, leather, metal)"""
        rgb = bytearray(width * height * 3)

        for y in range(height):
            for x in range(width):
                u = x / width
                v = y / height

                if pattern == "leather":
                    # Fine grain leather bumps
                    n1 = math.sin(u * 120.0) * math.cos(v * 120.0)
                    n2 = math.sin(u * 280.0 + v * 280.0) * 0.5
                    height_val = n1 + n2
                    dx = math.cos(u * 120.0) * 120.0 * math.cos(v * 120.0)
                    dy = -math.sin(u * 120.0) * math.sin(v * 120.0) * 120.0
                elif pattern == "stone":
                    # Weathered adobe stucco relief
                    n1 = math.sin(u * 40.0) * math.cos(v * 40.0)
                    n2 = math.sin(u * 80.0) * 0.4
                    dx = math.cos(u * 40.0) * 40.0 * math.cos(v * 40.0)
                    dy = -math.sin(u * 40.0) * math.sin(v * 40.0) * 40.0
                elif pattern == "sand":
                    # Desert wind ripples
                    freq = 60.0
                    dx = math.cos(u * freq + math.sin(v * 20.0) * 2.0) * freq
                    dy = math.sin(v * 20.0) * 20.0
                else:
                    # Contoured muscle / anatomical relief
                    dx = math.sin(u * 16.0) * 8.0
                    dy = math.cos(v * 16.0) * 8.0

                nx = -dx * strength * 0.02
                ny = -dy * strength * 0.02
                nz = 1.0

                length = math.sqrt(nx*nx + ny*ny + nz*nz)
                if length > 1e-5:
                    nx /= length
                    ny /= length
                    nz /= length

                r = int((nx * 0.5 + 0.5) * 255)
                g = int((ny * 0.5 + 0.5) * 255)
                b = int((nz * 0.5 + 0.5) * 255)

                idx = (y * width + x) * 3
                rgb[idx] = r
                rgb[idx + 1] = g
                rgb[idx + 2] = b

        TextureBaker.write_tga_rgb(output_path, width, height, bytes(rgb))
        print(f"{GREEN}Successfully baked Normal Map: {output_path} ({width}x{height} {pattern}){RESET}")

if __name__ == "__main__":
    textures_dir = ROOT_DIR / "GodHand_Project" / "assets" / "textures"
    textures_dir.mkdir(parents=True, exist_ok=True)

    TextureBaker.generate_procedural_normal_map(str(textures_dir / "gene_normal.tga"), 512, 512, "leather", strength=2.5)
    TextureBaker.generate_procedural_normal_map(str(textures_dir / "punk_normal.tga"), 512, 512, "leather", strength=2.5)
    TextureBaker.generate_procedural_normal_map(str(textures_dir / "desert_normal.tga"), 512, 512, "sand", strength=3.0)
    TextureBaker.generate_procedural_normal_map(str(textures_dir / "adobe_normal.tga"), 512, 512, "stone", strength=2.2)
