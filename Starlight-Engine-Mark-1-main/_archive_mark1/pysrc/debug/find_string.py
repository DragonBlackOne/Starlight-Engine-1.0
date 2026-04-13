
import os

file_path = "crates/engine_render/src/renderer/state.rs"
target = "Blit BG"
target2 = "create_bind_group"

with open(file_path, "r", encoding="utf-8") as f:
    lines = f.readlines()

print(f"Searching for '{target}' and '{target2}' in {file_path}...")
for i, line in enumerate(lines):
    if target in line:
        print(f"Match '{target}' at line {i+1}: {line.strip()}")
    if target2 in line:
        # Too many matches for create_bind_group, maybe just count?
        # print(f"Match '{target2}' at line {i+1}: {line.strip()}")
        pass

# Also check for "descriptor (2)" related code? No.
