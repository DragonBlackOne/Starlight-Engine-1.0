import subprocess
import sys


def run_command(command, cwd=None, description=""):
    print(f"\n🚀 {description}...")
    print(f"   > {command}")
    try:
        subprocess.check_call(command, shell=True, cwd=cwd)
        print("   ✅ Passed")
        return True
    except subprocess.CalledProcessError:
        print("   ❌ FAILED")
        return False

def main():
    print("========================================")
    print("🛡️  Starlight Engine - Quality Assurance")
    print("========================================")

    passed = True

    # 1. Rust Format
    if not run_command("cargo fmt --check", cwd="starlight_rust", description="Checking Rust Formatting"):
        passed = False

    # 2. Rust Clippy (Strict)
    # Using --all-features and -D warnings to fail on warnings
    if not run_command("cargo clippy --all-features -- -D warnings", cwd="starlight_rust", description="Running Rust Clippy (Strict)"):
        passed = False

    # 3. Python Ruff
    if not run_command("ruff check .", description="Running Ruff Linter"):
        passed = False

    # 4. Python MyPy
    if not run_command("mypy pysrc demos", description="Running MyPy Static Analysis"):
        passed = False

    print("\n========================================")
    if passed:
        print("✅ ALL CHECKS PASSED. SYSTEM IS STABLE.")
        sys.exit(0)
    else:
        print("❌ INTEGRITY FAILURE. FIX ERRORS BEFORE COMMITTING.")
        sys.exit(1)

if __name__ == "__main__":
    main()
