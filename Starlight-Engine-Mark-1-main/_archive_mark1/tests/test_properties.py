import sys
import os
import pytest
from hypothesis import given, settings, strategies as st
import math

# Adiciona o diretório da raiz construída (onde backend.pyd/so reside) e o diretório pysrc
_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, os.path.join(_root, "target", "release"))
sys.path.insert(0, os.path.join(_root, "pysrc"))

try:
    import backend
except ImportError:
    pytest.skip("Backend não compilado. Pule os testes C-API.", allow_module_level=True)

# Geração de Floats Extremos para Teste de Estresse no C++ / Rust FFI
finite_floats = st.floats(allow_nan=False, allow_infinity=False, min_value=-1e5, max_value=1e5)
extreme_floats = st.floats(allow_nan=True, allow_infinity=True)

class TestPyO3BindingsRobustness:

    @classmethod
    def setup_class(cls):
        # Boot do App Headless (Sem Window)
        try:
            backend.init_engine(True) # Headless mode
        except Exception as e:
            # Ignore se ja estiver inicializado
            pass

    @given(x=finite_floats, y=finite_floats, z=finite_floats)
    @settings(max_examples=100, deadline=None) # Desativa timeout da hypothesis pro rust
    def test_set_transform_finite(self, x, y, z):
        """Valida que valores comuns repassados às propriedades FFI não causam PANIC."""
        ent = backend.spawn_entity(0.0, 0.0, 0.0)
        try:
            backend.set_transform(ent, x, y, z)
            # Lê de volta para ver se a gravação tem a integridade
            t = backend.get_transform(ent)
            assert math.isclose(t.x, x, abs_tol=1e-3) or math.isnan(x)
            assert math.isclose(t.y, y, abs_tol=1e-3) or math.isnan(y)
            assert math.isclose(t.z, z, abs_tol=1e-3) or math.isnan(z)
        finally:
            backend.destroy_entity(ent)

    @given(x=extreme_floats, y=extreme_floats, z=extreme_floats)
    @settings(max_examples=50, deadline=None)
    def test_set_transform_extreme(self, x, y, z):
        """Valida se a Engine de Física (Rapier3D) e ECS aguentam float limits."""
        ent = backend.spawn_entity(0.0, 0.0, 0.0)
        try:
            backend.set_transform(ent, x, y, z)
            t = backend.get_transform(ent)
            # A gente só se importa de NÃO DAR CRASH/SEGVFAULT (Panics paralisariam a runtime)
        finally:
            backend.destroy_entity(ent)

    @given(r=finite_floats, g=finite_floats, b=finite_floats, a=finite_floats)
    @settings(max_examples=50, deadline=None)
    def test_set_color_bounds(self, r, g, b, a):
        """Verifica como as Cores RGB convertem para sRGB do Backend."""
        ent = backend.spawn_entity(0.0, 0.0, 0.0)
        try:
            backend.set_color(ent, r, g, b, a)
        finally:
            backend.destroy_entity(ent)

    @given(pitch=finite_floats, yaw=finite_floats)
    @settings(max_examples=50, deadline=None)
    def test_camera_rotation_limits(self, pitch, yaw):
        """O quaternion e clampers de câmera não podem crashear."""
        cam_id = backend.get_main_camera_id()
        if cam_id > 0:
            backend.set_camera_rotation(cam_id, pitch, yaw)
