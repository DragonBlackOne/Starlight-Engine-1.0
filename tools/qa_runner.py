#!/usr/bin/env python3
"""
Fusion ENGINE — Automated QA & System Health Runner
Runs the full suite of GoogleTest unit tests and engine validation routines,
records metrics, execution times, and updates system_health_report.md.
"""

import os
import sys
import time
import subprocess
from pathlib import Path

# ANSI Color Codes
GREEN = "\033[92m"
YELLOW = "\033[93m"
RED = "\033[91m"
CYAN = "\033[96m"
BOLD = "\033[1m"
RESET = "\033[0m"

ROOT_DIR = Path(__file__).resolve().parent.parent
TEST_EXE = ROOT_DIR / "build" / "StarlightEngine" / "tests" / "Release" / "StarlightTests.exe"
REPORT_FILE = ROOT_DIR / "system_health_report.md"

def run_qa_suite(filter_pattern: str = None):
    print(f"\n{BOLD}{CYAN}=== Starlight Engine v15.0 Autonomous QA & Health Suite ==={RESET}\n")

    if not TEST_EXE.exists():
        print(f"{RED}Error: Test executable not found at '{TEST_EXE}'.{RESET}")
        print(f"Please build the tests first: {YELLOW}python fusion.py build engine{RESET}")
        return 1

    cmd = [str(TEST_EXE)]
    if filter_pattern:
        cmd.append(f"--gtest_filter={filter_pattern}")

    print(f"Executing: {BOLD}{' '.join(cmd)}{RESET} ...\n")
    start_time = time.perf_counter()

    try:
        proc = subprocess.run(
            cmd,
            cwd=str(ROOT_DIR / "StarlightEngine"),
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace"
        )
    except Exception as e:
        print(f"{RED}Failed to run tests: {e}{RESET}")
        return 1

    elapsed = time.perf_counter() - start_time
    stdout = proc.stdout
    stderr = proc.stderr

    # Parse GTest output
    passed_count = 0
    failed_count = 0
    skipped_count = 0
    suite_count = 0

    for line in stdout.splitlines():
        if "[  PASSED  ]" in line and "tests" in line:
            parts = line.split()
            for p in parts:
                if p.isdigit():
                    passed_count = int(p)
                    break
        elif "[  FAILED  ]" in line and "tests" in line:
            parts = line.split()
            for p in parts:
                if p.isdigit():
                    failed_count = int(p)
                    break
        elif "[  SKIPPED ]" in line and "tests" in line:
            parts = line.split()
            for p in parts:
                if p.isdigit():
                    skipped_count = int(p)
                    break
        elif "test suites ran" in line:
            parts = line.split()
            for i, p in enumerate(parts):
                if p == "from" and i + 1 < len(parts) and parts[i+1].isdigit():
                    suite_count = int(parts[i+1])

    # Print summary
    print(f"{BOLD}QA Execution Results:{RESET}")
    print(f"  Total Duration : {elapsed:.2f} seconds")
    print(f"  Test Suites    : {suite_count}")
    print(f"  Passed Tests   : {GREEN}{passed_count}{RESET}")
    print(f"  Skipped Tests  : {YELLOW}{skipped_count}{RESET}")
    print(f"  Failed Tests   : {RED if failed_count > 0 else GREEN}{failed_count}{RESET}")

    # Generate Markdown Health Report
    status_icon = "🟢 HEALTHY" if failed_count == 0 else "🔴 ISSUES DETECTED"
    report_content = f"""# System Health Report

Generated on: {time.strftime('%Y-%m-%d %H:%M:%S')}
Status: **{status_icon}**

## Test Execution Summary
- **Total Test Suites**: {suite_count}
- **Tests Passed**: {passed_count}
- **Tests Skipped**: {skipped_count}
- **Tests Failed**: {failed_count}
- **Total Execution Time**: {elapsed:.3f} s

## Environment & Build Architecture
- **Compiler**: MSVC (C++20)
- **Target**: Release (x64)
- **Executable**: `{TEST_EXE.name}`
"""
    try:
        REPORT_FILE.write_text(report_content, encoding="utf-8")
        print(f"\n{GREEN}Updated system health report at: {REPORT_FILE.name}{RESET}\n")
    except Exception as e:
        print(f"{YELLOW}Warning: Could not write report file: {e}{RESET}")

    return 0 if failed_count == 0 else 1

if __name__ == "__main__":
    filter_arg = None
    if len(sys.argv) > 1 and sys.argv[1].startswith("--filter="):
        filter_arg = sys.argv[1].split("=", 1)[1]
    elif len(sys.argv) > 2 and sys.argv[1] == "--filter":
        filter_arg = sys.argv[2]
    sys.exit(run_qa_suite(filter_arg))
