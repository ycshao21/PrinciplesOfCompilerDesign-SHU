# Check if the case filename is given as an argument
if (-not $args) {
    Write-Host "Error: missing argument"
    exit 1
}

# Check if the extension is '.pl0'
if (-not ($args[0] -like "*.pl0")) {
    Write-Host "Error: invalid file extension"
    exit 1
}

# Check if binary file exists
$BIN="bin\Release\Windows_AMD64\exp01.exe"
if (-not (Test-Path $BIN -PathType Leaf)) {
    Write-Host "Error: $BIN does not exist"
    exit 1
}

# Check if source file exists
$SRC_FILE="tests\data\exp01-recognize-identifier\$($args[0])"
if (-not (Test-Path $SRC_FILE -PathType Leaf)) {
    Write-Host "Error: $SRC_FILE does not exist"
    exit 1
}

# Make directories for outputs
$OUT_DIR="tests\outputs\exp01-recognize-identifier"
if (-not (Test-Path $OUT_DIR -PathType Container)) {
    New-Item -ItemType Directory -Path $OUT_DIR | Out-Null
}
$OUT_FILE=Join-Path -Path $OUT_DIR -ChildPath $args[0] + ".out"

# Compile the program and save the output
& $BIN -f $SRC_FILE -o $OUT_FILE