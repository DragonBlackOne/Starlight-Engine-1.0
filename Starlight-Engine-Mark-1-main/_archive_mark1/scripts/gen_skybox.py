import os
import random

from PIL import Image, ImageDraw


def create_space_texture(filename, width=1024, height=1024):
    img = Image.new('RGB', (width, height), color=(5, 5, 10))
    draw = ImageDraw.Draw(img)

    # Draw stars
    for _ in range(500):
        x = random.randint(0, width)
        y = random.randint(0, height)
        brightness = random.randint(100, 255)
        size = random.randint(0, 2)
        draw.rectangle([x, y, x+size, y+size], fill=(brightness, brightness, brightness))

    os.makedirs(os.path.dirname(filename), exist_ok=True)
    img.save(filename)
    print(f"Created {filename}")

if __name__ == "__main__":
    base_dir = "assets/skybox"
    faces = ["right", "left", "top", "bottom", "front", "back"]

    for face in faces:
        create_space_texture(os.path.join(base_dir, f"{face}.jpg"))
