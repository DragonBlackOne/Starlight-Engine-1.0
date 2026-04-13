
import sys
import os
try:
    import starlight.backend as backend
    print("Backend loaded from:", backend.__file__)
    print("Attributes:", dir(backend))
    if hasattr(backend, 'generate_heightmap'):
        print("generate_heightmap FOUND")
    else:
        print("generate_heightmap NOT FOUND")
except ImportError as e:
    print("ImportError:", e)
except Exception as e:
    print("Error:", e)
