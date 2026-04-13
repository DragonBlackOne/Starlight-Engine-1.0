import os
import sys
import traceback

# Add src to path
current_dir = os.path.dirname(os.path.abspath(__file__))
root_dir = os.path.dirname(current_dir)
src_path = os.path.join(root_dir, "src")
sys.path.append(src_path)
print(f"PYTHONPATH: {sys.path}")

try:
    print("Importing starlight.studio.main...")
    from starlight.studio.main import StudioEditor

    print("Instantiating StudioEditor...")
    editor = StudioEditor()

    print("Running on_create()...")
    editor.on_create()

    print("Running editor loop (mock)...")
    editor.run()

except Exception:
    print("CRASH DETECTED")
    traceback.print_exc()
