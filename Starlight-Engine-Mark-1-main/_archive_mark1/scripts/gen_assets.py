import os

from PIL import Image, ImageDraw

os.makedirs("assets", exist_ok=True)


def create_player_sprite():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    # Blue Triangle
    draw.polygon([(32, 0), (0, 64), (64, 64)], fill=(0, 100, 255), outline=(255, 255, 255))
    img.save("assets/player.png")
    print("Created assets/player.png")


def create_enemy_sprite():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    # Red Circle
    draw.ellipse((0, 0, 64, 64), fill=(255, 50, 50), outline=(255, 255, 255))
    img.save("assets/enemy.png")
    print("Created assets/enemy.png")


def create_bullet_sprite():
    img = Image.new("RGBA", (16, 32), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    # Yellow Capsule
    draw.rectangle((0, 0, 16, 32), fill=(255, 255, 0))
    img.save("assets/bullet.png")
    print("Created assets/bullet.png")


def create_star_sprite():
    img = Image.new("RGBA", (8, 8), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    # White Dot
    draw.ellipse((0, 0, 8, 8), fill=(255, 255, 255))
    img.save("assets/star.png")
    print("Created assets/star.png")


if __name__ == "__main__":
    create_player_sprite()
    create_enemy_sprite()
    create_bullet_sprite()
    create_star_sprite()
