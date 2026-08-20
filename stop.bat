@echo off
title Stop Web Search Engine
echo ========================================================
echo          Stopping Search Engine Services
echo ========================================================
echo.

echo Stopping search_engine.exe processes...
taskkill /F /IM search_engine.exe 2>nul
if %errorlevel% equ 0 (
    echo [+] Terminated search_engine.exe
) else (
    echo [-] No search_engine.exe processes found.
)

echo.
echo Note: If Vite frontend is still running in its window, simply close that window.
echo Done!
pause
