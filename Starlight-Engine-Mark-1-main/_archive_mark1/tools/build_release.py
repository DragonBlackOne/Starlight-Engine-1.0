import os
import shutil
import subprocess
import sys

def build_release():
    print("[1/5] Building Rust Core (Release)...")
    # Using 'rust-build' skill logic
    subprocess.check_call(["cargo", "build", "--release", "-p", "engine_core"])

    print("[2/5] Creating Build Directory...")
    if os.path.exists("Build"):
        shutil.rmtree("Build")
    os.makedirs("Build/pysrc/engine")
    os.makedirs("Build/assets")

    print("[3/5] Copying Assets & Python Source...")
    shutil.copytree("assets", "Build/assets", dirs_exist_ok=True)
    shutil.copytree("pysrc/starlight", "Build/pysrc/starlight", dirs_exist_ok=True)
    
    # Copy backend artifact
    src_dll = "target/release/backend.dll"
    dst_pyd = "Build/pysrc/engine/backend.pyd"
    if not os.path.exists(src_dll):
        # Fallback for Windows sometimes naming it .pyd if configured in cargo
        src_dll = "target/release/backend.pyd"
    
    if os.path.exists(src_dll):
        shutil.copy2(src_dll, dst_pyd)
        print(f"   Copied {src_dll} -> {dst_pyd}")
    else:
        print(f"ERROR: Could not find {src_dll}")
        sys.exit(1)

    print("[4/5] Copying Demo Scripts...")
    shutil.copy2("demos/forest_magic.py", "Build/launch.py")

    print("[5/5] Done! Run 'python launch.py' inside Build/ folder.")

if __name__ == "__main__":
    build_release()
