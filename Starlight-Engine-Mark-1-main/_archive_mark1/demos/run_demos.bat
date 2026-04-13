@echo off
cd /d "%~dp0.."
if exist "venv\Scripts\python.exe" (
    echo [LAUNCHER] Using Venv Python...
    venv\Scripts\python.exe demos/run_forest.py > run_demo_log.txt 2>&1
) else (
    echo [ERROR] Venv not found!
    pause
)
