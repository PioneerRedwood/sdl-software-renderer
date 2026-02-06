$ErrorActionPreference = "Stop"

$buildDir = "build-vs2022-x64"

if (Test-Path $buildDir) {
  Remove-Item $buildDir -Recurse -Force
}

cmake -S . -B $buildDir -G "Visual Studio 17 2022" -A x64
cmake --build $buildDir --config Debug

Write-Host "Built: $buildDir"
Write-Host "Open solution: $buildDir\sdl-software-renderer.sln"
