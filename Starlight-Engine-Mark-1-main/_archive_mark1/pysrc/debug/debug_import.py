import sys
import os

try:
    import starlight
    print(f"Starlight imported from: {starlight.__file__}")
    print(f"Dir(starlight): {dir(starlight)}")
    
    try:
        from starlight import framework
        print(f"Framework imported: {framework}")
    except ImportError as e:
        print(f"Failed to import framework: {e}")

    try:
        print(f"Scene available? {starlight.Scene}")
    except AttributeError:
        print("starlight.Scene NOT available")

except ImportError as e:
    print(f"Failed to import starlight: {e}")
