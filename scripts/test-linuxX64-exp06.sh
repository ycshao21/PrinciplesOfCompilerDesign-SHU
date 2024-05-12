# Check if the case filename is given as an argument
if [ -z $1 ]; then
    echo "Error: missing argument"
    exit 1
fi

# Check if the extension is '.plq'
if [[ $1 != *.plq ]]; then
    echo "Error: invalid file extension"
    exit 1
fi

# Check if binary file exists
BIN=bin/Release/Linux_x86_64/exp06
if [ ! -f $BIN ]; then
    echo "Error: $BIN does not exist"
    exit 1
fi

# Check if source file exists
SRC_FILE=tests/data/exp06-optimize-code/$1
if [ ! -f $SRC_FILE ]; then
    echo "Error: $SRC_FILE does not exist"
    exit 1
fi

# Check if source file exists
_FILE=tests/data/exp06-optimize-code/$1
if [ ! -f $SRC_FILE ]; then
    echo "Error: $SRC_FILE does not exist"
    exit 1
fi

# Make directories for outputs
OUT_DIR=tests/outputs/exp06-optimize-code
if [ ! -d $OUT_DIR ]; then
    mkdir -p $OUT_DIR
fi
OUT_FILE=$OUT_DIR/$1.out

# Compile the program and save the output
$BIN -f $SRC_FILE -o $OUT_FILE