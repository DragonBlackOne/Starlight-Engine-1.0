import json
import os
from typing import Dict

class LocalizationManager:
    """
    Handles translation keys.
    Skill: localization-i18n
    """
    def __init__(self, lang: str = "en"):
        self.current_lang = lang
        self.strings: Dict[str, Dict[str, str]] = {}
        self.load_strings()

    def load_strings(self):
        path = "assets/strings.json"
        if os.path.exists(path):
            try:
                with open(path, "r", encoding="utf-8") as f:
                    self.strings = json.load(f)
            except Exception as e:
                print(f"[i18n] Error loading strings: {e}")
        else:
            print("[i18n] Warning: assets/strings.json not found.")

    def set_language(self, lang: str):
        if lang in self.strings:
            self.current_lang = lang
            print(f"[i18n] Language set to {lang}")

    def get(self, key: str) -> str:
        lang_data = self.strings.get(self.current_lang, {})
        return lang_data.get(key, key) # Fallback to key

# Global Instance
i18n = LocalizationManager()
