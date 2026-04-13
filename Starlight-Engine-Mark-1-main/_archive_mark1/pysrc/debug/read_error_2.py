
import os

file_path = "error_log_2.txt"
if os.path.exists(file_path):
    try:
        with open(file_path, "r", encoding="utf-16") as f:
            print(f.read())
    except Exception as e:
        print(f"Error reading utf-16: {e}")
        # Try utf-8 just in case
        with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
            print(f.read())
else:
    print("Log file not found.")
