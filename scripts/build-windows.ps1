$build_type=$args[0]

$PROJECT_ROOT_DIR = Get-Location

Remove-Item -Path "$PROJECT_ROOT_DIR\build" -Recurse -Force
New-Item -Path "$PROJECT_ROOT_DIR\build" -ItemType Directory

Set-Location -Path "$PROJECT_ROOT_DIR\build"

cmake .. -DCMAKE_BUILD_TYPE="${build_type}" -G "Ninja"

ninja

Set-Location -Path $PROJECT_ROOT_DIR

Write-Host "Build finished successfully."