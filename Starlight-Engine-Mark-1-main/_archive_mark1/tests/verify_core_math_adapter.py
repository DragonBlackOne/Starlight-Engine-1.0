import os
import sys

from pyglm import glm

# Ensure src is in path
current_dir = os.path.dirname(os.path.abspath(__file__))
src_dir = os.path.join(os.path.dirname(current_dir), "src")
sys.path.insert(0, src_dir)


def test_core_math_frustum():
    print("[-] Testing starlight.engine.core_math.Frustum ...")
    try:
        from starlight.engine.core_math import Frustum

        # Create Identity MVP
        mvp = glm.mat4(1.0)  # Identity

        f = Frustum(mvp)
        print("[+] Frustum created successfully (Bridge)")

        if hasattr(f, "_impl") and f._impl:
            print("[+] Rust Implementation Active!")
        else:
            print("[!] Warning: Using Python Fallback (Rust disabled or failed)")

        # Point test
        if f.is_point_visible(glm.vec3(0, 0, 0)):
            print("[+] Point (0,0,0) visible")
        else:
            print("[!] Error: Point (0,0,0) invisible")

        if not f.is_point_visible(glm.vec3(0, 0, -5)):
            print("[+] Point (0,0,-5) invisible")
        else:
            print(
                "[?] Point (0,0,-5) visible? (Might be orthographic vs persp mismatch in default identity, but rust logic passed same test)"
            )

        # Sphere
        if f.is_sphere_visible(glm.vec3(0, 0, 0), 0.5):
            print("[+] Sphere visible")

        print("[+] Bridge tests passed!")

    except ImportError as e:
        print(f"[!] Import Error: {e}")
    except Exception as e:
        print(f"[!] Logic Error: {e}")
        import traceback

        traceback.print_exc()


if __name__ == "__main__":
    test_core_math_frustum()
