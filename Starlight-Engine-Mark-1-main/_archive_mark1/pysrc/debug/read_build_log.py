import time
import os

path = "build_log.txt"
print("Waiting for build_log.txt...")
for i in range(20):
    if os.path.exists(path):
        with open(path, "r") as f:
            print(f.read())
        break
    time.sleep(1)
else:
    print("Timeout waiting for build_log.txt")
