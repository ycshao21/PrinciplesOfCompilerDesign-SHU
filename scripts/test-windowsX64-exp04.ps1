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
$BIN="bin\Release\Windows_AMD64\Release\exp04.exe"
if (-not (Test-Path $BIN -PathType Leaf)) {
    Write-Host "Error: $BIN does not exist"
    exit 1
}

# Check if source file exists
$SRC_FILE="tests\data\exp04-analyze-semantics\$($args[0])"
if (-not (Test-Path $SRC_FILE -PathType Leaf)) {
    Write-Host "Error: $SRC_FILE does not exist"
    exit 1
}

# Compile the program and save the output
& $BIN -f $SRC_FILE