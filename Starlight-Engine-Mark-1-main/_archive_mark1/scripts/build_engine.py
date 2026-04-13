
import subprocess
import shutil
import os
import sys

def build_engine(release=True):
    # Determine project root (assuming script is in scripts/)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    root_dir = os.path.dirname(script_dir)
    
    cmd = ["cargo", "build", "-p", "engine_core"]
    if release:
        cmd.append("--release")
    
    print(f"Building engine in {root_dir}: {' '.join(cmd)}")
    ret = subprocess.call(cmd, cwd=root_dir)
    if ret != 0:
        print("Build failed.")
        sys.exit(ret)
    
    target_dir = os.path.join(root_dir, "target", "release" if release else "debug")
    src_dll = os.path.join(target_dir, "backend.dll")
    
    if not os.path.exists(src_dll):
        print(f"Artifact not found: {src_dll}")
        sys.exit(1)
        
    # Destination 1: inside package
    dst_pkg = os.path.join(root_dir, "pysrc", "starlight", "backend.pyd")
    print(f"Copying to package: {dst_pkg}")
    shutil.copy2(src_dll, dst_pkg)
    
    # Destination 2: root (for quick tests)
    dst_root = os.path.join(root_dir, "backend.pyd")
    print(f"Copying to root: {dst_root}")
    shutil.copy2(src_dll, dst_root)
    
    print("Build and deploy complete.")

if __name__ == "__main__":
    build_engine()
