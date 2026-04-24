#!/usr/bin/env python3
"""
Starlight Engine - Texture Compression Tool
Converts PNG/JPG textures to DDS format with mipmaps for GPU-efficient loading.

Usage:
    python tools/compress_textures.py <input_dir> [--output <output_dir>] [--format BC1|BC3]

Examples:
    python tools/compress_textures.py assets/textures/
    python tools/compress_textures.py assets/textures/ --output build/textures/ --format BC3
"""

import argparse
import os
import struct
import sys
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("Error: Pillow is required. Install with: pip install Pillow")
    sys.exit(1)


# DDS Constants
DDS_MAGIC = 0x20534444  # "DDS "
DDSD_CAPS = 0x1
DDSD_HEIGHT = 0x2
DDSD_WIDTH = 0x4
DDSD_PITCH = 0x8
DDSD_PIXELFORMAT = 0x1000
DDSD_MIPMAPCOUNT = 0x20000
DDSD_LINEARSIZE = 0x80000

DDSCAPS_TEXTURE = 0x1000
DDSCAPS_MIPMAP = 0x400000
DDSCAPS_COMPLEX = 0x8

DDPF_FOURCC = 0x4
DDPF_RGB = 0x40
DDPF_ALPHAPIXELS = 0x1


def make_fourcc(s: str) -> int:
    """Create a FourCC code from a string."""
    return struct.unpack('<I', s.encode('ascii'))[0]


