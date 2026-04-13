
import os

file_path = "build_final_4.txt"
if os.path.exists(file_path):
    with open(file_path, "r", encoding="mbcs", errors="ignore") as f:
        print(f.read())
else:
    print("Log file not found.")
