import sys
import os
sys.path.insert(0, os.path.abspath("pysrc"))
import starlight
from starlight import backend

print(f"Backend content: {dir(backend)}")
