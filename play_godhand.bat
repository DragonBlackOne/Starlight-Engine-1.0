@echo off
title God Hand 3D - Fusion ENGINE
xcopy "%~dp0GodHand_Project\assets\*" "%~dp0build\GodHand_Project\Release\assets\" /Y /E /Q >nul 2>&1
cd /d "%~dp0build\GodHand_Project\Release"
start "" "%~dp0build\GodHand_Project\Release\GodHand_Project.exe"
