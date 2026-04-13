
import sys
import os
sys.path.append(os.path.join(os.getcwd(), "pysrc"))
try:
    from starlight import Input
    print("Success importing Input")
    from starlight import App
    print("Success importing App")
except ImportError as e:
    print(f"ImportError: {e}")
except Exception as e:
    print(f"Error: {e}")
