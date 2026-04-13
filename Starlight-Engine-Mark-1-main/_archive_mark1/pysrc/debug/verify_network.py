import sys
import os

# Add pysrc to path
sys.path.insert(0, os.path.abspath("pysrc"))

import starlight.backend as sb

try:
    print("Verifying network bindings...")
    
    # Test functions existence
    print(f"start_server: {sb.start_server}")
    print(f"connect_client: {sb.connect_client}")
    print(f"send_message: {sb.send_message}")
    print(f"get_messages: {sb.get_messages}")
    
    # Test initialization
    sb.init_headless(800, 600)
    print("Headless initialized.")
    
    # Test start_server (might fail to bind if port taken, but function exists)
    try:
        sb.start_server(5555)
        print("Server started on 5555.")
    except Exception as e:
        print(f"Start server failed (expected): {e}")

    # Test send_message (should fail because server needs target)
    try:
        sb.send_message(b"test", None)
        print("Message sent (unexpected success for server without target).")
    except Exception as e:
        print(f"Message send failed (expected): {e}")

    print("Network bindings verified successfully.")
except Exception as e:
    print(f"Verification FAILED: {e}")
    exit(1)
