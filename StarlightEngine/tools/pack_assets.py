#!/usr/bin/env python3
"""
Starlight Engine - Asset Packer Tool
Creates .pak (zip) archives from asset directories for distribution.

Usage:
    python tools/pack_assets.py <input_dir> <output.pak> [--level N]

Examples:
    python tools/pack_assets.py assets/ build/game_assets.pak
    python tools/pack_assets.py assets/ build/game_assets.pak --level 6
"""

import argparse
import os
import sys
import zipfile
import time


def pack_assets(input_dir: str, output_path: str, compression_level: int = 6) -> None:
    """Pack a directory into a .pak (zip) archive."""
    if not os.path.isdir(input_dir):
        print(f"Error: '{input_dir}' is not a valid directory.")
        sys.exit(1)

    # Ensure output directory exists
    output_dir = os.path.dirname(output_path)
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)

    # Collect files
    files_to_pack = []
    for root, _dirs, files in os.walk(input_dir):
        for f in files:
            full_path = os.path.join(root, f)
            # Archive name is relative to input_dir
            arc_name = os.path.relpath(full_path, input_dir).replace("\\", "/")
            files_to_pack.append((full_path, arc_name))

    if not files_to_pack:
        print("Warning: No files found to pack.")
        return

    print(f"Packing {len(files_to_pack)} files from '{input_dir}' -> '{output_path}'")
    print(f"Compression level: {compression_level}")

    start_time = time.time()
    total_size = 0
    compressed_size = 0

    with zipfile.ZipFile(output_path, 'w', zipfile.ZIP_DEFLATED, compresslevel=compression_level) as zf:
        for i, (full_path, arc_name) in enumerate(files_to_pack):
            file_size = os.path.getsize(full_path)
            total_size += file_size
            zf.write(full_path, arc_name)

            # Progress every 50 files
            if (i + 1) % 50 == 0 or (i + 1) == len(files_to_pack):
                pct = (i + 1) / len(files_to_pack) * 100
                print(f"  [{pct:5.1f}%] {i + 1}/{len(files_to_pack)} files...")

    compressed_size = os.path.getsize(output_path)
    elapsed = time.time() - start_time
    ratio = (1.0 - compressed_size / total_size) * 100 if total_size > 0 else 0

    print(f"\nDone in {elapsed:.2f}s!")
    print(f"  Original:   {total_size / 1024 / 1024:.2f} MB")
    print(f"  Compressed: {compressed_size / 1024 / 1024:.2f} MB")
    print(f"  Ratio:      {ratio:.1f}% reduction")
    print(f"  Output:     {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description="Starlight Engine - Asset Packer",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="Creates .pak (zip) archives compatible with the Starlight VFS."
    )
    parser.add_argument("input_dir", help="Directory containing assets to pack")
    parser.add_argument("output", help="Output .pak file path")
    parser.add_argument("--level", type=int, default=6,
                        help="Compression level 0-9 (default: 6)")

    args = parser.parse_args()
    pack_assets(args.input_dir, args.output, args.level)


if __name__ == "__main__":
    main()
