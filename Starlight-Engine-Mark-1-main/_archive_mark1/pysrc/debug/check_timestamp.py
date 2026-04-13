import os
import time

dll = "target/debug/backend.dll"
pyd = "pysrc/starlight/backend.pyd"

def print_time(path):
    if os.path.exists(path):
        mtime = os.path.getmtime(path)
        print(f"{path}: {time.ctime(mtime)}")
    else:
        print(f"{path}: Not found")

print_time(dll)
print_time(pyd)
