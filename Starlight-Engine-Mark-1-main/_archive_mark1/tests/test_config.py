import os
import json
import unittest
import tempfile
import tomllib
import sys
from pathlib import Path

# Adiciona o diretório pysrc/ ao sys.path para forçar carregamento local
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../pysrc')))

def load_game_config_class():
    import importlib.util
    current_dir = Path(__file__).parent.parent
    config_path = current_dir / "pysrc/starlight/config.py"

    spec = importlib.util.spec_from_file_location("starlight_config_standalone", config_path)
    module = importlib.util.module_from_spec(spec)
    sys.modules["starlight_config_standalone"] = module
    spec.loader.exec_module(module)
    return module.GameConfig

GameConfig = load_game_config_class()

class TestGameConfig(unittest.TestCase):
    def setUp(self):
        self.config = GameConfig()

    def test_default_values(self):
        """Test that default values match the requirements."""
        self.assertEqual(self.config.title, "Starlight Game")
        self.assertEqual(self.config.width, 1280)
        self.assertEqual(self.config.height, 720)
        self.assertTrue(self.config.vsync)
        self.assertEqual(self.config.shadow_resolution, 2048)
        self.assertEqual(self.config.fog_density, 0.01)
        self.assertEqual(self.config.sun_intensity, 1.0)

    def test_load_json(self):
        """Test loading configuration from a JSON file."""
        data = {
            "title": "JSON Test Game",
            "width": 1920,
            "height": 1080,
            "shadow_resolution": 4096,
            "custom_setting": "value"
        }

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as tmp:
            json.dump(data, tmp)
            tmp_path = tmp.name

        try:
            self.config.load_from_file(tmp_path)

            self.assertEqual(self.config.title, "JSON Test Game")
            self.assertEqual(self.config.width, 1920)
            self.assertEqual(self.config.height, 1080)
            self.assertEqual(self.config.shadow_resolution, 4096)
            # Check custom setting in settings dict
            self.assertEqual(self.config.settings["custom_setting"], "value")
            # Check untouched defaults remain
            self.assertEqual(self.config.fog_density, 0.01)

        finally:
            os.remove(tmp_path)

    def test_load_toml(self):
        """Test loading configuration from a TOML file."""
        toml_content = """
        title = "TOML Test Game"
        width = 800
        height = 600
        fog_density = 0.05
        extra_param = 123
        """

        with tempfile.NamedTemporaryFile(mode='w', suffix='.toml', delete=False) as tmp:
            tmp.write(toml_content)
            tmp_path = tmp.name

        try:
            self.config.load_from_file(tmp_path)

            self.assertEqual(self.config.title, "TOML Test Game")
            self.assertEqual(self.config.width, 800)
            self.assertEqual(self.config.height, 600)
            self.assertEqual(self.config.fog_density, 0.05)
            self.assertEqual(self.config.settings["extra_param"], 123)

        finally:
            os.remove(tmp_path)

    def test_invalid_file(self):
        """Test handling of non-existent files."""
        with self.assertRaises(FileNotFoundError):
            self.config.load_from_file("non_existent_config.json")

    def test_invalid_format(self):
        """Test handling of unsupported file extensions."""
        with tempfile.NamedTemporaryFile(suffix='.txt', delete=False) as tmp:
            tmp_path = tmp.name

        try:
            with self.assertRaises(ValueError):
                self.config.load_from_file(tmp_path)
        finally:
            os.remove(tmp_path)

if __name__ == '__main__':
    unittest.main()
