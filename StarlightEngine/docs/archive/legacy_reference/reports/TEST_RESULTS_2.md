============================= test session starts =============================
platform win32 -- Python 3.13.9, pytest-9.0.1, pluggy-1.6.0 -- C:\Users\alyss\AppData\Local\Programs\Python\Python313\python.exe
cachedir: .pytest_cache
rootdir: D:\Projetos\Starlight-Engine alternative generation
configfile: pyproject.toml
plugins: cov-7.0.0, mock-3.15.1, snapshot-0.9.0, xdist-3.8.0
collecting ... collected 2 items

tests/test_ricochet_v2.py::test_win_condition FAILED                     [ 50%]
tests/test_ricochet_v2.py::test_collision_logic FAILED                   [100%]

================================== FAILURES ===================================
_____________________________ test_win_condition ______________________________
tests\test_ricochet_v2.py:102: in test_win_condition
    game.update(0.16)
src\starlight\games\ricochet_v2\game.py:531: in update
    self.lbl_message.visible = False
    ^^^^^^^^^^^^^^^^
E   AttributeError: 'RicochetForTest' object has no attribute 'lbl_message'
____________________________ test_collision_logic _____________________________
tests\test_ricochet_v2.py:119: in test_collision_logic
    game.update(0.16)
src\starlight\games\ricochet_v2\game.py:531: in update
    self.lbl_message.visible = False
    ^^^^^^^^^^^^^^^^
E   AttributeError: 'RicochetForTest' object has no attribute 'lbl_message'
============================== warnings summary ===============================
tests\test_ricochet_v2.py:4
  D:\Projetos\Starlight-Engine alternative generation\tests\test_ricochet_v2.py:4: PendingDeprecationWarning: Importing PyGLM via "import glm" is going to be deprecated in the future.
  Use "from pyglm import glm" instead
    import glm

C:\Users\alyss\AppData\Local\Programs\Python\Python313\Lib\site-packages\pygame\pkgdata.py:25
  C:\Users\alyss\AppData\Local\Programs\Python\Python313\Lib\site-packages\pygame\pkgdata.py:25: UserWarning: pkg_resources is deprecated as an API. See https://setuptools.pypa.io/en/latest/pkg_resources.html. The pkg_resources package is slated for removal as early as 2025-11-30. Refrain from using this package or pin to Setuptools<81.
    from pkg_resources import resource_stream, resource_exists

C:\Users\alyss\AppData\Local\Programs\Python\Python313\Lib\site-packages\pkg_resources\__init__.py:3146
  C:\Users\alyss\AppData\Local\Programs\Python\Python313\Lib\site-packages\pkg_resources\__init__.py:3146: DeprecationWarning: Deprecated call to `pkg_resources.declare_namespace('sphinxcontrib')`.
  Implementing implicit namespace packages (as specified in PEP 420) is preferred to `pkg_resources.declare_namespace`. See https://setuptools.pypa.io/en/latest/references/keywords.html#keyword-namespace-packages
    declare_namespace(pkg)

-- Docs: https://docs.pytest.org/en/stable/how-to/capture-warnings.html
=========================== short test summary info ===========================
FAILED tests/test_ricochet_v2.py::test_win_condition - AttributeError: 'Ricoc...
FAILED tests/test_ricochet_v2.py::test_collision_logic - AttributeError: 'Ric...
======================== 2 failed, 3 warnings in 2.21s ========================
