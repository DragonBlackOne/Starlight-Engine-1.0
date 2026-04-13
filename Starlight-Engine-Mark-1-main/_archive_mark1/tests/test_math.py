
import math
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), "../pysrc"))
from starlight import math as smath


def test_proj():
    width = 1280
    height = 720
    fov_deg = 60.0
    near = 0.1
    far = 1000.0

    # 1. New Math Lib
    fov_y = fov_deg * math.pi / 180.0
    aspect = width / height
    proj = smath.perspective(fov_y, aspect, near, far)

    # 2. Manual (from forest_magic.py - known good)
    fov_y = 60.0 * math.pi / 180.0
    aspect = 1280.0 / 720.0
    val_f = 1.0 / math.tan(fov_y / 2.0)
    z_near = 0.1
    z_far = 1000.0

    manual_proj = [
        [val_f / aspect, 0.0, 0.0, 0.0],
        [0.0, val_f, 0.0, 0.0],
        [0.0, 0.0, z_far / (z_near - z_far), -1.0],
        [0.0, 0.0, (z_far * z_near) / (z_near - z_far), 0.0]
    ]

    print("MATCH?", proj == manual_proj)
    if proj != manual_proj:
        print("PROJ:", proj)
        print("MANUAL:", manual_proj)

if __name__ == "__main__":
    test_proj()
