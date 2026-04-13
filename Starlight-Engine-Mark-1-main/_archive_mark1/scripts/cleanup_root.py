import os
import shutil
import glob

def cleanup():
    root = os.getcwd()
    archive_dir = os.path.join(root, "logs", "archive")
    os.makedirs(archive_dir, exist_ok=True)
    
    # Patterns to delete (Temp/Junk)
    delete_patterns = [
        "demo_error_*.log",
        "tropical_fix_*.txt",
        "debug_log.txt",
        "log_final.txt",
        "tb.txt",
        "dir.txt",
        "debug2.txt",
        "check_braces.py",
        "analyze_image.py",
        "fix_state.py",
        "erros.md",
        "erros.txt",
        "checklist.md",
        "build_errors.log",
        "test_out.log",
        "test_output.txt",
        "run_demo_log.txt",
        "run_log.txt",
        "maturin_log.txt",
        "maturin_verify.log",
        "game_run.log",
        "starlight_engine.log",
        "crash.log",
        "crash_bind.log",
        "simulation_output.txt",
        "analysis_log.txt",
        "analysis_result_*.txt",
        "tropical_debug_log.txt",
        "tropical_log*.txt",
        "tropical_trace.txt",
        "safe_log.txt",
        "save_test_log*.txt",
        "error_log.txt",
        "cinematic_log.txt",
        "build_log.txt",
        "test_manual_build.bat",
        "mypy_log.txt",
        "debug_full.py",
        "debug_modules.py"
    ]
    
    # Patterns to move to archive (Logs that might be useful historically)
    archive_patterns = [
        "*.log", # Any other loose log
    ]
    
    # Files to KEEP (Whitelist implicit by distinct patterns)
    
    print(f"Cleaning root: {root}")
    
    for pattern in delete_patterns:
        for f in glob.glob(pattern):
            try:
                os.remove(f)
                print(f"Deleted: {f}")
            except Exception as e:
                print(f"Error deleting {f}: {e}")
                
    print("Cleanup complete.")

if __name__ == "__main__":
    cleanup()
