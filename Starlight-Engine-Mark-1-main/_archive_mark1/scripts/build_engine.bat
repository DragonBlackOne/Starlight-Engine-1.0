@echo off
setlocal
echo [STARLIGHT] Building Engine...

:: Navigate to project root from scripts/
cd /d "%~dp0.."

:: Activate Venv
if exist "venv\Scripts\python.exe" (
    set PYTHON_EXE=%CD%\venv\Scripts\python.exe
) else (
    echo [ERROR] Virtual Environment not found!
    exit /b 1
)

:: Uninstall existing package to prevent conflicts
%PYTHON_EXE% -m pip uninstall -y starlight-rust
if exist "starlight_rust.pyd" del "starlight_rust.pyd"
if exist "starlight_rust.dll" del "starlight_rust.dll"

:: Install Build Dependencies
%PYTHON_EXE% -m pip install --upgrade pip maturin

:: Set PYO3_PYTHON to ensure correct linkage
set PYO3_PYTHON=%PYTHON_EXE%

:: Clean and Build
cd starlight_rust
cargo clean
cargo build --release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build Failed!
    exit /b 1
)
copy target\release\starlight_rust.dll ..\starlight_rust.pyd /Y
cd ..

echo [SUCCESS] Engine built for Python (%PYTHON_EXE%).
endlocal

echo [SUCCESS] Engine built and installed into venv.
endlocal
