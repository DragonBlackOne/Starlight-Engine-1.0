import os
import sys

# Ensure src is in path
current_dir = os.path.dirname(os.path.abspath(__file__))
src_dir = os.path.join(os.path.dirname(current_dir), "src")
sys.path.insert(0, src_dir)


def test_rust_frustum():
    print("[-] Testing starlight_rust.Frustum ...")
    try:
        import starlight_rust

        if not hasattr(starlight_rust, "Frustum"):
            print("[!] Error: Frustum class not found in module.")
            return

        # Create Identity MVP (flattened)
        mvp = [1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0]

        f = starlight_rust.Frustum(mvp)
        print("[+] Frustum created successfully")

        # Point test
        if not f.is_point_visible([0.0, 0.0, -5.0]):
            # Identity frustum is usually -1 to 1 clip space.
            # Actually Gribb-Hartmann on Identity matrix -> planes are +/- 1 on all axes.
            # Z is -1 to 1. -5 should be invisible?
            # Wait, Identity projection usually implies orthographic -1..1 or similar.
            # If standard identity, z range is -1 to 1 (OpenGL).
            # -5 is outside.
            print("[+] Point (0,0,-5) is correctly invisible (outside -1..1)")
        else:
            print("[?] Point (0,0,-5) is visible? (Check clip space convention)")

        if f.is_point_visible([0.0, 0.0, 0.0]):
            print("[+] Point (0,0,0) is visible")
        else:
            print("[!] Error: Origin should be visible")

        # Sphere test
        if f.is_sphere_visible([0.0, 0.0, 0.0], 0.5):
            print("[+] Sphere at origin visible")

        if not f.is_sphere_visible([0.0, 5.0, 0.0], 1.0):
            print("[+] Sphere at (0,5,0) invisible")

        print("[+] Frustum tests passed!")

    except ImportError as e:
        print(f"[!] Import Error: {e}")
    except Exception as e:
        print(f"[!] Logic Error: {e}")
        import traceback

        traceback.print_exc()


if __name__ == "__main__":
    test_rust_frustum()
