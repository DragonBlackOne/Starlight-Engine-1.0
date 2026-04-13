import os
import sys
import unittest
import numpy  # Forces load of NumPy C-API for Rust backend

# Adiciona o diretório pai ao path
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# NOTE: physics_aabb foi removido. Física agora usa Rapier3D via backend Rust.
# Este teste verifica apenas que o backend carrega corretamente.

class TestCore(unittest.TestCase):
    def test_backend_import(self):
        """Testa se o backend Rust carrega corretamente."""
        try:
            from starlight import backend
            self.assertIsNotNone(backend)
        except ImportError as e:
            self.skipTest(f"Backend não compilado: {e}")

    def test_starlight_import(self):
        """Testa se os módulos principais do Starlight carregam."""
        from starlight import App, Entity, Keys, Input
        self.assertIsNotNone(App)
        self.assertIsNotNone(Entity)
        self.assertIsNotNone(Keys)
        self.assertIsNotNone(Input)

    def test_math_module(self):
        """Testa funções matemáticas básicas."""
        from starlight import math as smath
        # look_at deve retornar uma matriz 4x4
        mat = smath.look_at([0, 0, 5], [0, 0, 0], [0, 1, 0])
        import math
        self.assertEqual(len(mat), 4)
        self.assertEqual(len(mat[0]), 4)


if __name__ == "__main__":
    unittest.main()
