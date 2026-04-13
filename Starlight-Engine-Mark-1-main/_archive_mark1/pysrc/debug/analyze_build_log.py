
import sys
import os

def analyze(filename):
    if not os.path.exists(filename):
        print(f"File not found: {filename}")
        return

    content = None
    encodings = ['utf-16', 'utf-16le', 'utf-8', 'cp1252', 'latin1']
    
    for enc in encodings:
        try:
            with open(filename, 'r', encoding=enc) as f:
                content = f.read()
            print(f"Successfully read with encoding: {enc}")
            break
        except Exception:
            continue
            
    if content is None:
        print("Failed to read file with any common encoding.")
        return

    lines = content.splitlines()
    print(f"Total lines: {len(lines)}")
    
    print("\n--- LAST 50 LINES ---")
    for line in lines[-50:]:
        print(line)
        
    print("\n--- ERRORS FOUND ---")
    error_count = 0
    for i, line in enumerate(lines):
        if "error[" in line or "error:" in line:
            print(f"Line {i+1}: {line.strip()}")
            # Print context
            for j in range(1, 4):
                if i+j < len(lines):
                    print(f"  {lines[i+j].strip()}")
            error_count += 1
            if error_count > 10:
                print("... (too many errors) ...")
                break

if __name__ == "__main__":
    if len(sys.argv) > 1:
        analyze(sys.argv[1])
    else:
        analyze("build_log_hdr.txt")
