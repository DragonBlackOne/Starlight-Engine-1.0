import os
from PIL import Image

tex_dir = "assets"
textures = ["palm_bark_diffuse.png", "leaves_diffuse.png", "ground_grass_diffuse.png"]

print("Checking texture dimensions...")
for t in textures:
    path = os.path.join(tex_dir, t)
    if os.path.exists(path):
        img = Image.open(path)
        w, h = img.size
        print(f"{t}: {w}x{h}")
        if (w * 4) % 256 != 0:
            print(f"  WARNING: Width {w} is NOT 256-byte aligned (Row bytes: {w*4})")
            print(f"  Mod 256: {(w*4)%256}")
    else:
        print(f"{t}: Not found")