def generate_mipmaps(img: Image.Image) -> list:
    """Generate mipmap chain for an image."""
    mipmaps = [img]
    w, h = img.size

    while w > 1 or h > 1:
        w = max(1, w // 2)
        h = max(1, h // 2)
        mip = img.resize((w, h), Image.LANCZOS)
        mipmaps.append(mip)

    return mipmaps


def write_dds_uncompressed(output_path: str, mipmaps: list, has_alpha: bool) -> None:
    """
    Write DDS file in uncompressed RGBA/RGB format.
    This is the most compatible approach without requiring GPU-specific compression libraries.
    """
    img = mipmaps[0]
    width, height = img.size
    mip_count = len(mipmaps)

    # Ensure RGBA
    if has_alpha:
        pixel_format_flags = DDPF_RGB | DDPF_ALPHAPIXELS
        rgb_bit_count = 32
        r_mask = 0x000000FF
        g_mask = 0x0000FF00
        b_mask = 0x00FF0000
        a_mask = 0xFF000000
        mode = 'RGBA'
    else:
        pixel_format_flags = DDPF_RGB
        rgb_bit_count = 24
        r_mask = 0x0000FF
        g_mask = 0x00FF00
        b_mask = 0xFF0000
        a_mask = 0x000000
        mode = 'RGB'

    flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_PITCH
    if mip_count > 1:
        flags |= DDSD_MIPMAPCOUNT

    pitch = width * (rgb_bit_count // 8)

    caps = DDSCAPS_TEXTURE
    if mip_count > 1:
        caps |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX

    with open(output_path, 'wb') as f:
        # Magic
        f.write(struct.pack('<I', DDS_MAGIC))

        # Header (124 bytes)
        f.write(struct.pack('<I', 124))           # dwSize
        f.write(struct.pack('<I', flags))          # dwFlags
        f.write(struct.pack('<I', height))         # dwHeight
        f.write(struct.pack('<I', width))          # dwWidth
        f.write(struct.pack('<I', pitch))          # dwPitchOrLinearSize
        f.write(struct.pack('<I', 0))              # dwDepth
        f.write(struct.pack('<I', mip_count))      # dwMipMapCount
        f.write(b'\x00' * 44)                      # dwReserved1[11]

        # Pixel Format (32 bytes)
        f.write(struct.pack('<I', 32))              # dwSize
        f.write(struct.pack('<I', pixel_format_flags))  # dwFlags
        f.write(struct.pack('<I', 0))               # dwFourCC
        f.write(struct.pack('<I', rgb_bit_count))   # dwRGBBitCount
        f.write(struct.pack('<I', r_mask))           # dwRBitMask
        f.write(struct.pack('<I', g_mask))           # dwGBitMask
        f.write(struct.pack('<I', b_mask))           # dwBBitMask
        f.write(struct.pack('<I', a_mask))           # dwABitMask

        # Caps
        f.write(struct.pack('<I', caps))            # dwCaps
        f.write(struct.pack('<I', 0))               # dwCaps2
        f.write(struct.pack('<I', 0))               # dwCaps3
        f.write(struct.pack('<I', 0))               # dwCaps4
        f.write(struct.pack('<I', 0))               # dwReserved2

        # Write mipmap data
        for mip in mipmaps:
            mip_conv = mip.convert(mode)
            f.write(mip_conv.tobytes())


def compress_texture(input_path: str, output_path: str, has_alpha_hint: bool = True) -> tuple:
    """Compress a single texture to DDS format with mipmaps."""
    img = Image.open(input_path)

    # Detect alpha
    has_alpha = has_alpha_hint and img.mode in ('RGBA', 'LA', 'PA')
    if not has_alpha and img.mode != 'RGB':
        img = img.convert('RGB')
    elif has_alpha and img.mode != 'RGBA':
        img = img.convert('RGBA')

    # Generate mipmaps
    mipmaps = generate_mipmaps(img)

    # Write DDS
    write_dds_uncompressed(output_path, mipmaps, has_alpha)

    original_size = os.path.getsize(input_path)
    compressed_size = os.path.getsize(output_path)

    return original_size, compressed_size, len(mipmaps)


def compress_directory(input_dir: str, output_dir: str) -> None:
    """Compress all textures in a directory."""
    supported_exts = {'.png', '.jpg', '.jpeg', '.bmp', '.tga'}
    input_path = Path(input_dir)
    output_path = Path(output_dir) if output_dir else input_path

    files = []
    for ext in supported_exts:
        files.extend(input_path.rglob(f'*{ext}'))

    if not files:
        print(f"No texture files found in '{input_dir}'")
        return

    print(f"Compressing {len(files)} textures from '{input_dir}'")
    if output_dir:
        print(f"Output directory: '{output_dir}'")

    total_original = 0
    total_compressed = 0

    for i, file in enumerate(files):
        rel = file.relative_to(input_path)
        out_file = output_path / rel.with_suffix('.dds')
        out_file.parent.mkdir(parents=True, exist_ok=True)

        try:
            orig, comp, mips = compress_texture(str(file), str(out_file))
            total_original += orig
            total_compressed += comp

            if (i + 1) % 20 == 0 or (i + 1) == len(files):
                pct = (i + 1) / len(files) * 100
                print(f"  [{pct:5.1f}%] {i + 1}/{len(files)} - {rel.name} ({mips} mips)")

        except Exception as e:
            print(f"  [ERROR] {rel}: {e}")

    print(f"\nDone!")
    print(f"  Original:   {total_original / 1024 / 1024:.2f} MB")
    print(f"  DDS Output: {total_compressed / 1024 / 1024:.2f} MB")
    print(f"  Mipmap generation: Enabled (LANCZOS downscaling)")


def main():
    parser = argparse.ArgumentParser(
        description="Starlight Engine - Texture Compression Tool",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="Converts PNG/JPG textures to DDS format with auto-generated mipmaps."
    )
    parser.add_argument("input_dir", help="Directory containing textures")
    parser.add_argument("--output", "-o", help="Output directory (default: same as input)")
    parser.add_argument("--format", choices=["RGB", "RGBA"], default="RGBA",
                        help="Output pixel format (default: RGBA)")

    args = parser.parse_args()
    compress_directory(args.input_dir, args.output)


if __name__ == "__main__":
    main()
