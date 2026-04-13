path = "d:/Projetos/Starlight/Starlight-Engine-Mark-1/test_py.log"
print(f"Writing to {path}")
try:
    with open(path, "w") as f:
        f.write("Hello from Python")
    print("Success")
except Exception as e:
    print(f"Failed: {e}")
