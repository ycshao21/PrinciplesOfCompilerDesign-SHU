# Check if the case filename is given as an argument
if [ -z $1 ]; then
    echo "Error: missing argument"
    exit 1
fi

# Check if the extension is '.pl0'
if [[ $1 != *.pl0 ]]; then
    echo "Error: invalid file extension"
    exit 1
fi

# Check if binary file exists
BIN=bin/Release/Linux_x86_64/exp01
if [ ! -f $BIN ]; then
    echo "Error: $BIN does not exist"
    exit 1
fi

# Check if source file exists
SRC_FILE=tests/data/exp03-analyze-syntax/$1
if [ ! -f $SRC_FILE ]; then
    echo "Error: $SRC_FILE does not exist"
    exit 1
fi

# Compile the program and save the output
$BIN -f $SRC_FILE