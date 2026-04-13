import os
import random

from PIL import Image, ImageDraw, ImageFilter


def generate_noise_texture(path, size=(512, 512)):
    """Generates a procedural noise texture for monoliths."""
    img = Image.new("RGB", size, color=(30, 30, 35))  # Dark grey slate base
    draw = ImageDraw.Draw(img)

    # 1. Base noise pixels
    for _ in range(size[0] * size[1] // 2):
        x = random.randint(0, size[0] - 1)
        y = random.randint(0, size[1] - 1)
        c = random.randint(20, 60)
        draw.point((x, y), fill=(c, c, c + random.randint(0, 10)))

    # 2. Tech streaks / veins
    for _ in range(20):
        x1 = random.randint(0, size[0])
        y1 = random.randint(0, size[1])
        x2 = x1 + random.randint(-50, 50)
        y2 = y1 + random.randint(-50, 50)
        w = random.randint(1, 3)
        c = random.randint(100, 200)  # Brighter tech lines
        draw.line((x1, y1, x2, y2), fill=(c, c, c), width=w)

    # 3. Blur for softness
    img = img.filter(ImageFilter.SMOOTH)

    # Ensure directory exists
    os.makedirs(os.path.dirname(path), exist_ok=True)
    img.save(path)
    print(f"[ASSETS] Generated texture: {path}")


def generate_planet_texture(path, primary_color, secondary_color, noise_scale=0.5, size=(512, 512)):
    """Generates a procedural planet texture."""
    img = Image.new("RGB", size, color=primary_color)
    draw = ImageDraw.Draw(img)

    # Noise/Banding
    for y in range(size[1]):
        for x in range(size[0]):
            r = random.random()
            if r < noise_scale:
                # Blend colors
                factor = r / noise_scale
                c = tuple(
                    int(primary_color[i] * (1 - factor) + secondary_color[i] * factor)
                    for i in range(3)
                )
                draw.point((x, y), fill=c)

    img = img.filter(ImageFilter.GaussianBlur(radius=2))
    os.makedirs(os.path.dirname(path), exist_ok=True)
    img.save(path)
    print(f"[ASSETS] Generated texture: {path}")


if __name__ == "__main__":
    # Generate previous assets
    generate_noise_texture("assets/textures/monolith_noise.png")

    # Generate Solar System
    generate_planet_texture("assets/textures/sun.png", (255, 200, 50), (255, 255, 200), 0.8)
    generate_planet_texture("assets/textures/mercury.png", (100, 100, 100), (150, 130, 110), 0.4)
    generate_planet_texture("assets/textures/venus.png", (200, 180, 100), (255, 230, 150), 0.6)
    generate_planet_texture(
        "assets/textures/earth.png", (30, 100, 200), (50, 150, 50), 0.7
    )  # Oceans + Land
    generate_planet_texture("assets/textures/mars.png", (180, 50, 20), (100, 30, 10), 0.5)
    generate_planet_texture(
        "assets/textures/jupiter.png", (200, 150, 100), (100, 60, 30), 0.9
    )  # Bands
    generate_planet_texture("assets/textures/saturn.png", (220, 200, 150), (150, 130, 100), 0.8)
    generate_planet_texture("assets/textures/uranus.png", (150, 230, 255), (100, 200, 230), 0.3)
    generate_planet_texture("assets/textures/neptune.png", (50, 100, 255), (20, 50, 150), 0.3)
