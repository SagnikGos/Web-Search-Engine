# PowerShell launcher for Search Engine Backend & Frontend
param (
    [int]$BackendPort = 8080,
    [switch]$NoBrowser,
    [switch]$Build
)

$rootDir = $PSScriptRoot
Set-Location $rootDir

Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "         Web Search Engine Launcher (PowerShell)        " -ForegroundColor Cyan
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host ""

# 1. Locate / Build Backend Executable
$backendExe = Join-Path $rootDir "build\search_engine.exe"
if (-not (Test-Path $backendExe)) {
    if (Test-Path (Join-Path $rootDir "build\Release\search_engine.exe")) {
        $backendExe = Join-Path $rootDir "build\Release\search_engine.exe"
    } elseif (Test-Path (Join-Path $rootDir "build\Debug\search_engine.exe")) {
        $backendExe = Join-Path $rootDir "build\Debug\search_engine.exe"
    }
}

if ($Build -or (-not (Test-Path $backendExe))) {
    Write-Host "[*] Building backend project..." -ForegroundColor Yellow
    if (-not (Test-Path (Join-Path $rootDir "build"))) {
        New-Item -ItemType Directory -Path (Join-Path $rootDir "build") | Out-Null
    }
    cmake -B build
    if ($LASTEXITCODE -ne 0) {
        Write-Error "CMake configuration failed."
        exit 1
    }
    cmake --build build --config Release
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed."
        exit 1
    }
    if (Test-Path (Join-Path $rootDir "build\search_engine.exe")) {
        $backendExe = Join-Path $rootDir "build\search_engine.exe"
    } elseif (Test-Path (Join-Path $rootDir "build\Release\search_engine.exe")) {
        $backendExe = Join-Path $rootDir "build\Release\search_engine.exe"
    }
}

# 2. Check Search Index
$indexPath = Join-Path $rootDir "data\index\index.json"
if (-not (Test-Path $indexPath)) {
    Write-Host "[*] Index not found. Building search index from crawled pages..." -ForegroundColor Yellow
    & $backendExe index
    Write-Host ""
}

# 3. Check Frontend Dependencies
$frontendDir = Join-Path $rootDir "frontend"
$nodeModules = Join-Path $frontendDir "node_modules"
if (-not (Test-Path $nodeModules)) {
    Write-Host "[*] Installing frontend npm dependencies..." -ForegroundColor Yellow
    Push-Location $frontendDir
    npm install
    Pop-Location
    Write-Host ""
}

# 4. Launch Backend in a new process
Write-Host "[+] Launching Backend Server on port $BackendPort..." -ForegroundColor Green
$backendProcess = Start-Process -FilePath "cmd.exe" `
    -ArgumentList "/k", "title Search Engine - Backend (Port $BackendPort) && cd /d `"$rootDir`" && `"$backendExe`" server --port $BackendPort" `
    -PassThru

# 5. Launch Frontend in a new process
Write-Host "[+] Launching Frontend Dev Server (Vite)..." -ForegroundColor Green
$frontendProcess = Start-Process -FilePath "cmd.exe" `
    -ArgumentList "/k", "title Search Engine - Frontend (Vite) && cd /d `"$frontendDir`" && npm run dev" `
    -PassThru

Write-Host ""
Write-Host "Services are up and running!" -ForegroundColor Green
Write-Host "  - Backend API:  http://localhost:$BackendPort" -ForegroundColor White
Write-Host "  - Frontend UI:  http://localhost:5173" -ForegroundColor White
Write-Host ""

if (-not $NoBrowser) {
    Start-Sleep -Seconds 2
    Start-Process "http://localhost:5173"
}

Write-Host "To stop the servers, close their terminal windows or run .\stop.ps1" -ForegroundColor Gray
