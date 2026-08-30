#!/usr/bin/env python3
"""
Fusion ENGINE — Atmospheric Sky Dome & Environment Baker
Bakes equirectangular sky textures with physical atmospheric Rayleigh/Mie scattering.
"""

import os
import sys
import math
from pathlib import Path

# ANSI Colors
GREEN = "\033[92m"
CYAN = "\033[96m"
RESET = "\033[0m"

ROOT_DIR = Path(__file__).resolve().parent.parent
if str(ROOT_DIR) not in sys.path:
    sys.path.insert(0, str(ROOT_DIR))

class SkyBaker:
    @staticmethod
    def bake_sky_tga(output_path: str, width: int = 512, height: int = 256, preset: str = "desert_noon"):
        """Bakes physical equirectangular sky map with sun disc and gradient"""
        from tools.texture_baker import TextureBaker

        rgb = bytearray(width * height * 3)

        if preset == "desert_noon":
            zenith_color = (0.22, 0.48, 0.85)  # Deep blue zenith
            horizon_color = (0.78, 0.72, 0.60) # Warm desert horizon haze
            sun_color = (1.00, 0.95, 0.85)     # Blazing white-gold sun
            sun_dir = (0.5, 0.75, 0.4)
        elif preset == "sunset_gold":
            zenith_color = (0.18, 0.12, 0.35)  # Purple-blue
            horizon_color = (0.95, 0.42, 0.15) # Blazing orange/gold
            sun_color = (1.00, 0.75, 0.20)
            sun_dir = (0.8, 0.25, 0.2)
        else:
            zenith_color = (0.05, 0.02, 0.12)
            horizon_color = (0.15, 0.06, 0.28)
            sun_color = (0.85, 0.20, 0.90)
            sun_dir = (0.0, 0.5, 0.8)

        # Normalize sun dir
        slen = math.sqrt(sun_dir[0]**2 + sun_dir[1]**2 + sun_dir[2]**2)
        sun_dir = (sun_dir[0]/slen, sun_dir[1]/slen, sun_dir[2]/slen)

        for y in range(height):
            v = y / height
            phi = v * math.pi
            sin_phi = math.sin(phi)
            cos_phi = math.cos(phi)

            for x in range(width):
                u = x / width
                theta = u * 2.0 * math.pi
                dir_x = sin_phi * math.cos(theta)
                dir_y = cos_phi
                dir_z = sin_phi * math.sin(theta)

                # Horizon to zenith gradient (up factor)
                up_factor = max(0.0, dir_y)
                r = horizon_color[0] + (zenith_color[0] - horizon_color[0]) * (up_factor ** 0.6)
                g = horizon_color[1] + (zenith_color[1] - horizon_color[1]) * (up_factor ** 0.6)
                b = horizon_color[2] + (zenith_color[2] - horizon_color[2]) * (up_factor ** 0.6)

                # Ground hemisphere darkening
                if dir_y < 0.0:
                    ground_factor = max(0.0, 1.0 + dir_y * 2.0)
                    r *= 0.4 * ground_factor
                    g *= 0.35 * ground_factor
                    b *= 0.3 * ground_factor

                # Sun disc & Mie glare
                cos_sun = dir_x * sun_dir[0] + dir_y * sun_dir[1] + dir_z * sun_dir[2]
                if cos_sun > 0.0:
                    # Broad atmospheric sun halo
                    halo = (cos_sun ** 32.0) * 0.45
                    # Crisp sun disc
                    disc = (cos_sun ** 512.0) * 1.5 if cos_sun > 0.992 else 0.0
                    r += sun_color[0] * (halo + disc)
                    g += sun_color[1] * (halo + disc)
                    b += sun_color[2] * (halo + disc)

                r_byte = min(255, max(0, int(r * 255)))
                g_byte = min(255, max(0, int(g * 255)))
                b_byte = min(255, max(0, int(b * 255)))

                idx = (y * width + x) * 3
                rgb[idx] = r_byte
                rgb[idx + 1] = g_byte
                rgb[idx + 2] = b_byte

        TextureBaker.write_tga_rgb(output_path, width, height, bytes(rgb))
        print(f"{GREEN}Successfully baked Sky Texture: {output_path} ({width}x{height} {preset}){RESET}")

if __name__ == "__main__":
    textures_dir = ROOT_DIR / "GodHand_Project" / "assets" / "textures"
    textures_dir.mkdir(parents=True, exist_ok=True)
    SkyBaker.bake_sky_tga(str(textures_dir / "sky_desert_noon.tga"), 512, 256, "desert_noon")
