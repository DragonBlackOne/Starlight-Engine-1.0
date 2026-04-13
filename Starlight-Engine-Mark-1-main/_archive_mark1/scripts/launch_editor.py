"""
Launch Starlight Studio with a sample game.
"""

import os
import sys

# Add project src to path
current_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.abspath(os.path.join(current_dir, ".."))
src_path = os.path.join(project_root, "src")
if src_path not in sys.path:
    sys.path.insert(0, src_path)

from starlight.editor.studio import run_studio
from starlight.games.nexus_demo import GenesisNexus

if __name__ == "__main__":
    run_studio(GenesisNexus)
