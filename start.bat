@echo off
setlocal enabledelayedexpansion

title Web Search Engine Launcher
echo ========================================================
echo          Starting Web Search Engine Stack
echo ========================================================
echo.

cd /d "%~dp0"

:: 1. Locate search_engine executable
set "BACKEND_EXE=build\search_engine.exe"
if not exist "!BACKEND_EXE!" (
    if exist "build\Release\search_engine.exe" (
        set "BACKEND_EXE=build\Release\search_engine.exe"
    ) else if exist "build\Debug\search_engine.exe" (
        set "BACKEND_EXE=build\Debug\search_engine.exe"
    )
)

:: If backend executable not found, attempt to build
if not exist "!BACKEND_EXE!" (
    echo [!] Backend executable not found. Building project now...
    if not exist "build" mkdir build
    cmake -B build
    if errorlevel 1 (
        echo [ERROR] CMake configuration failed. Please install CMake and a C++ compiler.
        pause
        exit /b 1
    )
    cmake --build build --config Release
    if errorlevel 1 (
        echo [ERROR] Build failed.
        pause
        exit /b 1
    )
    if exist "build\search_engine.exe" (
        set "BACKEND_EXE=build\search_engine.exe"
    ) else if exist "build\Release\search_engine.exe" (
        set "BACKEND_EXE=build\Release\search_engine.exe"
    )
)

:: 2. Check if index files exist; if not, build index
if not exist "data\index\index.json" (
    echo [!] Search index not found. Building initial index from crawled data...
    "!BACKEND_EXE!" index
    if errorlevel 1 (
        echo [WARNING] Index build returned an error. Starting server anyway...
    )
    echo.
)

:: 3. Check frontend dependencies
if not exist "frontend\node_modules" (
    echo [!] Frontend dependencies not found. Running npm install...
    pushd frontend
    call npm install
    popd
    echo.
)

:: 4. Start Backend Server in a new window
echo [+] Starting Backend Server on http://localhost:8080 ...
start "Search Engine - Backend (Port 8080)" cmd /k "cd /d "%~dp0" && "!BACKEND_EXE!" server --port 8080"

:: 5. Start Frontend Dev Server in a new window
echo [+] Starting Frontend Dev Server on http://localhost:5173 ...
start "Search Engine - Frontend (Vite)" cmd /k "cd /d "%~dp0\frontend" && npm run dev"

:: 6. Wait a moment and launch browser
echo.
echo All services launched!
echo - Backend API:  http://localhost:8080
echo - Frontend UI:  http://localhost:5173
echo.
echo Opening frontend in default browser...
timeout /t 2 /nobreak >nul
start http://localhost:5173

echo ========================================================
echo Servers are running in their respective command windows.
echo Close those windows or run 'stop.bat' to stop servers.
echo ========================================================
timeout /t 5
