import os
import sys
from PIL import Image # Requires pillow

def validate_assets():
    print("Validating Assets...")
    issues = 0
    
    # Check Textures
    tex_dir = "assets/textures"
    if os.path.exists(tex_dir):
        for root, _, files in os.walk(tex_dir):
            for f in files:
                if f.lower().endswith(('.png', '.jpg', '.jpeg')):
                    path = os.path.join(root, f)
                    try:
                        with Image.open(path) as img:
                            width, height = img.size
                            # Check Power of 2
                            if not ((width & (width-1) == 0) and (height & (height-1) == 0)):
                                print(f"[WARN] Texture {f} is {width}x{height} (Not Power of 2)")
                                issues += 1
                    except Exception as e:
                        print(f"[ERROR] Corrupt image {f}: {e}")
                        issues += 1
    
    print(f"Validation Complete. {issues} issues found.")

if __name__ == "__main__":
    try:
        validate_assets()
    except ImportError:
        print("Pillow not installed. Skipping texture checks.")
