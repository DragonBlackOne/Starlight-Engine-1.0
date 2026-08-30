# Launcher para o God Hand 3D no PowerShell
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "  FUSION ENGINE - GOD HAND 3D (PS2 BRAWLER EDITION)" -ForegroundColor Yellow
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "Iniciando God Hand 3D em janela nativa..." -ForegroundColor Green

$projectDir = "D:\Projetos\Fusion ENGINE\GodHand_Project"
$exePath = "D:\Projetos\Fusion ENGINE\build\GodHand_Project\Release\GodHand_Project.exe"
if (Test-Path $exePath) {
    Start-Process $exePath -WorkingDirectory $projectDir
    Write-Host "Jogo iniciado com sucesso na sua area de trabalho!" -ForegroundColor Cyan
} else {
    Write-Host "Erro: GodHand_Project.exe nao encontrado. Execute build_all.ps1 primeiro." -ForegroundColor Red
}
