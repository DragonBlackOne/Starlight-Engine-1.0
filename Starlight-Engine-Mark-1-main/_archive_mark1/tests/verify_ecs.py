import os
import sys
import time

# Ensure src is in path
current_dir = os.path.dirname(os.path.abspath(__file__))
src_dir = os.path.join(os.path.dirname(current_dir), "src")
sys.path.insert(0, src_dir)


def test_rust_ecs():
    print("[-] Testing starlight_rust.World (ECS) ...")
    try:
        import starlight_rust

        if not hasattr(starlight_rust, "World"):
            print("[!] World class not found!")
            return

        world = starlight_rust.World()
        print("[+] World created")

        # Spawn 10,000 entities
        count = 100000
        print(f"[*] Spawning {count} entities...")
        for i in range(count):
            world.spawn_entity(0.0, 0.0, 0.0)
            # Add some velocity
            world.set_velocity(i, 1.0, 0.5, 0.0)

        print("[+] Entities spawned successfully")

        # Bench Update
        print("[*] Benchmarking physics update (Rayon Parallelized)...")
        start = time.perf_counter()
        steps = 60
        for _ in range(steps):
            world.update(0.016)
        end = time.perf_counter()

        duration = end - start
        avg = duration / steps * 1000.0
        print(f"[+] {steps} Updates took {duration:.4f}s. Avg: {avg:.4f}ms per frame")

        # Verify position
        # Entity 0 started at 0,0,0 with vel 1,0.5,0
        # After 60 updates of 0.016s -> total time 0.96s
        # Pos should be approx 0.96, 0.48, 0

        positions = world.get_positions()
        print(f"[+] Retrieved positions array: {positions.shape}")

        p0 = positions[0]
        print(f"[*] Entity 0 Position: {p0}")

        expected_x = 1.0 * (steps * 0.016)
        if abs(p0[0] - expected_x) < 0.1:
            print("[+] Physics Integration Verified OK")
        else:
            print(f"[!] Physics Drift/Error! Expected X ~{expected_x}, got {p0[0]}")

    except Exception as e:
        print(f"[!] Error: {e}")
        import traceback

        traceback.print_exc()


if __name__ == "__main__":
    test_rust_ecs()
