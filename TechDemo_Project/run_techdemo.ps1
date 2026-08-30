# Launcher para o Tech Demo da Fusion ENGINE
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "  FUSION ENGINE - NEXT-GEN ARCHITECTURAL TECH DEMO" -ForegroundColor Yellow
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "Iniciando Tech Demo em janela nativa..." -ForegroundColor Green

$projectDir = "D:\Projetos\Fusion ENGINE\TechDemo_Project"
$exePath = "D:\Projetos\Fusion ENGINE\build\TechDemo_Project\Release\TechDemo_Project.exe"
if (Test-Path $exePath) {
    Start-Process $exePath -WorkingDirectory $projectDir
    Write-Host "Tech Demo iniciado com sucesso na sua area de trabalho!" -ForegroundColor Cyan
} else {
    Write-Host "Erro: TechDemo_Project.exe nao encontrado. Execute build_all.ps1 primeiro." -ForegroundColor Red
}
