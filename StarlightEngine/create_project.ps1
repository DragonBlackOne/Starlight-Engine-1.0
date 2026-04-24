# create_project.ps1 - TitanEngine SDK
# Este script cria um novo projeto comercial baseado no template do motor.

param (
    [Parameter(Mandatory=$true)]
    [string]$ProjectName
)

$SDK_DIR = (Get-Location).Path.Replace('\', '/')
$PARENT_DIR = (Split-Path -Path $SDK_DIR -Parent).Replace('\', '/')
$TARGET_DIR = Join-Path $PARENT_DIR $ProjectName

Write-Host ">>> Iniciando Criação do Projeto Commercial: $ProjectName <<<" -ForegroundColor Cyan

# 1. Criar estrutura de pastas
New-Item -ItemType Directory -Path (Join-Path $TARGET_DIR "src") -Force
New-Item -ItemType Directory -Path (Join-Path $TARGET_DIR "assets") -Force
New-Item -ItemType Directory -Path (Join-Path $TARGET_DIR "build") -Force

# 2. Copiar Boilerplate
$MainContent = Get-Content (Join-Path $SDK_DIR "src\main.cpp")
$MainContent = $MainContent -replace "TitanEngine Commercial Project", "$ProjectName"
$MainContent | Set-Content (Join-Path $TARGET_DIR "src\main.cpp")

# 3. Gerar CMakeLists.txt do Projeto (Com tratamento de strings do PowerShell)
$CMakeContent = "cmake_minimum_required(VERSION 3.20)`n"
$CMakeContent += "project($ProjectName)`n`n"
$CMakeContent += "set(CMAKE_CXX_STANDARD 20)`n`n"
$CMakeContent += "# Localizar o SDK do TitanEngine`n"
$CMakeContent += "set(TITAN_SDK_DIR ""$SDK_DIR"")`n"
$CMakeContent += "include(`${TITAN_SDK_DIR}/TitanEngineConfig.cmake)`n`n"
$CMakeContent += "add_executable($ProjectName src/main.cpp)`n"
$CMakeContent += "target_link_libraries($ProjectName PRIVATE TitanCore)`n`n"
$CMakeContent += "# Copiar Assets para o build`n"
$CMakeContent += "add_custom_command(TARGET $ProjectName POST_BUILD`n"
$CMakeContent += "    COMMAND `${CMAKE_COMMAND} -E copy_directory`n"
$CMakeContent += "    `${CMAKE_CURRENT_SOURCE_DIR}/assets`n"
$CMakeContent += "    `\$<TARGET_FILE_DIR:$ProjectName>/assets`n"
$CMakeContent += ")`n"

$CMakeContent | Set-Content (Join-Path $TARGET_DIR "CMakeLists.txt")

Write-Host ">>> Projeto $ProjectName criado com sucesso em: $TARGET_DIR <<<" -ForegroundColor Green
