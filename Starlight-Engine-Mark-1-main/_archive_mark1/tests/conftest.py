import pytest
from hypothesis import settings, Phase

# Profile local de Hypothesis: Testes rápidos para Iteração de Dev
settings.register_profile("dev", max_examples=10, deadline=None)

# Profile CI de Hypothesis: Testes demorados, para bater forte no Rust C-API antes de um Release
settings.register_profile("ci", max_examples=500, deadline=None)

# Apply default profile
settings.load_profile("dev")

def pytest_addoption(parser):
    parser.addoption(
        "--run-slow", action="store_true", default=False, help="run slow tests"
    )

def pytest_configure(config):
    config.addinivalue_line("markers", "slow: mark test as slow to run")
    # Se rodando via Github Actions, carregue o perfil CI
    import os
    if os.getenv('GITHUB_ACTIONS') == 'true':
        settings.load_profile("ci")

def pytest_collection_modifyitems(config, items):
    if config.getoption("--run-slow"):
        # --run-slow dado na CLI: nao pule os lentos
        return
    skip_slow = pytest.mark.skip(reason="need --run-slow option to run")
    for item in items:
        if "slow" in item.keywords:
            item.add_marker(skip_slow)
