$build_type=$args[0]

$PROJECT_ROOT_DIR = Get-Location

Remove-Item -Path "$PROJECT_ROOT_DIR\build" -Recurse -Force
New-Item -Path "$PROJECT_ROOT_DIR\build" -ItemType Directory

Set-Location -Path "$PROJECT_ROOT_DIR\build"

cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE="$build_type"
cmake --build . --config "$build_type"

Set-Location -Path $PROJECT_ROOT_DIR

Write-Host "Build finished successfully."