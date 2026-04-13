import argparse
import subprocess
import sys


def run_command(command, cwd=None, ignore_errors=False):
    print(f"Running: {' '.join(command)}")
    try:
        subprocess.run(command, cwd=cwd, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        if not ignore_errors:
            print(f"Error executing command: {e}")
            sys.exit(1)
        else:
            print(f"Command failed (ignored): {e}")


def check():
    print("=== Checking Rust (Clippy) ===")
    run_command(
        ["cargo", "clippy", "--all-targets", "--all-features", "--", "-D", "warnings"],
        cwd="starlight_rust",
    )

    print("\n=== Checking Rust (Formatting) ===")
    run_command(["cargo", "fmt", "--", "--check"], cwd="starlight_rust")

    print("\n=== Checking Python (Ruff) ===")
    # Check if ruff is installed, if not, try to run it via python -m ruff or warn
    try:
        run_command(["ruff", "check", "."], ignore_errors=True)
    except FileNotFoundError:
        print("Ruff not found. Skipping validation.")


def fix():
    print("=== Fixing Rust (Clippy) ===")
    # Allow dirty to fix uncommited changes
    run_command(
        ["cargo", "clippy", "--fix", "--allow-dirty", "--allow-staged"],
        cwd="starlight_rust",
        ignore_errors=True,
    )

    print("\n=== Fixing Rust (Formatting) ===")
    run_command(["cargo", "fmt"], cwd="starlight_rust")

    print("\n=== Fixing Python (Ruff) ===")
    try:
        run_command(["ruff", "check", "--fix", "."], ignore_errors=True)
        run_command(["ruff", "format", "."], ignore_errors=True)
    except FileNotFoundError:
        print("Ruff not found. Skipping fix.")


def test():
    print("=== Testing Rust ===")
    run_command(["cargo", "test"], cwd="starlight_rust")

    print("\n=== Testing Python ===")
    run_command(["pytest"], ignore_errors=True)


def main():
    parser = argparse.ArgumentParser(description="Starlight Dev Tools")
    parser.add_argument("action", choices=["check", "fix", "test"], help="Action to perform")

    if len(sys.argv) < 2:
        parser.print_help()
        sys.exit(1)

    args = parser.parse_args()

    if args.action == "check":
        check()
    elif args.action == "fix":
        fix()
    elif args.action == "test":
        test()


if __name__ == "__main__":
    main()
