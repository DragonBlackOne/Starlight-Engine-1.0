import sys
import os

# Add pysrc to path
sys.path.append(os.path.abspath("pysrc"))

try:
    import starlight.backend as backend
    print("Backend loaded successfully.")
    print("Attributes:", dir(backend))
    if hasattr(backend, "capture_screenshot"):
        print("capture_screenshot EXISTS.")
    else:
        print("capture_screenshot MISSING.")
except ImportError as e:
    print(f"ImportError: {e}")
except Exception as e:
    print(f"Error: {e}")
