@echo off
setlocal enabledelayedexpansion

title Web Search Engine - Build
echo ========================================================
echo         Building Web Search Engine (Backend + Frontend)
echo ========================================================
echo.

cd /d "%~dp0"

:: 1. Terminate running search_engine.exe to prevent file lock during linking
tasklist /FI "IMAGENAME eq search_engine.exe" 2>NUL | find /I /N "search_engine.exe">NUL
if "%ERRORLEVEL%"=="0" (
    echo [*] Detected running search_engine.exe process. Terminating for build...
    taskkill /F /IM search_engine.exe >nul 2>&1
)

:: 2. Build Backend (C++)
echo.
echo --------------------------------------------------------
echo [1/3] Building C++ Backend...
echo --------------------------------------------------------
if not exist "build" (
    echo [*] Configuring CMake in 'build' directory...
    cmake -B build
    if errorlevel 1 (
        echo [ERROR] CMake configuration failed.
        pause
        exit /b 1
    )
)

cmake --build build --config Release
if errorlevel 1 (
    echo [ERROR] Backend build failed.
    pause
    exit /b 1
)
echo [+] Backend build successful!

:: Locate backend binary
set "BACKEND_EXE=build\search_engine.exe"
if not exist "!BACKEND_EXE!" (
    if exist "build\Release\search_engine.exe" (
        set "BACKEND_EXE=build\Release\search_engine.exe"
    ) else if exist "build\Debug\search_engine.exe" (
        set "BACKEND_EXE=build\Debug\search_engine.exe"
    )
)

:: 3. Build Frontend (React / Vite)
echo.
echo --------------------------------------------------------
echo [2/3] Building React Frontend...
echo --------------------------------------------------------
pushd frontend

if not exist "node_modules" (
    echo [*] Installing frontend dependencies (npm install)...
    call npm install
    if errorlevel 1 (
        echo [ERROR] npm install failed.
        popd
        pause
        exit /b 1
    )
)

echo [*] Building production bundle (npm run build)...
call npm run build
if errorlevel 1 (
    echo [ERROR] Frontend build failed.
    popd
    pause
    exit /b 1
)
popd
echo [+] Frontend build successful!

:: 4. Verify / Build Search Index
echo.
echo --------------------------------------------------------
echo [3/3] Checking Search Index...
echo --------------------------------------------------------
if not exist "data\index\index.json" (
    echo [*] Search index not found. Generating initial index...
    "!BACKEND_EXE!" index
    if errorlevel 1 (
        echo [WARNING] Initial index generation failed. You can run index manually later.
    ) else (
        echo [+] Search index built successfully!
    )
) else (
    echo [+] Search index is already present in data\index\
)

echo.
echo ========================================================
echo                   BUILD COMPLETE!
echo ========================================================
echo - Backend executable: !BACKEND_EXE!
echo - Frontend bundle:     frontend\dist\
echo - Search index:        data\index\
echo.
echo You can start the full stack by running 'start.bat'.
echo ========================================================
echo.
pause
