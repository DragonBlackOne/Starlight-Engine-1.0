import os

from PIL import Image, ImageDraw

os.makedirs("assets", exist_ok=True)


def create_ricochet_assets():
    # Paddle (White Rect)
    img_paddle = Image.new("RGBA", (20, 100), (255, 255, 255, 255))
    img_paddle.save("assets/paddle.png")

    # Ball (White Circle)
    img_ball = Image.new("RGBA", (20, 20), (0, 0, 0, 0))
    # draw circle
    draw = ImageDraw.Draw(img_ball)
    draw.ellipse((0, 0, 20, 20), fill=(255, 255, 255, 255))
    img_ball.save("assets/ball.png")

    # Wall (White Horizontal)
    img_wall = Image.new("RGBA", (20, 20), (255, 255, 255, 255))  # Stretchable
    img_wall.save("assets/wall.png")

    print("Ricochet assets created.")


if __name__ == "__main__":
    create_ricochet_assets()
