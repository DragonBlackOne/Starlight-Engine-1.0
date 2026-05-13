# Build All Projects - Fusion ENGINE

Write-Host "Building StarlightEngine..." -ForegroundColor Green
cd StarlightEngine
cmake -B build -S .
cmake --build build --config Release
cd ..



Write-Host "Building CapitalOdyssey..." -ForegroundColor Green
cd CapitalOdyssey
cmake -B build -S .
cmake --build build --config Release
cd ..

Write-Host "Building Pong_Project..." -ForegroundColor Green
cd Pong_Project
cmake -B build -S .
cmake --build build --config Release
cd ..

Write-Host "Building Tetris_Project..." -ForegroundColor Green
cd Tetris_Project
cmake -B build -S .
cmake --build build --config Release
cd ..

Write-Host "All builds completed successfully!" -ForegroundColor Cyan
