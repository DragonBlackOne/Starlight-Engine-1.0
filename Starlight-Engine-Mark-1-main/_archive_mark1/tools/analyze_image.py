import os
import sys

def analyze_png(filepath):
    print(f"Analyzing: {filepath}", flush=True)
    if not os.path.exists(filepath):
        print("Error: File not found", flush=True)
        return

    try:
        with open(filepath, 'rb') as f:
            data = f.read()
            
        size = len(data)
        print(f"File size: {size} bytes", flush=True)
        
        if size < 100:
            print("Error: File too small", flush=True)
            return

        if data[:8] != b'\x89PNG\r\n\x1a\n':
            print("Error: Not a valid PNG signature", flush=True)
            return

        # Find IDAT chunk
        idat_start = data.find(b'IDAT')
        if idat_start == -1:
            print("Error: No IDAT chunk found", flush=True)
            return
            
        # Analyze entropy of 1KB sample from IDAT
        sample_size = min(1000, len(data) - idat_start)
        sample = data[idat_start:idat_start+sample_size]
        unique_bytes = len(set(sample))
        
        print(f"Entropy/Complexity Metric: {unique_bytes}/{sample_size}", flush=True)
        
        if unique_bytes < 20:
            print("RESULT: LOW_DETAIL (Likely Black/Solid Screen)", flush=True)
        else:
            print("RESULT: HIGH_DETAIL (Likely Rendered Content)", flush=True)
            
    except Exception as e:
        print(f"Analysis failed: {e}", flush=True)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python analyze_image.py <image_path>", flush=True)
    else:
        analyze_png(sys.argv[1])
