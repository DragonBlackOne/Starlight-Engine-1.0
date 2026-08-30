# generate_ps2_textures.py
# Generates high-fidelity PS2 hand-painted texture maps for God Hand

import math
import os
import struct

def make_png(filename, width, height, pixels):
    """Simple uncompressed PNG writer in pure Python without external dependencies"""
    import zlib
    
    # Pack raw scanlines
    raw_data = bytearray()
    for y in range(height):
        raw_data.append(0) # filter type 0: None
        for x in range(width):
            r, g, b, a = pixels[y * width + x]
            raw_data.extend((r, g, b, a))
            
    compressed = zlib.compress(bytes(raw_data), 9)
    
    # PNG signature
    png = bytearray(b'\x89PNG\r\n\x1a\n')
    
    # IHDR
    ihdr_data = struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0)
    ihdr_crc = zlib.crc32(b'IHDR' + ihdr_data)
    png.extend(struct.pack('>I', len(ihdr_data)) + b'IHDR' + ihdr_data + struct.pack('>I', ihdr_crc))
    
    # IDAT
    idat_crc = zlib.crc32(b'IDAT' + compressed)
    png.extend(struct.pack('>I', len(compressed)) + b'IDAT' + compressed + struct.pack('>I', idat_crc))
    
    # IEND
    iend_crc = zlib.crc32(b'IEND')
    png.extend(struct.pack('>I', 0) + b'IEND' + struct.pack('>I', iend_crc))
    
    os.makedirs(os.path.dirname(os.path.abspath(filename)), exist_ok=True)
    with open(filename, 'wb') as f:
        f.write(png)
    print(f"Generated texture: {filename} ({width}x{height})")

def generate_gene_face(path):
    W, H = 256, 256
    pixels = []
    for y in range(H):
        ny = y / H
        for x in range(W):
            nx = x / W
            # Base Skin Tone
            r, g, b = 240, 194, 158
            
            # Subtle shaded facial gradients
            dist_center = math.sqrt((nx - 0.5)**2 + (ny - 0.5)**2)
            shade = 1.0 - dist_center * 0.35
            r = int(r * shade)
            g = int(g * shade)
            b = int(b * shade)
            
            # Anime Eyes (y ~ 0.42 to 0.52, x ~ 0.32 and 0.68)
            for eye_x in [0.35, 0.65]:
                if abs(nx - eye_x) < 0.08 and abs(ny - 0.44) < 0.04:
                    # Eye White & Dark Iris
                    iris_dist = math.sqrt(((nx - eye_x)*1.5)**2 + ((ny - 0.44)*2.0)**2)
                    if iris_dist < 0.045:
                        r, g, b = 45, 30, 20 # Dark brown iris / pupil
                    else:
                        r, g, b = 245, 245, 250 # Sclera
                # Eyebrows (y ~ 0.36)
                if abs(nx - eye_x) < 0.09 and abs(ny - 0.36) < 0.02:
                    r, g, b = 70, 50, 35
                    
            # Gene's Signature Cheek Bandage (under left eye, x ~ 0.32, y ~ 0.56)
            if abs(nx - 0.33) < 0.07 and abs(ny - 0.55) < 0.022:
                r, g, b = 245, 242, 235
                if abs(nx - 0.33) > 0.06 or abs(ny - 0.55) > 0.018:
                    r, g, b = 180, 160, 140 # Bandage border
                    
            # Mouth / Grin (y ~ 0.72, x ~ 0.50)
            if abs(nx - 0.50) < 0.08 and abs(ny - 0.70) < 0.015:
                r, g, b = 160, 85, 75
                
            pixels.append((max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b)), 255))
    make_png(path, W, H, pixels)

