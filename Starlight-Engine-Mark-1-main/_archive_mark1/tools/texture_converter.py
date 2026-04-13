import os
import sys
import subprocess
import urllib.request
import argparse

TEXCONV_URL = "https://github.com/microsoft/DirectXTex/releases/latest/download/texconv.exe"
TEXCONV_EXE = os.path.join(os.path.dirname(__file__), "texconv.exe")

def ensure_texconv():
    if not os.path.exists(TEXCONV_EXE):
        print(f"Downloading texconv.exe from {TEXCONV_URL}...")
        try:
            urllib.request.urlretrieve(TEXCONV_URL, TEXCONV_EXE)
            print("Download complete.")
        except Exception as e:
            print(f"Failed to download texconv.exe: {e}")
            sys.exit(1)

def convert_to_dds(input_file, output_dir=None, format="BC7_UNORM", generate_mips=True):
    ensure_texconv()
    
    cmd = [TEXCONV_EXE]
    cmd.extend(["-f", format])
    if generate_mips:
        cmd.extend(["-m", "0"]) # 0 means all mip levels down to 1x1
    else:
        cmd.extend(["-m", "1"])
    
    cmd.extend(["-y"]) # overwrite
    
    if output_dir:
        cmd.extend(["-o", output_dir])
        
    cmd.append(input_file)
    
    print(f"Converting: {input_file} -> {format} (Mips: {generate_mips})")
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode != 0:
        print(f"Error converting {input_file}:\n{result.stderr}\n{result.stdout}")
    else:
        print(f"Success.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Convert textures to DDS format using Microsoft texconv")
    parser.add_argument("input", help="Input image file or directory")
    parser.add_argument("-o", "--output", help="Output directory", default=None)
    parser.add_argument("-f", "--format", help="DXGI Format (default: BC7_UNORM). Use BC1_UNORM for RGB, BC3_UNORM for RGBA, BC7_UNORM for high quality.", default="BC7_UNORM")
    parser.add_argument("--no-mips", action="store_true", help="Disable mipmap generation")
    
    args = parser.parse_args()
    
    if os.path.isfile(args.input):
        convert_to_dds(args.input, args.output, args.format, not args.no_mips)
    elif os.path.isdir(args.input):
        for root, _, files in os.walk(args.input):
            for file in files:
                if file.lower().endswith(('.png', '.jpg', '.jpeg', '.tga')):
                    input_path = os.path.join(root, file)
                    out_dir = args.output if args.output else root
                    convert_to_dds(input_path, out_dir, args.format, not args.no_mips)
    else:
        print(f"Input path not found: {args.input}")
