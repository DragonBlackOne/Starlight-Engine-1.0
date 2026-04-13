@echo off
echo Starting Ricochet V2 Ultimate...
set PYTHONPATH=%PYTHONPATH%;%~dp0..\src
python "%~dp0..\src\starlight\games\ricochet_v2\main.py"
if %errorlevel% neq 0 (
    echo.
    echo Game crashed or closed with error!
    pause
)
