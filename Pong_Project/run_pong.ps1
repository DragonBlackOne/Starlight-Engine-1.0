# Launcher para o Fusion Pong
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "  FUSION PONG - CYBERPUNK NEON TURBO ARCADE" -ForegroundColor Yellow
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "Iniciando Fusion Pong em janela nativa..." -ForegroundColor Green

$projectDir = "D:\Projetos\Fusion ENGINE\Pong_Project"
$exePath = "D:\Projetos\Fusion ENGINE\build\Pong_Project\Release\Pong_Project.exe"
if (Test-Path $exePath) {
    Start-Process $exePath -WorkingDirectory $projectDir
    Write-Host "Pong iniciado com sucesso na sua area de trabalho!" -ForegroundColor Cyan
} else {
    Write-Host "Erro: Pong_Project.exe nao encontrado. Execute build_all.ps1 primeiro." -ForegroundColor Red
}
