try:
    with open("forest_v4_debug.log", "r", encoding="utf-8", errors="ignore") as f:
        print("Reading log...")
        for line in f:
            if "Result:" in line or "Loaded Texture" in line:
                print(line.strip())
except Exception as e:
    print(f"Error: {e}")
