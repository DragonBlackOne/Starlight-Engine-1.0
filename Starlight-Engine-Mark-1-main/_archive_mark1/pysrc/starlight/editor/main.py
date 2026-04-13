"""
Starlight Studio — Entry Point

Usage:
    python pysrc/starlight/editor/main.py
"""
import os
import sys

# Ensure project root is in path
_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
if _root not in sys.path:
    sys.path.insert(0, _root)

from pysrc.starlight.editor.studio import StarlightStudio


def main():
    print("Starting Starlight Studio...")
    studio = StarlightStudio()
    studio.run()


if __name__ == "__main__":
    main()
