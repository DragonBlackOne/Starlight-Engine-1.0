"""Editor Asset Browser — VFS file listing with type filtering."""
from __future__ import annotations
from typing import Any
import os

ASSET_EXTENSIONS = {
    "mesh": {".glb", ".gltf", ".obj", ".fbx"},
    "texture": {".png", ".jpg", ".jpeg", ".dds", ".bmp", ".tga"},
    "audio": {".wav", ".ogg", ".mp3", ".flac"},
    "scene": {".json", ".scene"},
    "shader": {".wgsl", ".glsl", ".hlsl"},
}


class AssetBrowser:
    """Asset browser with type-based filtering."""

    def __init__(self) -> None:
        self.files: list[str] = []
        self.filter_text: str = ""
        self.filter_type: str = "all"

    def set_files(self, files: list[str]) -> None:
        self.files = sorted(files)

    def get_filtered(self) -> list[str]:
        result = self.files
        if self.filter_text:
            result = [f for f in result if self.filter_text.lower() in f.lower()]
        if self.filter_type != "all":
            exts = ASSET_EXTENSIONS.get(self.filter_type, set())
            result = [f for f in result if any(f.lower().endswith(e) for e in exts)]
        return result

    @staticmethod
    def get_icon(filepath: str) -> str:
        ext = os.path.splitext(filepath)[1].lower()
        for category, exts in ASSET_EXTENSIONS.items():
            if ext in exts:
                icons = {"mesh": "📦", "texture": "🖼️", "audio": "🎵",
                         "scene": "🎬", "shader": "⚡"}
                return icons.get(category, "📄")
        return "📄"
