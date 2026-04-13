
import os
import time
import subprocess

print("Running space_shooter.py...")
try:
    subprocess.run(["python", "demos/space_shooter.py"], check=True, timeout=30)
except subprocess.TimeoutExpired:
    print("Timeout expired (which is expected if it doesn't exit fast enough, but we added exit logic).")
except subprocess.CalledProcessError as e:
    print(f"Error: {e}")

if os.path.exists("space_shooter_phase6.png"):
    print("Screenshot captured successfully.")
else:
    print("Screenshot NOT found.")
