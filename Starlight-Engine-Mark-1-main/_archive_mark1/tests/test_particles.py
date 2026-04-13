import os
import sys
import unittest
from unittest.mock import MagicMock, Mock

import numpy as np

# Add src to path if running directly
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../pysrc")))

# Mock moderngl
sys.modules["moderngl"] = MagicMock()

from starlight.engine.nebula import NebulaSystem


class TestNebulaSystem(unittest.TestCase):
    def setUp(self):
        self.ctx = Mock()
        self.assets = MagicMock()
        self.ctx.buffer = Mock()
        self.ctx.program = Mock()
        self.ctx.vertex_array = Mock()
        # Small buffer for easy testing of wrap-around
        self.max_particles = 100
        self.nebula = NebulaSystem(self.ctx, self.assets, max_particles=self.max_particles)

    def test_initialization(self):
        """Test that the system initializes with correct defaults."""
        self.assertEqual(self.nebula.head, 0)
        self.assertEqual(self.nebula.filled_count, 0)
        self.assertEqual(len(self.nebula.particle_data), 100)

    def test_emit_basic(self):
        """Test basic emission without wrapping."""
        self.nebula.emit((10, 10, 10), count=10)

        self.assertEqual(self.nebula.head, 10)
        self.assertEqual(self.nebula.filled_count, 10)

        # Verify position of first particle
        np.testing.assert_array_equal(self.nebula.particle_data[0, 0:3], [10, 10, 10])

    def test_emit_box(self):
        """Test box emission volume."""
        center = (0, 0, 0)
        size = (10, 10, 10)  # range [-5, 5]
        self.nebula.emit_box(center, size, count=50)

        self.assertEqual(self.nebula.filled_count, 50)

        # Check bounds
        positions = self.nebula.particle_data[:50, 0:3]

        # Check X
        self.assertTrue(np.all(positions[:, 0] >= -5.0))
        self.assertTrue(np.all(positions[:, 0] <= 5.0))

        # Check Y
        self.assertTrue(np.all(positions[:, 1] >= -5.0))
        self.assertTrue(np.all(positions[:, 1] <= 5.0))

        # Check Z
        self.assertTrue(np.all(positions[:, 2] >= -5.0))
        self.assertTrue(np.all(positions[:, 2] <= 5.0))

    def test_emit_sphere(self):
        """Test sphere emission volume."""
        center = (0, 0, 0)
        radius = 10.0
        self.nebula.emit_sphere(center, radius, count=50)

        self.assertEqual(self.nebula.filled_count, 50)

        # Check distance from center
        positions = self.nebula.particle_data[:50, 0:3]
        distances = np.linalg.norm(positions, axis=1)

        # All distances should be <= radius (allowing for small float error)
        self.assertTrue(np.all(distances <= radius + 0.001))

    def test_ring_buffer_wrap(self):
        """Test that new particles overwrite old ones correctly (Ring Buffer)."""
        # 1. Fill buffer (0 to 99)
        # Emit with distinct color to track (Red)
        self.nebula.emit((0, 0, 0), count=100, color=(1, 0, 0, 1))

        self.assertEqual(self.nebula.filled_count, 100)
        self.assertEqual(self.nebula.head, 0)  # Wrapped back to 0

        # Check first particle is Red
        np.testing.assert_array_equal(self.nebula.particle_data[0, 4:8], [1, 0, 0, 1])

        # 2. Emit 10 new particles (Blue)
        # Should overwrite indices 0-9
        self.nebula.emit((0, 0, 0), count=10, color=(0, 0, 1, 1))

        self.assertEqual(self.nebula.filled_count, 100)
        self.assertEqual(self.nebula.head, 10)

        # Check index 0 is Blue (New)
        np.testing.assert_array_equal(self.nebula.particle_data[0, 4:8], [0, 0, 1, 1])
        # Check index 9 is Blue (New)
        np.testing.assert_array_equal(self.nebula.particle_data[9, 4:8], [0, 0, 1, 1])
        # Check index 10 is still Red (Old)
        np.testing.assert_array_equal(self.nebula.particle_data[10, 4:8], [1, 0, 0, 1])

    def test_update_lifecycle(self):
        """Test that update reduces life and affects alpha."""
        self.nebula.emit((0, 0, 0), count=10, life=1.0, life_variance=0.0, size_variance=0.0)

        # Initial life check (exact 1.0)
        np.testing.assert_allclose(self.nebula.life[:10], 1.0, rtol=1e-5)

        # Update
        self.nebula.update(0.5)

        # Life should decrease
        np.testing.assert_allclose(self.nebula.life[:10], 0.5, rtol=1e-5)

        self.nebula.update(0.1)
        np.testing.assert_allclose(self.nebula.life[:10], 0.4, rtol=1e-5)

        # Check alpha update (should be 0.4 / 1.0 = 0.4)
        np.testing.assert_allclose(self.nebula.particle_data[:10, 7], 0.4, rtol=1e-5)


if __name__ == "__main__":
    unittest.main()
