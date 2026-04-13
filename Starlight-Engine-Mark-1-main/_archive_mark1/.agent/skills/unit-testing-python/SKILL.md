---
name: unit-testing-python
description: Testing Python logic.
---

# Unit Testing

## 1. Framework
Use Python's built-in `unittest` module.

## 2. Location
Tests go in `tests/` directory.

## 3. Template
```python
import sys
import unittest
sys.path.insert(0, "pysrc")

class TestMyFeature(unittest.TestCase):
    def test_transform_defaults(self):
        """Verify default transform values."""
        from`starlight.framework` import Transform
        t = Transform()
        self.assertEqual(t.position, [0, 0, 0])
        self.assertEqual(t.scale, [1, 1, 1])

    def test_entity_creation(self):
        """Entity should store name and position."""
        from`starlight.framework` import Entity
        e = Entity("TestCube", 1.0, 2.0, 3.0)
        self.assertEqual(e.name, "TestCube")

if __name__ == "__main__":
    unittest.main()
```

## 4. Running Tests
```powershell
python -m pytest tests/ -v
# or
python -m unittest discover tests/
```

## 5. Mocking the Backend
For testing logic without the full engine running:
```python
from unittest.mock import MagicMock
import sys

# Mock the backend module before importing starlight
mock_backend = MagicMock()
sys.modules['starlight.backend'] = mock_backend

# Now import and test
from`starlight.framework` import Scene
scene = Scene()
```


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
