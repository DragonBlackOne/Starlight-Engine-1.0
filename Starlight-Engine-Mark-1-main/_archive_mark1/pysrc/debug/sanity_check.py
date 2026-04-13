
print("Importing starlight...")
try:
    import starlight
    print("starlight imported.")
except Exception as e:
    print(f"Failed starlight: {e}")

print("Importing audio...")
try:
    import starlight.audio
    print("audio imported.")
except Exception as e:
    print(f"Failed audio: {e}")

print("Importing backend...")
try:
    import starlight.backend
    print("backend imported.")
except Exception as e:
    print(f"Failed backend: {e}")
