# PowerShell Build Script for Web Search Engine (Backend + Frontend)
param (
    [switch]$BackendOnly,
    [switch]$FrontendOnly,
    [switch]$Clean,
    [switch]$RebuildIndex
)

$rootDir = $PSScriptRoot
Set-Location $rootDir

Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "        Web Search Engine Build Script (PowerShell)      " -ForegroundColor Cyan
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host ""

# Terminate running search_engine.exe to avoid file lock issues during linking
$runningProc = Get-Process -Name "search_engine" -ErrorAction SilentlyContinue
if ($runningProc) {
    Write-Host "[*] Stopping running search_engine.exe process for build..." -ForegroundColor Yellow
    Stop-Process -Name "search_engine" -Force -ErrorAction SilentlyContinue
}

# 1. Clean Build if requested
if ($Clean) {
    Write-Host "[*] Cleaning build artifacts..." -ForegroundColor Yellow
    if (Test-Path (Join-Path $rootDir "build")) {
        Remove-Item -Recurse -Force (Join-Path $rootDir "build")
    }
    if (Test-Path (Join-Path $rootDir "frontend\dist")) {
        Remove-Item -Recurse -Force (Join-Path $rootDir "frontend\dist")
    }
}

# 2. Build Backend (C++)
if (-not $FrontendOnly) {
    Write-Host "--------------------------------------------------------" -ForegroundColor DarkGray
    Write-Host "[1/2] Building C++ Backend..." -ForegroundColor Cyan
    Write-Host "--------------------------------------------------------" -ForegroundColor DarkGray

    $buildDir = Join-Path $rootDir "build"
    if (-not (Test-Path $buildDir)) {
        New-Item -ItemType Directory -Path $buildDir | Out-Null
    }

    if (-not (Test-Path (Join-Path $buildDir "CMakeCache.txt"))) {
        Write-Host "[*] Configuring CMake in 'build'..." -ForegroundColor Yellow
        cmake -B build
        if ($LASTEXITCODE -ne 0) {
            Write-Error "CMake configuration failed."
            exit 1
        }
    }

    Write-Host "[*] Compiling backend targets..." -ForegroundColor Yellow
    cmake --build build --config Release
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Backend compilation failed."
        exit 1
    }
    Write-Host "[+] Backend build successful!" -ForegroundColor Green
    Write-Host ""
}

# Locate backend executable
$backendExe = Join-Path $rootDir "build\search_engine.exe"
if (-not (Test-Path $backendExe)) {
    if (Test-Path (Join-Path $rootDir "build\Release\search_engine.exe")) {
        $backendExe = Join-Path $rootDir "build\Release\search_engine.exe"
    } elseif (Test-Path (Join-Path $rootDir "build\Debug\search_engine.exe")) {
        $backendExe = Join-Path $rootDir "build\Debug\search_engine.exe"
    }
}

# 3. Build Frontend (React / Vite)
if (-not $BackendOnly) {
    Write-Host "--------------------------------------------------------" -ForegroundColor DarkGray
    Write-Host "[2/2] Building React Frontend..." -ForegroundColor Cyan
    Write-Host "--------------------------------------------------------" -ForegroundColor DarkGray

    $frontendDir = Join-Path $rootDir "frontend"
    Push-Location $frontendDir

    $nodeModules = Join-Path $frontendDir "node_modules"
    if (-not (Test-Path $nodeModules)) {
        Write-Host "[*] Installing frontend dependencies (npm install)..." -ForegroundColor Yellow
        npm install
        if ($LASTEXITCODE -ne 0) {
            Pop-Location
            Write-Error "npm install failed."
            exit 1
        }
    }

    Write-Host "[*] Building frontend production bundle (npm run build)..." -ForegroundColor Yellow
    npm run build
    if ($LASTEXITCODE -ne 0) {
        Pop-Location
        Write-Error "Frontend build failed."
        exit 1
    }
    Pop-Location
    Write-Host "[+] Frontend build successful!" -ForegroundColor Green
    Write-Host ""
}

# 4. Search Index verification / rebuild
$indexPath = Join-Path $rootDir "data\index\index.json"
if ($RebuildIndex -or (-not (Test-Path $indexPath))) {
    if (Test-Path $backendExe) {
        Write-Host "[*] Building / Updating search index from crawled pages..." -ForegroundColor Yellow
        & $backendExe index
        Write-Host "[+] Search index updated!" -ForegroundColor Green
        Write-Host ""
    }
}

Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "                   BUILD COMPLETE!                      " -ForegroundColor Green
Write-Host "========================================================" -ForegroundColor Cyan
if (Test-Path $backendExe) {
    Write-Host "  - Backend:  $backendExe" -ForegroundColor White
}
if (Test-Path (Join-Path $rootDir "frontend\dist")) {
    Write-Host "  - Frontend: $(Join-Path $rootDir 'frontend\dist')" -ForegroundColor White
}
if (Test-Path $indexPath) {
    Write-Host "  - Index:    $indexPath" -ForegroundColor White
}
Write-Host ""
Write-Host "Run .\start.ps1 (or start.bat) to launch the services." -ForegroundColor Gray