def generate_gene_coat(path):
    W, H = 256, 256
    pixels = []
    for y in range(H):
        ny = y / H
        for x in range(W):
            nx = x / W
            # Dark Slate Blue Denim Weave
            grain = (math.sin(x * 1.5) * math.cos(y * 1.5)) * 12
            r = int(34 + grain)
            g = int(42 + grain)
            b = int(64 + grain)
            
            # Orange Braided Chevron Seams
            for seam_x in [0.22, 0.78]:
                if abs(nx - seam_x) < 0.04:
                    # Orange / Bronze embroidery
                    zig = math.sin(ny * 40.0) * 0.015
                    if abs(nx - seam_x - zig) < 0.02:
                        r, g, b = 230, 115, 30
                    else:
                        r, g, b = 180, 85, 20
                        
            # Lower Hem Fringe Tassels (ny > 0.88)
            if ny > 0.88:
                tassel_stripe = int((x % 8) < 4)
                if tassel_stripe:
                    r, g, b = 235, 120, 25
                else:
                    r, g, b = 170, 75, 15
                    
            pixels.append((max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b)), 255))
    make_png(path, W, H, pixels)

def generate_gene_godhand(path):
    W, H = 256, 256
    pixels = []
    for y in range(H):
        ny = y / H
        for x in range(W):
            nx = x / W
            # Layered Polished Silver/Steel Plates
            plate_y = (y % 48) / 48.0
            spec = math.exp(-((plate_y - 0.3)**2) * 20.0) * 90.0
            r = int(210 + spec)
            g = int(218 + spec)
            b = int(230 + spec)
            
            # Gold filigree trim / runes
            if abs(nx - 0.5) < 0.06 or plate_y < 0.08:
                r, g, b = 245, 205, 50
                
            pixels.append((max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b)), 255))
    make_png(path, W, H, pixels)

def generate_gene_pants(path):
    W, H = 256, 256
    pixels = []
    for y in range(H):
        ny = y / H
        for x in range(W):
            nx = x / W
            # Rich Dark Brown Leather
            grain = (math.sin(x * 0.8) + math.cos(y * 0.8)) * 8
            r = int(68 + grain)
            g = int(44 + grain)
            b = int(28 + grain)
            
            # Thigh Harness Strap (ny ~ 0.35)
            if abs(ny - 0.35) < 0.04:
                r, g, b = 25, 18, 12
                # Silver buckle rivet
                if abs(nx - 0.5) < 0.05:
                    r, g, b = 220, 220, 230
                    
            # Knee Pad (ny ~ 0.55)
            if abs(ny - 0.55) < 0.07 and abs(nx - 0.5) < 0.30:
                r, g, b = 45, 30, 20
                
            # Riding Boot with Heel (ny > 0.68)
            if ny > 0.68:
                r = int(32 + grain * 0.5)
                g = int(22 + grain * 0.5)
                b = int(15 + grain * 0.5)
                
            pixels.append((max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b)), 255))
    make_png(path, W, H, pixels)

def generate_adobe_wall(path):
    W, H = 256, 256
    pixels = []
    for y in range(H):
        ny = y / H
        for x in range(W):
            nx = x / W
            # Warm Adobe Stucco Texture
            noise = (math.sin(x * 0.4) * math.cos(y * 0.4) + math.sin(x * 1.2 + y * 0.8)) * 10
            r = int(216 + noise)
            g = int(198 + noise)
            b = int(164 + noise)
            
            # Weathered wood vigas beams along top (ny < 0.18)
            if ny < 0.18 and (x % 48) < 14:
                r, g, b = 90, 58, 32
                
            pixels.append((max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b)), 255))
    make_png(path, W, H, pixels)

def generate_desert_sand(path):
    W, H = 256, 256
    pixels = []
    for y in range(H):
        for x in range(W):
            # Rippled sand dune texture
            dune_wave = math.sin(x * 0.15 + math.sin(y * 0.08) * 4.0) * 14.0
            grain = (math.sin(x * 2.0) * math.cos(y * 2.0)) * 6.0
            r = int(200 + dune_wave + grain)
            g = int(165 + dune_wave * 0.85 + grain)
            b = int(118 + dune_wave * 0.65 + grain)
            pixels.append((max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b)), 255))
    make_png(path, W, H, pixels)

if __name__ == '__main__':
    base = "GodHand_Project/assets/textures"
    generate_gene_face(f"{base}/gene_face_diffuse.png")
    generate_gene_coat(f"{base}/gene_coat_diffuse.png")
    generate_gene_godhand(f"{base}/gene_godhand_diffuse.png")
    generate_gene_pants(f"{base}/gene_pants_diffuse.png")
    generate_adobe_wall(f"{base}/adobe_wall_diffuse.png")
    generate_desert_sand(f"{base}/desert_sand_diffuse.png")
