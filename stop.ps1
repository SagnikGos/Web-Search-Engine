# PowerShell script to stop Search Engine processes
Write-Host "Stopping search_engine.exe processes..." -ForegroundColor Yellow
$proc = Get-Process -Name "search_engine" -ErrorAction SilentlyContinue
if ($proc) {
    Stop-Process -Name "search_engine" -Force
    Write-Host "[+] search_engine.exe terminated." -ForegroundColor Green
} else {
    Write-Host "[-] No search_engine.exe process found." -ForegroundColor Gray
}

Write-Host "Done!" -ForegroundColor Green
